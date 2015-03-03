/**
* @author Micha³ Gawin
*/


#include "Tray.h"

CTray::CTray(HWND hwnd, HINSTANCE hInstance)
{
	TCHAR szAppName[MAX_PATH];
	LoadString(hInstance, IDS_APP_NAME, (TCHAR*)szAppName, sizeof(szAppName) / sizeof(TCHAR));

	TCHAR szAppAuthor[MAX_PATH];
	LoadString(hInstance, IDS_APP_AUTHOR, (TCHAR*)szAppAuthor, sizeof(szAppAuthor) / sizeof(TCHAR));

	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uCallbackMessage = WM_TRAY_ICON;
	_tcscpy(nid.szTip, szAppName);
	_tcscpy(nid.szInfo, szAppAuthor);
	_tcscpy(nid.szInfoTitle, szAppName);
}


VOID CTray::Show()
{
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	Shell_NotifyIcon(NIM_ADD, &nid);
}


VOID CTray::Hide()
{
	nid.uFlags = 0;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}


VOID CTray::ChangeIcon(TCHAR* szFormat, ...)
{
	if (nid.hIcon)
	{
		DestroyIcon(nid.hIcon);
		nid.hIcon = 0;
	}

	INT cxSmallIcon = GetSystemMetrics(SM_CXSMICON);
	INT cySmallIcon = GetSystemMetrics(SM_CYSMICON);

	HDC appDC = GetWindowDC(nid.hWnd);
	HDC memDC = CreateCompatibleDC(appDC);
	
	HBITMAP hBmpMask = CreateCompatibleBitmap(appDC, cxSmallIcon, cySmallIcon);

	HBITMAP hBmp = CreateCompatibleBitmap(appDC, cxSmallIcon, cySmallIcon);
	HDC oldMemDC = (HDC)SelectObject(memDC, hBmp);
	COLORREF crTxtColor = SetTextColor(memDC, RGB(255, 0, 0));

	va_list pArgList;
	va_start(pArgList, szFormat);

	TCHAR szDesktop[4];
	memset(szDesktop, 0, sizeof(szDesktop));
	_vsntprintf(szDesktop, _countof(szDesktop), szFormat, pArgList);

	va_end(pArgList);

	SIZE size;
	GetTextExtentPoint32(appDC, szDesktop, _tcslen(szDesktop), &size);
	TextOut(memDC, 0, 0, szDesktop, _tcslen(szDesktop));

	SetTextColor(memDC, crTxtColor);
	StretchBlt(memDC, 0, 0, cxSmallIcon, cySmallIcon, memDC, 0, 0, size.cx, size.cy, SRCCOPY);
	SelectObject(memDC, oldMemDC);
	
	ICONINFO ii;
	ii.fIcon = TRUE;
	ii.hbmMask = hBmpMask;
	ii.hbmColor = hBmp;
	nid.hIcon = CreateIconIndirect(&ii);
	nid.uFlags = NIF_ICON;
	Shell_NotifyIcon(NIM_MODIFY, &nid);

	DeleteDC(memDC);
	DeleteObject(hBmpMask);
	DeleteObject(hBmp);
}
