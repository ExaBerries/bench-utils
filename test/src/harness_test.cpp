#include <gtest/gtest.h>
#include <bench_utils/harness/harness.h>

namespace bench_utils {
	struct cmdline_config {
		uint64_t size = 10u;
		double score = 10;
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
		static constexpr std::string name = "none";

		static std::optional<std::vector<harness_run>> gen([[maybe_unused]] std::string_view opt_str) noexcept {
			std::vector<harness_run> output;
			output.emplace_back(harness_run{
				name,
				1u,
				[&]() -> double {
					cmdline_config config;
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
		static constexpr std::string name = "size";

		static std::optional<std::vector<harness_run>> gen(std::string_view opt_str) noexcept {
			std::vector<harness_run> output;
			if (!opt_str.empty() && opt_str != "auto") {
				auto try_parse_list = parse_list(opt_str);
				if (try_parse_list) {
					auto& list = try_parse_list.value();
					for (auto sv : list) {
						auto try_parse = parse_int<uint64_t>(sv);
						if (!try_parse) {
							output.clear();
							break;
						}
						auto val = try_parse.value();
						output.emplace_back(harness_run{
							std::to_string(try_parse.value()),
							1u,
							[val]() -> double {
								cmdline_config config;
								config.size = val;
								config.score = 25.0;
								return mock_run_bench(config);
							},
							{}
						});
					}
					if (!output.empty()) {
						return output;
					}
				}
				auto try_parse_range = sweep_helpers::parse_range(opt_str, 3u, 8u, 4u);
				if (try_parse_range) {
					auto expanded = sweep_helpers::expand_range(try_parse_range.value());
					for (auto size : expanded) {
						output.emplace_back(harness_run{
							std::to_string(size),
							1u,
							[size]() -> double {
								cmdline_config config;
								config.size = size;
								config.score = 25.0;
								return mock_run_bench(config);
							},
							{}
						});
					}
					if (!output.empty()) {
						return output;
					}
				}
			}
			for (auto i = 3u; i < 8u; i++) {
				output.emplace_back(harness_run{
					std::to_string(i),
					1u,
					[i]() -> double {
						cmdline_config config;
						config.size = i;
						config.score = 25.0 * i;
						return mock_run_bench(config);
					},
					{}
				});
			}
			return output;
		}
	};

	struct fake_isa_var {
		static constexpr std::string name = "isa";
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

		static std::optional<std::vector<harness_run>> gen(std::string_view opt_str) noexcept {
			std::vector<harness_run> output;
			if (!opt_str.empty() && opt_str != "auto") {
				auto try_parse_range = sweep_helpers::filter_for_range_str(opt_str, VALID_ISAS, "sse2", "avx2", 1u);
				if (try_parse_range) {
					auto& values = try_parse_range.value();
					for (const auto& val : values) {
						std::cout << val << std::endl;
						output.emplace_back(harness_run{
							std::string(val),
							1u,
							[]() -> double {
								cmdline_config config;
								config.size = 2u;
								config.score = 25.0;
								return mock_run_bench(config);
							},
							{}
						});
					}
					if (!output.empty()) {
						return output;
					}
				}
				auto try_parse_list = parse_list(opt_str);
				if (try_parse_list) {
					auto& list = try_parse_list.value();
					for (const auto& val : list) {
						output.emplace_back(harness_run{
							std::string(val),
							1u,
							[]() -> double {
								cmdline_config config;
								config.size = 2u;
								config.score = 25.0;
								return mock_run_bench(config);
							},
							{}
						});
					}
					if (!output.empty()) {
						return output;
					}
				}
			}
			for (auto val : VALID_ISAS) {
				output.emplace_back(harness_run{
					std::string(val),
					1u,
					[]() -> double {
						cmdline_config config;
						config.size = 2u;
						config.score = 25.0;
						return mock_run_bench(config);
					},
					{}
				});
			}
			return output;
		}
	};
} // namespace bench_utils

TEST(Harness, NoneSweep) {
	using namespace bench_utils;

	std::string test = "";

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"none"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(test, "test")
			["--test"]
			("test");

	harness_t<none_var> harness{1u, 3u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run();
}

TEST(Harness, SizeSweepDefault) {
	using namespace bench_utils;

	std::string test = "";

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"size"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(test, "test")
			["--test"]
			("test");

	harness_t<none_var, size_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run();
}

TEST(Harness, IsaSweepDefault) {
	using namespace bench_utils;

	std::string test = "";

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--harness-var",
		"isa"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(test, "test")
			["--test"]
			("test");

	harness_t<none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run();
}

TEST(Harness, IsaSweepRangeDefault) {
	using namespace bench_utils;

	std::string test = "";

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
		| lyra::opt(test, "test")
			["--test"]
			("test");

	harness_t<none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run();
}

TEST(Harness, IsaSweepRange) {
	using namespace bench_utils;

	std::string test = "";

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
		| lyra::opt(test, "test")
			["--test"]
			("test");

	harness_t<none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run();
}

TEST(Harness, IsaList) {
	using namespace bench_utils;

	std::string test = "";

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
		| lyra::opt(test, "test")
			["--test"]
			("test");

	harness_t<none_var, fake_isa_var> harness{1u, 2u};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run();
}
