#pragma once

namespace bench_utils {
	namespace simd {
		template <int64_t m0, int64_t m1, int64_t m2, int64_t m3, int64_t m4, int64_t m5, int64_t m6, int64_t m7>
		[[gnu::always_inline, nodiscard]] inline reg8i64 create_mask_8i64() noexcept {
			alignas(alignof(reg8i64)) static constexpr int64_t mask_data[8] = {
				m0, m1, m2, m3, m4, m5, m6, m7
			};
			return _mm512_load_si512(reinterpret_cast<const reg8i64*>(mask_data));
		}

		[[gnu::always_inline, nodiscard]] inline reg8d zero_reg8d() noexcept {
			return _mm512_setzero_pd();
		}

		[[gnu::always_inline, nodiscard]] inline reg8d load8d8d(const double& data) noexcept {
			return _mm512_load_pd(&data);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d load4d8d_both(const double& data) noexcept {
			return _mm512_broadcast_f64x4(_mm256_load_pd(&data));
		}

		[[gnu::always_inline, nodiscard]] inline reg8d load4d8d_lower(const double& data) noexcept {
			return _mm512_castpd256_pd512(_mm256_load_pd(&data));
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d load1d8d(const double& value) noexcept {
			return _mm512_set1_pd(value);
		}

		[[gnu::always_inline]] inline void store8d(double& out, const reg8d& reg) noexcept {
			_mm512_store_pd(&out, reg);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d reg4d8d(const reg4d& a) noexcept {
			return _mm512_castpd256_pd512(a);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d reg4d8d(const reg4d& a, const reg4d& b) noexcept {
			return _mm512_insertf64x4(_mm512_castpd256_pd512(a), b, 1);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d reg8d4d(const reg8d& a) noexcept {
			return _mm512_castpd512_pd256(a);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d add8d(const reg8d& a, const reg8d& b) noexcept {
			return _mm512_add_pd(a, b);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d sub8d(const reg8d& a, const reg8d& b) noexcept {
			return _mm512_sub_pd(a, b);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d mul8d(const reg8d& a, const reg8d& b) noexcept {
			return _mm512_mul_pd(a, b);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d div8d(const reg8d& a, const reg8d& b) noexcept {
			return _mm512_div_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d fmadd8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept {
			return _mm512_fmadd_pd(a, b, c);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d fmsub8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept {
			return _mm512_fmsub_pd(a, b, c);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d fmnadd8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept {
			return _mm512_fnmadd_pd(a, b, c);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg8d fmnsub8d(const reg8d& a, const reg8d& b, const reg8d& c) noexcept {
			return _mm512_fnmsub_pd(a, b, c);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d fmnadd8dmask3(const reg8d& a, const reg8d& b, const reg8d& c, const mask8& m) noexcept {
			return _mm512_mask3_fnmadd_pd(a, b, c, m);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d blend8dmask(const reg8d& a, const reg8d& b, const mask8& mask) noexcept {
			return _mm512_mask_blend_pd(mask, a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d permutexvar8d(const reg8d& a, const reg8i64& mask) noexcept {
			return _mm512_permutexvar_pd(mask, a);
		}

		[[gnu::always_inline, nodiscard]] inline reg8d permutex2var8d(const reg8d& a, const reg8d& b, const reg8i64& idx) noexcept {
			return _mm512_permutex2var_pd(a, idx, b);
		}
	} // namespace simd
} // namespace bench_utils
