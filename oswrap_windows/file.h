// file.h
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_windows
//
// Functions to easily work with files across platforms.
//

#pragma once

#include <stdlib.h>

// Returned strings are owned by the callee and can change
// with each call.

// Returns the full path of packaged file.
char *file__get_path(const char *filename);

// Returns nonzero on success.
int   file__make_dir_if_needed(const char *dir);

// Returns nonzero if it exists.
int   file__exists(const char *path);

char *file__contents(const char *path, size_t *size);

// Returns nonzero on success.
int   file__write(const char *path, const char *contents);
