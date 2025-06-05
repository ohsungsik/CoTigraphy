// \file CommandLineParser.cpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include "CommandLineParser.hpp"

#include <iomanip>
#include <iostream>
#include <unordered_set>

#include "VersionInfo.hpp"

namespace CoTigraphy
{
	CommandLineParser::CommandLineParser() noexcept
		= default;

	CommandLineParser::~CommandLineParser()
		= default;

	Error CommandLineParser::AddOption(const CommandLineOption& option)
	{
		if (option.IsValid() == false)
		{
			ASSERT(option.IsValid());
			return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
		}

		for (const auto& commandLineOption : mCommandLineOptions)
		{
			if (commandLineOption.mName == option.mName || commandLineOption.mShortName == option.mShortName)
			{
				return MAKE_ERROR(eErrorCode::CommandLineArgumentsAlreadyExists);
			}
		}

		mCommandLineOptions.emplace_back(option);

		return MAKE_ERROR(eErrorCode::Succeeded);
	}

	Error CommandLineParser::Parse(const int argc, wchar_t* argv[]) const
	{
		if (argc == 0 || argv == nullptr)
		{
			ASSERT(argc == 0 || argv != nullptr);
			return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
		}

		std::vector<std::wstring> args;
		args.reserve(argc);
		for (int i = 0; i < argc; ++i)
		{
			args.emplace_back(argv[i]);
		}

		return Parse(args);
	}

	Error CommandLineParser::Parse(const std::vector<std::wstring>& commandLineArguments) const
	{
		// 처리된 옵션인지 판단하기 위한 변수
		// 동일한 인자가 두번 이상 입력되면 오류로 처리한다.
		std::unordered_map<std::wstring, const CommandLineOption*> processedOptions;

		// 1. 종료 플래그(mCausesExit)가 있는 옵션을 먼저 처리
		for (size_t i = 0; i < commandLineArguments.size(); ++i)
		{
			const std::wstring& token = commandLineArguments[i];

			for (const auto& option : mCommandLineOptions)
			{
				if ((token == option.mName || token == option.mShortName) && option.mCausesExit)
				{
					if (processedOptions.find(token) != processedOptions.end())
					{
						// 중복된 인자
						// 이전에 이미 처리됨
						// 오류로 처리
						return MAKE_ERROR(eErrorCode::CommandLineArgumentsAlreadyExists);
					}
					processedOptions[token] = &option;

					std::wstring value;
					if (option.mRequiresValue)
					{
						if (i + 1 >= commandLineArguments.size())
							return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
						value = commandLineArguments[i + 1];

						if (value.empty())
						{
							// 값이 필요한데 비어있는 경우 오류 처리
							return MAKE_ERROR(eErrorCode::InvalidArguments);
						}
					}

					if (option.mHandler)
						option.mHandler(value);

					// 종료 플래스가 설정되었다면 다른 옵션을 처리하지 않고 종료
					return MAKE_ERROR(eErrorCode::EarlyExit);
				}
			}
		}

		// 2. 일반 옵션 처리
		for (size_t i = 0; i < commandLineArguments.size(); ++i)
		{
			const std::wstring& token = commandLineArguments[i];
			bool matched = false;

			for (const auto& option : mCommandLineOptions)
			{
				if (token == option.mName || token == option.mShortName)
				{
					matched = true;

					if (processedOptions.find(token) != processedOptions.end())
					{
						// 중복된 인자
						// 이전에 이미 처리됨
						// 오류로 처리
						return MAKE_ERROR(eErrorCode::CommandLineArgumentsAlreadyExists);
					}
					processedOptions[token] = &option;

					if (option.mRequiresValue)
					{
						// 다음 인자가 없는 경우 오류
						if (i + 1 >= commandLineArguments.size())
						{
							return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
						}

						const std::wstring& value = commandLineArguments[++i];
						if (option.mHandler)
							option.mHandler(value);
					}
					else
					{
						if (option.mHandler)
							option.mHandler(L"");
					}

					break;
				}
			}

			if (!matched)
			{
				// 등록되지 않은 옵션: 무시
				continue;
			}
		}

		return MAKE_ERROR(eErrorCode::Succeeded);
	}

	void CommandLineParser::PrintHelp() const
	{
		std::wcout << L"CoTigraphy " << VERSION_STRING_WIDE << L"\n";
		std::wcout << L"Copyright (C) 2025. Oh Sungsik. All rights reserved.\n\n";

		std::wcout << L"Usage:\n"
			L"  CoTigraphy [options]\n\n";

		std::wcout << L"Available options:\n";

		for (const auto& opt : mCommandLineOptions)
		{
			std::wostringstream oss;
			oss << opt.mShortName << L", " << opt.mName;

			if (opt.mRequiresValue)
			{
				oss << L" ";	// token과 값 구분자
				oss << L"<value>";
			}

			std::wcout << L"  " << std::setw(28) << std::left << oss.str()
				<< opt.mDescription << L"\n";
		}

		std::wcout << L"\n";
		std::wcout << L"For more information, visit: https://github.com/ohsungsik/CoTigraphy\n";
	}
}
