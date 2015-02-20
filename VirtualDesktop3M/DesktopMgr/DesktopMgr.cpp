/**
 * DesktopMgr.cpp : Defines the entry point for the DLL application.
 * @author Micha³ Gawin
 */


#include "DesktopMgr.h"
#include "resource.h"
#include "VirtualDesktop.h"
#include "DesktopsManager.h"


#pragma data_seg(".ASHARE")
INT CurrentDesktop = 0;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.ASHARE,RWS")


static HINSTANCE g_hInstance;
static CRITICAL_SECTION g_criticalSection;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeCriticalSection(&g_criticalSection);
		break;
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection(&g_criticalSection);
		break;
	default:
		break;
	}

    return TRUE;
}


extern "C" __declspec(dllexport) VOID __stdcall SetCurrentDesktop (INT desktop)
{
	EnterCriticalSection(&g_criticalSection);
	CurrentDesktop = desktop;
	LeaveCriticalSection(&g_criticalSection);
}


extern "C" __declspec(dllexport) INT __stdcall GetCurrentDesktop ()
{
	return CurrentDesktop;
}


extern "C" __declspec(dllexport) BOOL __stdcall ChangeDesktop (INT desktop)
{
	BOOL ret = FALSE;
	HWND hwnd = CDesktop::FindApplication(szClassName);

	if (hwnd != NULL)
	{
		EnterCriticalSection(&g_criticalSection);
		ret = (SendMessage(hwnd, WM_CHANGE_DESKTOP, 0, desktop) == 0) ? TRUE : FALSE;
		LeaveCriticalSection(&g_criticalSection);
	}

	return ret;
}
