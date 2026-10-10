#pragma once
#include <bench_box/isa/isa_macro.h>
#if defined(BENCH_BOX_ISA_X86_64) || defined(BENCH_BOX_ISA_X86)
#include <cstdint>
#include <immintrin.h>
#include <bench_box/isa/x86/fences.h>

namespace bench_box {
	[[nodiscard]] inline uint64_t read_tsc() noexcept {
		#if defined(_MSC_VER)
			return _rdtsc();
		#else
			return __rdtsc();
		#endif
	}

	[[nodiscard]] inline uint64_t read_tscp() noexcept {
		#if defined(_MSC_VER)
			return _rdtscp(nullptr);
		#else
			return __rdtscp(nullptr);
		#endif
	}

	[[nodiscard]] inline uint64_t fence_rdtsc() noexcept {
		lfence();
		auto tsc = read_tsc();
		lfence();
		return tsc;
	}

	[[nodiscard]] inline uint64_t fence_rdtscp() noexcept {
		auto tsc = read_tscp();
		lfence();
		return tsc;
	}
} // namespace bench_box
#endif
