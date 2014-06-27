// winutil.h
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_windows
//
// A set of windows-focused wrappers to enable easier
// cross-platform code.
//


#ifdef _WIN32

#include <windows.h>

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

#define strdup _strdup

static char *basename(char *path) {
  static char fname[_MAX_FNAME + _MAX_EXT];
  static char ext  [_MAX_EXT];
  _splitpath_s(
    path,
    NULL,  0,  // drive
    NULL,  0,  // dir
    fname, _MAX_FNAME,
    ext,   _MAX_EXT);
  strcat_s(fname, sizeof(fname), ext);
  return fname;
}

static char *strsep(char **stringp, const char *delim) {
  if (*stringp == NULL) { return NULL; }

  char *token_start = *stringp;
  *stringp = strpbrk(token_start, delim);
  if (*stringp) {
    **stringp = '\0';
    (*stringp)++;
  }

  return token_start;
}

#define vsnprintf(s, n, fmt, args) vsnprintf_s(s, n, _TRUNCATE, fmt, args)
#define strncpy(dst, src, num) strncpy_s(dst, num, src, _TRUNCATE)
#define snprintf(s, n, fmt, ...) sprintf_s(s, n, fmt, __VA_ARGS__)

static int vasprintf(char **ret, const char *fmt, va_list args) {
  *ret = NULL;
  
  va_list copy;
  va_copy(copy, args);
  int count = _vscprintf(fmt, args);
  if (count < 0) return -1;
  *ret = (char *)malloc(count + 1);
  if (*ret == NULL) return -1;
  vsprintf_s(*ret, count + 1, fmt, copy);
  va_end(copy);

  return count;
}

static int asprintf(char **ret, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int count = vasprintf(ret, fmt, args);
  va_end(args);
  return count;
}

static char *stpcpy(char *dst, const char *src) {
  for (; *dst = *src; ++dst, ++src);
  return dst;
}

#define strncat(dst, src, num) strncat_s(dst, num, src, _TRUNCATE)

#endif
