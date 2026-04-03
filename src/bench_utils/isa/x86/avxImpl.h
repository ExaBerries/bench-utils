#pragma once

namespace bench_utils {
	namespace simd {
		[[gnu::always_inline, nodiscard]] inline reg2d load2d2d(const double& data) noexcept {
			return _mm_load_pd(&data);
		}

		[[gnu::always_inline, nodiscard]] inline reg2d load1d2d(const double& value) noexcept {
			return _mm_load1_pd(&value);
		}

		[[gnu::always_inline, nodiscard]] inline reg2d load1d2d_lo(const double& value) noexcept {
			return _mm_load_sd(&value);
		}

		[[gnu::always_inline]] inline void store2d(double& out, const reg2d& reg) noexcept {
			_mm_store_pd(&out, reg);
		}

		[[gnu::always_inline]] inline void store2d_lo(double& out, const reg2d& reg) noexcept {
			_mm_store_sd(&out, reg);
		}

		[[gnu::always_inline, nodiscard]] inline reg2d add2d(const reg2d& a, const reg2d& b) noexcept {
			return _mm_add_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg2d sub2d(const reg2d& a, const reg2d& b) noexcept {
			return _mm_sub_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg2d mul2d(const reg2d& a, const reg2d& b) noexcept {
			return _mm_mul_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg2d div2d(const reg2d& a, const reg2d& b) noexcept {
			return _mm_div_pd(a, b);
		}
	} // namespace simd
} // namespace bench_utils
