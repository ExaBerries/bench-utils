#pragma once
#include <bench_utils/isa/isa_macro.h>
#if defined(BENCH_UTILS_ISA_X86_64) || defined(BENCH_UTILS_ISA_X86)
#include <immintrin.h>

namespace bench_utils {
	inline void lfence() noexcept {
		_mm_lfence();
	}

	inline void mfence() noexcept {
		_mm_mfence();
	}

	inline void sfence() noexcept {
		_mm_sfence();
	}

	inline void pause() noexcept {
		_mm_pause();
	}
} // namespace bench_utils
#endif
