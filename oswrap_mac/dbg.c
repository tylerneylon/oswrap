// dbg.c
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
//

#include "dbg.h"

#include <stdarg.h>
#include <stdio.h>

#ifndef _WIN32
#define OutputDebugString(s) printf("%s", s)
#else
#include "winutil.h"
#endif

// TODO Update this to work for arbitrary-length strings.

int dbg__printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char buffer[2048];
  int chars_written = vsnprintf(buffer, 2048, fmt, args);
  va_end(args);

  OutputDebugString(buffer);
  return chars_written;
}
