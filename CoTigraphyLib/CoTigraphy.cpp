// \file CoTigraphy.cpp
// \last_updated 2025-06-11
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CoTigraphy.hpp"

#include <iostream>
#include <shellapi.h>
#include <string_view>

#include "GitHubContributionCalendarClient.hpp"
#include "GridCanvas.hpp"
#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"
#include "VersionInfo.hpp"
#include "WebPWriter.hpp"

namespace CoTigraphy
{
    float ComputeScaleExponential(int cellIndex, int totalCells)
    {
        constexpr float headScale = 1.0f;
        constexpr float tailScale = 0.5f;

        const float t = static_cast<float>(cellIndex) / static_cast<float>(totalCells - 1);
        return headScale * std::pow(tailScale / headScale, t);
    }

    void TEST_WEBP(const ContributionInfo& grid)
    {
        constexpr int cellSize = 10; // 각 칸 크기 (px)
        constexpr int cellMargin = 3; // 칸 간격 (px)
        constexpr int daysPerWeek = 7; // Sunday~Saturday (7 rows)

        const size_t width = grid.mColoumCount * (cellSize + cellMargin) - cellMargin;
        constexpr size_t height = (daysPerWeek + 1) * (cellSize + cellMargin) - cellMargin;

        GridCanvasContext context;
        context.mWidth = static_cast<int>(width);
        context.mHeight = height;
        context.mCellSize = cellSize;
        context.mCellMargin = cellMargin;

        GridCanvas gridCanvas;
        gridCanvas.Create(context);
        gridCanvas.ClearTo(RGB(0x01, 0x04, 0x09));

        for (const auto& cell : grid.mContributionCells)
        {
            const size_t y = cell.mYIndex + 1;
            const size_t x = cell.mXIndex;

            gridCanvas.DrawRect((unsigned int)y, (unsigned int)x, cell.mColor);
        }

        constexpr COLORREF wormBaseColor = RGB(0xFF, 0xA5, 0x00);
        for (int i = 0; i < 4; ++i)
        {
            const float scale = ComputeScaleExponential(i, 4);
            gridCanvas.DrawRectScale(0, i, scale, wormBaseColor);
        }

        WebPWriter webPWriter;
        const std::wstring fileName = L"animated.webp";
        webPWriter.SaveToFile(fileName, gridCanvas.GetBuffer(), gridCanvas.GetBufferSize(), context.mWidth,
                              context.mHeight);
    }

    Error Initialize()
    {
        CoTigraphy::MemoryLeakDetector::Initialize();
        CoTigraphy::HandleLeakDetector::Initialize();

        std::wstring githubToken;

        CoTigraphy::CommandLineParser commandLineParser;
        Error error = SetupCommandLineParser(commandLineParser, githubToken);
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

        GitHubContributionCalendarClient contributionCalendarClient;
        contributionCalendarClient.Initialize();
        contributionCalendarClient.SetAccessToken(githubToken);
        ContributionInfo response = contributionCalendarClient.FetchContributionInfo();

        TEST_WEBP(response);


        contributionCalendarClient.Uninitialize();

        return MAKE_ERROR(eErrorCode::Succeeded);
    }

    void Uninitialize() noexcept
    {
    }

    Error SetupCommandLineParser(CoTigraphy::CommandLineParser& commandLineParser, std::wstring& githubToken)
    {
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

        return MAKE_ERROR(eErrorCode::Succeeded);
    }
} // namespace CoTigraphy
