// \file CoTigraphy.hpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

namespace CoTigraphy
{
	class CommandLineParser;

	Error Initialize(_Out_opt_ std::wstring& githubToken, _Out_opt_ std::wstring& userName);

	// Initialize
	Error SetupCommandLineParser(_In_ CoTigraphy::CommandLineParser& commandLineParser, _Out_opt_ std::wstring& githubToken, _Out_opt_ std::wstring& userName);

	Error Run(_In_ const std::wstring& githubToken, _In_ const std::wstring& userName);
	
}	// namespace CoTigraphy
