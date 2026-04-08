#pragma once
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Weffc++"
#endif
#include <lyra/lyra.hpp>
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <iostream>
#include <thread>
#include <chrono>
#include <type_traits>

namespace bench_utils {
	struct harness_run {
		std::string name{};
		uint32_t est_runtime_s = 0ull;
		std::function<double()> run_bench;
		std::vector<std::optional<double>> sub_run_results{};
	};

	template <typename P>
	concept Param = requires(P p) {
		{ P::name } -> std::convertible_to<std::string>;
		{ P::gen_default() } -> std::same_as<std::optional<std::vector<harness_run>>>;
	};

	template <typename ... PARAMS>
	struct harness_t {
		static_assert((Param<PARAMS> && ...), "All PARAMS must satisfy Param concept");

		uint32_t num_sub_runs = 3u;
		std::vector<harness_run> runs{};

		std::string var = "none";
		std::string min_opt = "auto";
		std::string max_opt = "auto";
		std::string stride_opt = "auto";
		std::string list_opt = "auto";

		harness_t() noexcept {}

		void add_to_lyra(lyra::cli& cli) noexcept {
			cli.add_argument(
				lyra::opt(var, "var")
					["--var"]
					("Variable for the harness to test over")
					.choices({PARAMS::name...})
			);
			cli.add_argument(
				lyra::opt(num_sub_runs, "harness-sub-runs")
					["--harness-sub-runs"]
					("How many times to run the benchmark for each parameter sweep point")
					.choices({PARAMS::name...})
			);
		}

		void run() noexcept {
			std::optional<std::vector<harness_run>> runs_opt{};
			bool matched = ((var == PARAMS::name ? (runs_opt = PARAMS::gen_default(), true) : false) || ...);

			if (!matched) {
				std::cerr << "unknown var " << var << std::endl;
				return;
			}

			if (!runs_opt) {
				std::cerr << "couldn't generate runs for harness for " << var << std::endl;
				return;
			}

			runs = *runs_opt;

			auto total_time = 0u;
			auto total_runs = 0u;
			for (auto& run : runs) {
				total_time += (run.est_runtime_s + 2) * num_sub_runs;
				total_runs += num_sub_runs;
			}

			std::cout << "!!HARNESS!! estimated total time = " << total_time << "s" << std::endl;

			auto sub_run = 0u;
			for (auto& run : runs) {
				run.sub_run_results.resize(num_sub_runs);
				for (auto i = 0u; i < num_sub_runs; i++) {
					std::cout << std::endl;
					std::cout << "!!HARNESS!! run " << (sub_run + 1) << "/" << total_runs << std::endl;
					std::cout << std::endl;
					run.sub_run_results[i] = run.run_bench();
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(2s);
					sub_run++;
				}
			}

			std::cout << std::endl;
			std::cout << "!!HARNESS!!" << std::endl;
			std::cout << "=====" << var << " results=====" << std::endl;

			constexpr auto name_fmt_str = " {:<8} |";
			constexpr auto res_col_fmt_str = " {:>8}";
			constexpr auto avg_min_max_fmt_str = " | {:>8} | {:>8}";

			std::cout << std::format(name_fmt_str, var);
			for (auto i = 0u; i < num_sub_runs; i++) {
				std::cout << std::format(res_col_fmt_str, std::format("run {}", i + 1));
			}
			std::cout << std::format(avg_min_max_fmt_str, "avg", "max/min") << std::endl;
			std::cout << std::string(12 + 9 * num_sub_runs + 22, '=') << std::endl;
			for (const auto& run : runs) {
				double max = 0.0;
				double min = 1e20;
				auto total = 0.0;
				auto completed = 0u;
				for (auto val : run.sub_run_results) {
					if (val) {
						total += *val;
						max = std::max(max, *val);
						min = std::min(min, *val);
						completed++;
					}
				}
				auto ratio_p_str = ([&]() -> std::string {
					if (max >= 1e-10) { // implies max and min found
						return std::format("{:.2f}%", (max / min - 1.0) * 100.0);
					} else {
						return "---";
					}
				})();
				auto mean_str = ([&]() -> std::string {
					if (total >= 1e-10) {
						return std::format("{:.2f}", total / completed);
					} else {
						return "---";
					}
				})();
				auto val_str_or_dash = [](const std::optional<double>& val) noexcept -> std::string {
					if (val) {
						return std::format("{:.2f}", *val);
					} else {
						return "---";
					}
				};
				std::cout << std::format(name_fmt_str, run.name);
				for (auto i = 0u; i < num_sub_runs; i++) {
					std::cout << std::format(res_col_fmt_str, val_str_or_dash(run.sub_run_results[i]));
				}
				std::cout << std::format(avg_min_max_fmt_str, mean_str, ratio_p_str) << std::endl;
			}
		}
	};
} // namespace bench_utils
