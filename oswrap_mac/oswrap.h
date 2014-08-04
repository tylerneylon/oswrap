// oswrap.h
//
// https://github.com/tylerneylon/oswrap in oswrap_mac
//
// One header to include them all.
// Include this header to make all functions
// of oswrap available at once.
//

#pragma once

// Make it easy to include oswrap from C or C++.

#ifdef __cplusplus
extern "C" {
#endif


#include "audio.h"
#include "crypt.h"
#include "cursor.h"
#include "dbg.h"
#include "draw.h"
#include "file.h"
#include "img.h"
#include "io.h"
#include "now.h"
#ifdef _WIN32
#include "winutil.h"
#endif
#include "xy.h"


#ifdef __cplusplus
}
#endif
