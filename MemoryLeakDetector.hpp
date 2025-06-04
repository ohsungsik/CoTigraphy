// \file MemoryLeakDetector.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
    class MemoryLeakDetector final
    {
    public:
        explicit MemoryLeakDetector() noexcept;

        MemoryLeakDetector(const MemoryLeakDetector& other) = delete;
        MemoryLeakDetector(MemoryLeakDetector&& other) = delete;

        MemoryLeakDetector& operator=(const MemoryLeakDetector& rhs) = delete;
        MemoryLeakDetector& operator=(MemoryLeakDetector&& rhs) = delete;

        ~MemoryLeakDetector();

        static void Initialize() noexcept;

    private:
        static void OnProcessExit() noexcept;
    };
}
