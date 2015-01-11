/**
 * @author Micha³ Gawin
 */


#include "Reg.h"


CRegistry::CRegistry(TCHAR* szRootKey)
{
	INT len = _tcslen(szRootKey) + 1;
	m_szRootKey = new TCHAR[len];
	memset(m_szRootKey, 0, len*sizeof(TCHAR));
	_tcscpy(m_szRootKey, szRootKey);
}


CRegistry::~CRegistry()
{
	if (m_szRootKey) delete[] m_szRootKey;
	m_szRootKey = NULL;
}


BOOL CRegistry::Exists(TCHAR* szSubKey)
{
	BOOL bStatus = FALSE;
	HKEY hKey = NULL;
	TCHAR szPath[MAX_PATH];

	memset(szPath, 0, sizeof (szPath));
	_tcscpy(szPath, m_szRootKey);
	if (szSubKey)
	{
		_tcscat(szPath, TEXT("\\"));
		_tcscat(szPath, szSubKey);
	}

	if (RegOpenKeyEx(m_hKey, szPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) bStatus = TRUE;
	RegCloseKey(hKey);

	return bStatus;
}


BOOL CRegistry::Create(TCHAR* szSubKey)
{
	BOOL bStatus = FALSE;
	HKEY hKey = NULL;
	TCHAR szPath[MAX_PATH];

	memset(szPath, 0, sizeof (szPath));
	_tcscpy(szPath, m_szRootKey);
	if (szSubKey)
	{
		_tcscat(szPath, TEXT("\\"));
		_tcscat(szPath, szSubKey);
	}

	if (RegOpenKeyEx(m_hKey, NULL, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		HKEY hKey2 = NULL;
		if (RegCreateKeyEx(hKey, szPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey2, NULL) == ERROR_SUCCESS) bStatus = TRUE;
		
		RegCloseKey(hKey2);
	}

	return bStatus;
}


BOOL CRegistry::Set(TCHAR* szValueName, VOID* szData, INT iSize, TCHAR* szSubKey)
{
	BOOL bStatus = FALSE;
	HKEY hKey = NULL;
	TCHAR szPath[MAX_PATH];

	memset(szPath, 0, sizeof (szPath));
	_tcscpy(szPath, m_szRootKey);
	if (szSubKey)
	{
		_tcscat(szPath, TEXT("\\"));
		_tcscat(szPath, szSubKey);
	}

	if (RegOpenKeyEx(m_hKey, szPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		TCHAR szAscii[MAX_PATH];
		memset(szAscii, 0, sizeof (szAscii));

		if (!Exists(szSubKey)) Create(szSubKey);
		if (RegSetValueEx(hKey, szValueName, 0, REG_SZ, (const BYTE*)szData, iSize*sizeof(TCHAR)) == ERROR_SUCCESS) bStatus = TRUE;

		RegCloseKey(hKey);
	}

	return bStatus;
}


BOOL CRegistry::Get(TCHAR* szValueName, TCHAR* szData, INT iSize, TCHAR* szSubKey)
{
	BOOL bStatus = FALSE;
	HKEY hKey = NULL;
	TCHAR szPath[MAX_PATH];

	memset(szPath, 0, sizeof (szPath));
	_tcscpy(szPath, m_szRootKey);
	if (szSubKey)
	{
		_tcscat(szPath, TEXT("\\"));
		_tcscat(szPath, szSubKey);
	}

	if (RegOpenKeyEx(m_hKey, szPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		PSTR pMultiByteStr = new CHAR[MAX_PATH];
		DWORD cchLength = MAX_PATH*sizeof(CHAR);
		DWORD dwType = REG_SZ;

		if (RegQueryValueEx(hKey, szValueName, NULL, &dwType, (PBYTE)pMultiByteStr, &cchLength) == ERROR_SUCCESS)
		{
			if (IsTextUnicode(szData, iSize, NULL))
			{
				PWSTR pWideCharStr;
				int nLenOfWideCharStr;

				nLenOfWideCharStr = MultiByteToWideChar(CP_ACP, 0, pMultiByteStr, cchLength, NULL, 0);
				pWideCharStr = (PWSTR)HeapAlloc(GetProcessHeap(), 0, nLenOfWideCharStr*sizeof(wchar_t));
				MultiByteToWideChar(CP_ACP, 0, pMultiByteStr, cchLength, pWideCharStr, nLenOfWideCharStr);

				if (!wmemcpy_s(szData, iSize, pWideCharStr, nLenOfWideCharStr)) bStatus = TRUE;

				HeapFree(GetProcessHeap(), 0, pWideCharStr);
			}
			else
			{
				if (!memcpy_s(szData, iSize, pMultiByteStr, cchLength)) bStatus = TRUE;
			}
		}
		RegCloseKey(hKey);
	}

	return bStatus;
}
