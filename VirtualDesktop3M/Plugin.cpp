/**
* @author Micha³ Gawin
*/


#include "Plugin.h"


CPlugin::CPlugin()
{
	m_hLib = NULL;
	m_szFilePath = NULL;
	m_pfMakeDialog = NULL;
	m_pfCloseDialog = NULL;
}

CPlugin::~CPlugin()
{
	if (m_hLib) FreeLibrary((HMODULE)m_hLib);
	m_hLib = NULL;

	if (m_szFilePath) delete[] m_szFilePath;
	m_szFilePath = NULL;
}


VOID CPlugin::SetFile(TCHAR* szFilePath)
{
	TCHAR* temp = m_szFilePath;

	if (szFilePath)
	{
		INT len = _tcslen(szFilePath) + 1;
		m_szFilePath = new TCHAR[len];
		memset(m_szFilePath, 0, sizeof(TCHAR)* len);
		_tcscpy(m_szFilePath, szFilePath);

		if (temp) delete[] temp;
		temp = NULL;
	}
}

BOOL CPlugin::Load(const char* szFuncOpenName, const char* szFuncCloseName)
{
	BOOL ret = FALSE;

	if (m_szFilePath)
	{
		if ((m_hLib = LoadLibrary(m_szFilePath)))
		{
			if (GetFunc(szFuncOpenName, (VOID**)&m_pfMakeDialog) && GetFunc(szFuncCloseName, (VOID**)&m_pfCloseDialog))
			{
				ret = TRUE;
			}
		}
	}

	return ret;
}

VOID CPlugin::Unload()
{
	m_pfMakeDialog = NULL;
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
									 TCHAR ext_name[MAX_PATH];
									 memset(ext_name, 0, sizeof (ext_name));
									 LoadString(hInstance, IDS_EXT_NAME_DLL, (TCHAR*)ext_name, _countof(ext_name));
									 _tcscat(ext_name + _tcslen(ext_name) + 1, TEXT("*.dll"));

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
