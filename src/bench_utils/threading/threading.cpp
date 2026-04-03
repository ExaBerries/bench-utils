#include <bench_utils/threading/threading.h>
#include <numeric>
#include <map>
#include <algorithm>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
#elif defined(__linux__)
	#include <pthread.h>
#endif

namespace bench_utils {

	void assign_thread(const threading_manager& thread_manager, uint32_t thread_id) noexcept {
		if (thread_id < thread_manager.thread_mapping.size()) {
			[[maybe_unused]] const auto& lp = thread_manager.thread_mapping[thread_id];
			#if defined(_WIN32)
				GROUP_AFFINITY affinity = {};
				affinity.Mask = (1ULL << lp.os_id);
				affinity.Group = lp.group_id;
				SetThreadGroupAffinity(GetCurrentThread(), &affinity, nullptr);
			#elif defined(__linux__)
				cpu_set_t cpuset;
				CPU_ZERO(&cpuset);
				CPU_SET(lp.os_id, &cpuset);
				pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
			#endif
		}
	}

	[[nodiscard]] std::vector<logical_processor> create_mapping_basic() noexcept {
		return {};
	}

	struct groups_t {
		std::vector<std::vector<const core*>> groups{};
		std::size_t max_threads = 0u;
	};

	static groups_t build_groups_int_numa_int_llc(const topology_tree& topo_tree, bool high_perf) noexcept {
		std::vector<std::vector<const core*>> groups;
		std::size_t max_threads = 0u;
		std::size_t max_llc_count = 0;
		for (const auto& node : topo_tree.numa_nodes) {
			max_llc_count = std::max(max_llc_count, node.llc_groups.size());
		}

		for (auto llc_index = 0u; llc_index < max_llc_count; llc_index++) {
			for (const auto& node : topo_tree.numa_nodes) {
				if (llc_index < node.llc_groups.size()) {
					std::vector<const core*> cores_in_group;
					for (const auto& c : node.llc_groups[llc_index].cores) {
						if (c.high_perf == high_perf && !c.threads.empty()) {
							cores_in_group.push_back(&c);
							max_threads = std::max(max_threads, c.threads.size());
						}
					}
					if (!cores_in_group.empty()) {
						groups.push_back(std::move(cores_in_group));
					}
				}
			}
		}
		return {std::move(groups), max_threads};
	}

	using thread_lists_t = std::vector<std::vector<const logical_processor*>>;

	static void initalize_thread_lists(thread_lists_t& thread_lists, std::size_t max_threads) noexcept {
		thread_lists.clear();
		for (auto i = 0u; i < max_threads; i++) {
			thread_lists.emplace_back(std::vector<const logical_processor*>{});
		}
	}

	static void interleave(thread_lists_t& thread_lists, const std::vector<std::vector<const core*>>& groups) noexcept {
		const auto max_threads = thread_lists.size();
		size_t max_cores = 0;
		for (const auto& g : groups) {
			max_cores = std::max(max_cores, g.size());
		}

		for (size_t i = 0; i < max_cores; ++i) {
			for (const auto& g : groups) {
				if (i < g.size()) {
					for (auto t = 0u; t < max_threads; t++) {
						if (t < g[i]->threads.size()) {
							thread_lists[t].push_back(&g[i]->threads[t]);
						}
					}
				}
			}
		}
	}

	static void add_threads_to_result(std::vector<logical_processor>& result, const thread_lists_t& thread_lists) noexcept {
		for (const auto& thread_list : thread_lists) {
			for (const auto* lp : thread_list) {
				result.push_back(*lp);
			}
		}
	}

	[[nodiscard]] std::vector<logical_processor> create_mapping_numa(const topology_tree& topo_tree) noexcept {
		std::vector<logical_processor> result{};
		if (topo_tree.numa_nodes.empty()) {
			return result;
		}

		std::vector<std::vector<const logical_processor*>> thread_lists;

		// p cores first then e cores
		auto [p_groups, p_mt] = build_groups_int_numa_int_llc(topo_tree, true);
		auto [e_groups, e_mt] = build_groups_int_numa_int_llc(topo_tree, false);

		initalize_thread_lists(thread_lists, std::max(p_mt, e_mt));

		interleave(thread_lists, p_groups);
		interleave(thread_lists, e_groups);

		add_threads_to_result(result, thread_lists);

		return result;
	}

	[[nodiscard]] std::vector<logical_processor> create_mapping_phte(const topology_tree& topo_tree) noexcept {
		std::vector<logical_processor> result{};
		if (topo_tree.numa_nodes.empty()) {
			return result;
		}

		std::vector<std::vector<const logical_processor*>> thread_lists;

		for (auto p_or_e : {true, false}) {
			auto [groups, mt] = build_groups_int_numa_int_llc(topo_tree, p_or_e);
			initalize_thread_lists(thread_lists, mt);
			interleave(thread_lists, groups);
			add_threads_to_result(result, thread_lists);
		}

		return result;
	}

	[[nodiscard]] uint32_t get_default_thread_count(const topology_tree& tree) noexcept {
		if (tree.numa_nodes.empty()) {
			return std::thread::hardware_concurrency();
		}

		uint32_t count = 0;
		for (const auto& node : tree.numa_nodes) {
			for (const auto& llc : node.llc_groups) {
				count += static_cast<uint32_t>(llc.cores.size());
			}
		}
		return count;
	}

} // namespace bench_utils
