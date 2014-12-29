/**
 * @author Micha³ Gawin
 */


#include "Reg.h"


BOOL IsRegistryEntry (TCHAR* branch, TCHAR* key, TCHAR* subkey)
{
	HKEY hKey = NULL;
	BOOL status = FALSE;
	TCHAR path[MAX_PATH];

	memset (path, 0, sizeof (path));
	_tcscpy (path, branch);
	_tcscat (path, TEXT("\\"));
	_tcscat (path, key);

	if (RegOpenKeyEx (HKEY_CURRENT_USER, path, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		status = TRUE;
	}
	else
	{
		if (RegOpenKeyEx (HKEY_CURRENT_USER, branch, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			HKEY hKey2 = NULL;
			if (RegCreateKeyEx (hKey, key, 0, NULL, 0, KEY_WRITE, NULL, &hKey2, NULL) == ERROR_SUCCESS)
			{
				HKEY hKey3 = NULL;
				if (RegCreateKeyEx (hKey2, subkey, 0, NULL, 0, KEY_WRITE, NULL, &hKey3, NULL) == ERROR_SUCCESS)
				{
					RegCloseKey (hKey3);
				}
				RegCloseKey (hKey2);
			}
		}
	}
	RegCloseKey (hKey);

	return status;
}


BOOL SetInRegistry (TCHAR* branch, TCHAR* key, TCHAR* subkey, TCHAR* name, VOID* value, INT size)
{
	HKEY hKey = NULL;
	BOOL status = FALSE;
	LONG error = 0;
	TCHAR path[MAX_PATH];

	memset (path, 0, sizeof (path));
	_tcscpy (path, branch);
	_tcscat (path, TEXT("\\"));
	_tcscat (path, key);
	if (subkey)
	{
		_tcscat (path, subkey);
	}

	error = RegOpenKeyEx (HKEY_CURRENT_USER, path, 0, KEY_WRITE, &hKey);
	if (error == ERROR_SUCCESS)
	{
		TCHAR szAscii[MAX_PATH];
		memset (szAscii, 0, sizeof (szAscii));
		
		DWORD cbLength = _tcslen((TCHAR*) value) * sizeof (TCHAR);
		error = RegSetValueEx (hKey, name, 0, REG_SZ, (const BYTE*) value, cbLength);
		if (error == ERROR_SUCCESS)
		{
			status = TRUE;
		}

		RegCloseKey (hKey);
	}

	return status;
}


BOOL GetFromRegistry (TCHAR* branch, TCHAR* key, TCHAR* subkey, TCHAR* name, TCHAR* value, INT size)
{
	HKEY hKey = NULL;
	BOOL status = FALSE;
	BYTE val[MAX_PATH];
	ULONG s = MAX_PATH;
	DWORD type = REG_SZ;
	TCHAR path[MAX_PATH];

	memset (path, 0, sizeof (TCHAR) * MAX_PATH);
	_tcscpy (path, branch);
	_tcscat (path, TEXT("\\"));
	_tcscat (path, key);
	if (subkey)
	{
		_tcscat (path, subkey);
	}

	if (RegOpenKeyEx (HKEY_CURRENT_USER, path, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx (hKey, name, 0, &type, val, &s) == ERROR_SUCCESS)
		{
			status = TRUE;
		}

		if (IsTextUnicode (value, size, NULL))
		{
			WCHAR wszVal[MAX_PATH];
			memset (wszVal, 0, sizeof (WCHAR) * MAX_PATH);

			MultiByteToWideChar (CP_ACP,
								 0,
								 (const char*) val,
								 strlen ((const char*) val)+1,
								 wszVal,
								 sizeof (wszVal) / sizeof (WCHAR)
								 );

			memcpy (value, wszVal, sizeof (WCHAR) * MAX_PATH);
			
		}
		else
		{
			memcpy (value, val, s);
		}

		RegCloseKey (hKey);
	}

	return status;
}
