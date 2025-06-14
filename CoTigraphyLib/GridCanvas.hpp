// \file GridCanvas.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
    class Grid;
    class Worm;

    /**
     * @brief GridCanvas를 생성할 때 필요한 캔버스 구성 정보 구조체
     */
    struct GridCanvasContext
    {
        size_t mWidth = 0; // 전체 캔버스의 가로 픽셀 수
        size_t mHeight = 0; // 전체 캔버스의 세로 픽셀 수
        size_t mCellSize = 0; // 각 셀의 한 변 길이 (픽셀 단위)
        size_t mCellMargin = 0; // 셀 간의 간격 (픽셀 단위)
    };

    /**
     * @brief RGBA 버퍼 기반의 그리드 렌더링 캔버스를 관리하는 클래스
     * @details
     * - 외부에서 전달된 Grid/Worm 정보를 기반으로 그리드 셀과 지렁이(Worm)를 그림
     * - 내부적으로 RGBA 포맷의 메모리 버퍼를 직접 관리
     */
    class GridCanvas final
    {
    public:
        explicit GridCanvas() noexcept;
        GridCanvas(const GridCanvas& other) = delete;
        GridCanvas(GridCanvas&& other) = delete;

        GridCanvas& operator=(const GridCanvas& rhs) = delete;
        GridCanvas& operator=(GridCanvas&& rhs) = delete;

        /**
         * @brief 소멸자 (내부 버퍼 해제)
         */
        ~GridCanvas();

        /**
         * @brief 내부 캔버스 버퍼를 생성하고 초기화
         * @param gridCanvasContext 캔버스 크기, 셀 크기, 마진 등의 초기 구성 정보
         * @pre 각 항목이 유효한 값이어야 함 (0이 아니어야 함)
         */
        void Create(const GridCanvasContext& gridCanvasContext);

        /**
         * @brief RGBA 버퍼의 포인터를 반환
         * @return 내부 픽셀 데이터가 저장된 버퍼 포인터
         */
        [[nodiscard]] const uint8_t* GetBuffer() const noexcept { return mBuffer; }

        /**
         * @brief 버퍼 전체를 지정된 색상으로 클리어
         * @param color COLORREF (R, G, B) 값으로 채울 색상
         */
        void Clear(const COLORREF color) const;

        /**
         * @brief 주어진 Grid 객체를 기반으로 그리드를 그림
         * @param grid Grid 정보 객체
         */
        void DrawGrid(const Grid& grid) const;

        /**
         * @brief 주어진 Worm 객체를 기반으로 지렁이를 그림
         * @param worm Worm 정보 객체
         */
        void DrawWorm(const Worm& worm) const;

    private:
        /**
         * @brief 지정된 위치에 하나의 셀을 정사각형 형태로 그림
         * @param week 열 인덱스 (0부터 시작, 주 단위)
         * @param day 행 인덱스 (0부터 시작, 요일)
         * @param color 그릴 색상 (COLORREF)
         */
        void DrawCell(_In_ const size_t& week, _In_ const size_t& day, _In_ const COLORREF color) const;

        /**
         * @brief 지정된 위치(y, x)에 스케일 적용된 셀을 RGBA 색상으로 그립니다.
         * @param week 열 인덱스 (주)
         * @param day 행 인덱스 (요일)
         * @param scale 셀 크기 조절 비율 (0.0f ~ 1.0f)
         * @param color COLORREF 형식의 색상 (RGB)
         * @pre y < mGridCanvasContext.mHeight
         * @pre x < mGridCanvasContext.mWidth
         * @pre scale > 0.0f && scale <= 1.0f
         * @details
         * - RECT 좌표를 계산 후, 실제 캔버스 크기를 초과하지 않도록 Clamp 처리함.
         * - RGBA 4바이트 버퍼에 직접 픽셀 색상 정보를 저장하여 렌더링 성능 최적화.
         * - RECT 내부 픽셀을 순회하며 지정된 색상으로 칠함.
         */
        void DrawCellWithScale(_In_ const size_t& week, _In_ const size_t& day, _In_ const float scale,
                               _In_ const COLORREF color) const;

        /**
         * @brief 스케일 적용 셀의 RECT 좌표 영역을 계산
         * @param week 열 인덱스 (주 단위)
         * @param day 행 인덱스 (요일)
         * @param scale 셀 크기 비율
         * @return 해당 셀의 픽셀 영역 (RECT)
         */
        [[nodiscard]] RECT GetRect(_In_ const size_t& week, _In_ const size_t& day, _In_ const float scale) const;

    private:
        const size_t mBytesPerPixel = 4; // RGBA 4개 채널
        const size_t mAlignmentBits = mBytesPerPixel * CHAR_BIT; // 32 bit

        GridCanvasContext mGridCanvasContext; // 현재 캔버스 구성 정보 

        size_t mBufferSize = 0; // 전체 버퍼 크기
        uint8_t* mBuffer = nullptr; // RGBA 버퍼 포인터
    };
} // CoTigraphy
