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

	m_pfMakeDialog = NULL;
	m_pfCloseDialog = NULL;
}


VOID CPlugin::SetFile(TCHAR* szFilePath)
{
	TCHAR* temp = m_szFilePath;

	if (szFilePath)
	{
		INT len = _tcslen(szFilePath) + 1;
		m_szFilePath = new TCHAR[len];
		memset(m_szFilePath, 0, sizeof(TCHAR)* len);
		_tcscpy_s(m_szFilePath, len, szFilePath);

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
	if (m_hLib) FreeLibrary((HMODULE)m_hLib);
	m_hLib = NULL;

	m_pfMakeDialog = NULL;
	m_pfCloseDialog = NULL;
}

BOOL CPlugin::GetFunc(const char* szFuncName, VOID** pFun)
{
	if (m_hLib) *pFun = (VOID*)GetProcAddress(m_hLib, szFuncName);

	return (*pFun != NULL);
}
