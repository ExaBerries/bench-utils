#pragma once
#include <bench_utils/isa/isa_macro.h>
#if defined(BENCH_UTILS_ISA_X86_64) || defined(BENCH_UTILS_ISA_X86)
#include <string>
#include <cstdint>

namespace bench_utils {
	struct x86_extensions {
		bool mmx = false;
		bool sse = false;
		bool sse2 = false;
		bool sse3 = false;
		bool ssse3 = false;
		bool sse4_1 = false;
		bool sse4_2 = false;
		bool sse4a = false;
		bool popcnt = false;
		bool aes = false;
		bool rdrand = false;
		bool bmi1 = false;
		bool bmi2 = false;
		bool lm = false;
		bool lzcnt = false;
		bool xop = false;
		bool avx = false;
		bool avx2 = false;
		bool fma = false;
		bool avx512f = false;
		bool avx512bw = false;
		bool avx512vl = false;
		bool avx512dq = false;
		bool avx512ifma = false;
		bool avx512pf = false;
		bool avx512er = false;
		bool avx512cd = false;
		bool avx512vbmi = false;
		bool avx512vbmi2 = false;
		bool avx512vnni = false;
		bool avx512bitalg = false;
		bool avx512vpopcntdq = false;
		bool gfni = false;
		bool vaes = false;
		bool vpclmulqdq = false;
		bool avx_vnni = false;
		bool avx_ifma = false;
		bool sha = false;
		bool movdiri = false;
		bool movdir64b = false;
		bool serialize = false;
		bool tsxldtrk = false;
		bool pku = false;
		bool ospke = false;
		bool rdpid = false;
		bool waitpkg = false;
		bool clwb = false;
		bool clflushopt = false;
		bool sgx = false;
		bool avx512_4vnniw = false;
		bool avx512_4fmaps = false;
		bool avx512_vp2intersect = false;
		bool amx_tile = false;
		bool amx_int8 = false;
		bool amx_bf16 = false;
		bool amx_fp16 = false;
		bool ibt = false;
		bool shstk = false;
		bool fsrm = false;
		bool hreset = false;
		bool uintr = false;
		bool avx512_bf16 = false;
		bool avx512_fp16 = false;
	};

	[[nodiscard]] std::string get_cpu_brand_string() noexcept;
	[[nodiscard]] x86_extensions get_cpu_supported_isas() noexcept;
	[[nodiscard]] std::string get_cpu_isa_string() noexcept;
} // namespace bench_utils
#endif
