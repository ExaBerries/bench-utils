#include <bench_utils/isa/x86/cpuid.h>
#if defined(BENCH_UTILS_ISA_X86_64) || defined(BENCH_UTILS_ISA_X86)
#include <array>
#include <string>
#include <cstring>
#include <vector>

#if defined(_MSC_VER)
	#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
	#include <cpuid.h>
#endif

namespace bench_utils {
	inline void cpuid(uint32_t leaf, uint32_t subleaf, int32_t regs[4]) noexcept {
		#if defined(_MSC_VER)
			__cpuidex(regs, leaf, subleaf);
		#else
			__cpuid_count(leaf, subleaf, regs[0], regs[1], regs[2], regs[3]);
		#endif
	}

	inline uint64_t xgetbv(uint32_t index) noexcept {
		#if defined(_MSC_VER)
			return _xgetbv(index);
		#else
			uint32_t eax;
			uint32_t edx;
			__asm__ volatile ("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
			return (static_cast<uint64_t>(edx) << 32) | eax;
		#endif
	}


	[[nodiscard]] std::string get_cpu_brand_string() noexcept {
		[[maybe_unused]] std::array<int32_t, 4> cpui{};
		std::array<char, 49> brand{};

		#if defined(_MSC_VER)
			__cpuid(cpui.data(), 0x80000000);
			uint32_t max_ext_id = cpui[0];

			if (max_ext_id >= 0x80000004) {
				__cpuid(reinterpret_cast<int*>(brand.data() + 0), 0x80000002);
				__cpuid(reinterpret_cast<int*>(brand.data() + 16), 0x80000003);
				__cpuid(reinterpret_cast<int*>(brand.data() + 32), 0x80000004);
			}
		#elif defined(__GNUC__) || defined(__clang__)
			uint32_t eax = 0u;
			uint32_t ebx = 0u;
			uint32_t ecx = 0u;
			uint32_t edx = 0u;

			if (__get_cpuid_max(0x80000000, nullptr) >= 0x80000004) {
				__get_cpuid(0x80000002, &eax, &ebx, &ecx, &edx);
				std::memcpy(brand.data() + 0, &eax, 4);
				std::memcpy(brand.data() + 4, &ebx, 4);
				std::memcpy(brand.data() + 8, &ecx, 4);
				std::memcpy(brand.data() + 12, &edx, 4);

				__get_cpuid(0x80000003, &eax, &ebx, &ecx, &edx);
				std::memcpy(brand.data() + 16, &eax, 4);
				std::memcpy(brand.data() + 20, &ebx, 4);
				std::memcpy(brand.data() + 24, &ecx, 4);
				std::memcpy(brand.data() + 28, &edx, 4);

				__get_cpuid(0x80000004, &eax, &ebx, &ecx, &edx);
				std::memcpy(brand.data() + 32, &eax, 4);
				std::memcpy(brand.data() + 36, &ebx, 4);
				std::memcpy(brand.data() + 40, &ecx, 4);
				std::memcpy(brand.data() + 44, &edx, 4);
			}
		#endif

		brand[48] = '\0';
		return std::string(brand.data());
	}


	[[nodiscard]] x86_extensions get_cpu_supported_isas() noexcept {
		x86_extensions result{};
		int32_t r[4];

		cpuid(0, 0, r);
		auto max_basic = r[0];

		cpuid(0x80000000, 0, r);
		auto max_ext = r[0];

		cpuid(1, 0, r);
		auto ecx = r[2];
		auto edx = r[3];

		result.mmx = (edx & (1 << 23));
		result.sse = (edx & (1 << 25));
		result.sse2 = (edx & (1 << 26));
		result.sse3 = (ecx & (1 << 0));
		result.ssse3 = (ecx & (1 << 9));
		result.sse4_1 = (ecx & (1 << 19));
		result.sse4_2 = (ecx & (1 << 20));
		result.popcnt = (ecx & (1 << 23));
		result.aes = (ecx & (1 << 25));
		result.fma = (ecx & (1 << 12));
		result.rdrand = (ecx & (1 << 30));

		bool osxsave = (ecx & (1 << 27));
		bool avx_hw  = (ecx & (1 << 28));

		bool avx_enabled = false;
		bool avx512_os_enabled = false;

		if (osxsave && avx_hw) {
			uint64_t xcr0 = xgetbv(0);

			avx_enabled = ((xcr0 & 0x6) == 0x6);
			avx512_os_enabled = ((xcr0 & 0xE6) == 0xE6);

			result.avx = avx_enabled;
		}

		if (max_basic >= 7) {
			cpuid(7, 0, r);
			auto ebx = r[1];
			auto ecx7 = r[2];
			auto edx7 = r[3];

			result.bmi1 = (ebx & (1 << 3));
			result.bmi2 = (ebx & (1 << 8));

			result.avx2 = ((ebx & (1 << 5)) && avx_enabled);

			result.avx512f = ((ebx & (1 << 16)) && avx512_os_enabled);
			result.avx512dq = ((ebx & (1 << 17)) && avx512_os_enabled);
			result.avx512ifma = ((ebx & (1 << 21)) && avx512_os_enabled);
			result.avx512pf = ((ebx & (1 << 26)) && avx512_os_enabled);
			result.avx512er = ((ebx & (1 << 27)) && avx512_os_enabled);
			result.avx512cd = ((ebx & (1 << 28)) && avx512_os_enabled);
			result.avx512bw = ((ebx & (1 << 30)) && avx512_os_enabled);
			result.avx512vl = ((ebx & (1 << 31)) && avx512_os_enabled);

			result.avx512vbmi = ((ecx7 & (1 << 1)) && avx512_os_enabled);
			result.gfni = ((ecx7 & (1 << 8)) && avx_enabled);
			result.vaes = ((ecx7 & (1 << 9)) && avx_enabled);
			result.vpclmulqdq = ((ecx7 & (1 << 10)) && avx_enabled);
			result.avx512vnni = ((ecx7 & (1 << 11)) && avx512_os_enabled);
			result.avx512bitalg = ((ecx7 & (1 << 12)) && avx512_os_enabled);
			result.avx512vpopcntdq = ((ecx7 & (1 << 14)) && avx512_os_enabled);
			result.rdpid = (ecx7 & (1 << 22));
			result.clflushopt = (ecx7 & (1 << 23));
			result.clwb = (ecx7 & (1 << 24));
			result.sha = (ecx7 & (1 << 29));
			result.avx512vbmi2 = ((ecx7 & (1 << 6)) && avx512_os_enabled);
			result.waitpkg = (ecx7 & (1 << 5));
			result.movdiri = (ecx7 & (1 << 27));
			result.movdir64b = (ecx7 & (1 << 28));
			result.serialize = (ecx7 & (1 << 14));
			result.tsxldtrk = (ecx7 & (1 << 16));
			result.pku = (ecx7 & (1 << 3));
			result.ospke = (ecx7 & (1 << 4));

			result.avx512_4vnniw = ((edx7 & (1 << 2)) && avx512_os_enabled);
			result.avx512_4fmaps = ((edx7 & (1 << 3)) && avx512_os_enabled);
			result.avx512_vp2intersect = ((edx7 & (1 << 8)) && avx512_os_enabled);
			result.amx_tile = (edx7 & (1 << 24));
			result.amx_int8 = (edx7 & (1 << 25));
			result.amx_bf16 = (edx7 & (1 << 22));
			result.amx_fp16 = (edx7 & (1 << 23));
			result.ibt = (edx7 & (1 << 20));
			result.shstk = (edx7 & (1 << 7));
			result.fsrm = (edx7 & (1 << 4));
			result.hreset = (edx7 & (1 << 22));
			result.uintr = (edx7 & (1 << 5));

			cpuid(7, 1, r);
			auto eax1 = r[0];

			result.avx512_bf16 = ((eax1 & (1 << 5)) && avx512_os_enabled);
			result.avx512_fp16 = ((eax1 & (1 << 23)) && avx512_os_enabled);
		}

		if (max_ext >= 0x80000001) {
			cpuid(0x80000001, 0, r);
			auto ecx8 = r[2];
			auto edx8 = r[3];

			result.lm = (edx8 & (1 << 29));
			result.lzcnt = (ecx8 & (1 << 5));
			result.sse4a = (ecx8 & (1 << 6));
			result.xop = (ecx8 & (1 << 11));
		}

		return result;
	}

	std::string get_cpu_isa_string() noexcept {
		auto ext = get_cpu_supported_isas();
		std::string result;

		auto append = [&](bool supported, const char* name) {
			if (supported) {
				if (!result.empty()){
					result += ' ';
				}
				result += name;
			}
		};

		append(ext.mmx, "MMX");
		append(ext.sse, "SSE");
		append(ext.sse2, "SSE2");
		append(ext.sse3, "SSE3");
		append(ext.ssse3, "SSSE3");
		append(ext.sse4_1, "SSE4.1");
		append(ext.sse4_2, "SSE4.2");
		append(ext.sse4a, "SSE4a");
		append(ext.popcnt, "POPCNT");
		append(ext.aes, "AES");
		append(ext.fma, "FMA");
		append(ext.rdrand, "RDRAND");
		append(ext.bmi1, "BMI1");
		append(ext.bmi2, "BMI2");
		append(ext.lzcnt, "LZCNT");
		append(ext.xop, "XOP");
		append(ext.avx, "AVX");
		append(ext.avx2, "AVX2");
		append(ext.avx512f, "AVX512F");
		append(ext.avx512bw, "AVX512BW");
		append(ext.avx512vl, "AVX512VL");
		append(ext.avx512dq, "AVX512DQ");
		append(ext.avx512ifma, "AVX512IFMA");
		append(ext.avx512pf, "AVX512PF");
		append(ext.avx512er, "AVX512ER");
		append(ext.avx512cd, "AVX512CD");
		append(ext.avx512vbmi, "AVX512VBMI");
		append(ext.avx512vbmi2, "AVX512VBMI2");
		append(ext.avx512vnni, "AVX512VNNI");
		append(ext.avx512bitalg, "AVX512BITALG");
		append(ext.avx512vpopcntdq, "AVX512VPOPCNTDQ");
		append(ext.gfni, "GFNI");
		append(ext.vaes, "VAES");
		append(ext.vpclmulqdq, "VPCLMULQDQ");
		append(ext.sha, "SHA");
		append(ext.rdpid, "RDPID");
		append(ext.waitpkg, "WAITPKG");
		append(ext.movdiri, "MOVDIRI");
		append(ext.movdir64b, "MOVDIR64B");
		append(ext.serialize, "SERIALIZE");
		append(ext.tsxldtrk, "TSXLDTRK");
		append(ext.pku, "PKU");
		append(ext.ospke, "OSPKE");
		append(ext.sgx, "SGX");
		append(ext.clwb, "CLWB");
		append(ext.clflushopt, "CLFLUSHOPT");
		append(ext.avx512_bf16, "AVX512_BF16");
		append(ext.avx512_fp16, "AVX512_FP16");
		append(ext.avx512_4vnniw, "AVX512_4VNNIW");
		append(ext.avx512_4fmaps, "AVX512_4FMAPS");
		append(ext.avx512_vp2intersect, "AVX512_VP2INTERSECT");
		append(ext.amx_tile, "AMX_TILE");
		append(ext.amx_int8, "AMX_INT8");
		append(ext.amx_bf16, "AMX_BF16");
		append(ext.amx_fp16, "AMX_FP16");
		append(ext.ibt, "CET_IBT");
		append(ext.shstk, "CET_SHSTK");
		append(ext.fsrm, "FSRM");
		append(ext.hreset, "HRESET");
		append(ext.uintr, "UINTR");

		return result;
	}
} // namespace bench_utils
#endif
