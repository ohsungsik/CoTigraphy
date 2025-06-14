// \file GridCanvas.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "GridCanvas.hpp"

#include "Grid.hpp"
#include "Worm.hpp"

namespace CoTigraphy
{
    GridCanvas::GridCanvas() noexcept
    = default;

    GridCanvas::~GridCanvas()
    {
        _aligned_free(mBuffer);
        mBuffer = nullptr;
    }

    void GridCanvas::Create(const GridCanvasContext& gridCanvasContext)
    {
        PRECONDITION(gridCanvasContext.mWidth != 0);
        PRECONDITION(gridCanvasContext.mHeight != 0);
        PRECONDITION(gridCanvasContext.mCellSize != 0);
        PRECONDITION(gridCanvasContext.mCellMargin != 0);

        mGridCanvasContext = gridCanvasContext;

        mBufferSize = mGridCanvasContext.mWidth * mGridCanvasContext.mHeight * mBytesPerPixel;

        mBuffer = static_cast<uint8_t*>(_aligned_malloc(mBufferSize, mAlignmentBits));

        POSTCONDITION(mBuffer);
    }

    void GridCanvas::Clear(const COLORREF color) const
    {
        PRECONDITION(mBuffer != nullptr);
        PRECONDITION(mBufferSize != 0);

        for (size_t i = 0; i < mBufferSize; i += mBytesPerPixel)
        {
            uint8_t* buffer = mBuffer + i;
            ASSERT(buffer != nullptr);

            *(buffer + 0) = GetRValue(color);
            *(buffer + 1) = GetGValue(color);
            *(buffer + 2) = GetBValue(color);
            *(buffer + 3) = 0xff;
        }
    }

    void GridCanvas::DrawGrid(const Grid& grid) const
    {
        for (size_t week = 0; week < grid.GetWeekCount(); ++week)
        {
            for (size_t day = 0; day < grid.GetDayCount(); ++day)
            {
                const GridCell& cell = grid.GetCell(week, day);
                DrawCell(cell.mWeek, cell.mDay, cell.mColor);
            }
        }
    }

    void GridCanvas::DrawWorm(const Worm& worm) const
    {
        const std::array<WormSegment, 4>& wormSegments = worm.GetWorm();
        for (const auto& wormInfo : wormSegments)
        {
            DrawCellWithScale(wormInfo.mPoint.x, wormInfo.mPoint.y, wormInfo.mScale, wormInfo.mColor);
        }
    }

    void GridCanvas::DrawCell(_In_ const size_t& week, _In_ const size_t& day, _In_ const COLORREF color) const
    {
        DrawCellWithScale(week, day, 1.0f, color);
    }

    void GridCanvas::DrawCellWithScale(_In_ const size_t& week, _In_ const size_t& day, _In_ const float scale,
        _In_ const COLORREF color) const
    {
        PRECONDITION(mGridCanvasContext.mCellSize != 0);

        const RECT rect = GetRect(week, day, scale);

        ASSERT(mGridCanvasContext.mWidth <= static_cast<size_t>(std::numeric_limits<LONG>::max()));
        ASSERT(mGridCanvasContext.mHeight <= static_cast<size_t>(std::numeric_limits<LONG>::max()));

        const LONG width = static_cast<LONG>(mGridCanvasContext.mWidth);
        const LONG height = static_cast<LONG>(mGridCanvasContext.mHeight);

        const LONG left = std::max<LONG>(0, rect.left);
        const LONG right = std::min(width, rect.right);
        const LONG top = std::max<LONG>(0, rect.top);
        const LONG bottom = std::min(height, rect.bottom);

        // 바이트 접근 오프셋 정의
        constexpr size_t red = 0;
        constexpr size_t green = 1;
        constexpr size_t blue = 2;
        constexpr size_t alpha = 3;

        // RGB을 COLORREF로 부터 얻어온다
        const uint8_t r = GetRValue(color);
        const uint8_t g = GetGValue(color);
        const uint8_t b = GetBValue(color);
        constexpr uint8_t a = 255;

        for (size_t yPos = top; yPos < static_cast<size_t>(bottom); ++yPos)
        {
            for (size_t xPos = left; xPos < static_cast<size_t>(right); ++xPos)
            {
                const size_t index = (yPos * mGridCanvasContext.mWidth + xPos)
                    * mBytesPerPixel;
                mBuffer[index + red] = r;
                mBuffer[index + green] = g;
                mBuffer[index + blue] = b;
                mBuffer[index + alpha] = a;
            }
        }
    }

#pragma warning(disable: 4267)  // conversion from 'size_t' to 'const LONG', possible loss of data)
    RECT GridCanvas::GetRect(_In_ const size_t& week, _In_ const size_t& day, _In_ const float scale) const
    {
        PRECONDITION(week < mGridCanvasContext.mWidth);
        PRECONDITION(day < mGridCanvasContext.mHeight);        
        PRECONDITION(0.0f < scale);
        PRECONDITION(scale <= 1.0f);

        ASSERT(week <= static_cast<size_t>(std::numeric_limits<LONG>::max()));
        ASSERT(day <= static_cast<size_t>(std::numeric_limits<LONG>::max()));
        ASSERT(mGridCanvasContext.mCellMargin <= static_cast<size_t>(std::numeric_limits<LONG>::max()));
        ASSERT(mGridCanvasContext.mCellSize <= static_cast<size_t>(std::numeric_limits<LONG>::max()));

        const LONG weekLong = static_cast<LONG>(week);
        const LONG dayLong = static_cast<LONG>(day);
        // 마진 계산
        const LONG xMarginCount = (weekLong == 0) ? 0 : weekLong;
        const LONG yMarginCount = (dayLong == 0) ? 0 : dayLong;

        // 셀 크기 가져오기
        const LONG cellSize = static_cast<LONG>(mGridCanvasContext.mCellSize);

        // 기본 좌표 계산
        const LONG leftBase = (weekLong * cellSize) + (xMarginCount * mGridCanvasContext.mCellMargin);
        const LONG topBase = (dayLong * cellSize) + (yMarginCount * mGridCanvasContext.mCellMargin);
        const LONG rightBase = leftBase + cellSize;
        const LONG bottomBase = topBase + cellSize;

        // 중앙 좌표 계산
        const float centerX = static_cast<float>(leftBase + rightBase) * 0.5f;
        const float centerY = static_cast<float>(topBase + bottomBase) * 0.5f;

        // 스케일링된 반폭, 반높이 계산
        const float halfWidth = (static_cast<float>(cellSize) * scale) * 0.5f;
        const float halfHeight = (static_cast<float>(cellSize) * scale) * 0.5f;

        // 최종 좌표 계산
        const LONG left = static_cast<LONG>(centerX - halfWidth);
        const LONG right = static_cast<LONG>(centerX + halfWidth);
        const LONG top = static_cast<LONG>(centerY - halfHeight);
        const LONG bottom = static_cast<LONG>(centerY + halfHeight);

        return {left, top, right, bottom};
    }
} // CoTigraphy
