// file.c
//
// https://github.com/tylerneylon/oswrap in oswrap_windows
//

#include "file.h"

#include "windows.h"
#include "shlwapi.h"

#include "winutil.h"

#include <io.h>
#include <stdio.h>

#include "cbit.h"

#define path_len 1024


// Internal globals.

static char homedir[path_len];

// Internal functions.

static void init_if_needed() {
  static bit is_initialized = false;
  if (is_initialized) return;

  GetModuleFileName(NULL /* hModule */, homedir, path_len);

  // Extract the dir name from homedir without the executable name.
  char drive[path_len];
  char dir[path_len];
  _splitpath_s(homedir, drive, path_len, dir, path_len, NULL, 0, NULL, 0);
  dir[strlen(dir) - 1] = '\0';  // Drop the trailing slash.
  snprintf(homedir, path_len, "%s%s", drive, dir);

  is_initialized = true;
}

static int write_or_append(const char *path, const char *contents, const char *mode) {
  FILE *f;
  fopen_s(&f, path, mode);
  if (f == NULL) {
    OutputDebugString("Error: file__write failed to open the path:");
    OutputDebugString(path);
    return false;
  }
  size_t num_bytes = strlen(contents);
  size_t bytes_written = fwrite(contents, 1 /* elt size */, num_bytes /* num elts */, f);
  fclose(f);
  if (bytes_written < num_bytes) {
    OutputDebugString("Error: file__write: fread wrote less bytes than expected at path:");
    OutputDebugString(path);
    return false;
  }
  return true;
}


// Public functions.

char *file__get_path(const char *filename) {
  init_if_needed();

  static char path[path_len];
  snprintf(path, path_len, "%s\\%s", homedir, filename);

  return path;
}

char *file__save_dir_for_app(const char *app_name) {
  init_if_needed();

  return homedir;  // The app_name is only used on the mac side.
}

int file__make_dir_if_needed(const char *dir_) {

  bit did_succeed = true;

  if (file__exists(dir_)) return did_succeed;

  char drive[path_len];
  char dir[path_len];
  char file[path_len];
  char ext[path_len];
  _splitpath_s(dir_, drive, path_len, dir, path_len, file, path_len, ext, path_len);

  // _splitpath_s incorrectly assumes the last path component is a file; add it back.
  strncat(dir, file, path_len);
  strncat(dir, ext, path_len);

  char path[path_len];
  strncpy(path, drive, path_len);

  char *token;
  char *dir_ptr = dir;
  while ((token = strsep(&dir_ptr, "\\"))) {
    if (strlen(token) == 0) continue;
    if (strlen(path)) strncat(path, "\\", path_len);
    strncat(path, token, path_len);
    if (!file__exists(path)) {
      did_succeed = did_succeed &&  CreateDirectory(path, NULL);
    }
  }

  return did_succeed;
}

int file__exists(const char *path) {
  int access_mode = 0;  // Check for existence.
  return _access(path, access_mode) == 0;
}

char *file__contents(const char *path, size_t *size) {
  FILE *f;
  errno_t err = fopen_s(&f, path, "rb");
  if (err) {
    char msg[128];
    sprintf_s(msg, 128, "Error: fopen_s failed with errno=%d in call to %s.\n", err, __FUNCTION__);
    OutputDebugString(msg);
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  *size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *file_contents = malloc(*size + 1);
  fread(file_contents, 1, *size, f);
  file_contents[*size] = '\0';
  fclose(f);

  return file_contents;
}

int file__write(const char *path, const char *contents) {
  return write_or_append(path, contents, "wb");
}

int file__append(const char *path, const char *contents) {
  return write_or_append(path, contents, "ab");
}

char file__path_sep = '\\';
