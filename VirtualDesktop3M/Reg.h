/**
 * @author Micha³ Gawin
 */


#ifndef _REG_H_VD_
#define _REG_H_VD_

#include "VirtualDesktop.h"

#define	VD_MAIN_KEY	TEXT("VirtualDesktop")
#define	VD_PLUGIN_KEY	TEXT("Plugin")
#define VD_PLUGIN_PATH_KEY	TEXT("PluginPath")

/**
 * Function init registry
 * @return TRUE if registry was initialized
 */
BOOL IsRegistryEntry (TCHAR* branch, TCHAR* key, TCHAR* subkey);

/**
 * Function set new value in registry
 * @return TRUE if operation completed success
 * @param name new subkey in registry
 * @param value value placed in subkey
 * @param size rozmiar value
 */
BOOL SetInRegistry (TCHAR* branch, TCHAR* key, TCHAR* subkey, TCHAR* name, VOID* value, INT size);

/**
 * Function get value from registry
 * @return TRUE if operation completed success
 * @param name name of subkey
 * @param value will keep value from subkey
 * @param size not used
 */
BOOL GetFromRegistry (TCHAR* branch, TCHAR* key, TCHAR* subkey, TCHAR* name, TCHAR* value, INT size);


#endif //_REG_H_VD_
