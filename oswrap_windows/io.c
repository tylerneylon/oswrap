// io.c
//
// https://github.com/tylerneylon/oswrap
//

#include "io.h"

#include <windows.h>

int io__convert_system_code(int system_key_code) {

  switch (system_key_code) {

  case VK_BACK:
    return io__delete;

  case VK_ESCAPE:
    return io__esc;

  case VK_SHIFT:
    return io__shift;

  case VK_F1:
    return io__f1;

  }

  // io__tab and io__return are already the same.
  // io__enter has no equivalent on windows.

  return system_key_code;
}
