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
#include "Plugin.h"
#include "About.h"

#include "DesktopMgr\DesktopMgr.h"			// header to shared dll

WindowsOnDesktop windowsOnDesktop[DESKTOPS];			// Struct keeps data for each desktop

/**
* Function show popup menu
* @return TRUE if successfully created menu, otherwise return FALSE
* @param hwnd handle of window
* @param check point on status of checkbox "Always on top"
*/
BOOL CreatePopupMenuInTray(HWND hwnd, BOOL check)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	static HMENU menu = NULL;
	POINT mouse;

	if (menu)
	{
		DestroyMenu(menu);
	}

	menu = CreatePopupMenu();
	if (!menu)
	{
		return FALSE;
	}

	TCHAR szOnTop[MAX_PATH];
	LoadString(hInstance, IDS_ON_TOP, (TCHAR*)szOnTop, _countof(szOnTop));
	AppendMenu(menu, MF_BYPOSITION | MF_STRING | (check ? MF_CHECKED : MF_UNCHECKED), CMD_AOT, (LPCTSTR)szOnTop);

	TCHAR szContDeskManager[MAX_PATH];
	LoadString(hInstance, IDS_DESK_CONTENT_MANAGER, (TCHAR*)szContDeskManager, _countof(szContDeskManager));
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, CMD_DSKMGR, (LPCTSTR)szContDeskManager);

	TCHAR szPlugin[MAX_PATH];
	LoadString(hInstance, IDS_PLUGIN, (TCHAR*)szPlugin, _countof(szPlugin));
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, CMD_PLUGIN, (LPCTSTR)szPlugin);

	AppendMenu(menu, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

	TCHAR szAbout[MAX_PATH];
	LoadString(hInstance, IDS_ABOUT, (TCHAR*)szAbout, _countof(szAbout));
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, CMD_ABOUT, (LPCTSTR)szAbout);

	AppendMenu(menu, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

	TCHAR szQuit[MAX_PATH];
	LoadString(hInstance, IDS_QUIT, (TCHAR*)szQuit, _countof(szQuit));
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, CMD_QUIT, (LPCTSTR)szQuit);

	SetMenuDefaultItem(menu, CMD_DSKMGR, FALSE);

	GetCursorPos(&mouse);
	SetForegroundWindow(hwnd);
	TrackPopupMenu(menu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, mouse.x, mouse.y, 0, hwnd, 0);
	PostMessage(hwnd, WM_NULL, 0, 0);

	return TRUE;
}


// Main Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass;
	memset(&wndclass, 0, sizeof (WNDCLASS));

	if (FindApplication(szClassName))
	{
		TCHAR szAppName[MAX_PATH];
		LoadString(hInstance, IDS_APP_NAME, (TCHAR*)szAppName, _countof(szAppName));

		TCHAR szAppLaunched[MAX_PATH];
		LoadString(hInstance, IDS_APP_LAUNCHED, (TCHAR*)szAppLaunched, _countof(szAppLaunched));

		MessageBox(NULL, szAppLaunched, szAppName, MB_OK);
		return -1;
	}

	INITCOMMONCONTROLSEX initControls;
	initControls.dwSize = sizeof (INITCOMMONCONTROLSEX);
	initControls.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&initControls);	//activate tooltips

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szClassName;

	if (!RegisterClass(&wndclass))
	{
		TCHAR szError[MAX_PATH];
		LoadString(hInstance, IDS_ERROR, (TCHAR*)szError, _countof(szError));

		TCHAR szNotSuppOS[MAX_PATH];
		LoadString(hInstance, IDS_ERR_NOT_SUPPORTED_OS, (TCHAR*)szNotSuppOS, _countof(szNotSuppOS));

		MessageBox(NULL, szError, szNotSuppOS, MB_OK | MB_ICONERROR);
		return -1;
	}

	TCHAR szAppName[MAX_PATH];
	LoadString(hInstance, IDS_APP_NAME, (TCHAR*)szAppName, _countof(szAppName));

	hwnd = CreateWindow(szClassName,
		szAppName,
		WS_POPUP,
		0, 0, 0, 0,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);

	// enable hot keys to change desktops
	RegisterHotKey(hwnd, NEXT_WIN, MOD_CONTROL, VK_TAB);
	RegisterHotKey(hwnd, PREV_WIN, MOD_CONTROL | MOD_SHIFT, VK_TAB);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterHotKey(hwnd, NEXT_WIN);
	UnregisterHotKey(hwnd, PREV_WIN);

	return msg.wParam;
}


/* Main Window Procedures */
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CRITICAL_SECTION s_criticalSection;
	static HINSTANCE s_hInstance;							// instance to program
	static HMODULE s_hSharedLib = NULL;				// handle to instance of shared dll
	static TCHAR s_szOrginalWallpaper[MAX_PATH];		// hold orginal desktop wallpaper
	static BOOL s_bAlwaysOnTop = TRUE;						// hold status of check field "Always on top"
	static CTray* s_Tray = NULL;
	static CRegistry* s_Registry = NULL;
	static CPlugin* s_Plugin = NULL;

	TCHAR szWallpaperTemplate[] = TEXT("Wallpaper#%d");		// wallpaper name template

	switch (uMsg)
	{
	case WM_CREATE:
	{
					  InitializeCriticalSection(&s_criticalSection);

					  s_hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

					  s_hSharedLib = LoadLibrary(SZ_DESKTOP_MGR);	// load shared library
					  if (!s_hSharedLib)
					  {
						  TCHAR szAppName[MAX_PATH];
						  LoadString(s_hInstance, IDS_APP_NAME, (TCHAR*)szAppName, _countof(szAppName));

						  TCHAR szErrDeskManager[MAX_PATH];
						  LoadString(s_hInstance, IDS_ERR_NO_DESKTOP_MANAGER, (TCHAR*)szErrDeskManager, _countof(szErrDeskManager));

						  MessageBox(NULL, szErrDeskManager, szAppName, MB_OK);
					  }

					  memset(s_szOrginalWallpaper, 0, _countof(s_szOrginalWallpaper)*sizeof(TCHAR));
					  SystemParametersInfo(SPI_GETDESKWALLPAPER, _countof(s_szOrginalWallpaper), s_szOrginalWallpaper, 0);

					  for (int i = 0; i < DESKTOPS; i++)
					  {
						  windowsOnDesktop[i].szWallpaper = new TCHAR[MAX_PATH];
						  memset(windowsOnDesktop[i].szWallpaper, 0, MAX_PATH * sizeof (TCHAR));
					  }

					  s_Plugin = new CPlugin();
					  s_Registry = new CRegistry(SZ_VD_MAIN_KEY);

					  TCHAR szWallpaper[MAX_PATH];
					  if (!s_Registry->Exists())	// create entry in registry
					  {
						  s_Registry->Create();

						  for (int i = 0; i < DESKTOPS; i++)
						  {
							  memset(szWallpaper, 0, sizeof (szWallpaper));
							  _stprintf(szWallpaper, szWallpaperTemplate, i);

							  if (SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, windowsOnDesktop[i].szWallpaper, 0))
							  {
								  s_Registry->Set(szWallpaper, (VOID*)windowsOnDesktop[i].szWallpaper, _tcslen(windowsOnDesktop[i].szWallpaper));
							  }
						  }

						  s_Registry->Set(SZ_VD_PLUGIN_PATH_KEY, (VOID*)SZ_DEFAULT_PLUGIN_NAME, _tcslen(SZ_DEFAULT_PLUGIN_NAME));
						  s_Plugin->SetFile(SZ_DEFAULT_PLUGIN_NAME);	// Set plugin path

						  if (!s_Registry->Exists(SZ_VD_PLUGIN_PATH_KEY)) s_Registry->Create(SZ_VD_PLUGIN_PATH_KEY);
					  }
					  else	// configuration already exists in registry 
					  {
						  for (int i = 0; i < DESKTOPS; i++)
						  {
							  memset(szWallpaper, 0, sizeof (szWallpaper));
							  _stprintf(szWallpaper, szWallpaperTemplate, i);

							  if (!s_Registry->Get(szWallpaper, windowsOnDesktop[i].szWallpaper, MAX_PATH))
							  {
								  SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, windowsOnDesktop[i].szWallpaper, 0);
							  }
						  }

						  TCHAR szLibFullPath[MAX_PATH];
						  memset(szLibFullPath, 0, sizeof (TCHAR)*_countof(szLibFullPath));

						  if (s_Registry->Get(SZ_VD_PLUGIN_PATH_KEY, szLibFullPath, _countof(szLibFullPath))) s_Plugin->SetFile(szLibFullPath);	// Set plugin path
						  else s_Plugin->SetFile(SZ_DEFAULT_PLUGIN_NAME);

						  if (_tcslen(windowsOnDesktop[GetCurrentDesktop()].szWallpaper) > 0)
						  {
							  SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen(windowsOnDesktop[GetCurrentDesktop()].szWallpaper), windowsOnDesktop[GetCurrentDesktop()].szWallpaper, 0);
						  }
					  }

					  if (!s_Plugin->Load())	// load GUI plugin
					  {
						  TCHAR szAppName[MAX_PATH];
						  LoadString(s_hInstance, IDS_APP_NAME, (TCHAR*)szAppName, _countof(szAppName));

						  TCHAR szNoPlugin[MAX_PATH];
						  LoadString(s_hInstance, IDS_ERR_NO_PLUGIN, (TCHAR*)szNoPlugin, _countof(szNoPlugin));
						  TCHAR szMessage[MAX_PATH];
						  _stprintf(szMessage, szNoPlugin, GetLastError());

						  MessageBox(NULL, szMessage, szAppName, MB_OK);
					  }

					  s_Tray = new CTray(hwnd, s_hInstance, LoadIcon(s_hInstance, MAKEINTRESOURCE(IDI_ICON)));
					  s_Tray->Show();

					  return 0;
	}
	case WM_PARENTNOTIFY:	// notification while children DestroyWindow
	{
								return 0;
	}
	case WM_TRAY_ICON:
	{
						 switch (lParam)
						 {
						 case WM_LBUTTONDOWN:
						 {
												HWND hDlg = s_Plugin->m_pfMakeDialog(hwnd, s_hSharedLib);
												break;
						 }
						 case WM_RBUTTONDOWN:
						 {
												CreatePopupMenuInTray(hwnd, s_bAlwaysOnTop);
												break;
						 }
						 }
						 return 0;
	}
	case WM_HOTKEY:
	{
					  switch (HIWORD(lParam))
					  {
					  case VK_TAB:
					  {
									 switch (LOWORD(lParam))
									 {
									 case MOD_CONTROL | MOD_SHIFT:
									 {
																	 int desk = GetCurrentDesktop();
																	 desk--;


																	 if (desk < 0 || desk >= DESKTOPS)
																	 {
																		 desk = (desk + DESKTOPS) % DESKTOPS;
																	 }

																	 ChangeDesktop(desk);
																	 break;
									 }
									 case MOD_CONTROL:
									 {
														 int desk = GetCurrentDesktop();
														 desk++;

														 desk = desk % DESKTOPS;

														 ChangeDesktop(desk);
														 break;
									 }
									 default:
									 {
												break;
									 }
									 }
					  }
					  default:
					  {
								 break;
					  }
					  }
					  return 0;
	}
	case WM_CHANGE_DESKTOP:	// lParam - no. of desktop
	{
								EnterCriticalSection(&s_criticalSection);
								INT ret = 0;

								if ((lParam >= 0) && (lParam < DESKTOPS))
								{
									SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen(windowsOnDesktop[lParam].szWallpaper), windowsOnDesktop[lParam].szWallpaper, 0);
									HideWindows(hwnd, windowsOnDesktop[GetCurrentDesktop()].table, TRUE);
									// WARNING!!! Work only if icons in resources are in ascending sequence!
									s_Tray->ChangeIcon(LoadIcon(s_hInstance, MAKEINTRESOURCE(IDI_ICON + lParam)));

									SetCurrentDesktop(lParam);

									ShowWindows(windowsOnDesktop[GetCurrentDesktop()].table);
									if (s_bAlwaysOnTop) s_Plugin->m_pfMakeDialog(hwnd, s_hSharedLib);
									else s_Plugin->m_pfCloseDialog();
								}
								else ret = ERR_DESKTOP_NUM;

								LeaveCriticalSection(&s_criticalSection);

								return ret;
	}
	case WM_COMMAND:
	{
					   switch (LOWORD(wParam))
					   {
					   case CMD_AOT:
					   {
									   s_bAlwaysOnTop = !s_bAlwaysOnTop;
									   break;
					   }
					   case CMD_DSKMGR:
					   {
										  GetWindowsFromDesktop(hwnd, windowsOnDesktop[GetCurrentDesktop()].table);
										  WindowsOnDesktop wod = {};
										  for (vHandleItor itor = windowsOnDesktop[GetCurrentDesktop()].table.begin(); itor != windowsOnDesktop[GetCurrentDesktop()].table.end();)
										  {
											  if (IsWindow(*itor))
											  {
												  wod.table.push_back(*itor);
												  itor++;
											  }
											  else
											  {
												  itor = windowsOnDesktop[GetCurrentDesktop()].table.erase(itor++);
											  }
										  }
										  if (DialogBoxParam(s_hInstance, MAKEINTRESOURCE(IDD_DESKTOP_MANAGER), hwnd, DlgDesktopManagerProc, (LPARAM)windowsOnDesktop))
										  {
											  HideWindows(hwnd, wod.table, FALSE);
											  ShowWindows(windowsOnDesktop[GetCurrentDesktop()].table);
											  SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen(windowsOnDesktop[GetCurrentDesktop()].szWallpaper), windowsOnDesktop[GetCurrentDesktop()].szWallpaper, 0);
										  }
										  break;
					   }
					   case CMD_PLUGIN:
					   {
										  DialogBoxParam(s_hInstance, MAKEINTRESOURCE(IDD_SELECT_PLUGIN), hwnd, DlgPluginProc, (LPARAM)s_Plugin);
										  //DialogBox(hInstance, MAKEINTRESOURCE(IDD_SELECT_PLUGIN), hwnd, DlgPluginProc);
										  break;
					   }
					   case CMD_ABOUT:
					   {
										 DialogBox(s_hInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwnd, DlgAboutProc);
										 break;
					   }
					   case CMD_QUIT:
					   {
										SendMessage(hwnd, WM_CLOSE, 0, 0);
										break;
					   }
					   }
					   return 0;
	}
	case WM_QUERYENDSESSION:
	{
							   //SendMessage(hwnd, WM_DESTROY, 0, 0);
							   return TRUE;
	}
	case WM_CLOSE: // catch ALT+F4 to prevent hide tray icon
	{
					   TCHAR szAppName[MAX_PATH];
					   LoadString(s_hInstance, IDS_APP_NAME, (TCHAR*)szAppName, _countof(szAppName));

					   TCHAR szExitQuestion[MAX_PATH];
					   LoadString(s_hInstance, IDS_EXIT_QUESTION, (TCHAR*)szExitQuestion, _countof(szExitQuestion));
					   if (MessageBox(NULL, szExitQuestion, szAppName, MB_YESNO) == IDYES)
					   {
						   SendMessage(hwnd, WM_DESTROY, 0, 0);
					   }
					   return 0;
	}
	case WM_ENDSESSION:
	case WM_DESTROY:
	{
					   TCHAR szWallpaper[MAX_PATH];
					   for (int i = 0; i < DESKTOPS; i++)
					   {
						   memset(szWallpaper, 0, sizeof (szWallpaper));
						   _stprintf(szWallpaper, szWallpaperTemplate, i);

						   s_Registry->Set(szWallpaper, (VOID*)windowsOnDesktop[i].szWallpaper, _tcslen(windowsOnDesktop[i].szWallpaper));
					   }
					   //Save plugin path in registry
					   s_Registry->Set(SZ_VD_PLUGIN_PATH_KEY, (VOID*)s_Plugin->GetFile(), _tcslen(s_Plugin->GetFile()));

					   s_Tray->Hide();

					   SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen(s_szOrginalWallpaper), s_szOrginalWallpaper, 0);

					   for (int i = 0; i < DESKTOPS; i++)
					   {
						   SetCurrentDesktop(i);
						   ShowWindows(windowsOnDesktop[GetCurrentDesktop()].table);
					   }

					   SetCurrentDesktop(0);

					   FreeLibrary(s_hSharedLib);

					   DeleteCriticalSection(&s_criticalSection);

					   PostQuitMessage(0);
					   return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
