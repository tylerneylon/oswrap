// cursor.c
//
// https://github.com/tylerneylon/oswrap in oswrap_windows
//

#include "cursor.h"

#include "cbit.h"

#include <windows.h>


static HCURSOR cursors[3] = { NULL, NULL, NULL };

// Internal functions.

static void init_if_needed() {
  static bit is_initialized = false;
  if (is_initialized) return;

  cursors[cursor__default] = LoadCursor(NULL /* hInstance */, IDC_ARROW);
  cursors[cursor__ibeam] =   LoadCursor(NULL /* hInstance */, IDC_IBEAM);
  cursors[cursor__hand] =    LoadCursor(NULL /* hInstance */, IDC_HAND);

  is_initialized = true;
}

cursor__Type cursor__get() {
  init_if_needed();

  HCURSOR c = GetCursor();

  for (int i = 0; i < 3; ++i) {
    if (c == cursors[i]) return i;
  }

  return cursor__other;
}

void cursor__set(cursor__Type which_cursor) {
  init_if_needed();

  SetCursor(cursors[which_cursor]);
}
