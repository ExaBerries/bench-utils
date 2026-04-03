#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <new>
#include <iostream>

namespace bench_utils {
	[[nodiscard]] uint64_t get_available_mem() noexcept;
	[[nodiscard]] std::size_t get_large_page_size() noexcept;

	[[nodiscard]] void* try_malloc_large_page(std::size_t bytes) noexcept;
	void free_large_page(void* ptr, std::size_t bytes) noexcept;

	[[nodiscard]] void* try_malloc_aligned_64(std::size_t bytes) noexcept;
	void free_aligned_64(void* ptr, [[maybe_unused]] std::size_t bytes) noexcept;

	template <typename T>
	struct lp_flex_allocator_t {
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;
		using is_always_equal = std::false_type;

		// don't modify after creation
		const bool use_large_pages = false;

		lp_flex_allocator_t() noexcept = default;
		explicit lp_flex_allocator_t(bool use_lp) noexcept : use_large_pages(use_lp) {}

		template <typename U>
		lp_flex_allocator_t(const lp_flex_allocator_t<U>& other) noexcept : use_large_pages(other.use_large_pages) {}

		template <typename U>
		struct rebind { using other = lp_flex_allocator_t<U>; };

		value_type* allocate(size_type n) {
			if (n == 0) return nullptr;
			void* ptr = use_large_pages ? try_malloc_large_page(n * sizeof(T)) : try_malloc_aligned_64(n * sizeof(T));
			if (!ptr) {
				throw std::bad_alloc();
			}
			return static_cast<value_type*>(ptr);
		}

		void deallocate(value_type* ptr, size_type n) noexcept {
			if (!ptr) return;
			if (use_large_pages) {
				free_large_page(ptr, n * sizeof(T));
			} else {
				free_aligned_64(ptr, n * sizeof(T));
			}
		}

		bool operator==(const lp_flex_allocator_t& other) const noexcept {
			return use_large_pages == other.use_large_pages;
		}

		bool operator!=(const lp_flex_allocator_t& other) const noexcept {
			return !(*this == other);
		}
	};
} // namespace bench_utils
