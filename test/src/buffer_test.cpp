#include <gtest/gtest.h>
#include <bench_utils/memory/memory.h>
#include <bench_utils/memory/buffer.h>

TEST(Buffer, DefaultAlloc) {
	using namespace bench_utils;

	buffer<int> buf(64);
	buf.free();
}

TEST(Buffer, NonLP) {
	using namespace bench_utils;

	lp_flex_allocator_t<int> alloc(false);

	buffer<int, lp_flex_allocator_t<int>> buf(64u * 1024u, alloc);
	ASSERT_NE(buf, nullptr);
	buf.free();
}

TEST(Buffer, LP) {
	using namespace bench_utils;

	lp_flex_allocator_t<int> alloc(true);

	buffer<int, lp_flex_allocator_t<int>> buf(64u * 1024u, alloc);
	ASSERT_NE(buf, nullptr);
	buf.free();
}
