// \file pch.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"

#include <LinkLibraries.hpp>


// win32 라이브러리
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")


// 외부 라이브러리
#pragma comment(lib, "curl" STATIC_LIBRARY_SUFFIX)
#pragma comment(lib, "webp" STATIC_LIBRARY_SUFFIX)
