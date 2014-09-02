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

#### ❑ `audio__Obj audio__new(const char *path);`

This allocates memory for and loads the audio data from the file
at the given path. Currently only `mp3` files are guaranteed to be
supported. The sound won't start playing yet.

#### ❑ `void audio__delete(audio__Obj obj);`

This frees memory used by an audio file previously loaded with
a call to `audio__new`. The mac version assumes ARC
(automatic reference counting) is being used, and deallocates
the object when all referencing variables have gone out of scope.

#### ❑ `void audio__play(audio__Obj obj);`

This begins playing the given sound from the beginning.
This plays the sound even if it has already been played before.

#### ❑ `void audio__stop(audio__Obj obj);`

The immediately stops the sound if it's currently playing.

#### ❑ `void audio__fade_in(audio__Obj obj);`

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

#### ❑ `void audio__fade_out(audio__Obj obj);`

This fades the volume down to silence over 3 seconds, and
stops playing when the volume reaches silences.

If either `audio__fade_in` or `audio__fade_out` is
in operation when another such call is made, the
earlier operation is effectively cancelled, although
the current volume is used as the starting point
for the new fade.

#### ❑ `void audio__set_loop(audio__Obj obj, bit do_loop);`

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

#### ❑ `char *crypt__sha1(const char *input);`

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

#### ❑ `cursor__Type cursor__get();`

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

#### ❑ `void cursor__set(cursor__Type which_cursor);`

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

#### ❑ `int dbg__printf(const char *fmt, ...);`

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

#### ❑ `draw__Bitmap draw__new_bitmap(int w, int h);`
#### ❑ `void draw__delete_bitmap(draw__Bitmap bitmap);`
#### ❑ `void draw__set_bitmap(draw__Bitmap bitmap);`
#### ❑ `void *draw__get_bitmap_data(draw__Bitmap bitmap);`

### Text rendering

#### ❑ `draw__Font draw__new_font(const char *name, int size);`
#### ❑ `void draw__delete_font(draw__Font font);`
#### ❑ `void draw__set_font(draw__Font font);`
#### ❑ `void draw__set_font_color(draw__Color color);`
#### ❑ `xy__Float draw__string(const char *s, int x, int y, int w, float pos);`

### Line and rectangle drawing

#### ❑ `draw__Color draw__new_color(double r, double g, double b);`
#### ❑ `void draw__delete_color(draw__Color color);`
#### ❑ `void draw__rgb_fill_color(double r, double g, double b);`
#### ❑ `void draw__rgb_stroke_color(double r, double g, double b);`
#### ❑ `void draw__fill_rect(xy__Rect rect);`
#### ❑ `void draw__stroke_rect(xy__Rect rect);`
#### ❑ `void draw__line(xy__Float x1, xy__Float y1, xy__Float x2, xy__Float y2);`


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

#### ❑ `double now();`

This function returns a timestamp from the machine's
monotonic clock. The timestamps are in seconds relative
to an undefined 0-instant. This means that
intervals between timestamps within a single run of
the app are meaningful. The returned values attempt to
have resolution on the order of microseconds or better.

---
