/**
* @author Micha³ Gawin
*/


#include "VirtualDesktop.h"

#include <windowsx.h>
#include <Commctrl.h>

#include "DesktopMgr\DesktopMgr.h"
#include "WindowsManager.h"
#include "Wallpaper.h"


VOID ShowPopupMenu(POINT point, HWND hwnd, BOOL copy, BOOL cut, BOOL paste, BOOL del)
{
	static HMENU menu = NULL;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

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
		LoadString(hInstance, IDS_CUT, (TCHAR*)szCut, sizeof(szCut) / sizeof(TCHAR));
		AppendMenu(menu, cut ? flag : flag | MF_GRAYED, CMD_TREE_CUT, (LPCTSTR)szCut);

		TCHAR szPaste[MAX_PATH];
		LoadString(hInstance, IDS_PASTE, (TCHAR*)szPaste, sizeof(szPaste) / sizeof(TCHAR));
		AppendMenu(menu, paste ? flag : flag | MF_GRAYED, CMD_TREE_PASTE, (LPCTSTR)szPaste);

		TCHAR szDelete[MAX_PATH];
		LoadString(hInstance, IDS_DELETE, (TCHAR*)szDelete, sizeof(szDelete) / sizeof(TCHAR));
		AppendMenu(menu, del ? flag : flag | MF_GRAYED, CMD_TREE_DELETE, (LPCTSTR)szDelete);

		AppendMenu(menu, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

		TCHAR szProperties[MAX_PATH];
		LoadString(hInstance, IDS_PROPERTIES, (TCHAR*)szProperties, sizeof(szProperties) / sizeof(TCHAR));
		AppendMenu(menu, flag, CMD_TREE_PROPERTIES, (LPCTSTR)szProperties);

		SetForegroundWindow(hwnd);
		TrackPopupMenu(menu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, point.x, point.y, 0, hwnd, 0);
		PostMessage(hwnd, WM_NULL, 0, 0);
	}
}


VOID DrawTree(HWND hTree, pWindowsOnDesktop pWOD)
{
	HIMAGELIST hIml;
	HBITMAP hBmp;
	INT nAppUp = -1;
	INT nAppDown = -1;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hTree, GWL_HINSTANCE);

	TreeView_DeleteAllItems(hTree);

	hBmp = LoadBitmap(NULL, MAKEINTRESOURCE(OBM_ZOOM));
	if (hBmp)
	{
		BITMAP bmp;
		INT status = GetObject(hBmp, sizeof (BITMAP), &bmp);
		hIml = ImageList_Create(bmp.bmWidth, bmp.bmHeight, FALSE, 2, 0);

		nAppUp = ImageList_Add(hIml, hBmp, (HBITMAP)NULL);
		DeleteObject(hBmp);

		hBmp = LoadBitmap(NULL, MAKEINTRESOURCE(OBM_ZOOMD));
		nAppDown = ImageList_Add(hIml, hBmp, (HBITMAP)NULL);
		DeleteObject(hBmp);

		if (ImageList_GetImageCount(hIml) >= 2)
			TreeView_SetImageList(hTree, hIml, TVSIL_NORMAL);
	}

	for (int i = 0; i < DESKTOPS; i++)
	{
		TCHAR szDesktopNum[MAX_PATH];
		LoadString(hInstance, IDS_DEF_DESKTOP_PREVIEW, (TCHAR*)szDesktopNum, sizeof(szDesktopNum) / sizeof(TCHAR));

		TCHAR szBuffer[MAX_PATH];
		memset(szBuffer, 0, sizeof (szBuffer));
		_stprintf(szBuffer, szDesktopNum, i);

		TVITEM		tvi;
		TVINSERTSTRUCT	tvis;

		memset(&tvi, 0, sizeof(tvi));
		tvi.mask = TVIF_TEXT | TVIF_PARAM;
		tvi.pszText = szBuffer;
		tvi.lParam = (LPARAM)i;
		if ((nAppUp != -1) && (nAppDown != -1))
		{
			tvi.mask |= TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvi.iImage = nAppUp;
			tvi.iSelectedImage = nAppDown;
		}
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

				memset(&tvi, 0, sizeof(tvi));
				tvi.mask = TVIF_TEXT | TVIF_PARAM;
				tvi.pszText = szBuffer;
				tvi.lParam = (LPARAM)*itor;
				if ((nAppUp != -1) && (nAppDown != -1))
				{
					tvi.mask |= TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					tvi.iImage = nAppUp;
					tvi.iSelectedImage = nAppDown;
				}
				tvis.item = tvi;
				tvis.hParent = hItem;
				tvis.hInsertAfter = TVI_SORT;
				HTREEITEM hItemChild = TreeView_InsertItem(hTree, &tvis);

				itor++;
			}
		}
	}
}


INT FindItem(HWND hTreeView, HWND hWindow, HTREEITEM htiParent = NULL)
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
	static HINSTANCE hInstance;
	static pWindowsOnDesktop pWOD = NULL;
	static pWindowsOnDesktop temp_pWOD = NULL;
	static HWND	hTree = NULL;
	static HWND hCopy = NULL;				//handle to window to copy
	static BOOL bCut = FALSE;
	static HTREEITEM htiDrag = NULL;		//refers to dragged item in TreeView
	static HTREEITEM htiSelected = NULL;	//refers to selected item in TreeView
	static HTREEITEM htiCopy = NULL;		//refers to copied item in TreeView
	static BOOL bDragging = FALSE;			//inform about drag-and-drop operation

	switch (message)
	{
	case WM_INITDIALOG:
	{
						  pWOD = (pWindowsOnDesktop)lParam;
						  temp_pWOD = new WindowsOnDesktop[DESKTOPS];
						  hInstance = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);

						  for (int i = 0; i < DESKTOPS; i++)
						  {
							  temp_pWOD[i].szWallpaper = new TCHAR[MAX_PATH];
							  memset(temp_pWOD[i].szWallpaper, 0, MAX_PATH * sizeof (TCHAR));
							  _tcscpy(temp_pWOD[i].szWallpaper, pWOD[i].szWallpaper);

							  for (vHandleItor itor = pWOD[i].table.begin(); itor != pWOD[i].table.end();)
							  {
								  if (IsWindow(*itor))
								  {
									  temp_pWOD[i].table.push_back(*itor);
									  itor++;
								  }
								  else
								  {
									  itor = pWOD[i].table.erase(itor++);
								  }
							  }
						  }

						  TCHAR szWindowName[MAX_PATH];
						  LoadString(hInstance, IDS_VD_MANAGER, (TCHAR*)szWindowName, sizeof (szWindowName) / sizeof (TCHAR));
						  SetWindowText(hDlg, szWindowName);

						  hTree = GetDlgItem(hDlg, IDC_TREE);
						  DrawTree(hTree, temp_pWOD);
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
											//continue in CMD_TREE_COPY
					   }
					   case CMD_TREE_COPY:
					   {
											 if (TreeView_GetSelection(hTree) && htiSelected)
											 {
												 if (TreeView_GetParent(hTree, htiSelected))
												 {
													 htiCopy = htiSelected;
													 TVITEM tvi;

													 tvi.mask = TVIF_HANDLE | TVIF_PARAM;
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

												  tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
												  tvi.hItem = htiCopy;
												  tvi.pszText = szBuffer;
												  tvi.cchTextMax = sizeof (szBuffer) / sizeof (TCHAR);
												  tvi.lParam = (LPARAM)hCopy;
												  TreeView_GetItem(hTree, &tvi);

												  TVINSERTSTRUCT	tvis;

												  HTREEITEM hParent = TreeView_GetParent(hTree, htiPaste);
												  tvis.hParent = hParent ? hParent : htiPaste;
												  tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
												  tvis.item = tvi;
												  tvis.hInsertAfter = TVI_SORT;

												  if (!FindItem(hTree, (HWND)tvi.lParam, tvis.hParent))
												  {
													  TreeView_InsertItem(hTree, &tvis);

													  if (bCut)
													  {
														  TreeView_DeleteItem(hTree, htiCopy);
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
													   }
												   }
												   else if (count > 1)
												   {
													   TreeView_DeleteItem(hTree, htiDelete);
												   }
											   }
											   htiSelected = NULL;
											   return TRUE;
					   }
					   case CMD_TREE_PROPERTIES:
					   {
												   if (TreeView_GetSelection(hTree) && htiSelected)
												   {
													   if (!TreeView_GetParent(hTree, htiSelected))
													   {
														   HTREEITEM htiSel = htiSelected;
														   TVITEM tvi;

														   tvi.mask = TVIF_HANDLE | TVIF_PARAM;
														   tvi.hItem = htiSel;
														   TreeView_GetItem(hTree, &tvi);

														   INT iDesktop = (INT)tvi.lParam;
														   DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_WALLPAPER), hDlg, DlgWallpaperProc, (LPARAM)(temp_pWOD + iDesktop));
													   }
												   }
												   return TRUE;
					   }
					   case IDOK:
					   {
									BOOL bRet = TRUE;

									HTREEITEM hCurrentRoot;
									int i;
									for (i = 0, hCurrentRoot = TreeView_GetRoot(hTree); hCurrentRoot != NULL; hCurrentRoot = TreeView_GetNextItem(hTree, hCurrentRoot, TVGN_NEXT), i++)
									{
										memset(pWOD[i].szWallpaper, 0, MAX_PATH * sizeof (TCHAR));
										_tcscpy(pWOD[i].szWallpaper, temp_pWOD[i].szWallpaper);

										pWOD[i].table.clear();

										for (HTREEITEM hCurrentChild = TreeView_GetChild(hTree, hCurrentRoot); hCurrentChild != NULL; hCurrentChild = TreeView_GetNextItem(hTree, hCurrentChild, TVGN_NEXT))
										{
											TVITEM tvi;
											tvi.mask = TVIF_HANDLE;
											tvi.hItem = hCurrentChild;
											TreeView_GetItem(hTree, &tvi);
											pWOD[i].table.push_back((HWND)tvi.lParam);
										}
									}

									if (i != DESKTOPS)
									{
										bRet = FALSE;
									}
									
									for (int i = 0; i < DESKTOPS; i++)
									{
										delete[] temp_pWOD[i].szWallpaper;
									}
									delete[] temp_pWOD;

									EndDialog(hDlg, bRet);

									return TRUE;
					   }
					   case IDCANCEL:
					   {
										for (int i = 0; i < DESKTOPS; i++)
										{
											delete[] temp_pWOD[i].szWallpaper;
										}
										delete[] temp_pWOD;

										EndDialog(hDlg, FALSE);
										return TRUE;
					   }
					   default:
					   {
								  return FALSE;
					   }
					   } //switch
	} //WM_COMMAND
	case WM_NOTIFY:
	{
					  switch (((LPNMHDR)lParam)->code)
					  {
					  case TVN_GETINFOTIP:
					  {
											 LPNMTVGETINFOTIP pTip = (LPNMTVGETINFOTIP)lParam;
											 HTREEITEM item = pTip->hItem;

											 TVITEM tvitem;
											 tvitem.mask = TVIF_TEXT;
											 tvitem.hItem = item;
											 TCHAR infoTipBuff[1024];
											 tvitem.pszText = infoTipBuff;
											 tvitem.cchTextMax = sizeof(infoTipBuff) / sizeof(TCHAR);
											 TreeView_GetItem(hTree, &tvitem);

											 _tcscpy_s(pTip->pszText, pTip->cchTextMax, tvitem.pszText);
											 return TRUE;
					  }
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

										POINT point;
										GetCursorPos(&point);
										if (!TreeView_GetParent(hTree, hCurrentItem))
										{
											ShowPopupMenu(point, hDlg, FALSE, FALSE, (BOOL)hCopy, FALSE);
										}
										else
										{
											ShowPopupMenu(point, hDlg, TRUE, TRUE, (BOOL)hCopy, TRUE);
										}
										return TRUE;
					  }
					  case TVN_KEYDOWN:
					  {
										  switch (((LPNMTVKEYDOWN)lParam)->wVKey)
										  {
										  case VK_F5:
										  {
														BOOL status = ChangeDesktop(GetCurrentDesktop());		//update applications on current desktop
														if (status)
														{
															for (int i = 0; i < DESKTOPS; i++)
															{
																delete[] temp_pWOD[i].szWallpaper;
															}
															delete[] temp_pWOD;

															temp_pWOD = new WindowsOnDesktop[DESKTOPS];

															for (int i = 0; i < DESKTOPS; i++)
															{
																temp_pWOD[i].szWallpaper = new TCHAR[MAX_PATH];
																memset(temp_pWOD[i].szWallpaper, 0, MAX_PATH * sizeof (TCHAR));
																_tcscpy(temp_pWOD[i].szWallpaper, pWOD[i].szWallpaper);

																for (vHandleItor itor = pWOD[i].table.begin(); itor != pWOD[i].table.end();)
																{
																	if (IsWindow(*itor))
																	{
																		temp_pWOD[i].table.push_back(*itor);
																		itor++;
																	}
																	else
																	{
																		itor = pWOD[i].table.erase(itor++);
																	}
																}
															}
															DrawTree(hTree, temp_pWOD);
														}
														else return FALSE;
														break;
										  }
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
										  case VK_APPS:
										  {
														  HTREEITEM hCurrentItem = TreeView_GetSelection(hTree);
														  if (hCurrentItem)
														  {
															  TreeView_SelectItem(hTree, hCurrentItem);

															  RECT rcItem;
															  GetWindowRect(hTree, &rcItem);

															  POINT point = { rcItem.left, rcItem.top };

															  if (TreeView_GetItemRect(hTree, hCurrentItem, &rcItem, TRUE))
															  {
																  point.x += rcItem.left;
																  point.y += rcItem.top;

																  if (!TreeView_GetParent(hTree, hCurrentItem))
																  {
																	  ShowPopupMenu(point, hDlg, FALSE, FALSE, (BOOL)hCopy, FALSE);
																  }
																  else
																  {
																	  ShowPopupMenu(point, hDlg, TRUE, TRUE, (BOOL)hCopy, TRUE);
																  }
															  }
														  }
														  else
														  {
															  return FALSE;
														  }
														  break;
										  }
										  default:
										  {
													 break;
										  }
										  }
										  return TRUE;
					  }
					  default:
					  {
								 return TRUE;
					  }
					  }
					  return TRUE;
	} //WM_NOTIFY
	} //main-switch
	return FALSE;
}