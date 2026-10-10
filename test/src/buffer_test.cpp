#include <gtest/gtest.h>
#include <bench_box/memory/memory.h>
#include <bench_box/memory/buffer.h>

TEST(Buffer, DefaultAlloc) {
	using namespace bench_box;

	buffer<uint32_t> buf(64);
	buf.free();
}

TEST(Buffer, NonLP) {
	using namespace bench_box;

	lp_flex_allocator_t<uint32_t> alloc(false);

	buffer<uint32_t, lp_flex_allocator_t<uint32_t>> buf(64u * 1024u, alloc);
	ASSERT_NE(buf, nullptr);
	buf.free();
}

TEST(Buffer, DISABLED_LP) { // fails if it can't allocate a large page, may not be an error depending on user privileges
	using namespace bench_box;

	lp_flex_allocator_t<uint32_t> alloc(true);

	buffer<uint32_t, lp_flex_allocator_t<uint32_t>> buf(64u * 1024u, alloc);
	ASSERT_NE(buf, nullptr);
	buf.free();
}
