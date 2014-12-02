/**
* @author Micha³ Gawin
*/


#include "VirtualDesktop.h"
#include "WindowsManager.h"
#include "WndMgr.h"
#include "DesktopMgr\DesktopMgr.h"
#include <windowsx.h>
#include <Commctrl.h>


static pWindowsOnDesktop pWOD = NULL;
static HINSTANCE hInstance;


VOID ShowPopupMenu(HWND hwnd, BOOL copy, BOOL cut, BOOL paste, BOOL del)
{
	static HMENU menu = NULL;
	hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	POINT mouse;

	if (menu)
	{
		DestroyMenu(menu);
	}

	menu = CreatePopupMenu();
	if (!menu)
	{
		DWORD err = GetLastError();

		TCHAR szErrPopupMenu[MAX_PATH];
		memset(szErrPopupMenu, 0, sizeof (szErrPopupMenu));
		LoadString(hInstance, IDS_ERR_POPUP_MENU, szErrPopupMenu, sizeof(szErrPopupMenu) / sizeof(TCHAR));

		TCHAR szErr[MAX_PATH];
		memset(szErr, 0, sizeof (szErr));
		LoadString(hInstance, IDS_ERROR, szErr, sizeof(szErr) / sizeof(TCHAR));

		TCHAR szMsg[MAX_PATH];
		_stprintf(szMsg, szErrPopupMenu, err);

		MessageBox(NULL, szMsg, szErr, MB_OK | MB_ICONERROR);
	}
	else
	{
		DWORD flag = MF_BYPOSITION | MF_STRING;

		TCHAR szCopy[MAX_PATH];
		LoadString(hInstance, IDS_COPY, (TCHAR*)szCopy, sizeof(szCopy) / sizeof(TCHAR));

		AppendMenu(menu, copy ? flag : flag | MF_GRAYED, CMD_TREE_COPY, (LPCTSTR)szCopy);
		
		TCHAR szCut[MAX_PATH];
		LoadString (hInstance, IDS_CUT, (TCHAR*) szCut, sizeof(szCut) / sizeof(TCHAR));

		AppendMenu (menu, cut ? flag : flag | MF_GRAYED, CMD_TREE_CUT, (LPCTSTR) szCut);
		
		TCHAR szPaste[MAX_PATH];
		LoadString(hInstance, IDS_PASTE, (TCHAR*)szPaste, sizeof(szPaste) / sizeof(TCHAR));

		AppendMenu(menu, paste ? flag : flag | MF_GRAYED, CMD_TREE_PASTE, (LPCTSTR)szPaste);

		TCHAR szDelete[MAX_PATH];
		LoadString(hInstance, IDS_DELETE, (TCHAR*)szDelete, sizeof(szDelete) / sizeof(TCHAR));

		AppendMenu(menu, del ? flag : flag | MF_GRAYED, CMD_TREE_DELETE, (LPCTSTR)szDelete);

		GetCursorPos(&mouse);
		SetForegroundWindow(hwnd);
		TrackPopupMenu(menu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, mouse.x, mouse.y, 0, hwnd, 0);
		PostMessage(hwnd, WM_NULL, 0, 0);
	}
}


VOID DrawTree(HWND hTree)
{
	TVITEM		tvi;
	TVINSERTSTRUCT	tvis;

	for (int i = 0; i < DESKTOPS; i++)
	{
		TCHAR szDesktopNum[MAX_PATH];
		LoadString(hInstance, IDS_DEF_DESKTOP_PREVIEW, (TCHAR*)szDesktopNum, sizeof(szDesktopNum) / sizeof(TCHAR));

		TCHAR szBuffer[MAX_PATH];
		memset(szBuffer, 0, sizeof (szBuffer));
		_stprintf(szBuffer, szDesktopNum, i);

		tvi.mask = TVIF_TEXT;
		tvi.pszText = szBuffer;
		tvis.hParent = tvis.hInsertAfter = TVI_ROOT;
		tvis.item = tvi;
		HTREEITEM hItem = TreeView_InsertItem(hTree, &tvis);

		for (vHandleItor itor = pWOD[i].table.begin(); itor != pWOD[i].table.end();)
		{
			if (!IsWindow(*itor))
			{
				itor = pWOD[i].table.erase(itor++);
			}
			else
			{
				memset(szBuffer, 0, sizeof (szBuffer));
				int len = 0;

				len = GetWindowText(*itor, szBuffer, MAX_PATH);

				if (_tcslen((const TCHAR *)szBuffer) <= 0)	// window without name
				{
					TCHAR szAppNoName[MAX_PATH];
					LoadString(hInstance, IDS_APP_NO_NAME, (TCHAR*)szAppNoName, sizeof(szAppNoName) / sizeof(TCHAR));

					memset(szBuffer, 0, sizeof (szBuffer));
					_stprintf(szBuffer, szAppNoName, (DWORD)*itor);
				}

				tvi.pszText = szBuffer;
				tvi.mask = TVIF_TEXT | TVIF_PARAM;
				tvi.lParam = (LPARAM)*itor;
				tvis.item = tvi;
				tvis.hParent = hItem;
				tvis.hInsertAfter = TVI_SORT;
				HTREEITEM hItemChild = TreeView_InsertItem(hTree, &tvis);

				itor++;
			}
		}
	}
}


INT FindItem(HWND hTreeView, HWND hWindow, HTREEITEM htiParent=NULL)
{
	INT found = 0;
	for (HTREEITEM hCurrentRoot = TreeView_GetRoot(hTreeView); (!found || !htiParent) && hCurrentRoot; hCurrentRoot = TreeView_GetNextItem(hTreeView, hCurrentRoot, TVGN_NEXT))
	{
		if (!htiParent || (htiParent == hCurrentRoot))
		{
			for (HTREEITEM hCurrentChild = TreeView_GetChild(hTreeView, hCurrentRoot); (!found || !htiParent) && hCurrentChild; hCurrentChild = TreeView_GetNextItem(hTreeView, hCurrentChild, TVGN_NEXT))
			{
				TVITEM tvi;
				tvi.mask = TVIF_HANDLE;
				tvi.hItem = hCurrentChild;
				TreeView_GetItem(hTreeView, &tvi);

				if ((HWND)tvi.lParam == hWindow)
					found += 1;
			}
		}
	}

	return found;
}


BOOL CALLBACK DlgDesktopManagerProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND	hTree = NULL;
	static HWND hCopy = NULL;				//handle to window to copy
	static BOOL bCut = FALSE;
	static HWND hDelete = NULL;				//handle to window to remove
	static HTREEITEM htiDrag = NULL;		//refers to dragged item in TreeView
	static HTREEITEM htiSelected = NULL;	//refers to selected item in TreeView
	static HTREEITEM htiCopy = NULL;		//refers to copied item in TreeView
	static BOOL bDragging = FALSE;			//inform about drag-and-drop operation

	switch (message)
	{
	case WM_INITDIALOG:
	{
						  pWOD = (pWindowsOnDesktop)lParam;

						  hTree = GetDlgItem(hDlg, IDC_TREE);
						  DrawTree(hTree);
						  return TRUE;
	}
	case WM_MOUSEMOVE:
	{
						 if (bDragging)
						 {
							 HTREEITEM htiTarget;  // Handle to target item. 
							 TVHITTESTINFO tvht;   // Hit test information. 
							 POINT point;

							 point.x = GET_X_LPARAM(lParam);
							 point.y = GET_Y_LPARAM(lParam);
							 ClientToScreen(hDlg, &point);
							 ScreenToClient(hTree, &point);
							 ImageList_DragMove(point.x, point.y);

							 // Turn off the dragged image so the background can be refreshed.
							 ImageList_DragShowNolock(FALSE);

							 tvht.pt.x = point.x;
							 tvht.pt.y = point.y;
							 if ((htiTarget = TreeView_HitTest(hTree, &tvht)) != NULL)
							 {
								 TreeView_SelectDropTarget(hTree, htiTarget);
							 }
							 ImageList_DragShowNolock(TRUE);
						 }
						 return TRUE;
	}
	case WM_LBUTTONUP:
	{
						 if (bDragging)
						 {
							 HTREEITEM htiDest = NULL;  //refers to destination item
							 htiDest = TreeView_GetDropHilight(hTree);
							 if (htiDest != NULL)
							 {
								 HTREEITEM htiParent = TreeView_GetParent(hTree, htiDest);
								 if (htiParent)
								 {
									 htiDest = htiParent;
								 }

								 TVITEM		tvi;
								 TVINSERTSTRUCT	tvis;
								 TCHAR szBuffer[MAX_PATH];
								 memset(szBuffer, 0, sizeof(szBuffer));

								 tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_HANDLE;
								 tvi.pszText = szBuffer;
								 tvi.cchTextMax = sizeof (szBuffer) / sizeof (TCHAR);
								 tvi.hItem = htiDrag;

								 TreeView_GetItem(hTree, &tvi);

								 tvis.item = tvi;
								 tvis.hParent = htiDest;
								 tvis.hInsertAfter = TVI_SORT;
								 TreeView_InsertItem(hTree, &tvis);

								 ShowWindow((HWND)tvi.lParam, SW_HIDE);
								 UpdateWindow((HWND)tvi.lParam);
								 TreeView_DeleteItem(hTree, htiDrag);
							 }
							 ImageList_EndDrag();
							 TreeView_SelectDropTarget(hTree, NULL);

							 ReleaseCapture();
							 ShowCursor(TRUE);
							 bDragging = FALSE;
						 }
						 return TRUE;
	}
	case WM_COMMAND:
	{
					   switch (LOWORD(wParam))
					   {
					   case CMD_TREE_CUT:
					   {
											bCut = TRUE;
					   }
					   case CMD_TREE_COPY:
					   {
											 if (TreeView_GetSelection(hTree) && htiSelected)
											 {
												 if (TreeView_GetParent(hTree, htiSelected))
												 {
													 htiCopy = htiSelected;
													 TVITEM tvi;

													 tvi.mask = TVIF_HANDLE;
													 tvi.hItem = htiCopy;
													 TreeView_GetItem(hTree, &tvi);

													 hCopy = (HWND)tvi.lParam;
													 return TRUE;
												 }
											 }
											 return FALSE;
					   }
					   case CMD_TREE_PASTE:
					   {
											  if (hCopy && htiCopy && TreeView_GetSelection(hTree) && htiSelected)
											  {
												  HTREEITEM htiPaste = htiSelected;
												  TVITEM		tvi;
												  TCHAR szBuffer[MAX_PATH];
												  memset(szBuffer, 0, sizeof (szBuffer));

												  tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
												  tvi.hItem = htiCopy;
												  tvi.pszText = szBuffer;
												  tvi.cchTextMax = sizeof (szBuffer) / sizeof (TCHAR);
												  tvi.lParam = (LPARAM)hCopy;
												  TreeView_GetItem(hTree, &tvi);

												  TVINSERTSTRUCT	tvis;

												  HTREEITEM hParent = TreeView_GetParent(hTree, htiPaste);
												  tvis.hParent = hParent ? hParent : htiPaste;
												  tvi.mask = TVIF_TEXT | TVIF_PARAM;
												  tvis.item = tvi;
												  tvis.hInsertAfter = TVI_SORT;

												  if (!FindItem(hTree, (HWND)tvi.lParam, tvis.hParent))
												  {
													  TreeView_InsertItem(hTree, &tvis);

													  if (bCut)
													  {
														  TreeView_DeleteItem(hTree, htiCopy);
														  hDelete = (HWND)hCopy;
													  }
												  }
												  bCut = FALSE;
											  }
											  return TRUE;
					   }
					   case CMD_TREE_DELETE:
					   {
											   if (htiSelected)
											   {
												   HTREEITEM htiDelete = htiSelected;
												   TVITEM tvi;
												   tvi.mask = TVIF_HANDLE | TVIF_PARAM;
												   tvi.hItem = htiDelete;
												   TreeView_GetItem(hTree, &tvi);

												   int count = FindItem(hTree, (HWND)tvi.lParam);
												   if (count == 1)
												   {
													   TCHAR szAppLastInstance[MAX_PATH];
													   LoadString(hInstance, IDS_APP_LAST_INSTANCE, (TCHAR*)szAppLastInstance, sizeof(szAppLastInstance) / sizeof(TCHAR));

													   TCHAR szAppName[MAX_PATH];
													   LoadString(hInstance, IDS_APP_NAME, (TCHAR*)szAppName, sizeof(szAppName) / sizeof(TCHAR));

													   if (MessageBox(hDlg, szAppLastInstance, szAppName, MB_ICONWARNING | MB_YESNOCANCEL) == IDYES)
													   {
														   TreeView_DeleteItem(hTree, htiDelete);
														   hDelete = (HWND)tvi.lParam;
													   }
												   }
												   else if (count > 1)
												   {
													   TreeView_DeleteItem(hTree, htiDelete);
													   hDelete = (HWND)tvi.lParam;
												   }
											   }
											   htiSelected = NULL;
											   return TRUE;
					   }
					   case IDOK:
					   {
									if (hDelete)
										ShowWindow(hDelete, SW_HIDE);
									hDelete = NULL;

									HTREEITEM hCurrentRoot = TreeView_GetRoot(hTree);
									if (hCurrentRoot)
									{
										int i = 0;
										do
										{
											HTREEITEM hCurrentChild = TreeView_GetChild(hTree, hCurrentRoot);
											pWOD[i].table.clear();
											if (hCurrentChild)
											{
												do
												{
													TVITEM tvi;
													tvi.mask = TVIF_HANDLE;
													tvi.hItem = hCurrentChild;
													TreeView_GetItem(hTree, &tvi);
													pWOD[i].table.push_back((HWND)tvi.lParam);
												} while (hCurrentChild = TreeView_GetNextItem(hTree, hCurrentChild, TVGN_NEXT));
											}
											i++;
										} while (hCurrentRoot = TreeView_GetNextItem(hTree, hCurrentRoot, TVGN_NEXT));
									}
									else
									{
										return FALSE;
									}
									ShowWindows(windowsOnDesktop[GetCurrentDesktop()].table);
									EndDialog(hDlg, TRUE);
									return TRUE;
					   }
					   case IDCANCEL:
					   {
										EndDialog(hDlg, FALSE);
										return TRUE;
					   }
					   } //switch
	} //WM_COMMAND
	case WM_NOTIFY:
	{
					  switch (((LPNMHDR)lParam)->code)
					  {
					  case TVN_BEGINDRAG:
					  {
											HIMAGELIST himl;
											RECT rcItem;

											HTREEITEM htiParent = TreeView_GetParent(hTree, ((LPNMTREEVIEW)lParam)->itemNew.hItem);
											if (!htiParent)
											{
												htiDrag = NULL;
											}
											else
											{
												himl = TreeView_CreateDragImage(hTree, ((LPNMTREEVIEW)lParam)->itemNew.hItem);

												TreeView_GetItemRect(hTree, ((LPNMTREEVIEW)lParam)->itemNew.hItem, &rcItem, TRUE);

												ImageList_BeginDrag(himl, 0, 0, 0);
												ImageList_DragEnter(hTree, ((LPNMTREEVIEW)lParam)->ptDrag.x, ((LPNMTREEVIEW)lParam)->ptDrag.y);
												ShowCursor(FALSE);
												SetCapture(GetParent(hTree));
												bDragging = TRUE;

												htiDrag = ((LPNMTREEVIEW)lParam)->itemNew.hItem;
											}
											return TRUE;
					  }
					  case TVN_SELCHANGED:
					  {
											 htiSelected = ((LPNMTREEVIEW)lParam)->itemNew.hItem;
											 return TRUE;
					  }
					  case NM_RCLICK:
					  {
										HTREEITEM hCurrentItem = TreeView_GetSelection(hTree);
										if (!hCurrentItem)
											return FALSE;

										TreeView_SelectItem(hTree, hCurrentItem);

										if (!TreeView_GetParent(hTree, hCurrentItem))
										{
											ShowPopupMenu(hDlg, FALSE, FALSE, (BOOL)hCopy, FALSE);
										}
										else
										{
											ShowPopupMenu(hDlg, TRUE, TRUE, (BOOL)hCopy, TRUE);
										}
										return TRUE;
					  }
					  case TVN_KEYDOWN:
					  {
										  switch (((LPNMTVKEYDOWN)lParam)->wVKey)
										  {
										  case VK_DELETE:
										  {
														SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(CMD_TREE_DELETE, 0), 0);
														break;
										  }
										  case 'C':
										  {
													  if (GetKeyState(VK_CONTROL))
													  {
														  SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(CMD_TREE_COPY, 0), 0);
													  }
													  break;
										  }
										  case 'X':
										  {
													  if (GetKeyState(VK_CONTROL))
													  {
														  SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(CMD_TREE_CUT, 0), 0);
													  }
													  break;
										  }
										  case 'V':
										  {
													  if (GetKeyState(VK_CONTROL))
													  {
														  SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(CMD_TREE_PASTE, 0), 0);
													  }
													  break;
										  }
										  }
										  return TRUE;
					  }
					  default:
						  return TRUE;
					  }
	} //WM_NOTIFY
	} //switch
	return FALSE;
}