#pragma once
#include <cstdint>
#include <chrono>
#include <string_view>

namespace bench_utils {
	using basic_timer = std::chrono::high_resolution_clock;

	enum struct timer_source : uint8_t {
		UNKNOWN,
		TSC,
		INVARIANT_TSC,
		HPET,
		ACPI_PM,
		PIT,
		RTC,
	};

	[[nodiscard]] std::string_view to_string(timer_source source) noexcept;

	[[nodiscard]] inline int64_t duration_ms(basic_timer::time_point start, basic_timer::time_point end) noexcept {
		return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}

	[[nodiscard]] inline int64_t duration_us(basic_timer::time_point start, basic_timer::time_point end) noexcept {
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	struct precision_timer {
		int64_t frequency = 0ll;
		timer_source source = timer_source::UNKNOWN;

		precision_timer() noexcept;

		[[nodiscard]] int64_t now() const noexcept;

		[[nodiscard]] int64_t duration_ms(int64_t start, int64_t end) const noexcept;
		[[nodiscard]] int64_t duration_us(int64_t start, int64_t end) const noexcept;
	};

	[[nodiscard]] uint64_t get_tick_count_ms() noexcept;
} // namespace bench_utils
