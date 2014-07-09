// io.h
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_windows
//
// Tools for working with keyboard io.
//

#pragma once

// TODO Consider renaming this to "keys" if it just deals with the keyboard.

enum {
  io__tab    = 0x09,
  io__delete = 0x7f,
  io__enter  = 0x03,
  io__return = 0x0d,
  io__shift  = 0x80,
  io__esc    = 0x81,
  io__f1     = 0x7A
};
