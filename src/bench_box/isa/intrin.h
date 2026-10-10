#pragma once

#include <bench_box/isa/isa_macro.h>

#if defined(BENCH_BOX_SIMD_USE_INTRIN_AVX512F)
#define BENCH_BOX_SIMD_USE_INTRIN_STR "intrin-avx512f"
#elif defined(BENCH_BOX_SIMD_USE_INTRIN_AVX512VL)
#define BENCH_BOX_SIMD_USE_INTRIN_STR "intrin-avx512vl"
#elif defined(BENCH_BOX_SIMD_USE_INTRIN_FMA)
#define BENCH_BOX_SIMD_USE_INTRIN_STR "intrin-fma"
#endif

#if defined(BENCH_BOX_ISA_X86) || defined(BENCH_BOX_ISA_X86_64)
	#include <immintrin.h>
	#include <bench_box/isa/x86/misc.h>

	#if defined(__AVX__)
		#include <xmmintrin.h>

		#include <bench_box/isa/x86/misc.h>

		#include <bench_box/isa/x86/avx.h>

		#if defined(__AVX2__)
			#include <bench_box/isa/x86/avx2.h>
		#endif

		#if defined(__AVX512F__)
			#include <bench_box/isa/x86/avx512.h>
		#endif
	#endif
#endif
