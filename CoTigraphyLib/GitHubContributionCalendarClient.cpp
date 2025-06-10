// \file GitHubContributionCalendarClient.cpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "GitHubContributionCalendarClient.hpp"

#pragma warning(push, 0)   // simdjson 경고 비활성화
#include <simdjson.h>
#pragma warning(pop)

namespace CoTigraphy
{
    GitHubContributionCalendarClient::GitHubContributionCalendarClient() noexcept
    = default;

    GitHubContributionCalendarClient::~GitHubContributionCalendarClient()
    = default;

    bool GitHubContributionCalendarClient::Initialize()
    {
        const CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
        ASSERT(code == CURLE_OK);

        mCurl = curl_easy_init();
        ASSERT(mCurl != nullptr);

        mHeaders = curl_slist_append(mHeaders, "User-Agent: CoTigraphy/1.0");
        mHeaders = curl_slist_append(mHeaders, "Content-Type: application/json");

        return true;
    }

    void GitHubContributionCalendarClient::Uninitialize() const
    {
        if (mHeaders)
        {
            curl_slist_free_all(mHeaders);
        }

        if (mCurl)
        {
            curl_easy_cleanup(mCurl);
        }

        curl_global_cleanup();
    }

    void GitHubContributionCalendarClient::SetAccessToken(const std::wstring& token)
    {
        // Convert token to UTF-8
        const std::string tokenUtf8 = WideStringToUtf8(token);

        const std::string authHeader = "Authorization: Bearer " + tokenUtf8;

        mHeaders = curl_slist_append(mHeaders, authHeader.c_str());
    }

    std::vector<ContributionDay> GitHubContributionCalendarClient::Get() const
    {
        const std::string url = WideStringToUtf8(L"https://api.github.com/graphql");

        // GraphQL Query Payload
        const char* graphqlQuery = R"({
            "query": "query { user(login: \"ohsungsik\") { contributionsCollection { contributionCalendar { totalContributions weeks { contributionDays { date contributionCount } } } } } }"
        })";

        std::vector<char> responseBuffer;
        responseBuffer.push_back('\0'); // Initialize with empty string

        curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, mHeaders);

        curl_easy_setopt(mCurl, CURLOPT_POST, 1L);
        curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, graphqlQuery);

        curl_easy_setopt(mCurl, CURLOPT_VERBOSE, 0L);

        curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &responseBuffer);

        curl_easy_setopt(mCurl, CURLOPT_FORBID_REUSE, 1L);  //  connection 재사용 방지
        curl_easy_setopt(mCurl, CURLOPT_FRESH_CONNECT, 1L); // connection pool에서 즉시 종료
        curl_easy_setopt(mCurl, CURLOPT_SSL_SESSIONID_CACHE, 0L);   // Schannel 사용 시 강제 cleanup

        const CURLcode res = curl_easy_perform(mCurl);

        ASSERT(res == CURLE_OK);

        const std::string responseStr(responseBuffer.data());

        return Parse(responseStr);
    }

    std::vector<ContributionDay> GitHubContributionCalendarClient::Parse(const std::string& response) const
    {
        std::vector<ContributionDay> contributionDays;

        simdjson::ondemand::parser parser;
        const simdjson::padded_string padded = simdjson::padded_string(response);
        simdjson::ondemand::document doc = parser.iterate(padded);

        auto days = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"];

        for (auto week : days.get_array())
        {
            for (auto day : week["contributionDays"].get_array())
            {
                const std::string dateStr = std::string(day["date"].get_string().value());
                const uint64_t count = day["contributionCount"].get_uint64().value();

                ContributionDay contributionDay;
                contributionDay.mDate = ParseDateString(dateStr);
                contributionDay.mCount = count;

                contributionDays.push_back(contributionDay);
            }
        }

        return contributionDays;
    }
}
