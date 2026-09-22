#pragma once
#include <bench_utils/threading/threading.h>

namespace bench_utils {
	[[nodiscard]] topology_tree create_7700k_tree() noexcept;
	[[nodiscard]] topology_tree create_2s2c2t_tree() noexcept;
	[[nodiscard]] topology_tree create_alder2p2e_tree() noexcept;
	[[nodiscard]] topology_tree create_meteor2p2e2lpe_tree() noexcept;
	[[nodiscard]] topology_tree create_2ccd_tree() noexcept;
	[[nodiscard]] topology_tree create_2s2ccd_tree() noexcept;
} // namespace bench_utils
