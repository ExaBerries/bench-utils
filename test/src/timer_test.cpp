#include <gtest/gtest.h>
#include <bench_utils/timer.h>

#include <chrono>
#include <limits>
#include <thread>
#include <type_traits>

static_assert(std::is_trivially_copyable_v<bench_utils::timer>);
static_assert(std::is_trivially_destructible_v<bench_utils::timer>);

TEST(Timer, fast_timer_duration) {
	using namespace bench_utils;

	const timer t = create_fast_timer();
	ASSERT_GT(t.report_frequency, 0);
	EXPECT_GE(t.underlying_frequency, 0);

	const int64_t start = fast_now();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	const int64_t end = fast_now();

	const int64_t elapsed_ms = duration(t, start, end);
	const int64_t elapsed_us = duration<std::chrono::microseconds>(t, start, end);

	EXPECT_GE(elapsed_ms, 40);
	EXPECT_LE(elapsed_ms, 2000);
	EXPECT_GE(elapsed_us, 40000);
	EXPECT_LE(elapsed_us, 2000000);
}

TEST(Timer, coarse_timer_duration) {
	using namespace bench_utils;

	const timer t = create_coarse_timer();
	ASSERT_EQ(t.report_frequency, 1000);
	EXPECT_GE(t.underlying_frequency, 0);
	#if defined(__linux__)
		EXPECT_GT(t.underlying_frequency, 0);
	#endif

	const int64_t start = coarse_now();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	const int64_t end = coarse_now();

	const int64_t elapsed_ms = duration(t, start, end);
	const int64_t elapsed_us = duration<std::chrono::microseconds>(t, start, end);

	EXPECT_GE(elapsed_ms, 40);
	EXPECT_LE(elapsed_ms, 2000);
	EXPECT_EQ(elapsed_us, elapsed_ms * 1000);
}

TEST(Timer, coarse_agrees_with_fast) {
	using namespace bench_utils;

	const timer fast = create_fast_timer();
	const timer coarse = create_coarse_timer();

	const int64_t fast_start = fast_now();
	const int64_t coarse_start = coarse_now();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	const int64_t coarse_end = coarse_now();
	const int64_t fast_end = fast_now();

	const int64_t fast_ms = duration(fast, fast_start, fast_end);
	const int64_t coarse_ms = duration(coarse, coarse_start, coarse_end);

	EXPECT_GE(fast_ms, 80);
	EXPECT_GE(coarse_ms, 80);
	EXPECT_NEAR(static_cast<double>(coarse_ms), static_cast<double>(fast_ms), 50.0);
}

TEST(Timer, overflow_check) {
	using namespace bench_utils;

	const timer fast = create_fast_timer();
	const timer coarse = create_coarse_timer();

	EXPECT_FALSE(will_overflow_within(fast, fast_now(), 0));
	EXPECT_FALSE(will_overflow_within(coarse, coarse_now(), 0));
	EXPECT_FALSE(will_overflow_within(fast, fast_now(), 3600));
	EXPECT_FALSE(will_overflow_within(coarse, coarse_now(), 3600));
	EXPECT_TRUE(will_overflow_within(fast, fast_now(), std::numeric_limits<int64_t>::max()));
	EXPECT_TRUE(will_overflow_within(coarse, coarse_now(), std::numeric_limits<int64_t>::max()));
}

TEST(Timer, aggregate_construction) {
	using namespace bench_utils;

	const timer t{.report_frequency = 1'000'000'000ll, .underlying_frequency = 0ll, .source = timer_source::INVARIANT_TSC};
	ASSERT_EQ(t.report_frequency, 1'000'000'000ll);
	EXPECT_EQ(t.source, timer_source::INVARIANT_TSC);
	EXPECT_EQ(duration(t, 1'000'000'000ll, 3'500'000'000ll), 2500);
	EXPECT_EQ(duration<std::chrono::microseconds>(t, 1'000'000'000ll, 3'500'000'000ll), 2'500'000);

	const timer empty{};
	EXPECT_EQ(empty.report_frequency, 0ll);
	EXPECT_EQ(duration(empty, 0ll, 1000ll), 0ll);
	EXPECT_EQ(empty.source, timer_source::UNKNOWN);
}
