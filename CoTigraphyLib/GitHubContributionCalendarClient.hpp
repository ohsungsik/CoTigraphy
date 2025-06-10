// \file GitHubContributionCalendarClient.hpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <chrono>
#include <iomanip>
#include <vector>

#include <curl/curl.h>

namespace CoTigraphy
{
    struct ContributionDay
    {
        std::tm mDate;
        uint64_t mCount;
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

        std::vector<ContributionDay> Get() const;

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

        std::tm ParseDateString(const std::string& dateStr) const
        {
            std::tm tm{};
            std::istringstream ss(dateStr);
            ss >> std::get_time(&tm, "%Y-%m-%d");

            if (ss.fail())
            {
                return {};
            }

            return tm;
        }

    private:
        std::vector<ContributionDay> Parse(const std::string& response) const;

    private:
        CURL* mCurl = nullptr;
        curl_slist* mHeaders = nullptr;
    };
}
