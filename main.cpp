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
    if (commandLineParser.Parse(argc, argv) == false)
    {
        return 1;
    }

    return 0;
}
