/**
 * @author Micha³ Gawin
 */


#ifndef _SAMPLE_PLUGIIN_H_VD_
#define _SAMPLE_PLUGIIN_H_VD_

#include "targetver.h"
#include "VirtualDesktop.h"

#define ID_TIMER    1

#define WIDE_RATIO	8	//ratio of window to the screen
#define HIGH_RATIO	3	//ratio of window to the screen

#define BUTTON_ID0	10000	//id of first desktop-button
#define DESKTOPS_ON_PAGE	5	//no. of desktop-buttons on page


/**
 * Creates GUI to change desktop.
 * @return handle to plugin's window
 * @param hwnd handle of parent window
 * @param hSharedLib handle to instance of plugin -- not used by SamplePlugin
 */
extern "C" __declspec(dllexport) HWND __stdcall MakeDialog (HWND hwnd, HANDLE hSharedLib);

const char szExFunNameMakeDialog[] = "_MakeDialog@8";
typedef HWND(__stdcall *t_pfMakeDialog) (HWND hwnd, HINSTANCE shared_DLL);

/**
 * Close plugin's window
 * @return TRUE if window was destroyed with success, FALSE otherwise
 */
extern "C" __declspec(dllexport) BOOL __stdcall CloseDialog ();

const char szExFunNameCloseDialog[] = "_CloseDialog@0";
typedef BOOL(__stdcall *t_pfCloseDialog) ();

#endif //_SAMPLE_PLUGIIN_H_VD_
