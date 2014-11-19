/**
 * @author Micha³ Gawin
 */


#ifndef _TRAY_H_VD_
#define _TRAY_H_VD_


#include "VirtualDesktop.h"


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
	CTray (HWND hwnd, HINSTANCE hInstance, HICON icon);

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
	 * @param nid pointer on NOTIFYICONDATA structure
	 */
	VOID ChangeIcon (HICON);
};


#endif //_TRAY_H_VD_
