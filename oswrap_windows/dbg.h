// dbg.h
//
// https://github.com/tylerneylon/oswrap in oswrap_windows
//
// Debugging tools.
//

#pragma once

#include <stdarg.h>

// A version of printf that provides debug output on both windows and mac. If
// the resulting string is 2k or longer, it will be truncated to 2047
// characters.
int dbg__printf(const char *fmt, ...);
int dbg__vprintf(const char *fmt, va_list args);
