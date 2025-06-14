// \file GitHubContributionCalendarClient.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "GitHubContributionCalendarClient.hpp"

#pragma warning(push)   // nlohmann json 라이브러리에서 발생하는 경고 비활성화, 해결하기 귀찮은 것들...
#pragma warning(disable: 26429) // Symbol is never tested for nullness, it can be marked as not_null (f.23)
#pragma warning(disable: 26432) // If you define or delete any default operation in a type, define or delete them all (c.21)
#pragma warning(disable: 26438) // Avoid 'goto' (es.76)
#pragma warning(disable: 26440) // Function can be declared 'noexcept' (f.6)
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4)
#pragma warning(disable: 26447) // The function is declared 'noexcept' but calls function which may throw exceptions (f.6)
#pragma warning(disable: 26451) // Arithmetic overflow: cast before subtraction to avoid overflow (io.2)
#pragma warning(disable: 26455) // Default constructor should not throw. Declare it 'noexcept' (f.6)
#pragma warning(disable: 26472) // Don't use a static_cast for arithmetic conversions (type.1)
#pragma warning(disable: 26476) // Symbol uses a naked union with multiple type pointers: Use variant instead (type.7)
#pragma warning(disable: 26482) // Only index into arrays using constant expressions (bounds.2)
#pragma warning(disable: 26493) // Don't use C-style casts (type.4)
#pragma warning(disable: 26497) // Consider marking function as constexpr (f.4)
#pragma warning(disable: 26819) // Unannotated fallthrough between switch labels (es.78)

#include <nlohmann/json.hpp>
#pragma warning(pop)

namespace CoTigraphy
{
    GitHubContributionCalendarClient::GitHubContributionCalendarClient() noexcept
    = default;

    GitHubContributionCalendarClient::~GitHubContributionCalendarClient()
    = default;

    void GitHubContributionCalendarClient::Initialize()
    {
        const CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
        ASSERT(code == CURLE_OK);

        mCurl = curl_easy_init();
        ASSERT(mCurl != nullptr);

        mHeaders = curl_slist_append(mHeaders, "User-Agent: CoTigraphy/1.0");
        ASSERT(mHeaders != nullptr);

        mHeaders = curl_slist_append(mHeaders, "Content-Type: application/json");
        ASSERT(mHeaders != nullptr);

        POSTCONDITION(mCurl != nullptr);
        POSTCONDITION(mHeaders != nullptr);
    }

    void GitHubContributionCalendarClient::Uninitialize()
    {
        PRECONDITION(mHeaders != nullptr);
        PRECONDITION(mCurl != nullptr);

        curl_slist_free_all(mHeaders);
        mHeaders = nullptr;

        curl_easy_cleanup(mCurl);
        mCurl = nullptr;

        curl_global_cleanup();

        POSTCONDITION(mHeaders == nullptr);
        POSTCONDITION(mCurl == nullptr);
    }

    void GitHubContributionCalendarClient::SetAccessToken(_In_ const std::wstring& token)
    {
        PRECONDITION(token.empty() == false);
        PRECONDITION(mHeaders != nullptr); // Initialize를 먼저 호출해야 함

        const std::string tokenUtf8 = WideStringToUtf8(token);
        const std::string authHeader = "Authorization: Bearer " + tokenUtf8;

        mHeaders = curl_slist_append(mHeaders, authHeader.c_str());

        POSTCONDITION(mHeaders != nullptr);
    }

    /**
     * @brief GitHub의 기여 캘린더 데이터를 요청하고 파싱하여 GridData로 반환
     * @param userName GitHub 사용자 로그인 이름
     * @param fields GraphQL 요청 시 포함할 필드 목록 (예: "date contributionCount color")
     * @return GridData 파싱된 기여 데이터
     * @contract strong 모든 요청 실패 시 ASSERT 종료
     * @pre mHeaders != nullptr (SetAccessToken 이후 호출해야 함)
     * @post GridData 내부에 기여 데이터가 2차원 벡터로 채워짐
     * @details
     * - GraphQL API를 통해 JSON 형태로 기여 정보 요청
     * - 응답 결과는 Parse() 함수를 통해 파싱됨
     */
    GridData GitHubContributionCalendarClient::FetchContributionInfo(_In_ const std::wstring& userName,
                                                                     _In_ const std::wstring& fields) const
    {
        PRECONDITION(mCurl != nullptr); // Initialize()를 먼저 호출해야 함
        PRECONDITION(mHeaders != nullptr); // SetAccessToken()을 먼저 호출해야 함

        // GraphQL Query Payload
        constexpr const wchar_t* const kGraphQlUrl = L"https://api.github.com/graphql";
        const std::string url = WideStringToUtf8(kGraphQlUrl);

        const std::wstring graphqlQueryW = BuildContributionQuery(userName, fields);
        const std::string graphqlQuery = WideStringToUtf8(graphqlQueryW);

        std::vector<char> responseBuffer(1, '\0');

        curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, mHeaders);

        curl_easy_setopt(mCurl, CURLOPT_POST, 1L);
        curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, graphqlQuery.c_str());

        curl_easy_setopt(mCurl, CURLOPT_VERBOSE, 0L);

        curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &responseBuffer);

        curl_easy_setopt(mCurl, CURLOPT_FORBID_REUSE, 1L); //  connection 재사용 방지
        curl_easy_setopt(mCurl, CURLOPT_FRESH_CONNECT, 1L); // connection pool에서 즉시 종료
        curl_easy_setopt(mCurl, CURLOPT_SSL_SESSIONID_CACHE, 0L); // Schannel 사용 시 강제 cleanup

        const CURLcode res = curl_easy_perform(mCurl);
        ASSERT(res == CURLE_OK);

        const std::string responseStr(responseBuffer.data());

        return Parse(responseStr);
    }

    // https://docs.github.com/en/graphql/reference/objects#contributionscollection
    std::wstring GitHubContributionCalendarClient::BuildContributionQuery(_In_ const std::wstring& userName,
                                                                          _In_ const std::wstring& fields) const
    {
        PRECONDITION(userName.empty() == false);
        PRECONDITION(fields.empty() == false);

        std::wstringstream ss;
        ss << L"{ \"query\": \"query { ";
        ss << L"user(login: \\\"" << EscapeJsonString(userName) << L"\\\") { ";
        ss << L"contributionsCollection { ";
        ss << L"contributionCalendar { ";
        ss << L"weeks { ";
        ss << L"contributionDays { " << EscapeJsonString(fields) << L" } ";
        ss << L"} } } } }\" }";

        const std::wstring ret = ss.str();
        POSTCONDITION(ret.empty() == false);

        return ret;
    }

    std::wstring GitHubContributionCalendarClient::EscapeJsonString(_In_ const std::wstring& input) const
    {
        ASSERT(input.empty() == false);

        std::wstringstream escaped;
        escaped << std::hex << std::setfill(L'0');

        for (const wchar_t ch : input)
        {
            switch (ch)
            {
            case L'\"': escaped << L"\\\"";
                break;
            case L'\\': escaped << L"\\\\";
                break;
            case L'\b': escaped << L"\\b";
                break;
            case L'\f': escaped << L"\\f";
                break;
            case L'\n': escaped << L"\\n";
                break;
            case L'\r': escaped << L"\\r";
                break;
            case L'\t': escaped << L"\\t";
                break;
            default:
                if (ch < 0x20 || ch == 0x7F || (ch >= 0xD800 && ch <= 0xDFFF))
                {
                    escaped << L"\\u" << std::setw(4) << static_cast<unsigned int>(ch);
                }
                else
                {
                    escaped << ch;
                }
                break;
            }
        }

        const std::wstring ret = escaped.str();
        POSTCONDITION(ret.empty() == false);

        return ret;
    }

    GridData GitHubContributionCalendarClient::Parse(_In_ const std::string& response) const
    {
        PRECONDITION(response.empty() == false);

        GridData gridData;

        nlohmann::json root = nlohmann::json::parse(response, nullptr, false);
        ASSERT(root.contains("data"));

        const auto& weeks = root["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"];
        ASSERT(weeks.is_array());

        gridData.mWeekCount = weeks.size();

        size_t dayIndex = 0;
        size_t weekIndex = 0;

        for (const auto& week : weeks)
        {
            const auto& days = week["contributionDays"];
            ASSERT(days.is_array());

            const size_t rowCount = days.size();
            if (gridData.mDayCount != 0)
            {
                // 오늘이 수요일인 경우
                // 일, 월, 화, 수 까지 rowCount가 4가 될 수 있다.
                // 따라서 작거나 같은경우까지 혀용한다.
                ASSERT(rowCount <= gridData.mDayCount);
            }

            gridData.mDayCount = rowCount;

            std::vector<GridCell> gridCells;

            for (const auto& day : days)
            {
                GridCell cell;
                cell.mCount = day.value("contributionCount", 0);
                const std::string colorHex = day.value("color", "#FFFFFF");
                cell.mColor = HexToColorRef(Utf8ToWideString(colorHex));
                cell.mWeek = weekIndex;
                cell.mDay = dayIndex;

                gridData.mMaxCount = std::max(cell.mCount, gridData.mMaxCount);
                gridCells.push_back(cell);

                ++dayIndex;
            }

            gridData.mCells.push_back(gridCells);

            weekIndex++;
            dayIndex = 0;
        }

        POSTCONDITION(gridData.mWeekCount != 0);
        POSTCONDITION(gridData.mDayCount != 0);
        POSTCONDITION(gridData.mMaxCount != 0);
        POSTCONDITION(gridData.mCells.empty() == false);

        return gridData;
    }

    std::wstring GitHubContributionCalendarClient::Utf8ToWideString(_In_ const std::string& utf8) const
    {
        if (utf8.empty())
            return L"";

        const int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
        if (sizeRequired <= 0)
            return L"";

        std::wstring wide(sizeRequired, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wide.data(), sizeRequired);

        wide.resize(static_cast<size_t>(sizeRequired) - 1);  // null 문자 제거
        return wide;
    }

    std::string GitHubContributionCalendarClient::WideStringToUtf8(_In_ const std::wstring& wide) const
    {
        if (wide.empty())
            return "";

        const int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (sizeRequired <= 0)
            return "";

        std::string utf8(sizeRequired, 0);
        WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, utf8.data(), sizeRequired, nullptr, nullptr);

        utf8.resize(static_cast<size_t>(sizeRequired) - 1);  // null 문자 제거
        return utf8;
    }

    COLORREF GitHubContributionCalendarClient::HexToColorRef(_In_ const std::wstring& hex) const
    {
        PRECONDITION(hex.length() == 7);
        PRECONDITION(hex[0] == L'#');

        const unsigned int r = std::stoi(hex.substr(1, 2), nullptr, 16);
        const unsigned int g = std::stoi(hex.substr(3, 2), nullptr, 16);
        const unsigned int b = std::stoi(hex.substr(5, 2), nullptr, 16);

        return RGB(r, g, b); // Macro: ((BYTE)(r) | ((BYTE)(g) << 8) | ((BYTE)(b) << 16))
    }

    size_t GitHubContributionCalendarClient::WriteCallback(const void* contents, const size_t size, const size_t nmemb,
                                                           void* userp)
    {
        const size_t totalSize = size * nmemb;
        std::vector<char>* buffer = static_cast<std::vector<char>*>(userp);
        ASSERT(buffer != nullptr);

        size_t currentSize = buffer->size();
        if (currentSize > 0)
            currentSize -= 1;

        if (totalSize > std::numeric_limits<size_t>::max() - currentSize - 1)
            return 0; // overflow 방지

        buffer->resize(currentSize + totalSize + 1);
        memcpy(buffer->data() + currentSize, contents, totalSize);

        buffer->at(currentSize + totalSize) = '\0';
        return totalSize;
    }
} // CoTigraphy
