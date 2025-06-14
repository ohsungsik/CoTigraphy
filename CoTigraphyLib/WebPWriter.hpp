// \file WebPWriter.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <webp/encode.h>
#include <webp/mux.h>

namespace CoTigraphy
{
    class WebPWriter final
    {
    public:
        explicit WebPWriter() noexcept;

        WebPWriter(const WebPWriter& other) = delete;
        WebPWriter(WebPWriter&& other) = delete;

        WebPWriter& operator=(const WebPWriter& rhs) = delete;
        WebPWriter& operator=(WebPWriter&& rhs) = delete;

        ~WebPWriter();

        void Initialize(_In_ const size_t& width, _In_ const size_t& height);
        bool AddFrame(_In_ const uint8_t* const buffer);
        void SaveToFile(_In_ const std::wstring& fileName) const;

    private:
        const size_t mFrameDelayMs = 80; // ms
        size_t mEncodedFrame = 0;
        WebPAnimEncoder* mEncoder = nullptr;
        WebPPicture mPicture{};
        WebPConfig mConfig{};
    };
}
