// \file HandleLeakDetector.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
    class HandleLeakDetector final
    {
    public:
        explicit HandleLeakDetector() noexcept;

        HandleLeakDetector(const HandleLeakDetector& other) = delete;
        HandleLeakDetector(HandleLeakDetector&& other) = delete;

        HandleLeakDetector& operator=(const HandleLeakDetector& rhs) = delete;
        HandleLeakDetector& operator=(HandleLeakDetector&& rhs) = delete;

        ~HandleLeakDetector();

        static void Initialize() noexcept;

    private:
        static void OnProcessExit();
    };
}
