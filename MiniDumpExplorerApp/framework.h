#pragma once

#include <Unknwn.h> // To enable support for non-WinRT interfaces, unknwn.h must be included before any C++/WinRT headers.

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <vector>
#include <algorithm>
#include <iterator>

#include <winrt/Windows.System.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/MiniDumpExplorer.h>

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>

// C RunTime Header Files
#include <cstdlib>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
