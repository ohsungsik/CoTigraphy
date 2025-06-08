// \file CommandLineParser.cpp
// \last_updated 2025-06-08
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CommandLineParser.hpp"

#include <iomanip>
#include <iostream>
#include <unordered_set>

#include "VersionInfo.hpp"

namespace CoTigraphy
{
    CommandLineParser::CommandLineParser() noexcept
    = default;

    CommandLineParser::~CommandLineParser()
    = default;

    /**
     * @details
     * - 옵션 유효성 검사: option.IsValid() 확인
     * - 중복 등록 방지: mLookup에 이름/단축 이름 중복 여부 확인
     * - 등록 순서 유지: mOptions 벡터에 복사본 추가
     * - 해시맵 갱신: mLookup에 옵션 이름과 단축 이름 매핑
     * - 디버그 빌드에서 assert로 프로그래머 오류 검출
     */
    _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
    Error CommandLineParser::AddOption(const CommandLineOption& option)
    {
        if (option.IsValid() == false)
        {
            ASSERT(false); // 유효하지 않은 입력
            return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
        }

        if (mLookup.count(option.mName) || mLookup.count(option.mShortName))
        {
            ASSERT(false); // 중복된 명령
            return MAKE_ERROR(eErrorCode::CommandLineArgumentsAlreadyExists);
        }


        // Keep a copy for help ordering
        mOptions.emplace_back(option);
        const size_t idx = mOptions.size() - 1;

        // Map names to the index
        mLookup[mOptions[idx].mName] = idx;
        mLookup[mOptions[idx].mShortName] = idx;
        return MAKE_ERROR(eErrorCode::Succeeded);
    }

    /**
     * @details
     * - argc/argv 유효성 확인 (argc>=1, argv!=nullptr)
     * - argv[1..]을 std::wstring 벡터로 변환
     * - 내부 Parse(vector) 호출
     */
    _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
    Error CommandLineParser::Parse(_In_ const int argc, _In_reads_opt_z_(argc) wchar_t* argv[])
    {
        if (argc < 1 || argv == nullptr)
        {
            ASSERT(false); // 명령줄 인자를 찾을 수 없음
            return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
        }

        std::vector<std::wstring> args;
        args.reserve(static_cast<size_t>(argc) - 1);
        for (int i = 1; i < argc; ++i)
            args.emplace_back(argv[i]);

        return Parse(args);
    }

    /**
     * @details
     * - Early-exit 옵션(--help, --version 등) 우선 처리
     * - 일반 옵션 순차 처리
     * - 각 토큰은 ProcessToken에서 개별 처리
     */
    _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
    Error CommandLineParser::Parse(_In_ const std::vector<std::wstring>& args)
    {
        // 1) Handle exit-causing options first, regardless of position
        for (size_t idx = 0; idx < args.size(); ++idx)
        {
            const auto& it = mLookup.find(args[idx]);
            if (it != mLookup.end() && mOptions[it->second].mCausesExit)
            {
                // Process and then exit immediately
                Error err = ProcessToken(args, idx);
                return err.IsFailed() ? err : MAKE_ERROR(eErrorCode::EarlyExit);
            }
        }

        // 2) No exit-causing option found: normal parsing
        size_t idx = 0;
        while (idx < args.size())
        {
            Error err = ProcessToken(args, idx);
            if (err.IsFailed())
                return err;
            ++idx;
        }
        return MAKE_ERROR(eErrorCode::Succeeded);
    }

    /**
     * @details
     * - mOptions 벡터를 순회하며 옵션별 도움말 라인 생성
     * - 옵션 이름, 단축 이름, 값 필요 유무, 설명 등을 포맷
     * - GitHub 저장소 링크 추가
     */
    std::wostream& CommandLineParser::PrintHelpTo(_In_ std::wostream& os) const
    {
        std::wstringstream stringstream;
        stringstream << L"CoTigraphy " << VERSION_STRING_WIDE << L"\n"
            << COPYRIGHT_WIDE << L"\n\n"
            << L"Usage:\n  CoTigraphy [options]\n\n"
            << L"Available options:\n";

        for (const auto& opt : mOptions)
        {
            std::wostringstream oss;
            oss << opt.mShortName << L", " << opt.mName;
            if (opt.mRequiresValue)
                oss << L" <value>";

            stringstream << L"  " << std::setw(28) << std::left << oss.str() << opt.mDescription << L"\n";
        }

        stringstream << L"\nFor more information, visit: https://github.com/ohsungsik/CoTigraphy\n";
        os << stringstream.str();
        return os;
    }

    /**
     * @details
     * - 토큰 존재 여부 확인 및 매핑
     * - 값 필요 시 다음 인자 소비 후 빈 문자열 검사
     * - 핸들러(option.mHandler) 호출
     * - index 매개변수는 값 소비 시 1 증가
     */
    _Success_(static_cast<eErrorCode>(return) == eErrorCode::Succeeded)
    Error CommandLineParser::ProcessToken(_In_ const std::vector<std::wstring>& args, _Inout_ size_t& index)
    {
        const auto& token = args[index];
        const auto& it = mLookup.find(token);
        if (it == mLookup.end())
            return MAKE_ERROR(eErrorCode::CommandLineArgumentNotFound);
        const CommandLineOption& option = mOptions[it->second];

        // Determine and consume value if required
        std::wstring value;
        if (option.mRequiresValue)
        {
            if (index + 1 >= args.size())
                return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
            value = args[index + 1];
            ++index;

            // 빈 문자열 값은 허용 안함
            if (value.empty())
                return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
        }

        // Dispatch handler
        if (option.mHandler)
            option.mHandler(value);

        return MAKE_ERROR(eErrorCode::Succeeded);
    }
}   // namespace CoTigraphy
