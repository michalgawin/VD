/**
 * DesktopMgr.cpp : Defines the entry point for the DLL application.
 * @author Micha³ Gawin
 */


#include <windows.h>
#include "resource.h"
#include "DesktopMgr.h"
#include "..\Sys.h"
#include "..\VirtualDesktop.h"


#pragma data_seg(".ASHARE")
INT CurrentDesktop = 0;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.ASHARE,RWS")


HINSTANCE hInstance;
CRITICAL_SECTION criticalSection;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hInstance = (HINSTANCE) hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeCriticalSection(&criticalSection);
		break;
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection(&criticalSection);
		break;
	default:
		break;
	}

    return TRUE;
}


extern "C" __declspec(dllexport) VOID __stdcall SetCurrentDesktop (INT desktop)
{
	EnterCriticalSection(&criticalSection);
	CurrentDesktop = desktop;
	LeaveCriticalSection(&criticalSection);
}


extern "C" __declspec(dllexport) INT __stdcall GetCurrentDesktop ()
{
	return CurrentDesktop;
}


extern "C" __declspec(dllexport) VOID __stdcall ChangeDesktop (int desktop)
{
	HWND hwnd = FindWindowEx (NULL, NULL, szClassName, NULL);

	if (hwnd != NULL)
	{
		EnterCriticalSection(&criticalSection);
		SendMessage (hwnd, WM_CHANGE_DESKTOP, 0, desktop);
		LeaveCriticalSection(&criticalSection);
	}
}
