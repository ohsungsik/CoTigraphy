// \file main.cpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"

#include "CommandLineParser.hpp"
#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"

int wmain(int argc, wchar_t* argv[])
{
    CoTigraphy::MemoryLeakDetector::Initialize();
    CoTigraphy::HandleLeakDetector::Initialize();

    const CoTigraphy::CommandLineParser commandLineParser;
    const CoTigraphy::Error error = commandLineParser.Parse(argc, argv);
    if (error.IsFailed())
    {
        ASSERT(error.IsSucceeded());
        return static_cast<int>(error.GetErrorCode()); // ErrorCode는 long 타입으로 int 타입과 표현 범위가 동일하기 떄문에 int로 형변환이 가능하다
    }

    return 0;
}
