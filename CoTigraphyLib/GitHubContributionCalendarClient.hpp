// \file GitHubContributionCalendarClient.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <chrono>

#include <curl/curl.h>

#include "Grid.hpp"

namespace CoTigraphy
{
    /**
     * \brief Github의 Contribution calendar 정보를 가져오는 클라이언트 클래스
     * \details
     *  - GraphQL을 통해 contribution calendar 데이터를 가져옴
     *  - simdjson을 통해 가져온 데이터를 파싱
     *  - 사용 전 Initialize(), 사용 후 Uninitialize() 호출 필수
     */
    class GitHubContributionCalendarClient final
    {
    public:
        explicit GitHubContributionCalendarClient() noexcept;
        GitHubContributionCalendarClient(const GitHubContributionCalendarClient& other) = delete;
        GitHubContributionCalendarClient(GitHubContributionCalendarClient&& other) = delete;

        GitHubContributionCalendarClient& operator=(const GitHubContributionCalendarClient& rhs) = delete;
        GitHubContributionCalendarClient& operator=(GitHubContributionCalendarClient&& rhs) = delete;

        ~GitHubContributionCalendarClient();

        /**
         * \brief curl 초기화
         */
        void Initialize();


        /**
         * \brief curl 리소스 해제
         */
        void Uninitialize();

        /**
         * \brief Github personal access token을 authorization 헤더에 설정
         * \param token github personal access token
         * \details
         *  - GraphQL 쿼리는 authorization 헤더에 적절한 personal access token을 설정해야 유효한 값을 반환한다
         */
        void SetAccessToken(_In_ const std::wstring& token);


        /**
         * \brief 요청한 Github 사용자로부터 Contribution calendar 정보를 가져온다.
         * \return Contribution calendar를 GridData 형태로 파싱한 데이터
         */
        [[nodiscard]] GridData FetchContributionInfo(_In_ const std::wstring& userName, _In_ const std::wstring& fields) const;

    private:
        /**
         * @brief GraphQL 쿼리 문자열을 생성하여 반환
         * @param userName 사용자 GitHub 로그인 이름 (UTF-16)
         * @param fields 가져올 필드 목록 (예: L"date contributionCount color")
         * @return JSON 형식으로 감싼 GraphQL 쿼리 문자열
         * @contract strong loginName과 fields는 유효해야 함
         * @details
         * - 내부적으로 "query { user(login: \"...\") { ... } }" 형태 구성
         * - UTF-16 기반으로 구성되며, 이후 UTF-8로 변환해 사용
         */
        std::wstring BuildContributionQuery(_In_ const std::wstring& userName, _In_ const std::wstring& fields) const;

        /**
         * @brief JSON 문자열 내에서 필요한 특수문자를 이스케이프하여 안전하게 변환
         * @param input 원본 문자열
         * @return JSON 문자열로 안전하게 변환된 결과
         * @contract strong input은 비어있지 않아야 하며, 제어 문자 및 JSON 예약문자는 이스케이프됨
         * @details
         *  - " > \" / \\ > \\\\ / 제어문자(0x00~0x1F, 0x7F, surrogate 범위 등)은 \uXXXX 처리
         */
        std::wstring EscapeJsonString(_In_ const std::wstring& input) const;

        /**
         * \brief GraphQL JSON 응답을 GridData 형태로 파싱
         * \param response UTF-8 인코딩 된 JSON 응답 문자열
         * \return GridData 형태로 변환된 Contribution calendar 구조체
         */
        [[nodiscard]] GridData Parse(_In_ const std::string& response) const;

        // UTF-8 → wstring 변환
        [[nodiscard]] std::wstring Utf8ToWideString(_In_ const std::string& utf8) const;

        // wstring → UTF-8 변환
        [[nodiscard]] std::string WideStringToUtf8(_In_ const std::wstring& wide) const;

        /**
         * \brief HEX 색상 문자열을 COLORREF 형태로 파싱한다.
         * \param hex hex 문자열 e.g) #43FF12
         * \return COLORREF 값
         */
        [[nodiscard]] COLORREF HexToColorRef(_In_ const std::wstring& hex) const;

    private:
        // WriteCallback for libcurl
        static size_t WriteCallback(const void* contents, size_t size, size_t nmemb, void* userp);

    private:
        CURL* mCurl = nullptr; // curl 핸들
        curl_slist* mHeaders = nullptr; // curl http 헤더
    };
} // CoTigraphy
