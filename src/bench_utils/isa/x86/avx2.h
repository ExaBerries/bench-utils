#pragma once
#if defined(__AVX2__)
#include <cstdint>

// avx2 + fma

#define BENCH_UTILS_SIMD_REG4D
#define BENCH_UTILS_SIMD_FMA

namespace bench_utils {
	namespace simd {
		using reg4d = __m256d;
		using reg4i64 = __m256i;

		template <int64_t m0, int64_t m1, int64_t m2, int64_t m3>
		[[gnu::always_inline, nodiscard]] inline reg4i64 create_mask_4i64() noexcept;

		[[gnu::always_inline, nodiscard]] inline reg4d load4d4d(const double& data) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d load1d4d(const double& value) noexcept;

		[[gnu::always_inline]] inline void store4d(double& out, const reg4d& reg) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg4d add4d(const reg4d& a, const reg4d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d sub4d(const reg4d& a, const reg4d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d mul4d(const reg4d& a, const reg4d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d div4d(const reg4d& a, const reg4d& b) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg2d fmadd2d(const reg2d& a, const reg2d& b, const reg2d& c) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg4d fmadd4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d fmsub4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d fmnadd4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d fmnsub4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept;

		template <uint32_t m0, uint32_t m1, uint32_t m2, uint32_t m3>
		[[gnu::always_inline, nodiscard]] inline reg4d permute4d(const reg4d& a) noexcept;

		template <uint32_t mask>
		[[gnu::always_inline, nodiscard]] inline reg4d blend4d(const reg4d& a, const reg4d& b) noexcept;

		#if defined(__AVX512VL__)
			#define BENCH_UTILS_SIMD_REG4D_VL
			
			[[gnu::always_inline, nodiscard]] inline reg4d permutex2var4d(const reg4d& a, const reg4d& b, const reg4i64& idx) noexcept;
		#endif
	} // namespace simd
} // namespace bench_utils

#include "avx2Impl.h"

#endif
