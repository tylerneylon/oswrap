// draw.h
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
//
// Functions to delegate drawing commands to either
// windows's GDI framework or mac's core graphics.
//

#pragma once

#include "xy.h"

#ifdef __APPLE__
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>
#else
#include <windows.h>
#endif

// Types.
//
// These may be used directly by the underlying systems,
// with the exception of memory management.
//
// The draw__gl_format constant is designed for use as the
// format in calls to glTexSubImage2D and related functions.

#ifdef __APPLE__

typedef CGContextRef draw__Bitmap;
typedef CTFontRef    draw__Font;
typedef CGColorRef   draw__Color;

#define draw__gl_format GL_RGBA

#else

typedef HBITMAP     *draw__Bitmap;
typedef HFONT        draw__Font;
typedef COLORREF     draw__Color;

#define draw__gl_format GL_BGRA

#endif

// Bitmaps.

draw__Bitmap draw__new_bitmap     (int w, int h);
void         draw__delete_bitmap  (draw__Bitmap bitmap);
void         draw__set_bitmap     (draw__Bitmap bitmap);
// TODO draw__get_bitmap_data would make sense returning char * on
//      windows. Would that also make sense on mac?
// Do not directly free the returned memory; it is owned by the draw__Bitmap
// object.
void *       draw__get_bitmap_data(draw__Bitmap bitmap);

// Fonts and text.

draw__Font   draw__new_font      (const char *name, int size);
void         draw__delete_font   (draw__Font font);
void         draw__set_font      (draw__Font font);
void         draw__set_font_color(draw__Color color);

// Returns the x value at the end of the drawn text.
xy__Float    draw__string(const char *s,       // The string to draw.
                                  int x,       // The min x of the drawing box.
                                  int y,       // The min y of the drawing box.
                                  int w,       // The width of the drawing box;
                                               //   ignored when left-justified.
                                float pos);    // 0, 0.5, 1 = left, center, or
                                               //   right justified in the box.

// Colors.

draw__Color  draw__new_color       (double r, double g, double b);
void         draw__delete_color    (draw__Color color);
void         draw__rgb_fill_color  (double r, double g, double b);
void         draw__rgb_stroke_color(double r, double g, double b);

// Shapes and lines.

void         draw__fill_rect  (xy__Rect rect);
void         draw__stroke_rect(xy__Rect rect);
void         draw__line       (xy__Float x1, xy__Float y1,
                               xy__Float x2, xy__Float y2);
