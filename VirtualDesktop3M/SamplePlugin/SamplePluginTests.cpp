
#include "SamplePluginTests.h"
#include "gtest\gtest.h"


CSamplePluginTest::CSamplePluginTest()
{
	plugin.SetFile(SZ_DEFAULT_PLUGIN_NAME);
	plugin.Load();
}

CSamplePluginTest::~CSamplePluginTest()
{
	plugin.~CPlugin();
}


TEST_F(CSamplePluginTest, ChangeDesktop)
{
	HWND hWnd = plugin.m_pfMakeDialog(NULL, NULL);
	ASSERT_TRUE(IsWindow(hWnd));
	ASSERT_TRUE(IsWindowVisible(hWnd));

	ASSERT_FALSE(SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(BUTTON_ID0 + 1, 0), 0));
	ASSERT_FALSE(SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(BUTTON_ID0, 0), 0));

	ASSERT_TRUE(plugin.m_pfCloseDialog());
}

TEST_F(CSamplePluginTest, LButtonDown)
{
	HWND hWnd = plugin.m_pfMakeDialog(NULL, NULL);
	ASSERT_TRUE(IsWindow(hWnd));
	ASSERT_TRUE(IsWindowVisible(hWnd));
	
	ASSERT_FALSE(SendMessage(hWnd, WM_LBUTTONDOWN, MAKEWPARAM(MK_LBUTTON, 0), MAKELPARAM(40, 40)));
	ASSERT_FALSE(SendMessage(hWnd, WM_LBUTTONDOWN, MAKEWPARAM(MK_LBUTTON, 0), MAKELPARAM(10000, 10000)));

	ASSERT_TRUE(plugin.m_pfCloseDialog());
}

TEST_F(CSamplePluginTest, Ok)
{
	HWND hWnd = plugin.m_pfMakeDialog(NULL, NULL);
	ASSERT_TRUE(IsWindow(hWnd));
	ASSERT_TRUE(IsWindowVisible(hWnd));

	EXPECT_FALSE(SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0)) << "Command IDOK is handled";
	EXPECT_TRUE(IsWindow(hWnd));
	EXPECT_TRUE(IsWindowVisible(hWnd));

	EXPECT_TRUE(plugin.m_pfCloseDialog());
}

TEST_F(CSamplePluginTest, Cancel)
{
	HWND hWnd = plugin.m_pfMakeDialog(NULL, NULL);
	ASSERT_TRUE(IsWindow(hWnd));
	ASSERT_TRUE(IsWindowVisible(hWnd));

	ASSERT_TRUE(SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0)) << "Command IDCANCEL is not handled properly";
	ASSERT_FALSE(IsWindow(hWnd));
	ASSERT_FALSE(IsWindowVisible(hWnd));

	ASSERT_FALSE(plugin.m_pfCloseDialog()) << "Handle to window is still valid";
}
