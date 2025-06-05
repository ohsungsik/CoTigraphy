// \file ErrorCode.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once


#include <winerror.h>

///
/// HRESULT를 이용해서 Windows error code(win32 api), DirectX error code, yuwoll error code를 포함한 모든 에러 코드를 통합한다.
///
/// HRESULT는 아래와 같이 정의되어 있다 (출처 winerror.h(28984))
///
/// //  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
/// //  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
/// // +-+-+-+-+-+---------------------+-------------------------------+
/// // |S|R|C|N|r|    Facility         |               Code            |
/// // +-+-+-+-+-+---------------------+-------------------------------+
/// //
/// //  where
/// //
/// //      S - Severity - indicates success/fail
/// //
/// //          0 - Success
/// //          1 - Fail (COERROR)
/// //
/// //      R - reserved portion of the facility code, corresponds to NT's
/// //              second severity bit.
/// //
/// //      C - reserved portion of the facility code, corresponds to NT's
/// //              C field.
/// //
/// //      N - reserved portion of the facility code. Used to indicate a
/// //              mapped NT status value.
/// //
/// //      r - reserved portion of the facility code. Reserved for internal
/// //              use. Used to indicate HRESULT values that are not status
/// //              values, but are instead message ids for display strings.
/// //
/// //      Facility - is the facility code
/// //
/// //      Code - is the facility's status code
/// //
///
///	대부분의 경우 위의 양식을 이용하지만 몇몇 개의 함수는 NTSTATUS 코드와의 호환성을 고려해서 변경된 HRESULT양식을 따른다.
///	변경된 HRESULT의 양식은 아래와 같다
///
/// // Note: There is a slightly modified layout for HRESULT values below,
/// //        after the heading "COM Error Codes".
/// //
/// // Search for "**** Available SYSTEM error codes ****" to find where to
/// // insert new error codes
/// //
/// //  Values are 32 bit values laid out as follows:
/// //
/// //   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
/// //   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
/// //  +---+-+-+-----------------------+-------------------------------+
/// //  |Sev|C|R|     Facility          |               Code            |
/// //  +---+-+-+-----------------------+-------------------------------+
/// //
/// //  where
/// //
/// //      Sev - is the severity code
/// //
/// //          00 - Success
/// //          01 - Informational
/// //          10 - Warning
/// //          11 - Error
/// //
/// //      C - is the Customer code flag
/// //
/// //      R - is a reserved bit
/// //
/// //      Facility - is the facility code
/// //
/// //      Code - is the facility's status code
///
///	예를 들어 변경된 HRESULT의 값을 사용하는 Facility는 FACILITY_PIDGENX가 있다.
///
///
/// 두 HRESULT 양식을 모두 지원하며 YuPad 프로젝트에서 사용하는 양식으로 확장한다.
///
///
///	에러 코드 계산 방법
///
///	Severity를 1, Customer 코드 플래그와 고유한 Facility 값을 설정하여 HRESULT 값을 생성한다.
///
///	에러 코드는 다음과 같이 계산된다:
///
/// ```cpp
/// // Severity (1) << 31 | Customer Flag (1) << 29 | Facility Code << 16 | Error Code
/// HRESULT hr = (1 << 31) | (1 << 29) | (Facility << 16) | Code;
/// ```
///
/// 시작 HRESULT 값: 각 프로젝트에서 오류 코드를 정의할 때 이 값을 시작으로 순차적으로 증가시킨다.
///
///
///
/// Facility
///
///	Facility Code: 각 프로젝트에 고유한 Facility 코드를 부여하여 오류 코드를 구분한다.
///	11bit를 사용한다.
///
///	FACILITY_WMAAECMA(값: 1996)이 최대로 정의된 값이므로 해당 값 이상을 이용한다.
///
///	최대 Facility값은 2048까지 가능하다
///
/// ```cpp
/// Facility = 1998; // 0x7CE
/// Code = 1;
/// HRESULT hr = (1 << 31) | (1 << 29) | (1998 << 16) | 1;
/// // 계산하면 hr = 0xA7CE0001
/// ```
///
///	코드는 16비트로 최대 65536까지 가능하다
///	모든 코드의 0은 성공이다
///
///	Reference
/// HRESULT 양식에 대한 설명: [Wikipedia - HRESULT](https://en.wikipedia.org/wiki/HRESULT)
/// HRESULT 양식에 대한 간단한 설명: [Microsoft Docs - HRESULT](https://learn.microsoft.com/en-us/office/client-developer/outlook/mapi/hresult)
/// 정의된 HRESULT_FACILITY 값 목록: [Microsoft Docs - HRESULT Facility Codes](https://learn.microsoft.com/en-us/windows/win32/api/winerror/nf-winerror-hresult_facility)


namespace CoTigraphy
{
    /**
     * \brief 에러 코드에서 프로젝트 구분을 위해 사용하는 구분자
     *			에러 코드의 16-26 비트에 위치한다
     *			최대 2048까지 사용가능
     */
    enum class eFacility : unsigned short
    {
        CoTigraphy = 1997,
    };

    constexpr int HRESULT_SEVERITY_BIT = 31;
    constexpr int HRESULT_CUSTOMER_BIT = 29;
    constexpr int HRESULT_FACILITY_SHIFT = 16;
    constexpr int HRESULT_CUSTOMER_FLAG = 1;

    // 에러 코드(HRESULT)를 생성하는 매크로
#define MAKE_ERRORCODE(facility) ((SEVERITY_ERROR << HRESULT_SEVERITY_BIT) | (HRESULT_CUSTOMER_FLAG  << HRESULT_CUSTOMER_BIT) | ((static_cast<unsigned short>(facility)) << HRESULT_FACILITY_SHIFT))
#define MAKE_SUCCESSCODE(facility) ((SEVERITY_SUCCESS << HRESULT_SEVERITY_BIT) | (HRESULT_CUSTOMER_FLAG  << HRESULT_CUSTOMER_BIT) | ((static_cast<unsigned short>(facility)) << HRESULT_FACILITY_SHIFT))

    enum class eErrorCode : HRESULT
    {
        // Common Error code
        Succeeded = NOERROR,

        // CoTigraphy 프로젝트 성공 코드 시작
        EarlyExit = MAKE_SUCCESSCODE(eFacility::CoTigraphy),        // 조기 반환, 명령줄 인자의 --version 처럼 처리 후 이후 로직을 실행하지 않아야 하는 경우 이 코드를 반환한다.

        // CoTigraphy 프로젝트 에러 코드 시작 값
        InvalidArguments = MAKE_ERRORCODE(eFacility::CoTigraphy),   // 유효하지 않은 파라미터
        CommandLineArgumentsAlreadyExists,                          // 이미 추가된 커맨드 라인 파라미터

    };

    static_assert(sizeof(eErrorCode) == sizeof(HRESULT));   // eErrorCode는 HRESULT의 확장이기 때문에 크기가 동일해야 한다.

#undef MAKE_ERRORCODE
}
