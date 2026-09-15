#include <bench_utils/format.h>
#include <format>
#include <array>

namespace bench_utils {
	[[nodiscard]] std::string format_uint64_t(uint64_t val, std::string_view unit_str) noexcept {
		static constexpr std::array units{
			"B", "K", "M", "G", "T", "P", "E"
		};

		double size = static_cast<double>(val);
		std::size_t unit = 0;

		while (size >= 1000 && unit < units.size() - 1) {
			size /= 1000;
			unit++;
		}

		if (unit == 0) {
			return std::format("{} {}{}", val, units[unit], unit_str);
		} else {
			return std::format("{:.2f} {}{}", size, units[unit], unit_str);
		}
	}

	[[nodiscard]] std::string format_bytes(uint64_t bytes) noexcept {
		static constexpr std::array units{
			"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"
		};

		double size = static_cast<double>(bytes);
		std::size_t unit = 0;

		while (size >= 1024 && unit < units.size() - 1) {
			size /= 1024;
			unit++;
		}

		if (unit == 0) {
			return std::format("{} {}", bytes, units[unit]);
		} else {
			return std::format("{:.2f} {}", size, units[unit]);
		}
	}

	[[nodiscard]] std::string format_hms_flex(uint64_t total_seconds) noexcept {
		const auto hours = total_seconds / 3600ull;
		const auto minutes = (total_seconds % 3600ull) / 60ull;
		const auto seconds = total_seconds % 60ull;

		if (hours > 0ull) {
			return std::format("{}h {}m {}s", hours, minutes, seconds);
		} else if (minutes > 0ull) {
			return std::format("{}m {}s", minutes, seconds);
		} else {
			return std::format("{}s", seconds);
		}
	}
} // namespace bench_utils
