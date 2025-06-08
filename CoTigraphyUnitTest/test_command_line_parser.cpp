// \file test_command_line_parser.cpp
// \last_updated 2025-06-08
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include <CommandLineParser.hpp>

using namespace CoTigraphy;

// 테스트 케이스 데이터: CommandLineOption 상속 + expectedIsValid 추가
struct IsValidTestCase : CommandLineOption
{
    bool expectedIsValid;
};

class CommandLineOptionTest : public ::testing::TestWithParam<IsValidTestCase>
{
};

// 테스트 본문
TEST_P(CommandLineOptionTest, IsValidReturnsExpectedValue)
{
    const auto& param = GetParam();

    EXPECT_EQ(param.IsValid(), param.expectedIsValid)
        << L"mName='" << param.mName
        << L"' mShortName='" << param.mShortName
        << L"' mDescription='" << param.mDescription << L"'";
}

// 테스트 케이스 목록 등록
INSTANTIATE_TEST_SUITE_P(IsValidTests, CommandLineOptionTest, ::testing::Values(
                             // All valid
                             IsValidTestCase{ { L"--help", L"-h", L"Show help", false, false, nullptr }, true },
                             // mName empty
                             IsValidTestCase{ { L"", L"-h", L"Show help", false, false, nullptr }, false },
                             // mShortName empty
                             IsValidTestCase{ { L"--help", L"", L"Show help", false, false, nullptr }, false },
                             // mDescription empty
                             IsValidTestCase{ { L"--help", L"-h", L"", false, false, nullptr }, false },
                             // All empty
                             IsValidTestCase{ { L"", L"", L"", false, false, nullptr }, false }
                         ));


struct CommandLineParserTestCase
{
    std::vector<std::wstring> args;
    eErrorCode expectedErrorCode;
    bool expectedHandlerCalled;
    std::wstring expectedValue;
};

class CommandLineParserParseTest : public ::testing::TestWithParam<CommandLineParserTestCase>
{
protected:
    CommandLineParser parser;
    std::wstring capturedValue;
    bool handlerCalled = false;

    void SetUp() override
    {
        capturedValue.clear();
        handlerCalled = false;

        // 기본 옵션 추가
        AddTestOption(L"--test", L"-t", false, false);
        AddTestOption(L"--exit", L"-e", false, true);
        AddTestOption(L"--with-value", L"-w", true, false);

        // 일반 옵션인데 handler == nullptr
        std::ignore = parser.AddOption(CommandLineOption{
            L"--no-handler", L"-n", L"Test option", false, false, nullptr
            });

        // mShortName만 있는 옵션
        std::ignore = parser.AddOption(CommandLineOption{
            L"-s", L"", L"Short only", false, false,
            [&](const std::wstring& val)
            {
                handlerCalled = true;
                capturedValue = val;
            }
            });

        // 옵션 이름/짧은이름 같음
        std::ignore = parser.AddOption(CommandLineOption{
            L"--same", L"--same", L"Same names", true, false,
            [&](const std::wstring& val)
            {
                handlerCalled = true;
                capturedValue = val;
            }
            });
    }

    void AddTestOption(const std::wstring& name, const std::wstring& shortName, bool requiresValue, bool causesExit)
    {
        CommandLineOption opt;
        opt.mName = name;
        opt.mShortName = shortName;
        opt.mDescription = L"Test option";
        opt.mRequiresValue = requiresValue;
        opt.mCausesExit = causesExit;
        opt.mHandler = [&](const std::wstring& val)
            {
                handlerCalled = true;
                capturedValue = val;
            };
        parser.AddOption(opt);
    }
};

TEST_P(CommandLineParserParseTest, ReturnsExpectedResults)
{
    const CommandLineParserTestCase& param = GetParam();

    const Error err = parser.Parse(param.args);

    if (param.args.empty() == false)
        SCOPED_TRACE(::testing::Message() << "Args: " << param.args[0]);

    EXPECT_EQ(err.GetErrorCode(), param.expectedErrorCode);
    EXPECT_EQ(handlerCalled, param.expectedHandlerCalled);
    EXPECT_EQ(capturedValue, param.expectedValue);
}

INSTANTIATE_TEST_SUITE_P(ParseTests, CommandLineParserParseTest, ::testing::Values(
    // Normal option processed
    CommandLineParserTestCase{ { L"--test" }, eErrorCode::Succeeded, true, L"" },

    // Exit option triggers EarlyExit
    CommandLineParserTestCase{ { L"--exit" }, eErrorCode::EarlyExit, true, L"" },

    // Unknown option
    CommandLineParserTestCase{ { L"--unknown" }, eErrorCode::CommandLineArgumentNotFound, false, L"" },

    // Option with value processed
    CommandLineParserTestCase{ { L"--with-value", L"myvalue" }, eErrorCode::Succeeded, true, L"myvalue" },

    // Option with value missing argument
    CommandLineParserTestCase{ { L"--with-value" }, static_cast<eErrorCode>(E_INVALIDARG), false, L"" },

    // 인자 없는 Parse (빈 vector)
    CommandLineParserTestCase{ {}, eErrorCode::Succeeded, false, L"" },

    // 종료 옵션인데 값이 필요한 경우 (빈 문자열 → E_INVALIDARG)
    CommandLineParserTestCase{ { L"--with-value", L"" }, static_cast<eErrorCode>(E_INVALIDARG), false, L"" },

    // 일반 옵션인데 handler == nullptr
    CommandLineParserTestCase{ { L"--no-handler" }, eErrorCode::Succeeded, false, L"" },

    // mShortName만 있는 옵션
    CommandLineParserTestCase{ { L"-s", L"--slient" }, eErrorCode::CommandLineArgumentNotFound, false, L"" },

    // 옵션 이름/짧은이름 같음
    CommandLineParserTestCase{ { L"--same", L"val" }, eErrorCode::Succeeded, true, L"val" },

    // 옵션 순서 반전 (종료 옵션 뒤쪽에 있음)
    CommandLineParserTestCase{ { L"--test", L"--exit" }, eErrorCode::EarlyExit, true, L"" },

    // 등록되지 않은 인자 무시됨 (처음 옵션 정상 처리 후 unknown 에서 실패)
    CommandLineParserTestCase{ { L"--test", L"--unknown" }, eErrorCode::CommandLineArgumentNotFound, false, L"" },

    // 매우 긴 인자 문자열
    CommandLineParserTestCase{ { L"--with-value", std::wstring(10000, L'A') }, eErrorCode::Succeeded, true, std::wstring(10000, L'A') }
));


// 정상 옵션 등록
// 중복 옵션 등록 실패
// 인자 없는 Parse 실패
// 종료 옵션 정상 동작
// 종료 옵션 중복 호출 오류
// 종료 옵션인데 값이 필요한 경우 처리
// 일반 옵션 처리 - 값 없는 경우
// 일반 옵션 처리 - 값 포함
// 일반 옵션 중복 처리 오류
// 일반 옵션인데 값 누락
// 등록되지 않은 인자는 무시됨
// AddOption() - IsValid()가 false인 옵션 등록
// Parse(argc, argv) 경로
// 종료 옵션이면서 값이 비어 있을 때
// 일반 옵션 - Handler가 null일 때 정상 처리
// 종료 옵션인데 value 누락된 경우
// PrintHelp()에 대한 표면 테스트 (출력 리디렉션)
// mShortName만 있는 옵션 (mName 없음)
// option.mDescription가 비어있는 경우
// 옵션의 이름과 짧은 이름이 모두 같은 경우
// 옵션 순서 반전 (종료 옵션이 뒤에 있을 때)
// 옵션 핸들러가 예외를 던지는 경우
// 명령행 인자가 너무 많은 경우 (성능/안정성 보장 목적)
// Parse 함수 경계값 테스트
// 분기 조건 완전 커버리지
//  CommandLineOption::IsValid() 조건 조합
//  - mName.empty() == true && mShortName.empty() == false && mDescription.empty() == false
//  - mName.empty() == false && mShortName.empty() == true && mDescription.empty() == false  
//  - mName.empty() == false && mShortName.empty() == false && mDescription.empty() == true
//  - 모든 조건이 true인 경우 (false 반환)
// Parse 함수 경계값 테스트
//  argc = 0이지만 argv != nullptr인 경우
//  argc > 0이지만 argv == nullptr인 경우 (현재 ASSERT로만 처리됨)
//  commandLineArguments.size() == 0인 경우
//  매우 긴 인자 문자열 (메모리/성능 테스트)
// 옵션 매칭 로직
//  부분 일치하는 옵션 (예: "--help"와 "--h" 구분)
//  대소문자 구분 테스트
//  특수문자가 포함된 옵션명
//  공백이 포함된 옵션명/값
// 값 처리 경계 케이스
//  빈 문자열 값이 전달되는 경우 (mRequiresValue = true)
//  값에 특수문자/유니코드가 포함된 경우
//  값이 다른 옵션과 같은 형태인 경우 (예: 값이 "-h"인 경우)
// 메모리 및 예외 안전성
//  vector::emplace_back에서 메모리 할당 실패 시뮬레이션
//  std::function 복사 시 예외 발생
//  unordered_map 할당 실패
// 동시성 및 재진입 테스트
//  const 함수들의 스레드 안전성 (Parse 함수 동시 호출)
//  mHandler 실행 중 다른 Parse 호출
// AddOption에서의 중복 검사
//  (commandLineOption.mName == option.mName) == true && (commandLineOption.mShortName == option.mShortName) == false
//  (commandLineOption.mName == option.mName) == false && (commandLineOption.mShortName == option.mShortName) == true
//  둘 다 true인 경우
//  둘 다 false인 경우
// Parse에서의 복합 조건
//  (token == option.mName || token == option.mShortName) && option.mCausesExit
//  각 조건의 모든 true/false 조합
//  (i + 1 >= commandLineArguments.size()) 조건의 경계값
// PrintHelp 출력 검증
//  옵션이 없을 때 출력
//  mRequiresValue가 true/false일 때 출력 형식 차이
//  매우 긴 옵션명/설명일 때 포맷팅
//  유니코드 문자가 포함된 설명
// 상태 변경 테스트
//  AddOption 호출 후 다시 Parse 호출 (상태 일관성)
//  Parse 여러 번 호출 (멱등성)
//  생성자/소멸자 호출 순서
