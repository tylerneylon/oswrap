// crypt.h
//
// Home repo: https://github.com/tylerneylon/oswrap in oswrap_windows
//
// Cross-platform wrappers for security-related functions 
// such as SHA1.
//
// On windows, use of this library requires linking with
// advapi32.lib or dynamically loading advapi32.dll.
//

#pragma once

char *crypt__sha1(const char *input);
