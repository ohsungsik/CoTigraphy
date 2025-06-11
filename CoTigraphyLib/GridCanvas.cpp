// \file GridCanvas.cpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "GridCanvas.hpp"

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

		mBufferSize = static_cast<size_t>(mGridCanvasContext.mWidth) * mGridCanvasContext.mHeight * mBytesPerPixel;

		mBuffer = static_cast<uint8_t*>(_aligned_malloc(mBufferSize, m_alignment));

		POSTCONDITION(mBuffer);
	}

	void GridCanvas::DrawRect(const unsigned y, const unsigned x, const COLORREF color) const
	{
		DrawRectScale(y, x, 1.0f, color);
	}

	void GridCanvas::DrawRectScale(const unsigned int y, const unsigned int x, const float& scale, const COLORREF color) const
	{
		PRECONDITION(mGridCanvasContext.mCellSize != 0);
		PRECONDITION(y < mGridCanvasContext.mHeight);
		PRECONDITION(x < mGridCanvasContext.mWidth);

		const RECT rect = GetRect(y, x, scale);

		// Extract RGB from COLORREF (COLORREF is 0x00bbggrr)
		const uint8_t r = GetRValue(color);
		const uint8_t g = GetGValue(color);
		const uint8_t b = GetBValue(color);
		const uint8_t a = 255;

		// Clamp rect boundaries to image size (safe drawing)
		const LONG left = std::max<LONG>(0, rect.left);
		const LONG right = std::min<LONG>(mGridCanvasContext.mWidth, rect.right);
		const LONG top = std::max<LONG>(0, rect.top);
		const LONG bottom = std::min<LONG>(mGridCanvasContext.mHeight, rect.bottom);

		// Draw rectangle area
		for (LONG yPos = top; yPos < bottom; ++yPos)
		{
			for (LONG xPos = left; xPos < right; ++xPos)
			{
				const size_t index = (static_cast<size_t>(yPos) * mGridCanvasContext.mWidth + static_cast<size_t>(xPos)) * mBytesPerPixel;
				mBuffer[index + 0] = r;
				mBuffer[index + 1] = g;
				mBuffer[index + 2] = b;
				mBuffer[index + 3] = a;
			}
		}
	}

	void GridCanvas::ClearTo(const COLORREF color) const
	{
		PRECONDITION(mBuffer != nullptr);
		PRECONDITION(mBufferSize != 0);

		for (size_t i = 0; i < mBufferSize; i += mBytesPerPixel)
		{
			uint8_t* buffer = mBuffer + i;
			*(buffer + 0) = GetRValue(color);
			*(buffer + 1) = GetGValue(color);
			*(buffer + 2) = GetBValue(color);
			*(buffer + 3) = 0xff;
		}
	}

	RECT GridCanvas::GetRect(const unsigned int y, const unsigned int x, const float scale) const
	{
		PRECONDITION(y < mGridCanvasContext.mHeight);
		PRECONDITION(x < mGridCanvasContext.mWidth);
		PRECONDITION(0.0f < scale);
		PRECONDITION(scale <= 1.0f);

		const unsigned int yMarginCount = (y == 0) ? 0 : (y);
		const unsigned int xMarginCount = (x == 0) ? 0 : (x);

		const LONG cellSize = static_cast<LONG>(mGridCanvasContext.mCellSize);
		const LONG leftBase = (x * cellSize) + (xMarginCount * mGridCanvasContext.mCellMargin);
		const LONG topBase = (y * cellSize) + (yMarginCount * mGridCanvasContext.mCellMargin);
		const LONG rightBase = leftBase + cellSize;
		const LONG bottomBase = topBase + cellSize;

		// 중앙 좌표
		const float centerX = (leftBase + rightBase) * 0.5f;
		const float centerY = (topBase + bottomBase) * 0.5f;

		// scaled 반폭, 반높이
		const float halfWidth = (cellSize * scale) * 0.5f;
		const float halfHeight = (cellSize * scale) * 0.5f;

		const LONG left = static_cast<LONG>(centerX - halfWidth);
		const LONG right = static_cast<LONG>(centerX + halfWidth);
		const LONG top = static_cast<LONG>(centerY - halfHeight);
		const LONG bottom = static_cast<LONG>(centerY + halfHeight);

		return { left, top, right, bottom };
	}
} // CoTigraphy
