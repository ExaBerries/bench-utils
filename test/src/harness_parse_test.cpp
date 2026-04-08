#include <gtest/gtest.h>
#include <bench_utils/harness/harness.h>

TEST(HarnessParse, TokenizeRange) {
	using namespace bench_utils::sweep_helpers;

	{
		auto parsed_tokens = parse_min_max_stride_str("1-22:3");
		EXPECT_EQ(parsed_tokens.min, "1");
		EXPECT_EQ(parsed_tokens.max, "22");
		EXPECT_EQ(parsed_tokens.stride, "3");
	}

	{
		auto parsed_tokens = parse_min_max_stride_str("-22:3");
		EXPECT_EQ(parsed_tokens.min, "");
		EXPECT_EQ(parsed_tokens.max, "22");
		EXPECT_EQ(parsed_tokens.stride, "3");
	}

	{
		auto parsed_tokens = parse_min_max_stride_str("1-:3");
		EXPECT_EQ(parsed_tokens.min, "1");
		EXPECT_EQ(parsed_tokens.max, "");
		EXPECT_EQ(parsed_tokens.stride, "3");
	}

	{
		auto parsed_tokens = parse_min_max_stride_str(":3");
		EXPECT_EQ(parsed_tokens.min, "");
		EXPECT_EQ(parsed_tokens.max, "");
		EXPECT_EQ(parsed_tokens.stride, "3");
	}

	{
		auto parsed_tokens = parse_min_max_stride_str("1-22");
		EXPECT_EQ(parsed_tokens.min, "1");
		EXPECT_EQ(parsed_tokens.max, "22");
		EXPECT_EQ(parsed_tokens.stride, "");
	}

	{
		auto parsed_tokens = parse_min_max_stride_str("abc");
		EXPECT_EQ(parsed_tokens.min, "abc");
		EXPECT_EQ(parsed_tokens.max, "abc");
		EXPECT_TRUE(parsed_tokens.stride.empty());
	}
}

TEST(HarnessParse, ParseRange) {
	using namespace bench_utils::sweep_helpers;

	{
		auto parsed_range_opt = parse_range("1-22:3", 2ull, 3ull, 1ull);
		ASSERT_TRUE(parsed_range_opt);
		auto& parsed_range = parsed_range_opt.value();
		EXPECT_EQ(parsed_range.min, 1ull);
		EXPECT_EQ(parsed_range.max, 22ull);
		EXPECT_EQ(parsed_range.stride, 3ull);
	}

	{
		auto parsed_range_opt = parse_range("-22:3", 2ull, 3ull, 1ull);
		ASSERT_TRUE(parsed_range_opt);
		auto& parsed_range = parsed_range_opt.value();
		EXPECT_EQ(parsed_range.min, 2ull);
		EXPECT_EQ(parsed_range.max, 22ull);
		EXPECT_EQ(parsed_range.stride, 3ull);
	}

	{
		auto parsed_range_opt = parse_range("1-:3", 2ull, 3ull, 1ull);
		ASSERT_TRUE(parsed_range_opt);
		auto& parsed_range = parsed_range_opt.value();
		EXPECT_EQ(parsed_range.min, 1ull);
		EXPECT_EQ(parsed_range.max, 3ull);
		EXPECT_EQ(parsed_range.stride, 3ull);
	}

	{
		auto parsed_range_opt = parse_range(":3", 2ull, 3ull, 1ull);
		ASSERT_TRUE(parsed_range_opt);
		auto& parsed_range = parsed_range_opt.value();
		EXPECT_EQ(parsed_range.min, 2ull);
		EXPECT_EQ(parsed_range.max, 3ull);
		EXPECT_EQ(parsed_range.stride, 3ull);
	}
}
