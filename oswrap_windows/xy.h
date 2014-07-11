// xy.h
//
// https://github.com/tylerneylon/oswrap in oswrap_windows
//
// Coordinate-based types and functions.
//

#pragma once

#ifdef __APPLE__
#include <CoreGraphics/CoreGraphics.h>
typedef CGFloat xy__Float;
#else
typedef double  xy__Float;
#endif

typedef struct {
  xy__Float x;
  xy__Float y;
} xy__Pt;

typedef struct {
  xy__Float xmin;
  xy__Float ymin;
  xy__Float xmax;
  xy__Float ymax;
} xy__Rect;

#define xy__rect_pts(xmin, ymin, xmax, ymax) \
  ((xy__Rect) { xmin, ymin, xmax, ymax })

#define xy__rect_size(xmin, ymin, xsize, ysize) \
  ((xy__Rect) { xmin, ymin, xmin + xsize, ymin + ysize })

xy__Float xy__width (xy__Rect);
xy__Float xy__height(xy__Rect);

int   xy__pt_is_in_rect(xy__Pt, xy__Rect);
char *xy__str_of_rect(xy__Rect);
