// \file Grid.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "Grid.hpp"

namespace CoTigraphy
{
    Grid::Grid(GridData gridData) noexcept
        : mGridData(std::move(gridData))
    {
    }

    Grid::~Grid()
    = default;

    size_t Grid::GetDayCount() const noexcept
    {
        return mGridData.mDayCount;
    }

    size_t Grid::GetWeekCount() const noexcept
    {
        return mGridData.mWeekCount;
    }

    const GridCell& Grid::GetCell(_In_ const size_t& week, _In_ const size_t& day) const
    {
        PRECONDITION(IsInside(week, day));

        return mGridData.mCells[week][day];
    }

    bool Grid::IsInside(_In_ const size_t& week, _In_ const size_t& day) const noexcept
    {
        return week < mGridData.mWeekCount && day < mGridData.mDayCount;
    }

    uint64_t Grid::GetContributionCount(_In_ const size_t& week, _In_ const size_t& day) const
    {
        PRECONDITION(IsInside(week, day));

        return mGridData.mCells[week][day].mCount;
    }

    void Grid::SetContributionCount(_In_ const size_t& week, _In_ const size_t& day, _In_ const uint64_t& level)
    {
        PRECONDITION(IsInside(week, day));

        mGridData.mCells[week][day].mCount = level;
    }

    void Grid::SetColor(_In_ const size_t& week, _In_ const size_t& day, _In_ const COLORREF& color)
    {
        PRECONDITION(IsInside(week, day));

        mGridData.mCells[week][day].mColor = color;
    }
} // CoTigraphy
