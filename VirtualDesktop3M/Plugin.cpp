/**
* @author Micha³ Gawin
*/


#include "Plugin.h"


CPlugin g_PluginUI;

CPlugin::CPlugin()
{
	m_hLib = NULL;
	m_szFullPath = NULL;
	m_pfMakeDialog = NULL;
	m_pfCloseDialog = NULL;
}

CPlugin::~CPlugin()
{
	if (m_hLib) FreeLibrary((HMODULE)m_hLib);
	m_hLib = NULL;

	if (m_szFullPath) delete[] m_szFullPath;
	m_szFullPath = NULL;
}

BOOL CPlugin::LoadAll(TCHAR* szFullPath)
{
	BOOL ret = FALSE;

	if (szFullPath)
	{
		Unload();

		if (SetFullPath(szFullPath))
		{
			if (Load())
			{
				if (GetFunc(ExFunNameMakeDialog, (VOID**)&m_pfMakeDialog) && GetFunc(ExFunNameCloseDialog, (VOID**)&m_pfCloseDialog))
				{
					ret = TRUE;
				}
			}
		}
	}

	return ret;
}

TCHAR* CPlugin::SetFullPath(TCHAR* szFullPath)
{
	TCHAR* temp = m_szFullPath;

	if (szFullPath)
	{
		INT len = _tcslen(szFullPath) + 1;
		m_szFullPath = new TCHAR[len];
		memset(m_szFullPath, 0, sizeof(TCHAR)* len);
		_tcscpy(m_szFullPath, szFullPath);

		if (temp) delete[] temp;
		temp = NULL;
	}

	return m_szFullPath;
}

BOOL CPlugin::Load()
{
	BOOL ret = FALSE;

	if (m_szFullPath)
	{
		if ((m_hLib = LoadLibrary(m_szFullPath))) ret = TRUE;
	}

	return ret;
}

VOID CPlugin::Unload()
{
	m_pfMakeDialog = NULL;
	if (m_pfCloseDialog) m_pfCloseDialog();
	m_pfCloseDialog = NULL;

	if (m_hLib) FreeLibrary((HMODULE)m_hLib);
	m_hLib = NULL;
}

BOOL CPlugin::GetFunc(const char* szFuncName, VOID** pFun)
{
	if (m_hLib) *pFun = (VOID*)GetProcAddress(m_hLib, szFuncName);

	return (*pFun != NULL);
}


BOOL CALLBACK DlgPluginProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hPlug = NULL;
	static HINSTANCE hInstance;
	static INT cxScreen;
	static INT cyScreen;

	switch (message)
	{
	case WM_INITDIALOG:
	{
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

						  SetWindowText(GetDlgItem(hDlg, IDC_PLUGIN_EDIT), g_PluginUI.GetFullPath());

						  return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
					 TCHAR szFullPath[MAX_PATH];
					 memset(szFullPath, 0, sizeof(TCHAR)*MAX_PATH);

					 GetWindowText(GetDlgItem(hDlg, IDC_PLUGIN_EDIT), szFullPath, MAX_PATH);
					 if (_tcslen(szFullPath) > 4)
					 {
						 g_PluginUI.m_pfCloseDialog();
						 g_PluginUI.LoadAll(szFullPath);
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
									 TCHAR ext_name[MAX_PATH];
									 memset(ext_name, 0, sizeof (ext_name));
									 LoadString(hInstance, IDS_EXT_NAME_DLL, (TCHAR*)ext_name, sizeof (ext_name) / sizeof (TCHAR));
									 _tcscat(ext_name + _tcslen(ext_name) + 1, TEXT("*.dll"));

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
										 SetWindowText(GetDlgItem(hDlg, IDC_PLUGIN_EDIT), ofn.lpstrFile);
									 }

									 return TRUE;
		}
		}
	}
	return FALSE;
}
