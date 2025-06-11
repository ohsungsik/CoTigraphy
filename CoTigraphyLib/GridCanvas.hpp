// \file GridCanvas.hpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
	struct GridCanvasContext
	{
		unsigned int mWidth = 0;
		unsigned int mHeight = 0;
		unsigned int mCellSize = 0;
		unsigned int mCellMargin = 0;
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
		void DrawRect(const unsigned int y, const unsigned int x, const COLORREF color) const;
		void DrawRectScale(const unsigned int y, const unsigned int x, const float& scale, const COLORREF color) const;

		uint8_t* GetBuffer() const noexcept { return mBuffer; }
		size_t GetBufferSize() const noexcept { return mBufferSize; }
		void ClearTo(const COLORREF color) const;

	private:
		RECT GetRect(const unsigned int y, const unsigned int x, const float scale) const;

	private:
		const size_t mBytesPerPixel = 4; // RGBA 4개 채널
		const size_t m_alignment = mBytesPerPixel * CHAR_BIT;  // 32

		GridCanvasContext mGridCanvasContext;

		size_t mBufferSize = 0;
		uint8_t* mBuffer = nullptr;
	};
} // CoTigraphy
