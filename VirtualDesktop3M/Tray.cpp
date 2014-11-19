/**
 * @author Micha³ Gawin
 */


#include "Tray.h"

CTray::CTray (HWND hwnd, HINSTANCE hInstance, HICON icon)
{
	TCHAR szAppName[MAX_PATH];
	LoadString (hInstance, IDS_APP_NAME, (TCHAR*) szAppName, sizeof (szAppName));

	TCHAR szAppAuthor[MAX_PATH];
	LoadString (hInstance, IDS_APP_AUTHOR, (TCHAR*) szAppAuthor, sizeof (szAppAuthor));

	memset (&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uCallbackMessage = WM_TRAY_ICON;
	nid.hIcon = icon;
	_tcscpy (nid.szTip, szAppName);
	_tcscpy (nid.szInfo, szAppAuthor);
	_tcscpy (nid.szInfoTitle, szAppName);
}


VOID CTray::Show ()
{
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	Shell_NotifyIcon (NIM_ADD, &nid);
}


VOID CTray::Hide ()
{
	nid.uFlags = 0;
	Shell_NotifyIcon (NIM_DELETE, &nid);
}


VOID CTray::ChangeIcon (HICON icon)
{
	nid.hIcon = icon;
	nid.uFlags = NIF_ICON;
	Shell_NotifyIcon (NIM_MODIFY, &nid);
}
