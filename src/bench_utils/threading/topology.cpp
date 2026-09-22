#include <bench_utils/threading/topology.h>
#include <map>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <charconv>
#include <algorithm>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(__linux__)
	#include <dirent.h>
#elif defined(__APPLE__)
	#include <sys/sysctl.h>
#endif

namespace bench_utils {

#if defined(_WIN32)
	[[nodiscard]] topology_tree build_topo_tree() noexcept {
		topology_tree tree;

		DWORD length = 0;
		GetLogicalProcessorInformationEx(RelationAll, nullptr, &length);

		std::vector<uint8_t> buffer(length);
		auto* info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());

		if (!GetLogicalProcessorInformationEx(RelationAll, info, &length)) {
			return tree;
		}

		std::map<uint16_t, std::map<uint32_t, uint32_t>> cpu_to_numa;
		std::map<uint16_t, std::map<uint32_t, uint32_t>> cpu_to_llc;

		// NUMA + LLC
		uint8_t* ptr = buffer.data();
		uint32_t llc_counter = 0;

		while (ptr < buffer.data() + length) {
			auto* curr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);

			if (curr->Relationship == RelationNumaNode) {
				auto node = curr->NumaNode;
				for (uint32_t b = 0; b < 64; b++) {
					if ((node.GroupMask.Mask >> b) & 1ull) {
						cpu_to_numa[node.GroupMask.Group][b] = node.NodeNumber;
					}
				}
			} else if (curr->Relationship == RelationCache) {
				const auto& cache = curr->Cache;

				if (cache.Level == 3) {
					for (WORD i = 0; i < cache.GroupCount; i++) {
						auto& gm = cache.GroupMasks[i];
						for (uint32_t b = 0; b < 64; b++) {
							if ((gm.Mask >> b) & 1ull) {
								cpu_to_llc[gm.Group][b] = llc_counter;
							}
						}
					}
					llc_counter++;
				}
			}

			ptr += curr->Size;
		}

		// cores
		std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, core>>> topo;

		uint32_t max_perf_level = 0;
		uint32_t core_counter = 0;

		ptr = buffer.data();

		while (ptr < buffer.data() + length) {
			auto* curr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);

			if (curr->Relationship == RelationProcessorCore) {
				core c;
				c.core_id = core_counter++;
				c.perf_level = curr->Processor.EfficiencyClass;
				max_perf_level = std::max(max_perf_level, c.perf_level);

				uint32_t node_id = 0;

				for (WORD i = 0; i < curr->Processor.GroupCount; i++) {
					auto& gm = curr->Processor.GroupMask[i];

					for (uint32_t j = 0; j < 64; j++) {
						if ((gm.Mask >> j) & 1ull) {
							node_id = cpu_to_numa[gm.Group][j];
							c.threads.emplace_back(static_cast<uint32_t>(c.threads.size()), c.core_id, j, gm.Group);
						}
					}
				}

				uint32_t llc_id = 0;
				if (!c.threads.empty()) {
					auto& t = c.threads[0];
					llc_id = cpu_to_llc[t.group_id][t.os_id];
				}

				topo[node_id][llc_id][c.core_id] = std::move(c);
			}

			ptr += curr->Size;
		}

		// homogeneous: all cores are tier 0
		if (max_perf_level == 0) {
			for (auto& [_, llcs] : topo) {
				for (auto& [__, cores] : llcs) {
					for (auto& [___, c] : cores) {
						c.perf_level = 0;
					}
				}
			}
		}

		tree.perf_level_count = max_perf_level + 1;

		for (auto& [nid, llcs] : topo) {
			numa_node node{nid};
			for (auto& [lid, cores] : llcs) {
				llc_group llc{lid};
				for (auto& [_, c] : cores) {
					llc.cores.push_back(std::move(c));
				}
				node.llc_groups.push_back(std::move(llc));
			}

			tree.numa_nodes.push_back(std::move(node));
		}

		return tree;
	}
#elif defined(__linux__)
	[[nodiscard]] topology_tree build_topo_tree() noexcept {
		topology_tree tree;

		auto read_u32 = [](const std::filesystem::path& path, uint32_t def = 0u) -> uint32_t {
			std::ifstream f(path);
			uint32_t v = def;
			if (f) {
				f >> v;
			}
			return v;
		};

		// first pass: collect all cpu_capacity values to discover distinct tiers
		std::vector<std::pair<uint32_t, uint32_t>> cpu_entries; // (cpu_id, capacity)

		for (const auto& entry : std::filesystem::directory_iterator("/sys/devices/system/cpu")) {
			auto name = entry.path().filename().string();

			if (name.rfind("cpu", 0) != 0 || !std::isdigit(name[3])) {
				continue;
			}

			uint32_t cpu = 0u;
			{
				auto cpu_str = name.substr(3);
				auto [ptr, ec] = std::from_chars(cpu_str.data(), cpu_str.data() + cpu_str.size(), cpu);

				if (ec != std::errc{}) {
					continue;
				}
			}

			uint32_t capacity = read_u32(entry.path() / "cpu_capacity", 1024);
			cpu_entries.emplace_back(cpu, capacity);
		}

		// build sorted unique list of capacities (descending) to map to perf levels
		std::vector<uint32_t> capacities;
		capacities.reserve(cpu_entries.size());
		for (const auto& [_, cap] : cpu_entries) {
			capacities.push_back(cap);
		}
		std::sort(capacities.begin(), capacities.end(), std::greater<>());
		capacities.erase(std::unique(capacities.begin(), capacities.end()), capacities.end());

		// map capacity -> perf_level (highest capacity = tier 0)
		std::map<uint32_t, uint32_t> cap_to_tier;
		for (uint32_t i = 0; i < static_cast<uint32_t>(capacities.size()); i++) {
			cap_to_tier[capacities[i]] = i;
		}
		tree.perf_level_count = static_cast<uint32_t>(capacities.size());

		// second pass: build topology with assigned perf levels
		std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, core>>> topo;

		for (const auto& [cpu, capacity] : cpu_entries) {
			auto entry_path = std::filesystem::path("/sys/devices/system/cpu") / ("cpu" + std::to_string(cpu));

			uint32_t node_id = 0;
			for (const auto& n : std::filesystem::directory_iterator(entry_path)) {
				auto fname = n.path().filename().string();
				if (fname.rfind("node", 0) == 0) {
					auto nid_str = fname.substr(4);
					auto [ptr, ec] = std::from_chars(nid_str.data(), nid_str.data() + nid_str.size(), node_id);

					if (ec != std::errc{}) {
						continue;
					}
					break;
				}
			}

			uint32_t core_id = read_u32(entry_path / "topology/core_id");

			uint32_t llc_id = 0;
			{
				std::ifstream f(entry_path / "cache/index3/id");
				if (f) {
					f >> llc_id;
				}
			}

			auto& c = topo[node_id][llc_id][core_id];
			c.core_id = core_id;
			c.perf_level = cap_to_tier[capacity];

			logical_processor lp{};
			lp.core_id = core_id;
			lp.os_id = cpu;
			lp.thread_index = static_cast<uint32_t>(c.threads.size());

			c.threads.push_back(lp);
		}

		for (auto& [nid, llcs] : topo) {
			numa_node node{nid};

			for (auto& [lid, cores] : llcs) {
				llc_group llc{lid};

				for (auto& [_, c] : cores) {
					llc.cores.push_back(std::move(c));
				}

				node.llc_groups.push_back(std::move(llc));
			}

			tree.numa_nodes.push_back(std::move(node));
		}

		return tree;
	}
#elif defined(__APPLE__)
	[[nodiscard]] topology_tree build_topo_tree() noexcept {
		topology_tree tree;
		numa_node node{0};

		// probe for N perf levels (hw.perflevel0, hw.perflevel1, ...)
		// up to a reasonable maximum; stop when a level reports 0 cores
		constexpr uint32_t max_levels = 8;
		uint32_t gid = 0;

		for (uint32_t lvl = 0; lvl < max_levels; lvl++) {
			std::string count_key = "hw.perflevel" + std::to_string(lvl) + ".physicalcpu";
			uint32_t count = 0;
			size_t sz = sizeof(uint32_t);

			if (sysctlbyname(count_key.c_str(), &count, &sz, NULL, 0) != 0 || count == 0) {
				break;
			}

			llc_group llc{lvl};

			for (uint32_t i = 0; i < count; i++) {
				core c;
				c.core_id = gid;
				c.perf_level = lvl;

				logical_processor lp{};
				lp.core_id = gid;
				lp.os_id = gid;
				lp.thread_index = 0;

				c.threads.push_back(lp);
				llc.cores.push_back(std::move(c));

				gid++;
			}

			node.llc_groups.push_back(std::move(llc));
		}

		tree.perf_level_count = static_cast<uint32_t>(node.llc_groups.size());

		tree.numa_nodes.push_back(std::move(node));
		return tree;
	}
#else
	[[nodiscard]] topology_tree build_topo_tree() noexcept {
		return {};
	}
#endif

	void print_topo_debug(const topology_tree& tree) noexcept {
		std::cout << "perf_level_count=" << tree.perf_level_count << "\n";

		for (const auto& n : tree.numa_nodes) {
			std::cout << "numa node id=" << n.node_id << "\n";

			for (const auto& llc : n.llc_groups) {
				std::cout << "|--llc id=" << llc.llc_id << "\n";

				for (const auto& c : llc.cores) {
					std::cout << "|  |--core id=" << c.core_id
							  << " perf_level=" << c.perf_level << "\n";

					for (const auto& lp : c.threads) {
						std::cout << "|  |  |--logical idx=" << lp.thread_index
								  << " os=" << lp.os_id << "\n";
					}
				}
			}
		}
	}
} // namespace bench_utils