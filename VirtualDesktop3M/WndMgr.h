/**
 * @author Micha³ Gawin
 */


#ifndef _WND_MGR_H_VD_
#define _WND_MGR_H_VD_

#include "VirtualDesktop.h"


extern WindowsOnDesktop windowsOnDesktop[DESKTOPS];

/**
* Check whether Virtual Desktop is running already
* @return handle to window if works, NULL otherwise
* @param clsName - windows class name
*/
HWND FindApplication(const TCHAR const * clsName);

/**
 * Function get all windows from desktop
 * @return number of windows
 * @param hApp handle to application
 * @param table of windows handle to hide
 */
INT GetWindowsFromDesktop (HWND hApp, vHandle& table);

/**
 * Function hide windows
 * @return number of hided windows
 * @param hApp handle to application
 * @param table of windows handle to hide
 */
INT HideWindows (HWND hApp, vHandle& table, BOOL update);

/**
 * Function show windows
 * @return number of showed application
 * @param table of windows handle to show
 * @param number of handles in table
 */
INT ShowWindows (vHandle& table);


#endif //_WND_MGR_H_VD_
