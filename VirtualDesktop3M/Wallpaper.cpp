/**
* @author Micha³ Gawin
*/


#include "Wallpaper.h"


BOOL CALLBACK DlgWallpaperProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	static INT cxScreen;
	static INT cyScreen;
	static pWindowsOnDesktop pWOD = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
	{
						  pWOD = (pWindowsOnDesktop)lParam;

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

						  //WARNING!!! Work only if edit fields in resources are in ascending sequence!
						  for (int i = 0; i <= (IDC_WALL_EDIT4 - IDC_WALL_EDIT0); i++)
						  {
							  SetWindowText(GetDlgItem(hDlg, IDC_WALL_EDIT0 + i), pWOD[i].szWallpaper);
						  }

						  return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
					 for (int i = 0; i < DESKTOPS; i++)
					 {
						 memset(pWOD[i].szWallpaper, 0, MAX_PATH * sizeof (TCHAR));
					 }

					 //WARNING!!! Work only if edit fields in resources are in ascending sequence!
					 for (int i = 0; i <= (IDC_WALL_EDIT4 - IDC_WALL_EDIT0); i++)
					 {
						 GetWindowText(GetDlgItem(hDlg, IDC_WALL_EDIT0 + i), pWOD[i].szWallpaper, MAX_PATH);
					 }

					 SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen(pWOD[GetCurrentDesktop()].szWallpaper), pWOD[GetCurrentDesktop()].szWallpaper, 0);

					 EndDialog(hDlg, TRUE);
					 return TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
		case IDC_BUTTON_SEARCH0:
		case IDC_BUTTON_SEARCH1:
		case IDC_BUTTON_SEARCH2:
		case IDC_BUTTON_SEARCH3:
		case IDC_BUTTON_SEARCH4:
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
									   //WARNING!!! Work only if edit fields and buttons in resources are in ascending sequence!
									   SetWindowText(GetDlgItem(hDlg, IDC_WALL_EDIT0 + (LOWORD(wParam) - IDC_BUTTON_SEARCH0)), ofn.lpstrFile);
								   }

								   return TRUE;
		}
		}
	}
	return FALSE;
}
