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

		if (mOptionLookup.find(option.mName) != mOptionLookup.end())
			return MAKE_ERROR(eErrorCode::CommandLineArgumentsAlreadyExists);

		mCommandLineOptions.emplace_back(option);

		// 사용자가 Name 혹은 ShortName으로 입력할 수 있으므로 둘 다 매핑
		mOptionLookup[option.mName] = &mCommandLineOptions.back();
		if (option.mShortName.empty() == false)
			mOptionLookup[option.mShortName] = &mCommandLineOptions.back();

		return MAKE_ERROR(eErrorCode::Succeeded);
	}

	Error CommandLineParser::Parse(const int argc, wchar_t* argv[])
	{
		if (argc == 0 || argv == nullptr)
		{
			ASSERT(argc == 0 || argv != nullptr);
			return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);
		}

		std::vector<std::wstring> args;
		args.reserve(argc);
		for (int i = 1; i < argc; ++i)
		{
			args.emplace_back(argv[i]);
		}

		return Parse(args);
	}

	Error CommandLineParser::Parse(const std::vector<std::wstring>& commandLineArguments)
	{
		// 처리한 옵션(토큰별) 여부를 추적하기 위한 맵
		std::unordered_map<std::wstring, const CommandLineOption*> processedOptions{};

		// 1) 종료 옵션(Help, Version 등) 먼저 처리
		{
			size_t idx = 0;
			while (idx < commandLineArguments.size())
			{
				CommandLineOptionContext context{
					commandLineArguments, idx, processedOptions,
					nullptr, L"", L"", false, true // onlyCausesExit = true
				};

				const Error err = TryProcessOption(context);
				if (err.IsFailed())
					return err;

				if (context.mEarlyExit)
					return MAKE_ERROR(eErrorCode::EarlyExit);

				// 값을 소비했으면(RequiresValue=true) context.mIndex가 그 값 토큰을 가리킴
				idx = context.mIndex + 1;
			}
		}

		// 2) 일반 옵션 처리
		{
			size_t idx = 0;
			while (idx < commandLineArguments.size())
			{
				CommandLineOptionContext context{
					commandLineArguments, idx, processedOptions,
					nullptr, L"", L"", false, false // onlyCausesExit = false
				};

				const Error err = TryProcessOption(context);
				if (err.IsFailed())
					return err;

				idx = context.mIndex + 1;
			}
		}

		return MAKE_ERROR(eErrorCode::Succeeded);
	}

	std::wostream& CommandLineParser::PrintHelpTo(std::wostream& os) const
	{
		std::wstringstream stringstream;
		stringstream << L"CoTigraphy " << VERSION_STRING_WIDE << L"\n"
			<< L"Copyright (C) 2025. Oh Sungsik. All rights reserved.\n\n"
			<< L"Usage:\n  CoTigraphy [options]\n\n"
			<< L"Available options:\n";

		for (const auto& opt : mCommandLineOptions)
		{
			std::wostringstream oss;
			oss << opt.mShortName << L", " << opt.mName;
			if (opt.mRequiresValue)
				oss << L" <value>";

			stringstream << L"  " << std::setw(28) << std::left << oss.str() << opt.mDescription << L"\n";
		}

		stringstream << L"\nFor more information, visit: https://github.com/ohsungsik/CoTigraphy\n";
		os << stringstream.str();
		return os;
	}


	Error CommandLineParser::TryProcessOption(CommandLineOptionContext& context)
	{
		// 1) 옵션 이름이 mOptionLookup에 없으면 “알 수 없는 옵션” 처리
		if (!MatchOption(context))
		{
			// “알 수 없는 옵션”이 있을 때 에러 반환
			// (필요하다면, context.mToken을 std::wcerr로 찍어서 피드백 가능)
			return MAKE_ERROR(eErrorCode::CommandLineArgumentNotFound);
		}

		// 2) 중복 옵션 검사 및 값 소비 여부 확인
		if (const Error err = CheckOptionValidity(context); err.IsFailed())
			return err;

		// 3) 커스텀 핸들러 실행
		if (const Error err = DispatchOptionHandler(context); err.IsFailed())
			return err;

		// 4) 만약 “초기화 시 종료 옵션”(onlyCausesExit=true)이면 earlyExit 플래그를 세트
		if (context.mOption->mCausesExit)
		{
			context.mEarlyExit = true;
			return MAKE_ERROR(eErrorCode::EarlyExit);
		}

		return MAKE_ERROR(eErrorCode::Succeeded);
	}

	bool CommandLineParser::MatchOption(CommandLineOptionContext& context)
	{
		context.mToken = context.mArgs[context.mIndex];

		auto it = mOptionLookup.find(context.mToken);
		if (it == mOptionLookup.end())
			return false;

		context.mOption = it->second;
		if (context.mOption == nullptr)
			return false;

		// 종료 옵션 단계인지(false), 일반 옵션 단계인지(true)에 맞춰서 체크
		if (context.mOnlyCausesExit != context.mOption->mCausesExit)
			return false;

		return true;
	}

	Error CommandLineParser::CheckOptionValidity(CommandLineOptionContext& context) const
	{
		// 1) 이미 처리된 옵션이면 오류
		if (context.mProcessed.find(context.mToken) != context.mProcessed.end())
			return MAKE_ERROR(eErrorCode::CommandLineArgumentsAlreadyExists);

		context.mProcessed[context.mToken] = context.mOption;

		// 2) 만약 값이 필요한 옵션이라면, 다음 토큰이 값인지 판별
		if (context.mOption->mRequiresValue)
		{
			if (context.mIndex + 1 >= context.mArgs.size())
				return MAKE_ERROR_FROM_HRESULT(E_INVALIDARG);

			// 값 토큰으로 넘어가기
			context.mIndex += 1;
			context.mValue = context.mArgs[context.mIndex];
			if (context.mValue.empty())
				return MAKE_ERROR(eErrorCode::InvalidArguments);
		}

		return MAKE_ERROR(eErrorCode::Succeeded);
	}

	Error CommandLineParser::DispatchOptionHandler(const CommandLineOptionContext& context) const
	{
		if (context.mOption->mHandler)
			context.mOption->mHandler(context.mValue);

		return MAKE_ERROR(eErrorCode::Succeeded);
	}
}
