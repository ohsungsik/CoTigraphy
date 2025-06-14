// \file Grid.hpp
// \last_updated 2025-06-13
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <vector>

namespace CoTigraphy
{
	struct GridCell
	{
		size_t mXIndex = 0;
		size_t mYIndex = 0;
		uint64_t mCount = 0;
		COLORREF mColor = 0;
	};

	struct GridData
	{
		std::vector<std::vector<GridCell>> mCells;
		size_t mRowCount = 0; // 줄 수 -> 요일 갯수 -> 7
		size_t mColoumCount = 0; // 커럼 수 -> 주 갯수
		size_t mMaxCount = 0;	// 최대 커밋 횟수
	};

	class Grid final
	{
	public:
		explicit Grid(GridData gridData) noexcept;
		Grid(const Grid& other) = delete;
		Grid(Grid&& other) = delete;

		Grid& operator=(const Grid& rhs) = delete;
		Grid& operator=(Grid&& rhs) = delete;

		~Grid();

		const GridData& GetGridData() const noexcept { return mGridData; }
		bool IsInside(const size_t& y, const size_t& x) const;
		uint64_t GetLevel(const size_t& y, const size_t& x) const;

		void SetLevel(const size_t& y, const size_t& x, const uint64_t& level);
		void SetColor(const size_t& y, const size_t& x, const COLORREF& color);

	private:
		GridData mGridData;
		uint64_t mMoveableCount = 1;
	};
} // CoTigraphy
