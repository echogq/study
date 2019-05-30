
// Auto_TDXAssets.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Auto_TDXAssets.h"
#include "Auto_TDXAssetsDlg.h"
#include <vector>
#include <afxcview.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAuto_TDXAssetsApp

BEGIN_MESSAGE_MAP(CAuto_TDXAssetsApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAuto_TDXAssetsApp 构造

CAuto_TDXAssetsApp::CAuto_TDXAssetsApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

HTREEITEM GetNextTreeItem(HWND hWnd, HTREEITEM hCurTreeItem)
{
	if (NULL == hWnd || NULL == hCurTreeItem)
		return NULL;

	HTREEITEM hResultItem = TreeView_GetNextItem(hWnd, hCurTreeItem, TVGN_CHILD);
	if (NULL == hResultItem)
	{
		hResultItem = TreeView_GetNextItem(hWnd, hCurTreeItem, TVGN_NEXT);
	}
	//else
	//	::OutputDebugStringA("  ");


	if (NULL == hResultItem)
	{
		HTREEITEM hParentItem = TreeView_GetNextItem(hWnd, hCurTreeItem, TVGN_PARENT);
		hResultItem = TreeView_GetNextItem(hWnd, hParentItem, TVGN_NEXT);
	}

	return hResultItem;
}



void Func()
{
	::OutputDebugStringA("\r\n================================\r\n");
	//获取树控件的内容
	HWND hParent = (HWND)0xA0D24; //SysTreeView32父窗口句柄
	HWND hTree = FindWindowEx(hParent, NULL, _T("SysTreeView32"), _T(""));

	HWND hWnd = (HWND)0xA0D24; //主程序句柄
	DWORD PID;
	GetWindowThreadProcessId(hWnd, &PID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	//hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, PID);
	ASSERT(NULL != hProcess);

	UINT vItemCount = TreeView_GetCount(hTree);
	HTREEITEM hTreeItem = TreeView_GetRoot(hTree);
	TCHAR buf[256];

	std::vector<CString> vecItemText;
	CTreeView* pTreeView = (CTreeView*)CWnd::FromHandle(hTree);
	CTreeCtrl& treeCtrl = pTreeView->GetTreeCtrl();
	for (int i = 0; i < vItemCount; i++)
	{
		LPTV_ITEM pTreeItem = (LPTV_ITEM)VirtualAllocEx(hProcess, NULL, sizeof(TV_ITEM), MEM_COMMIT, PAGE_READWRITE);
		TCHAR* pItemText = (TCHAR*)VirtualAllocEx(hProcess, NULL, 256, MEM_COMMIT, PAGE_READWRITE);

		TV_ITEM item;
		item.mask = TVIF_TEXT;
		item.hItem = hTreeItem;
		item.pszText = pItemText;
		item.cchTextMax = 256;

		WriteProcessMemory(hProcess, pTreeItem, &item, sizeof(TV_ITEM), NULL);
		treeCtrl.GetItem(pTreeItem);
		ReadProcessMemory(hProcess, pItemText, buf, 256, NULL);

		VirtualFreeEx(hProcess, pTreeItem, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pItemText, 0, MEM_RELEASE);

		//if (TreeView_GetRoot(hTree) != hTreeItem)
		{
			CString sTreeItemText(buf);
			vecItemText.push_back(sTreeItemText);

			if (sTreeItemText == "资金股份")
			{
				TreeView_SelectItem(hTree, hTreeItem);

				HWND hTestWnd = (HWND)0x00120A66;
				////WM_PARENTNOTIFY fwEvent : WM_LBUTTONDOWN xPos : 59 yPos : 57[wParam:00000201 lParam : 0039003B]
				//::SendMessage((HWND)hTestWnd, WM_PARENTNOTIFY, 0x00000201, 0x04560048);

				////WM_MOUSEACTIVATE hwndTopLevel:00120A66 nHittest:HTCLIENT uMsg:WM_LBUTTONDOWN [wParam:00120A66 lParam:02010001]
				//::SendMessage((HWND)hTestWnd, WM_MOUSEACTIVATE, 0x00120A66, 0x02010001);
				//::SendMessage((HWND)hTestWnd, WM_WINDOWPOSCHANGING, 0, 0x0018FA40);
				//::SendMessage((HWND)hTestWnd, WM_WINDOWPOSCHANGED, 0, 0x0018FA34);
				//::SendMessage((HWND)hTestWnd, WM_ACTIVATEAPP, 1, 0);
				//::SendMessage((HWND)hTestWnd, WM_NCACTIVATE, 1, 0);
				//::SendMessage((HWND)hTestWnd, WM_IME_NOTIFY, 2, 0);
				//::SendMessage((HWND)hTestWnd, WM_IME_SETCONTEXT, 1, 0xC000000F);

				////WM_SETCURSOR hwnd:00110B0A nHittest:HTCLIENT wMouseMsg:WM_LBUTTONDOWN [wParam:00110B0A lParam:02010001]
				//::SendMessage((HWND)hTestWnd, WM_SETCURSOR, 0x00110B0A, 0x02010001);

				////message:0x17EF [User-defined:WM_USER+5103] wParam:00000000 lParam:00000000
				//::SendMessage((HWND)hTestWnd, WM_USER + 5103, 0, 0);
				////message : 0x17ED[User - defined:WM_USER + 5101] wParam : 00000000 lParam : 00000000
				//::SendMessage((HWND)hTestWnd, WM_USER + 5101, 0, 0);
				////message : 0x17F0[User - defined:WM_USER + 5104] wParam : 00000000 lParam : 000000
				//::SendMessage((HWND)hTestWnd, WM_USER + 5104, 0, 0);

				{
					//开工
					//HTREEITEM hItem = TreeView_GetRoot(hTree);
					//hItem = TreeView_GetNextItem(hTree, hItem, TVGN_CHILD);
					//::SendMessage(hTree, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);
					//TreeView_EnsureVisible(hTree, hItem);
					//DWORD pid = 0;
					//GetWindowThreadProcessId(hTree, &pid);
					//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, pid);

					RECT *prect = (RECT*)VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);
					RECT rect = { 0 };
					unsigned long n = 0;

					*(HTREEITEM*)&rect = hTreeItem;
					WriteProcessMemory(hProcess, prect, &rect, sizeof(RECT), NULL);
					::SendMessage(hTree, TVM_GETITEMRECT, (WPARAM)FALSE, (LPARAM)prect);
					ReadProcessMemory(hProcess, prect, &rect, sizeof(RECT), &n);

					POINT pt = { 0 };
					pt.x = rect.left + (rect.right - rect.left) / 2;
					pt.y = rect.top + (rect.bottom - rect.top) / 2;

					//::SendMessage(hTree, WM_LBUTTONDBLCLK, 0, MAKELPARAM((WORD)pt.x, (WORD)pt.y));

					//释放内存
					CloseHandle(hProcess);

					if (prect)
					{
						VirtualFreeEx(hProcess, prect, 0, MEM_RELEASE);
					}

					//转换下坐标
					::ClientToScreen(hTree, &pt);

					//鼠标事件
					SetCursorPos(pt.x - 8, pt.y);

					POINT pt2 = { 0 };
					while ((pt2.x != pt.x - 8) || (pt2.y != pt.y))
					{
						Sleep(10);
						GetCursorPos(&pt2);
					}

// 					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
// 					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

// 					::SendMessage(hTestWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(72, 1110));
// 					::SendMessage(hTestWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(72, 1110));
					Sleep(10);
				}
			}
		}
		hTreeItem = GetNextTreeItem(hTree, hTreeItem);
	}

	for (auto it = vecItemText.begin(); it != vecItemText.end(); ++it)
	{
		//可根据Item内容判断转出的pdf是否可用
		::OutputDebugString(*it);
		::OutputDebugStringA("\r\n");
	}
}


// 唯一的一个 CAuto_TDXAssetsApp 对象

CAuto_TDXAssetsApp theApp;


// CAuto_TDXAssetsApp 初始化

BOOL CAuto_TDXAssetsApp::InitInstance()
{
	CWinApp::InitInstance();


	Func();
	//::SendMessage((HWND)0x120a66, WM_COMMAND, MAKEWPARAM(5338, 0), NULL);

	return FALSE;


	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CAuto_TDXAssetsDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
	}


#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

