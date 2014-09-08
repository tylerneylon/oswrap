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
---------|--------------
`audio`  | audio loading and playback
`crypt`  | cryptographic functions
`cursor` | setting the mouse cursor
`dbg`    | universal debug output
`draw`   | basic 2D drawing
`file`   | using files
`img`    | loading image files
`now`    | high-resolution timestamps
`winutil`| posix-like functions on windows
`xy`     | working with points and rectangles

---
## audio

The audio module supports loading and playback
of `mp3` files encapsulated as instances of the
`audio__Obj` type.
For example, the code below could be used
to start playing and then fade out the file
`mysound.mp3`.

On windows, using these functions requires linking with
either `winmm.lib` or `winmm.dll`.

```
const char *mysound_path = file__get_path("mysound.mp3");
audio__Obj  mysound      = audio__new(mysound_path);
audio__play(mysound);
audio__fade_out(mysound);  // Begin to fade it out.
audio__delete(mysound);    // Free the memory when you're done with the sound.
```

Every `audio__Obj` allocated with `audio__new` uses memory
until it is freed by a corresponding call to `audio__delete`.

##### ❑ `audio__Obj audio__new(const char *path);`

This allocates memory for and loads the audio data from the file
at the given path. Currently only `mp3` files are guaranteed to be
supported. The sound won't start playing yet.

##### ❑ `void audio__delete(audio__Obj obj);`

This frees memory used by an audio file previously loaded with
a call to `audio__new`. The mac version assumes ARC
(automatic reference counting) is being used, and deallocates
the object when all referencing variables have gone out of scope.

##### ❑ `void audio__play(audio__Obj obj);`

This begins playing the given sound from the beginning.
This plays the sound even if it has already been played before.

##### ❑ `void audio__stop(audio__Obj obj);`

The immediately stops the sound if it's currently playing.

##### ❑ `void audio__fade_in(audio__Obj obj);`

This fades the volume up to 100% over 3 seconds.
If the sound was previously stopped, it begins to play,
starting at volume 0. If it was already playing, it
continues to play at the current volume, and that
volume is gradually increased to 100%.

If either `audio__fade_in` or `audio__fade_out` is
in operation when another such call is made, the
earlier operation is effectively cancelled, although
the current volume is used as the starting point
for the new fade.

##### ❑ `void audio__fade_out(audio__Obj obj);`

This fades the volume down to silence over 3 seconds, and
stops playing when the volume reaches silences.

If either `audio__fade_in` or `audio__fade_out` is
in operation when another such call is made, the
earlier operation is effectively cancelled, although
the current volume is used as the starting point
for the new fade.

##### ❑ `void audio__set_loop(audio__Obj obj, bit do_loop);`

This causes the sound to repeat itself indefinitely and
without any pauses. It is still necessary to call - or
to have already called - `audio__play` to begin playing
the sound. This function uses the `do_loop` parameter to
determine whether or not the sound should loop when played.

---
## crypt

The crypt module contains a single function
which calculates the
[SHA1](http://en.wikipedia.org/wiki/SHA-1)
hash of a string.

On windows, using this function requires linking with
either `advapi32.lib` or `advapi32.dll`.

Here's an example of computing a SHA1 hash value:

```
char *mystring = "where is my towel?";
char *hash_val = crypt__sha1(mystring);
```

##### ❑ `char *crypt__sha1(const char *input);`

This returns a string representing the SHA1 hash of the
given `input`. The returned string contains a hex
representation, so that every byte of the raw hash value
becomes two ascii bytes of the returned string; for
example, the raw byte `0x1f` becomes the substring `"1f"`.
All letters are lower case.

The returned string is static memory that should not be
freed, and is only guaranteed to be safe to use until the
next call to `crypt__sha1`. Therefore, this function
is not thread-safe.

---
## cursor

The cursor module offers simple controls to get
and set the type of mouse cursor. For example,
the ibeam type is used to indicate that the mouse
is over a text input, and the hand is often used
to indicate a link in web pages.

Here's an example of setting the cursor to an ibeam
when the mouse is over a text input, and later
restoring it to its previous state:
```
cursor__Type old_cursor = cursor__get();
cursor__set(cursor__ibeam);
// Later, when the cursor is no longer on the text input.
if (old_cursor == cursor__other) old_cursor = cursor__default;
cursor__set(old_cursor);
```

##### ❑ `cursor__Type cursor__get();`

Returns the type of the current mouse cursor.
The type of the return value, `cursor__Type`,
is an enum defined
in `cursor.h`. It can return any of the types
listed below in the `cursor__set` documentation,
as well as one additional type:

* `cursor__other` = *an unknown cursor type*,

which indicates that the current mouse type is
one not known to `oswrap`. In this case, if you
change the mouse type using `cursor__set`, you
won't be able to restore the previous cursor
type using `oswrap`. This is not generally an issue
if you exclusively use `oswrap` to control the
cursor in your app.

##### ❑ `void cursor__set(cursor__Type which_cursor);`

Sets the current mouse cursor to the given value. The
following values of `which_cursor` are supported:

* `cursor__default` = *the default mouse pointer*,
* `cursor__ibeam` = *the usual text-input mouse pointer*,
* `cursor__hand` = *the usual hyperlink pointing hand*.

In some cases — in particular, this is common on windows —
your app window will have a default cursor type, and that
type will quickly override a call to `cursor__set`, so that
the user may not even realize you attempted to programmatically
set the cursor.

There are two solutions to this.
First, you can set the `hCursor` field of `WNDCLASSEX` to
`NULL` when calling `RegisterClassEx`, which means that
your window has no default mouse cursor. Alternatively,
you could call `cursor__set` periodically in your event
loop. In practice, it appears to be sufficient to call this
for each `WM_MOUSEMOVE` message received, and does not
adversely affect performance.


---
## dbg

The dbg module contains a single function, `dbg__printf`,
which is capable of providing `printf`-stye debug output
on either platform. Xcode automatically captures the
output of the usual `printf` function, but windows does not,
offering instead the weaker `OutputDebugString` function,
which only accepts a static string without formatting
or variadic parameters. The `dbg__printf` function adds
a universal, full-featured variadic interface to windows.

Here is a usage example:
```
// Earlier, a function was declared: float f(int i);
for (int i = 0; i < 4; ++i) {
  dbg__printf("i=%d; f(i)=%g\n", i, f(i));
}
```

##### ❑ `int dbg__printf(const char *fmt, ...);`

This function acts identically to the usual `printf`
except that its output is visible in Visual Studio's
output window, unlike the the usual `printf` function.

There is still one significant difference between the
format specifiers used on mac versus windows. On mac,
the specifier `%zd` is used to indicate a `size_t` parameter
type, whereas on windows, the correct specifier is `%Id`.

As a reminder, the return value of `printf` indicates the
number of characters printed.


---
## draw

The draw module contains functions to handle basic 2D
drawing functionality and text rendering. Images are
rendered to a memory buffer which can then be accessed
directly or used a texture to be passed into OpenGL.

The functions of the draw module are described in the
following subsections:

* bitmap and pixel management,
* text rendering, and
* line and rectangle drawing.

Usage examples are given in each subsection.

### Bitmap and pixel management

Work with a bitmap by first creating it, using it,
and then deleting it.
A bitmap is an object of type `draw__Bitmap`.

New bitmaps are created either by calling
`draw__new_bitmap` for a blank canvas, or by
calling `img__new_bitmap`, documented in the `img` section,
for loading an image file. Call `draw__set_bitmap` to
select which bitmap will be affected by subsequent drawing calls.
When you're done with a bitmap, free the memory by calling
`draw__delete_bitmap`.

Here's an example:
```
draw__Bitmap bitmap = draw__new_bitmap(1024, 1024);
draw__set_bitmap(bitmap);                        // Set which bitmap to draw to.
my_drawing_routine();                            // Draw to the currently set bitmap.
save_pixel_data(draw__get_bitmap_data(bitmap));  // Access raw pixel data.
draw__delete_bitmap(bitmap);                     // Free memory used by the bitmap.
```

If you'd like to use native drawing functions, you can
treat a `draw__Bitmap` as a `CGContextRef` on mac, or as
a `HBITMAP *` on windows; though doing so will
make it harder to keep your code cross-platform.

##### ❑ `draw__Bitmap draw__new_bitmap(int w, int h);`

Create a new in-memory bitmap canvas of width `w` and
height `h`. This requires at least `4 * w * h` bytes along
with some per-bitmap overhead. See the notes for
`draw__get_bitmap_data` for more information about the
exact pixel format used.

##### ❑ `void draw__delete_bitmap(draw__Bitmap bitmap);`

Free the memory associated with the given bitmap.

##### ❑ `void draw__set_bitmap(draw__Bitmap bitmap);`

Choose which bitmap will be affected by subsequent draw calls.
This is an inexpensive operation; if you're not certain that
your bitmap is already set as the active one, the suggested
practice is to call `draw__set_bitmap` liberally to make
coding easier without any significant performance cost.

Because the draw module internally tracks some state, including
the active bitmap,
it is not thread-safe.

##### ❑ `void *draw__get_bitmap_data(draw__Bitmap bitmap);`

This returns a pointer to the raw pixel data associated with the
given bitmap. This data is safe to either read from or write to.
The data is guaranteed to live until you call `draw__delete_bitmap`
on the associated bitmap. Do not perform any direct memory
management on the pointer returned by `draw__get_bitmap_data`.

The return value has pixels stored as one byte per RGB, plus
a byte for an alpha channel. The exact layout is `RGBA` on mac
and `BGRA` on windows; when passing data directly to OpenGL,
use the `draw__gl_format` constant to indicate the pixel format.

### Text rendering

Similar to `draw__Bitmap` objects, there is a `draw__Font` object
which must be initialized, set as active, and then used to render
text. In addition, a font color of type `draw__Color` must also be
allocated and set as active. Both fonts and colors use resources
that can be
explicitly freed by calling `draw__delete_{font,color}` when
they're no longer needed.

Here's an example that draws the string "hello!" in blue with the
lower-left corner of the text at (10, 10); this interpretation
assumes the image will be rendered with the lower-left corner
as position (0, 0).

```
// A draw__Bitmap has already been created and set.

// Choose a font.
draw__Font font = draw__new_font("Helvetica", 26);
draw__set_font(font);

// Choose a font color.
draw__Color blue = draw__new_color(0.0, 0.0, 1.0);
draw__set_font_color(blue);

// Draw the text.
int width = 100;
float left_align = 0.0;
draw__string("hello!", 10, 10, width, left_align);
```

#### Fonts

##### ❑ `draw__Font draw__new_font(const char *name, int size);`

Creates a new `draw__Font` object based on the given font face name
and font size. You must call this function once for each font size
you'd like to use.

##### ❑ `void draw__set_font(draw__Font font);`

Set the given font object as currently active. Some font
must be set for text rendering to work.

##### ❑ `void draw__delete_font(draw__Font font);`

Release any resources used by the `draw__Font` object.
The font object can no longer be used after it's deleted.

#### Font colors

The *new* and *delete* color functions are not obviously specific to
fonts, but so far color objects can only be used with fonts.
Line- and rectangle-drawing functions use a different color
interface explained below.

##### ❑ `draw__Color draw__new_color(double r, double g, double b);`

Create a new color object with the given red/green/blue values.
The values are expected to be in the range 0 to 1.

##### ❑ `void draw__set_font_color(draw__Color color);`

Set the given color as the active font color.

##### ❑ `void draw__delete_color(draw__Color color);`

Release any resources used by the `draw__Color` object.
The color object can no longer be used after it's deleted.

#### Rendering

##### ❑ `xy__Float draw__string(const char *s, int x, int y, int w, float pos);`

Draw the given string `s` in the rectangle with minimum corner
`x, y` and width `w`. The alignment of the text depends on the `pos`
parameter as follows:

| `pos` | alignment
|-------|-------------
| 0     | **left**: left edge of text starts at `x`
| 0.5   | **center**: center of text is at `x + w/2`
| 1     | **right**: right edge of text is at `x + w`

In order for text rending to work, you must have first set an
active bitmap by calling `draw__set_bitmp`, then set an
active font and font color by calling
`draw__set_font` and `draw__set_font_color`.

### Line and rectangle drawing

##### ❑ `void draw__rgb_fill_color(double r, double g, double b);`
##### ❑ `void draw__rgb_stroke_color(double r, double g, double b);`
##### ❑ `void draw__fill_rect(xy__Rect rect);`
##### ❑ `void draw__stroke_rect(xy__Rect rect);`
##### ❑ `void draw__line(xy__Float x1, xy__Float y1, xy__Float x2, xy__Float y2);`

---
## file

The file module provides easier uniform access to files. While the standard
`f{open,read,write,close}` interface works on both windows and mac machines,
there are some differences worth abstracting away.

First, windows files are *not* open in binary mode by default, while mac files are.
The `file__contents` provides binary data without text-mode interpretation, which is
what you'd expect if you're used to working with linux or mac file systems.
The `file__write` function is a convenient one-liner — the write-version analog to
`file__contents`. If more fine-grained file control is needed, direct use of the
`fopen`-and-friends functions is encouraged, keeping in mind to use binary-mode
in `fopen`.

Mac apps are packaged in a fundamentally different way from windows apps in that
a mac app is itself a directory, while on windows an executable is just a file that
must come packaged explicitly with other files, rather than partially hidden within
any special kind of directory. Each os contains different paths for either:

* files that came bundled with your app, or
* files that your app created after installation.

The function `file__get_path` helps you find files bundled with your app.
The function `file__save_dir_for_app` provides the absolute path of the directory
where your app can save to or load from files it creates.

Here is an example use case by a theoretical game:
```
// Locate and load the bundled file intro_story.txt.

char *path  = file__get_path("intro_story.txt");
char *story = file__contents(path);
show_text_to_user(story);
free(story);

// Save the player's game.

char *base_dir = file__save_dir_for_app("MyGame");

char save_dir[2048];
snprintf(save_dir, 2048, "%s%c%s", base_dir, file__path_sep, "saves");
file__make_dir_if_needed(save_dir);

char save_path[2048];
snprintf(save_path, 2048, "%s%c%s", save_dir, file__path_sep, "mysavefile");
file__write(save_path, get_save_data());
```

The current `oswrap` implementation is not guaranteed to
recognize `~` as a shorthand path element for the
current user's home directory.

##### ❑ `char *file__get_path(const char *filename);`

Returns the absolute path of the given bundled filename,
or `NULL` if the file isn't found. The returned string
is owned by the callee and should not be freed; it is
safe to use until `file__get_path` is called again.

##### ❑ `char *file__save_dir_for_app(const char *app_name);`

Returns the absolute path of a directory in which it is
reasonable to save files. On mac, this is your app's
directory in the user's `Application Support` directory;
on windows, this is the same directory that your
executable file resides in.

The returned path does *not* include a trailing slash character.

The returned value is owned by the callee and should not
be freed; it is safe to use until `file__save_dir_for_app`
is called again.

##### ❑ `int file__make_dir_if_needed(const char *dir);`

If a directory or file already exists at the given location,
this function does nothing and returns 1 indicating
success. If nothing exists at the given location, then it
is created as a directory. If any intermediate directories
are also missing, they are also created. The return value
is 1 when the call is successful, and 0 if there is an error.

##### ❑ `int file__exists(const char *path);`

Returns 0 if there is nothing at the given path;
returns nonzero if a file or directory exists there.

##### ❑ `char *file__contents(const char *path, size_t *size);`

Reads the entire contents of the file at `path` into a
newly-allocated, null-terminated buffer and returns a pointer to that
buffer. The size of the buffer is also written to `size`, which
must *not* be `NULL`. The caller
is responsible for freeing the returned buffer.

##### ❑ `int file__write(const char *path, const char *contents);`

Opens the file at `path` for writing and sets the contents of that
file to `contents`. The length of `contents` is determined by treating
it as a null-terminated string. If a file at `path` already existed,
it is over-written; otherwise a new file is created with the given
contents. The return value is 0 when there is an error and nonzero
when the write succeeds.

##### ❑ `extern char file__path_sep;`

This is a single character used by the system as a separator
between directories in a path string. On mac it is `/` while
on windows it is `\`.

---
## img

The img module contains a single function, `img__new_bitmap`,
which loads an image file into a `draw__Bitmap` object.
The interpretation of file data is left up to the os, so that
the list of supported file types is also system-dependent.
In practice, popular file types such as `.jpg` and `.png`
files appear to be universally supported.

The loaded `draw__Bitmap` can be drawn onto, edited at
the raw pixel level, or sent into OpenGL for rendering
as a texture.

##### ❑ `draw__Bitmap img__new_bitmap(const char *path, int *w, int *h);`

Loads the image file at `path` into a new `draw__Bitmap` object.
The caller owns the returned bitmap; free it by calling `draw__delete_bitmap`
to deallocate its resources.
The width and height of the image are output as the values of
`*w` and `*h`; these pointers are expected to be non-NULL.


---
## now

The now module contains a single function, `now`,
which returns a high-precision timestamp from the
machine's monotonic clock. This value is not guaranteed
to reflect wall-clock time, but rather is designed to
increase over time as your app runs.

Here is a usage example:
```
double start_time = now();
f(x);
double time_passed = now() - start_time;
printf("f(x) took %g seconds.\n", time_passed);
```

##### ❑ `double now();`

This function returns a timestamp from the machine's
monotonic clock. The timestamps are in seconds relative
to an undefined 0-instant. This means that
intervals between timestamps within a single run of
the app are meaningful. The returned values attempt to
have resolution on the order of microseconds or better.

---
