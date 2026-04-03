#pragma once
#include <type_traits>

namespace bench_utils {
	namespace math {
		constexpr vec3d::vec3d(double scalar) noexcept : x(scalar), y(scalar), z(scalar) {
		}
		
		constexpr vec3d::vec3d(double x, double y, double z) noexcept : x(x), y(y), z(z) {
		}

		constexpr vec3d& vec3d::set(const vec3d& vector) noexcept {
			x = vector.x;
			y = vector.y;
			z = vector.z;
			return *this;
		}

		constexpr vec3d& vec3d::set(double nx, double ny, double nz) noexcept {
			x = nx;
			y = ny;
			z = nz;
			return *this;
		}

		constexpr double vec3d::mag2() const noexcept {
			#if defined(BENCH_UTILS_SIMD_USE_INTRIN_FMA)
			if (std::is_constant_evaluated()) {
			#endif

				return x * x + y * y + z * z;

			#if defined(BENCH_UTILS_SIMD_USE_INTRIN_FMA)
			} else {
				// yes were loading only the lower half of each, mostly taking advantage of FMA
				simd::reg2d rx = simd::load1d2d_lo(x);
				simd::reg2d ry = simd::load1d2d_lo(y);
				simd::reg2d rz = simd::load1d2d_lo(z);
				simd::reg2d i = simd::mul2d(ry, ry); // y^2
				i = simd::fmadd2d(rx, rx, i); // += y^2
				i = simd::fmadd2d(rz, rz, i); // += z^2
				double out;
				simd::store2d_lo(out, i);
				return out;
			}
			#endif
		}

		inline double vec3d::mag() const noexcept {
			return std::sqrt(mag2());
		}

		inline vec3d& vec3d::nor() noexcept {
			double mag2 = vec3d::mag2();
			if (mag2 != 0.0 && mag2 != 1.0) {
				double mag = std::sqrt(mag2);
				x /= mag;
				y /= mag;
				z /= mag;
			}
			return *this;
		}

		constexpr double vec3d::dst2(const vec3d& vector) const noexcept {
			return (*this - vector).mag2();
		}

		inline double vec3d::dst(const vec3d& vector) const noexcept {
			return std::sqrt(dst2(vector));
		}

		constexpr double vec3d::dot(const vec3d& vector) const noexcept {
			return vector.x * x + vector.y * y + vector.z * z;
		}

		constexpr vec3d& vec3d::crs(const vec3d& vector) noexcept {
			double cx = y * vector.z - z * vector.y;
			double cy = z * vector.x - x * vector.z;
			double cz = x * vector.y - y * vector.x;
			x = cx;
			y = cy;
			z = cz;
			return *this;
		}

		constexpr bool vec3d::operator==(const vec3d& vector) const noexcept {
			return (x == vector.x && y == vector.y && z == vector.z);
		}

		constexpr bool vec3d::operator!=(const vec3d& vector) const noexcept {
			return (x != vector.x || y != vector.y || z != vector.z);
		}

		constexpr vec3d operator+(const vec3d& left, const vec3d& right) noexcept {
			return {left.x + right.x, left.y + right.y, left.z + right.z};
		}

		constexpr vec3d operator-(const vec3d& left, const vec3d& right) noexcept {
			return {left.x - right.x, left.y - right.y, left.z - right.z};
		}

		constexpr vec3d operator+(const vec3d& left, double value) noexcept {
			return {left.x + value, left.y + value, left.z + value};
		}

		constexpr vec3d operator-(const vec3d& left, double value) noexcept {
			return {left.x - value, left.y - value, left.z - value};
		}

		constexpr vec3d operator*(const vec3d& left, double value) noexcept {
			return {left.x * value, left.y * value, left.z * value};
		}

		constexpr vec3d operator/(const vec3d& left, double value) noexcept {
			return {left.x / value, left.y / value, left.z / value};
		}

		constexpr vec3d& vec3d::operator+=(const vec3d& vector) noexcept {
			x += vector.x;
			y += vector.y;
			z += vector.z;
			return *this;
		}

		constexpr vec3d& vec3d::operator-=(const vec3d& vector) noexcept {
			x -= vector.x;
			y -= vector.y;
			z -= vector.z;
			return *this;
		}

		constexpr vec3d& vec3d::operator+=(double value) noexcept {
			x += value;
			y += value;
			z += value;
			return *this;
		}

		constexpr vec3d& vec3d::operator-=(double value) noexcept {
			x -= value;
			y -= value;
			z -= value;
			return *this;
		}

		constexpr vec3d& vec3d::operator*=(double value) noexcept {
			x *= value;
			y *= value;
			z *= value;
			return *this;
		}

		constexpr vec3d& vec3d::operator/=(double value) noexcept {
			x /= value;
			y /= value;
			z /= value;
			return *this;
		}
	} // namespace math
} // namespace bench_utils

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
