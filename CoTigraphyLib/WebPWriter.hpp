// \file WebPWriter.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <webp/encode.h>
#include <webp/mux.h>

namespace CoTigraphy
{
    /**
     * @brief WebP 애니메이션 프레임을 생성하고 저장하는 클래스
     * @details
     * - libwebp를 이용하여 RGBA 버퍼 데이터를 WebP 애니메이션으로 저장
     * - 프레임 단위로 RGBA 데이터를 입력받아 애니메이션을 구성
     * - Initialize -> AddFrame 반복 -> SaveToFile 순으로 사용
     */
    class WebPWriter final
    {
    public:
        explicit WebPWriter() noexcept;

        WebPWriter(const WebPWriter& other) = delete;
        WebPWriter(WebPWriter&& other) = delete;

        WebPWriter& operator=(const WebPWriter& rhs) = delete;
        WebPWriter& operator=(WebPWriter&& rhs) = delete;

        ~WebPWriter();

        /**
         * @brief WebPWriter를 초기화하고 버퍼 크기 지정
         * @param width 출력 애니메이션 가로 해상도 (픽셀)
         * @param height 출력 애니메이션 세로 해상도 (픽셀)
         * @pre width > 0 && height > 0
         * @post 내부 Encoder가 초기화되고 프레임 추가 가능 상태가 됨
         */
        void Initialize(_In_ const size_t& width, _In_ const size_t& height);

        /**
         * @brief RGBA 프레임을 애니메이션에 추가
         * @param buffer RGBA8888 (4바이트) 포맷의 프레임 픽셀 데이터
         * @return 성공 여부 (true = 성공, false = 실패)
         * @pre Initialize() 이후에만 호출 가능
         * @post 내부 애니메이션 프레임이 1개 추가됨
         * @warning buffer 크기는 width × height × 4 바이트이어야 함
         */
        bool AddFrame(_In_ const uint8_t* const buffer);

        /**
         * @brief 지금까지 추가된 프레임을 WebP 애니메이션으로 저장
         * @param fileName 저장할 파일 경로 (.webp 확장자 권장)
         * @pre 최소 1개의 프레임이 AddFrame()을 통해 등록되어 있어야 함
         * @post 지정된 경로에 WebP 애니메이션 파일이 생성됨
         */
        void SaveToFile(_In_ const std::wstring& fileName) const;

    private:
        const size_t mFrameDelayMs = 80; // 프레임 간 딜레이 (단위: ms)
        size_t mEncodedFrame = 0; // 현재까지 인코딩된 프레임 수
        WebPAnimEncoder* mEncoder = nullptr; // libwebp 애니메이션 인코더 핸들
        WebPConfig mConfig{}; // WebP 인코딩 설정 정보
        WebPPicture mPicture{}; // 현재 프레임 데이터를 담는 구조체
    };
}
