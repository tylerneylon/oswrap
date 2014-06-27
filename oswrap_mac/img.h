// img.h
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_mac
//
// Functions for loading image files into bitmaps.
//
// On windows, using these functions requires linking with
// either gdiplus.lib or gdiplus.dll.
//

#pragma once

#include "draw.h"

// Release the returned object by calling draw__delete_bitmap on it.
draw__Bitmap img__new_bitmap(const char *path, int *w, int *h);
