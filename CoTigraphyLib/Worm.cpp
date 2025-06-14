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
        mGrid.SetColor(static_cast<size_t>(next.y), static_cast<size_t>(next.x), RGB(255, 255, 255));

        return true;
    }

    bool Worm::FindPathToClosestTarget(const uint64_t& currentLevel, std::vector<POINT>& outPath) const
    {
        constexpr int dx[4] = {-1, 1, 0, 0};
        constexpr int dy[4] = {0, 0, -1, 1};

        std::queue<POINT> queue;
        std::map<POINT, POINT, PointLess> parent;
        std::set<POINT, PointLess> visited;

        const POINT start = mWormSegments[0].mPoint;
        queue.push(start);
        visited.insert(start);

        while (!queue.empty())
        {
            const POINT current = queue.front();
            queue.pop();

            // Target 조건 → level == currentLevel
            if (mGrid.GetLevel(static_cast<size_t>(current.y), static_cast<size_t>(current.x)) == currentLevel)
            {
                // 경로 복원
                outPath.clear();
                for (POINT p = current; !IsEqual(p, start); p = parent[p])
                {
                    outPath.push_back(p);
                }
                std::reverse(outPath.begin(), outPath.end());
                return true;
            }

            // 4방향 이동
            for (int dir = 0; dir < 4; ++dir)
            {
                const int nx = current.x + dx[dir];
                const int ny = current.y + dy[dir];

                if (!mGrid.IsInside(static_cast<size_t>(ny), static_cast<size_t>(nx)))
                    continue;

                POINT next{nx, ny};

                // Worm body 통과 금지 → mWormSegments 검사
                bool isBody = false;
                for (const auto& segment : mWormSegments)
                {
                    if (segment.mPoint.x == next.x && segment.mPoint.y == next.y)
                    {
                        isBody = true;
                        break;
                    }
                }
                if (isBody) continue;

                const uint64_t level = mGrid.GetLevel(static_cast<size_t>(next.y), static_cast<size_t>(next.x));
                if (level > currentLevel)
                    continue;

                if (visited.count(next) == 0)
                {
                    visited.insert(next);
                    parent[next] = current;
                    queue.push(next);
                }
            }
        }

        return false; // Target 없음
    }
} // CoTigraphy
