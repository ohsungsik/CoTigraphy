// \file HandleLeakDetector.cpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "HandleLeakDetector.hpp"

#include <cstdlib>
#include <iomanip>
#include <string>
#include <sstream>

// warning C4702: unreachable code
// > 이 경고는 코드 흐름상 절대 도달할 수 없는 코드에 대해 발생한다.
// > 예를 들어:
//      #ifndef _DEBUG
//          return;
//      #endif
//      const bool leakDetected = ...; // 컴파일러는 이 줄을 '절대 도달 불가'로 판단 → C4702 발생
//
//    비활성화가 필요한 이유:
//      디버그 모드 전용 코드와 릴리즈 코드가 같은 함수 안에 공존하기 때문에,
//      컴파일러는 릴리즈 모드에서 "이후 모든 줄은 실행 불가"로 경고하게 된다.
//      Initialize() 함수 전체를 분리하는 대신, 공통 구조를 유지하면서 개발 효율을 높이기 위해 무시합니다.
#pragma warning(disable: 4702)	// unreachable code

namespace CoTigraphy
{
    DWORD gStartHandleCount = 0;

    HandleLeakDetector::HandleLeakDetector() noexcept
    = default;

    HandleLeakDetector::~HandleLeakDetector()
    = default;

    void HandleLeakDetector::Initialize() noexcept
    {
        // 디버그 모드에서만 핸들 릭 체크
#ifndef _DEBUG
        return;
#endif

        const DWORD pid = ::GetCurrentProcessId();
        const HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        ::GetProcessHandleCount(hProcess, &gStartHandleCount);
        CloseHandle(hProcess);

        const int ret = atexit(OnProcessExit);

        if (ret != 0 && IsDebuggerPresent())
        {
            DebugBreak(); // 메모리 릭 탐지 코드 등록 실패
        }
    }

    void HandleLeakDetector::OnProcessExit()
    {
        const DWORD pid = ::GetCurrentProcessId();
        const HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

        DWORD curHandleCount = 0;
        ::GetProcessHandleCount(hProcess, &curHandleCount);

        /// 일부 시스템 콜이나 라이브러리 내부에서 짧게 쓰는 핸들을 최종적으로 해제시키지
        /// 못하는 경우가 있으므로 허용치 설정
        /// 
        /// HTTPS 프로토콜을 사용하는 경우 CURL 내부에서 Schannel을 사용하여 처리됨
        /// 이 경우 종료 시 OS에서 해제를 관리하기 때문에 OS에의해 완전히 정리되기 전까지 핸들이 남아있어 핸들릭 처럼 보임...
        constexpr DWORD leakedThreshold = 100;
        const DWORD leakedCount = curHandleCount - gStartHandleCount;

        constexpr int labelWidth = 22; // ':' 이전까지의 라벨 고정 폭
        std::wostringstream oss;
        oss << L"[HandleLeak] ==== Handle Leak Report ========\n";
        oss << L"[HandleLeak]   " << std::left << std::setw(labelWidth)
            << L"PID" << L": " << pid << L"\n";
        oss << L"[HandleLeak]   " << std::left << std::setw(labelWidth)
            << L"Start Handle Count" << L": " << gStartHandleCount << L"\n";
        oss << L"[HandleLeak]   " << std::left << std::setw(labelWidth)
            << L"End Handle Count" << L": " << curHandleCount << L"\n";
        oss << L"[HandleLeak]   " << std::left << std::setw(labelWidth)
            << L"Leaked Handles" << L": " << leakedCount;

        if (leakedCount > leakedThreshold)
            oss << L"  **** OVER THRESHOLD (limit=" << leakedThreshold << L")";

        oss << L"\n[HandleLeak] ================================\n";

        const std::wstring msg = oss.str();
        OutputDebugStringW(msg.c_str());

        if (leakedCount > leakedThreshold && IsDebuggerPresent())
            DebugBreak();
    }
}   // namespace CoTigraphy
