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

TEST(PluginTest, LoadPluginLibrary)
{
	CPlugin Library;
	Library.LoadAll(SZ_PLUGIN_NAME);
	ASSERT_TRUE((Library.m_pfMakeDialog != NULL) && (Library.m_pfCloseDialog != NULL));
	Library.~CPlugin();
}

TEST(PluginTest, OpenPluginWindow)
{
	CPlugin Library;
	Library.LoadAll(SZ_PLUGIN_NAME);
	ASSERT_TRUE((Library.m_pfMakeDialog != NULL) && (Library.m_pfCloseDialog != NULL));

	HWND hWnd = Library.m_pfMakeDialog(NULL, NULL);
	ASSERT_TRUE(IsWindow(hWnd));
	ASSERT_TRUE(IsWindowVisible(hWnd));
}

TEST(PluginTest, ClosePluginWindow)
{
	CPlugin Library;
	Library.LoadAll(SZ_PLUGIN_NAME);
	ASSERT_TRUE((Library.m_pfMakeDialog != NULL) && (Library.m_pfCloseDialog != NULL));

	HWND hWnd = Library.m_pfMakeDialog(NULL, NULL);

	BOOL status = Library.m_pfCloseDialog();
	ASSERT_TRUE(status);
	ASSERT_TRUE(!IsWindow(hWnd));
}

TEST(PluginTest, ChangeDesktop)
{
	CPlugin Library;
	Library.LoadAll(SZ_PLUGIN_NAME);
	ASSERT_TRUE((Library.m_pfMakeDialog != NULL) && (Library.m_pfCloseDialog != NULL));

	HWND hWnd = Library.m_pfMakeDialog(FindWindowEx(NULL, NULL, szClassName, NULL), NULL);
	ASSERT_TRUE(IsWindow(hWnd) && IsWindowVisible(hWnd));
	
	BOOL status = SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(BUTTON_ID0, 0), 0);
	BOOL status2 = SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(BUTTON_ID0+1, 0), 0);
	ASSERT_TRUE(status || status2);

	status = Library.m_pfCloseDialog();
	ASSERT_TRUE(status);
}