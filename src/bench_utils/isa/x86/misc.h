#pragma once
#include <immintrin.h>

namespace bench_utils {
	// automatically round denormals to zero
	inline void round_denormals() noexcept {
		_mm_setcsr(_mm_getcsr() | 0x8000 | 0x0040);
	}
} // namespace bench_utils
