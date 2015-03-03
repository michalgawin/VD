/**
* SamplePlugin.cpp : Defines the entry point for the DLL application.
* @author Micha³ Gawin
*/


#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "SamplePlugin.h"
#include "DesktopButton.h"
#include "VirtualDesktop.h"
#include "Sys.h"
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


BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	g_hPlugin = (HINSTANCE)hModule;
	return TRUE;
}


BOOL CALLBACK PluginProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


extern "C" __declspec(dllexport) HWND __stdcall MakeDialog(HWND hwnd, HANDLE hSharedLib)
{
	if (!g_hWnd)
	{
		g_hWnd = CreateDialog(g_hPlugin, MAKEINTRESOURCE(IDD_DIALOG_TRANSPARENT), hwnd, PluginProc);
		SetWindowName(g_hWnd);
	}
	UpdateWindow(g_hWnd);

	return g_hWnd;
}


extern "C" __declspec(dllexport) BOOL __stdcall CloseDialog()
{
	BOOL ret = FALSE;

	if (g_hWnd)
	{
		ret = DestroyWindow(g_hWnd);
		g_hWnd = NULL;	//just in case if .exe will try to use that
	}

	return ret;
}


BOOL CALLBACK PluginProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static INT s_nDesktops = DESKTOPS+1;
	static std::vector<CDesktopButton> s_vButton(DESKTOPS, NULL);
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
							  HWND hStart = FindWindowEx(NULL, NULL, TEXT("Shell_TrayWnd"), NULL);

							  INT h = START_MENU_HEIGHT;
							  if (hStart)
							  {
								  RECT rcStart = { 0, 0, 0, 0 };
								  GetWindowRect(hStart, &rcStart);
								  h = rcStart.bottom - rcStart.top;
							  }

							  s_Wp.x = s_Wp.cx*(WIDE_RATIO - 1) - h;
							  s_Wp.y = s_Wp.cy*(HIGH_RATIO - 1) - h;
						  }

						  SetWindowPos(s_Wp.hwnd,
							  s_Wp.hwndInsertAfter,
							  s_Wp.x, s_Wp.y,
							  s_Wp.cx, s_Wp.cy,
							  s_Wp.flags);

						  INT cxImage = s_Wp.cx;
						  INT cyImage = s_Wp.cy / DESKTOPS_ON_PAGE;

						  for (unsigned int i = 0; i < s_nDesktops; i++)
						  {
							  TCHAR szButtonName[MAX_PATH];
							  memset(szButtonName, 0, sizeof (szButtonName));
							  _stprintf(szButtonName, TEXT("#%d"), i);

							  HWND hBtn = CreateWindow(TEXT("button"), szButtonName,
								  WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_BORDER,
								  0, cyImage*i, cxImage, cyImage,
								  s_Wp.hwnd, (HMENU)(BUTTON_ID0 + i), g_hInstance, NULL);

							  if (hBtn == NULL)
							  {
								  ShowErr(g_hInstance, IDS_P_ERROR, IDS_P_ERR_CREATE_WINDOW, szButtonName, GetLastError());
							  }

							  if ((i == (s_nDesktops - 1)) && (s_vButton.size() < s_nDesktops)) s_vButton.push_back(hBtn);
							  else s_vButton[i] = hBtn;

							  if (s_vButton[i].GetBitmap() == NULL)
							  {
								  TCHAR szDesktop[MAX_PATH];
								  memset(szDesktop, 0, sizeof(szDesktop));

								  if (i == (s_nDesktops - 1))
								  {
									  _stprintf(szDesktop, TEXT("<+>"));
								  }
								  else
								  {
									  TCHAR szDefDesktopPreview[MAX_PATH];
									  LoadString(g_hInstance, IDS_P_DEF_DESKTOP_PREVIEW, (TCHAR*)szDefDesktopPreview, sizeof (szDefDesktopPreview) / sizeof (TCHAR));

									  _stprintf(szDesktop, szDefDesktopPreview, i);
								  }

								  s_vButton[i].UpdateImage(szDesktop);
							  }
						  }

						  RECT rc;
						  GetClientRect(s_Wp.hwnd, &rc);

						  s_Si.cbSize = sizeof(s_Si);
						  s_Si.fMask = SIF_RANGE | SIF_PAGE;
						  s_Si.nMin = 0;
						  s_Si.nMax = cyImage * max(s_nDesktops, DESKTOPS_ON_PAGE);
						  s_Si.nPage = rc.bottom - rc.top;
						  SetScrollInfo(s_Wp.hwnd, SB_VERT, &s_Si, TRUE);

						  SetTimer(s_Wp.hwnd, ID_TIMER, 250, NULL);

						  if (!MakeWindowTransparent(s_Wp.hwnd, 120))	//it should be enough to read documents through it
						  {
							  return FALSE;
						  }

						  return TRUE;
	}
	case WM_TIMER:
	{
					 InvalidateRect(s_vButton[GetCurrentDesktop()].GetButton(), NULL, FALSE);
					 return TRUE;
	}
	case WM_VSCROLL:
	{
					   s_Si.cbSize = sizeof(s_Si);
					   s_Si.fMask = SIF_ALL;
					   GetScrollInfo(s_Wp.hwnd, SB_VERT, &s_Si);
					   INT iVertPos = s_Si.nPos;

					   switch (LOWORD(wParam))
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
					   return TRUE;
	}
	case WM_COMMAND:
	{
					   UINT id = LOWORD(wParam);

					   if (id == IDCANCEL)
					   {
						   CloseDialog();
					   }
					   else if ((id >= BUTTON_ID0) && (id < (BUTTON_ID0 + s_vButton.size())))
					   {
						   BOOL ret = FALSE;
						   INT currDskID = GetCurrentDesktop();
						   INT dskID = wParam - BUTTON_ID0;

						   if (currDskID != dskID)
						   {
							   s_vButton[currDskID].UpdateImage();
							   s_vButton[currDskID].Redraw();

							   if (dskID == (s_vButton.size() - 1))
							   {
								   s_vButton[dskID].RemoveBitmap();
								   s_nDesktops++;
							   }
							   ret = ChangeDesktop(dskID);
							   if (!ret) s_nDesktops--;
						   }

						   return ret;
					   }
					   return TRUE;
	}
	case WM_DRAWITEM:
	{
						LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;

						INT iBtnID = abs((int)(pdis->CtlID - BUTTON_ID0));

						if (iBtnID < s_vButton.size())
						{
							static INT lastDskID = -1; // to check whether last time was another desktop
							INT currDskID = GetCurrentDesktop();
							BITMAP bmp;

							INT status = GetObject(s_vButton[iBtnID].GetBitmap(), sizeof (BITMAP), &bmp);

							if (!status)
							{
								ShowErr(g_hInstance, IDS_P_ERROR, IDS_P_ERR_GET_OBJ, GetLastError(), status, iBtnID, bmp.bmWidth, bmp.bmHeight);
							}
							else
							{
								if (iBtnID == currDskID) s_vButton[currDskID].UpdateImage();
								s_vButton[iBtnID].Redraw();
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
