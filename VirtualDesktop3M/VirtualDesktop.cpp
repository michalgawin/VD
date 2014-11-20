/**
 * VirtualDesktop.cpp : Defines the entry point for the application.
 * @author Micha³ Gawin
 */


#include "VirtualDesktop.h"

#include "Sys.h"
#include "Reg.h"
#include "Tray.h"
#include "WndMgr.h"

// Plugins
#include "WindowsManager.h"
#include "Wallpaper.h"
#include "Plugin.h"
#include "About.h"

#include "DesktopMgr\DesktopMgr.h"			// header to shared dll

WindowsOnDesktop windowsOnDesktop[DESKTOPS];			// Struct keep data for each desktop

HINSTANCE hPlugin;

extern PluginGUI pluginGUI;

/* Function check did one of Virtual Desktop instance allready works
 * @return TRUE if works
 * @param clsName - windows class name
 */
BOOL isRunning (const TCHAR const * clsName)
{
	BOOL status = FALSE;
	HWND hApp = NULL;

	hApp = FindWindowEx (NULL, NULL, clsName, NULL);

	if (hApp)
	{
		status = TRUE;
	}

	return status;
}


/**
 * Function show popup menu
 * @return TRUE if successfully created menu, otherwise return FALSE
 * @param hwnd handle of window
 * @param check point on status of checkbox "Always on top"
 */
BOOL CreatePopupMenuInTray (HWND hwnd, BOOL check)
{
	HINSTANCE hInstance = (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE);
	static HMENU menu = NULL;
	POINT mouse;

	if (menu)
	{
		DestroyMenu (menu);
	}

	menu = CreatePopupMenu ();
	if (!menu)
	{
		return FALSE;
	}

	TCHAR szOnTop[MAX_PATH];
	LoadString (hInstance, IDS_ON_TOP, (TCHAR*) szOnTop, sizeof (szOnTop));

	if (check)
		AppendMenu (menu, MF_BYPOSITION | MF_STRING | MF_CHECKED, CMD_AOT, (LPCTSTR) szOnTop);
	else
		AppendMenu (menu, MF_BYPOSITION | MF_STRING | MF_UNCHECKED, CMD_AOT, (LPCTSTR) szOnTop);

	TCHAR szContDeskManager[MAX_PATH];
	LoadString (hInstance, IDS_DESK_CONTENT_MANAGER, (TCHAR*) szContDeskManager, sizeof (szContDeskManager));

	AppendMenu (menu, MF_BYPOSITION | MF_STRING, CMD_DSKMGR, (LPCTSTR) szContDeskManager);

	TCHAR szPlugin[MAX_PATH];
	LoadString (hInstance, IDS_PLUGIN, (TCHAR*) szPlugin, sizeof (szPlugin));

	AppendMenu (menu, MF_BYPOSITION | MF_STRING, CMD_PLUGIN, (LPCTSTR) szPlugin);

	TCHAR szWallpapers[MAX_PATH];
	LoadString (hInstance, IDS_WALLPAPERS, (TCHAR*) szWallpapers, sizeof (szWallpapers));

	AppendMenu (menu, MF_BYPOSITION | MF_STRING, CMD_WALL, (LPCTSTR) szWallpapers);
	AppendMenu (menu, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

	TCHAR szAbout[MAX_PATH];
	LoadString (hInstance, IDS_ABOUT, (TCHAR*) szAbout, sizeof (szAbout));

	AppendMenu (menu, MF_BYPOSITION | MF_STRING, CMD_ABOUT, (LPCTSTR) szAbout);
	AppendMenu (menu, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

	TCHAR szQuit[MAX_PATH];
	LoadString (hInstance, IDS_QUIT, (TCHAR*) szQuit, sizeof (szQuit));

	AppendMenu (menu, MF_BYPOSITION | MF_STRING, CMD_QUIT, (LPCTSTR) szQuit);

	SetMenuDefaultItem (menu, CMD_WALL, FALSE);

	GetCursorPos (&mouse);
	SetForegroundWindow (hwnd);
	TrackPopupMenu (menu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, mouse.x, mouse.y, 0, hwnd, 0);
	PostMessage (hwnd, WM_NULL, 0, 0);

	return TRUE;
}


// Main Window Procedure
LRESULT CALLBACK WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain (HINSTANCE hInstance,
					  HINSTANCE hPrevInstance,
					  LPSTR     lpCmdLine,
					  int       nCmdShow)
{
	MSG msg;
	HWND hwnd;
 	WNDCLASS wndclass;
	memset (&wndclass, 0, sizeof (WNDCLASS));

	if (isRunning (szClassName))
	{
		TCHAR szAppName[MAX_PATH];
		LoadString (hInstance, IDS_APP_NAME, (TCHAR*) szAppName, sizeof (szAppName));
		
		TCHAR szAppLaunched[MAX_PATH];
		LoadString (hInstance, IDS_APP_LAUNCHED, (TCHAR*) szAppLaunched, sizeof (szAppLaunched));

		MessageBox (NULL, szAppLaunched, szAppName, MB_OK);
		return -1;
	}

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject(DKGRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szClassName;

	if (!RegisterClass (&wndclass))
	{
		TCHAR szError[MAX_PATH];
		LoadString (hInstance, IDS_ERROR, (TCHAR*) szError, sizeof (szError));
		
		TCHAR szNotSuppOS[MAX_PATH];
		LoadString (hInstance, IDS_NOT_SUPPORTED_OS, (TCHAR*) szNotSuppOS, sizeof (szNotSuppOS));

		MessageBox (NULL, szError, szNotSuppOS, MB_OK | MB_ICONERROR);
		return -1;
	}

	TCHAR szAppName[MAX_PATH];
	LoadString (hInstance, IDS_APP_NAME, (TCHAR*) szAppName, sizeof (szAppName));

	hwnd = CreateWindow (szClassName,
				 szAppName,
				 WS_POPUP,
				 0, 0, 0, 0,
				 NULL,
				 NULL,
				 hInstance,
				 NULL
				 );

	ShowWindow (hwnd, SW_HIDE);
	UpdateWindow (hwnd);

	// enable hot keys to change desktops
	RegisterHotKey (hwnd, NEXT_WIN, MOD_CONTROL, VK_TAB);
	RegisterHotKey (hwnd, PREV_WIN, MOD_CONTROL|MOD_SHIFT, VK_TAB);

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	UnregisterHotKey (hwnd, NEXT_WIN);
	UnregisterHotKey (hwnd, PREV_WIN);

	return msg.wParam;
}


/* Main Window Procedures */
LRESULT CALLBACK WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;							// instance to program
	static HINSTANCE shared_Dll = NULL;				// handle to instance of shared dll
	static TCHAR szOrginalWallpaper[MAX_PATH];		// hold orginal desktop wallpaper
	static BOOL check = TRUE;						// hold status of check field "Always on top"
	static CTray* tray = NULL;

	TCHAR szWallpaper[MAX_PATH];							// string which is use to create bitmap
	TCHAR szWallpaperTemplate[] = TEXT("Wallpaper#%d");		// wallpaper name template
	TCHAR szDefaultPluginName[] = TEXT("SamplePlugin.dll");	// default plugin name

	switch (uMsg)
	{
	case WM_CREATE:
		{
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance;

		memset (szOrginalWallpaper, 0, sizeof (szOrginalWallpaper));
		SystemParametersInfo (SPI_GETDESKWALLPAPER, MAX_PATH, szOrginalWallpaper, 0);

		for (int i = 0; i < DESKTOPS; i++)
		{
			windowsOnDesktop[i].szWallpaper = new TCHAR[MAX_PATH];
			memset (windowsOnDesktop[i].szWallpaper, 0, MAX_PATH * sizeof (TCHAR));
		}

		if (!IsRegistryEntry (TEXT("Software"), VD_MAIN_KEY, VD_PLUGIN_KEY))	// create entry in registry
		{
			for (int i = 0; i < DESKTOPS; i++)
			{
				memset (szWallpaper, 0, sizeof (szWallpaper));
				_stprintf (szWallpaper, szWallpaperTemplate, i);

				SystemParametersInfo (SPI_GETDESKWALLPAPER, MAX_PATH, windowsOnDesktop[i].szWallpaper, 0);
				SetInRegistry (TEXT("Software"), VD_MAIN_KEY, NULL, szWallpaper, (VOID*) windowsOnDesktop[i].szWallpaper, _tcslen(windowsOnDesktop[i].szWallpaper));
			}

			SetInRegistry (TEXT("Software"), VD_MAIN_KEY, NULL, VD_PLUGIN_PATH_KEY, (VOID*) szDefaultPluginName, _tcslen(szDefaultPluginName));
			_tcscpy (pluginGUI.szPluginPath, szDefaultPluginName);	// Set plugin path
		}
		else	// configuration already exists in registry 
		{
			for (int i = 0; i < DESKTOPS; i++)
			{
				memset (szWallpaper, 0, sizeof (szWallpaper));
				_stprintf (szWallpaper, szWallpaperTemplate, i);

				if (!GetFromRegistry (TEXT("Software"), VD_MAIN_KEY, NULL, szWallpaper, windowsOnDesktop[i].szWallpaper, MAX_PATH))
				{
					SystemParametersInfo (SPI_GETDESKWALLPAPER, MAX_PATH, windowsOnDesktop[i].szWallpaper, 0);
				}
			}

			TCHAR szDllPath[MAX_PATH];
			memset (szDllPath, 0, sizeof (szDllPath));

			GetFromRegistry (TEXT("Software"), VD_MAIN_KEY, NULL, VD_PLUGIN_PATH_KEY, szDllPath, MAX_PATH);
			_tcscpy (pluginGUI.szPluginPath, szDllPath);	// Set plugin path
			if (_tcslen (windowsOnDesktop[0].szWallpaper) > 0)
			{
				SystemParametersInfo (SPI_SETDESKWALLPAPER, _tcslen (windowsOnDesktop[0].szWallpaper), windowsOnDesktop[0].szWallpaper, 0);
			}
		}

		shared_Dll = LoadPlugin (DESKTOP_MGR_SZ);	// load shared library
		if (!shared_Dll)
		{
			TCHAR szAppName[MAX_PATH];
			LoadString (hInstance, IDS_APP_NAME, (TCHAR*) szAppName, sizeof (szAppName));
			
			TCHAR szErrDeskManager[MAX_PATH];
			LoadString (hInstance, IDS_NO_DESKTOP_MANAGER, (TCHAR*) szErrDeskManager, sizeof (szErrDeskManager));

			MessageBox (NULL, szErrDeskManager, szAppName, MB_OK);
		}

		if (!GetPlugin (hPlugin, pluginGUI.szPluginPath, pluginGUI.PluginOpenDlgFun, pluginGUI.PluginCloseDlgFun))	// load GUI plugin
		{
			TCHAR szAppName[MAX_PATH];
			LoadString (hInstance, IDS_APP_NAME, (TCHAR*) szAppName, sizeof (szAppName));
			
			TCHAR szNoPlugin[MAX_PATH];
			LoadString (hInstance, IDS_NO_PLUGIN, (TCHAR*) szNoPlugin, sizeof (szNoPlugin));

			MessageBox (NULL, szNoPlugin, szAppName, MB_OK);
		}
		
		tray = new CTray (hwnd, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON)));
		tray->Show ();
		return 0;
		}
	case WM_PARENTNOTIFY:	// notification while children DestroyWindow
		return 0;
	case WM_TRAY_ICON:
		switch (lParam)
		{
		case WM_LBUTTONDOWN:
			{
			BOOL visible = FALSE;
			if (pluginGUI.hWnd)
			{
				visible = IsWindowVisible (pluginGUI.hWnd);
			}
			if (!visible)
			{
				if (hPlugin != NULL)
				{
					pluginGUI.hWnd = pluginGUI.PluginOpenDlgFun (hwnd, shared_Dll);
					UpdateWindow (pluginGUI.hWnd);
				}
			}
			}
			break;
		case WM_RBUTTONDOWN:
			CreatePopupMenuInTray (hwnd, check);
			break;
		}
		return 0;
	case WM_HOTKEY:
		switch (HIWORD (lParam))
		{
		case VK_TAB:
		{
			switch (LOWORD (lParam))
			{
			case MOD_CONTROL | MOD_SHIFT:
			{
				int desk = GetCurrentDesktop ();
				desk--;

				
				if (desk < 0 || desk >= DESKTOPS)
				{
					desk = (desk+DESKTOPS) % DESKTOPS;
				}

				ChangeDesktop (desk);
				break;
			}
			case MOD_CONTROL:
			{
				int desk = GetCurrentDesktop ();
				desk++;

				desk = desk % DESKTOPS;

				ChangeDesktop (desk);
				break;
			}
			default:
				break;
			}
		}
		default:
			break;
		}
		return 0;
	case WM_CHANGE_DESKTOP:	// lParam - no. of desktop
		{
		SystemParametersInfo (SPI_SETDESKWALLPAPER, _tcslen (windowsOnDesktop[lParam].szWallpaper), windowsOnDesktop[lParam].szWallpaper, 0);

		HideWindows (hwnd, pluginGUI.hWnd, windowsOnDesktop[GetCurrentDesktop ()].table, TRUE);

		INT icon = 0;	// id of icon in tray

		// WARNING!!! Work only if icons in resources are in ascending sequence!
		tray->ChangeIcon (LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON + lParam)));

		SetCurrentDesktop (lParam);

		ShowWindows (windowsOnDesktop[GetCurrentDesktop ()].table);

		if (check)
		{
			if (hPlugin != NULL)
			{
				if (pluginGUI.hWnd)
				{
					BOOL visible = FALSE;

					visible = IsWindowVisible (pluginGUI.hWnd);
					if (visible)
					{
						UpdateWindow (pluginGUI.hWnd);
					}
				}
			}
		}
		else
		{
			if (pluginGUI.hWnd != NULL)
			{
				pluginGUI.PluginCloseDlgFun (pluginGUI.hWnd);
				pluginGUI.hWnd = NULL;
			}
		}
		}
		return 0;
	case WM_COMMAND:
		{
		switch (LOWORD (wParam))
		{
		case CMD_AOT:
			check = !check;
			break;
		case CMD_DSKMGR:
			GetWindowsFromDesktop (hwnd, pluginGUI.hWnd, windowsOnDesktop[GetCurrentDesktop ()].table);
			if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_DESKTOP_MANAGER), hwnd, DlgDesktopManagerProc, (LPARAM) windowsOnDesktop))
			{
				//HideWindows (hwnd, pluginGUI.hWnd, windowsTab[GetCurrentDesktop ()].table, FALSE);
			}
			break;
		case CMD_PLUGIN:
			{
			DialogBox (hInstance, MAKEINTRESOURCE (IDD_SELECT_PLUGIN), hwnd, DlgPluginProc);
			}
			break;
		case CMD_WALL:
			DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_DIALOG_WALLPAPER), hwnd, DlgWallpaperProc, (LPARAM) windowsOnDesktop);
			break;
		case CMD_ABOUT:
			DialogBox (hInstance, MAKEINTRESOURCE (IDD_DIALOG_ABOUT), hwnd, DlgAboutProc);
			break;
		case CMD_QUIT:
			SendMessage (hwnd, WM_CLOSE, 0, 0);
			break;
		}
		}
		return 0;
	case WM_QUERYENDSESSION:
		SendMessage (hwnd, WM_DESTROY, 0, 0);
		return 1;
	case WM_CLOSE: // catch ALT+F4 to prevent hide tray icon
		{
		TCHAR szAppName[MAX_PATH];
		LoadString (hInstance, IDS_APP_NAME, (TCHAR*) szAppName, sizeof (szAppName));
		
		TCHAR szExitQuestion[MAX_PATH];
		LoadString (hInstance, IDS_EXIT_QUESTION, (TCHAR*) szExitQuestion, sizeof (szExitQuestion));
		if (MessageBox (NULL, szExitQuestion, szAppName, MB_YESNO) == IDYES)
		{
			SendMessage (hwnd, WM_DESTROY, 0, 0);
		}
		}
		return 0;
	case WM_DESTROY:
		{
		if (hPlugin)
		{
			pluginGUI.PluginCloseDlgFun (pluginGUI.hWnd);
		}

		for (int i = 0; i < DESKTOPS; i++)
		{
			memset (szWallpaper, 0, sizeof (szWallpaper));
			_stprintf (szWallpaper, szWallpaperTemplate, i);

			SetInRegistry (TEXT("Software"), VD_MAIN_KEY, NULL, szWallpaper, (VOID*) windowsOnDesktop[i].szWallpaper, _tcslen (windowsOnDesktop[i].szWallpaper));
		}
		//Save plugin path in registry
		SetInRegistry (TEXT("Software"), VD_MAIN_KEY, NULL, VD_PLUGIN_PATH_KEY, (VOID*) pluginGUI.szPluginPath, _tcslen (pluginGUI.szPluginPath));

		tray->Hide ();

		SystemParametersInfo (SPI_SETDESKWALLPAPER, _tcslen (szOrginalWallpaper), szOrginalWallpaper, 0);

		for (int i = 0; i < DESKTOPS; i++)
		{
			SetCurrentDesktop (i);
			ShowWindows (windowsOnDesktop[GetCurrentDesktop ()].table);
		}

		SetCurrentDesktop (0);

		FreePlugin (hPlugin);
		hPlugin = NULL;
		pluginGUI.PluginCloseDlgFun = NULL;
		pluginGUI.PluginOpenDlgFun = NULL;

		FreePlugin (shared_Dll);

		PostQuitMessage (0);
		return 0;
		}
	}
	return DefWindowProc (hwnd, uMsg, wParam, lParam);
}
