#include <gtest/gtest.h>
#include <bench_utils/memory/memory.h>
#include <bench_utils/memory/buffer.h>

TEST(Buffer, DefaultAlloc) {
	using namespace bench_utils;

	buffer<uint32_t> buf(64);
	buf.free();
}

TEST(Buffer, NonLP) {
	using namespace bench_utils;

	lp_flex_allocator_t<uint32_t> alloc(false);

	buffer<uint32_t, lp_flex_allocator_t<uint32_t>> buf(64u * 1024u, alloc);
	ASSERT_NE(buf, nullptr);
	buf.free();
}

TEST(Buffer, LP) {
	using namespace bench_utils;

	lp_flex_allocator_t<uint32_t> alloc(true);

	buffer<uint32_t, lp_flex_allocator_t<uint32_t>> buf(64u * 1024u, alloc);
	ASSERT_NE(buf, nullptr);
	buf.free();
}
