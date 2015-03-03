/**
 * @author Micha³ Gawin
 */


#ifndef _TRAY_H_VD_
#define _TRAY_H_VD_

#pragma comment(lib, "Msimg32.lib")

#include "VirtualDesktop.h"
#include <WinGdi.h>


class CTray
{
private:
	NOTIFYICONDATA nid;				// struct to tray icon

public:
	/**
	 * Method initialize tray structure
	 * @param hwnd handle to window
	 * @param hInstance handle to instance of application
	 * @param icon id of icon in application resources
	 */
	CTray (HWND hwnd, HINSTANCE hInstance);

	/**
	 * Function show tray icon
	 * @return void
	 * @param nid pointer on NOTIFYICONDATA structure
	 */
	VOID Show ();

	/**
	 * Function hide tray icon
	 * @return void
	 * @param nid pointer on NOTIFYICONDATA structure
	 */
	VOID Hide ();

	/**
	 * Function modify tray icon
	 * @return void
	 * @param szFormat format of arguments
	 */
	VOID ChangeIcon(TCHAR* szFormat, ...);
};


#endif //_TRAY_H_VD_
