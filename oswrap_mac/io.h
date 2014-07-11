// io.h
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
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

// Call this with the following input to get an os-agnostic keycode:
//  windows: wParam from the WM_KEYDOWN message
//  mac:     [event keyCode] from the keyDown: method
// TODO More fully document the mac case.
int io__convert_system_code(int system_key_code);
