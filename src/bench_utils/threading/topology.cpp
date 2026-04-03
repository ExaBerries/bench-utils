#include <bench_utils/threading/topology.h>
#include <map>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <charconv>

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

		bool non_homo = false;
		uint32_t core_counter = 0;

		ptr = buffer.data();

		while (ptr < buffer.data() + length) {
			auto* curr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);

			if (curr->Relationship == RelationProcessorCore) {
				core c;
				c.core_id = core_counter++;
				c.high_perf = curr->Processor.EfficiencyClass;
				non_homo |= c.high_perf;

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

		if (!non_homo) {
			for (auto& [_, llcs] : topo) {
				for (auto& [__, cores] : llcs) {
					for (auto& [___, c] : cores) {
						c.high_perf = true;
					}
				}
			}
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
#elif defined(__linux__)
	[[nodiscard]] topology_tree build_topo_tree() noexcept {
		topology_tree tree;

		std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, core>>> topo;

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
					continue; // invalid entry
				}
			}

			auto read_u32 = [&](const std::string& leaf, uint32_t def = 0u) {
				std::ifstream f(entry.path() / leaf);
				uint32_t v = def;
				if (f) {
					f >> v;
				}
				return v;
			};

			uint32_t node_id = 0;
			for (const auto& n : std::filesystem::directory_iterator(entry.path())) {
				auto fname = n.path().filename().string();
				if (fname.rfind("node", 0) == 0) {
					auto nid_str = fname.substr(4);
					auto [ptr, ec] = std::from_chars(nid_str.data(), nid_str.data() + nid_str.size(), node_id);

					if (ec != std::errc{}) {
						continue; // invalid entry
					}
					break;
				}
			}

			uint32_t core_id = read_u32("topology/core_id");

			uint32_t llc_id = 0;
			{
				std::ifstream f(entry.path() / "cache/index3/id");
				if (f) {
					f >> llc_id;
				}
			}

			uint32_t capacity = read_u32("cpu_capacity", 1024);

			auto& c = topo[node_id][llc_id][core_id];
			c.core_id = core_id;
			c.high_perf = (capacity > 700);

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

		uint32_t p = 0, e = 0;
		size_t sz = sizeof(uint32_t);

		sysctlbyname("hw.perflevel0.physicalcpu", &p, &sz, NULL, 0);
		sysctlbyname("hw.perflevel1.physicalcpu", &e, &sz, NULL, 0);

		uint32_t gid = 0;

		auto add_cluster = [&](uint32_t count, bool high_perf, uint32_t llc_id) {
			llc_group llc{llc_id};

			for (uint32_t i = 0; i < count; i++) {
				core c;
				c.core_id = gid;
				c.high_perf = high_perf;

				logical_processor lp{};
				lp.core_id = gid;
				lp.os_id = gid;
				lp.thread_index = 0;

				c.threads.push_back(lp);
				llc.cores.push_back(std::move(c));

				gid++;
			}

			node.llc_groups.push_back(std::move(llc));
		};

		add_cluster(p, true, 0); // big cluster
		add_cluster(e, false, 1); // small cluster

		tree.numa_nodes.push_back(std::move(node));
		return tree;
	}
#else
	[[nodiscard]] topology_tree build_topo_tree() noexcept {
		return {};
	}
#endif

	void print_topo_debug(const topology_tree& tree) noexcept {
		for (const auto& n : tree.numa_nodes) {
			std::cout << "numa node id=" << n.node_id << "\n";

			for (const auto& llc : n.llc_groups) {
				std::cout << "|--llc id=" << llc.llc_id << "\n";

				for (const auto& c : llc.cores) {
					std::cout << "|  |--core id=" << c.core_id
							  << " " << (c.high_perf ? "p" : "e") << "\n";

					for (const auto& lp : c.threads) {
						std::cout << "|  |  |--logical idx=" << lp.thread_index
								  << " os=" << lp.os_id << "\n";
					}
				}
			}
		}
	}
} // namespace bench_utils