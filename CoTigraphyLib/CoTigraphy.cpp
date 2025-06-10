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
    constexpr int width = 256;
    constexpr int height = 256;
    constexpr int frame_count = 10;
    constexpr int frame_delay_ms = 100; // 100ms per frame

    constexpr int cellSize = 12; // 각 칸 크기 (px)
    constexpr int cellMargin = 2; // 칸 간격 (px)
    constexpr int daysPerWeek = 7; // Sunday~Saturday (7 rows)
    constexpr int imageHeight = daysPerWeek * (cellSize + cellMargin);

    struct ContributionCell
    {
        int weekIndex;
        int weekday; // 0 = Sunday, 1 = Monday, ..., 6 = Saturday
        uint64_t count;
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
        memset(rgba, 0, width * height * 4);
    }

    // 사각형 그리기 (RGBA 버퍼에 직접 그리기)
    void DrawRect(uint8_t* rgba, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        for (int j = y; j < y + h; ++j)
        {
            if (j < 0 || j >= height) continue;
            for (int i = x; i < x + w; ++i)
            {
                if (i < 0 || i >= width) continue;
                int index = (j * width + i) * 4;
                rgba[index + 0] = r;
                rgba[index + 1] = g;
                rgba[index + 2] = b;
                rgba[index + 3] = a;
            }
        }
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
        for (int j = 0; j < cellSize; ++j)
        {
            for (int i = 0; i < cellSize; ++i)
            {
                int px = x + i;
                int py = y + j;

                if (px < 0 || px >= imgWidth || py < 0 || py >= imageHeight) continue;

                int index = (py * imgWidth + px) * 4;
                rgba[index + 0] = r;
                rgba[index + 1] = g;
                rgba[index + 2] = b;
                rgba[index + 3] = 255;
            }
        }
    }

    void SaveStaticWebP(const std::vector<ContributionCell>& grid, int maxWeeks, const wchar_t* filename)
    {
        const int weeks = maxWeeks;

        const int imageWidth = weeks * (cellSize + cellMargin);

        WebPAnimEncoderOptions enc_options;
        WebPAnimEncoderOptionsInit(&enc_options);

        WebPAnimEncoder* enc = WebPAnimEncoderNew(imageWidth, imageHeight, &enc_options);
        ASSERT(enc);

        // WebPConfig 설정
        WebPConfig config;
        WebPConfigInit(&config);
        config.quality = 90.0f;

        WebPPicture pic;
        WebPPictureInit(&pic);
        pic.width = imageWidth;
        pic.height = imageHeight;
        pic.use_argb = 1;


        std::vector<uint8_t> rgba(imageWidth * imageHeight * 4, 0); // 초기화 (배경 검정)

        // 그리기
        for (const auto& cell : grid)
        {
            int x = cell.weekIndex * (cellSize + cellMargin);
            int y = cell.weekday * (cellSize + cellMargin);

            uint8_t r, g, b;
            GetColorForCount(cell.count, r, g, b);

            DrawCell(rgba.data(), imageWidth, x, y, r, g, b);
        }

        // RGBA → WebPPicture 로 변환
        int r = WebPPictureImportRGBA(&pic, rgba.data(), imageWidth * 4);
        (r);

        // 프레임 추가
        if (!WebPAnimEncoderAdd(enc, &pic, 0, &config)) {
            ASSERT(false);
        }

        // 마지막 frame 마킹
        WebPAnimEncoderAdd(enc, NULL, 0, NULL);

        // WebP 애니메이션 출력
        WebPData webp_data;
        WebPDataInit(&webp_data);
        if (!WebPAnimEncoderAssemble(enc, &webp_data)) {
            fprintf(stderr, "Failed to assemble WebP animation\n");
            ASSERT(false);
        }

        // 파일로 저장
        FILE* f;
        _wfopen_s(&f, filename, L"wb");
        if (f) {
            fwrite(webp_data.bytes, webp_data.size, 1, f);
            fclose(f);
            printf("Saved animation.webp\n");
        }
        else {
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

        WebPAnimEncoder* enc = WebPAnimEncoderNew(width, height, &enc_options);
        if (!enc) {
            fprintf(stderr, "Failed to create WebPAnimEncoder\n");
        }

        WebPPicture pic;
        WebPPictureInit(&pic);
        pic.width = width;
        pic.height = height;
        pic.use_argb = 1;

        WebPConfig config;
        WebPConfigInit(&config);
        config.quality = 90.0f;

        uint8_t* rgba = (uint8_t*)malloc(width * height * 4);

        for (int frame = 0; frame < frame_count; ++frame) {
            ClearBuffer(rgba);

            // 움직이는 사각형 그리기
            int x = frame * 20;
            DrawRect(rgba, x, 100, 50, 50, 255, 0, 0, 255);

            // RGBA → WebPPicture 로 변환
            WebPPictureImportRGBA(&pic, rgba, width * 4);

            // 프레임 추가
            if (!WebPAnimEncoderAdd(enc, &pic, frame * frame_delay_ms, &config)) {
                fprintf(stderr, "Failed to add frame %d\n", frame);
            }
        }

        // 마지막 frame 마킹
        WebPAnimEncoderAdd(enc, NULL, frame_count * frame_delay_ms, NULL);

        // WebP 애니메이션 출력
        WebPData webp_data;
        WebPDataInit(&webp_data);
        if (!WebPAnimEncoderAssemble(enc, &webp_data)) {
            fprintf(stderr, "Failed to assemble WebP animation\n");
        }


        // 파일로 저장
        FILE* f;
        _wfopen_s(&f, L"test.webp", L"wb");
        if (f) {
            fwrite(webp_data.bytes, webp_data.size, 1, f);
            fclose(f);
            printf("Saved animation.webp\n");
        }
        else {
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
