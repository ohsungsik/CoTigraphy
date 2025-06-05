#include "pch.hpp"
#include "CoTigraphy.hpp"

#include <iostream>
#include <shellapi.h>


#include <string_view>


#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"
#include "VersionInfo.hpp"

namespace CoTigraphy
{
	Error Initialize()
	{
		CoTigraphy::MemoryLeakDetector::Initialize();
		CoTigraphy::HandleLeakDetector::Initialize();

		CoTigraphy::CommandLineParser commandLineParser;
		Error error = SetupCommandLineParser(commandLineParser);
		if (error.IsFailed())
			return error;

		int argc = 0;
		LPWSTR* const argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		error = commandLineParser.Parse(argc, argv);
		LocalFree(argv);
		if (error.IsFailed())
		{
			ASSERT(error.IsSucceeded());	// Commandl line arguments 파싱 실패
			return error;
		}


		return MAKE_ERROR(eErrorCode::Succeeded);
	}

	void Uninitialize() noexcept
	{

	}

	Error SetupCommandLineParser(CoTigraphy::CommandLineParser& commandLineParser)
	{
		Error error = commandLineParser.AddOption(CommandLineOption{
	            L"--help",                      // mName
	            L"-h",                          // mShortName
	            L"Show help message",          // mDescription
	            false,                         // mRequiresValue
	            true,                          // mCausesExit
	            [&](const std::wstring_view&)
		        {
		            commandLineParser.PrintHelpTo(std::wcout);
	            }
			});
		if (error.IsFailed())
		{
			ASSERT(error.IsSucceeded());
			return error;
		}

		error = commandLineParser.AddOption(CommandLineOption{
				L"--version",					// mName
				L"-v",							// mShortName
			    L"Show program version",		// mDescription
				false,							// mRequiresValue
				false,							// mCausesExit
				[&](const std::wstring_view& value)	// mHandler
				{
					UNREFERENCED_PARAMETER(value);

					std::wstring versionInfo = L"Version: ";
					versionInfo += VERSION_STRING_WIDE;
					versionInfo += L"\n";
					OutputDebugStringW(versionInfo.c_str());
				}
			});
		if (error.IsFailed())
		{
			ASSERT(error.IsSucceeded());
			return error;
		}

		error = commandLineParser.AddOption(CommandLineOption{
				L"--token",									// mName
				L"-t",										// mShortName
				L"Github personal access token",			// mDescription
				true,										// mRequiresValue
				false,										// mCausesExit
				[&](const std::wstring_view& value)				// mHandler
				{
					std::wstring accessToken = L"Github presonal access token: ";
					accessToken += value;
					accessToken += L"\n";
					OutputDebugStringW(accessToken.c_str());
				}
			});
		if (error.IsFailed())
		{
			ASSERT(error.IsSucceeded());
			return error;
		}

		return MAKE_ERROR(eErrorCode::Succeeded);
	}
}
