#pragma once
#if defined(__AVX512F__)

#define BENCH_UTILS_SIMD_REG8D

namespace bench_utils {
	namespace simd {
		using reg8d = __m512d;
		using reg8i64 = __m512i;
		using mask8 = __mmask8;

		enum reg_loc {
			LOWER,
			UPPER
		};

		template <int64_t m0, int64_t m1, int64_t m2, int64_t m3, int64_t m4, int64_t m5, int64_t m6, int64_t m7>
		[[gnu::always_inline, nodiscard]] inline reg8i64 create_mask_8i64() noexcept;

		[[gnu::always_inline, nodiscard]] inline reg8d zero_reg8d() noexcept;

		[[gnu::always_inline, nodiscard]] inline reg8d load8d8d(const double& data) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d load4d8d_both(const double& data) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d load4d8d_lower(const double& data) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d load1d8d(const double& value) noexcept;

		[[gnu::always_inline]] inline void store8d(double& out, const reg8d& reg) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg8d reg4d8d(const reg4d& a) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d reg4d8d(const reg4d& a, const reg4d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg4d reg8d4d(const reg8d& a) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg8d add8d(const reg8d& a, const reg8d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d sub8d(const reg8d& a, const reg8d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d mul8d(const reg8d& a, const reg8d& b) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d div8d(const reg8d& a, const reg8d& b) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg8d fmadd8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d fmsub8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d fmnadd8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d fmnsub8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d fmnadd8dmask3(const reg8d& a, const reg8d& b, const reg8d& c, const mask8& m) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg8d blend8dmask(const reg8d& a, const reg8d& b, const mask8& mask) noexcept;

		[[gnu::always_inline, nodiscard]] inline reg8d permutexvar8d(const reg8d& a, const reg8i64& mask) noexcept;
		[[gnu::always_inline, nodiscard]] inline reg8d permutex2var8d(const reg8d& a, const reg8d& b, const reg8i64& idx) noexcept;
	} // namespace simd
} // namespace bench_utils

#include "avx512Impl.h"

#endif
