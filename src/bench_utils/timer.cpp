#include <bench_utils/timer.h>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

namespace bench_utils {
	precision_timer::precision_timer() noexcept {
		#if defined(_WIN32)
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			this->frequency = freq.QuadPart;
		#endif
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
} // namespace bench_utils
