#include "now.h"

#ifdef _WIN32
#include <windows.h>
#else
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_time.h>
#else
#include <time.h>
#endif
#endif

#include "cbit.h"



double now() {
#ifdef _WIN32

  // Windows version.
  static double counts_per_sec;

  static bit is_initialized = false;
  if (!is_initialized) {
    LARGE_INTEGER counts_per_sec_int;
    QueryPerformanceFrequency(&counts_per_sec_int);
    counts_per_sec = (double)counts_per_sec_int.QuadPart;
    is_initialized = true;
  }

  LARGE_INTEGER counts;
  QueryPerformanceCounter(&counts);
  double seconds = (double)counts.QuadPart / counts_per_sec;
  return seconds;

#else

#ifdef __APPLE__

  // Mac version.
  static int did_initialize = FALSE;
  static mach_timebase_info_data_t timebase_info;
  if (!did_initialize) {
    mach_timebase_info(&timebase_info);
    did_initialize = TRUE;
  }

  uint64_t abs_time = mach_absolute_time();
  return (double)abs_time * timebase_info.numer / timebase_info.denom / 1e9f;

#else

  // Linux version.
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  return (double)time.tv_sec + (double)time.tv_nsec / 1e9f;

#endif  // __APPLE__
#endif  // _WIN32
}

