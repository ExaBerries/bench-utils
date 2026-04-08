#include <gtest/gtest.h>
#include <bench_utils/parse.h>

TEST(Parse, sv_uint64) {
	using namespace bench_utils;

	EXPECT_EQ(*parse_int<uint64_t>("123"), 123ull);
	EXPECT_EQ(*parse_int<uint64_t>("70"), 70ull);
	EXPECT_FALSE(parse_int<uint64_t>("-70"));
	EXPECT_FALSE(parse_int<uint64_t>("a0"));
	EXPECT_FALSE(parse_int<uint64_t>("abx"));;
}

TEST(Parse, sv_int32) {
	using namespace bench_utils;

	EXPECT_EQ(parse_int<int32_t>("123"), 123);
	EXPECT_EQ(parse_int<int32_t>("70"), 70);
	EXPECT_EQ(parse_int<int32_t>("-70"), -70);
	EXPECT_FALSE(parse_int<int32_t>("abx"));
}
