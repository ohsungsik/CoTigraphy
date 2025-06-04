// \file Error.cpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "Error.hpp"

namespace CoTigraphy
{
    Error Error::FromErrorCode(const eErrorCode& errorCode, const wchar_t* filePath, const int lineNumber)
    {
        Error error;
        error.mErrorCode = errorCode;
        error.mSourceLocation = {filePath, lineNumber};
        return error;
    }

    Error Error::FromHResult(const HRESULT& hResult, const wchar_t* filePath, const int lineNumber)
    {
        Error error;
        error.mErrorCode = static_cast<eErrorCode>(hResult);
        error.mSourceLocation = {filePath, lineNumber};
        return error;
    }

    Error::Error(const Error& other) noexcept
    = default;

    Error::Error(Error&& other) noexcept
        : mErrorCode(other.mErrorCode),
          mSourceLocation(std::move(other.mSourceLocation))
    {
    }

    Error& Error::operator=(const Error& rhs) noexcept
    {
        if (this == &rhs)
        {
            return *this;
        }

        mErrorCode = rhs.mErrorCode;
        mSourceLocation = rhs.mSourceLocation;
        return *this;
    }

    Error& Error::operator=(Error&& rhs) noexcept
    {
        if (this == &rhs)
        {
            return *this;
        }

        mErrorCode = rhs.mErrorCode;
        mSourceLocation = rhs.mSourceLocation;
        return *this;
    }

    Error::~Error()
        = default;


    std::wstring Error::GetErrorMessage() const
    {
        std::wstring errorMessage;
        // FormatMessage()를 사용해 시스템 에러 메시지를 가져온다.
        LPVOID lpMsgBuf = nullptr; // 시스템 에러 메시지를 저장할 버퍼 포인터
        const DWORD result = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            static_cast<DWORD>(GetErrorCode()),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPTSTR>(&lpMsgBuf),
            0,
            nullptr);
        if (result != 0)
        {
            // 가져온 시스템 메시지를 원래 메시지에 추가한다.
            errorMessage = std::wstring(static_cast<LPCWSTR>(lpMsgBuf));

            // 할당된 버퍼를 해제한다.
            LocalFree(lpMsgBuf);
        }

        return errorMessage;
    }


    Error::Error() noexcept
    = default;
}
