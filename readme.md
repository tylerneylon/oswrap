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

### ❑ `audio__Obj audio__new(const char *path);`

This allocates memory for and loads the audio data from the file
at the given path. Currently only `mp3` files are guaranteed to be
supported. The sound won't start playing yet.

### ❑ `void audio__delete(audio__Obj obj);`

This frees memory used by an audio file previously loaded with
a call to `audio__new`. The mac version assumes ARC
(automatic reference counting) is being used, and deallocates
the object when all referencing variables have gone out of scope.

### ❑ `void audio__play(audio__Obj obj);`

This begins playing the given sound from the beginning.
This plays the sound even if it has already been played before.

### ❑ `void audio__stop(audio__Obj obj);`

The immediately stops the sound if it's currently playing.

### ❑ `void audio__fade_in(audio__Obj obj);`

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

### ❑ `void audio__fade_out(audio__Obj obj);`

This fades the volume down to silence over 3 seconds, and
stops playing when the volume reaches silences.

If either `audio__fade_in` or `audio__fade_out` is
in operation when another such call is made, the
earlier operation is effectively cancelled, although
the current volume is used as the starting point
for the new fade.

### ❑ `void audio__set_loop(audio__Obj obj, bit do_loop);`

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

### ❑ `char *crypt__sha1(const char *input);`

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
