#pragma once
#include <cmath>
#include <iostream>
#include <bench_utils/isa/intrin.h>

// TODO proper float comparison, for now remove the noise
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wshadow"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wshadow"
#endif

#if defined(BENCH_UTILS_SIMD_REG4D)
#define VEC3D_ALIGN alignof(simd::reg4d)
#else
#define VEC3D_ALIGN 32
#endif

namespace bench_utils {
	namespace math {
		struct alignas(VEC3D_ALIGN) vec3d {
			double x = 0;
			double y = 0;
			double z = 0;

			private:
			[[maybe_unused]] double padding = 0; // padding to allow for better use with reg4d if available
			public:

			vec3d() = default;
			constexpr vec3d(double scalar) noexcept;
			constexpr vec3d(double x, double y, double z) noexcept;
			vec3d(const vec3d&) = default;
			vec3d(vec3d&&) noexcept = default;
			~vec3d() = default; 

			constexpr vec3d& set(const vec3d& vector) noexcept;
			constexpr vec3d& set(double x, double y, double z) noexcept;

			[[nodiscard]] constexpr double mag2() const noexcept;
			[[nodiscard]] inline double mag() const noexcept;
			inline vec3d& nor() noexcept;
			[[nodiscard]] constexpr double dst2(const vec3d& vector) const noexcept;
			[[nodiscard]] inline double dst(const vec3d& vector) const noexcept;
			[[nodiscard]] constexpr double dot(const vec3d& vector) const noexcept;
			constexpr vec3d& crs(const vec3d& vector) noexcept;

			[[nodiscard]] constexpr bool operator==(const vec3d& vector) const noexcept;
			[[nodiscard]] constexpr bool operator!=(const vec3d& vector) const noexcept;

			constexpr vec3d& operator+=(const vec3d& vector) noexcept;
			constexpr vec3d& operator-=(const vec3d& vector) noexcept;

			constexpr vec3d& operator+=(double value) noexcept;
			constexpr vec3d& operator-=(double value) noexcept;
			constexpr vec3d& operator*=(double value) noexcept;
			constexpr vec3d& operator/=(double value) noexcept;

			vec3d& operator=(const vec3d&) = default;
			vec3d& operator=(vec3d&&) noexcept = default;
		};

		constexpr vec3d operator+(const vec3d& left, const vec3d& right) noexcept;
		constexpr vec3d operator-(const vec3d& left, const vec3d& right) noexcept;

		constexpr vec3d operator+(const vec3d& left, double value) noexcept;
		constexpr vec3d operator-(const vec3d& left, double value) noexcept;
		constexpr vec3d operator*(const vec3d& left, double value) noexcept;
		constexpr vec3d operator/(const vec3d& left, double value) noexcept;

		std::ostream& operator<<(std::ostream& stream, const vec3d& vector);
	} // namespace math
} // namespace bench_utils

#include "vec3dImpl.h"
