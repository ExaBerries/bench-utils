#include <gtest/gtest.h>
#include <bench_utils/harness/harness.h>

namespace bench_utils {
	struct cmdline_config {
		uint32_t size = 10u;
		double score = 10;
	};

	static double mock_run_bench(const cmdline_config& config) noexcept {
		using namespace std::chrono_literals;
		for (auto i = 0u; i < config.size; i++) {
			std::this_thread::sleep_for(100ms);
		}
		std::cout << "score!! " << config.score << std::endl;
		return config.score;
	}

	struct none_param {
		using supports_min_max_stride = std::false_type;
		using supports_list = std::false_type;
		static constexpr std::string name = "none";

		static std::optional<std::vector<harness_run>> gen_default() noexcept {
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
} // namespace bench_utils

TEST(Harness, NoneSweep) {
	using namespace bench_utils;

	std::string test = "";

	constexpr auto argv = std::array{
		"mybin",
		"--test",
		"test",
		"--var",
		"none"
	};
	constexpr auto argc = argv.size();

	auto cli = lyra::cli()
		| lyra::opt(test, "test")
			["--test"]
			("test");

	harness_t<none_param> harness{};

	harness.add_to_lyra(cli);

	auto result = cli.parse({argc, argv.data()});
	if (!result) {
		std::cerr << result.message() << std::endl;
	}
	EXPECT_TRUE(result);

	harness.run();
}
