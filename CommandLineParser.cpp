// \file CommandLineParser.cpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CommandLineParser.hpp"

namespace CoTigraphy
{
    CommandLineParser::CommandLineParser() noexcept
    = default;

    CommandLineParser::~CommandLineParser()
    = default;

    bool CommandLineParser::Parse(const int argc, wchar_t* argv[]) const noexcept
    {
        ASSERT(argc == 0 || argv != nullptr);

        std::vector<std::wstring> args;
        args.reserve(argc);
        for (int i = 1; i < argc; ++i)
        {
            args.emplace_back(argv[i]);
        }

        return Parse(args);
    }

    bool CommandLineParser::Parse(const std::vector<std::wstring>& commandLineArguments) const noexcept
    {
        UNREFERENCED_PARAMETER(commandLineArguments);
        return false;
    }
}
