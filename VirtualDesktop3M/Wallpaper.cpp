/**
 * @author Micha³ Gawin
 */


#include "Wallpaper.h"


BOOL CALLBACK DlgWallpaperProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	static INT cxScreen;
	static INT cyScreen;
	static pWindowsOnDesktop pWOD = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
		{
		pWOD = (pWindowsOnDesktop) lParam;

		GetWindowRect (hDlg, &rc);
		cxScreen = GetSystemMetrics (SM_CXSCREEN);
		cyScreen = GetSystemMetrics (SM_CYSCREEN);

		SetWindowPos (hDlg,
					  NULL,							// ignored by SWP_NOZORDER
					  (cxScreen-(rc.right-rc.left)) / 2,
					  (cyScreen-(rc.bottom-rc.top)) / 2,
					  0,					// ignored by SWP_NOSIZE
					  0,					// ignored by SWP_NOSIZE
					  SWP_NOZORDER | SWP_NOSIZE);

		//WARNING!!! Work only if edit fields in resources are in ascending sequence!
		for (int i = 0; i <= (IDC_WALL_EDIT4-IDC_WALL_EDIT0); i++)
		{
			SetWindowText (GetDlgItem (hDlg, IDC_WALL_EDIT0+i), pWOD[i].szWallpaper);
		}

		return TRUE;
		}
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDOK:
			{
			for (int i = 0; i < DESKTOPS; i++)
			{
				memset (pWOD[i].szWallpaper, 0, MAX_PATH * sizeof (TCHAR));
			}

			//WARNING!!! Work only if edit fields in resources are in ascending sequence!
			for (int i = 0; i <= (IDC_WALL_EDIT4-IDC_WALL_EDIT0); i++)
			{
				GetWindowText (GetDlgItem (hDlg, IDC_WALL_EDIT0+i), pWOD[i].szWallpaper, MAX_PATH);
			}
			
			SystemParametersInfo (SPI_SETDESKWALLPAPER, _tcslen (pWOD[GetCurrentDesktop ()].szWallpaper), pWOD[GetCurrentDesktop ()].szWallpaper, 0);

			EndDialog (hDlg, TRUE);
			return TRUE;
			}
		case IDCANCEL:
			EndDialog (hDlg, TRUE);
			return TRUE;
		case IDC_BUTTON_SEARCH0:
		case IDC_BUTTON_SEARCH1:
		case IDC_BUTTON_SEARCH2:
		case IDC_BUTTON_SEARCH3:
		case IDC_BUTTON_SEARCH4:
			{
			OPENFILENAME ofn;
			char szFile[MAX_PATH];

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Bitmap files\0*.bmp\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName (&ofn)) 
			{
				//WARNING!!! Work only if edit fields and buttons in resources are in ascending sequence!
				SetWindowText (GetDlgItem (hDlg, IDC_WALL_EDIT0 + (LOWORD (wParam) - IDC_BUTTON_SEARCH0)), ofn.lpstrFile);
			}

			return TRUE;
			}
		}
	}
	return FALSE;
}
