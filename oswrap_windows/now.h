// now.h
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_windows
//
// A cross-platform way to get a high-resolution
// timestamp.
//

#pragma once

// Returns the current time in seconds with nanosecond resolution.
// This is meant as a monotonic clock rather than a wall clock.
double now();
