/**
 * @author Micha³ Gawin
 */


#ifndef _PLUGIN_H_VD_
#define _PLUGIN_H_VD_

#include "VirtualDesktop.h"

const char ExFunNameMakeDialog[] = "_MakeDialog@8";
const char ExFunNameCloseDialog[] = "_CloseDialog@4";

/** Pointer on function which open plugin
 * @return void
 * @param hwnd handle to window
 */
typedef HWND (__stdcall *t_PluginFunc) (HWND hwnd, HINSTANCE shared_DLL);

/** Pointer on function which close plugin
 * @param hwnd handle to plugin window
 * @return void
 */
typedef VOID (__stdcall *t_PluginClose) (HWND hDlg);

/** Pointer on function which change desktop
 * @return void
 * @param new number of desktop
 */
typedef VOID (__stdcall *t_PluginChangeDsk) (INT);

/** Pointer on function which get current number of desktop
 * @return number of desktop
 */
typedef INT (__stdcall *t_PluginGetCurDsk) ();

typedef struct __PluginGUI
{
	t_PluginFunc PluginOpenDlgFun;
	t_PluginClose PluginCloseDlgFun;

	HWND hWnd;
	TCHAR szPluginPath[MAX_PATH];					// path to plugin
} PluginGUI;


/**
 * Function load plugin
 * @return instance of dll file
 * @param dll_name name of loaded dll file
 */
HINSTANCE LoadPlugin (TCHAR* szDll);

/**
 * Function load function from dll
 * @return address of loaded function
 * @param hPlug handle to dll library
 * @param fun_name name of loaded function
 */
VOID* LoadPluginFunc (HINSTANCE hPlug, const char const * szFunc);

/**
 * Function free resources of loaded plugin
 * @return TRUE if wasn't error
 * @param hPlug handle to dll which we want free
 */
BOOL FreePlugin (HINSTANCE hPlug);

/**
 * Function load plugin and get all needed functions
 * @return TRUE if all was loaded success
 * @param hPlug handle to instance of plugin
 * @param PluginPath path to plugin
 * @param PluginDlg handle to exported function to open dialog
 * @param PluginClose handle to exported function to close dialog
 */
BOOL GetPlugin (HINSTANCE &hPlug, TCHAR* szPath, t_PluginFunc& PluginOpenDlg, t_PluginClose& PluginCloseDlg);

/**
 * Dialog window proc
 */
BOOL CALLBACK DlgPluginProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#endif //_PLUGIN_H_VD_
