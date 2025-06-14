// \file Grid.cpp
// \last_updated 2025-06-13
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

	bool Grid::IsInside(const size_t& y, const size_t& x) const
	{
		return x < mGridData.mColoumCount && y < mGridData.mRowCount;
	}

	uint64_t Grid::GetLevel(const size_t& y, const size_t& x) const
	{
		return mGridData.mCells[x][y].mCount;
	}

	void Grid::SetLevel(const size_t& y, const size_t& x, const uint64_t& level)
	{
		mGridData.mCells[x][y].mCount = level;
	}

	void Grid::SetColor(const size_t& y, const size_t& x, const COLORREF& color)
	{
		mGridData.mCells[x][y].mColor = color;
	}
} // CoTigraphy
