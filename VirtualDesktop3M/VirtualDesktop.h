/**
 * @author Micha³ Gawin
 */


#ifndef _VIRTUAL_DESKTOP_H_VD_
#define _VIRTUAL_DESKTOP_H_VD_


#define _WIN32_IE		0x0501 // need for NOTIFYICONDATA
#define _WIN32_WINNT	0x0501 // need for SetLayeredWindowAttributes

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <vector>

#include "resource.h"
#include "Sys.h"

#define DESKTOPS			5

typedef std::vector<HWND> vHandle;
typedef vHandle::iterator vHandleItor;

/**
 * Structure contain information about windows on each desktop
 */
typedef struct __WindowsOnDesktop
{
	vHandle table;
	TCHAR *szWallpaper;						/** keep path to wallpaper */
} WindowsOnDesktop, *pWindowsOnDesktop;


/**
 * IDs for hot keys
 */
#define PREV_WIN		1
#define NEXT_WIN		2


#endif //_VIRTUAL_DESKTOP_H_VD_
