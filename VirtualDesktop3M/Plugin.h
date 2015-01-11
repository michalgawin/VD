/**
* @author Micha³ Gawin
*/


#ifndef _PLUGIN_H_VD_
#define _PLUGIN_H_VD_

#include "VirtualDesktop.h"
#include "SamplePlugin\SamplePlugin.h"


class CPlugin
{
private:
	HMODULE m_hLib;		//handle to library (dll)
	TCHAR* m_szFilePath;	//full path, including file name, to library

public:
	t_pfMakeDialog m_pfMakeDialog;	//pointer to function which creates window
	t_pfCloseDialog m_pfCloseDialog;	//pointer to function which close window

public:
	CPlugin();
	~CPlugin();
	BOOL Load(IN const char* szFuncOpenName = szExFunNameMakeDialog, IN const char* szFuncCloseName = szExFunNameCloseDialog);
	VOID Unload();
	VOID SetFile(TCHAR* szFilePath);
	TCHAR* GetFile() { return m_szFilePath; };

private:
	BOOL GetFunc(IN const char* szFuncName, OUT VOID** pFun);
};

/**
* Dialog window proc
*/
BOOL CALLBACK DlgPluginProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#endif //_PLUGIN_H_VD_
