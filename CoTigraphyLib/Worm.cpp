// \file Worm.cpp
// \last_updated 2025-06-13
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "Worm.hpp"

#include <map>
#include <set>
#include <queue>

#include "Grid.hpp"

namespace CoTigraphy
{
	Worm::Worm(Grid& grid) noexcept
		: mGrid(grid)
	{
		constexpr COLORREF wormBaseColor = RGB(0xFF, 0xA5, 0x00);
		mWormSegments = {
			WormSegment{{0, 0}, wormBaseColor, 1.0f},
			WormSegment{{1, 0}, wormBaseColor, 1.0f},
			WormSegment{{2, 0}, wormBaseColor, 1.0f},
			WormSegment{{3, 0}, wormBaseColor, 1.0f},
		};
	}

	Worm::~Worm()
		= default;

	bool Worm::Move(const uint64_t& currentLevel)
	{
		// 시작은 1부터 시작
		// 0만 있으면 아무것도 하지 않음
		// 1을 찾아야 함

		// 이미 계획된 경로가 있으면 경로로 이동해야 하고

		if (mPlannedPath.empty())
		{
			if (!FindPathToClosestTarget(currentLevel, mPlannedPath))
			{
				// 이동 가능한 Target 없음 → 아무것도 하지 않음
				return false;
			}
		}

		if (mPlannedPath.empty()) return false; // 안전성 확보

		const POINT next = mPlannedPath.front();
		mPlannedPath.erase(mPlannedPath.begin());

		// > Head 는 0번째, 나머지 뒤로 이동
		for (size_t i = mWormSegments.size() - 1; i >= 1; --i)
		{
			mWormSegments[i] = mWormSegments[i - 1];
		}

		mWormSegments[0].mPoint = next;
		mGrid.SetLevel(static_cast<size_t>(next.y), static_cast<size_t>(next.x), 0);
		mGrid.SetColor(static_cast<size_t>(next.y), static_cast<size_t>(next.x), RGB(255, 255, 255));

		return true;
	}

	bool Worm::FindPathToClosestTarget(const uint64_t& currentLevel, std::vector<POINT>& outPath) const
	{
		struct Node
		{
			POINT pt;
			POINT parent;
		};

		const POINT start = mWormSegments[0].mPoint;

		std::queue<Node> q;
		std::set<std::pair<int, int>> visited;
		std::map<std::pair<int, int>, POINT> parentMap;

		q.push({ start, {-1, -1} });
		visited.insert({ start.x, start.y });

		constexpr std::array<POINT, 4> directions = {
			POINT { -1, 0 },
			POINT { 1, 0 },
			POINT { 0, -1 },
			POINT { 0, 1 }
		};

		while (!q.empty())
		{
			Node node = q.front();
			q.pop();

			const int cx = node.pt.x;
			const int cy = node.pt.y;

			if (mGrid.IsInside(static_cast<size_t>(cy), static_cast<size_t>(cx))
				&& mGrid.GetLevel(static_cast<size_t>(cy), static_cast<size_t>(cx)) <= currentLevel
				&& mGrid.GetLevel(static_cast<size_t>(cy), static_cast<size_t>(cx)) != 0
				&& !(cx == start.x && cy == start.y)) // 자신은 제외
			{
				// 역추적하여 경로 복원
				POINT cur = node.pt;
				while (true)
				{
					outPath.push_back(cur);
					const auto it = parentMap.find({ cur.x, cur.y });
					if (it == parentMap.end()) break; // start에 도달했거나 map 누락
					cur = it->second;
				}
				std::reverse(outPath.begin(), outPath.end());
				return true;
			}

			for (const POINT& d : directions)
			{
				const POINT next = { cx + d.x, cy + d.y };
				const auto key = std::make_pair(next.x, next.y);

				if (!mGrid.IsInside(static_cast<size_t>(next.y), static_cast<size_t>(next.x))) continue;
				if (visited.count(key)) continue;

				visited.insert(key);
				parentMap[key] = node.pt;
				q.push({ next, node.pt });
			}
		}

		return false; // 대상 없음
	}

} // CoTigraphy
