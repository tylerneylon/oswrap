# oswrap

*A thin layer enabling cross-platform apps.*

This is a small collection of wrappers enabling
C code that works on both windows and mac os x.
This library was originally written to act as
part of OpenGL-based games, although it may be
useful for any cross-platform app.

The library is made up of the following
modules:

module   |functionality
---------|0-------------
`audio`  | audio loading and playback
`crypt`  | cryptographic functions
`cursor` | setting the mouse cursor
`draw`   | basic 2D drawing
`file`   | using files
`img`    | loading image files
`now`    | high-resolution timestamps
`winutil`| posix-like functions on windows
`xy`     | working with points and rectangles


