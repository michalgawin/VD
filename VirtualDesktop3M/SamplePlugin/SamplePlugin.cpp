/**
 * SamplePlugin.cpp : Defines the entry point for the DLL application.
 * @author Micha³ Gawin
 */


#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "SamplePlugin.h"
#include "..\VirtualDesktop.h"
#include "..\DesktopMgr\DesktopMgr.h"
#include "..\WndAttr.h"

#define ID_TIMER    1

#define WIDE_RATIO	8
#define HIGH_RATIO	3

static HINSTANCE hPlugin = NULL;


BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	hPlugin = (HINSTANCE) hModule;
    return TRUE;
}


BOOL CALLBACK PluginProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


extern "C" __declspec(dllexport) HWND __stdcall MakeDialog (HWND hwnd, HANDLE hDll)
{
	return CreateDialog (hPlugin, MAKEINTRESOURCE (IDD_DIALOG_TRANSPARENT), hwnd, PluginProc);
}


extern "C" __declspec(dllexport) VOID __stdcall CloseDialog (HWND hDialog)
{
	if (hDialog)
	{
		DestroyWindow (hDialog);
	}
}


BOOL CALLBACK PluginProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static INT cxImage = 0;									// width of bitmap
	static INT cyImage = 0;									// height of bitmap
	static INT const iButtonID[] = {10000, 10001, 10002, 10003, 10004};
	static HBITMAP hBmp[DESKTOPS] = {NULL, NULL, NULL, NULL, NULL};	// obtain image of each desktop
	static HWND hButton[DESKTOPS] = {NULL, NULL, NULL, NULL, NULL};	// desktops' buttons
	static WINDOWPOS properties;
	static HINSTANCE hInstance;
	static SCROLLINFO si;


	switch (message)
	{
	case WM_INITDIALOG:
		{
		properties.hwnd = hDlg;
		properties.hwndInsertAfter = HWND_TOPMOST;
		properties.flags = SWP_SHOWWINDOW;

		hInstance = (HINSTANCE) GetWindowLong (properties.hwnd, GWL_HINSTANCE);

		properties.cx = GetSystemMetrics (SM_CXSCREEN) / WIDE_RATIO;
		properties.cy = GetSystemMetrics (SM_CYSCREEN) / HIGH_RATIO;

		if (!properties.x || !properties.y)
		{
			HWND hStart = FindWindowEx (NULL, NULL, TEXT("Shell_TrayWnd"), NULL);

			INT h = 30;									// height of Start menu (for XP default size equal 30)
			if (hStart)
			{
				RECT rcStart = {0, 0, 0, 0};
				GetWindowRect (hStart, &rcStart);
				h = rcStart.bottom - rcStart.top;
			}

			properties.x = properties.cx*(WIDE_RATIO-1) - h;	//minus h do not cover vertical scrollbar
			properties.y = properties.cy*(HIGH_RATIO-1) - h;	//minus h to do not cover start menu
		}

		SetWindowPos (properties.hwnd,
					  properties.hwndInsertAfter,
					  properties.x, properties.y,
					  properties.cx, properties.cy,
					  properties.flags);

		HDC wndDC = GetWindowDC (properties.hwnd);
		cxImage = properties.cx;
		cyImage = properties.cy / DESKTOPS;

		for (unsigned int i = 0; i < DESKTOPS; i++)
		{
			TCHAR szButtonName[MAX_PATH];
			memset (szButtonName, 0, MAX_PATH);
			_stprintf (szButtonName, TEXT("#%d"), i);

			hButton[i] = CreateWindow (TEXT("button"), TEXT (szButtonName),
										WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_BORDER,
										0, cyImage*i, properties.cx, cyImage,
										properties.hwnd, (HMENU) iButtonID[i], hInstance, NULL);
			if (hButton[i] == NULL)
			{
				DWORD err = GetLastError();

				TCHAR szError[MAX_PATH];
				LoadString (hInstance, IDS_ERROR, (TCHAR*) szError, sizeof (szError));

				TCHAR szCreateWindowError[MAX_PATH];
				LoadString (hInstance, IDS_CREATE_WINDOW_ERR, (TCHAR*) szCreateWindowError, sizeof (szCreateWindowError));

				TCHAR szMsg[MAX_PATH];
				memset (szMsg, 0, MAX_PATH);
				_stprintf (szMsg, TEXT(szCreateWindowError), TEXT(szButtonName), err);

				MessageBox(NULL, TEXT(szMsg), TEXT(szError), MB_OK);
			}

			HDC btnDC = GetWindowDC (hButton[i]);
			if (hBmp[i] == NULL)
			{
				TCHAR szDefDesktopPreview[MAX_PATH];
				LoadString (hInstance, IDS_DEF_DESKTOP_PREVIEW, (TCHAR*) szDefDesktopPreview, sizeof (szDefDesktopPreview));

				TCHAR szDesktop[MAX_PATH];
				memset (szDesktop, 0, sizeof (szDesktop));
				_stprintf (szDesktop, TEXT(szDefDesktopPreview), i);

				SIZE size;
				GetTextExtentPoint32 (btnDC, TEXT(szDesktop), _tcslen(TEXT(szDesktop)), &size);

				HDC memDC = CreateCompatibleDC (btnDC);
				hBmp[i] = CreateCompatibleBitmap (btnDC, size.cx, size.cy);

				HDC oldMemDC = (HDC)SelectObject (memDC, hBmp[i]);
				TextOut (memDC, 0, 0, TEXT(szDesktop), _tcslen(TEXT(szDesktop)));
				SelectObject (memDC, oldMemDC);

				DeleteDC (memDC);
				memDC = NULL;
			}
			ReleaseDC (hButton[i], btnDC);
			btnDC = NULL;
		}

		ReleaseDC (hDlg, wndDC);
		wndDC = NULL;

		RECT rc;
		GetClientRect(properties.hwnd, &rc);
		
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = properties.cy;
		si.nPage = rc.bottom - rc.top;
		SetScrollInfo (properties.hwnd, SB_VERT, &si, TRUE);
		
		SetTimer (properties.hwnd, ID_TIMER, 250, NULL);

		if (MakeWindowTransparent (properties.hwnd, 120))	//it should be enough to read documents through it
		{
			return FALSE;
		}
		
		return TRUE;
		}
	case WM_TIMER:
		InvalidateRect (hButton[GetCurrentDesktop ()], NULL, FALSE);
		return TRUE;
	case WM_VSCROLL:
		{
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo (properties.hwnd, SB_VERT, &si);
		INT iVertPos = si.nPos;

		switch (LOWORD (wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			si.nPos += 1;
			break;
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo (properties.hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo (properties.hwnd, SB_VERT, &si);

		if (si.nPos != iVertPos)
		{                    
			ScrollWindow (properties.hwnd, 0, iVertPos - si.nPos, NULL, NULL) ;
			UpdateWindow (properties.hwnd) ;
		}
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			DestroyWindow (properties.hwnd);
			break;
		case IDOK:
			break;
		case 10000:	//Desktop0
		case 10001:
		case 10002:
		case 10003:
		case 10004:	//Desktop4
			{
				INT currDskID = GetCurrentDesktop ();
				INT dskID = wParam - 10000;

				if (currDskID == dskID)	//do not change desktop
				{
					break;
				}
				
				//Save screen before change desktop
				HDC dcDesktop = GetWindowDC (NULL);

				HDC memDC = CreateCompatibleDC (dcDesktop);
				DeleteObject (hBmp[currDskID]);
				hBmp[currDskID] = NULL;
				hBmp[currDskID] = CreateCompatibleBitmap (dcDesktop, cxImage, cyImage);

				HDC oldMemDC = (HDC)SelectObject (memDC, hBmp[currDskID]);
				StretchBlt (memDC, 0, 0, cxImage, cyImage, dcDesktop, 0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN), SRCCOPY);
				SelectObject (memDC, oldMemDC);
				
				DeleteDC (memDC);
				memDC = NULL;

				ReleaseDC (NULL, dcDesktop);
				dcDesktop = NULL;

				ChangeDesktop (dskID);
			}
			break;
		}
		return TRUE;
	case WM_DRAWITEM:
		{
		LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT) lParam;

		INT bmpID = abs((int)(pdis->CtlID - iButtonID[0]));
		if (bmpID < DESKTOPS)
		{
			INT currDskID = GetCurrentDesktop ();
			BITMAP bmp;

			INT status = GetObject (hBmp[bmpID], sizeof (BITMAP), &bmp);
		
			if (!status)
			{
				TCHAR szGetObjectError[MAX_PATH];
				LoadString (hInstance, IDS_GET_OBJ_ERR, (TCHAR*) szGetObjectError, sizeof (szGetObjectError));

				TCHAR szError[MAX_PATH];
				LoadString (hInstance, IDS_ERROR, (TCHAR*) szError, sizeof (szError));

				TCHAR szMsg[MAX_PATH];
				memset (szMsg, 0, MAX_PATH);
				_stprintf (szMsg, szGetObjectError, GetLastError(), status, bmpID, bmp.bmWidth, bmp.bmHeight);
				MessageBox (NULL, TEXT(szMsg), TEXT(szError), MB_OK);
			}
			else
			{
				HDC memDC;
				HDC oldMemDC;

				if (bmpID == currDskID) //Paint Current Desktop
				{
					HDC dcDesktop = GetWindowDC (NULL);
					memDC = CreateCompatibleDC (dcDesktop);
					DeleteObject (hBmp[currDskID]);
					hBmp[currDskID] = NULL;
					hBmp[currDskID] = CreateCompatibleBitmap (dcDesktop, cxImage, cyImage);
					oldMemDC = (HDC)SelectObject (memDC, hBmp[bmpID]);

					StretchBlt (memDC, 0, 0, cxImage, cyImage, dcDesktop, 0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN), SRCCOPY);

					ReleaseDC (NULL, dcDesktop);
					dcDesktop = NULL;
				}
				else
				{
					memDC = CreateCompatibleDC (pdis->hDC);
					oldMemDC = (HDC)SelectObject (memDC, hBmp[bmpID]);
				}
		
				StretchBlt (pdis->hDC, 0, 0, cxImage, cyImage, memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);//MERGECOPY

				SelectObject (memDC, oldMemDC);
				DeleteDC (memDC);
				memDC = NULL;

				ReleaseDC (hButton[bmpID], pdis->hDC);
			}
		}
		
		return TRUE;
		}
	case WM_DESTROY:
		return TRUE;
	}
	return FALSE;
}
