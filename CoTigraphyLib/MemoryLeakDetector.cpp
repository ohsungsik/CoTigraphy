// \file MemoryLeakDetector.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "MemoryLeakDetector.hpp"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#include <sstream>

/// warning C4127: conditional expression is constant
/// > 이 경고는 'if (true)' 같은 상수 조건식에 대해 발생합니다.
/// > 본 코드에서는 `#ifdef _DEBUG`가 정의되지 않은 경우, 릴리즈 빌드에서
///    'OnProcessExit()' 함수 내부가 컴파일되더라도 아래 조건식은
///    const bool leakDetected = (_CrtDumpMemoryLeaks() == TRUE);
///    와 같은 형태로 "컴파일러가 보기에는 항상 일정한 상수"처럼 보인다.
///
///    예를 들어:
///      const bool leakDetected = (false);    // 릴리즈 모드에서 _CrtDumpMemoryLeaks는 빈 매크로 처리됨
///      if (leakDetected) { ... }             // 이 조건은 항상 false로 간주됨 > C4127 발생
///
///    비활성화가 필요한 이유:
///      _DEBUG 여부에 따라 코드가 완전히 제거되거나 포함되며,
///      해당 경고는 의도된 조건 분기이므로 무해하며 무시해도 안전하다.
#pragma warning(disable: 4127)	// conditional expression is constant

/// warning C4702: unreachable code
/// > 이 경고는 코드 흐름상 절대 도달할 수 없는 코드에 대해 발생한다.
/// > 예를 들어:
///      #ifndef _DEBUG
///          return;
///      #endif
///      const bool leakDetected = ...; // 컴파일러는 이 줄을 '절대 도달 불가'로 판단 → C4702 발생
///
///    비활성화가 필요한 이유:
///      디버그 모드 전용 코드와 릴리즈 코드가 같은 함수 안에 공존하기 때문에,
///      컴파일러는 릴리즈 모드에서 "이후 모든 줄은 실행 불가"로 경고하게 된다.
///      Initialize() 함수 전체를 분리하는 대신, 공통 구조를 유지하면서 개발 효율을 높이기 위해 무시합니다.
#pragma warning(disable: 4702)	// unreachable code

/// warning C26814: The const variable can be computed at compile-time. Consider using constexpr (con.5).
/// > 이 경고는 컴파일 타임에 상수로 판단할 수 있는 변수에 대해 const를 사용할 경우 발생한다.
/// > 예를 들어:
///      const int a = 0;
///    비활성화가 필요한 이유:
/// _CrtDumpMemoryLeaks 함수가 Debug, Release 모드에 따라 정의가 바뀐다.
///     Debug:
///         _ACRTIMP int __cdecl _CrtDumpMemoryLeaks(void);
///     Release:
///         #define _CrtDumpMemoryLeaks()               ((int)0)
///
/// 따라서 일관되게 constexpr을 적용할 수 없으므로 무시한다.
#pragma warning(disable: 26814) // The const variable 'leakDetected' can be computed at compile-time. Consider using constexpr (con.5).

namespace CoTigraphy
{
    MemoryLeakDetector::MemoryLeakDetector() noexcept
    = default;

    MemoryLeakDetector::~MemoryLeakDetector()
    = default;

    void MemoryLeakDetector::Initialize() noexcept
    {
        // 디버그 모드에서만 메모리 릭 체크
#ifndef _DEBUG
		return;
#endif

        const int ret = atexit(OnProcessExit);

        if (ret != 0 && IsDebuggerPresent())
        {
            OutputDebugStringW(L"[MemoryLeak] ERROR: Failed to register atexit handler.\n");
            DebugBreak(); // 메모리 릭 탐지 코드 등록 실패
        }
    }

    void MemoryLeakDetector::OnProcessExit() noexcept
    {
        // 메모리 릭 여부 판단
        const bool leakDetected = _CrtDumpMemoryLeaks();
        // 디버거가 연결된 상태에서 릭이 감지된 경우 중단
        const bool debuggerAttached = (IsDebuggerPresent() == TRUE);
        if (leakDetected && debuggerAttached)
        {
            DebugBreak(); // 메모리 릭 발생!!!
        }
    }
}   // namespace CoTigraphy
