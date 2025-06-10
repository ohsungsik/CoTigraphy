// \file CoTigraphy.hpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#pragma once

#include "CommandLineParser.hpp"

namespace CoTigraphy
{
	Error Initialize();
	void Uninitialize() noexcept;


	// Initialize
	Error SetupCommandLineParser(CoTigraphy::CommandLineParser& commandLineParser, std::wstring& githubToken);



	// Uninitialize
}	// namespace CoTigraphy
