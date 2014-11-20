/**
 * @author Micha³ Gawin
 */


#include "VirtualDesktop.h"
#include "WindowsManager.h"


static pWindowsOnDesktop pWOD = NULL;
static HINSTANCE hInstance;


VOID PopupMenuTree (HWND hwnd, BOOL copy, BOOL cut, BOOL paste, BOOL del)
{
	hInstance = (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE);
	static HMENU menu = NULL;
	POINT mouse;

	if (menu)
	{
		DestroyMenu (menu);
	}

	menu = CreatePopupMenu ();
	if (!menu)
	{
		return;
	}

	DWORD flag = MF_BYPOSITION | MF_STRING;

	TCHAR szCopy[MAX_PATH];
	LoadString (hInstance, IDS_COPY, (TCHAR*) szCopy, sizeof (szCopy));

	AppendMenu (menu, copy ? flag : flag | MF_GRAYED, CMD_TREE_COPY, (LPCTSTR) szCopy);

	TCHAR szCut[MAX_PATH];
	LoadString (hInstance, IDS_CUT, (TCHAR*) szCut, sizeof (szCut));

	AppendMenu (menu, cut ? flag : flag | MF_GRAYED, CMD_TREE_CUT, (LPCTSTR) szCut);

	TCHAR szPaste[MAX_PATH];
	LoadString (hInstance, IDS_PASTE, (TCHAR*) szPaste, sizeof (szPaste));

	AppendMenu (menu, paste ? flag : flag | MF_GRAYED, CMD_TREE_PASTE, (LPCTSTR) szPaste);

	TCHAR szDelete[MAX_PATH];
	LoadString (hInstance, IDS_DELETE, (TCHAR*) szDelete, sizeof (szDelete));

	AppendMenu (menu, del ? flag : flag | MF_GRAYED, CMD_TREE_DELETE, (LPCTSTR) szDelete);

	GetCursorPos (&mouse);
	SetForegroundWindow (hwnd);
	TrackPopupMenu (menu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, mouse.x, mouse.y, 0, hwnd, 0);
	PostMessage (hwnd, WM_NULL, 0, 0);
}


VOID DrawTree (HWND hTree)
{
	TVITEM		tvi;
	TVINSERTSTRUCT	tvis;

	for (int i = 0; i < DESKTOPS; i++)
	{
		TCHAR szDesktopNum[MAX_PATH];
		LoadString (hInstance, IDS_DEF_DESKTOP_PREVIEW, (TCHAR*) szDesktopNum, sizeof (szDesktopNum));

		TCHAR szBuffer[MAX_PATH];
		memset (szBuffer, 0, sizeof (szBuffer));
		_stprintf (szBuffer, szDesktopNum, i);

		tvi.mask = TVIF_TEXT;
		tvi.pszText = szBuffer;
		tvis.hParent = tvis.hInsertAfter = TVI_ROOT;
		tvis.item = tvi;
		HTREEITEM hItem = TreeView_InsertItem (hTree, &tvis);

		for (vHandleItor itor = pWOD[i].table.begin(); itor != pWOD[i].table.end();)
		{
			if (!IsWindow (*itor))
			{
				itor = pWOD[i].table.erase(itor++);
				continue;
			}

			memset (szBuffer, 0, sizeof (szBuffer));
			int len = 0;

			len = GetWindowText (*itor, szBuffer, MAX_PATH);

			if (_tcslen ((const TCHAR *) szBuffer) <= 0)
			{
				TCHAR szAppNoName[MAX_PATH];
				LoadString (hInstance, IDS_APP_NO_NAME, (TCHAR*) szAppNoName, sizeof (szAppNoName));

				memset (szBuffer, 0, sizeof (szBuffer));
				_stprintf (szBuffer, szAppNoName, (DWORD) *itor);
			}

			tvi.pszText = szBuffer;
			tvi.mask = TVIF_TEXT | TVIF_PARAM;
			tvi.lParam = (LPARAM) *itor;
			tvis.item = tvi;
			tvis.hParent = hItem;
			tvis.hInsertAfter = TVI_SORT;
			HTREEITEM hItemChild = TreeView_InsertItem(hTree, &tvis);

			itor++;
		}
	}
}


BOOL CALLBACK DlgDesktopManagerProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND	hTree = NULL;
	static HWND hCopy = NULL;
	static HTREEITEM htiParent = NULL;
	static HTREEITEM hti2Delete = NULL;
	static BOOL bCut = FALSE;

	switch (message)
	{
	case WM_INITDIALOG:
		{
		pWOD = (pWindowsOnDesktop) lParam;

		hTree = GetDlgItem (hDlg, IDC_TREE);
		DrawTree (hTree);
		return TRUE;
		}
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case CMD_TREE_CUT:
			bCut = TRUE;
		case CMD_TREE_COPY:
			{
			HTREEITEM hCurrentItem = TreeView_GetSelection (hTree);
			TreeView_SelectItem (hTree, hCurrentItem);

			hti2Delete = hCurrentItem;

			htiParent = TreeView_GetParent (hTree, hCurrentItem);
			if (!htiParent)
			{
				htiParent = hCurrentItem;
				return FALSE;
			}

			TVITEM tvi;
			tvi.mask = TVIF_HANDLE;
			tvi.hItem = hCurrentItem;
			TreeView_GetItem (hTree, &tvi);
			hCopy = (HWND) tvi.lParam;
			return TRUE;
			}
		case CMD_TREE_PASTE:
			{
			if (hCopy == NULL)
			{
				return FALSE;
			}

			HTREEITEM hCurrentItem = TreeView_GetSelection (hTree);
			if (!hCurrentItem)
			{
				return FALSE;
			}
			TreeView_SelectItem (hTree, hCurrentItem);

			TCHAR szBuffer[MAX_PATH];
			memset (szBuffer, 0, sizeof (MAX_PATH));

			int len = GetWindowText (hCopy, szBuffer, MAX_PATH);
			if (len <= 0)
			{
				TCHAR szAppNoName[MAX_PATH];
				LoadString (hInstance, IDS_APP_NO_NAME, (TCHAR*) szAppNoName, sizeof (szAppNoName));

				memset (szBuffer, 0, sizeof (MAX_PATH));
				_stprintf (szBuffer, szAppNoName, (DWORD) hCopy);
			}

			TVITEM		tvi;
			TVINSERTSTRUCT	tvis;

			HTREEITEM hParent = TreeView_GetParent (hTree, hCurrentItem);
			if (hParent)
			{
				tvis.hParent = hParent;
			}
			else
			{
				tvis.hParent = hCurrentItem;
			}

			if (hParent == htiParent)
			{
				return FALSE;
			}

			tvi.pszText = szBuffer;
			tvi.mask = TVIF_TEXT | TVIF_PARAM;
			tvi.lParam = (LPARAM) hCopy;
			tvis.item = tvi;
			tvis.hInsertAfter = TVI_SORT;
			HTREEITEM hItemChild = TreeView_InsertItem(hTree, &tvis);

			hCopy = NULL;
			htiParent = NULL;

			if (bCut)
			{
				TreeView_DeleteItem (hTree, hti2Delete);
				hti2Delete = NULL;
				bCut = FALSE;
			}
			return TRUE;
			}
		case CMD_TREE_DELETE:
			{
			HTREEITEM hCurrentItem = TreeView_GetSelection (hTree);
			TreeView_SelectItem (hTree, hCurrentItem);
			TVITEM tvi;
			tvi.mask = TVIF_HANDLE;
			tvi.hItem = hCurrentItem;
			TreeView_GetItem (hTree, &tvi);

			for (int i = 0; i < DESKTOPS; i++)
			{
				for (vHandleItor itor = pWOD[i].table.begin(); itor != pWOD[i].table.end(); itor++)
				{
					if (((HWND) tvi.lParam) == *itor)
					{
						TCHAR szAppLastInstance[MAX_PATH];
						LoadString (hInstance, IDS_APP_LAST_INSTANCE, (TCHAR*) szAppLastInstance, sizeof (szAppLastInstance));

						TCHAR szAppName[MAX_PATH];
						LoadString (hInstance, IDS_APP_NAME, (TCHAR*) szAppName, sizeof (szAppName));

						if (MessageBox (hDlg,
										szAppLastInstance, szAppName,
										MB_ICONWARNING | MB_YESNOCANCEL) == IDYES)
							TreeView_DeleteItem (hTree, hCurrentItem);
					}
				}
			}
			return TRUE;
			}
		case IDOK:
			{
			int i = 0;
			HTREEITEM hCurrentRoot = TreeView_GetRoot (hTree);
			do
			{
				if (!hCurrentRoot)
				{
					return FALSE;
				}
				HTREEITEM hCurrentChild = TreeView_GetChild (hTree, hCurrentRoot);
				pWOD[i].table.clear();
				if (hCurrentChild)
				{
					do
					{
						TVITEM tvi;
						tvi.mask = TVIF_HANDLE;
						tvi.hItem = hCurrentChild;
						TreeView_GetItem (hTree, &tvi);
						pWOD[i].table.push_back((HWND) tvi.lParam);
					} while (hCurrentChild = TreeView_GetNextItem (hTree, hCurrentChild, TVGN_NEXT));
				}
				i++;
			} while (hCurrentRoot = TreeView_GetNextItem (hTree, hCurrentRoot, TVGN_NEXT));

			EndDialog (hDlg, TRUE);
			return TRUE;
			}
		case IDCANCEL:
			EndDialog (hDlg, FALSE);
			return TRUE;
		}
	case WM_NOTIFY:
		{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_RCLICK:
			{
			HTREEITEM hCurrentItem = TreeView_GetSelection (hTree);
			if (!hCurrentItem)
			{
				return FALSE;
			}
			TreeView_SelectItem (hTree, hCurrentItem);

			if (!TreeView_GetParent (hTree, hCurrentItem))
			{
				PopupMenuTree (hDlg, FALSE, FALSE, (BOOL) hCopy, FALSE);
			}
			else
			{
				PopupMenuTree (hDlg, TRUE, TRUE, (BOOL) hCopy, TRUE);
			}
			return TRUE;
			}
		}
		}
	}
	return FALSE;
}
