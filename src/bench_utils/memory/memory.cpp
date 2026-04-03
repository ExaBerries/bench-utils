#include <bench_utils/memory/memory.h>
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
#elif defined(__linux__)
	#include <sys/mman.h>
	#include <fstream>
	#include <string>
#endif

namespace bench_utils {
	[[nodiscard]] uint64_t get_available_mem() noexcept {
		#if defined(_WIN32)
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			if (GlobalMemoryStatusEx(&memInfo)) {
				DWORDLONG availablePhysicalMemory = memInfo.ullAvailPhys;
				return availablePhysicalMemory;
			} else {
				return 0ull;
			}
		#elif defined(__linux__)
			std::string line;
			auto huge_page_size_kb = 0ull;
			std::ifstream file("/proc/meminfo");
			if (file.is_open()) {
				while (std::getline(file, line)) {
					if (line.find("MemAvailable:") != std::string::npos) {
						auto start = line.find_first_of("0123456789");
						auto end = line.find(" kB", start);
						if (start != std::string::npos) {
							huge_page_size_kb = std::stoull(line.substr(start, end - start));
						}
						break;
					}
				}
				file.close();
			}
			return huge_page_size_kb * 1024ull;
		#else
			return 0ull;
		#endif
	}

	static void enable_large_pages() noexcept {
		#if defined(_WIN32)
			HANDLE hToken;
			TOKEN_PRIVILEGES tp;
			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
			LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &tp.Privileges[0].Luid);
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		#endif
	}

	[[nodiscard]] static std::size_t get_large_page_size_impl() noexcept {
		enable_large_pages();
		#if defined(_WIN32)
			return GetLargePageMinimum();
		#elif defined(__linux__)
			std::string line;
			auto avil_mem_kb = 0ull;
			std::ifstream file("/proc/meminfo");
			if (file.is_open()) {
				while (std::getline(file, line)) {
					if (line.find("Hugepagesize:") != std::string::npos) {
						auto start = line.find_first_of("0123456789");
						auto end = line.find(" kB", start);
						if (start != std::string::npos) {
							avil_mem_kb = std::stoull(line.substr(start, end - start));
						}
						break;
					}
				}
				file.close();
			}
			return avil_mem_kb * 1024ull;
		#else
			return 0ull;
		#endif
	}

	[[nodiscard]] std::size_t get_large_page_size() noexcept {
		static auto page_size = get_large_page_size_impl();
		return page_size;
	}

	[[nodiscard]] void* try_malloc_large_page(std::size_t bytes) noexcept {
		const auto large_page_size = get_large_page_size();
		[[maybe_unused]] auto bytes_rounded_up = (bytes + large_page_size - 1) & ~(large_page_size - 1);
		#if defined(_WIN32)
			return VirtualAlloc(NULL, bytes_rounded_up, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
		#elif defined(__linux__)
			auto ptr = mmap(nullptr, bytes_rounded_up, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
			if (ptr == MAP_FAILED) {
				ptr = nullptr;
			}
			return ptr;
		#else
			return nullptr;
		#endif
	}

	void free_large_page([[maybe_unused]] void* ptr, [[maybe_unused]] std::size_t bytes) noexcept {
		[[maybe_unused]] const auto large_page_size = get_large_page_size();
		if (!ptr) {
			return;
		}

		#if defined(_WIN32)
			VirtualFree(ptr, 0, MEM_RELEASE);
		#elif defined(__linux__)
			auto bytes_rounded_up = (bytes + large_page_size - 1) & ~(large_page_size - 1);
			munmap(ptr, bytes_rounded_up);
		#else
			return;
		#endif
	}

	[[nodiscard]] void* try_malloc_aligned_64(std::size_t bytes) noexcept {
		#if defined(_WIN32)
			auto ptr = _aligned_malloc(bytes, 64);
			return ptr;
		#else
			void* ptr = nullptr;
			if (posix_memalign(&ptr, 64, bytes) != 0) {
				ptr = nullptr;
			}
			return ptr;
		#endif
	}

	void free_aligned_64(void* ptr, [[maybe_unused]] std::size_t bytes) noexcept {
		#if defined(_WIN32)
			_aligned_free(ptr);
		#else
			std::free(ptr);
		#endif
	}
} // namespace bench_utils
