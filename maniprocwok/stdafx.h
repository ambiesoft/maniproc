// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define WIN32_NO_STATUS					// Exclude STATUS_XXX
#define _CRT_SECURE_NO_WARNINGS

// Windows Header Files:
#include <windows.h>

#include <string>
#include <iostream>

// TODO: reference additional headers your program requires here
#include <ph.h>

#define DllExport __declspec( dllexport )
#include "../common/Header.h"

#define I18NS(s) (s)
