#include "pch.hpp"

#include "HandleLeakDetector.hpp"
#include "MemoryLeakDetector.hpp"

int wmain()
{
	MemoryLeakDetector::Initialize();
	HandleLeakDetector::Initialize();

	return 0;
}