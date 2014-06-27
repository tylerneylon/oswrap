// img.m
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_mac
//

#include "img.h"


// Internal functions.

// The function CreateARGBBitmapContext was copied from here:
// https://developer.apple.com/library/mac/qa/qa1509/_index.html

static CGContextRef CreateARGBBitmapContext (CGImageRef inImage) {
  CGContextRef    context = NULL;
  CGColorSpaceRef colorSpace;
  int             bitmapByteCount;
  int             bitmapBytesPerRow;
  
  // Get image width, height. We'll use the entire image.
  size_t pixelsWide = CGImageGetWidth(inImage);
  size_t pixelsHigh = CGImageGetHeight(inImage);
  
  // Declare the number of bytes per row. Each pixel in the bitmap in this
  // example is represented by 4 bytes; 8 bits each of red, green, blue, and
  // alpha.
  bitmapBytesPerRow   = (int)(pixelsWide * 4);
  bitmapByteCount     = (int)(bitmapBytesPerRow * pixelsHigh);
  
  // Use the generic RGB color space.
  colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
  if (colorSpace == NULL) {
    fprintf(stderr, "Error allocating color space\n");
    return NULL;
  }
  
  // Create the bitmap context. We want pre-multiplied ARGB, 8-bits
  // per component. Regardless of what the source image format is
  // (CMYK, Grayscale, and so on) it will be converted over to the format
  // specified here by CGBitmapContextCreate.
  context = CGBitmapContextCreate (NULL,
                                   pixelsWide,
                                   pixelsHigh,
                                   8,      // bits per component
                                   bitmapBytesPerRow,
                                   colorSpace,
                                   (enum CGBitmapInfo)kCGImageAlphaPremultipliedLast);
  if (context == NULL) {
    fprintf(stderr, "Context not created!\n");
  }
  
  // Make sure and release colorspace before returning
  CGColorSpaceRelease(colorSpace);
  
  return context;
}


// Public functions.

draw__Bitmap img__new_bitmap(const char *path, int *w, int *h) {
  assert(w && h);  // These are both expected to be valid pointers.
  
  NSString *nsstr_path = [NSString stringWithUTF8String:path];
  CFURLRef urlRef = (__bridge CFURLRef)[NSURL fileURLWithPath:nsstr_path];
  CGImageSourceRef myImageSourceRef = CGImageSourceCreateWithURL(urlRef, NULL);
  CGImageRef imageRef = CGImageSourceCreateImageAtIndex(myImageSourceRef, 0, NULL);
  
  // TODO In the future, I suspect I can avoid the drawing step and simply use the
  //      data directly using the following two lines. I think the point of the drawing
  //      code is simply to standardize the colorspace.
  //CFDataRef dataRef = CGDataProviderCopyData(CGImageGetDataProvider(imageRef));
  //unsigned char *data = CFDataGetBytePtr(dataRef);
  
  // Create the bitmap context
  CGContextRef ctx = CreateARGBBitmapContext(imageRef);
  if (ctx == NULL) {
    fprintf(stderr, "Error creating CG context\n");
    return NULL;
  }
  
  // Get image width, height. We'll use the entire image.
  *w = (int)CGImageGetWidth(imageRef);
  *h = (int)CGImageGetHeight(imageRef);
  CGRect rect = {{0, 0}, {*w, *h}};
  
  CGContextTranslateCTM(ctx, 0, *h);
  CGContextScaleCTM    (ctx, 1.0, -1.0);
  
  // Draw the image to the bitmap context. Once we draw, the memory
  // allocated for the context for rendering will then contain the
  // raw image data in the specified color space.
  CGContextDrawImage(ctx, rect, imageRef);
  
  CGImageRelease(imageRef);
  CFRelease(myImageSourceRef);
  
  return ctx;
}
