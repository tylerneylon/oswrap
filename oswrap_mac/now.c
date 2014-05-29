#include "now.h"

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_time.h>
#else
#include <time.h>
#endif

double now() {
#ifdef __APPLE__
  static int did_initialize = FALSE;
  static mach_timebase_info_data_t timebase_info;
  if (!did_initialize) {
    mach_timebase_info(&timebase_info);
    did_initialize = TRUE;
  }

  uint64_t abs_time = mach_absolute_time();
  return (double)abs_time * timebase_info.numer / timebase_info.denom / 1e9f;
#else
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  return (double)time.tv_sec + (double)time.tv_nsec / 1e9f;
#endif
}

