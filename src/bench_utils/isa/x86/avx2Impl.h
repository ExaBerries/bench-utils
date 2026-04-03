#pragma once

namespace bench_utils {
	namespace simd {
		template <int64_t m0, int64_t m1, int64_t m2, int64_t m3>
		[[gnu::always_inline, nodiscard]] inline reg4i64 create_mask_4i64() noexcept {
			alignas(alignof(reg4i64)) static constexpr int64_t mask_data[4] = {
				m0, m1, m2, m3
			};
			return _mm256_load_si256(reinterpret_cast<const reg4i64*>(mask_data));
		}

		[[gnu::always_inline, nodiscard]] inline reg4d load4d4d(const double& data) noexcept {
			return _mm256_load_pd(&data);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d load1d4d(const double& value) noexcept {
			return _mm256_set1_pd(value);
		}

		[[gnu::always_inline]] inline void store4d(double& out, const reg4d& reg) noexcept {
			_mm256_store_pd(&out, reg);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d add4d(const reg4d& a, const reg4d& b) noexcept {
			return _mm256_add_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d sub4d(const reg4d& a, const reg4d& b) noexcept {
			return _mm256_sub_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d mul4d(const reg4d& a, const reg4d& b) noexcept {
			return _mm256_mul_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d div4d(const reg4d& a, const reg4d& b) noexcept {
			return _mm256_div_pd(a, b);
		}

		[[gnu::always_inline, nodiscard]] inline reg2d fmadd2d(const reg2d& a, const reg2d& b, const reg2d& c) noexcept {
			return _mm_fmadd_sd(a, b, c);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d fmadd4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept {
			return _mm256_fmadd_pd(a, b, c);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d fmsub4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept {
			return _mm256_fmsub_pd(a, b, c);
		}
		
		[[gnu::always_inline, nodiscard]] inline reg4d fmnadd4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept {
			return _mm256_fnmadd_pd(a, b, c);
		}

		[[gnu::always_inline, nodiscard]] inline reg4d fmnsub4d(const reg4d& a, const reg4d& b, const reg4d& c) noexcept {
			return _mm256_fnmsub_pd(a, b, c);
		}

		template <uint32_t m0, uint32_t m1, uint32_t m2, uint32_t m3>
		[[gnu::always_inline, nodiscard]] inline reg4d permute4d(const reg4d& a) noexcept {
			return _mm256_permute4x64_pd(a, _MM_SHUFFLE(m0, m1, m2, m3));
		}

		template <uint32_t mask>
		[[gnu::always_inline, nodiscard]] inline reg4d blend4d(const reg4d& a, const reg4d& b) noexcept {
			return _mm256_blend_pd(a, b, mask);
		}

		#if defined(__AVX512VL__)
			#define BENCH_UTILS_SIMD_REG4D_VL
			
			[[gnu::always_inline, nodiscard]] inline reg4d permutex2var4d(const reg4d& a, const reg4d& b, const reg4i64& idx) noexcept {
				return _mm256_permutex2var_pd(a, idx, b);
			}
		#endif
	} // namespace simd
} // namspace fdtd_bench
