/**
* @author Micha³ Gawin
*/


#include "Sys.h"
#include "VirtualDesktop.h"


void ShowErr(HINSTANCE hInst, UINT idsCaption, UINT idsText, ...)
{
	TCHAR szCaption[MAX_PATH];
	LoadString(hInst, idsCaption, (TCHAR*)szCaption, _countof(szCaption));

	TCHAR szText[MAX_PATH];
	LoadString(hInst, idsText, (TCHAR*)szText, _countof(szText));

	va_list pArgList;
	va_start(pArgList, idsCaption);

	TCHAR szMessage[MAX_PATH];
	_vsntprintf_s(szMessage, _countof(szMessage), _TRUNCATE, szText, pArgList);

	va_end(pArgList);

	MessageBox(NULL, szMessage, szCaption, MB_OK);
}
