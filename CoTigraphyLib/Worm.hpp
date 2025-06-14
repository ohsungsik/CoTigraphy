// \file Worm.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <array>
#include <map>
#include <queue>
#include <set>
#include <vector>

namespace CoTigraphy
{
    class Grid;

    /**
     * @brief 지렁이의 한 구간(segment)을 나타내는 구조체
     */
    struct WormSegment
    {
        POINT mPoint; // 위치 (index)
        COLORREF mColor; // 색상
        float mScale; // 크기 비율 (0.0f ~ 1.0f)
    };

    /**
     * @brief 지렁이 애니메이션 생명체 객체
     * @details
     * - 내부적으로 길이 4의 지렁이 구간을 보유하며, Grid 객체를 참조하여 이동
     * - 각 이동마다 가장 가까운 목표 셀을 찾아 경로를 생성하고, 해당 경로로 한 칸씩 이동
     * - 지렁이는 아직 방문하지 않은 셀(currentLevel 이하)에 도달하기 위해 BFS 기반 경로 탐색을 수행
     */
    class Worm final
    {
    public:
        /**
         * @brief 지렁이 객체를 생성하고 초기 위치를 설정
         * @param grid 이동 대상이 되는 Grid 객체 (non-owning reference)
         * @note 지렁이는 Grid 내에 위치하며, 외부에서 grid를 먼저 생성해두어야 함
         */
        explicit Worm(Grid& grid) noexcept;
        Worm(const Worm& other) = delete;
        Worm(Worm&& other) = delete;

        Worm& operator=(const Worm& rhs) = delete;
        Worm& operator=(Worm&& rhs) = delete;

        ~Worm();

        /**
         * @brief 지렁이 전체 구간 배열을 반환
         * @return 길이 4의 WormSegment 배열
         */
        [[nodiscard]] std::array<WormSegment, 4> GetWorm() const noexcept { return mWormSegments; };

        /**
         * @brief 현재 레벨의 목표 셀을 찾아 한 칸 이동
         * @param contributionCount 현재 탐색 중인 Contributon 횟수 (즉, 1 → 2 → ... → MaxCount 순)
         * @return 이동 성공 여부 (false면 더 이상 이동할 수 없음)
         * @details
         * - 내부적으로 경로를 계산하여, 한 스텝씩 이동
         * - 목표 셀에 도달하면 다음 레벨로 넘어가기 위해 false 반환
         */
        [[nodiscard]] bool Move(_In_ const uint64_t& contributionCount);

    private:
        /**
         * @brief 현재 레벨에서 도달 가능한 가장 가까운 목표 셀로의 경로를 찾음 (BFS 기반)
         * @param currentContributionCount 검색 대상인 Contributon 레벨
         * @param outPath 탐색 결과로 얻은 경로 (시작점 포함, 도착점까지)
         * @return 목표 셀을 찾았고, 경로 계산에 성공했으면 true
         */
        [[nodiscard]] bool FindPathToClosestTarget(_In_ const uint64_t& currentContributionCount,
                                                   _Out_opt_ std::vector<POINT>& outPath) const;

        /**
         * @brief 좌표를 방문 기록용 key로 변환합니다.
         *
         * @param pt 좌표 (POINT 구조체)
         * @return (x, y) 쌍을 pair로 반환하여 std::set/map 키로 사용 가능
         */
        [[nodiscard]] std::pair<int, int> MakeKey(_In_ const POINT& pt) const noexcept;


        /**
         * @brief 주어진 좌표가 유효한 목표(먹이)인지 판단합니다.
         *
         * @param pt 검사할 좌표
         * @param currentContributionCount 현재 지렁이가 먹을 수 있는 최대 Contribution Count 값
         * @param start 지렁이의 시작 위치 (자기 자신 제외 목적)
         * @return true: 먹이로 유효
         *         false: 유효하지 않음
         *
         * @details 다음 조건을 모두 만족해야 유효한 먹이로 간주:
         * - 그리드 범위 내 좌표일 것
         * - 해당 셀의 contribution count가 0보다 크고 현재 레벨 이하일 것
         * - 자기 자신(start 위치)이 아닐 것
         */
        [[nodiscard]] bool IsTargetCell(_In_ const POINT& pt, _In_ const uint64_t& currentContributionCount,
                                        _In_ const POINT& start) const;

        /**
         * @brief BFS 탐색을 통해 찾은 목표 지점에서 시작점까지의 경로를 역추적하여 outPath에 저장합니다.
         *
         * @param goal 목표 지점 좌표
         * @param parentMap 자식 → 부모 좌표 정보를 담은 맵
         * @param outPath 경로 저장 대상 벡터 (결과로서 반환됨)
         *
         * @details goal부터 시작점까지 parentMap을 따라 거슬러 올라가며 경로를 구성하고,
         * 이후 reverse()를 통해 경로를 올바른 순서(시작 -> 목표)로 재배열합니다.
         */
        void BuildPath(_In_ const POINT& goal, _In_ const std::map<std::pair<int, int>, POINT>& parentMap,
                       _Out_ std::vector<POINT>& outPath) const;

        /**
         * @brief 주어진 현재 좌표에서 상하좌우 방향으로 이동 가능한 셀을 큐에 추가합니다.
         *
         * @param current 현재 위치 좌표
         * @param bfsQueue BFS 큐
         * @param visited 방문한 좌표 집합
         * @param parentMap 자식 -> 부모 관계 저장용 맵
         *
         * @details
         * - 현재 좌표를 기준으로 4방향 탐색
         * - 그리드 내부이고 아직 방문하지 않은 좌표만 BFS 큐에 추가
         * - 동시에 parentMap에 현재 좌표를 부모로 기록하여 경로 복원에 활용
         */
        void EnqueueNeighbors(_In_ const POINT& current, _In_ std::queue<POINT>& bfsQueue,
                              _Inout_ std::set<std::pair<int, int>>& visited,
                              _Inout_ std::map<std::pair<int, int>, POINT>& parentMap) const;

    private:
        Grid& mGrid;

        std::array<WormSegment, 4> mWormSegments;
        std::vector<POINT> mPlannedPath;
    };
} // CoTigraphy
