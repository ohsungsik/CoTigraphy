// \file CommandLineParser.hpp
// \last_updated 2025-06-05
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include <functional>
#include <string>
#include <vector>

namespace CoTigraphy
{
	struct CommandLineOption
	{
		std::wstring mName;                 // 예: "--help"
		std::wstring mShortName;			// 예: "-h"
		std::wstring mDescription;			// 예: "Print help and exit"
		bool mRequiresValue;				// 값이 필요한 옵션인지 여부
		bool mCausesExit;					// 인자 처리 후 종료되어야 하는지 여부
		std::function<void(const std::wstring_view&)> mHandler;	// TODO: 이게 여기에 있는게 맞나?

		bool IsValid() const noexcept
		{
			return mName.empty() == false
				&& mShortName.empty() == false
				&& mDescription.empty() == false;
		}
	};

	struct CommandLineOptionContext
	{
		const std::vector<std::wstring>& mArgs;
		size_t mIndex = 0;
		std::unordered_map<std::wstring, const CommandLineOption*>& mProcessed;
		const CommandLineOption* mOption = nullptr;
		std::wstring mToken;
		std::wstring mValue;
		bool mEarlyExit = false;
		bool mOnlyCausesExit = false;
	};

	class CommandLineParser final
	{
	public:
		explicit CommandLineParser() noexcept;
		CommandLineParser(const CommandLineParser& other) = delete;
		CommandLineParser(CommandLineParser&& other) = delete;

		CommandLineParser& operator=(const CommandLineParser& rhs) = delete;
		CommandLineParser& operator=(CommandLineParser&& rhs) = delete;

		~CommandLineParser();

		Error AddOption(const CommandLineOption& option);

		Error Parse(const int argc, wchar_t* argv[]);
		Error Parse(const std::vector<std::wstring>& commandLineArguments);

		std::wostream& PrintHelpTo(std::wostream& os) const;

	private:
		Error TryProcessOption(CommandLineOptionContext& context);
		bool MatchOption(CommandLineOptionContext& context);
		Error CheckOptionValidity(CommandLineOptionContext& context) const;
		Error DispatchOptionHandler(const CommandLineOptionContext& context) const;

	private:
		std::unordered_map<std::wstring, const CommandLineOption*> mOptionLookup;

	private:
		std::vector<CommandLineOption> mCommandLineOptions;
	};
}
