#include "pch.hpp"

TEST(command_line_parser, Init)
{
    
}

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
