// \file Worm.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "Worm.hpp"

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

    bool Worm::Move(_In_ const uint64_t& contributionCount)
    {
        // 시작은 1부터 시작
        // 0만 있으면 아무것도 하지 않음
        // 1을 찾아야 함

        // 이미 계획된 경로가 있으면 경로로 이동해야 하고

        if (mPlannedPath.empty())
        {
            if (!FindPathToClosestTarget(contributionCount, mPlannedPath))
            {
                // 이동 가능한 Target 없음 → 아무것도 하지 않음
                return false;
            }
        }

        if (mPlannedPath.empty()) return false; // 안전성 확보

        const POINT next = mPlannedPath.front();
        mPlannedPath.erase(mPlannedPath.begin());

        // > Head 는 0번째, 나머지 뒤로 이동
        std::copy_backward(
            mWormSegments.begin(), // 시작 위치
            mWormSegments.end() - 1, // 끝 위치 (마지막 요소 제외)
            mWormSegments.end() // 대상 끝 위치
        );

        mWormSegments[0].mPoint = next;
        mGrid.SetContributionCount(static_cast<size_t>(next.x), static_cast<size_t>(next.y), 0);
        mGrid.SetColor(static_cast<size_t>(next.x), static_cast<size_t>(next.y), RGB(255, 255, 255));

        return true;
    }

    bool Worm::FindPathToClosestTarget(_In_ const uint64_t& currentContributionCount,
                                       _Out_opt_ std::vector<POINT>& outPath) const
    {
        outPath.clear();

        // BFS 시작 위치는 지렁이 머리 (맨 앞 세그먼트)
        const POINT start = mWormSegments[0].mPoint;

        std::queue<POINT> bfsQueue; // BFS 탐색 큐
        std::set<std::pair<int, int>> visited; // 방문한 위치를 저장 (중복 방문 방지)
        std::map<std::pair<int, int>, POINT> parentMap; // 역추적을 위한 부모 정보 저장 (자식 위치 -> 부모 위치)

        // 시작 지점 등록
        bfsQueue.push(start);
        visited.insert({start.x, start.y});


        // BFS 탐색 시작
        while (!bfsQueue.empty())
        {
            const POINT current = bfsQueue.front();
            bfsQueue.pop();

            // 현재 위치가 목표 조건에 부합하면 경로 복원
            if (IsTargetCell(current, currentContributionCount, start))
            {
                // 역추적 경로 생성
                BuildPath(current, parentMap, outPath);
                return true;
            }

            // 다음 탐색 가능한 이웃 노드를 큐에 추가
            EnqueueNeighbors(current, bfsQueue, visited, parentMap);
        }

        // 더 이상 유효한 목표가 없음
        return false;
    }

    std::pair<int, int> Worm::MakeKey(_In_ const POINT& pt) const noexcept
    {
        return {pt.x, pt.y};
    }

    bool Worm::IsTargetCell(_In_ const POINT& pt, _In_ const uint64_t& currentContributionCount,
                            _In_ const POINT& start) const
    {
        if (!mGrid.IsInside(static_cast<size_t>(pt.x), static_cast<size_t>(pt.y)))
            return false;

        const uint64_t count = mGrid.GetContributionCount(static_cast<size_t>(pt.x), static_cast<size_t>(pt.y));
        return (count != 0 && count <= currentContributionCount && !(pt.x == start.x && pt.y == start.y));
    }

    void Worm::BuildPath(_In_ const POINT& goal, _In_ const std::map<std::pair<int, int>, POINT>& parentMap,
                         _Out_ std::vector<POINT>& outPath) const
    {
        POINT cur = goal;
        while (true)
        {
            outPath.push_back(cur);
            const auto& it = parentMap.find(MakeKey(cur));
            if (it == parentMap.end()) break;
            cur = it->second;
        }
        std::reverse(outPath.begin(), outPath.end());
    }

    void Worm::EnqueueNeighbors(_In_ const POINT& current, _In_ std::queue<POINT>& bfsQueue,
                                _Inout_ std::set<std::pair<int, int>>& visited,
                                _Inout_ std::map<std::pair<int, int>, POINT>& parentMap) const
    {
        // 상하좌우 방향 정의
        constexpr std::array<POINT, 4> directions = {
            POINT{-1, 0}, // 위
            POINT{1, 0}, // 아래
            POINT{0, -1}, // 왼쪽
            POINT{0, 1} // 오른쪽
        };

        for (const POINT& d : directions)
        {
            const POINT next = {current.x + d.x, current.y + d.y};
            const std::pair<int, int> key = MakeKey(next);

            if (!mGrid.IsInside(static_cast<size_t>(next.x), static_cast<size_t>(next.y))) continue;
            if (visited.count(key)) continue;

            visited.insert(key);
            parentMap[key] = current;
            bfsQueue.push(next);
        }
    }
} // CoTigraphy
