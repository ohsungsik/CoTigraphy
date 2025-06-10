// \file GridCanvas.hpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
    struct CanvasContext
    {
        unsigned int mWidth;
        unsigned int mHeight;
        unsigned int mCellSize;
        unsigned int mCellMargin;
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

        void Create(const CanvasContext& canvasContext);

    private:
        CanvasContext mCanvasContext;

        size_t mBufferSize = 0;
        uint8_t* mBuffer = nullptr;
    };
} // CoTigraphy
