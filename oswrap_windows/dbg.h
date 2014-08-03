// dbg.h
//
// https://github.com/tylerneylon/oswrap in oswrap_windows
//
// Debugging tools.
//

// A version of printf that provides debug output on both windows and mac.
// If the resulting string is 2k or longer, it will be truncated to 2047 characters.
int dbg__printf(const char *fmt, ...);
