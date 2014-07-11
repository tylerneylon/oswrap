// crypt.c
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
//

#include "crypt.h"

#include <CommonCrypto/CommonDigest.h>

#include <stdio.h>
#include <string.h>

#define sha1_ascii_len (2 * CC_SHA1_DIGEST_LENGTH + 1)

char *crypt__sha1(const char *input) {
  static char hex_out[sha1_ascii_len];
  uint8_t digest[CC_SHA1_DIGEST_LENGTH];
  CC_SHA1(input, (int)strlen(input), digest);

  char *s     = hex_out;
  char *s_end = hex_out + sha1_ascii_len;

  for (int i = 0; i < CC_SHA1_DIGEST_LENGTH; ++i) {
    s += snprintf(s, (s_end - s), "%02X", digest[i]);
  }

  return hex_out;
}
