// draw.c
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_windows
//

#include "draw.h"

#include "cbit.h"
#include "winutil.h"


// Internal types and globals.

// This struct is designed to allow for casting between
// the Bitmap * and HBITMAP * types.
typedef struct {
  HBITMAP bitmap;
  char *  bytes;
  int     x_size;
  int     y_size;
} Bitmap;

// Drawing happens with these objects.
static HDC     active_hdc    = NULL;
static Bitmap *active_bitmap = NULL;

// Handles that are useful for deselecting user-made objects.
static HBITMAP system_bitmap  = NULL;
static HFONT   system_font    = NULL;


// Internal functions.

static intptr_t err_msg(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *str;
  vasprintf(&str, fmt, args);
  va_end(args);

  OutputDebugString(str);

  return 0;
}

static void init_if_needed() {
  static bit is_initialized = false;
  if (is_initialized) return;

  active_hdc = CreateCompatibleDC(NULL);

  if (active_hdc == NULL) err_msg("Error: CreateCompatibleDC failed.\n");

  SetGraphicsMode(active_hdc, GM_ADVANCED);
  SetBkMode(active_hdc, TRANSPARENT);

  is_initialized = true;
}

static void UseObject(HGDIOBJ obj) {
  HGDIOBJ old_obj = SelectObject(active_hdc, obj);
  if (old_obj != obj) DeleteObject(old_obj);
}


// Public functions.

draw__Bitmap draw__new_bitmap(int w, int h) {
  Bitmap *b = malloc(sizeof(Bitmap));

  b->x_size = w;
  b->y_size = h;

  BITMAPINFOHEADER bitmap_header;
  memset(&bitmap_header, 0, sizeof(bitmap_header));
  bitmap_header.biSize        = sizeof(bitmap_header);
  bitmap_header.biWidth       = w;
  bitmap_header.biHeight      = h;
  bitmap_header.biPlanes      = 1;
  bitmap_header.biBitCount    = 32;
  bitmap_header.biCompression = BI_RGB;

  b->bitmap = CreateDIBSection(
    NULL,
    (BITMAPINFO *)&bitmap_header,
    DIB_RGB_COLORS,
    &b->bytes,
    NULL,  // file-mapping object
    0);    // offset to bits in file-mapping object

  if (b->bitmap == NULL) {
    err_msg("Error: CreateDIBSection failed in %s.\n", __FUNCTION__);
    return NULL;
  }

  if (system_bitmap == NULL) {
    init_if_needed();  // Ensure active_hdc != NULL.
    // Keep a handle to the constant 1x1 bitmap object
    // so we may later deselect a bitmap from an hdc.
    system_bitmap = SelectObject(active_hdc, b->bitmap);
  }

  return (draw__Bitmap)b;
}

void draw__delete_bitmap(draw__Bitmap bitmap) {
  Bitmap *b = (Bitmap *)bitmap;

  // Ensure the bitmap is not currently selected.
  HBITMAP current_bitmap = (HBITMAP)GetCurrentObject(active_hdc, OBJ_BITMAP);
  if (current_bitmap == b->bitmap) SelectObject(active_hdc, system_bitmap);
  DeleteObject(b->bitmap);
}

void draw__set_bitmap(draw__Bitmap bitmap) {
  init_if_needed();

  Bitmap *b = (Bitmap *)bitmap;
  UseObject(b->bitmap);

  // Use a bottom-up coordinate system; (0, 0) is the lower-left corner.
  XFORM yflip = { 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, (FLOAT)(b->y_size - 1) };
  SetWorldTransform(active_hdc, &yflip);

  active_bitmap = b;
}

void *draw__get_bitmap_data(draw__Bitmap bitmap) {
  Bitmap *b = (Bitmap *)bitmap;
  return b->bytes;
}

// Fonts and text.

draw__Font draw__new_font(const char *name, int size) {
  LOGFONT font_info;
  memset(&font_info, 0, sizeof(font_info));
  strcat_s(font_info.lfFaceName, LF_FACESIZE, name);

  // Negative values in lfHeight are used to specify a traditional font size;
  // positive values indicate the font leading, basically the line height.
  font_info.lfHeight = -1 * size;
  HFONT font = CreateFontIndirect(&font_info);

  if (font == NULL) {
    err_msg("Error: CreateFontIndirect failed in %s.\n", __FUNCTION__);
    return NULL;
  }

  if (system_font == NULL) {
    init_if_needed();  // Ensure active_hdc != NULL.
    // Keep a handle to the system font so we may
    // later deselect a font from an hdc.
    system_font = SelectObject(active_hdc, font);
  }

  return font;
}

void draw__delete_font(draw__Font font) {
  // Ensure the bitmap is not currently selected.
  HFONT current_font = (HFONT)GetCurrentObject(active_hdc, OBJ_FONT);
  if (current_font == font) SelectObject(active_hdc, system_font);
  DeleteObject(font);
}

void draw__set_font(draw__Font font) {
  UseObject(font);
}

void draw__set_font_color(draw__Color color) {
  SetTextColor(active_hdc, color);
}

// Returns the x value at the end of the drawn text.
xy__Float draw__string(
  const char *s,  // The string to draw.
  int x,          // The min x of the drawing box.
  int y,          // The min y of the drawing box.
  int w,          // The width of the drawing box; ignored when left-justified.
  float pos) {    // 0, 0.5, 1 = left, center, or right justified in the box.

  // Temporarily unflip the coordinate system; otherwise text appears upside-down.
  ModifyWorldTransform(active_hdc, NULL, MWT_IDENTITY);  // World transform = identity.
  int ymax = active_bitmap->y_size - 1;
  y = ymax - y;

  SIZE str_size;
  size_t s_len = strlen(s);
  BOOL is_ok = GetTextExtentPoint32(active_hdc, s, s_len, &str_size);

  if (!is_ok) {
    err_msg("Error: GetTextExtentPoint32 failed in %s.\n", __FUNCTION__);
    str_size.cx = 0;
  }

  if (pos != 0 && pos != 0.5 && pos != 1) {
    err_msg("Error: in %s, pos should be 0, 0.5, or 1.\n", __FUNCTION__);
    pos = 0;
  }

  UINT align_modes[] = { TA_LEFT, TA_CENTER, TA_RIGHT };
  SetTextAlign(active_hdc, TA_BOTTOM | align_modes[(int)(pos * 2)]);

  int start_x = x;
  if (pos == 1.0) x += w;
  if (pos == 0.5) x += (int)(w / 2.0);

  is_ok = TextOut(active_hdc, x, y, s, s_len);
  if (!is_ok) { err_msg("Error: TextOut failed in %s.\n", __FUNCTION__); }

  // Re-flip the coordinate system to a bottom-up orientation.
  XFORM yflip = { 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, (FLOAT)(ymax) };
  SetWorldTransform(active_hdc, &yflip);

  return start_x + str_size.cx;
}

// Colors.

draw__Color draw__new_color(double r, double g, double b) {
  return RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));
}

void draw__delete_color(draw__Color color) {
  // Colors on windows are not dynamically allocated, so don't need to be deleted.
}

void draw__rgb_fill_color(double r, double g, double b) {
  HBRUSH brush = CreateSolidBrush(draw__new_color(r, g, b));
  UseObject(brush);
}

void draw__rgb_stroke_color(double r, double g, double b) {
  HPEN pen = CreatePen(PS_SOLID, 1 /* width */, draw__new_color(r, g, b));
  UseObject(pen);
}

// Shapes and lines.

void draw__fill_rect(xy__Rect rect) {
  SaveDC(active_hdc);
  HPEN pen = (HPEN)GetStockObject(NULL_PEN);
  SelectObject(active_hdc, pen);
  Rectangle(active_hdc, (int)rect.xmin, (int)rect.ymin, (int)rect.xmax, (int)rect.ymax);
  RestoreDC(active_hdc, -1 /* restore last saved state */);
}

void draw__stroke_rect(xy__Rect rect) {
  SaveDC(active_hdc);
  HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
  SelectObject(active_hdc, brush);
  Rectangle(active_hdc, (int)rect.xmin, (int)rect.ymin, (int)rect.xmax, (int)rect.ymax);
  RestoreDC(active_hdc, -1 /* restore last saved state */);
}

void draw__line(xy__Float x1, xy__Float y1, xy__Float x2, xy__Float y2) {
  MoveToEx(active_hdc, (int)x1, (int)y1, NULL);
  LineTo(active_hdc, (int)x2, (int)y2);
}
