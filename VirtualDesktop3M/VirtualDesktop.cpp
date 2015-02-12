/**
* VirtualDesktop.cpp : Defines the entry point for the application.
* @author Micha³ Gawin
*/


#include "VirtualDesktop.h"
#include "Sys.h"
#include "Registry.h"
#include "Tray.h"
#include "WndMgr.h"

// Plugins
#include "WindowsManager.h"
#include "Plugin.h"
#include "About.h"

#include "DesktopMgr\DesktopMgr.h"			// header to shared dll


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

	if (CDesktop::FindApplication(szClassName))
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
	static HINSTANCE s_hInstance;	// instance to program
	static pCDesktopsManager s_pDskMgr;	// Struct keeps data for each desktop
	static HMODULE s_hSharedLib = NULL;	// handle to instance of shared dll
	static BOOL s_bAlwaysOnTop = TRUE;	// hold status of check field "Always on top"
	static CTray* s_Tray = NULL;
	static CRegistry* s_Registry = NULL;
	static CPlugin* s_Plugin = NULL;
	static TCHAR s_szWallValOrg[MAX_PATH];	// hold orginal desktop wallpaper
	TCHAR szWallNameTemplate[] = TEXT("Wallpaper#%d");	// wallpaper registry entry name template

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

					  s_pDskMgr = new CDesktopsManager(DESKTOPS);

					  memset(s_szWallValOrg, 0, _countof(s_szWallValOrg)*sizeof(TCHAR));
					  SystemParametersInfo(SPI_GETDESKWALLPAPER, _countof(s_szWallValOrg), s_szWallValOrg, 0);

					  s_Plugin = new CPlugin();
					  s_Registry = new CRegistry(SZ_VD_MAIN_KEY);

					  if (!s_Registry->Exists())	// create entry in registry
					  {
						  s_Registry->Create();

						  for (int i = 0; i < s_pDskMgr->GetDesktopsNumber(); i++)
						  {
							  TCHAR szWallpaperName[MAX_PATH];
							  memset(szWallpaperName, 0, sizeof (szWallpaperName));
							  _stprintf(szWallpaperName, szWallNameTemplate, i);

							  TCHAR szWallpaperValue[MAX_PATH];
							  memset(szWallpaperValue, 0, sizeof (szWallpaperValue));
							  _stprintf(szWallpaperValue, szWallNameTemplate, i);

							  if (SystemParametersInfo(SPI_GETDESKWALLPAPER, _countof(szWallpaperValue), szWallpaperValue, 0))
							  {
								  (*s_pDskMgr)[i]->SetWallpaper(szWallpaperValue);
								  s_Registry->Set(szWallpaperName, (VOID*)(*s_pDskMgr)[i]->GetWallpaper(), _tcslen((*s_pDskMgr)[i]->GetWallpaper()));
							  }
						  }

						  s_Registry->Set(SZ_VD_PLUGIN_PATH_KEY, (VOID*)SZ_DEFAULT_PLUGIN_NAME, _tcslen(SZ_DEFAULT_PLUGIN_NAME));
						  s_Plugin->SetFile(SZ_DEFAULT_PLUGIN_NAME);	// Set plugin path

						  if (!s_Registry->Exists(SZ_VD_PLUGIN_PATH_KEY)) s_Registry->Create(SZ_VD_PLUGIN_PATH_KEY);
					  }
					  else	// configuration already exists in registry 
					  {
						  for (int i = 0; i < s_pDskMgr->GetDesktopsNumber(); i++)
						  {
							  TCHAR szWallpaperName[MAX_PATH];
							  memset(szWallpaperName, 0, sizeof (szWallpaperName));
							  _stprintf(szWallpaperName, szWallNameTemplate, i);

							  TCHAR szWallpaperValue[MAX_PATH];
							  memset(szWallpaperValue, 0, sizeof (szWallpaperValue));
							  _stprintf(szWallpaperValue, szWallNameTemplate, i);

							  if (!s_Registry->Get(szWallpaperName, szWallpaperValue, MAX_PATH))
							  {
								  SystemParametersInfo(SPI_GETDESKWALLPAPER, _countof(szWallpaperValue), szWallpaperValue, 0);
							  }
							  (*s_pDskMgr)[i]->SetWallpaper(szWallpaperValue);
						  }

						  TCHAR szLibFullPath[MAX_PATH];
						  memset(szLibFullPath, 0, sizeof (TCHAR)*_countof(szLibFullPath));

						  if (s_Registry->Get(SZ_VD_PLUGIN_PATH_KEY, szLibFullPath, _countof(szLibFullPath))) s_Plugin->SetFile(szLibFullPath);	// Set plugin path
						  else s_Plugin->SetFile(SZ_DEFAULT_PLUGIN_NAME);

						  if (_tcslen((*s_pDskMgr)[GetCurrentDesktop()]->GetWallpaper()) > 0)
						  {
							  SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen((*s_pDskMgr)[GetCurrentDesktop()]->GetWallpaper()), (*s_pDskMgr)[GetCurrentDesktop()]->GetWallpaper(), 0);
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


																	 if (desk < 0 || desk >= s_pDskMgr->GetDesktopsNumber())
																	 {
																		 desk = (desk + s_pDskMgr->GetDesktopsNumber()) % s_pDskMgr->GetDesktopsNumber();
																	 }

																	 ChangeDesktop(desk);
																	 break;
									 }
									 case MOD_CONTROL:
									 {
														 int desk = GetCurrentDesktop();
														 desk++;

														 desk = desk % s_pDskMgr->GetDesktopsNumber();

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

								if ((lParam >= 0) && (lParam < s_pDskMgr->GetDesktopsNumber()))
								{
									SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen((*s_pDskMgr)[lParam]->GetWallpaper()), (*s_pDskMgr)[lParam]->GetWallpaper(), 0);
									(*s_pDskMgr)[GetCurrentDesktop()]->HideWindows(hwnd, TRUE);
									// WARNING!!! Work only if icons in resources are in ascending sequence!
									s_Tray->ChangeIcon(LoadIcon(s_hInstance, MAKEINTRESOURCE(IDI_ICON + lParam)));

									SetCurrentDesktop(lParam);

									(*s_pDskMgr)[GetCurrentDesktop()]->ShowWindows();
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
										  (*s_pDskMgr)[GetCurrentDesktop()]->GetWindowsFromDesktop(hwnd);

										  CDesktop AppMgr;
										  AppMgr.SetWallpaper((*s_pDskMgr)[GetCurrentDesktop()]->GetWallpaper());
										  AppMgr = *(*s_pDskMgr)[GetCurrentDesktop()];

										  if (DialogBoxParam(s_hInstance, MAKEINTRESOURCE(IDD_DESKTOP_MANAGER), hwnd, DlgDesktopManagerProc, (LPARAM)s_pDskMgr))
										  {
											  AppMgr.HideWindows(hwnd, FALSE);
											  (*s_pDskMgr)[GetCurrentDesktop()]->ShowWindows();
											  SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen((*s_pDskMgr)[GetCurrentDesktop()]->GetWallpaper()), (*s_pDskMgr)[GetCurrentDesktop()]->GetWallpaper(), 0);
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
					   TCHAR szWallpaperName[MAX_PATH];
					   for (int i = 0; i < s_pDskMgr->GetDesktopsNumber(); i++)
					   {
						   memset(szWallpaperName, 0, sizeof (szWallpaperName));
						   _stprintf(szWallpaperName, szWallNameTemplate, i);

						   s_Registry->Set(szWallpaperName, (VOID*)(*s_pDskMgr)[i]->GetWallpaper(), _tcslen((*s_pDskMgr)[i]->GetWallpaper()));
					   }
					   //Save plugin path in registry
					   s_Registry->Set(SZ_VD_PLUGIN_PATH_KEY, (VOID*)s_Plugin->GetFile(), _tcslen(s_Plugin->GetFile()));

					   s_Tray->Hide();

					   SystemParametersInfo(SPI_SETDESKWALLPAPER, _tcslen(s_szWallValOrg), s_szWallValOrg, 0);

					   for (int i = 0; i < s_pDskMgr->GetDesktopsNumber(); i++)
					   {
						   SetCurrentDesktop(i);
						   (*s_pDskMgr)[GetCurrentDesktop()]->ShowWindows();
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
