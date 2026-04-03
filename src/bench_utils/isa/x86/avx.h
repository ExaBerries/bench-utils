#pragma once
#if defined(__AVX__)

#define BENCH_UTILS_SIMD_REG2D

#include <cstdint>

namespace bench_utils {
	namespace simd {
		using reg2d = __m128d;

		[[gnu::always_inline, nodiscard]] inline reg2d load2d2d(const double& data) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg2d load1d2d(const double& value) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg2d load1d2d_lo(const double& value) noexcept;

		[[gnu::always_inline]] inline void store2d(double& out, const reg2d& reg) noexcept;
		[[gnu::always_inline]] inline void store2d_lo(double& out, const reg2d& reg) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg2d add2d(const reg2d& a, const reg2d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg2d sub2d(const reg2d& a, const reg2d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg2d mul2d(const reg2d& a, const reg2d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg2d div2d(const reg2d& a, const reg2d& b) noexcept;
	} // namespace simd
} // namespace bench_utils

#include "avxImpl.h"

#endif
