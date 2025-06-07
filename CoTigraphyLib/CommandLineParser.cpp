// \file CommandLineParser.cpp
// \last_updated 2025-06-08
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CommandLineParser.hpp"

#include <iomanip>
#include <iostream>
#include <unordered_set>

#include "VersionInfo.hpp"

namespace CoTigraphy
{
    CommandLineParser::CommandLineParser() noexcept
    = default;

    CommandLineParser::~CommandLineParser()
    = default;

    Error CommandLineParser::AddOption(const CommandLineOption& option)
    {
        if (option.IsValid() == false)
        {
            ASSERT(false); // 유효하지 않은 입력
            return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
        }

        if (mLookup.count(option.mName) || mLookup.count(option.mShortName))
        {
            ASSERT(false); // 중복된 명령
            return MAKE_ERROR(eErrorCode::CommandLineArgumentsAlreadyExists);
        }


        // Keep a copy for help ordering
        mOptions.emplace_back(option);
        const size_t idx = mOptions.size() - 1;

        // Map names to the index
        mLookup[mOptions[idx].mName] = idx;
        mLookup[mOptions[idx].mShortName] = idx;
        return MAKE_ERROR(eErrorCode::Succeeded);
    }

    Error CommandLineParser::Parse(const int argc, wchar_t* argv[])
    {
        if (argc < 1 || argv == nullptr)
        {
            ASSERT(false); // 명령줄 인자를 찾을 수 없음
            return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
        }

        std::vector<std::wstring> args;
        args.reserve(static_cast<size_t>(argc) - 1);
        for (int i = 1; i < argc; ++i)
            args.emplace_back(argv[i]);

        return Parse(args);
    }

    Error CommandLineParser::Parse(const std::vector<std::wstring>& args)
    {
        // 1) Handle exit-causing options first, regardless of position
        for (size_t idx = 0; idx < args.size(); ++idx)
        {
            auto it = mLookup.find(args[idx]);
            if (it != mLookup.end() && mOptions[it->second].mCausesExit)
            {
                // Process and then exit immediately
                Error err = ProcessToken(args, idx);
                return err.IsFailed() ? err : MAKE_ERROR(eErrorCode::EarlyExit);
            }
        }

        // 2) No exit-causing option found: normal parsing
        size_t idx = 0;
        while (idx < args.size())
        {
            Error err = ProcessToken(args, idx);
            if (err.IsFailed())
                return err;
            ++idx;
        }
        return MAKE_ERROR(eErrorCode::Succeeded);
    }

    std::wostream& CommandLineParser::PrintHelpTo(std::wostream& os) const
    {
        std::wstringstream stringstream;
        stringstream << L"CoTigraphy " << VERSION_STRING_WIDE << L"\n"
            << L"Copyright (C) 2025. Oh Sungsik. All rights reserved.\n\n"
            << L"Usage:\n  CoTigraphy [options]\n\n"
            << L"Available options:\n";

        for (const auto& opt : mOptions)
        {
            std::wostringstream oss;
            oss << opt.mShortName << L", " << opt.mName;
            if (opt.mRequiresValue)
                oss << L" <value>";

            stringstream << L"  " << std::setw(28) << std::left << oss.str() << opt.mDescription << L"\n";
        }

        stringstream << L"\nFor more information, visit: https://github.com/ohsungsik/CoTigraphy\n";
        os << stringstream.str();
        return os;
    }

    Error CommandLineParser::ProcessToken(const std::vector<std::wstring>& args, size_t& index)
    {
        const auto& token = args[index];
        auto it = mLookup.find(token);
        if (it == mLookup.end())
            return MAKE_ERROR(eErrorCode::CommandLineArgumentNotFound);
        const CommandLineOption& option = mOptions[it->second];

        // Determine and consume value if required
        std::wstring value;
        if (option.mRequiresValue)
        {
            if (index + 1 >= args.size())
                return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
            value = args[index + 1];
            ++index;
        }

        // Dispatch handler
        if (option.mHandler)
            option.mHandler(value);

        return MAKE_ERROR(eErrorCode::Succeeded);
    }
}
