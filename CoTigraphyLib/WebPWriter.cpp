// \file WebPWriter.cpp
// \last_updated 2025-06-13
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "WebPWriter.hpp"

#include <webp/encode.h>
#include <webp/mux.h>

namespace CoTigraphy
{
    WebPWriter::WebPWriter() noexcept
    = default;

    WebPWriter::~WebPWriter()
    {
        WebPPictureFree(&mPicture);
        WebPAnimEncoderDelete(mEncoder);
    }

    void WebPWriter::Initialize(const int width, const int height)
    {
        WebPAnimEncoderOptions encoderOptions;
        WebPAnimEncoderOptionsInit(&encoderOptions);

        mEncoder = WebPAnimEncoderNew(width, height, &encoderOptions);

        WebPPictureInit(&mPicture);
        mPicture.width = width;
        mPicture.height = height;
        mPicture.use_argb = 1;

        WebPConfigInit(&mConfig);
        mConfig.quality = 90.0f;

        POSTCONDITION(mEncoder != nullptr);
    }

    bool WebPWriter::AddFrame(const uint8_t* const buffer)
    {
        // RGBA → WebPPicture 로 변환
        int ret = WebPPictureImportRGBA(&mPicture, buffer, mPicture.width * 4);
        ASSERT(ret != 0);

        // 프레임 추가
        ret = WebPAnimEncoderAdd(mEncoder, &mPicture, static_cast<int>(mEncodedFrame * mFrameDelayMs), &mConfig);
        ASSERT(ret != 0);

        mEncodedFrame++;

        return true;
    }

    void WebPWriter::SaveToFile(const std::wstring& fileName) const
    {
        // 마지막 frame 마킹
        int ret = WebPAnimEncoderAdd(mEncoder, nullptr, static_cast<int>(mEncodedFrame * mFrameDelayMs), nullptr);
        ASSERT(ret != 0);

        // WebP 애니메이션 출력
        WebPData webpData;
        WebPDataInit(&webpData);
        ret = WebPAnimEncoderAssemble(mEncoder, &webpData);
        ASSERT(ret != 0);

        // 파일로 저장
        FILE* file;
        _wfopen_s(&file, fileName.c_str(), L"wb");
        ASSERT(file != nullptr);

        std::ignore = fwrite(webpData.bytes, webpData.size, 1, file);
        std::ignore = fclose(file);

        WebPDataClear(&webpData);
    }
}
