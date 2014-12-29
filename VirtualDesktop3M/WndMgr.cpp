/**
 * @author Micha³ Gawin
 */


#include "WndMgr.h"


INT GetWindowsFromDesktop (HWND hApp, vHandle& table)
{
	HWND hDesktop = GetDesktopWindow();
	HWND hTaskBar = FindWindowEx(NULL, NULL, TEXT("Shell_TrayWnd"), NULL);
	HWND hDesktopIcon = FindWindowEx(NULL, NULL, TEXT("Progman"), NULL);	// icons "My Computer" etc.

	table.clear();

	for (HWND hWindowTop = GetTopWindow(hDesktop); hWindowTop; hWindowTop = GetWindow(hWindowTop, GW_HWNDNEXT))
	{
		if (IsWindowVisible(hWindowTop) && GetParent(hWindowTop) != hApp && hWindowTop != hTaskBar && hWindowTop != hDesktop && hWindowTop != hDesktopIcon && hWindowTop != hApp && GetParent(hWindowTop) != hTaskBar)
			table.push_back(hWindowTop);
	}

	return table.size();
}


INT HideWindows (HWND hApp, vHandle& table, BOOL update)
{
	if (update)
	{
		GetWindowsFromDesktop (hApp, table);
	}

	HWND hPlug = GetWindow(hApp, GW_CHILD);

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
