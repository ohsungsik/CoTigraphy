// \file pch.hpp
// \last_updated 2025-06-09
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <pch-common.hpp>

#pragma warning(disable: 26446)	// Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26481)	// Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26485)	// Expression 'argv': No array to pointer decay (bounds.3).
#pragma warning(disable: 26490)	// Don't use reinterpret_cast (type.1).

#define CURL_STATICLIB
#define HAVE_CONFIG_H
#define BUILDING_LIBCURL
