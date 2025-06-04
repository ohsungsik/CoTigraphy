// \file main.cpp
// \last_updated 2025-06-04
// \author Oh Sungsik <ohsungsik@outlook.com>
// \copyright (C) 2025. Oh Sungsik. All rights reserved.

#include "pch.hpp"

#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"

int wmain()
{
	MemoryLeakDetector::Initialize();
	HandleLeakDetector::Initialize();

	return 0;
}
