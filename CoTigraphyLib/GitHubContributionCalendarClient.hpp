// \file GitHubContributionCalendarClient.hpp
// \last_updated 2025-06-11
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <chrono>
#include <iomanip>
#include <vector>

#include <curl/curl.h>

namespace CoTigraphy
{
    struct ContributionCell
    {
        size_t mXIndex = 0;
        size_t mYIndex = 0;
        uint64_t mCount = 0;
        COLORREF mColor = 0;
    };

    struct ContributionInfo
    {
        std::vector<ContributionCell> mContributionCells;
        size_t mRowCount = 0; // 줄 수 -> 요일 갯수 -> 7
        size_t mColoumCount = 0; // 커럼 수 -> 주 갯수
    };

    class GitHubContributionCalendarClient final
    {
    public:
        explicit GitHubContributionCalendarClient() noexcept;
        GitHubContributionCalendarClient(const GitHubContributionCalendarClient& other) = delete;
        GitHubContributionCalendarClient(GitHubContributionCalendarClient&& other) = delete;

        GitHubContributionCalendarClient& operator=(const GitHubContributionCalendarClient& rhs) = delete;
        GitHubContributionCalendarClient& operator=(GitHubContributionCalendarClient&& rhs) = delete;

        ~GitHubContributionCalendarClient();

        bool Initialize();
        void Uninitialize() const;

        void SetAccessToken(const std::wstring& token);

        ContributionInfo FetchContributionInfo() const;

    private:
        // UTF-8 → wstring 변환
        static std::wstring Utf8ToWideString(const std::string& utf8)
        {
            if (utf8.empty())
                return L"";

            const int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
            if (sizeRequired <= 0)
                return L"";

            std::wstring wide(sizeRequired, 0);
            MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wide.data(), sizeRequired);

            // Remove trailing null terminator added by MultiByteToWideChar
            wide.resize(sizeRequired - 1);
            return wide;
        }

        // wstring → UTF-8 변환
        static std::string WideStringToUtf8(const std::wstring& wide)
        {
            if (wide.empty())
                return "";

            const int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (sizeRequired <= 0)
                return "";

            std::string utf8(sizeRequired, 0);
            WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, utf8.data(), sizeRequired, nullptr, nullptr);

            // Remove trailing null terminator
            utf8.resize(sizeRequired - 1);
            return utf8;
        }

        // WriteCallback for libcurl
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
        {
            const size_t totalSize = size * nmemb;
            std::vector<char>* buffer = static_cast<std::vector<char>*>(userp);

            size_t currentSize = buffer->size();
            if (currentSize > 0)
                currentSize -= 1; // overwrite the '\0'

            buffer->resize(currentSize + totalSize + 1);
            memcpy(buffer->data() + currentSize, contents, totalSize);

            buffer->at(currentSize + totalSize) = '\0';
            return totalSize;
        }

        static COLORREF HexToColorRef(const std::wstring& hex)
        {
            PRECONDITION(hex.length() == 7);
            PRECONDITION(hex[0] == L'#');

            const unsigned int r = std::stoi(hex.substr(1, 2), nullptr, 16);
            const unsigned int g = std::stoi(hex.substr(3, 2), nullptr, 16);
            const unsigned int b = std::stoi(hex.substr(5, 2), nullptr, 16);

            POSTCONDITION(r <= 255);
            POSTCONDITION(g <= 255);
            POSTCONDITION(g <= 255);

            return RGB(r, g, b); // Macro: ((BYTE)(r) | ((BYTE)(g) << 8) | ((BYTE)(b) << 16))
        }

    private:
        ContributionInfo Parse(const std::string& response) const;

    private:
        CURL* mCurl = nullptr;
        curl_slist* mHeaders = nullptr;
    };
} // CoTigraphy
