#pragma once
#include <string>
#include <string_view>
#include <cstdint>

namespace bench_utils {
	[[nodiscard]] std::string format_uint64_t(uint64_t val, std::string_view unit_str = "") noexcept;
	[[nodiscard]] std::string format_bytes(uint64_t bytes) noexcept;
} // namespace bench_utils
