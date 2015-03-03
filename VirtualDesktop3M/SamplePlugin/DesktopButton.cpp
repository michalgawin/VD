/**
* @author Micha³ Gawin
*/

#include "DesktopButton.h"


CDesktopButton::CDesktopButton(HWND hBtn)
{
	m_hButton = hBtn;
	m_hBmp = NULL;

	RECT rc = { 0, 0, 0, 0 };
	GetWindowRect(hBtn, &rc);

	m_cxImage = rc.right - rc.left;
	m_cyImage = rc.bottom - rc.top;
}


CDesktopButton& CDesktopButton::operator = (const CDesktopButton& right)
{
	m_hButton = right.m_hButton;
	//m_hBmp = right.m_hBmp;
	m_cxImage = right.m_cxImage;
	m_cyImage = right.m_cyImage;

	return *this;
}


BOOL CDesktopButton::operator==(const CDesktopButton& right)
{
	return m_hButton == right.m_hButton;
}


BOOL CDesktopButton::AddButton(HWND hBtn)
{
	if (m_hButton) DestroyWindow(m_hButton);
	m_hButton = hBtn;
	m_hBmp = NULL;

	return TRUE;
}


void CDesktopButton::UpdateImage(TCHAR* szText)
{
	if (szText) AddText(szText);
	else AddScreenshot();
}


void CDesktopButton::AddText(TCHAR* szText)
{
	HDC btnDC = GetWindowDC(m_hButton);

	SIZE size;
	GetTextExtentPoint32(btnDC, szText, _tcslen(szText), &size);

	HDC memDC = CreateCompatibleDC(btnDC);
	m_hBmp = CreateCompatibleBitmap(btnDC, size.cx, size.cy);

	HDC oldMemDC = (HDC)SelectObject(memDC, m_hBmp);
	TextOut(memDC, 0, 0, szText, _tcslen(szText));
	SelectObject(memDC, oldMemDC);

	DeleteDC(memDC);
	memDC = NULL;

	ReleaseDC(m_hButton, btnDC);
	btnDC = NULL;
}


void CDesktopButton::AddScreenshot()
{
	BITMAP bmp;
	GetObject(m_hBmp, sizeof (BITMAP), &bmp);

	HDC dcDesktop = GetWindowDC(NULL);

	HDC memDC = CreateCompatibleDC(dcDesktop);

	DeleteObject(m_hBmp);
	m_hBmp = CreateCompatibleBitmap(dcDesktop, m_cxImage, m_cyImage);

	HDC oldMemDC = (HDC)SelectObject(memDC, m_hBmp);
	StretchBlt(memDC, 0, 0, m_cxImage, m_cyImage, dcDesktop, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SRCCOPY);
	SelectObject(memDC, oldMemDC);

	DeleteDC(memDC);
	memDC = NULL;

	ReleaseDC(NULL, dcDesktop);
	dcDesktop = NULL;
}


void CDesktopButton::Redraw()
{
	BITMAP bmp;
	GetObject(m_hBmp, sizeof (BITMAP), &bmp);

	HDC dcButton = GetWindowDC(m_hButton);

	HDC memDC = CreateCompatibleDC(dcButton);

	HDC oldMemDC = (HDC)SelectObject(memDC, m_hBmp);
	StretchBlt(dcButton, 0, 0, m_cxImage, m_cyImage, memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	SelectObject(memDC, oldMemDC);

	DeleteDC(memDC);
	memDC = NULL;

	DeleteDC(dcButton);
	dcButton = NULL;
}
