#include <gtest/gtest.h>
#include <bench_utils/harness/harness.h>
#include <bench_utils/harness/harness_helpers.h>

namespace bench_utils {
	struct cmdline_config {
		std::string test = "";
		uint64_t size = 10u;
		double score = 10;
	};

	struct harness_context {
		cmdline_config& config_ref;
	};

	static double mock_run_bench(const cmdline_config& config) noexcept {
		using namespace std::chrono_literals;
		std::cout << "run!! " << config.size << std::endl;
		for (auto i = 0u; i < config.size; i++) {
			std::this_thread::sleep_for(100ms);
		}
		std::cout << "score!! " << config.score << std::endl;
		return config.score;
	}

	struct none_var {
		[[nodiscard]] static std::string name() noexcept {
			return "none";
		}

		[[nodiscard]] static std::optional<std::vector<harness_run>> gen([[maybe_unused]] std::string_view opt_str, harness_context& context) noexcept {
			std::vector<harness_run> output;
			output.emplace_back(harness_run{
				name(),
				1u,
				[context]() -> std::optional<double> {
					cmdline_config config = context.config_ref;
					config.size = 5u;
					config.score = 25.0;
					return mock_run_bench(config);
				},
				{}
			});
			return output;
		}
	};

	struct size_var {
		static constexpr auto VALID_SIZES = std::to_array<uint64_t>({
			4ull,
			5ull,
			7ull,
			11ull
		});

		[[nodiscard]] static std::string name() noexcept {
			return "size";
		}

		static std::optional<std::vector<harness_run>> gen(std::string_view opt_str, harness_context& context) noexcept {
			return sweep_helpers::make_filtered_sweep(
				opt_str,
				context,
				VALID_SIZES,
				[](const harness_context& ctx, uint64_t val) noexcept -> harness_run {
					return harness_run{
						std::to_string(val),
						1ull,
						[ctx, val]() noexcept -> std::optional<double> {
							auto config = ctx.config_ref;
							config.score = 25.0 * static_cast<double>(val);
							return mock_run_bench(config);
						},
						{}
					};
				},
				5ul,
				11ul
			);
		}
	};

	struct threads_var {
		[[nodiscard]] static std::string name() noexcept {
			return "threads";
		}

		static std::optional<std::vector<harness_run>> gen(std::string_view opt_str, harness_context& context) noexcept {
			return sweep_helpers::make_numeric_sweep(
				opt_str,
				context,
				[](const harness_context& ctxt, uint64_t val) -> harness_run {
					return harness_run{
						std::to_string(val),
						1u,
						[val, ctxt]() noexcept -> std::optional<double> {
							auto config = ctxt.config_ref;
							config.size = val;
							config.score = 25.0 * static_cast<double>(val);
							return mock_run_bench(config);
						},
						{}
					};
				},
				3ull,
				8ull
			);
		}
	};

	struct fake_isa_var {
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

		[[nodiscard]] static std::string name() noexcept {
			return "isa";
		}

		static std::optional<std::vector<harness_run>> gen(std::string_view opt_str, harness_context& context) noexcept {
			return sweep_helpers::make_filtered_sweep(
				opt_str,
				context,
				VALID_ISAS,
				[](const harness_context& ctx, const std::string& isa) noexcept -> harness_run {
					return harness_run{
						isa,
						1u,
						[ctx, isa]() noexcept -> std::optional<double> {
							auto config = ctx.config_ref;
							config.score = 25.0;
							return mock_run_bench(config);
						},
						{}
					};
				},
				"sse2",
				"avx512f"
			);
		}
	};
} // namespace bench_utils

TEST(Harness, NoneSweep) {
	using namespace bench_utils;

	cmdline_config config;

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"none"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(config.test, "test")
			["--test"]
			("test");

	harness_context h_context{std::ref(config)};

	harness_t<harness_context, none_var> harness{1u, 3u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run(h_context);
}

TEST(Harness, SizeSweepDefault) {
	using namespace bench_utils;

	cmdline_config config;

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"threads"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(config.test, "test")
			["--test"]
			("test");

	harness_context h_context{std::ref(config)};

	harness_t<harness_context, none_var, threads_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run(h_context);
}

TEST(Harness, IsaSweepDefault) {
	using namespace bench_utils;

	cmdline_config config;

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"isa"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(config.test, "test")
			["--test"]
			("test");

	harness_context h_context{std::ref(config)};

	harness_t<harness_context, none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run(h_context);
}

TEST(Harness, IsaSweepRangeDefault) {
	using namespace bench_utils;

	cmdline_config config;

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"isa",
		"--harness-opt",
		"-"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(config.test, "test")
			["--test"]
			("test");

	harness_context h_context{std::ref(config)};

	harness_t<harness_context, none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run(h_context);
}

TEST(Harness, IsaSweepRange) {
	using namespace bench_utils;

	cmdline_config config;

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"isa",
		"--harness-opt",
		"avx-avx512f"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(config.test, "test")
			["--test"]
			("test");

	harness_context h_context{std::ref(config)};

	harness_t<harness_context, none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run(h_context);
}

TEST(Harness, IsaList) {
	using namespace bench_utils;

	cmdline_config config;

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"isa",
		"--harness-opt",
		"avx,sse2,avx512vl"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(config.test, "test")
			["--test"]
			("test");

	harness_context h_context{std::ref(config)};

	harness_t<harness_context, none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run(h_context);
}
