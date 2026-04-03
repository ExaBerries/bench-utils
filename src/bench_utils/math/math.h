#pragma once
#include <bench_utils/math/vec3d.h>

namespace bench_utils {
	namespace math {
		template <typename T>
		[[nodiscard]] constexpr T cube(T n) noexcept {
			return n * n * n;
		}
	} // namespace math	
} // namespace bench_utils
