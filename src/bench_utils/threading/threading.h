#pragma once
#include <bench_utils/threading/topology.h>
#include <cstdint>
#include <thread>

namespace bench_utils {
	struct threading_manager {
		const uint32_t thread_count = 0u;
		std::vector<logical_processor> thread_mapping{};
	};

	// call from within thread
	void assign_thread(const threading_manager& thread_manager, uint32_t thread_id) noexcept;

	[[nodiscard]] std::vector<logical_processor> create_mapping_basic() noexcept;
	[[nodiscard]] std::vector<logical_processor> create_mapping_numa(const topology_tree& topo_tree) noexcept;
	[[nodiscard]] std::vector<logical_processor> create_mapping_phte(const topology_tree& topo_tree) noexcept;

	[[nodiscard]] uint32_t get_default_thread_count(const topology_tree& tree) noexcept;
} // namespace bench_utils
