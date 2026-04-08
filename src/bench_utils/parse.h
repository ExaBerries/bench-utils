#pragma once
#include <cstdint>
#include <charconv>
#include <string_view>
#include <type_traits>

namespace bench_utils {
	template <std::integral T>
	inline std::optional<T> parse_int(std::string_view sv) noexcept {
		if (sv.empty()) {
			return std::nullopt;
		}
		T value = 0;
		auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
		if (ec != std::errc()) {
			return std::nullopt;
		}
		return value;
	}

	// comma separated list -> opt<std:;vector<std::string_view>>
	inline std::optional<std::vector<std::string_view>> parse_list(std::string_view list_sv) noexcept {
		if (list_sv == "auto") {
			return std::nullopt;
		}
		std::vector<std::string_view> result;
		std::string_view sv = list_sv;
		size_t start = 0;
		while (true) {
			auto comma = sv.find(',', start);
			std::string token = std::string(sv.substr(start, comma - start));
			result.push_back(token.c_str());
			if (comma == std::string_view::npos) {
				break;
			}
			start = comma + 1;
		}
		return result;
	}
} // namespace bench_utils
