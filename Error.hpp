// \file Error.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include "ErrorCode.hpp"

namespace CoTigraphy
{
    struct SourceLocation
    {
        std::wstring mFilePath;
        int mLineNumber;
    };

    class Error final
    {
    public:
        static Error FromErrorCode(const eErrorCode& errorCode,
                                   const wchar_t* const filePath = __FILEW__,
                                   const int lineNumber = __LINE__);
        static Error FromHResult(const HRESULT& hResult,
                                 const wchar_t* const filePath = __FILEW__,
                                 const int lineNumber = __LINE__);

    public:
        Error(const Error& other) noexcept;
        Error(Error&& other) noexcept;

        Error& operator=(const Error& rhs) noexcept;
        Error& operator=(Error&& rhs) noexcept;

        ~Error();

    public:
        [[nodiscard]] bool IsSucceeded() const noexcept { return SUCCEEDED(mErrorCode); }
        [[nodiscard]] bool IsFailed() const noexcept { return FAILED(mErrorCode); }

        // HRESULT_CUSTOMER_BIT에 해당하는 비트를 추출하여, 0이면 Customer flag가 없는 것으로 간주.
        [[nodiscard]] bool IsWin32Error() const noexcept
        {
            return (((static_cast<HRESULT>(mErrorCode) >> HRESULT_CUSTOMER_BIT) & 1) == 0);
        }

        [[nodiscard]] eErrorCode GetErrorCode() const noexcept { return mErrorCode; }
        [[nodiscard]] std::wstring GetErrorMessage() const;
        [[nodiscard]] std::wstring GetSourceFilePath() const noexcept { return mSourceLocation.mFilePath; }
        [[nodiscard]] int GetSourceLineNumber() const noexcept { return mSourceLocation.mLineNumber; }

    private:
        explicit Error() noexcept;

    private:
        eErrorCode mErrorCode;
        SourceLocation mSourceLocation;
    };

#define MAKE_ERROR_FROM_WIN32(Win32ErrorCode) CoTigraphy::Error(HRESULT_FROM_WIN32(Win32ErrorCode))
#define MAKE_ERROR_FROM_LAST_WIN32_ERROR() MAKE_ERROR_FROM_WIN32(::GetLastError())
}
