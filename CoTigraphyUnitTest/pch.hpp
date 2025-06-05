
#pragma once

#include <pch-common.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


// googletest 라이브러리에서 발생하는 오류 무시
#pragma warning (disable: 26409)	// Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11).
#pragma warning (disable: 26426)	// Global initializer calls a non-constexpr function (i.22).
#pragma warning (disable: 26440)	// Function 'command_line_parser_Init_Test::TestBody' can be declared 'noexcept' (f.6).
#pragma warning (disable: 26455)	// Default constructor should not throw. Declare it 'noexcept' (f.6).
