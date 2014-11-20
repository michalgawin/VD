/**
 * @author Micha� Gawin
 */


#include "Plugin.h"


PluginGUI pluginGUI;


HINSTANCE LoadPlugin (TCHAR* szDll)
{
	return LoadLibrary (szDll);
}


BOOL FreePlugin (HINSTANCE hPlug)
{
	BOOL ret = FALSE;

	if (hPlug != NULL)
	{
		if (FreeLibrary ((HMODULE) hPlug))
		{
			ret = TRUE;
		}
	}

	return ret;
}


VOID* LoadPluginFunc (HINSTANCE hPlug, const char const * szFunc)
{
	VOID* PluginFunc = NULL;

	PluginFunc = (VOID*) GetProcAddress (hPlug, szFunc); 

	return PluginFunc;
}


BOOL GetPlugin (HINSTANCE &hPlug, TCHAR* szPath, t_PluginFunc& PluginOpenDlg, t_PluginClose& PluginCloseDlg)
{
	BOOL status = FALSE;

	if (hPlug != NULL)
	{
		FreePlugin (hPlug);
	}

	hPlug = LoadPlugin (szPath);
	if (hPlug != NULL)
	{
		PluginOpenDlg = (t_PluginFunc) LoadPluginFunc (hPlug, ExFunNameMakeDialog);
		PluginCloseDlg = (t_PluginClose) LoadPluginFunc (hPlug, ExFunNameCloseDialog);

		if (PluginOpenDlg && PluginCloseDlg)
		{
			status = TRUE;
		}
		else
		{
			FreePlugin (hPlug);
			hPlug = NULL;
			PluginOpenDlg = NULL;
			PluginCloseDlg = NULL;
		}
	}

	return status;
}


BOOL CALLBACK DlgPluginProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hPlug = NULL;
	static HINSTANCE hInstance;
	static INT cxScreen;
	static INT cyScreen;

	switch (message)
	{
	case WM_INITDIALOG:
		{
		hInstance = (HINSTANCE) GetWindowLong (hDlg, GWL_HINSTANCE);

		RECT rc;
		GetWindowRect (hDlg, &rc);
		cxScreen = GetSystemMetrics (SM_CXSCREEN);
		cyScreen = GetSystemMetrics (SM_CYSCREEN);

		SetWindowPos (hDlg,
					  NULL,
					  (cxScreen-(rc.right-rc.left)) / 2,
					  (cyScreen-(rc.bottom-rc.top)) / 2,
					  0,
					  0,
					  SWP_NOZORDER | SWP_NOSIZE);

		SetWindowText (GetDlgItem (hDlg, IDC_PLUGIN_EDIT), pluginGUI.szPluginPath);

		return TRUE;
		}
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDOK:
			{
			GetWindowText (GetDlgItem (hDlg, IDC_PLUGIN_EDIT), pluginGUI.szPluginPath, MAX_PATH);
			if (_tcslen (pluginGUI.szPluginPath) > 4)
			{
				if (hPlug != NULL)
				{
					pluginGUI.PluginCloseDlgFun (pluginGUI.hWnd);
				}
				
				GetPlugin (hPlug, pluginGUI.szPluginPath, pluginGUI.PluginOpenDlgFun, pluginGUI.PluginCloseDlgFun);
			}
			EndDialog (hDlg, TRUE);
			return TRUE;
			}
		case IDCANCEL:
			EndDialog (hDlg, TRUE);
			return TRUE;
		case IDC_BUTTON_SELPLUGIN:
			{
			TCHAR ext_name[MAX_PATH];
			memset (ext_name, 0, sizeof (ext_name));
			LoadString (hInstance, IDS_EXT_NAME_DLL, (TCHAR*)ext_name, sizeof (ext_name) / sizeof (TCHAR));
			_tcscat (ext_name+_tcslen(ext_name)+1, TEXT("*.dll"));

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

			if (GetOpenFileName (&ofn)) 
			{
				SetWindowText (GetDlgItem (hDlg, IDC_PLUGIN_EDIT), ofn.lpstrFile);
			}

			return TRUE;
			}
		}
	}
	return FALSE;
}