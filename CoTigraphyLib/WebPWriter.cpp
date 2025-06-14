// \file WebPWriter.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "WebPWriter.hpp"

#include <filesystem>
#include <set>
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

    Error WebPWriter::SaveToFile(_In_ const std::wstring& fileName) const
    {
        PRECONDITION(fileName.empty() == false);

        const std::filesystem::path path(fileName);
        // 확장자가 유효한지 확인, 대소문자 무시
        if (_wcsicmp(path.extension().c_str(), L".webp") != 0)
        {
            return MAKE_ERROR(eErrorCode::InvalidFileExtension);
        }

        // 파일 이름 (확장자 제외)이 비었는지 확인
        if (path.stem().empty())
        {
            return MAKE_ERROR(eErrorCode::MissingFileName);
        }

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
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return MAKE_ERROR_FROM_LAST_WIN32_ERROR();
        }

        // 데이터가 유효한지 확인
        ASSERT(webpData.bytes != nullptr);
        ASSERT(webpData.size > 0);

        // 파일에 데이터 쓰기
        size_t totalWritten = 0;
        const BYTE* buffer = webpData.bytes;
        const size_t totalSize = webpData.size;

        // 네트워크 드라이브 디스크에서 간혹 한번에 안써지는 경우가 발생..
        // 따라서 모든 데이터를 다 쓸떄 까지 반복하여 쓰기 시도
        while (totalWritten < totalSize)
        {
            DWORD bytesWritten = 0;

            // 한 번에 WriteFile이 처리 가능한 최대 크기 계산
            const DWORD chunkSize = static_cast<DWORD>(
                std::min<size_t>(totalSize - totalWritten, static_cast<size_t>(MAXDWORD))
            );

            const BOOL writeResult = WriteFile(
                hFile,
                buffer + totalWritten,
                chunkSize,
                &bytesWritten,
                nullptr
            );

            if (!writeResult)
            {
                // 실패 처리
                CloseHandle(hFile);
                WebPDataClear(&webpData);
                return MAKE_ERROR(eErrorCode::FileIOFailure);
            }

            totalWritten += bytesWritten;
        }

        // 파일 핸들 정리
        const BOOL closeResult = CloseHandle(hFile);
        ASSERT(closeResult != FALSE);

        // WebP 데이터 정리
        WebPDataClear(&webpData);

        return MAKE_ERROR(eErrorCode::Succeeded);
    }
}
