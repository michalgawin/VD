/**
* @author Micha³ Gawin
*/


#include "DesktopsManager.h"
#include <algorithm>


CDesktopsManager::CDesktopsManager(int nDesktops)
{
	m_vDesktops.resize(nDesktops);
}


CDesktopsManager::CDesktopsManager(CDesktopsManager& org)
{
	m_vDesktops.resize(org.m_vDesktops.size());
	std::copy(org.m_vDesktops.begin(), org.m_vDesktops.end(), m_vDesktops.begin());
}


CDesktopsManager::~CDesktopsManager()
{
	m_vDesktops.clear();
}


BOOL CDesktopsManager::AddDesktop()
{
	m_vDesktops.push_back(CDesktop());
	return TRUE;
}


BOOL CDesktopsManager::RemoveDesktop(int nDesktop)
{
	m_vDesktops.erase(m_vDesktops.begin() + nDesktop);
	return TRUE;
}


CDesktop& CDesktopsManager::operator[](INT nDesktop)
{
	if ((nDesktop < 0) && (nDesktop >= m_vDesktops.size()))
		throw std::range_error("CDesktopManager: index of CDesktop is out of range");
	return m_vDesktops[nDesktop];
}


CDesktop::CDesktop() : m_szWallpaper(NULL)
{
	m_vApps.clear();
}


CDesktop::CDesktop(CDesktop& org)
{
	m_vApps.resize(org.m_vApps.size());
	std::copy_if(org.m_vApps.begin(), org.m_vApps.end(), m_vApps.begin(), IsWindow);
	SetWallpaper(org.GetWallpaper());
}


CDesktop::~CDesktop()
{
	m_vApps.clear();

	if (m_szWallpaper) delete[] m_szWallpaper;
	m_szWallpaper = NULL;
}


CDesktop& CDesktop::operator = (CDesktop& right)
{
	if (this != &right)
	{
		m_vApps.resize(right.m_vApps.size());
		std::copy_if(right.m_vApps.begin(), right.m_vApps.end(), m_vApps.begin(), IsWindow);
		SetWallpaper(right.GetWallpaper());
	}

	return *this;
}


CDesktop& CDesktop::operator+(CDesktop& right)
{
	t_vHWND apps(m_vApps.size() + right.m_vApps.size());
	std::merge(m_vApps.begin(), m_vApps.end(), right.m_vApps.begin(), right.m_vApps.end(), apps.begin());
	m_vApps = apps;

	return *this;
}


void CDesktop::SetWallpaper(TCHAR* szWallpaper)
{
	if (m_szWallpaper) delete[] m_szWallpaper;
	m_szWallpaper = NULL;

	INT iLen = _tcslen(szWallpaper) + 1;
	m_szWallpaper = new TCHAR[iLen];
	_tcscpy_s(m_szWallpaper, iLen, szWallpaper);
}


HWND CDesktop::FindApplication(const TCHAR const * clsName)
{
	return FindWindowEx(NULL, NULL, clsName, NULL);
}


INT CDesktop::GetWindowsFromDesktop(HWND hApp)
{
	HWND hDesktop = GetDesktopWindow();
	HWND hTaskBar = FindWindowEx(NULL, NULL, TEXT("Shell_TrayWnd"), NULL);
	HWND hDesktopIcon = FindWindowEx(NULL, NULL, TEXT("Progman"), NULL);	// icons "My Computer" etc.

	m_vApps.clear();

	for (HWND hWindowTop = GetTopWindow(hDesktop); hWindowTop; hWindowTop = GetWindow(hWindowTop, GW_HWNDNEXT))
	{
		if (IsWindowVisible(hWindowTop) && GetParent(hWindowTop) != hApp && hWindowTop != hTaskBar && hWindowTop != hDesktop && hWindowTop != hDesktopIcon && hWindowTop != hApp && GetParent(hWindowTop) != hTaskBar)
			m_vApps.push_back(hWindowTop);
	}

	return m_vApps.size();
}


INT CDesktop::HideWindows(HWND hApp, BOOL update)
{
	if (update) GetWindowsFromDesktop(hApp);

	HDWP s = BeginDeferWindowPos(m_vApps.size());
	for (t_vHWNDItor itor = m_vApps.begin(); itor != m_vApps.end(); itor++)
	{
		s = DeferWindowPos(s, *itor, 0, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	EndDeferWindowPos(s);

	return m_vApps.size();
}


INT CDesktop::ShowWindows()
{
	for (t_vHWNDItor itor = m_vApps.begin(); itor != m_vApps.end();)
	{
		//If window not exists (e.g. process was closed from task manager) remove handle from table
		if (!IsWindow(*itor))
		{
			itor = m_vApps.erase(itor++);
		}
		else
			itor++;
	}

	HDWP s = BeginDeferWindowPos(m_vApps.size());
	for (t_vHWNDItor itor = m_vApps.begin(); itor != m_vApps.end(); itor++)
	{
		s = DeferWindowPos(s, *itor, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	}
	EndDeferWindowPos(s);

	return m_vApps.size();
}
