/**
 * @author Micha³ Gawin
 */


#ifndef _REG_H_VD_
#define _REG_H_VD_

#include "VirtualDesktop.h"

#define	SZ_VD_MAIN_KEY	TEXT("Software\\VirtualDesktop")
#define	SZ_VD_PLUGIN_KEY	TEXT("Plugin")
#define SZ_VD_PLUGIN_PATH_KEY	TEXT("PluginPath")

class CRegistry {
private:
	HKEY m_hKey = HKEY_CURRENT_USER;
	TCHAR* m_szRootKey;

public:
	/**
	 * Handle registry key for application
	 * @param hKey is one of the predefined keys
	 * @szRootKey main key of application, all subkeys are relative to this one
	 */
	CRegistry(TCHAR* szRootKey);

	~CRegistry();

	/**
	 * Check if registry entry is already created.
	 * @return TRUE if registry exists, FALSE otherwise
	 * @param szSubKey relative key of szRootKey to create
	 */
	BOOL Exists(TCHAR* szSubKey = NULL);

	/**
	* Create registry entry based on szRootKey and szSubKey.
	* @return TRUE if registry exists, FALSE otherwise
	* @param szSubKey relative key of szRootKey to create
	*/
	BOOL Create(TCHAR* szSubKey=NULL);

	/**
	 * Set new value in registry
	 * @return TRUE if operation completed with success, FALSE otherwise
	 * @param szValueName name of new value in registry
	 * @param szData data of szValueName
	 * @param iSize size in characters of szData
	 * @param szSubKey sub key of szKey in registry
	 */
	BOOL Set(IN TCHAR* szValueName, IN VOID* szData, IN INT iSize, IN TCHAR* szSubKey=NULL);

	/**
	 * Function get value from registry
	 * @return TRUE if operation completed with success, FALSE otherwise
	 * @param szValueName name of value in registry
	 * @param szData pointer in which data of szValueName will be stored
	 * @param iSize szData buffer size in characters
	 * @param szSubKey sub key of szKey in registry
	 */
	BOOL Get(IN TCHAR* szValueName, OUT TCHAR* szData, IN INT iSize, IN TCHAR* szSubKey=NULL);
};

#endif //_REG_H_VD_
