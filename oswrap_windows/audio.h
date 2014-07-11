// audio.h
//
// https://github.com/tylerneylon/oswrap in oswrap_windows
//
// Functions for playing audio files such as mp3's.
//
// On windows, using these functions requires linking with
// either winmm.lib or winmm.dll.
//

#pragma once

#include "cbit.h"


typedef void *audio__Obj;

audio__Obj audio__new      (const char *path);
void       audio__delete   (audio__Obj obj);

void       audio__play     (audio__Obj obj);
void       audio__stop     (audio__Obj obj);

void       audio__fade_in  (audio__Obj obj);
void       audio__fade_out (audio__Obj obj);

void       audio__set_loop (audio__Obj obj, bit do_loop);
