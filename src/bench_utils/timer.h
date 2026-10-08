#pragma once
#include <cstdint>
#include <chrono>
#include <string_view>

namespace bench_utils {
	using basic_timer = std::chrono::steady_clock;

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

	[[nodiscard]] int64_t duration_ms(basic_timer::time_point start, basic_timer::time_point end) noexcept;
	[[nodiscard]] int64_t duration_us(basic_timer::time_point start, basic_timer::time_point end) noexcept;

	struct timer {
		int64_t report_frequency = 0ll;
		int64_t underlying_frequency = 0ll;
		timer_source source = timer_source::UNKNOWN;
	};

	[[nodiscard]] timer create_fast_timer() noexcept;
	[[nodiscard]] timer create_coarse_timer() noexcept;

	[[nodiscard]] int64_t fast_now() noexcept;
	[[nodiscard]] int64_t coarse_now() noexcept;

	[[nodiscard]] bool will_overflow_within(const timer& t, int64_t now, int64_t seconds) noexcept;

	template <typename Duration = std::chrono::milliseconds>
	[[nodiscard]] int64_t duration(const timer& t, int64_t start, int64_t end) noexcept;

} // namespace bench_utils
