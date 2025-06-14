// \file main.cpp
// \last_updated 2025-06-14
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"
#include <CoTigraphy.hpp>

int wmain()
{
	std::wstring githubToken;
	std::wstring userName;

	CoTigraphy::Error error = CoTigraphy::Initialize(githubToken, userName);
	if (error.IsFailed())
		return static_cast<int>(error.GetErrorCode());

	error = CoTigraphy::Run(githubToken, userName);
	if (error.IsFailed())
		return static_cast<int>(error.GetErrorCode());

	return 0;
}
