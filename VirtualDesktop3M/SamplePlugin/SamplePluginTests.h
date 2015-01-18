
#include "SamplePlugin.h"
#include "Plugin.h"
#include "gtest\gtest.h"


class CSamplePluginTest : public ::testing::Test
{
protected:
	CSamplePluginTest();
	~CSamplePluginTest();

	CPlugin plugin;
};
