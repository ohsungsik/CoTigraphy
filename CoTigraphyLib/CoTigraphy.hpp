
#pragma once

#include "CommandLineParser.hpp"

namespace CoTigraphy
{
	Error Initialize();
	void Uninitialize() noexcept;


	// Initialize
	Error SetupCommandLineParser(CoTigraphy::CommandLineParser& commandLineParser);



	// Uninitialize
}
