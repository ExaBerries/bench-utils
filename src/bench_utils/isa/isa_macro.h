#pragma once

#if defined(__x86_64__) || defined(_M_X64)
	#define BENCH_UTILS_ISA_X86_64
#elif defined(__i386__) || defined(_M_IX86)
	#define BENCH_UTILS_ISA_X86
#elif defined(__aarch64__) || defined(_M_ARM64)
	#define BENCH_UTILS_ISA_ARM64
#endif
