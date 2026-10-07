#include <bench_utils/timer.h>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

#include <bench_utils/isa/x86/cpuid.h>

namespace bench_utils {
	[[nodiscard]] static timer_source detect_timer_source([[maybe_unused]] int64_t frequency) noexcept {
		#if defined(_WIN32) && (defined(BENCH_UTILS_ISA_X86_64) || defined(BENCH_UTILS_ISA_X86))
		constexpr int64_t tsc_threshold = 1'000'000'000ll;   // 1 GHz
		constexpr int64_t hpet_threshold = 10'000'000ll;     // ~10 MHz
		constexpr int64_t acpi_khz = 3579ll;                 // 3.579545 MHz PM timer
		constexpr int64_t pit_frequency = 1'193'182ll;       // 1.193182 MHz
		constexpr int64_t rtc_frequency = 32'000'000ll;      // 32 kHz derived

		if (frequency == pit_frequency) {
			return timer_source::PIT;
		}
		if (frequency == rtc_frequency) {
			return timer_source::RTC;
		}
		if (frequency > tsc_threshold) {
			return is_invariant_tsc() ? timer_source::INVARIANT_TSC : timer_source::TSC;
		}
		if (frequency / 1000ll == acpi_khz) {
			return timer_source::ACPI_PM;
		}
		if (frequency > hpet_threshold) {
			return timer_source::HPET;
		}
		#endif
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

	precision_timer::precision_timer() noexcept {
		#if defined(_WIN32) && (defined(BENCH_UTILS_ISA_X86_64) || defined(BENCH_UTILS_ISA_X86))
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			this->frequency = freq.QuadPart;
			this->source = classify_frequency(this->frequency);
		#else
			this->frequency = std::chrono::high_resolution_clock::period::den / std::chrono::high_resolution_clock::period::num;
		#endif
		this->source = detect_timer_source(this->frequency);
	}

	[[nodiscard]] int64_t precision_timer::now() const noexcept {
		#if defined(_WIN32)
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			return time.QuadPart;
		#else 
			return std::chrono::high_resolution_clock::now().time_since_epoch().count();
		#endif
	}

	[[nodiscard]] int64_t precision_timer::duration_ms(int64_t start, int64_t end) const noexcept {
		#if defined(_WIN32)
			return (end - start) * 1000ll / frequency;
		#else 
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::duration(end - start)).count();
		#endif
	}
	
	[[nodiscard]] int64_t precision_timer::duration_us(int64_t start, int64_t end) const noexcept {
		#if defined(_WIN32)
			return (end - start) * 1000000ll / frequency;
		#else 
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::duration(end - start)).count();
		#endif
	}

	[[nodiscard]] uint64_t get_tick_count_ms() noexcept {
		#if defined(_WIN32)
			return GetTickCount64();
		#else
			auto duration = std::chrono::steady_clock::now().time_since_epoch();
			return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
		#endif
	}
} // namespace bench_utils
