// \file Grid.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <vector>

namespace CoTigraphy
{
    struct GridCell
    {
        size_t mWeek = 0;
        size_t mDay = 0;
        uint64_t mCount = 0;
        COLORREF mColor = 0;
    };

    struct GridData
    {
        std::vector<std::vector<GridCell>> mCells;
        size_t mDayCount = 0; // 줄 수 -> 요일 갯수 -> 7과 같거나 작음
        size_t mWeekCount = 0; // 커럼 수 -> 주 갯수
        uint64_t mMaxCount = 0; // 최대 커밋 횟수
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

        /**
         * \brief 현재 그리드의 행(row) 개수를 반환
         * \return 수직 방향 셀(row)의 개수 (최대 7, 요일 개수에 해당)
         * \details
         *  - GitHub Contribution Calendar는 최대 7개의 행을 가짐 (일~토)
         *  - 실제 row 수(day)는 캘린더 기준일에 따라 7 이하일 수 있음
         *  - mGridData 내부에 저장된 rowCount를 그대로 반환
         */
        [[nodiscard]] size_t GetDayCount() const noexcept;

        /**
         * \brief 현재 그리드의 열(column) 개수를 반환
         * \return 수평 방향 셀(column)의 개수 (보통 52 또는 53, 주 단위)
         * \details
         *  - mGridData 내부에 저장된 coloumCount를 그대로 반환
         *  - 이 값은 GitHub Contribution Calendar 상의 주(week) 개수를 나타냄
         *  - API 호출 시 받은 데이터의 실제 주 개수에 따라 달라질 수 있음
         */
        [[nodiscard]] size_t GetWeekCount() const noexcept;

        /**
         * \brief 지정한 좌표에 위치한 GridCell을 반환
         * \param week 열 인덱스 (0부터 시작, 최대 GetWeekCount() - 1)
         * \param day 행 인덱스 (0부터 시작, 최대 GetDayCount() - 1)
         * \return 주어진 좌표의 GridCell에 대한 const 참조
         * \pre IsInside(y, x) == true (그리드 내부 좌표여야 함)
         * \post 반환되는 참조는 Grid 내부 데이터의 불변 참조
         * \details
         *  - 반환된 참조를 통해 셀의 좌표, 색상, 카운트 등을 조회할 수 있음
         *  - 데이터 변경은 허용되지 않음 (const 참조 반환)
         */
        [[nodiscard]] const GridCell& GetCell(_In_ const size_t& week, _In_ const size_t& day) const;

        /**
         * @brief 지정한 좌표가 Grid 영역 안에 포함되는지 검사
         * \param week 열 인덱스 (0부터 시작, 최대 GetWeekCount() - 1)
         * \param day 행 인덱스 (0부터 시작, 최대 GetDayCount() - 1)
         * @return Grid 내부라면 true, 아니면 false
         */
        [[nodiscard]] bool IsInside(_In_ const size_t& week, _In_ const size_t& day) const noexcept;

        /**
        * @brief 지정한 좌표의 Contributon 횟수를 반환
         * \param week 열 인덱스 (0부터 시작, 최대 GetWeekCount() - 1)
         * \param day 행 인덱스 (0부터 시작, 최대 GetDayCount() - 1)
        * @return 해당 셀의 Contributon 횟수
        * @pre IsInside(y, x) == true
        * @contract strong 유효 범위 내 접근이 보장되어야 함
        */
        [[nodiscard]] uint64_t GetContributionCount(_In_ const size_t& week, _In_ const size_t& day) const;

        /**
         * @brief 지정한 좌표의 Contributon 횟수를 설정
         * \param week 열 인덱스 (0부터 시작, 최대 GetWeekCount() - 1)
         * \param day 행 인덱스 (0부터 시작, 최대 GetDayCount() - 1)
         * @param level 설정할 Contributon 횟수
         * @pre IsInside(y, x) == true
         */
        void SetContributionCount(_In_ const size_t& week, _In_ const size_t& day, _In_ const uint64_t& level);

        /**
         * @brief 지정한 좌표의 색상을 설정
         * \param week 열 인덱스 (0부터 시작, 최대 GetWeekCount() - 1)
         * \param day 행 인덱스 (0부터 시작, 최대 GetDayCount() - 1)
         * @param color COLORREF 타입의 색상
         * @pre IsInside(y, x) == true
         */
        void SetColor(_In_ const size_t& week, _In_ const size_t& day, _In_ const COLORREF& color);

    private:
        GridData mGridData;
    };
} // CoTigraphy
