#pragma once

#pragma warning(disable:4099) // disable "no pdf was found with linked library"
#pragma message("precompiling engine pch")

#include <cstdio>
#include <cstdarg>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <exception>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <Windows.h>
#undef _WINSOCK2API_
#undef _WINSOCKAPI_
#include <comdef.h>
#include <vector>
#include <queue>
#include <map>
#include <codecvt>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <unordered_map>
#include <wincodec.h>
#include <PxPhysicsAPI.h>
#include <io.h>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <wrl.h>
#include <ppltasks.h>
#include <cmath>
#include <fbxsdk.h>
#include <typeindex>
#include <assert.h>
#include <WinInet.h>
#pragma comment(lib, "wininet")

#include "core/rendering/direct3d.h"

#define PICOJSON_USE_INT64
#include "core/utilities/pico_json.h"
#include "core/utilities/json.hpp"

#include <QtWidgets\qdesktopwidget.h>

#ifdef _DEBUG
#include <bitset>
#endif