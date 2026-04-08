#pragma once
#include <bench_utils/parse.h>
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

	template <typename HV>
	concept HarnessVariable = requires(HV hv, std::string_view opt_str) {
		{ HV::name } -> std::convertible_to<std::string>;
		{ HV::gen(opt_str) } -> std::same_as<std::optional<std::vector<harness_run>>>;
	};

	template <typename ... HARNESS_VARIABLES>
	struct harness_t {
		static_assert((HarnessVariable<HARNESS_VARIABLES> && ...), "All HARNESS_VARIABLES must satisfy HarnessVariable concept");

		uint32_t wait_time_ms = 2000u;
		uint32_t num_sub_runs = 3u;
		std::vector<harness_run> runs{};

		std::string var = "none";
		std::string opt_str = "";

		harness_t(uint32_t wt_ms, uint32_t def_runs) noexcept : wait_time_ms(wt_ms), num_sub_runs(def_runs) {}

		void add_to_lyra(lyra::cli& cli) noexcept {
			cli.add_argument(
				lyra::opt(var, "var")
					["--harness-var"]
					("Variable for the harness to test over")
					.choices({HARNESS_VARIABLES::name...})
			);
			cli.add_argument(
				lyra::opt(num_sub_runs, "harness-sub-runs")
					["--harness-sub-runs"]
					("How many times to run the benchmark for each parameter sweep point")
			);
			cli.add_argument(
				lyra::opt(opt_str, "harness-option-string")
					["--harness-opt"]
					("String parsed by selected variable to specify scope if supported")
			);
		}

		void run() noexcept {
			std::optional<std::vector<harness_run>> runs_opt{};
			bool matched = ((var == HARNESS_VARIABLES::name ? (runs_opt = HARNESS_VARIABLES::gen(opt_str), true) : false) || ...);

			if (!matched) {
				std::cerr << "unknown var " << var << std::endl;
				return;
			}

			if (!runs_opt) {
				std::cerr << "couldn't generate runs for harness for " << var << "\t" << opt_str << std::endl;
				return;
			}

			runs = runs_opt.value();

			auto total_time = runs.size() * wait_time_ms / 1000u;
			auto total_runs = 0u;
			for (auto& run : runs) {
				total_time += run.est_runtime_s * num_sub_runs;
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
					std::this_thread::sleep_for(1ms * wait_time_ms);
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
						total += val.value();
						max = std::max(max, val.value());
						min = std::min(min, val.value());
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
						return std::format("{:.2f}", val.value());
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

	namespace sweep_helpers {
		struct min_max_stride_str_parsed {
			std::string_view min{};
			std::string_view max{};
			std::string_view stride{};
		};

		inline min_max_stride_str_parsed parse_min_max_stride_str(std::string_view str) noexcept {
			min_max_stride_str_parsed output;

			auto colon_pos = str.find(':');
			std::string_view range_part = str.substr(0, colon_pos);
			std::string_view stride_part = (colon_pos == std::string_view::npos) ? std::string_view{} : str.substr(colon_pos + 1);

			if (!stride_part.empty()) {
				output.stride = stride_part;
			}

			auto dash_pos = range_part.find('-');
			if (dash_pos == std::string_view::npos) {
				if (!range_part.empty()) {
					output.min = range_part;
					output.max = range_part;
				}
			} else {
				std::string_view min_str = range_part.substr(0, dash_pos);
				std::string_view max_str = range_part.substr(dash_pos + 1);

				if (!min_str.empty()) {
					output.min = min_str;
				}
				if (!max_str.empty()) {
					output.max = max_str;
				}
			}

			return output;
		}

		template <std::integral T>
		struct parsed_range_int {
			T min = 0ull;
			T max = 0ull;
			T stride = 0ull;
		};

		template <std::integral T>
		inline std::optional<parsed_range_int<T>> parse_range(std::string_view str, T default_min, T default_max, T default_stride) noexcept {
			if (str.empty() || str == "auto") {
				return parsed_range_int{default_min, default_max, default_stride};
			}

			T min = default_min;
			T max = default_max;
			T stride = default_stride;

			auto range_str_parsed = parse_min_max_stride_str(str);

			if (!range_str_parsed.min.empty()) {
				auto try_min = parse_int<T>(range_str_parsed.min);
				if (try_min) {
					min = try_min.value();
				} else {
					return std::nullopt;
				}
			}

			if (!range_str_parsed.max.empty()) {
				auto try_max = parse_int<T>(range_str_parsed.max);
				if (try_max) {
					max = try_max.value();
				} else {
					return std::nullopt;
				}
			}

			if (!range_str_parsed.stride.empty()) {
				auto try_stride = parse_int<T>(range_str_parsed.stride);
				if (try_stride) {
					stride = try_stride.value();
				} else {
					return std::nullopt;
				}
			}

			if (min > max) {
				return std::nullopt;
			}

			return parsed_range_int{min, max, stride};
		}

		template <std::integral T>
		inline std::optional<std::vector<T>> filter_for_range_str(std::string_view str, std::vector<T> values, T default_min, T default_max, T default_stride) noexcept {
			auto parsed_range_opt = parse_range(str, default_min, default_max, default_stride);
			if (!parsed_range_opt) {
				return std::nullopt;
			}

			auto& parsed_range = parsed_range_opt.value();

			auto start = std::find(values.begin(), values.end(), parsed_range.min);
			auto end = std::find(values.begin(), values.end(), parsed_range.max);

			auto stride = parsed_range.stride();
			if (start == values.end() || end == values.end() || start > end) {
				return std::nullopt;
			}

			std::vector<T> result;

			for (auto it = start; it <= end; std::advance(it, stride)) {
				result.push_back(*it);

				if (std::distance(it, end) < stride) {
					break;
				}
			}

			return result;
		}

		inline std::optional<std::vector<std::string_view>> filter_for_range_str(std::string_view str, std::vector<std::string_view> values, std::string_view default_min, std::string_view default_max, uint32_t default_stride) noexcept {
			auto tokens = parse_min_max_stride_str(str);

			auto start = std::find(values.begin(), values.end(), tokens.min.empty() ? default_min : tokens.min);
			auto end = std::find(values.begin(), values.end(), tokens.max.empty() ? default_max : tokens.max);

			if (start == values.end() || end == values.end() || start > end) {
				return std::nullopt;
			}

			std::vector<std::string_view> result;
			auto stride = default_stride;
			if (!tokens.stride.empty()) {
				auto stride_opt = parse_int<uint32_t>(tokens.stride);
				if (!stride_opt) {
					return std::nullopt;
				}
				stride = stride_opt.value();
			}

			for (auto it = start; it <= end; std::advance(it, stride)) {
				result.push_back(*it);

				if (std::distance(it, end) < stride) {
					break;
				}
			}

			return result;
		}

		template <std::integral T>
		inline std::vector<T> expand_range(const parsed_range_int<T>& range) noexcept {
			std::vector<T> out;
			for (auto val = range.min; val <= range.max; val += range.stride) {
				out.push_back(val);
			}
			return out;
		}
	} // namespace bench_utils::sweep_helpers
} // namespace bench_utils
