// \file WebPWriter.cpp
// \last_updated 2025-06-14
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

#pragma warning(disable: 4267)  // conversion from 'size_t' to 'int', possible loss of data)
    void WebPWriter::Initialize(_In_ const size_t& width, _In_ const size_t& height)
    {
        ASSERT(width <= static_cast<size_t>(std::numeric_limits<int>::max()));
        ASSERT(height <= static_cast<size_t>(std::numeric_limits<int>::max()));

        WebPAnimEncoderOptions encoderOptions;
        WebPAnimEncoderOptionsInit(&encoderOptions);

        mEncoder = WebPAnimEncoderNew(static_cast<int>(width), static_cast<int>(height), &encoderOptions);

        WebPPictureInit(&mPicture);
        mPicture.width = static_cast<int>(width);
        mPicture.height = static_cast<int>(height);
        mPicture.use_argb = 1;

        WebPConfigInit(&mConfig);
        mConfig.quality = 90.0f;

        POSTCONDITION(mEncoder != nullptr);
    }

    bool WebPWriter::AddFrame(_In_ const uint8_t* const buffer)
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

    void WebPWriter::SaveToFile(_In_ const std::wstring& fileName) const
    {
        PRECONDITION(fileName.empty() == false);

        // 마지막 frame 마킹
        int ret = WebPAnimEncoderAdd(mEncoder, nullptr, static_cast<int>(mEncodedFrame * mFrameDelayMs), nullptr);
        ASSERT(ret != 0);

        // WebP 애니메이션 출력
        WebPData webpData;
        WebPDataInit(&webpData);
        ret = WebPAnimEncoderAssemble(mEncoder, &webpData);
        ASSERT(ret != 0);

        // 파일로 저장
        const HANDLE hFile = CreateFileW(
            fileName.c_str(), // 파일 이름
            GENERIC_WRITE, // 쓰기 권한
            0, // 공유 모드 없음
            nullptr, // 보안 속성
            CREATE_ALWAYS, // 항상 새로 생성
            FILE_ATTRIBUTE_NORMAL, // 일반 파일 속성
            nullptr // 템플릿 파일 없음
        );

        // 파일 생성 성공 여부 확인
        ASSERT(hFile != INVALID_HANDLE_VALUE);

        // 데이터가 유효한지 확인
        ASSERT(webpData.bytes != nullptr);
        ASSERT(webpData.size > 0);

        // 파일에 데이터 쓰기
        DWORD bytesWritten = 0;
        const BOOL writeResult = WriteFile(
            hFile, // 파일 핸들
            webpData.bytes, // 쓸 데이터
            static_cast<DWORD>(webpData.size), // 데이터 크기
            &bytesWritten, // 실제 쓰여진 바이트 수
            nullptr // 오버랩 구조체 없음
        );

        // 쓰기 성공 및 모든 데이터가 쓰여졌는지 확인
        ASSERT(writeResult != FALSE);
        ASSERT(bytesWritten == webpData.size);

        // 파일 핸들 정리
        const BOOL closeResult = CloseHandle(hFile);
        ASSERT(closeResult != FALSE);

        // WebP 데이터 정리
        WebPDataClear(&webpData);
    }
}
