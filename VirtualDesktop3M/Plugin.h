/**
* @author Micha³ Gawin
*/


#ifndef _PLUGIN_H_VD_
#define _PLUGIN_H_VD_

#include "VirtualDesktop.h"

const char ExFunNameMakeDialog[] = "_MakeDialog@8";
const char ExFunNameCloseDialog[] = "_CloseDialog@0";

/** Pointer on function which open plugin
* @return void
* @param hwnd handle to window
*/
typedef HWND(__stdcall *t_PluginFunc) (HWND hwnd, HINSTANCE shared_DLL);

/** Pointer on function which close plugin
* @param hwnd handle to plugin window
* @return void
*/
typedef BOOL(__stdcall *t_PluginClose) ();

/** Pointer on function which change desktop
* @return void
* @param new number of desktop
*/
typedef VOID(__stdcall *t_PluginChangeDsk) (INT);

/** Pointer on function which get current number of desktop
* @return number of desktop
*/
typedef INT(__stdcall *t_PluginGetCurDsk) ();


class CPlugin
{
private:
	HMODULE m_hLib;		//handle to library (dll)
	TCHAR* m_szFullPath;	//full path, including file name, to library

public:
	t_PluginFunc m_pfOpenDlg;	//pointer to function which creates window
	t_PluginClose m_pfCloseDlg;	//pointer to function which close window

public:
	CPlugin();
	~CPlugin();
	BOOL LoadAll(TCHAR* szFullPath);
	TCHAR* SetFullPath(TCHAR* szFullPath);
	TCHAR* GetFullPath() { return m_szFullPath; };

private:
	BOOL Load();
	VOID Unload();
	BOOL GetFunc(IN const char* szFuncName, OUT VOID** pFun);
};

/**
* Dialog window proc
*/
BOOL CALLBACK DlgPluginProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#endif //_PLUGIN_H_VD_
