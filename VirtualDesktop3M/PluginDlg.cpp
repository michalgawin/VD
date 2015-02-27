/**
* @author Micha³ Gawin
*/


#include "PluginDlg.h"
#include "Plugin.h"


BOOL CALLBACK DlgPluginProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hPlug = NULL;
	static HINSTANCE hInstance;
	static INT cxScreen;
	static INT cyScreen;
	static CPlugin* s_Plugin = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
	{
						  s_Plugin = (CPlugin*)lParam;
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

						  SetWindowText(GetDlgItem(hDlg, IDC_PLUGIN_EDIT), s_Plugin->GetFile());

						  return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
					 TCHAR szFullPath[MAX_PATH];
					 memset(szFullPath, 0, sizeof(szFullPath));

					 GetWindowText(GetDlgItem(hDlg, IDC_PLUGIN_EDIT), szFullPath, _countof(szFullPath));
					 if (_tcslen(szFullPath) > 4)
					 {
						 s_Plugin->m_pfCloseDialog();
						 s_Plugin->Unload();
						 s_Plugin->SetFile(szFullPath);
						 s_Plugin->Load();
					 }
					 EndDialog(hDlg, TRUE);
					 return TRUE;
		}
		case IDCANCEL:
		{
						 EndDialog(hDlg, TRUE);
						 return TRUE;
		}
		case IDC_BUTTON_SELPLUGIN:
		{
									 static TCHAR ext_name[MAX_PATH];
									 memset(ext_name, 0, sizeof (ext_name));
									 LoadString(hInstance, IDS_EXT_NAME_DLL, (TCHAR*)ext_name, _countof(ext_name));
									 _tcscat_s(ext_name + _tcslen(ext_name) + 1, _countof(ext_name), TEXT("*.dll"));

									 OPENFILENAME ofn;
									 TCHAR szFile[MAX_PATH];

									 ZeroMemory(&ofn, sizeof(ofn));
									 ofn.lStructSize = sizeof(ofn);
									 ofn.hwndOwner = hDlg;
									 ofn.lpstrFile = szFile;
									 ofn.lpstrFile[0] = TEXT('\0');
									 ofn.nMaxFile = _countof(szFile);
									 ofn.lpstrFilter = ext_name;
									 ofn.nFilterIndex = 1;
									 ofn.lpstrFileTitle = NULL;
									 ofn.nMaxFileTitle = 0;
									 ofn.lpstrInitialDir = NULL;
									 ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

									 if (GetOpenFileName(&ofn))
									 {
										 SetWindowText(GetDlgItem(hDlg, IDC_PLUGIN_EDIT), ofn.lpstrFile);
									 }

									 return TRUE;
		}
		}
	}
	return FALSE;
}
