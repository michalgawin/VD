/**
 * @author Micha³ Gawin
 */


#ifndef _WND_MGR_H_VD_
#define _WND_MGR_H_VD_

#include "VirtualDesktop.h"


typedef std::vector<HWND> t_vHWND;
typedef t_vHWND::iterator t_vHWNDItor;
typedef t_vHWND::const_iterator t_const_vHWNDItor;

class CDesktop
{
	t_vHWND m_vApp;
	TCHAR* m_szWallpaper;

public:
	CDesktop();
	CDesktop(CDesktop& org);
	~CDesktop();
	CDesktop& operator=(CDesktop& right);
	void SetWallpaper(TCHAR* szWallpaper);
	TCHAR* GetWallpaper() { return m_szWallpaper; }
	void AddApp(HWND hApp) { m_vApp.push_back(hApp); }
	t_vHWND GetApps() { return m_vApp; }
	void ClearApps() { m_vApp.clear(); }
	static HWND FindApplication(const TCHAR const * clsName);

	/**
	 * Function get all windows from desktop
	 * @return number of applications
	 * @param hApp handle to this application window
	 */
	INT GetWindowsFromDesktop (HWND hApp);

	/**
	 * Function hide windows
	 * @return number of hidden applications
	 * @param hApp handle to this application window
	 * @param update TRUE to update list of applications, FALSE otherwise
	 */
	INT HideWindows (HWND hApp, BOOL update);

	/**
	 * Function show windows
	 * @return number of showed applications
	 */
	INT ShowWindows ();
};

typedef CDesktop* pCDesktop;

typedef std::vector<pCDesktop> t_vpCDesktop;
typedef t_vpCDesktop::iterator t_vpCDesktopItor;
typedef t_vpCDesktop::const_iterator t_const_vpCDesktopItor;

class CDesktopsManager
{
	t_vpCDesktop m_vpDesktop;

public:
	CDesktopsManager(int nDesktops);
	~CDesktopsManager();
	INT GetDesktopsNumber() { return m_vpDesktop.size(); }
	BOOL AddDesktop();
	BOOL RemoveDesktop(int nDesktop);
	pCDesktop operator[](INT nDesktop);
};

typedef CDesktopsManager* pCDesktopsManager;

#endif //_WND_MGR_H_VD_
