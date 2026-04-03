#pragma once
#include <cstdint>
#include <chrono>

namespace bench_utils {
	using basic_timer = std::chrono::high_resolution_clock;

	[[nodiscard]] inline int64_t duration_ms(basic_timer::time_point start, basic_timer::time_point end) noexcept {
		return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}

	[[nodiscard]] inline int64_t duration_us(basic_timer::time_point start, basic_timer::time_point end) noexcept {
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	struct precision_timer {
		int64_t frequency = 0ll;

		precision_timer() noexcept;

		[[nodiscard]] int64_t now() const noexcept;

		[[nodiscard]] int64_t duration_ms(int64_t start, int64_t end) const noexcept;
		[[nodiscard]] int64_t duration_us(int64_t start, int64_t end) const noexcept;
	};
} // namespace bench_utils
