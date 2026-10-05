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

	// full range (default)
	{
		auto res = filter_for_range_str("", VALID_ISAS, DEF_MIN, DEF_MAX);
		ASSERT_TRUE(res.has_value());
		EXPECT_EQ(res->size(), VALID_ISAS.size());
	}

	// single value
	{
		auto res = filter_for_range_str("avx2", VALID_ISAS, DEF_MIN, DEF_MAX);
		ASSERT_TRUE(res.has_value());
		ASSERT_EQ(res->size(), 1);
		EXPECT_EQ((*res)[0], "avx2");
	}

	// proper subrange
	{
		auto res = filter_for_range_str("sse4.1-avx2", VALID_ISAS, DEF_MIN, DEF_MAX);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {
			"sse4.1", "sse4.2", "avx", "avx2"
		};
		EXPECT_EQ(*res, expected);
	}

	{
		auto res = filter_for_range_str("sse2-avx2:2", VALID_ISAS, DEF_MIN, DEF_MAX);
		EXPECT_FALSE(res.has_value());
	}

	{
		auto res = filter_for_range_str(":2", VALID_ISAS, DEF_MIN, DEF_MAX);
		EXPECT_FALSE(res.has_value());
	}

	// unknown token
	{
		auto res = filter_for_range_str("invalid-avx2", VALID_ISAS, DEF_MIN, DEF_MAX);
		EXPECT_FALSE(res.has_value());
	}

	// reversed range
	{
		auto res = filter_for_range_str("avx2-sse2", VALID_ISAS, DEF_MIN, DEF_MAX);
		EXPECT_FALSE(res.has_value());
	}

	// zero stride
	{
		auto res = filter_for_range_str("sse2-avx2:0", VALID_ISAS, DEF_MIN, DEF_MAX);
		EXPECT_FALSE(res.has_value());
	}

	{
		auto res = filter_for_range_str("sse2-avx2:99", VALID_ISAS, DEF_MIN, DEF_MAX);
		EXPECT_FALSE(res.has_value());
	}
}

TEST(HarnessParse, ParseRangeRejectsBadStride) {
	using namespace bench_utils::sweep_helpers;

	{
		auto parsed_range_opt = parse_range("1-22:0", 2ull, 3ull, 1ull);
		EXPECT_FALSE(parsed_range_opt.has_value());
	}

	{
		auto parsed_range_opt = parse_range("1-22:-2", 2ull, 3ull, 1ull);
		EXPECT_FALSE(parsed_range_opt.has_value());
	}
}

TEST(HarnessParse, FilterNumericRange) {
	using namespace bench_utils::sweep_helpers;

	static constexpr auto VALID_SIZES = std::to_array<uint64_t>({
		4ull,
		5ull,
		7ull,
		11ull
	});

	constexpr uint64_t DEF_MIN = 5ull;
	constexpr uint64_t DEF_MAX = 11ull;
	constexpr uint64_t DEF_STRIDE = 1ull;

	{
		auto res = filter_for_range_str("5-11:2", VALID_SIZES, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());

		std::vector<uint64_t> expected = {
			5ull, 7ull, 11ull
		};
		EXPECT_EQ(*res, expected);
	}

	{
		auto res = filter_for_range_str("5-11:99", VALID_SIZES, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());

		std::vector<uint64_t> expected = {
			5ull
		};
		EXPECT_EQ(*res, expected);
	}

	{
		auto res = filter_for_range_str("5-11:3", VALID_SIZES, DEF_MIN, DEF_MAX, DEF_STRIDE);
		ASSERT_TRUE(res.has_value());

		std::vector<uint64_t> expected = {
			5ull, 11ull
		};
		EXPECT_EQ(*res, expected);
	}

	{
		auto res = filter_for_range_str("4,7,11", VALID_SIZES, DEF_MIN, DEF_MAX, DEF_STRIDE);
		EXPECT_FALSE(res.has_value());
	}
}

TEST(HarnessParse, StrideMatchesExpandRange) {
	using namespace bench_utils::sweep_helpers;

	static constexpr auto VALID_SIZES = std::to_array<uint64_t>({
		4ull,
		5ull,
		7ull,
		11ull
	});

	for (auto spec : {"5-11:1", "5-11:2", "5-11:3", "5-11:4", "5-11:6", "5-11:99"}) {
		auto range = parse_range(spec, uint64_t{5}, uint64_t{11}, uint64_t{1});
		ASSERT_TRUE(range.has_value()) << spec;

		auto expected = expand_range(range.value());
		std::vector<uint64_t> want;
		for (auto v : expected) {
			if (std::find(VALID_SIZES.begin(), VALID_SIZES.end(), v) != VALID_SIZES.end()) {
				want.push_back(v);
			}
		}

		auto res = filter_for_range_str(spec, VALID_SIZES, uint64_t{5}, uint64_t{11}, uint64_t{1});
		ASSERT_TRUE(res.has_value()) << spec;
		EXPECT_EQ(*res, want) << spec;
	}
}

TEST(HarnessParse, ExpandRangeNoWrap) {
	using namespace bench_utils::sweep_helpers;

	{
		auto range = parse_range("5-11:18446744073709551615", uint64_t{5}, uint64_t{11}, uint64_t{1});
		ASSERT_TRUE(range.has_value());

		std::vector<uint64_t> expected = {
			5ull
		};
		EXPECT_EQ(expand_range(range.value()), expected);
	}

	{
		auto range = parse_range("5-11:3", uint64_t{5}, uint64_t{11}, uint64_t{1});
		ASSERT_TRUE(range.has_value());

		std::vector<uint64_t> expected = {
			5ull, 8ull, 11ull
		};
		EXPECT_EQ(expand_range(range.value()), expected);
	}

	{
		auto range = parse_range("3-6:2", uint64_t{3}, uint64_t{6}, uint64_t{1});
		ASSERT_TRUE(range.has_value());

		std::vector<uint64_t> expected = {
			3ull, 5ull
		};
		EXPECT_EQ(expand_range(range.value()), expected);
	}
}

TEST(HarnessParse, MakeNumericSweep) {
	using namespace bench_utils;
	using namespace bench_utils::sweep_helpers;

	struct dummy_context {};

	static constexpr auto CREATOR = [](const dummy_context&, uint64_t val) noexcept -> harness_run {
		return harness_run{
			std::to_string(val),
			1u,
			[]() noexcept -> std::optional<double> {
				return std::nullopt;
			},
			{}
		};
	};

	auto names = [](const std::optional<std::vector<harness_run>>& res) {
		std::vector<std::string> out;
		if (res) {
			for (const auto& run : res.value()) {
				out.push_back(run.name);
			}
		}
		return out;
	};

	constexpr uint64_t BEGIN = 3ull;
	constexpr uint64_t END = 8ull;

	{
		auto res = make_numeric_sweep("3-6", dummy_context{}, CREATOR, BEGIN, END);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {"3", "4", "5", "6"};
		EXPECT_EQ(names(res), expected);
	}

	{
		auto res = make_numeric_sweep("3-6:2", dummy_context{}, CREATOR, BEGIN, END);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {"3", "5"};
		EXPECT_EQ(names(res), expected);
	}

	{
		auto res = make_numeric_sweep("3,5,7", dummy_context{}, CREATOR, BEGIN, END);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {"3", "5", "7"};
		EXPECT_EQ(names(res), expected);
	}

	{
		auto res = make_numeric_sweep("auto", dummy_context{}, CREATOR, BEGIN, END);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {"3", "4", "5", "6", "7"};
		EXPECT_EQ(names(res), expected);
	}

	{
		auto res = make_numeric_sweep("3-8:18446744073709551615", dummy_context{}, CREATOR, BEGIN, END);
		ASSERT_TRUE(res.has_value());

		std::vector<std::string> expected = {"3"};
		EXPECT_EQ(names(res), expected);
	}
}

TEST(HarnessParse, MakeFilteredSweepList) {
	using namespace bench_utils;
	using namespace bench_utils::sweep_helpers;

	static constexpr auto VALID_SIZES = std::to_array<uint64_t>({
		4ull,
		5ull,
		7ull,
		11ull
	});

	struct dummy_context {};

	{
		auto res = make_filtered_sweep(
			"4,7,11",
			dummy_context{},
			VALID_SIZES,
			[](const dummy_context&, uint64_t val) noexcept -> harness_run {
				return harness_run{
					std::to_string(val),
					1u,
					[]() noexcept -> std::optional<double> {
						return std::nullopt;
					},
					{}
				};
			},
			static_cast<uint64_t>(5u),
			static_cast<uint64_t>(11u)
		);

		ASSERT_TRUE(res.has_value());
		ASSERT_EQ(res->size(), 3);

		std::vector<std::string> expected = {
			"4", "7", "11"
		};
		std::vector<std::string> actual;
		for (const auto& run : res.value()) {
			actual.push_back(run.name);
		}
		EXPECT_EQ(actual, expected);
	}
}
