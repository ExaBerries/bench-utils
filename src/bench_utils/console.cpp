#include <bench_utils/console.h>
#include <cstdio>

#if defined(_WIN32)
	#include <windows.h>
#endif

namespace bench_utils {
	void reset_cursor(uint32_t lines) noexcept {
		if (lines == 0) return;

		#if defined(_WIN32)
			static bool ansi_enabled = false;
			static bool ansi_checked = false;

			if (!ansi_checked) {
				ansi_checked = true;

				HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				if (hOut != INVALID_HANDLE_VALUE) {
					DWORD mode = 0;
					if (GetConsoleMode(hOut, &mode)) {
						// Try enabling ANSI escape processing
						if (SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
							ansi_enabled = true;
						}
					}
				}
			}

			if (ansi_enabled) {
				// ANSI path (modern windows)
				std::printf("\x1b[%uF", lines);
			} else {
				// fallback: Win32 API
				HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
				if (h_out == INVALID_HANDLE_VALUE) return;

				CONSOLE_SCREEN_BUFFER_INFO info;
				if (!GetConsoleScreenBufferInfo(h_out, &info)) return;

				COORD pos = info.dwCursorPosition;

				if (lines > static_cast<uint32_t>(pos.Y)) {
					pos.Y = 0;
				} else {
					pos.Y -= static_cast<SHORT>(lines);
				}
				pos.X = 0;

				SetConsoleCursorPosition(h_out, pos);
			}
		#else
			// linux / macOS (ANSI standard)
			std::printf("\x1b[%uF", lines);
		#endif

		std::fflush(stdout);
	}
} // namespace bench_utils
