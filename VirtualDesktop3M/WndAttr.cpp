/**
 * @author Micha³ Gawin
 */


#include "WndAttr.h"


BOOL MakeWindowTransparent (HWND hwnd, UCHAR factor)
{
   SetLastError (0);

   SetWindowLong (hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

   if (GetLastError ())
      return FALSE;

   factor = __min (factor, 255);
   factor = __max (0, factor);

   return SetLayeredWindowAttributes (hwnd, 0, factor, LWA_ALPHA);
}


BOOL SlowHideWindow (HWND hWnd, INT time, INT value)
{
	if (value <= 0)
	{
		return FALSE;
	}

	DWORD period = time / value;
	while (value--)
	{
		MakeWindowTransparent (hWnd, value);
		Sleep (period);
	}
	MakeWindowTransparent (hWnd, 0);

	return TRUE;
}


BOOL SlowShowWindow (HWND hWnd, INT time, INT value)
{
	if (value <= 0)
	{
		return FALSE;
	}

	DWORD period = time / value;
	for (int i = 1; i < value; i++)
	{
		MakeWindowTransparent (hWnd, i);
		Sleep (period);
	}

	return TRUE;
}
