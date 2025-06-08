// \file CommandLineParser.hpp
// \last_updated 2025-06-08
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <functional>
#include <string>
#include <vector>

namespace CoTigraphy
{
    // Describes a single command-line option
    struct CommandLineOption
    {
        std::wstring mName; // Long name, e.g. "--help"
        std::wstring mShortName; // Short name, e.g. "-h"
        std::wstring mDescription; // Help description
        bool mRequiresValue; // Does this option require a following value?
        bool mCausesExit; // Should parsing stop after this option?
        std::function<void(const std::wstring&)> mHandler; // Called with the option's value (or empty)

        bool IsValid() const noexcept
        {
            return !mName.empty() && !mShortName.empty() && !mDescription.empty();
        }
    };

    /**
     * \brief 커맨드 라인 옵션을 등록하고 파싱 하는 클래스
     * \contract weak 모든 입력을 검사하고 오류 코드로 처리함
     * * \pre 모든 옵션은 AddOption 호출을 통해 사전에 등록되어야 함.
     * \details
     *  - 외부에서 받은 argc/argv를 파싱하여 등록된 옵션을 처리
     *  - Help 메시지를 출력하거나, 옵션에 따라 핸들러를 호출
     *  - Early-exit 옵션(예: --help, --version)을 우선 처리
     */
    class CommandLineParser final
    {
    public:
        explicit CommandLineParser() noexcept;
        CommandLineParser(const CommandLineParser& other) = delete;
        CommandLineParser(CommandLineParser&& other) = delete;

        CommandLineParser& operator=(const CommandLineParser& rhs) = delete;
        CommandLineParser& operator=(CommandLineParser&& rhs) = delete;

        ~CommandLineParser();

        /**
         * \brief 새 커맨트 라인 옵션 등록
         * \param option 등록할 커맨드 라인 옵션 객체, 유효한 욥션이여야 함.
         * \return 등록 성공 여부 반환
         * \pre option.IsValid() == true
         */
        _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
        [[nodiscard]] Error AddOption(const CommandLineOption& option);

        
        /**
         * \brief argv/argc 형식의 커맨드 라인 인자를 파싱
         * \contract weak argc와, argv를 검사하고 필요 시 오류 반환
         * \param argc 인자 갯수
         * \param argv 인자 문자열 배열. argv[0]은 프로그램 경로로 판단하며, 나머지가 파싱 대상
         * \return 파싱 성공 여부 오류 코드 반환
         * \pre argc >= 1, argv != nullptr
         */
        _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
        [[nodiscard]] Error Parse(_In_ const int argc, _In_reads_opt_z_(argc) wchar_t* argv[]);
        
        /**
         * \brief 문자열 벡터 형식의 커맨드 라인 인자를 파싱
         * \contract weak 입력 베터를 검사하고 필요 시 오류 반환
         * \param args 파싱할 인자 문자열 벡터. 프로그램 경로는 포함되지 않음
         * \return 성공 여부 오류 코드 반환
         * \pre args.empty() == false
         */
        _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
        [[nodiscard]] Error Parse(_In_ const std::vector<std::wstring>& args);

        /**
         * \brief 등록된 옵션에 대한 도움말 메시지를 지정한 스트림에 출력
         * \param os 도움말 메시지를 출력할 wide ostream
         * \return wide ostream 참조
         * \pre os.good() == true
         */
        std::wostream& PrintHelpTo(_In_ std::wostream& os) const;

    private:
        
        /**
         * \brief 개별 토큰을 처리하여 옵션 핸들러를 호출
         * \param args 파싱할 인자 문자열 벡터
         * \param index 처리할 토큰 인덱스, 완료 후 인덱스가 증가될 수 있음
         * \return 성공 여부 오류 코드 반환
         * \pre index < args.size()
         */
        _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
        [[nodiscard]] Error ProcessToken(_In_ const std::vector<std::wstring>& args, _Inout_ size_t& index);

    private:
        // Store options in vector for ordered help output
        std::vector<CommandLineOption> mOptions;
        // Map each name to the index in mOptions (avoids duplicate storage)
        std::unordered_map<std::wstring, size_t> mLookup;
    };
}
