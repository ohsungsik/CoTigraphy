// \file CoTigraphy.cpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CoTigraphy.hpp"

#include <iostream>
#include <shellapi.h>
#include <string_view>

#include <webp/encode.h>
#include <webp/mux.h>
#include <webp/mux_types.h>

#include "GitHubContributionCalendarClient.hpp"
#include "GridCanvas.hpp"
#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"
#include "VersionInfo.hpp"
#include "WebPWriter.hpp"

namespace CoTigraphy
{
	struct ContributionCell
	{
		int weekIndex;
		int weekday; // 0 = Sunday, 1 = Monday, ..., 6 = Saturday
		uint64_t count;
		COLORREF color; // RGBA
	};

	// GitHub는 보통 "Sunday" 시작
	int GetWeekIndex(const std::tm& date, const std::tm& startDate)
	{
		// days since startDate
		std::time_t tDate = std::mktime(const_cast<std::tm*>(&date));
		std::time_t tStart = std::mktime(const_cast<std::tm*>(&startDate));

		int daysDiff = static_cast<int>(std::difftime(tDate, tStart) / (60 * 60 * 24));
		return daysDiff / 7;
	}

	int GetWeekday(const std::tm& date)
	{
		// std::tm.tm_wday: 0 = Sunday, 1 = Monday, ..., 6 = Saturday
		return date.tm_wday;
	}

	COLORREF HexToColorRef(const std::wstring& hex)
	{
		POSTCONDITION(hex.length() == 7);
		POSTCONDITION(hex[0] == L'#');

		const unsigned int r = std::stoi(hex.substr(1, 2), nullptr, 16);
		const unsigned int g = std::stoi(hex.substr(3, 2), nullptr, 16);
		const unsigned int b = std::stoi(hex.substr(5, 2), nullptr, 16);

		return RGB(r, g, b); // Macro: ((BYTE)(r) | ((BYTE)(g) << 8) | ((BYTE)(b) << 16))
	}

	float ComputeScaleExponential(int cellIndex, int totalCells)
	{
		constexpr float headScale = 1.0f;
		constexpr float tailScale = 0.5f;

		const float t = static_cast<float>(cellIndex) / static_cast<float>(totalCells - 1);
		return headScale * std::pow(tailScale / headScale, t);
	}

	void TEST_WEBP(const std::vector<ContributionCell>& grid, int maxWeeks)
	{
		constexpr int cellSize = 10; // 각 칸 크기 (px)
		constexpr int cellMargin = 3; // 칸 간격 (px)
		constexpr int daysPerWeek = 7; // Sunday~Saturday (7 rows)

		const int width = maxWeeks * (cellSize + cellMargin) - cellMargin;
		constexpr int height = (daysPerWeek + 1) * (cellSize + cellMargin) - cellMargin;

		GridCanvasContext context;
		context.mWidth = width;
		context.mHeight = height;
		context.mCellSize = cellSize;
		context.mCellMargin = cellMargin;

		GridCanvas gridCanvas;
		gridCanvas.Create(context);
		gridCanvas.ClearTo(RGB(0x01, 0x04, 0x09));

		for (const auto& cell : grid)
		{
			const unsigned int y = cell.weekday + 1;
			const unsigned int x = cell.weekIndex;

			gridCanvas.DrawRect(y, x, cell.color);
		}

		const COLORREF wormBaseColor = HexToColorRef(L"#FFA500");
		for (int i = 0; i < 4; ++i)
		{
			const float scale = ComputeScaleExponential(i, 4);
			gridCanvas.DrawRectScale(0, i, scale, wormBaseColor);
		}

		WebPWriter webPWriter;
		const std::wstring fileName = L"animated.webp";
		webPWriter.SaveToFile(fileName, gridCanvas.GetBuffer(), gridCanvas.GetBufferSize(), context.mWidth, context.mHeight);
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
		std::vector<ContributionDay> response = contributionCalendarClient.Get();

		std::vector<ContributionCell> grid;
		std::tm startDate = response.front().mDate; // 첫 날짜 (시작점)

		// 모든 데이터를 grid로 변환
		int maxWeeks = 0;
		for (const auto& day : response)
		{
			ContributionCell cell;
			cell.weekIndex = GetWeekIndex(day.mDate, startDate);
			cell.weekday = GetWeekday(day.mDate);
			cell.count = day.mCount;
			cell.color = HexToColorRef(day.mColor);

			maxWeeks = max(maxWeeks, cell.weekIndex);

			grid.push_back(cell);
		}

		TEST_WEBP(grid, maxWeeks);


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
				::TerminateProcess(::GetCurrentProcess(), 0);                       \
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
