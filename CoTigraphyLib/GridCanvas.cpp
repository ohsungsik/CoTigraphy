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

    void GridCanvas::Create(const CanvasContext& canvasContext)
    {
        PRECONDITION(canvasContext.mWidth != 0);
        PRECONDITION(canvasContext.mHeight != 0);
        PRECONDITION(canvasContext.mCellSize != 0);
        PRECONDITION(canvasContext.mCellMargin != 0);

        constexpr size_t bytesPerPixel = 4; // RGBA 4개 채널
        constexpr size_t alignment = bytesPerPixel * CHAR_BIT;

        mBufferSize = static_cast<size_t>(canvasContext.mWidth) * canvasContext.mHeight * bytesPerPixel;

        mBuffer = static_cast<uint8_t*>(_aligned_malloc(mBufferSize, alignment));

        POSTCONDITION(mBuffer);
    }
} // CoTigraphy
