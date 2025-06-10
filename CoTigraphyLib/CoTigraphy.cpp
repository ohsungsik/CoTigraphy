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
#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"
#include "VersionInfo.hpp"

namespace CoTigraphy
{
    constexpr int gWidth = 256;
    constexpr int gHeight = 256;
    constexpr int gFrame_count = 10;
    constexpr int gFrame_delay_ms = 100;

    constexpr int gCellSize = 10; // 각 칸 크기 (px)
    constexpr int gCellMargin = 3; // 칸 간격 (px)
    constexpr int gDaysPerWeek = 7; // Sunday~Saturday (7 rows)
    constexpr int gImageHeight = (gDaysPerWeek + 1) * (gCellSize + gCellMargin);

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

    // RGBA 버퍼를 초기화 (검은색 배경)
    void ClearBuffer(uint8_t* rgba)
    {
        memset(rgba, 0, gWidth * gHeight * 4);
    }

    // 사각형 그리기 (RGBA 버퍼에 직접 그리기)
    void DrawRect(uint8_t* rgba, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        for (int j = y; j < y + h; ++j)
        {
            if (j < 0 || j >= gHeight) continue;
            for (int i = x; i < x + w; ++i)
            {
                if (i < 0 || i >= gWidth) continue;
                int index = (j * gWidth + i) * 4;
                rgba[index + 0] = r;
                rgba[index + 1] = g;
                rgba[index + 2] = b;
                rgba[index + 3] = a;
            }
        }
    }

    COLORREF HexToColorRef(const std::wstring& hex)
    {
        POSTCONDITION(hex.length() == 7);
        POSTCONDITION(hex[0] == L'#');

        unsigned int r = std::stoi(hex.substr(1, 2), nullptr, 16);
        unsigned int g = std::stoi(hex.substr(3, 2), nullptr, 16);
        unsigned int b = std::stoi(hex.substr(5, 2), nullptr, 16);

        return RGB(r, g, b); // Macro: ((BYTE)(r) | ((BYTE)(g) << 8) | ((BYTE)(b) << 16))
    }

    void GetColorForCount(uint64_t count, uint8_t& r, uint8_t& g, uint8_t& b)
    {
        if (count == 0)
        {
            r = g = b = 20; // Dark gray
        }
        else if (count < 5)
        {
            r = 0;
            g = 64;
            b = 0;
        }
        else if (count < 10)
        {
            r = 0;
            g = 128;
            b = 0;
        }
        else if (count < 20)
        {
            r = 0;
            g = 192;
            b = 0;
        }
        else
        {
            r = 0;
            g = 255;
            b = 0;
        }
    }

    void DrawCell(uint8_t* rgba, int imgWidth, int x, int y, uint8_t r, uint8_t g, uint8_t b)
    {
        for (int j = 0; j < gCellSize; ++j)
        {
            for (int i = 0; i < gCellSize; ++i)
            {
                int px = x + i;
                int py = y + j;

                if (px < 0 || px >= imgWidth || py < 0 || py >= gImageHeight) continue;

                int index = (py * imgWidth + px) * 4;
                rgba[index + 0] = r;
                rgba[index + 1] = g;
                rgba[index + 2] = b;
                rgba[index + 3] = 255;
            }
        }
    }

    void DrawCell_v2(uint8_t* rgba, int imgWidth, int imgHeight, int centerX, int centerY, int cellSize, uint8_t r,
                     uint8_t g, uint8_t b)
    {
        int halfSize = cellSize / 2;

        for (int j = -halfSize; j < cellSize - halfSize; ++j)
        {
            for (int i = -halfSize; i < cellSize - halfSize; ++i)
            {
                int px = centerX + i;
                int py = centerY + j;

                if (px < 0 || px >= imgWidth || py < 0 || py >= imgHeight) continue;

                int index = (py * imgWidth + px) * 4;
                rgba[index + 0] = r;
                rgba[index + 1] = g;
                rgba[index + 2] = b;
                rgba[index + 3] = 255;
            }
        }
    }

    void DrawWorm(
        uint8_t* rgba,
        int imageWidth,
        int imageHeight,
        int startX,
        int startY,
        int numSegments,
        int maxCellSize,
        uint8_t r, uint8_t g, uint8_t b)
    {
        for (int segment = 0; segment < numSegments; ++segment)
        {
            // 0.0 (head) → 1.0 (tail)
            float t = static_cast<float>(segment) / (numSegments - 1);

            // Cell size 점점 감소 (선형 or 곡선)
            int cellSize = static_cast<int>(maxCellSize * (1.0f - 0.7f * t));
            if (cellSize < 2) cellSize = 2;

            // 중심 좌표 계산 (→ 오른쪽 이동)
            int offset = segment * (maxCellSize + 1); // +1: 간격
            int centerX = startX + offset;
            int centerY = startY;

            // 그림
            DrawCell_v2(rgba, imageWidth, imageHeight, centerX, centerY, cellSize, r, g, b);
        }
    }


    void SaveStaticWebP(const std::vector<ContributionCell>& grid, int maxWeeks, const wchar_t* filename)
    {
        const int weeks = maxWeeks;

        const int imageWidth = weeks * (gCellSize + gCellMargin);

        WebPAnimEncoderOptions enc_options;
        WebPAnimEncoderOptionsInit(&enc_options);

        WebPAnimEncoder* enc = WebPAnimEncoderNew(imageWidth, gImageHeight, &enc_options);
        ASSERT(enc);

        // WebPConfig 설정
        WebPConfig config;
        WebPConfigInit(&config);
        config.quality = 100.0f;

        WebPPicture pic;
        WebPPictureInit(&pic);
        pic.width = imageWidth;
        pic.height = gImageHeight;
        pic.use_argb = 1;


        std::vector<uint8_t> rgba(imageWidth * gImageHeight * 4, 0); // 초기화
        for (size_t i = 0; i < imageWidth * gImageHeight; ++i) // 배경 초기화
        {
            rgba[i * 4 + 0] = 0x01; // R
            rgba[i * 4 + 1] = 0x04; // G
            rgba[i * 4 + 2] = 0x09; // B
            rgba[i * 4 + 3] = 0x0d; // A
        }

        // 셀 그리기
        if (0)
        {
            for (const auto& cell : grid)
            {
                int x = cell.weekIndex * (gCellSize + gCellMargin);
                int y = (cell.weekday + 1) * (gCellSize + gCellMargin);

                uint8_t r = GetRValue(cell.color);
                uint8_t g = GetGValue(cell.color);
                uint8_t b = GetBValue(cell.color);

                DrawCell(rgba.data(), imageWidth, x, y, r, g, b);
            }
        }

        for (const auto& cell : grid)
        {
            int x = cell.weekIndex * (gCellSize + gCellMargin);
            int y = (cell.weekday + 1) * (gCellSize + gCellMargin);

            // 중앙 좌표 계산
            int centerX = x + gCellSize / 2;
            int centerY = y + gCellSize / 2;

            uint8_t r = GetRValue(cell.color);
            uint8_t g = GetGValue(cell.color);
            uint8_t b = GetBValue(cell.color);

            // 고정된 크기로 먼저 그리기 (기존과 동일하게)
            DrawCell_v2(rgba.data(), imageWidth, gImageHeight, centerX, centerY, gCellSize, r, g, b);
        }

        //const COLORREF wormColor = HexToColorRef(L"#FFA500");
        const COLORREF wormColor = HexToColorRef(L"#FF69B4");
        const uint8_t wormColorR = GetRValue(wormColor);
        const uint8_t wormColorG = GetGValue(wormColor);
        const uint8_t wormColorB = GetBValue(wormColor);

        // 지렁이 그리기
        DrawWorm(rgba.data(),
                 imageWidth,
                 gImageHeight,
                 10, // startX
                 gImageHeight / 2, // centerY
                 12, // segment 수
                 10, // 최대 gCellSize
                 wormColorR, wormColorG, wormColorB); // 보라색

        // RGBA → WebPPicture 로 변환
        WebPPictureImportRGBA(&pic, rgba.data(), imageWidth * 4);

        // 프레임 추가
        if (!WebPAnimEncoderAdd(enc, &pic, 0, &config))
        {
            ASSERT(false);
        }

        // 마지막 frame 마킹
        WebPAnimEncoderAdd(enc, NULL, 0, NULL);

        // WebP 애니메이션 출력
        WebPData webp_data;
        WebPDataInit(&webp_data);
        if (!WebPAnimEncoderAssemble(enc, &webp_data))
        {
            fprintf(stderr, "Failed to assemble WebP animation\n");
            ASSERT(false);
        }

        // 파일로 저장
        FILE* f;
        _wfopen_s(&f, filename, L"wb");
        if (f)
        {
            fwrite(webp_data.bytes, webp_data.size, 1, f);
            fclose(f);
            printf("Saved animation.webp\n");
        }
        else
        {
            fprintf(stderr, "Failed to open output file\n");
        }

        // 정리
        WebPDataClear(&webp_data);
        WebPAnimEncoderDelete(enc);
        WebPPictureFree(&pic);
    }

    // 애니메이션 webp
    void TEST_WEBP_ANIMATED()
    {
        // 초기화
        WebPAnimEncoderOptions enc_options;
        WebPAnimEncoderOptionsInit(&enc_options);

        WebPAnimEncoder* enc = WebPAnimEncoderNew(gWidth, gHeight, &enc_options);
        if (!enc)
        {
            fprintf(stderr, "Failed to create WebPAnimEncoder\n");
        }

        WebPPicture pic;
        WebPPictureInit(&pic);
        pic.width = gWidth;
        pic.height = gHeight;
        pic.use_argb = 1;

        WebPConfig config;
        WebPConfigInit(&config);
        config.quality = 90.0f;

        uint8_t* rgba = (uint8_t*)malloc(gWidth * gHeight * 4);

        for (int frame = 0; frame < gFrame_count; ++frame)
        {
            ClearBuffer(rgba);

            // 움직이는 사각형 그리기
            int x = frame * 20;
            DrawRect(rgba, x, 100, 50, 50, 255, 0, 0, 255);

            // RGBA → WebPPicture 로 변환
            WebPPictureImportRGBA(&pic, rgba, gWidth * 4);

            // 프레임 추가
            if (!WebPAnimEncoderAdd(enc, &pic, frame * gFrame_delay_ms, &config))
            {
                fprintf(stderr, "Failed to add frame %d\n", frame);
            }
        }

        // 마지막 frame 마킹
        WebPAnimEncoderAdd(enc, NULL, gFrame_count * gFrame_delay_ms, NULL);

        // WebP 애니메이션 출력
        WebPData webp_data;
        WebPDataInit(&webp_data);
        if (!WebPAnimEncoderAssemble(enc, &webp_data))
        {
            fprintf(stderr, "Failed to assemble WebP animation\n");
        }


        // 파일로 저장
        FILE* f;
        _wfopen_s(&f, L"test.webp", L"wb");
        if (f)
        {
            fwrite(webp_data.bytes, webp_data.size, 1, f);
            fclose(f);
            printf("Saved animation.webp\n");
        }
        else
        {
            fprintf(stderr, "Failed to open output file\n");
        }

        // 정리
        WebPDataClear(&webp_data);
        WebPAnimEncoderDelete(enc);
        WebPPictureFree(&pic);
        free(rgba);
    }

    void TEST_WEBP(const std::vector<ContributionCell>& grid, int maxWeeks)
    {
        //TEST_WEBP_ANIMATED();
        SaveStaticWebP(grid, maxWeeks, L"animated.webp");
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
