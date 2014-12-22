/**
 * @author Micha³ Gawin
 */


#ifndef _SYS_H_VD_
#define _SYS_H_VD_


/** Desktop manager */
#define DESKTOP_MGR_SZ		TEXT("DesktopMgr.dll")

/** main window class name */
const TCHAR szClassName[] = TEXT("_VirtualDesktop_");

/** Private windows messages */

/** Message while press tray's icon */
#define WM_TRAY_ICON		(WM_USER + 1)

/** Message send to application to change desktop */
#define WM_CHANGE_DESKTOP	(WM_TRAY_ICON + 1)


/** WM_COMMAND messages */

/** id represents set plugin always on top */
#define	CMD_AOT			1

/** id represents dialog to open desktop manager */
#define	CMD_DSKMGR		(CMD_AOT + 1)

/** id represents dialog in menu to change plugin */
#define	CMD_PLUGIN		(CMD_DSKMGR + 1)

/** id represents dialog in menu to set wallpate*/
#define CMD_WALL		(CMD_PLUGIN + 1)

/** id represents dialog 'About' in menu */
#define CMD_ABOUT		(CMD_WALL + 1)

/** id represents dialog in menu to close application */
#define CMD_QUIT		(CMD_ABOUT + 1)

/** id represents copy node from tree popup menu */
#define CMD_TREE_COPY	(CMD_QUIT + 1)

/** id represents cut node from tree popup menu */
#define CMD_TREE_CUT	(CMD_TREE_COPY + 1)

/** id represents paste node from tree popup menu */
#define CMD_TREE_PASTE	(CMD_TREE_CUT + 1)

/** id represents delete node from tree popup menu */
#define CMD_TREE_DELETE	(CMD_TREE_PASTE+1)

/** id represents properties node from tree popup menu */
#define CMD_TREE_PROPERTIES (CMD_TREE_DELETE+1)


#endif //_SYS_H_VD_
