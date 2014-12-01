/**
 * @author Micha³ Gawin
 */


#ifndef _WINDOWS_MANAGER_H_VD_
#define _WINDOWS_MANAGER_H_VD_

#pragma comment(lib, "Comctl32.lib")

#include "VirtualDesktop.h"
#include <commctrl.h>


BOOL CALLBACK DlgDesktopManagerProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#endif //_WINDOWS_MANAGER_H_VD_
