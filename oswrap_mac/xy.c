// xy.c
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
//

#include "xy.h"

xy__Float xy__width(xy__Rect rect) {
  return rect.xmax - rect.xmin;
}

xy__Float xy__height(xy__Rect rect) {
  return rect.ymax - rect.ymin;
}

int xy__pt_is_in_rect(xy__Pt p, xy__Rect r) {
  return (p.x >= r.xmin &&
          p.x <  r.xmax &&
          p.y >= r.ymin &&
          p.y <  r.ymax);
}

char *xy__str_of_rect(xy__Rect r) {
  static char s[512];
  snprintf(s, 512, "(%g,%g)->(%g,%g)", r.xmin, r.ymin, r.xmax, r.ymax);
  return s;
}
