/**
 * @author Micha³ Gawin
 */


#ifndef _WINDOWS_MANAGER_H_VD_
#define _WINDOWS_MANAGER_H_VD_

#pragma comment(lib, "Comctl32.lib")

#include "VirtualDesktop.h"
#include <commctrl.h>

#define OBM_ZOOM            32748
#define OBM_ZOOMD           32745

BOOL CALLBACK DlgDesktopManagerProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#endif //_WINDOWS_MANAGER_H_VD_
