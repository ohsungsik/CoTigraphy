// \file Assert.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <sstream>
#include <string>

#include "Error.hpp"

#if defined(_DEBUG)

#define ASSERT(expr)                                                                        \
    do {                                                                                    \
        if (!(expr))                                                                        \
        {                                                                                   \
            std::wostringstream oss;                                                        \
            oss << L"[ASSERT FAILED] " << L#expr << L"\n"                                   \
                << L"File: " << __FILEW__ << L"\n"                                          \
                << L"Line: " << __LINE__ << L"\n";                                          \
            ::OutputDebugStringW(oss.str().c_str());                                        \
            if (::IsDebuggerPresent()) __debugbreak();                                      \
        }                                                                                   \
    } while (0)

#define ASSERT_MSG(expr, msg)                                                               \
    do {                                                                                    \
        if (!(expr))                                                                        \
        {                                                                                   \
            std::wostringstream oss;                                                        \
            oss << L"[ASSERT FAILED] " << L#expr << L"\n"                                   \
                << L"Message: " << (msg) << L"\n"                                           \
                << L"File: " << __FILEW__ << L"\n"                                          \
                << L"Line: " << __LINE__ << L"\n";                                          \
            ::OutputDebugStringW(oss.str().c_str());                                        \
            if (::IsDebuggerPresent()) __debugbreak();                                      \
        }                                                                                   \
    } while (0)

#else
#define ASSERT(expr)            ((void)0)
#define ASSERT_MSG(expr, msg)   ((void)0)
#endif
