// \file Error.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include "ErrorCode.hpp"

namespace CoTigraphy
{
    struct SourceLocation
    {
        std::wstring mFilePath{};
        int mLineNumber = -1;
    };

    class Error final
    {
    public:
        static Error FromErrorCode(const eErrorCode& errorCode,
                                   const wchar_t* const filePath,
                                   const int lineNumber);
        static Error FromHResult(const HRESULT& hResult,
                                 const wchar_t* const filePath,
                                 const int lineNumber);

    public:
        Error(const Error& other) noexcept;
        Error(Error&& other) noexcept;

        Error& operator=(const Error& rhs) noexcept;
        Error& operator=(Error&& rhs) noexcept;

        ~Error();

        bool operator==(const Error& rhs) const noexcept;
        bool operator==(const eErrorCode& rhs) const noexcept;

        [[nodiscard]] explicit operator eErrorCode() const noexcept
        {
            return mErrorCode;
        }

    public:
        [[nodiscard]] bool IsFailed() const noexcept { return IS_ERROR(mErrorCode); }
        [[nodiscard]] bool IsSucceeded() const noexcept { return IsFailed() == false; }

        // HRESULT_CUSTOMER_BIT에 해당하는 비트를 추출하여, 0이면 Customer flag가 없는 것으로 판단
        [[nodiscard]] bool IsWin32Error() const noexcept
        {
            return (((static_cast<HRESULT>(mErrorCode) >> HRESULT_CUSTOMER_BIT) & 1) == 0);
        }

        [[nodiscard]] eErrorCode GetErrorCode() const noexcept { return mErrorCode; }
        [[nodiscard]] std::wstring GetErrorMessage() const;
        [[nodiscard]] std::wstring GetSourceFilePath() const { return mSourceLocation.mFilePath; }
        [[nodiscard]] int GetSourceLineNumber() const noexcept { return mSourceLocation.mLineNumber; }

    private:
        explicit Error() noexcept;

    private:
        eErrorCode mErrorCode = eErrorCode::Succeeded;
        SourceLocation mSourceLocation{};
    };

#define MAKE_ERROR(ErrorCode) CoTigraphy::Error::FromErrorCode(ErrorCode, __FILEW__, __LINE__)
#define MAKE_ERROR_FROM_HRESULT(ErrorCode) CoTigraphy::Error::FromHResult(ErrorCode, __FILEW__, __LINE__)
#define MAKE_ERROR_FROM_WIN32(ErrorCode) MAKE_ERROR_FROM_HRESULT(HRESULT_FROM_WIN32(ErrorCode))
#define MAKE_ERROR_FROM_LAST_WIN32_ERROR() MAKE_ERROR_FROM_WIN32(::GetLastError())

#define RETURN_IF_FAILED(expr) if (const CoTigraphy::Error _error = (expr); \
                                    _error.IsFailed()) return _error
} // namespace CoTigraphy
