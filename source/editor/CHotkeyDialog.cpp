/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "CHotkeyDialog.h"

IMPLEMENT_DYNAMIC(CHotkeyDialog, CDialog)

BEGIN_MESSAGE_MAP(CHotkeyDialog, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


CHotkeyDialog::CHotkeyDialog(CWnd* pParent) : CDialog(CHotkeyDialog::IDD, pParent)
{
	
}

CHotkeyDialog::~CHotkeyDialog()
{

}


BOOL CHotkeyDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcList;
	list.GetClientRect(rcList);

	int w1 = rcList.Width() / 2;
	int w2 = rcList.Width() - w1;

	list.InsertColumn(0, "Keys", 0, w1);
	list.InsertColumn(1, "Description", 0, w2);

	AddHotkey("Alt + Left Mouse",		"scroll viewport");
	AddHotkey("Right Mouse",			"cancel drag");
	AddHotkey("S",						"sprite stamp tool");
	AddHotkey("V",						"move tool");
	AddHotkey("Z",						"zoom to selection");
	AddHotkey("Delete",					"delete selection");
	AddHotkey("Cursor Keys",			"move selection");
	AddHotkey("Ctrl + Left Mouse",		"add to selection");
	AddHotkey("G",						"toggle grid snapping");
	AddHotkey("Ctrl + G",				"toggle grid");
	AddHotkey("Ctrl + A",				"select all");
	AddHotkey("Ctrl + D",				"deselect all");
	AddHotkey("Ctrl + Up",				"bring forward");
	AddHotkey("Ctrl + Shift + Up",		"bring to front");
	AddHotkey("Ctrl + Down",			"send backward");
	AddHotkey("Ctrl + Shift + Down",	"send to back");
	AddHotkey("Ctrl + Plus",			"Scale object up by 0.05");
	AddHotkey("Ctrl + Minus",			"Scale object down by 0.05");
	AddHotkey("Ctrl + Z",				"Undo");
	AddHotkey("Ctrl + Shift + Z",		"Redo");
	AddHotkey("Ctrl + Y",				"Redo");
	AddHotkey("Ctrl + C",				"Connect Graph Nodes");
	AddHotkey("Ctrl + P",				"Toggle Graph Node as a Goal");
	AddHotkey("Ctrl + Backspace",		"Break All Connections for Selected Graph Nodes");
	AddHotkey("Ctrl + Backspace",		"Break Connections between Selected Graph Nodes");

	return TRUE;
}

void CHotkeyDialog::AddHotkey(const char *keys, const char *description)
{
	int idx = list.InsertItem(0xFFFF, keys);
	list.SetItemText(idx, 1, description);
}

void CHotkeyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LV_HOTKEYS, list);
}

void CHotkeyDialog::OnClose()
{
	EndDialog(0);
}


void CHotkeyDialog::OnOK()
{
	EndDialog(0);
}
