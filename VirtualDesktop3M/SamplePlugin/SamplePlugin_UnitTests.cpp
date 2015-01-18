// SamplePlugin_UnitTests.cpp : Defines the entry point for the console application.
//

#include "SamplePlugin.h"
#include "gtest\gtest.h"
#include "Plugin.h"


int _tmain(int argc, _TCHAR* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

