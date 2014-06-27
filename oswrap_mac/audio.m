// audio.m
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_mac
//

#include "audio.h"

#define fade_delta_timestep 0.3

#define snd(x) ((__bridge Sound *)(x))

// Internal class.

@interface Sound : NSSound {
  NSTimer *timer;
  float    dir;
}
@end

@implementation Sound

- (void)fadeWithTimer:(NSTimer *)timer_ {

  if (dir > 0 && ![self isPlaying]) {
    [self play];
    [self setVolume:0.0];
  }
  
  float v = [self volume] + dir;
  if (v > 1) v = 1;
  if (v < 0) v = 0;
  [self setVolume:v];
  
  if (v == 0) [self stop];
  
  if (v == 0 || v == 1) {
    [timer invalidate];
    timer = nil;
  }
  
}

- (void)fadeInDirection:(NSNumber *)ns_dir {
  
  if (![NSThread isMainThread]) {
    [self performSelectorOnMainThread:@selector(fadeInDirection:) withObject:ns_dir waitUntilDone:NO];
    return;
  }
  
  dir = [ns_dir floatValue];
  
  if (!timer) {
    timer = [NSTimer scheduledTimerWithTimeInterval:fade_delta_timestep
                                             target:self
                                           selector:@selector(fadeWithTimer:)
                                           userInfo:nil
                                            repeats:YES];
  }
}

@end

// Public functions.

audio__Obj audio__new(const char *path) {
  NSString *nsstr_path = [NSString stringWithUTF8String:path];
  return (__bridge_retained void *)[[Sound alloc] initWithContentsOfFile:nsstr_path byReference:NO];
}

void audio__delete(audio__Obj obj) {
  // Do nothing since we're using ARC.
  // This function is useful on windows, though. (I think.)
}

void audio__play(audio__Obj obj) {
  [snd(obj) play];
}

void audio__stop(audio__Obj obj) {
  [snd(obj) stop];
}

void audio__fade_in(audio__Obj obj) {
  [snd(obj) fadeInDirection:@0.1];
}

void audio__fade_out(audio__Obj obj) {
  [snd(obj) fadeInDirection:@-0.1];
}

void audio__set_loop(audio__Obj obj, bit do_loop) {
  [snd(obj) setLoops:do_loop];
}
