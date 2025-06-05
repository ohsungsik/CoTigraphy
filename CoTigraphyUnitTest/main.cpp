#include "pch.hpp"


int wmain(int argc, wchar_t* argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	const int ret = RUN_ALL_TESTS();
	return ret;
}
