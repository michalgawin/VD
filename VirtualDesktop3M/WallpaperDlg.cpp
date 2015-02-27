/**
* @author Micha³ Gawin
*/


#include "WallpaperDlg.h"
#include "DesktopsManager.h"


BOOL CALLBACK DlgWallpaperProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	static INT cxScreen;
	static INT cyScreen;
	static pCDesktop s_pDsk = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
	{
						  s_pDsk = (pCDesktop)lParam;

						  hInstance = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);

						  RECT rc;
						  GetWindowRect(hDlg, &rc);
						  cxScreen = GetSystemMetrics(SM_CXSCREEN);
						  cyScreen = GetSystemMetrics(SM_CYSCREEN);

						  SetWindowPos(hDlg,
							  NULL,
							  (cxScreen - (rc.right - rc.left)) / 2,
							  (cyScreen - (rc.bottom - rc.top)) / 2,
							  0,
							  0,
							  SWP_NOZORDER | SWP_NOSIZE);

						  SetWindowText(GetDlgItem(hDlg, IDC_WALL_EDIT0), s_pDsk->GetWallpaper());

						  return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
					 BOOL bRet = FALSE;
					 TCHAR* szWallpaper2 = new TCHAR[MAX_PATH];
					 GetWindowText(GetDlgItem(hDlg, IDC_WALL_EDIT0), szWallpaper2, MAX_PATH);

					 if (_tcscmp(s_pDsk->GetWallpaper(), szWallpaper2))
					 {
						 s_pDsk->SetWallpaper(szWallpaper2);
						 bRet = TRUE;
					 }

					 EndDialog(hDlg, bRet);
					 return TRUE;
		}
		case IDCANCEL:
		{
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		case IDC_BUTTON_SEARCH0:
		{
								   TCHAR ext_name[MAX_PATH];
								   memset(ext_name, 0, sizeof (ext_name));
								   LoadString(hInstance, IDS_EXT_NAME_BMP, (TCHAR*)ext_name, sizeof(ext_name) / sizeof(TCHAR));
								   _tcscat(ext_name + _tcslen(ext_name) + 1, TEXT("*.bmp"));

								   OPENFILENAME ofn;
								   TCHAR szFile[MAX_PATH];

								   ZeroMemory(&ofn, sizeof(ofn));
								   ofn.lStructSize = sizeof(ofn);
								   ofn.hwndOwner = hDlg;
								   ofn.lpstrFile = szFile;
								   ofn.lpstrFile[0] = TEXT('\0');
								   ofn.nMaxFile = sizeof(szFile) / sizeof(TCHAR);
								   ofn.lpstrFilter = ext_name;
								   ofn.nFilterIndex = 1;
								   ofn.lpstrFileTitle = NULL;
								   ofn.nMaxFileTitle = 0;
								   ofn.lpstrInitialDir = NULL;
								   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

								   if (GetOpenFileName(&ofn))
								   {
									   SetWindowText(GetDlgItem(hDlg, IDC_WALL_EDIT0), ofn.lpstrFile);
								   }

								   return TRUE;
		}
		}
	}
	return FALSE;
}
