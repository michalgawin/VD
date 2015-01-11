/**
 * SamplePlugin.cpp : Defines the entry point for the DLL application.
 * @author Micha³ Gawin
 */


#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "SamplePlugin.h"
#include "VirtualDesktop.h"
#include "DesktopMgr\DesktopMgr.h"
#include "WndAttr.h"


static HINSTANCE g_hPlugin = NULL;
static HWND g_hWnd = NULL;
static HINSTANCE g_hInstance = NULL;


VOID SetWindowName(HWND hWnd)
{
	TCHAR szDesktopName[MAX_PATH];
	LoadString(g_hInstance, IDS_P_WINDOW_NAME, (TCHAR*)szDesktopName, sizeof (szDesktopName) / sizeof (TCHAR));

	TCHAR szWindowName[MAX_PATH];
	memset(szWindowName, 0, sizeof (szWindowName));
	_stprintf(szWindowName, szDesktopName, GetCurrentDesktop());

	SetWindowText(hWnd, szWindowName);
}


BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hPlugin = (HINSTANCE)hModule;
    return TRUE;
}


BOOL CALLBACK PluginProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


extern "C" __declspec(dllexport) HWND __stdcall MakeDialog(HWND hwnd, HANDLE hSharedLib)
{
	if (!g_hWnd)
	{
		g_hWnd = CreateDialog(g_hPlugin, MAKEINTRESOURCE(IDD_DIALOG_TRANSPARENT), hwnd, PluginProc);
	}
	UpdateWindow(g_hWnd);

	return g_hWnd;
}


extern "C" __declspec(dllexport) BOOL __stdcall CloseDialog()
{
	BOOL ret = FALSE;

	if (g_hWnd)
	{
		ret = DestroyWindow (g_hWnd);
		g_hWnd = NULL;	//just in case if .exe will try to use that
	}

	return ret;
}


BOOL CALLBACK PluginProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static INT s_cxImage = 0;									// width of bitmap
	static INT s_cyImage = 0;									// height of bitmap
	static HBITMAP s_hBmp[DESKTOPS] = {NULL, NULL, NULL, NULL, NULL};	// obtain image of each desktop
	static HWND s_hButton[DESKTOPS] = {NULL, NULL, NULL, NULL, NULL};	// desktops' buttons
	static WINDOWPOS s_Wp;
	static SCROLLINFO s_Si;


	switch (message)
	{
	case WM_INITDIALOG:
		{
		s_Wp.hwnd = hDlg;
		s_Wp.hwndInsertAfter = HWND_TOPMOST;
		s_Wp.flags = SWP_SHOWWINDOW;

		g_hInstance = (HINSTANCE)GetWindowLong(s_Wp.hwnd, GWL_HINSTANCE);

		s_Wp.cx = GetSystemMetrics(SM_CXSCREEN) / WIDE_RATIO;
		s_Wp.cy = GetSystemMetrics(SM_CYSCREEN) / HIGH_RATIO;

		if (!s_Wp.x || !s_Wp.y)
		{
			HWND hStart = FindWindowEx (NULL, NULL, TEXT("Shell_TrayWnd"), NULL);

			INT h = 30;									// height of Start menu (for XP default size equal 30)
			if (hStart)
			{
				RECT rcStart = {0, 0, 0, 0};
				GetWindowRect (hStart, &rcStart);
				h = rcStart.bottom - rcStart.top;
			}

			s_Wp.x = s_Wp.cx*(WIDE_RATIO - 1) - h;	//minus h do not cover vertical scrollbar
			s_Wp.y = s_Wp.cy*(HIGH_RATIO - 1) - h;	//minus h to do not cover start menu
		}

		SetWindowPos(s_Wp.hwnd,
			s_Wp.hwndInsertAfter,
			s_Wp.x, s_Wp.y,
			s_Wp.cx, s_Wp.cy,
			s_Wp.flags);

		HDC wndDC = GetWindowDC(s_Wp.hwnd);
		s_cxImage = s_Wp.cx;
		s_cyImage = s_Wp.cy / DESKTOPS_ON_PAGE;

		for (unsigned int i = 0; i < DESKTOPS; i++)
		{
			TCHAR szButtonName[MAX_PATH];
			memset (szButtonName, 0, sizeof (szButtonName));
			_stprintf (szButtonName, TEXT("#%d"), i);

			s_hButton[i] = CreateWindow (TEXT("button"), szButtonName,
										WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_BORDER,
										0, s_cyImage*i, s_Wp.cx, s_cyImage,
										s_Wp.hwnd, (HMENU)(BUTTON_ID0 + i), g_hInstance, NULL);
			if (s_hButton[i] == NULL)
			{
				DWORD err = GetLastError();

				TCHAR szError[MAX_PATH];
				LoadString (g_hInstance, IDS_P_ERROR, (TCHAR*) szError, sizeof (szError) / sizeof (TCHAR));

				TCHAR szCreateWindowError[MAX_PATH];
				LoadString (g_hInstance, IDS_P_ERR_CREATE_WINDOW, (TCHAR*) szCreateWindowError, sizeof (szCreateWindowError) / sizeof (TCHAR));

				TCHAR szMsg[MAX_PATH];
				memset (szMsg, 0, sizeof (szMsg));
				_stprintf (szMsg, szCreateWindowError, szButtonName, err);

				MessageBox(NULL, szMsg, szError, MB_OK);
			}

			HDC btnDC = GetWindowDC (s_hButton[i]);
			if (s_hBmp[i] == NULL)
			{
				TCHAR szDefDesktopPreview[MAX_PATH];
				LoadString (g_hInstance, IDS_P_DEF_DESKTOP_PREVIEW, (TCHAR*) szDefDesktopPreview, sizeof (szDefDesktopPreview) / sizeof (TCHAR));

				TCHAR szDesktop[MAX_PATH];
				memset (szDesktop, 0, sizeof (szDesktop));
				_stprintf (szDesktop, szDefDesktopPreview, i);

				SIZE size;
				GetTextExtentPoint32 (btnDC, szDesktop, _tcslen(szDesktop), &size);

				HDC memDC = CreateCompatibleDC (btnDC);
				s_hBmp[i] = CreateCompatibleBitmap (btnDC, size.cx, size.cy);

				HDC oldMemDC = (HDC)SelectObject (memDC, s_hBmp[i]);
				TextOut (memDC, 0, 0, szDesktop, _tcslen(szDesktop));
				SelectObject (memDC, oldMemDC);

				DeleteDC (memDC);
				memDC = NULL;
			}
			ReleaseDC (s_hButton[i], btnDC);
			btnDC = NULL;
		}

		ReleaseDC (hDlg, wndDC);
		wndDC = NULL;

		RECT rc;
		GetClientRect(s_Wp.hwnd, &rc);
		
		s_Si.cbSize = sizeof(s_Si);
		s_Si.fMask = SIF_RANGE | SIF_PAGE;
		s_Si.nMin = 0;
		s_Si.nMax = s_cyImage * max(DESKTOPS, DESKTOPS_ON_PAGE);
		s_Si.nPage = rc.bottom - rc.top;
		SetScrollInfo(s_Wp.hwnd, SB_VERT, &s_Si, TRUE);
		
		SetTimer(s_Wp.hwnd, ID_TIMER, 250, NULL);

		if (MakeWindowTransparent(s_Wp.hwnd, 120))	//it should be enough to read documents through it
		{
			return FALSE;
		}

		return TRUE;
		}
	case WM_TIMER:
		InvalidateRect (s_hButton[GetCurrentDesktop ()], NULL, FALSE);
		return TRUE;
	case WM_VSCROLL:
		{
		s_Si.cbSize = sizeof(s_Si);
		s_Si.fMask = SIF_ALL;
		GetScrollInfo (s_Wp.hwnd, SB_VERT, &s_Si);
		INT iVertPos = s_Si.nPos;

		switch (LOWORD (wParam))
		{
		case SB_TOP:
			s_Si.nPos = s_Si.nMin;
			break;
		case SB_BOTTOM:
			s_Si.nPos = s_Si.nMax;
			break;
		case SB_LINEUP:
			s_Si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			s_Si.nPos += 1;
			break;
		case SB_PAGEUP:
			s_Si.nPos -= s_Si.nPage;
			break;
		case SB_PAGEDOWN:
			s_Si.nPos += s_Si.nPage;
			break;
		case SB_THUMBTRACK:
			s_Si.nPos = s_Si.nTrackPos;
			break;
		default:
			break;
		}

		s_Si.fMask = SIF_POS;
		SetScrollInfo(s_Wp.hwnd, SB_VERT, &s_Si, TRUE);
		GetScrollInfo(s_Wp.hwnd, SB_VERT, &s_Si);

		if (s_Si.nPos != iVertPos)
		{                    
			ScrollWindow(s_Wp.hwnd, 0, iVertPos - s_Si.nPos, NULL, NULL);
			UpdateWindow(s_Wp.hwnd);
		}
		}
		return TRUE;
	case WM_COMMAND:
		{
		UINT id = LOWORD(wParam);

		if (id == IDCANCEL)
		{
			CloseDialog();
		}
		else if ((id >= BUTTON_ID0) && (id < (BUTTON_ID0+DESKTOPS)))
		{
			BOOL ret = FALSE;
			INT currDskID = GetCurrentDesktop ();
			INT dskID = wParam - 10000;

			if (currDskID != dskID)
			{
				//Save screen before switch desktop
				HDC dcDesktop = GetWindowDC (NULL);

				HDC memDC = CreateCompatibleDC (dcDesktop);
				DeleteObject (s_hBmp[currDskID]);
				s_hBmp[currDskID] = NULL;
				s_hBmp[currDskID] = CreateCompatibleBitmap (dcDesktop, s_cxImage, s_cyImage);

				HDC oldMemDC = (HDC)SelectObject (memDC, s_hBmp[currDskID]);
				StretchBlt (memDC, 0, 0, s_cxImage, s_cyImage, dcDesktop, 0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN), SRCCOPY);
				SelectObject (memDC, oldMemDC);
				
				DeleteDC (memDC);
				memDC = NULL;

				ReleaseDC (NULL, dcDesktop);
				dcDesktop = NULL;

				ret = ChangeDesktop (dskID);
			}

			return ret;
		}
		}
		return TRUE;
	case WM_DRAWITEM:
		{
		LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT) lParam;

		INT bmpID = abs((int)(pdis->CtlID - BUTTON_ID0));

		if (bmpID < DESKTOPS)
		{
			static INT lastDskID = -1; // to check whether last time was another desktop
			INT currDskID = GetCurrentDesktop ();
			BITMAP bmp;

			INT status = GetObject (s_hBmp[bmpID], sizeof (BITMAP), &bmp);
		
			if (!status)
			{
				TCHAR szGetObjectError[MAX_PATH];
				LoadString(g_hInstance, IDS_P_ERR_GET_OBJ, (TCHAR*)szGetObjectError, sizeof (szGetObjectError) / sizeof (TCHAR));

				TCHAR szError[MAX_PATH];
				LoadString (g_hInstance, IDS_P_ERROR, (TCHAR*) szError, sizeof (szError) / sizeof (TCHAR));

				TCHAR szMsg[MAX_PATH];
				memset (szMsg, 0, sizeof (szMsg));
				_stprintf (szMsg, szGetObjectError, GetLastError(), status, bmpID, bmp.bmWidth, bmp.bmHeight);
				MessageBox (NULL, szMsg, szError, MB_OK);
			}
			else
			{
				HDC memDC;
				HDC oldMemDC;

				if (bmpID == currDskID) //Paint Current Desktop
				{
					HDC dcDesktop = GetWindowDC (NULL);
					memDC = CreateCompatibleDC (dcDesktop);
					DeleteObject (s_hBmp[currDskID]);
					s_hBmp[currDskID] = NULL;
					s_hBmp[currDskID] = CreateCompatibleBitmap (dcDesktop, s_cxImage, s_cyImage);
					oldMemDC = (HDC)SelectObject (memDC, s_hBmp[bmpID]);

					StretchBlt (memDC, 0, 0, s_cxImage, s_cyImage, dcDesktop, 0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN), SRCCOPY);

					ReleaseDC (NULL, dcDesktop);
					dcDesktop = NULL;
				}
				else
				{
					memDC = CreateCompatibleDC (pdis->hDC);
					oldMemDC = (HDC)SelectObject (memDC, s_hBmp[bmpID]);
				}
		
				StretchBlt (pdis->hDC, 0, 0, s_cxImage, s_cyImage, memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

				SelectObject (memDC, oldMemDC);
				DeleteDC (memDC);
				memDC = NULL;

				ReleaseDC (s_hButton[bmpID], pdis->hDC);
			}

			if (currDskID != lastDskID)
			{
				SetWindowName(s_Wp.hwnd);
				lastDskID = currDskID;
			}
		}
		
		return TRUE;
		}
	case WM_DESTROY:
		return TRUE;
	}
	return FALSE;
}
