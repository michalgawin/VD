
#include "CPluginTests.h"
#include "gtest\gtest.h"


CPluginTest::CPluginTest() {}


TEST_F(CPluginTest, Load)
{
	EXPECT_TRUE(plugin.GetFile() == NULL);
	EXPECT_TRUE((plugin.m_pfMakeDialog == NULL) && (plugin.m_pfCloseDialog == NULL));

	plugin.SetFile(SZ_DEFAULT_PLUGIN_NAME);
	EXPECT_TRUE(plugin.GetFile() != NULL) << "Cannot get path to plugin file";

	EXPECT_TRUE(plugin.Load()) << "Invalid load status of plugin";
	ASSERT_TRUE(plugin.m_pfMakeDialog != NULL) << "Cannot get pointer to MakeDialog function";
	ASSERT_TRUE(plugin.m_pfCloseDialog != NULL) << "Cannot get pointer to CloseDialog function";

	plugin.~CPlugin();
}

TEST_F(CPluginTest, Unload)
{
	plugin.SetFile(SZ_DEFAULT_PLUGIN_NAME);

	EXPECT_TRUE(plugin.Load());

	plugin.Unload();
	EXPECT_TRUE(plugin.GetFile() != NULL) << "Path to plugin should not be removed";
	ASSERT_TRUE(plugin.m_pfMakeDialog == NULL) << "Pointer to MakeDialog is not erased";
	ASSERT_TRUE(plugin.m_pfCloseDialog == NULL) << "Pointer to CloseDialog is not erased";

	plugin.~CPlugin();
	EXPECT_TRUE(plugin.GetFile() == NULL) << "Path to plugin should be removed";
}

TEST_F(CPluginTest, OpenCloseWindow)
{
	plugin.SetFile(SZ_DEFAULT_PLUGIN_NAME);
	EXPECT_TRUE(plugin.Load());
	ASSERT_TRUE(plugin.m_pfMakeDialog != NULL) << "Pointer to MakeDialog is not valid";
	ASSERT_TRUE(plugin.m_pfCloseDialog != NULL) << "Pointer to CloseDialog is not valid";

	HWND hWnd = plugin.m_pfMakeDialog(NULL, NULL);
	EXPECT_TRUE(IsWindow(hWnd)) << "Cannot create window";
	EXPECT_TRUE(IsWindowVisible(hWnd)) << "Window is not visible";

	EXPECT_TRUE(plugin.m_pfCloseDialog()) << "Cannot close window";
	EXPECT_FALSE(IsWindow(hWnd)) << "Window is visible";

	plugin.Unload();
}

TEST_F(CPluginTest, ChangeDesktop)
{
	plugin.SetFile(SZ_DEFAULT_PLUGIN_NAME);
	EXPECT_TRUE(plugin.Load());
	ASSERT_TRUE((plugin.m_pfMakeDialog != NULL) && (plugin.m_pfCloseDialog != NULL));
	
	HWND hWnd = plugin.m_pfMakeDialog(NULL, NULL);
	EXPECT_TRUE(IsWindow(hWnd) && IsWindowVisible(hWnd));

	EXPECT_FALSE(SendMessage(hWnd, WM_COMMAND, MAKEWPARAM((BUTTON_ID0 + 1), 0), 0));	//Virtual Desktop is shutdown
	EXPECT_FALSE(SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(BUTTON_ID0, 0), 0));	//Virtual Desktop is shutdown

	EXPECT_TRUE(plugin.m_pfCloseDialog());
	
	plugin.Unload();
}
