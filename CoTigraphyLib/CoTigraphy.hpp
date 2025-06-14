// \file CoTigraphy.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
    // 전방 선언
    class CommandLineParser;

    /**
     * @brief 프로그램 전체 초기화 함수
     * @param[out] githubToken GitHub Personal Access Token (명령줄에서 추출)
     * @param[out] userName GitHub 사용자 이름 (명령줄에서 추출)
     * @param[out] outputPath WebP 출력 경로
     * @return 성공 시 Succeeded, 실패 시 에러 코드
     * @details
     * - 메모리/핸들 릭 감지기 초기화
     * - 명령줄 파서 초기화 및 파싱 수행
     */
    Error Initialize(_Out_opt_ std::wstring& githubToken, _Out_opt_ std::wstring& userName,
                     _Out_opt_ std::wstring& outputPath);

    /**
     * @brief 명령줄 파서 구성 함수
     * @param[in,out] commandLineParser 파서를 구성할 CommandLineParser 인스턴스
     * @param[out] githubToken 사용자 입력으로 받은 토큰 값
     * @param[out] userName 사용자 입력으로 받은 이름 값
     * @param[out] outputPath 사용자 입력으로 받은 WebP 출력 경로
     * @return 성공 시 Succeeded, 실패 시 에러 코드
     * @details
     * - "--help", "--version", "--token", "--userName" 옵션을 등록
     */
    Error SetupCommandLineParser(_In_ CoTigraphy::CommandLineParser& commandLineParser,
                                 _Out_opt_ std::wstring& githubToken, _Out_opt_ std::wstring& userName,
                                 _Out_opt_ std::wstring& outputPath);


    /**
     * @brief GitHub Contribution calendar를 이용해 애니메이션 WebP를 생성
     * @param[in] githubToken Personal Access Token
     * @param[in] userName GitHub 사용자 이름
     * @param[out] outputPath WebP 출력 경로
     * @return 성공 시 Succeeded, 실패 시 에러 코드
     * @details
     * - API로 기여 정보 가져오기 -> Worm 시뮬레이션 -> WebP 프레임 생성 -> 파일 저장
     */
    Error Run(_In_ const std::wstring& githubToken, _In_ const std::wstring& userName,
              _In_ const std::wstring& outputPath);
} // namespace CoTigraphy
