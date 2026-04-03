#pragma once

namespace bench_utils {
	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>::buffer(const ALLOCATOR& allocator) noexcept : alloc(allocator), size(0), data(nullptr) {
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>::buffer(uint64_t size_n, const ALLOCATOR& allocator) noexcept : alloc(allocator) {
		static_assert(std::is_default_constructible_v<TYPE>);
		try {
			this->data = alloc.allocate(size_n);
			std::uninitialized_default_construct_n(this->data, size_n);
			this->size = size_n;
		} catch (const std::bad_alloc&) {
			this->data = nullptr;
			this->size = 0ull;
		}
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>::buffer(std::nullptr_t, const ALLOCATOR& allocator) noexcept : alloc(allocator), size(0ull), data(nullptr) {
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>::buffer(uint64_t size_n, TYPE* data_n, const ALLOCATOR& allocator) noexcept : alloc(allocator), size(size_n), data(data_n) {
	}

	template <typename TYPE, typename ALLOCATOR>
	void buffer<TYPE, ALLOCATOR>::realloc(uint64_t size_n) noexcept {
		free();
		try {
			this->data = alloc.allocate(size_n);
			std::uninitialized_default_construct_n(this->data, size_n);
			this->size = size_n;
		} catch (const std::bad_alloc&) {
			this->data = nullptr;
			this->size = 0ull;
		}
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>::buffer(buffer<TYPE, ALLOCATOR>&& source) noexcept : alloc(std::move(source.alloc)), size(source.size), data(source.data) {
		source.size = 0ull;
		source.data = nullptr;
	}

	template <typename TYPE, typename ALLOCATOR>
	void buffer<TYPE, ALLOCATOR>::free() noexcept {
		static_assert(std::is_destructible_v<TYPE>);
		if (data == nullptr) {
			return;
		}
		std::destroy_n(data, size);
		alloc.deallocate(data, size);
		data = nullptr;
		size = 0ull;
	}

	template <typename TYPE, typename ALLOCATOR>
	constexpr TYPE* buffer<TYPE, ALLOCATOR>::begin() noexcept {
		return data;
	}

	template <typename TYPE, typename ALLOCATOR>
	constexpr const TYPE* buffer<TYPE, ALLOCATOR>::begin() const noexcept {
		return data;
	}

	template <typename TYPE, typename ALLOCATOR>
	constexpr TYPE* buffer<TYPE, ALLOCATOR>::end() noexcept {
		return data + size;
	}

	template <typename TYPE, typename ALLOCATOR>
	constexpr const TYPE* buffer<TYPE, ALLOCATOR>::end() const noexcept {
		return data + size;
	}

	template <typename TYPE, typename ALLOCATOR>
	TYPE& buffer<TYPE, ALLOCATOR>::operator[](std::size_t idx) noexcept {
		return data[idx];
	}

	template <typename TYPE, typename ALLOCATOR>
	const TYPE& buffer<TYPE, ALLOCATOR>::operator[](std::size_t idx) const noexcept {
		return data[idx];
	}

	template <typename TYPE, typename ALLOCATOR>
	template <typename OTHERTYPE, typename OTHERALLOCATOR>
	buffer<TYPE, ALLOCATOR>::operator buffer<OTHERTYPE, OTHERALLOCATOR>() noexcept {
		static_assert(sizeof(OTHERTYPE) % sizeof(TYPE) == 0);
		buffer<OTHERTYPE> buff(size * sizeof(TYPE) / sizeof(OTHERTYPE), reinterpret_cast<OTHERTYPE*>(data));
		return buff;
	}


	template <typename TYPE, typename ALLOCATOR>
	template <typename OTHERTYPE, typename OTHERALLOCATOR>
	buffer<TYPE, ALLOCATOR>::operator buffer<OTHERTYPE, OTHERALLOCATOR>() const noexcept {
		static_assert(sizeof(OTHERTYPE) % sizeof(TYPE) == 0);
		const buffer<OTHERTYPE> buff(size * sizeof(TYPE) / sizeof(OTHERTYPE), reinterpret_cast<OTHERTYPE*>(data));
		return buff;
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>::operator TYPE*() noexcept {
		return data;
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>::operator const TYPE*() const noexcept {
		return data;
	}

	template <typename TYPE, typename ALLOCATOR>
	template <typename OTHERTYPE>
	buffer<TYPE, ALLOCATOR>::operator OTHERTYPE*() noexcept {
		static_assert(sizeof(OTHERTYPE) % sizeof(TYPE) == 0);
		return static_cast<OTHERTYPE*>(data);
	}

	template <typename TYPE, typename ALLOCATOR>
	template <typename OTHERTYPE>
	buffer<TYPE, ALLOCATOR>::operator const OTHERTYPE*() noexcept {
		static_assert(sizeof(OTHERTYPE) % sizeof(TYPE) == 0);
		return static_cast<const OTHERTYPE*>(data);
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>& buffer<TYPE, ALLOCATOR>::operator=(std::nullptr_t) noexcept {
		size = 0;
		data = nullptr;
		return *this;
	}

	template <typename TYPE, typename ALLOCATOR>
	buffer<TYPE, ALLOCATOR>& buffer<TYPE, ALLOCATOR>::operator=(buffer<TYPE, ALLOCATOR>&& source) noexcept {
		this->alloc = std::move(source.alloc);
		this->size = source.size;
		this->data = source.data;
		source.size = 0ull;
		source.data = nullptr;
		return *this;
	}

	template <typename TYPE, typename ALLOCATOR>
	bool operator==(buffer<TYPE, ALLOCATOR>& buffer, const void* ptr) noexcept {
		return buffer.data == ptr;
	}

	template <typename TYPE, typename ALLOCATOR>
	bool operator!=(buffer<TYPE, ALLOCATOR>& buffer, const void* ptr) noexcept {
		return buffer.data != ptr;
	}
} // namspace fdtd_bench
