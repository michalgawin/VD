// DesktopMgr_UnitTests.cpp : Defines the entry point for the console application.
//


#include "DesktopMgr.h"
#include "gtest\gtest.h"


int _tmain(int argc, _TCHAR* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


TEST(DesktopMgrTest, LoadSharedLibrary)
{
	HMODULE hPlug = LoadLibrary(SZ_DESKTOP_MGR);
	ASSERT_TRUE(hPlug != NULL);

	FreeLibrary(hPlug);
}

TEST(DesktopMgrTest, GetFunSetCurrentDesktop)
{
	HMODULE hPlug = LoadLibrary(SZ_DESKTOP_MGR);
	ASSERT_TRUE(hPlug != NULL);

	t_pfSetCurrentDesktop SetCurrentDesktop = (t_pfSetCurrentDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameSetCurrentDesktop));
	ASSERT_TRUE(SetCurrentDesktop != NULL);

	FreeLibrary(hPlug);
}

TEST(DesktopMgrTest, GetFunGetCurrentDesktop)
{
	HMODULE hPlug = LoadLibrary(SZ_DESKTOP_MGR);
	ASSERT_TRUE(hPlug != NULL);

	t_pfGetCurrentDesktop GetDesktopNumber = (t_pfGetCurrentDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameGetCurrentDesktop));
	ASSERT_TRUE(GetDesktopNumber != NULL);

	FreeLibrary(hPlug);
}

TEST(DesktopMgrTest, GetFunChangeDesktop)
{
	HMODULE hPlug = LoadLibrary(SZ_DESKTOP_MGR);
	ASSERT_TRUE(hPlug != NULL);

	t_pfChangeDesktop ChangeDesktop = (t_pfChangeDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameChangeDesktop));
	ASSERT_TRUE(ChangeDesktop != NULL);

	FreeLibrary(hPlug);
}

TEST(DesktopMgrTest, InitDesktopNumber)
{
	HMODULE hPlug = LoadLibrary(SZ_DESKTOP_MGR);
	t_pfGetCurrentDesktop GetDesktopNumber = (t_pfGetCurrentDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameGetCurrentDesktop));

	ASSERT_EQ(GetDesktopNumber(), 0);

	FreeLibrary(hPlug);
}

TEST(DesktopMgrTest, SetGetDesktopNumber)
{
	HMODULE hPlug = LoadLibrary(SZ_DESKTOP_MGR);
	t_pfSetCurrentDesktop SetCurrentDesktop = (t_pfSetCurrentDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameSetCurrentDesktop));
	t_pfGetCurrentDesktop GetDesktopNumber = (t_pfGetCurrentDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameGetCurrentDesktop));

	SetCurrentDesktop(0);
	ASSERT_EQ(GetDesktopNumber(), 0);

	SetCurrentDesktop(INT_MAX);
	ASSERT_EQ(GetDesktopNumber(), INT_MAX);

	SetCurrentDesktop(INT_MIN);
	ASSERT_EQ(GetDesktopNumber(), INT_MIN);

	FreeLibrary(hPlug);
}

TEST(DesktopMgrTest, ChangeDesktop)
{
	HMODULE hPlug = LoadLibrary(SZ_DESKTOP_MGR);
	t_pfGetCurrentDesktop GetDesktopNumber = (t_pfGetCurrentDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameGetCurrentDesktop));
	t_pfChangeDesktop ChangeDesktop = (t_pfChangeDesktop)((VOID*)GetProcAddress(hPlug, ExFunNameChangeDesktop));

	BOOL ret;

	ret = FALSE;
	ret = ChangeDesktop(0);
	ASSERT_TRUE(ret);
	ASSERT_EQ(GetDesktopNumber(), 0);

	ret = FALSE;
	ret = ChangeDesktop(INT_MAX);
	ASSERT_FALSE(ret);
	ASSERT_EQ(GetDesktopNumber(), 0);

	ret = FALSE;
	ret = ChangeDesktop(INT_MIN);
	ASSERT_FALSE(ret);
	ASSERT_EQ(GetDesktopNumber(), 0);

	FreeLibrary(hPlug);
}
