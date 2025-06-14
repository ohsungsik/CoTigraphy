// \file CoTigraphy.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CoTigraphy.hpp"

#include <iostream>
#include <shellapi.h>
#include <string_view>

#include "CommandLineParser.hpp"
#include "GitHubContributionCalendarClient.hpp"
#include "GridCanvas.hpp"
#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"
#include "VersionInfo.hpp"
#include "WebPWriter.hpp"
#include "Worm.hpp"

namespace CoTigraphy
{
    Error Initialize(_Out_opt_ std::wstring& githubToken, _Out_opt_ std::wstring& userName)
    {
        CoTigraphy::MemoryLeakDetector::Initialize();
        CoTigraphy::HandleLeakDetector::Initialize();

        githubToken.clear();
        userName.clear();

        CoTigraphy::CommandLineParser commandLineParser;
        Error error = SetupCommandLineParser(commandLineParser, githubToken, userName);
        if (error.IsFailed())
            return error;

        int argc = 0;
        LPWSTR* const argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        error = commandLineParser.Parse(argc, argv);
        LocalFree(argv);
        if (error.IsFailed())
        {
            ASSERT(error.IsSucceeded()); // Commandl line arguments 파싱 실패
            return error;
        }

        return MAKE_ERROR(eErrorCode::Succeeded);
    }

    Error SetupCommandLineParser(_In_ CoTigraphy::CommandLineParser& commandLineParser,
                                 _Out_opt_ std::wstring& githubToken,
                                 _Out_opt_ std::wstring& userName)
    {
        githubToken.clear();
        userName.clear();

        Error error = commandLineParser.AddOption(CommandLineOption{
            L"--help", // mName
            L"-h", // mShortName
            L"Show help message", // mDescription
            false, // mRequiresValue
            true, // mCausesExit
            [&](const std::wstring_view&)
            {
                commandLineParser.PrintHelpTo(std::wcout);
                ::TerminateProcess(::GetCurrentProcess(), 0);
            }
        });
        if (error.IsFailed())
        {
            ASSERT(error.IsSucceeded());
            return error;
        }

        error = commandLineParser.AddOption(CommandLineOption{
            L"--version", // mName
            L"-v", // mShortName
            L"Show program version", // mDescription
            false, // mRequiresValue
            false, // mCausesExit
            [&](const std::wstring_view& value) // mHandler
            {
                UNREFERENCED_PARAMETER(value);

                std::wstring versionInfo = L"Version: ";
                versionInfo += VERSION_STRING_WIDE;
                versionInfo += L"\n";
                OutputDebugStringW(versionInfo.c_str());
            }
        });
        if (error.IsFailed())
        {
            ASSERT(error.IsSucceeded());
            return error;
        }

        error = commandLineParser.AddOption(CommandLineOption{
            L"--token", // mName
            L"-t", // mShortName
            L"Github personal access token", // mDescription
            true, // mRequiresValue
            false, // mCausesExit
            [&](const std::wstring_view& value) // mHandler
            {
                githubToken = value;
            }
        });
        if (error.IsFailed())
        {
            ASSERT(error.IsSucceeded());
            return error;
        }

        error = commandLineParser.AddOption(CommandLineOption{
            L"--userName", // mName
            L"-n", // mShortName
            L"Github user name", // mDescription
            true, // mRequiresValue
            false, // mCausesExit
            [&](const std::wstring_view& value) // mHandler
            {
                userName = value;
            }
        });
        if (error.IsFailed())
        {
            ASSERT(error.IsSucceeded());
            return error;
        }

        return MAKE_ERROR(eErrorCode::Succeeded);
    }

    Error Run(_In_ const std::wstring& githubToken, _In_ const std::wstring& userName)
    {
        GitHubContributionCalendarClient contributionCalendarClient;
        contributionCalendarClient.Initialize();
        contributionCalendarClient.SetAccessToken(githubToken);

        const std::wstring reuiqredFields = L"date contributionCount color"; // 필요한 field
        const GridData gridData = contributionCalendarClient.FetchContributionInfo(userName, reuiqredFields);
        contributionCalendarClient.Uninitialize();

        constexpr int cellSize = 10; // 각 칸 크기 (px)
        constexpr int cellMargin = 3; // 칸 간격 (px)
        constexpr int daysPerWeek = 7; // Sunday~Saturday (7 rows)

        const size_t width = gridData.mWeekCount * (cellSize + cellMargin) - cellMargin;
        constexpr size_t height = daysPerWeek * (cellSize + cellMargin) - cellMargin;

        GridCanvasContext context;
        context.mWidth = static_cast<int>(width);
        context.mHeight = height;
        context.mCellSize = cellSize;
        context.mCellMargin = cellMargin;

        GridCanvas gridCanvas;
        gridCanvas.Create(context);

        Grid grid(gridData);
        Worm worm(grid);

        WebPWriter webPWriter;
        webPWriter.Initialize(context.mWidth, context.mHeight);

        uint64_t currentLevel = 1;
        while (true)
        {
            const bool ret = worm.Move(currentLevel);
            if (ret == false)
            {
                currentLevel++;

                if (currentLevel > gridData.mMaxCount)
                    break;

                continue;
            }

            gridCanvas.Clear(RGB(0x01, 0x04, 0x09));
            gridCanvas.DrawGrid(grid);
            gridCanvas.DrawWorm(worm);

            webPWriter.AddFrame(gridCanvas.GetBuffer());
        }

        const std::wstring fileName = L"animated.webp";
        webPWriter.SaveToFile(fileName);

        return MAKE_ERROR(eErrorCode::Succeeded);
    }
} // namespace CoTigraphy
