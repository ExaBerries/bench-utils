#include <bench_utils/timer.h>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(__linux__)
	#include <time.h>
#endif

#include <bench_utils/isa/x86/cpuid.h>

#include <fstream>
#include <limits>
#include <string>

namespace bench_utils {
	[[nodiscard]] static timer_source detect_timer_source([[maybe_unused]] int64_t frequency) noexcept {
		#if defined(_WIN32) && (defined(BENCH_UTILS_ISA_X86_64) || defined(BENCH_UTILS_ISA_X86))
		constexpr int64_t hpet_min_threshold = 10'000'000ll; // ~10 MHz
		constexpr int64_t typical_hpet_min = 14'3100'000ull;
		constexpr int64_t typical_hpet_max = 14'3300'000ull;
		constexpr int64_t hpet_max_threshold = 100'000'000ll; // ~100 MHz
		constexpr int64_t acpi_khz = 3579ll; // 3.579545 MHz PM timer
		constexpr int64_t pit_frequency = 1'193'182ll; // 1.193182 MHz
		constexpr int64_t rtc_frequency = 32'000'000ll; // 32 kHz derived

		if (frequency == pit_frequency) {
			return timer_source::PIT;
		}
		if (frequency == rtc_frequency) {
			return timer_source::RTC;
		}
		if (frequency / 1000ll == acpi_khz) {
			return timer_source::ACPI_PM;
		}

		const auto tsc_info = get_tsc_clock_info();
		const auto inv_or_not_tsc = tsc_info.is_invariant_tsc ? timer_source::INVARIANT_TSC : timer_source::TSC;

		if (frequency < typical_hpet_max && frequency > typical_hpet_min) { // obvious hpet
			return timer_source::HPET;
		}
		if (frequency >= hpet_max_threshold) { // way too high to be hpet -> TSC
			return timer_source::TSC;
		}

		if (frequency == hpet_min_threshold) { // 10 MHz should be windows manually scaling TSC
			return inv_or_not_tsc;
		}

		if (frequency > hpet_min_threshold) { // windows is not manually scaling to 10 MHz
			if (tsc_info.tsc_frequency_hz != 0ull) { // try to rule out TSC if CPUID reports it
				if (tsc_info.tsc_frequency_hz == static_cast<uint64_t>(frequency)) {
					return inv_or_not_tsc;
				} else {
					return timer_source::HPET;
				}
			}
		}
		#endif

		// future idea: analyze time it takes for timer calls -> a very quick timer call is probably TSC, long -> HPET

		return timer_source::UNKNOWN;
	}

	[[nodiscard]] std::string_view to_string(timer_source source) noexcept {
		switch (source) {
			case timer_source::TSC: return "TSC";
			case timer_source::INVARIANT_TSC: return "iTSC";
			case timer_source::HPET: return "HPET";
			case timer_source::ACPI_PM: return "ACPI";
			case timer_source::PIT: return "PIT";
			case timer_source::RTC: return "RTC";
			case timer_source::UNKNOWN: return "unknown";
			default: return "unknown";
		}
	}

	[[nodiscard]] int64_t duration_ms(basic_timer::time_point start, basic_timer::time_point end) noexcept {
		return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}

	[[nodiscard]] int64_t duration_us(basic_timer::time_point start, basic_timer::time_point end) noexcept {
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	template <typename Duration>
	[[nodiscard]] static int64_t ticks_to_duration(int64_t start, int64_t end, int64_t frequency) noexcept {
		if (frequency <= 0ll) {
			return 0ll;
		}
		constexpr int64_t units_per_second = static_cast<int64_t>(Duration::period::den / Duration::period::num);
		static_assert(units_per_second > 0);
		const int64_t delta = end - start;
		return (delta / frequency) * units_per_second + (delta % frequency) * units_per_second / frequency;
	}

	template <typename Duration>
	[[nodiscard]] int64_t duration(const timer& t, int64_t start, int64_t end) noexcept {
		return ticks_to_duration<Duration>(start, end, t.report_frequency);
	}

	template int64_t duration<std::chrono::milliseconds>(const timer& t, int64_t start, int64_t end) noexcept;
	template int64_t duration<std::chrono::microseconds>(const timer& t, int64_t start, int64_t end) noexcept;
	template int64_t duration<std::chrono::nanoseconds>(const timer& t, int64_t start, int64_t end) noexcept;
	template int64_t duration<std::chrono::seconds>(const timer& t, int64_t start, int64_t end) noexcept;

	#if defined(__linux__)
		[[nodiscard]] static int64_t detect_linux_underlying_frequency() noexcept {
			std::ifstream clocksource_file("/sys/devices/system/clocksource/clocksource0/current_clocksource");
			if (!clocksource_file) {
				return 0ll;
			}

			std::string clocksource;
			std::getline(clocksource_file, clocksource);

			if (clocksource == "tsc") {
				#if defined(BENCH_UTILS_ISA_X86_64) || defined(BENCH_UTILS_ISA_X86)
					return static_cast<int64_t>(get_tsc_clock_info().tsc_frequency_hz);
				#else
					return 0ll;
				#endif
			}
			if (clocksource == "hpet") {
				return 14'318'180ll;
			}
			if (clocksource == "acpi_pm") {
				return 3'579'545ll;
			}
			if (clocksource == "pit") {
				return 1'193'182ll;
			}
			return 0ll;
		}

		template <typename T>
		[[nodiscard]] static int64_t get_linux_clk_frequency_for(T clock) noexcept {
			timespec res{};
			if (clock_getres(clock, &res) != 0) {
				return 1'000ll; // fall back to the usual 1ms jiffy
			}
			const int64_t nanoseconds = static_cast<int64_t>(res.tv_sec) * 1'000'000'000ll + static_cast<int64_t>(res.tv_nsec);
			return nanoseconds > 0ll ? 1'000'000'000ll / nanoseconds : 1'000ll;
		}
	#endif

	[[nodiscard]] timer create_fast_timer() noexcept {
		timer result{};
		#if defined(_WIN32)
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			result.report_frequency = freq.QuadPart;
			result.underlying_frequency = result.report_frequency;
		#elif defined(__linux__)
			result.report_frequency = 1'000'000'000ll;
			result.underlying_frequency = detect_linux_underlying_frequency();
		#else
			result.report_frequency = std::chrono::high_resolution_clock::period::den / std::chrono::high_resolution_clock::period::num;
			result.underlying_frequency = 0ll;
		#endif
		result.source = detect_timer_source(result.report_frequency);
		return result;
	}

	[[nodiscard]] int64_t fast_now() noexcept {
		#if defined(_WIN32)
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			return time.QuadPart;
		#elif defined(__linux__)
			timespec ts;
			clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
			return static_cast<int64_t>(ts.tv_sec) * 1'000'000'000ll + static_cast<int64_t>(ts.tv_nsec);
		#else
			return std::chrono::high_resolution_clock::now().time_since_epoch().count();
		#endif
	}

	[[nodiscard]] timer create_coarse_timer() noexcept {
		timer result{};
		#if defined(_WIN32)
			result.report_frequency = 1000ll; // GetTickCount64 ticks every millisecond
			DWORD adjustment = 0ul;
			DWORD increment = 0ul;
			BOOL enabled = FALSE;
			if (GetSystemTimeAdjustment(&adjustment, &increment, &enabled) && increment != 0ul) {
				result.underlying_frequency = 10'000'000ll / static_cast<int64_t>(increment);
			}
		#elif defined(__linux__)
			result.report_frequency = 1'000ll;
			result.underlying_frequency = get_linux_clk_frequency_for(CLOCK_MONOTONIC_COARSE);
		#else
			result.report_frequency = 1000ll;
		#endif
		return result;
	}

	[[nodiscard]] int64_t coarse_now() noexcept {
		#if defined(_WIN32)
			return static_cast<int64_t>(GetTickCount64());
		#elif defined(__linux__)
			timespec ts;
			clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
			return static_cast<int64_t>(ts.tv_sec) * 1'000ll + static_cast<int64_t>(ts.tv_nsec) / 1'000'000ll;
		#else
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		#endif
	}

	[[nodiscard]] static int64_t seconds_until_overflow(int64_t now, int64_t frequency) noexcept {
		if (frequency <= 0ll) {
			return std::numeric_limits<int64_t>::max();
		}
		return (std::numeric_limits<int64_t>::max() - now) / frequency;
	}

	[[nodiscard]] bool will_overflow_within(const timer& t, int64_t now, int64_t seconds) noexcept {
		return seconds_until_overflow(now, t.report_frequency) <= seconds;
	}
} // namespace bench_utils
