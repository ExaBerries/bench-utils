#pragma once
#include <cstdint>

namespace bench_utils {
	// move console cursor to the start of the line back the specified number of lines
	void reset_cursor(uint32_t lines) noexcept;
} // namespace bench_utils
