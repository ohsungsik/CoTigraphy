// \file Assert.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 명시적 프로그램 종료 - Fail Fast 지원 (Release에서도 크래시 보장)
/// @details Contract 위반 시 즉시 프로그램을 종료하여 시스템 안정성 확보
////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXPLICIT_CRASH() do { \
    ::OutputDebugStringW(L"[EXPLICIT_CRASH] Contract 위반으로 인한 프로그램 종료.\n");     \
    if (::IsDebuggerPresent()) __debugbreak();                                          \
    ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(-1));                   \
} while(false)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief ASSERT 매크로
/// @param expr 검증할 조건식
/// @details 조건이 거짓일 경우 디버거 정보 출력 후 프로그램 종료
////////////////////////////////////////////////////////////////////////////////////////////////////
#define ASSERT(expr)                                                                        \
    do {                                                                                    \
        if (!(expr))                                                                        \
        {                                                                                   \
            std::wostringstream debugStream;                                                \
            debugStream << L"[ASSERT 실패] " << L#expr << L"\n"                              \
                       << L"파일: " << __FILEW__ << L"\n"                                    \
                       << L"라인: " << __LINE__ << L"\n";                                    \
            ::OutputDebugStringW(debugStream.str().c_str());                                \
            EXPLICIT_CRASH();                                                               \
        }                                                                                   \
    } while (false)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 메시지 포함 ASSERT 매크로
/// @param expr 검증할 조건식
/// @param msg 추가 메시지 (Wide String)
/// @details 조건이 거짓일 경우 사용자 정의 메시지와 함께 디버거 정보 출력 후 프로그램 종료
////////////////////////////////////////////////////////////////////////////////////////////////////
#define ASSERT_MSG(expr, msg)                                                               \
    do {                                                                                    \
        if (!(expr))                                                                        \
        {                                                                                   \
            std::wostringstream debugStream;                                                \
            debugStream << L"[ASSERT 실패] " << L#expr << L"\n"                              \
                       << L"메시지: " << (msg) << L"\n"                                      \
                       << L"파일: " << __FILEW__ << L"\n"                                    \
                       << L"라인: " << __LINE__ << L"\n";                                    \
            ::OutputDebugStringW(debugStream.str().c_str());                                 \
                EXPLICIT_CRASH();                                                            \
        }                                                                                    \
    } while (false)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Contract 전제조건 검증 매크로
/// @param condition 전제조건 식
/// @details 함수 진입 시 전제조건 검증용 (Strong Contract 지원)
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PRECONDITION(condition) ASSERT_MSG((condition), L"전제조건 위반")

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Contract 후조건 검증 매크로
/// @param condition 후조건 식
/// @details 함수 종료 시 후조건 검증용 (Strong Contract 지원)
////////////////////////////////////////////////////////////////////////////////////////////////////
#define POSTCONDITION(condition) ASSERT_MSG((condition), L"후조건 위반")

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Contract 불변조건 검증 매크로
/// @param condition 불변조건 식
/// @details 객체 상태 불변조건 검증용 (Strong Contract 지원)
////////////////////////////////////////////////////////////////////////////////////////////////////
#define INVARIANT(condition) ASSERT_MSG((condition), L"불변조건 위반")

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 도달 불가능한 코드 경로 표시 매크로
/// @details 논리적으로 실행되어서는 안 되는 코드 경로에 사용
////////////////////////////////////////////////////////////////////////////////////////////////////
#define UNREACHABLE() ASSERT_MSG(false, L"도달 불가능한 코드 경로 실행됨")

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 미구현 기능 표시 매크로
/// @details 개발 중인 기능에 대한 명시적 표시
////////////////////////////////////////////////////////////////////////////////////////////////////
#define NOT_IMPLEMENTED() ASSERT_MSG(false, L"미구현 기능 호출됨")
