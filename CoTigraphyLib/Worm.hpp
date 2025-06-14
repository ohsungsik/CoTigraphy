// \file Worm.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <array>
#include <vector>

namespace CoTigraphy
{
    class Grid;

    struct PointLess
    {
        bool operator()(const POINT& lhs, const POINT& rhs) const noexcept
        {
            return (lhs.y < rhs.y) || (lhs.y == rhs.y && lhs.x < rhs.x);
        }
    };

    struct WormSegment
    {
        POINT mPoint;
        COLORREF mColor;
        float mScale;
    };

    class Worm final
    {
    public:
        explicit Worm(Grid& grid) noexcept;
        Worm(const Worm& other) = delete;
        Worm(Worm&& other) = delete;

        Worm& operator=(const Worm& rhs) = delete;
        Worm& operator=(Worm&& rhs) = delete;

        ~Worm();

        WormSegment GetHead() const noexcept { return mWormSegments[0]; }
        std::array<WormSegment, 4> GetWorm() const noexcept { return mWormSegments; };
        bool Move(const uint64_t& currentLevel);

    private:
        bool FindPathToClosestTarget(const uint64_t& currentLevel, std::vector<POINT>& outPath) const;

        bool IsEqual(const POINT& lhs, const POINT& rhs) const noexcept;

    private:
        Grid& mGrid;

        std::array<WormSegment, 4> mWormSegments;
        std::vector<POINT> mPlannedPath;
    };
} // CoTigraphy
