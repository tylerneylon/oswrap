// draw.c
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
//

#include "draw.h"

#include "cbit.h"


// Internal globals.

static CGColorSpaceRef generic_rgb_colorspace = NULL;
static draw__Bitmap    ctx                    = NULL;
static draw__Font      font                   = NULL;
static draw__Color     font_color             = NULL;


// Internal functions.

static void init_if_needed() {
  static int is_initialized = false;
  if (is_initialized) return;

  generic_rgb_colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
  if (generic_rgb_colorspace == NULL) {
    fprintf(stderr, "Error allocating generic rgb color space.\n");
  }

  is_initialized = true;
}

#define cg_rect_from_xy(rect) \
  ((CGRect) { { rect.xmin, rect.ymin }, { xy__width(rect), xy__height(rect) } })


// Bitmaps.

draw__Bitmap draw__new_bitmap(int w, int h) {
  init_if_needed();

  int bytes_per_row = w * 4;
  CGBitmapInfo bitmap_info = (CGBitmapInfo)kCGImageAlphaPremultipliedLast;
  draw__Bitmap bitmap = CGBitmapContextCreate(NULL,   // data
                                              w,
                                              h,
                                              8,      // bits per component
                                              bytes_per_row,
                                              generic_rgb_colorspace,
                                              bitmap_info);  // premult. alpha

  // Make (0, 0) correspond to the lower-left corner.
  CGContextTranslateCTM(bitmap, 0, h);
  CGContextScaleCTM(bitmap, 1.0, -1.0);

  return bitmap;
}

void draw__delete_bitmap(draw__Bitmap bitmap) {
  CGContextRelease(bitmap);
}

void draw__set_bitmap(draw__Bitmap bitmap) {
  ctx = bitmap;
}

void *draw__get_bitmap_data(draw__Bitmap bitmap) {
  return CGBitmapContextGetData(bitmap);
}

// Fonts and text.

draw__Font draw__new_font(const char *name, int size) {
  CFStringRef font_name = CFStringCreateWithCString(kCFAllocatorDefault,
                                                    name,
                                                    kCFStringEncodingUTF8);
  draw__Font  font      = CTFontCreateWithName(font_name,
                                               size,
                                               NULL);  // transform matrix
  CFRelease  (font_name);
  return      font;
}

void draw__delete_font(draw__Font font) {
  CFRelease(font);
}

void draw__set_font(draw__Font new_font) {
  font = new_font;
}

void draw__set_font_color(draw__Color color) {
  font_color = color;
}

xy__Float draw__string(const char *s, int x, int y, int w, float pos) {
  CFStringRef string = CFStringCreateWithCString(kCFAllocatorDefault, s,
                                                 kCFStringEncodingUTF8);

  if (font == NULL || font_color == NULL) {
    fprintf(stderr,
            "Error in %s: need both font & font_color to be non-NULL.\n",
            __FUNCTION__);
    return x;
  }

  CFStringRef keys[] = { kCTFontAttributeName,
                         kCTForegroundColorAttributeName };
  CFTypeRef values[] = { font, font_color };

  CFDictionaryRef attributes =
  CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
                     (const void**)&values, sizeof(keys) / sizeof(keys[0]),
                     &kCFTypeDictionaryKeyCallBacks,
                     &kCFTypeDictionaryValueCallBacks);

  CFAttributedStringRef attrString =
      CFAttributedStringCreate(kCFAllocatorDefault, string, attributes);
  CFRelease(string);
  CFRelease(attributes);

  CTLineRef line = CTLineCreateWithAttributedString(attrString);

  // This position set is needed to get a useful x_pos value on the next line.
  CGContextSetTextPosition(ctx, x, y);
  double x_pos = CTLineGetPenOffsetForFlush(line, pos, w);

  CGFloat descent;
  double line_width = CTLineGetTypographicBounds(line, NULL, &descent, NULL);

  CGContextSetTextPosition(ctx, x + x_pos, y + descent);

  CTLineDraw(line, ctx);
  CFRelease(line);

  return x + x_pos + line_width;
}

// Colors.

draw__Color draw__new_color(double r, double g, double b) {
  return CGColorCreateGenericRGB(r, g, b, 1.0);
}

void draw__delete_color(draw__Color color) {
  CGColorRelease(color);
}

void draw__rgb_fill_color(double r, double g, double b) {
  CGContextSetRGBFillColor(ctx, r, g, b, 1.0);
}

void draw__rgb_stroke_color(double r, double g, double b) {
  CGContextSetRGBStrokeColor(ctx, r, g, b, 1.0);
}


// Shapes and lines.

void draw__fill_rect(xy__Rect rect) {
  CGContextFillRect(ctx, cg_rect_from_xy(rect));
}

void draw__stroke_rect(xy__Rect rect) {
  CGContextStrokeRect(ctx, cg_rect_from_xy(rect));
}

void draw__line(xy__Float x1, xy__Float y1, xy__Float x2, xy__Float y2) {
  CGContextMoveToPoint   (ctx, x1, y1);
  CGContextAddLineToPoint(ctx, x2, y2);
  CGContextStrokePath    (ctx);
}

