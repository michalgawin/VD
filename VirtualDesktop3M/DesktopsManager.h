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
	t_vHWND m_vApps;
	TCHAR* m_szWallpaper;

public:
	CDesktop();
	CDesktop(const CDesktop& org);
	~CDesktop();

	CDesktop& operator=(const CDesktop& right);
	CDesktop& operator+=(CDesktop& right);
	CDesktop& operator+=(HWND hApp) { AddApp(hApp); return *this; }

	void SetWallpaper(TCHAR* szWallpaper);
	TCHAR* GetWallpaper() const { return m_szWallpaper; }

	void AddApp(HWND hApp) { if (IsWindow(hApp)) m_vApps.push_back(hApp); }
	t_vHWND& GetApps() { return m_vApps; }
	void ClearApps() { m_vApps.clear(); }

	/**
	 * Function get all windows from desktop
	 * @return number of applications
	 * @param hApp handle to this application window
	 */
	INT GetAppsFromDesktop(HWND hApp);
	/**
	* Function hide windows
	* @return number of hidden applications
	* @param hApp handle to this application window
	* @param update TRUE to update list of applications, FALSE otherwise
	*/
	INT HideApps(HWND hApp, BOOL update);
	/**
	 * Function show windows
	 * @return number of showed applications
	 */
	INT ShowApps();
};

HWND FindApplication(const TCHAR const * clsName);

typedef CDesktop* pCDesktop;
typedef std::vector<CDesktop> t_vCDesktop;
typedef t_vCDesktop::iterator t_vpCDesktopItor;
typedef t_vCDesktop::const_iterator t_const_vpCDesktopItor;

class CDesktopsManager
{
	t_vCDesktop m_vDesktops;

public:
	CDesktopsManager(int nDesktops);
	CDesktopsManager(const CDesktopsManager& org);
	~CDesktopsManager();
	INT GetDesktopsNumber() { return m_vDesktops.size(); }
	BOOL AddDesktop();
	BOOL RemoveDesktop(int nDesktop);
	CDesktop& operator[](INT nDesktop) throw(std::range_error);
};

typedef CDesktopsManager* pCDesktopsManager;

#endif //_WND_MGR_H_VD_
