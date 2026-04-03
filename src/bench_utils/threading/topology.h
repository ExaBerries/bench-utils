#pragma once
#include <vector>
#include <cstdint>
#include <string>

namespace bench_utils {
	struct logical_processor {
		uint32_t thread_index = 0u;
		uint32_t core_id = 0u; // to easily find the actual core without making a LUT
		uint32_t os_id = 0u; // os specific core id
		uint16_t group_id = 0u; // for thread groups in windows
	};

	struct core {
		uint32_t core_id = 0u;
		bool high_perf = false;
		std::vector<logical_processor> threads{};
	};

	struct llc_group {
		uint32_t llc_id = 0u;
		std::vector<core> cores{};
	};

	struct numa_node {
		uint32_t node_id = 0u;
		std::vector<llc_group> llc_groups{};
	};

	struct topology_tree {
		std::vector<numa_node> numa_nodes{};
	};

	[[nodiscard]] topology_tree build_topo_tree() noexcept;

	void print_topo_debug(const topology_tree& tree) noexcept;
} // namespace bench_utils
