#pragma once
#include <memory>
#include <cstdint>

namespace bench_utils {
	template <typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
	struct buffer {
		using ElementType = TYPE;
		using AllocatorType = ALLOCATOR;
		ALLOCATOR alloc = ALLOCATOR();
		uint64_t size = 0;
		TYPE* data = nullptr;

		buffer() noexcept = default;
		buffer(const ALLOCATOR& allocator = ALLOCATOR()) noexcept;
		buffer(uint64_t size_n, const ALLOCATOR& allocator = ALLOCATOR()) noexcept;
		buffer(uint64_t size_n, TYPE* data_n, const ALLOCATOR& allocator = ALLOCATOR()) noexcept;
		buffer(std::nullptr_t, const ALLOCATOR& allocator = ALLOCATOR()) noexcept;
		buffer(const buffer&) noexcept = delete;
		buffer(buffer&& source) noexcept;
		~buffer() noexcept = default;

		void realloc(uint64_t size_n) noexcept;
		void free() noexcept;

		[[nodiscard]] constexpr TYPE* begin() noexcept;
		[[nodiscard]] constexpr const TYPE* begin() const noexcept;
		[[nodiscard]] constexpr TYPE* end() noexcept;
		[[nodiscard]] constexpr const TYPE* end() const noexcept;

		[[nodiscard]] TYPE& operator[](std::size_t idx) noexcept;
		[[nodiscard]] const TYPE& operator[](std::size_t idx) const noexcept;

		template <typename OTHERTYPE, typename OTHERALLOCATOR>
		[[nodiscard]] explicit operator buffer<OTHERTYPE, OTHERALLOCATOR>() noexcept;
		template <typename OTHERTYPE, typename OTHERALLOCATOR>
		[[nodiscard]] explicit operator buffer<OTHERTYPE, OTHERALLOCATOR>() const noexcept;
		[[nodiscard]] operator TYPE*() noexcept;
		[[nodiscard]] operator const TYPE*() const noexcept;
		template <typename OTHERTYPE>
		[[nodiscard]] explicit operator OTHERTYPE*() noexcept;
		template <typename OTHERTYPE>
		[[nodiscard]] explicit operator const OTHERTYPE*() noexcept;

		buffer& operator=(std::nullptr_t) noexcept;
		buffer& operator=(const buffer&) = delete;
		buffer& operator=(buffer&&) noexcept;
	};

	template <typename TYPE, typename ALLOCATOR>
	[[nodiscard]] bool operator==(buffer<TYPE, ALLOCATOR>& buffer, const void* ptr) noexcept;

	template <typename TYPE, typename ALLOCATOR>
	[[nodiscard]] bool operator!=(buffer<TYPE, ALLOCATOR>& buffer, const void* ptr) noexcept;
} // namespace bench_utils

#include "bufferImpl.h"
