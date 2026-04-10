#pragma once
#include <bench_utils/parse.h>
#include <bench_utils/harness/harness.h>
#include <string_view>
#include <vector>
#include <array>
#include <concepts>

namespace bench_utils {
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

		template <std::integral T, std::size_t AS>
		inline std::optional<std::vector<T>> filter_for_range_str(std::string_view str, const std::array<T, AS>& values, T default_min, T default_max, T default_stride) noexcept {
			auto parsed_range_opt = parse_range(str, default_min, default_max, default_stride);
			if (!parsed_range_opt) {
				return std::nullopt;
			}

			auto& parsed_range = parsed_range_opt.value();

			auto start = std::find(values.begin(), values.end(), parsed_range.min);
			auto end = std::find(values.begin(), values.end(), parsed_range.max);

			auto stride = parsed_range.stride;
			if (start == values.end() || end == values.end() || start > end) {
				return std::nullopt;
			}

			if (stride <= 0) {
				return std::nullopt;
			}

			std::vector<T> result;

			for (auto it = start; it <= end; it += stride) {
				result.push_back(*it);

				if (it + stride > end) {
					break;
				}
			}

			return result;
		}

		template <std::size_t AS>
		inline std::optional<std::vector<std::string>> filter_for_range_str(std::string_view str, const std::array<std::string_view, AS>& values, std::string_view default_min, std::string_view default_max, uint32_t default_stride) noexcept {
			auto tokens = parse_min_max_stride_str(str);

			auto start = std::find(values.begin(), values.end(), tokens.min.empty() ? default_min : tokens.min);
			auto end = std::find(values.begin(), values.end(), tokens.max.empty() ? default_max : tokens.max);

			if (start == values.end() || end == values.end() || start > end) {
				return std::nullopt;
			}

			std::vector<std::string> result;
			auto stride = default_stride;
			if (!tokens.stride.empty()) {
				auto stride_opt = parse_int<uint32_t>(tokens.stride);
				if (!stride_opt) {
					return std::nullopt;
				}
				stride = stride_opt.value();
			}

			if (stride == 0) {
				return std::nullopt;
			}

			for (auto it = start; it <= end; it += stride) {
				result.push_back(std::string(*it));

				if (it + stride > end) {
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

		template <typename CONTEXT, std::integral T, typename CREATE_RUN>
		std::optional<std::vector<harness_run>> make_numeric_sweep(std::string_view opt_str, const CONTEXT& context, CREATE_RUN create_run, T default_begin, T default_end) noexcept {
			std::vector<harness_run> output;

			if (!opt_str.empty() && opt_str != "auto") {
				if (auto parsed = parse_list(opt_str)) {
					for (auto sv : parsed.value()) {
						auto v = parse_int<T>(sv);
						if (!v) {
							return std::nullopt;
						}
						output.emplace_back(create_run(context, v.value()));
					}
					if (!output.empty()) {
						return output;
					}
				}

				if (auto range = parse_range(opt_str, default_begin, default_end, static_cast<T>(1))) {
					for (auto v : expand_range(range.value())) {
						output.emplace_back(create_run(context, v));
					}
					if (!output.empty()) {
						return output;
					}
				}
			}

			for (T v = default_begin; v < default_end; ++v) {
				output.emplace_back(create_run(context, v));
			}

			return output;
		}

		template <typename CONTEXT, size_t AS, typename CREATE_RUN>
		std::optional<std::vector<harness_run>> make_filtered_sweep(std::string_view opt_str, const CONTEXT& context, const std::array<std::string_view, AS>& valid_values, CREATE_RUN create_run,  std::string_view default_min, std::string_view default_max) noexcept {
			std::vector<harness_run> output;

			if (!opt_str.empty() && opt_str != "auto") {
				if (auto filtered = sweep_helpers::filter_for_range_str(opt_str, valid_values, default_min, default_max, 1u)) {
					for (const auto& v : filtered.value()) {
						output.emplace_back(create_run(context, v));
					}
					if (!output.empty()) {
						return output;
					}
				}

				if (auto parsed = parse_list(opt_str)) {
					for (auto sv : parsed.value()) {
						auto it = std::find(valid_values.begin(), valid_values.end(), std::string{sv});
						if (it == valid_values.end()) {
							return std::nullopt;
						}

						output.emplace_back(create_run(context, std::string(*it)));
					}
					if (!output.empty()) {
						return output;
					}
				}
			}

			for (const auto& v : valid_values) {
				output.emplace_back(create_run(context, std::string(v)));
			}

			return output;
		}

		template <typename CONTEXT, std::integral T, size_t AS, typename CREATE_RUN>
		std::optional<std::vector<harness_run>> make_filtered_sweep(std::string_view opt_str, const CONTEXT& context, const std::array<T, AS>& valid_values, CREATE_RUN create_run,  T default_min, T default_max) noexcept {
			std::vector<harness_run> output;

			if (!opt_str.empty() && opt_str != "auto") {
				if (auto filtered = sweep_helpers::filter_for_range_str(opt_str, valid_values, default_min, default_max, static_cast<T>(1))) {
					for (const auto& v : filtered.value()) {
						output.emplace_back(create_run(context, v));
					}
					if (!output.empty()) {
						return output;
					}
				}

				if (auto parsed = parse_list(opt_str)) {
					for (auto v : parsed.value()) {
						auto it = std::find(valid_values.begin(), valid_values.end(), parse_int<T>(v));
						if (it == valid_values.end()) {
							return std::nullopt;
						}

						output.emplace_back(create_run(context, *it));
					}
					if (!output.empty()) {
						return output;
					}
				}
			}

			for (const auto& v : valid_values) {
				output.emplace_back(create_run(context, v));
			}

			return output;
		}
	} // namespace bench_utils::sweep_helpers
} // namespace bench_utils
