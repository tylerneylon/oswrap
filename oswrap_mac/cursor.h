// cursor.h
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_mac
//
// Basic control of the mouse cursor.
//

#pragma once

typedef enum {
  cursor__default,  // The default arrow.
  cursor__ibeam,    // An ibeam for text inputs.
  cursor__hand,     // A pointy hand for buttons.
  cursor__other     // Only for use as a return value.
} cursor__Type;

cursor__Type cursor__get();
void         cursor__set(cursor__Type which_cursor);
