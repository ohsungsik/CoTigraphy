// \file main.cpp
// \last_updated 2025-06-10
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include <CoTigraphy.hpp>

int wmain()
{
	const CoTigraphy::Error error = CoTigraphy::Initialize();
	if (error.IsFailed())
		return static_cast<int>(error.GetErrorCode());

	CoTigraphy::Uninitialize();

	return 0;
}
