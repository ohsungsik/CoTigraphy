// \file CommandLineParser.hpp
// \last_updated 2025-06-08
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <functional>
#include <string>
#include <vector>

namespace CoTigraphy
{
    // Describes a single command-line option
    struct CommandLineOption
    {
        std::wstring mName; // Long name, e.g. "--help"
        std::wstring mShortName; // Short name, e.g. "-h"
        std::wstring mDescription; // Help description
        bool mRequiresValue; // Does this option require a following value?
        bool mCausesExit; // Should parsing stop after this option?
        std::function<void(const std::wstring&)> mHandler; // Called with the option's value (or empty)

        bool IsValid() const noexcept
        {
            return !mName.empty() && !mShortName.empty() && !mDescription.empty();
        }
    };

    class CommandLineParser final
    {
    public:
        explicit CommandLineParser() noexcept;
        CommandLineParser(const CommandLineParser& other) = delete;
        CommandLineParser(CommandLineParser&& other) = delete;

        CommandLineParser& operator=(const CommandLineParser& rhs) = delete;
        CommandLineParser& operator=(CommandLineParser&& rhs) = delete;

        ~CommandLineParser();

        // Registers an option. Returns error if invalid or duplicate.
        Error AddOption(const CommandLineOption& option);

        // Parses argc/argv, skipping argv[0].
        Error Parse(int argc, wchar_t* argv[]);
        // Parses a vector of tokens.
        Error Parse(const std::vector<std::wstring>& args);

        std::wostream& PrintHelpTo(std::wostream& os) const;

    private:
        // Processes a single token at args[index], may consume the next token as value.
        Error ProcessToken(const std::vector<std::wstring>& args, size_t& index);

        // Store options in vector for ordered help output
        std::vector<CommandLineOption> mOptions;
        // Map each name to the index in mOptions (avoids duplicate storage)
        std::unordered_map<std::wstring, size_t> mLookup;
    };
}
