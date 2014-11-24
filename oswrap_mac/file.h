// file.h
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
//
// Functions to easily work with files across platforms.
//

#pragma once

#include <stdlib.h>

// Returned strings are owned by the callee and can change
// with each call.

// Returns the full path of packaged file.
char *file__get_path(const char *filename);

// Returns a directory that is safe to save files to.
char *file__save_dir_for_app(const char *app_name);

// Returns nonzero on success.
int   file__make_dir_if_needed(const char *dir);

// Returns nonzero if it exists.
int   file__exists(const char *path);

char *file__contents(const char *path, size_t *size);

// Returns nonzero on success.
int   file__write (const char *path, const char *contents);
int   file__append(const char *path, const char *contents);

// The path separator character; it's / on mac/linux and \ on windows.
extern char file__path_sep;
