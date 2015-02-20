/**
* @author Micha³ Gawin
*/


#include "WndMgr.h"
#include <algorithm>


CDesktopsManager::CDesktopsManager(int nDesktops)
{
	m_vpDesktop.resize(nDesktops);
	std::generate(m_vpDesktop.begin(), m_vpDesktop.end(), []() { return new CDesktop(); });
}


CDesktopsManager::~CDesktopsManager()
{
	std::for_each(m_vpDesktop.begin(), m_vpDesktop.end(), [](pCDesktop pDsk) { delete pDsk; });
	m_vpDesktop.clear();
}


BOOL CDesktopsManager::AddDesktop()
{
	m_vpDesktop.push_back(new CDesktop());
	return TRUE;
}


BOOL CDesktopsManager::RemoveDesktop(int nDesktop)
{
	delete m_vpDesktop[nDesktop];
	m_vpDesktop.erase(m_vpDesktop.begin() + nDesktop);
	return TRUE;
}


pCDesktop CDesktopsManager::operator[](INT nDesktop)
{
	if ((nDesktop >= 0) && (nDesktop < m_vpDesktop.size()))
		return m_vpDesktop[nDesktop];
	else
		return NULL;
}


CDesktop::CDesktop() : m_szWallpaper(NULL)
{
	m_vApp.clear();
}


CDesktop::CDesktop(CDesktop& org)
{
	m_vApp.resize(org.m_vApp.size());
	std::copy_if(org.m_vApp.begin(), org.m_vApp.end(), m_vApp.begin(), IsWindow);
	SetWallpaper(org.GetWallpaper());
}


CDesktop::~CDesktop()
{
	m_vApp.clear();

	if (m_szWallpaper) delete[] m_szWallpaper;
	m_szWallpaper = NULL;
}


CDesktop& CDesktop::operator = (CDesktop& right)
{
	if (this != &right)
	{
		m_vApp.resize(right.m_vApp.size());
		std::copy_if(right.m_vApp.begin(), right.m_vApp.end(), m_vApp.begin(), IsWindow);
		SetWallpaper(right.GetWallpaper());
	}

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

	m_vApp.clear();

	for (HWND hWindowTop = GetTopWindow(hDesktop); hWindowTop; hWindowTop = GetWindow(hWindowTop, GW_HWNDNEXT))
	{
		if (IsWindowVisible(hWindowTop) && GetParent(hWindowTop) != hApp && hWindowTop != hTaskBar && hWindowTop != hDesktop && hWindowTop != hDesktopIcon && hWindowTop != hApp && GetParent(hWindowTop) != hTaskBar)
			m_vApp.push_back(hWindowTop);
	}

	return m_vApp.size();
}


INT CDesktop::HideWindows(HWND hApp, BOOL update)
{
	if (update) GetWindowsFromDesktop(hApp);

	HDWP s = BeginDeferWindowPos(m_vApp.size());
	for (t_vHWNDItor itor = m_vApp.begin(); itor != m_vApp.end(); itor++)
	{
		s = DeferWindowPos(s, *itor, 0, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	EndDeferWindowPos(s);

	return m_vApp.size();
}


INT CDesktop::ShowWindows()
{
	for (t_vHWNDItor itor = m_vApp.begin(); itor != m_vApp.end();)
	{
		//If window not exists (e.g. process was closed from task manager) remove handle from table
		if (!IsWindow(*itor))
		{
			itor = m_vApp.erase(itor++);
		}
		else
			itor++;
	}

	HDWP s = BeginDeferWindowPos(m_vApp.size());
	for (t_vHWNDItor itor = m_vApp.begin(); itor != m_vApp.end(); itor++)
	{
		s = DeferWindowPos(s, *itor, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	}
	EndDeferWindowPos(s);

	return m_vApp.size();
}
