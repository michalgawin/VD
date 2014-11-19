/**
 * @author Micha³ Gawin
 */


#include "About.h"


BOOL CALLBACK DlgAboutProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static INT cxScreen;
	static INT cyScreen;

	switch (message)
	{
	case WM_INITDIALOG:
		{
		RECT rc;

		GetWindowRect (hDlg, &rc);
		cxScreen = GetSystemMetrics (SM_CXSCREEN);
		cyScreen = GetSystemMetrics (SM_CYSCREEN);

		SetWindowPos (hDlg,
					  NULL,							// ignored by SWP_NOZORDER
					  (cxScreen-(rc.right-rc.left)) / 2,
					  (cyScreen-(rc.bottom-rc.top)) / 2,
					  0,							// ignored by SWP_NOSIZE
					  0,							// ignored by SWP_NOSIZE
					  SWP_NOZORDER | SWP_NOSIZE);

		AnimateWindow (hDlg, 500, AW_VER_POSITIVE);
		if (MakeWindowTransparent (hDlg, 255))
		{
			return FALSE;
		}
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDOK:
		case IDCANCEL:
			SlowHideWindow (hDlg, 255, 255);
			EndDialog (hDlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
