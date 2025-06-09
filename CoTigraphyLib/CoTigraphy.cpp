// \file CoTigraphy.cpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CoTigraphy.hpp"

#include <iostream>
#include <shellapi.h>
#include <string_view>

#include <curl/curl.h>

#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"
#include "VersionInfo.hpp"

namespace CoTigraphy
{
    // 서버 응답을 버퍼에 저장하기 위한 콜백 함수
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        size_t totalSize = size * nmemb;
        char** responsePtr = (char**)userp;
        size_t oldSize = strlen(*responsePtr);

        char* buffer = (char*)realloc(*responsePtr, oldSize + totalSize + 1);
        if (buffer == NULL)
        {
            printf("Out of memory!\n");
            return 0;
        }

        *responsePtr = buffer;

        // 새 데이터 복사 (기존 데이터 뒤에 추가)
        memcpy(buffer + oldSize, contents, totalSize);
        buffer[oldSize + totalSize] = '\0'; // 널 종료

        return totalSize;
    }

    void TEST_CURL()
    {
        CURL* curl = nullptr;
        CURLcode res;

        const char* const token = "<github_token>";
        const char* const url = "https://api.github.com/user";

        // 응답 데이터를 저장할 버퍼
        char* response = (char*)malloc(1);
        if (!response)
        {
            printf("Out of memory!\n");
            return;
        }
        response[0] = '\0';

        // curl 전역 초기화
        curl_global_init(CURL_GLOBAL_DEFAULT);

        // curl 핸들 생성
        curl = curl_easy_init();
        if (curl)
        {
            // HTTP 헤더 준비
            struct curl_slist* headers = nullptr;
            char authHeader[512];
            snprintf(authHeader, sizeof(authHeader), "Authorization: Bearer %s", token);

            headers = curl_slist_append(headers, authHeader);
            headers = curl_slist_append(headers, "User-Agent: MyLibcurlClient/1.0"); // GitHub는 User-Agent 필수

            // 옵션 설정
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // 디버그 로그 출력
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // 응답 콜백 설정
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // 요청 실행
            res = curl_easy_perform(curl);

            // 에러 확인
            if (res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            else
            {
                // 성공 시 응답 출력
                printf("Response:\n%s\n", response);
            }

            // 정리
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }

        free(response);
        curl_global_cleanup();
    }
        Error Initialize()
    {
        CoTigraphy::MemoryLeakDetector::Initialize();
        CoTigraphy::HandleLeakDetector::Initialize();

        TEST_CURL();

        CoTigraphy::CommandLineParser commandLineParser;
        Error error = SetupCommandLineParser(commandLineParser);
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

    void Uninitialize() noexcept
    {
    }

    Error SetupCommandLineParser(CoTigraphy::CommandLineParser& commandLineParser)
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
                std::wstring accessToken = L"Github presonal access token: ";
                accessToken += value;
                accessToken += L"\n";
                OutputDebugStringW(accessToken.c_str());
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
