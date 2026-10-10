#include <gtest/gtest.h>
#include <bench_box/parse.h>

TEST(Parse, sv_uint64) {
	using namespace bench_box;

	EXPECT_EQ(*parse_int<uint64_t>("123"), 123ull);
	EXPECT_EQ(*parse_int<uint64_t>("70"), 70ull);
	EXPECT_FALSE(parse_int<uint64_t>("-70"));
	EXPECT_FALSE(parse_int<uint64_t>("a0"));
	EXPECT_FALSE(parse_int<uint64_t>("abx"));;

	EXPECT_FALSE(parse_int<uint64_t>("64,128,192"));
	EXPECT_FALSE(parse_int<uint64_t>("12 "));
	EXPECT_FALSE(parse_int<uint64_t>("0x10"));
}

TEST(Parse, sv_uint32) {
	using namespace bench_box;

	EXPECT_EQ(*parse_int<uint32_t>("123"), 123u);
	EXPECT_EQ(*parse_int<uint32_t>("70"), 70u);
	EXPECT_FALSE(parse_int<uint32_t>("-70"));
	EXPECT_FALSE(parse_int<uint32_t>("abx"));
}
