// \file VersionInfo.hpp
// \last_updated 2025-06-08
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include "BuildInfo.hpp"

// 프로그램 버전 정보
// 버전 정보는 Semantic Versioning 2.0.0 규칙을 따른다.
// https://semver.org/
#define VERSION_INFO_MAJOR 0
#define VERSION_INFO_MINOR 0
#define VERSION_INFO_PATCH 0
#define VERSION_INFO_BUILD BUILD_NUMBER

#define MULTIBYTE_STRINGIFY(x) #x
#define TO_MULTIBYTE_STRING(x) MULTIBYTE_STRINGIFY(x)

#define WIDE_STRINGIFY(x) L#x
#define TO_WIDE_STRING(x) WIDE_STRINGIFY(x)

#define VERSION_STRING_MULTIBYTE \
    TO_MULTIBYTE_STRING(VERSION_INFO_MAJOR) "." \
    TO_MULTIBYTE_STRING(VERSION_INFO_MINOR) "." \
    TO_MULTIBYTE_STRING(VERSION_INFO_PATCH) "." \
    TO_MULTIBYTE_STRING(VERSION_INFO_BUILD)

#define VERSION_STRING_WIDE \
    TO_WIDE_STRING(VERSION_INFO_MAJOR) L"." \
    TO_WIDE_STRING(VERSION_INFO_MINOR) L"." \
    TO_WIDE_STRING(VERSION_INFO_PATCH) L"." \
    TO_WIDE_STRING(VERSION_INFO_BUILD)

#define VERSION_STRING VERSION_STRING_WIDE


// Copyright
#define COPYRIGHT_TEXT Copyright (C) 2025. Oh Sungsik. All rights reserved.

#define COPYRIGHT_MULTIBYTE TO_MULTIBYTE_STRING(COPYRIGHT_TEXT)
#define COPYRIGHT_WIDE      TO_WIDE_STRING(COPYRIGHT_TEXT)
