// \file GridCanvas.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
    class Grid;
    class Worm;

    struct GridCanvasContext
    {
        size_t mWidth = 0;
        size_t mHeight = 0;
        size_t mCellSize = 0;
        size_t mCellMargin = 0;
    };

    class GridCanvas final
    {
    public:
        explicit GridCanvas() noexcept;
        GridCanvas(const GridCanvas& other) = delete;
        GridCanvas(GridCanvas&& other) = delete;

        GridCanvas& operator=(const GridCanvas& rhs) = delete;
        GridCanvas& operator=(GridCanvas&& rhs) = delete;

        ~GridCanvas();

        void Create(const GridCanvasContext& gridCanvasContext);

        [[nodiscard]] uint8_t* GetBuffer() const noexcept { return mBuffer; }
        [[nodiscard]] size_t GetBufferSize() const noexcept { return mBufferSize; }
        void Clear(const COLORREF color) const;

        void DrawGrid(const Grid& grid) const;
        void DrawWorm(const Worm& worm) const;

    private:
        void DrawCell(_In_ const size_t& week, _In_ const size_t& day, _In_ const COLORREF color) const;

        /**
         * @brief 지정된 위치(y, x)에 스케일 적용된 셀을 RGBA 색상으로 그립니다.
         * @param week 열 인덱스 (0부터 시작)
         * @param day 행 인덱스 (0부터 시작)
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

        [[nodiscard]] RECT GetRect(_In_ const size_t& week, _In_ const size_t& day, _In_ const float scale) const;

    private:
        const size_t mBytesPerPixel = 4; // RGBA 4개 채널
        const size_t mAlignmentBits = mBytesPerPixel * CHAR_BIT; // 32 bit

        GridCanvasContext mGridCanvasContext;

        size_t mBufferSize = 0;
        uint8_t* mBuffer = nullptr;
    };
} // CoTigraphy
