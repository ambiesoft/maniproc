#pragma once

#ifdef MANIPROC_WORK
#define DllExport __declspec( dllexport )
#else
#define DllExport __declspec( dllimport )
#endif
DllExport int workmain(bool bGui);
