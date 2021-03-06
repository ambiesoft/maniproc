// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define WIN32_NO_STATUS					// Exclude STATUS_XXX
#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_DCOM

// Windows Header Files:
#include <windows.h>
#include <Objbase.h>
#include <wbemidl.h>
#include <comip.h>
// #include <comutil.h>
#include <comdef.h>

#include <string>
#include <iostream>
#include <vector>
#include <cassert>

// TODO: reference additional headers your program requires here
#include <ph.h>

#include "../../lsMisc/CommandLineParser.h"
#include "../../lsMisc/CreateProcessCommon.h"
#include "../../lsMisc/GetLastErrorString.h"
#include "../../stlScopedClear.h"
#include "../../stdwin32/stdwin32.h"
#include "../../lsMisc/OpenCommon.h"
#include "../../lsMisc/CommandLineString.h"

#define MANIPROC_WORK
#include "../common/Header.h"

#include "Macro.h"
#define I18NS(s) (s)

#ifdef _WIN64
typedef unsigned long long POINTERSIZE;
#else
typedef unsigned long POINTERSIZE;
#endif

#ifdef _DEBUG
#define DASSERT(s) assert(s)
#else
#define DASSERT(s) (void(0))
#endif