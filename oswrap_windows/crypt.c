#include "crypt.h"

#include <windows.h>
#include <wincrypt.h>

#include "winutil.h"

#define ascii_len 64


// Internal functions.

char *error(const char *msg) {
  OutputDebugString(msg);

  DWORD last_error = GetLastError();
  char err_msg[512];
  if (last_error < 200) {
    snprintf(err_msg, 512, "Error code = %d.\n", last_error);
  } else {
    snprintf(err_msg, 512, "Error code = 0x%X.\n", last_error);
  }
  OutputDebugString(err_msg);
  return NULL;
}

// Public functions.

char *crypt__sha1(const char *input) {

  HCRYPTPROV crypt_provider;
  HCRYPTHASH hash;

  BOOL worked;

  worked = CryptAcquireContext(
    &crypt_provider,
    NULL,  // pszContainer
    NULL,  // pszProvider
    PROV_RSA_FULL,
    CRYPT_VERIFYCONTEXT);    // dwFlags

  if (!worked) return error("Error from CryptAcquireContext.\n");

  worked = CryptCreateHash(
    crypt_provider,
    CALG_SHA1,
    0,  // hKey
    0,  // dwFlags
    &hash);

  if (!worked) return error("Error from CryptCreateHash.\n");

  worked = CryptHashData(
    hash,
    input,
    strlen(input),
    0);  // dwFlags

  if (!worked) { return error("Error from CryptHashData.\n"); }

  DWORD hash_len;
  DWORD hash_len_size = sizeof(hash_len);

  worked = CryptGetHashParam(
    hash,
    HP_HASHSIZE,
    (BYTE *)&hash_len,
    &hash_len_size,
    0);  // dwFlags

  if (!worked) return error("Error from CryptGetHashParam.\n");

  char *hash_val = malloc(hash_len);

  if (!hash_val) return error("Error in sha1: malloc failed.\n");

  worked = CryptGetHashParam(
    hash,
    HP_HASHVAL,
    hash_val,
    &hash_len,
    0);  // dwFlags

  if (!worked) return error("Error from CryptGetHashParam.\n");

  // It's annoying that the Crypt* functions don't
  // supply a constant with the SHA1 output hash size.
  static char hex[ascii_len];
  char *s     = hex;
  char *s_end = s + ascii_len;

  for (unsigned int i = 0; i < hash_len; ++i) {
    s += snprintf(s, (s_end - s), "%02X", (unsigned char)hash_val[i]);
  }

  free(hash_val);

  return hex;
}
