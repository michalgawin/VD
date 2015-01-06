/**
 * @author Micha³ Gawin
 */


#ifndef _DESKTOP_MGR_H_VD_
#define _DESKTOP_MGR_H_VD_

#include "targetver.h"
#include "VirtualDesktop.h"

/**
 * Set current desktop. It does not change desktop.
 * @return
 * @param desktop new desktop no.
 */
extern "C" __declspec(dllexport) VOID __stdcall SetCurrentDesktop (INT desktop);

const char ExFunNameSetCurrentDesktop[] = "_SetCurrentDesktop@4";
typedef VOID(__stdcall *t_pfSetCurrentDesktop)(INT);

/**
 * Get current desktop.
 * @return number of current desktop
 */
extern "C" __declspec(dllexport) INT __stdcall GetCurrentDesktop ();

const char ExFunNameGetCurrentDesktop[] = "_GetCurrentDesktop@0";
typedef INT(__stdcall *t_pfGetCurrentDesktop) ();

/**
 * Change desktop.
 * @return TRUE if desktop has been changed with success, FALSE otherwise
 * @param desktop new desktop no.
 */
extern "C" __declspec(dllexport) BOOL __stdcall ChangeDesktop (int desktop);

const char ExFunNameChangeDesktop[] = "_ChangeDesktop@4";
typedef BOOL(__stdcall *t_pfChangeDesktop) (INT);

#endif //_DESKTOP_MGR_H_VD_
