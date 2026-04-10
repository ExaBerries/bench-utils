#include <gtest/gtest.h>
#include <bench_utils/harness/harness_helpers.h>

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

TEST(HarnessParse, FilterRange) {
	using namespace bench_utils::sweep_helpers;

	static constexpr auto VALID_ISAS = std::to_array<std::string_view>({
		"sse2",
		"sse4.1",
		"sse4.2",
		"avx",
		"avx2",
		"fma",
		"avx512vl",
		"avx512f"
	});

	constexpr std::string_view DEF_MIN = "sse2";
	constexpr std::string_view DEF_MAX = "avx512f";
	constexpr uint32_t DEF_STRIDE = 1;

	// full range (default)
	{
		auto res = filter_for_range_str("", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());
		EXPECT_EQ(res->size(), VALID_ISAS.size());
	}

	// single value
	{
		auto res = filter_for_range_str("avx2", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());
		ASSERT_EQ(res->size(), 1);
		EXPECT_EQ((*res)[0], "avx2");
	}

	// proper subrange
	{
		auto res = filter_for_range_str("sse4.1-avx2", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {
			"sse4.1", "sse4.2", "avx", "avx2"
		};
		EXPECT_EQ(*res, expected);
	}

	// range with stride
	{
		auto res = filter_for_range_str("sse2-avx2:2", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {
			"sse2", "sse4.2", "avx2"
		};
		EXPECT_EQ(*res, expected);
	}

	// default min/max with stride only
	{
		auto res = filter_for_range_str(":2", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {
			"sse2", "sse4.2", "avx2", "avx512vl"
		};
		EXPECT_EQ(*res, expected);
	}

	// unknown token
	{
		auto res = filter_for_range_str("invalid-avx2", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		EXPECT_FALSE(res.has_value());
	}

	// reversed range
	{
		auto res = filter_for_range_str("avx2-sse2", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		EXPECT_FALSE(res.has_value());
	}

	// zero stride
	{
		auto res = filter_for_range_str("sse2-avx2:0", VALID_ISAS, DEF_MIN, DEF_MAX, DEF_STRIDE);
		EXPECT_FALSE(res.has_value());
	}
}
