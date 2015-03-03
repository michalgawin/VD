/**
* @author Micha³ Gawin
*/


#include "SamplePlugin.h"
#include "DesktopsManager.h"


class CDesktopButton
{
	HWND m_hButton;
	HBITMAP m_hBmp;
	INT m_cxImage, m_cyImage;

public:
	CDesktopButton(HWND hBtn = NULL);
	CDesktopButton& operator=(const CDesktopButton&);
	BOOL operator==(const CDesktopButton&);
	BOOL AddButton(HWND hBtn);
	void UpdateImage(TCHAR* szText = NULL);
	void Redraw();
	HWND GetButton() const { return m_hButton; }
	HBITMAP GetBitmap() const { return m_hBmp; }
	void RemoveBitmap() { DeleteObject(m_hBmp); m_hBmp = NULL; }

private:
	void AddText(TCHAR* szText);
	void AddScreenshot();
};