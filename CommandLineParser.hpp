// \file CommandLineParser.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <string>
#include <vector>

namespace CoTigraphy
{
    class CommandLineParser final
    {
    public:
        explicit CommandLineParser() noexcept;
        CommandLineParser(const CommandLineParser& other) = delete;
        CommandLineParser(CommandLineParser&& other) = delete;

        CommandLineParser& operator=(const CommandLineParser& rhs) = delete;
        CommandLineParser& operator=(CommandLineParser&& rhs) = delete;

        ~CommandLineParser();

        bool Parse(const int argc, wchar_t* argv[]) const noexcept;
        bool Parse(const std::vector<std::wstring>& commandLineArguments) const noexcept;
    };
}
