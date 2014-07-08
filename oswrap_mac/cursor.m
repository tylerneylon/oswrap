// cursor.m
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_mac
//

#include "cursor.h"

NSCursor *cursors[3] = {nil, nil, nil};


// Internal functions.

static void init_if_needed() {
  static bool is_initialized = NO;
  if (is_initialized) return;

  cursors[cursor__default] = [NSCursor arrowCursor];
  cursors[cursor__ibeam]   = [NSCursor IBeamCursor];
  cursors[cursor__hand]    = [NSCursor pointingHandCursor];

  is_initialized = YES;
}

// Public functions.

cursor__Type cursor__get() {
  init_if_needed();

  NSCursor *c = [NSCursor currentCursor];

  for (int i = 0; i < 3; ++i) {
    if (c == cursors[i]) return i;
  }

  return cursor__other;
}

void cursor__set(cursor__Type which_cursor) {
  init_if_needed();

  [cursors[which_cursor] set];
}
