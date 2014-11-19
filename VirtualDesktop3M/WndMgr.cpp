/**
 * @author Micha³ Gawin
 */


#include "WndMgr.h"


INT GetWindowsFromDesktop (HWND hApp, HWND hPlug, vHandle& table)
{
	HWND hDesktop = NULL;
	HWND hWindowTop = NULL;
	HWND hDesktopIcon = NULL; // icon "My Computer" etc.
	HWND hTaskBar = NULL;

	hDesktop = GetDesktopWindow ();
	hWindowTop = GetTopWindow (hDesktop);

	table.clear();

	if (hWindowTop == NULL)
	{
		return table.size();
	}

	hTaskBar = FindWindowEx (NULL, NULL, "Shell_TrayWnd", NULL);
	hDesktopIcon = FindWindowEx (NULL, NULL, "Progman", NULL);

	do
	{
		if (hWindowTop && IsWindowVisible (hWindowTop) && hWindowTop != hTaskBar && hWindowTop != hDesktop && hWindowTop != hDesktopIcon && hWindowTop != hApp && hWindowTop != hPlug)
		{
			table.push_back(hWindowTop);
		}
	} while ((hWindowTop = GetWindow (hWindowTop, GW_HWNDNEXT)) != NULL);

	return table.size();
}


INT HideWindows (HWND hApp, HWND hPlug, vHandle& table, BOOL update)
{
	if (update)
	{
		GetWindowsFromDesktop (hApp, hPlug, table);
	}

	HDWP s = BeginDeferWindowPos (table.size());

	for (vHandleItor itor = table.begin(); itor != table.end(); itor++)
	{
		s = DeferWindowPos (s, *itor, 0, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	EndDeferWindowPos (s);

	return table.size();
}


INT ShowWindows (vHandle& table)
{
	for (vHandleItor itor = table.begin(); itor != table.end();)
	{
		//If window not exists (e.g. process was closed from task manager) remove handle from table
		if (!IsWindow (*itor))
		{
			itor = table.erase(itor++);
		}
		else
			itor++;
	}

	HDWP s = BeginDeferWindowPos (table.size());
	for (vHandleItor itor = table.begin(); itor != table.end(); itor++)
	{
		s = DeferWindowPos (s, *itor, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	}
	EndDeferWindowPos (s);

	return table.size();
}
