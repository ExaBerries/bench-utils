#pragma once
#include <bench_utils/math/vec3d.h>
#include <limits>
#include <optional>

namespace bench_utils {
	namespace math {
		template <typename T>
		[[nodiscard]] constexpr T cube(T n) noexcept {
			return n * n * n;
		}

		template <typename TO, typename FROM>
		[[nodiscard]] constexpr std::optional<TO> checked_narrow(FROM value) noexcept {
			if (value > std::numeric_limits<TO>::max()) {
				return std::nullopt;
			}
			return static_cast<TO>(value);
		}

		template <typename T>
		[[nodiscard]] constexpr std::optional<T> checked_mul(T a, T b) noexcept {
			if (a != 0 && b > std::numeric_limits<T>::max() / a) {
				return std::nullopt;
			}
			return a * b;
		}

		template <typename T>
		[[nodiscard]] constexpr std::optional<T> checked_add(T a, T b) noexcept {
			if (a > std::numeric_limits<T>::max() - b) {
				return std::nullopt;
			}
			return a + b;
		}
	} // namespace math	
} // namespace bench_utils
