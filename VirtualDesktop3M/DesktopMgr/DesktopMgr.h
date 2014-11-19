/**
 * @author Micha³ Gawin
 */


#ifndef _DESKTOP_MGR_H_VD_
#define _DESKTOP_MGR_H_VD_

/**
 * Function responsible for set current desktop
 * @return void
 * @param desktop desktop no.
 */
extern "C" __declspec(dllexport) VOID __stdcall SetCurrentDesktop (INT desktop);

/**
 * Function get current desktop
 * @return current desktop
 */
extern "C" __declspec(dllexport) INT __stdcall GetCurrentDesktop ();

/**
 * Function responsible for change desktop
 * @return void
 * @param desktop new desktop no.
 */
extern "C" __declspec(dllexport) VOID __stdcall ChangeDesktop (int desktop);

#endif //_DESKTOP_MGR_H_VD_
