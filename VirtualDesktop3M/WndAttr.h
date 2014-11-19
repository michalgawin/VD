/**
 * @author Micha³ Gawin
 */


#ifndef _WND_ATTR_H_VD_
#define _WND_ATTR_H_VD_


#include "VirtualDesktop.h"


/**
 * Function make windows transparent
 * @return TRUE if wasn't error
 * @param hWnd handle to window
 * @param factor value of transparent beetwen 0 and 255
 */
BOOL MakeWindowTransparent (HWND hWnd, UCHAR factor);

/**
 * Function slowly hide window
 * @return TRUE if wasn't error
 * @param hWnd handle to window
 * @param time value to postpone hide window
 * @param value from which window will be make transparent
 */
BOOL SlowHideWindow (HWND hWnd, INT time, INT value);

/**
 * Function slowly show window
 * @return TRUE if wasn't error
 * @param hWnd handle to window
 * @param time value to postpone show window
 * @param value from which window will be make transparent
 */
BOOL SlowShowWindow (HWND hWnd, INT time, INT value);


#endif //_WND_ATTR_H_VD_
