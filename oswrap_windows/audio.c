// audio.c
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_windows
//

#include "audio.h"

#include <windows.h>

#include "winutil.h"

#define out_str_len       512
#define cmd_len           512
#define max_fades          64
#define fade_interval_ms  300
#define fade_volume_delta 100
#define full_volume      1000

// Temporarily set this to true to see mci commands
// in the debug output.
#define show_commands   false


// Internal types and globals.

typedef struct {
  char *alias;
  bit   do_loop;
  int   volume;
  int   volume_delta;
} Audio;

static int next_alias = 0;

// These two arrays are index-synchronized.
static int        timer_ids[max_fades];
static Audio *fading_audios[max_fades];


// Internal functions.

static void init_if_needed() {
  static bit is_initialized = false;
  if (is_initialized) return;

  for (int i = 0; i < max_fades; ++i) {
    timer_ids[i] = -1;  // -1 means this is an empty slot.
  }

  is_initialized = true;
}

// Finds the first matching index based on either audio
// or timer_id; returns max_fades if none is found.
static int find_timer_index(Audio *audio, int timer_id) {
  for (int i = 0; i < max_fades; ++i) {
    if (audio && fading_audios[i] == audio) return i;
    if (!audio && timer_ids[i] == timer_id) return i;
  }
  return max_fades;
}

static void kill_timer(int audio_index) {
  int last_index = find_timer_index(NULL, -1);
  KillTimer(NULL, timer_ids[audio_index]);

  timer_ids[audio_index]     = timer_ids[last_index];
  fading_audios[audio_index] = fading_audios[last_index];

  timer_ids[last_index] = -1;
}

static char *run_command(const char *cmd) {
  static char out_str[out_str_len];

  // Debugging output; off by default.
  if (show_commands) {
    OutputDebugString("About to run the mci command:\n");
    OutputDebugString(cmd);
    OutputDebugString("\n");
  }
 
  MCIERROR err = mciSendString(cmd, out_str, out_str_len, 0 /* hwndCallback */);
  if (err) {
    char err_str[cmd_len];
    snprintf(err_str, cmd_len, "After mciSendString(%s), err=%d\n", cmd, err);
    OutputDebugString(err_str);
  }
  return out_str;
}

static void CALLBACK timer_callback(HWND hwnd, UINT msg, UINT_PTR timer_id, DWORD time) {
  int audio_index = find_timer_index(NULL, timer_id);
  Audio *audio = fading_audios[audio_index];

  audio->volume += audio->volume_delta;
  if (audio->volume > full_volume) audio->volume = full_volume;
  if (audio->volume < 0)           audio->volume = 0;

  char cmd[cmd_len];
  snprintf(cmd, cmd_len, "setaudio %s volume to %d", audio->alias, audio->volume);
  run_command(cmd);

  int v = audio->volume;
  if (v == 0 || v == full_volume) kill_timer(audio_index);
  if (v == 0) audio__stop(audio);
}

static void ensure_timer_is_set(Audio *audio) {
  init_if_needed();

  int i = find_timer_index(audio, 0);
  if (i < max_fades) return;

  i = find_timer_index(NULL, -1);
  if (i == max_fades) {
    OutputDebugString("Error in audio module: too many simultaneous fades requested.\n");
    return;
  }
  timer_ids[i] = SetTimer(
    NULL,              // hWnd
    0,                 // nIDEvent=0 tells the system to assign an id.
    fade_interval_ms,  // ms
    timer_callback);
  fading_audios[i] = audio;
}

static char *get_cmd_to_play(Audio *audio) {
  static char cmd[cmd_len];
  const char *play_flag = audio->do_loop ? " repeat" : "";
  snprintf(cmd, cmd_len, "play %s%s", audio->alias, play_flag);
  return cmd;
}


// Public functions.

audio__Obj audio__new(const char *path) {
  Audio *audio   = malloc(sizeof(Audio));
  audio->alias   = malloc(32);
  audio->do_loop = false;
  audio->volume  = full_volume;

  snprintf(audio->alias, 32, "alias%d", next_alias++);

  char cmd[cmd_len];
  snprintf(cmd, cmd_len, "open %s alias %s", path, audio->alias);
  run_command(cmd);

  return audio;
}

void audio__delete(audio__Obj obj) {
  Audio *audio = (Audio *)obj;

  char cmd[cmd_len];
  snprintf(cmd, cmd_len, "close %s", audio->alias);
  run_command(cmd);

  free(audio->alias);
  free(audio);
}

void audio__play(audio__Obj obj) {
  Audio *audio = (Audio *)obj;

  char *cmd = get_cmd_to_play(audio);
  run_command(cmd);
}

void audio__stop(audio__Obj obj) {
  Audio *audio = (Audio *)obj;

  char cmd[cmd_len];
  snprintf(cmd, cmd_len, "stop %s", audio->alias);
  run_command(cmd);
}

void audio__fade_in(audio__Obj obj) {
  Audio *audio = (Audio *)obj;

  audio->volume = 0;
  audio->volume_delta = fade_volume_delta;
  ensure_timer_is_set(audio);

  char cmd[cmd_len];
  snprintf(cmd, cmd_len, "setaudio %s volume to %d", audio->alias, audio->volume);
  run_command(cmd);

  audio__play(audio);
}

void audio__fade_out(audio__Obj obj) {
  Audio *audio = (Audio *)obj;

  audio->volume_delta = -fade_volume_delta;
  ensure_timer_is_set(audio);
}

void audio__set_loop(audio__Obj obj, bit do_loop) {
  Audio *audio   = (Audio *)obj;
  audio->do_loop = do_loop;

  char cmd[cmd_len];
  snprintf(cmd, cmd_len, "status %s mode", audio->alias);
  char *audio_mode = run_command(cmd);

  if (strcmp(audio_mode, "playing") == 0) {
    char *cmd = get_cmd_to_play(audio);
    run_command(cmd);
  }
}
