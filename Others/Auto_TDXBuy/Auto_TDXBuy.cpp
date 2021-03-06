
// Auto_TDXBuy.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Auto_TDXBuy.h"
#include "Auto_TDXBuyDlg.h"
#include "../Common/Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//HWND g_hMainWnd = NULL;
HWND g_hMainWnd = NULL;
HWND hPop_Wnd = NULL;

void LogTrace16380(LPCTSTR pszFormat, ...)
{
	va_list pArgs;

	char szMessageBuffer[16380] = { 0 };
	va_start(pArgs, pszFormat);
	_vsntprintf(szMessageBuffer, 16380, pszFormat, pArgs);
	va_end(pArgs);

	//获取应用程序目录
	char szapipath[16380];//（D:\Documents\Downloads\TEST.exe）
	memset(szapipath, 0, 16380);
	::GetModuleFileNameA(NULL, szapipath, 16380);

	//获取应用程序名称
	char szExe[16380] = "";//（TEST.exe）
	char *pbuf = NULL;
	char* szLine = strtok_s(szapipath, "\\", &pbuf);
	while (NULL != szLine)
	{
		strcpy_s(szExe, szLine);
		szLine = strtok_s(NULL, "\\", &pbuf);
	}

	memset(szapipath, 0, 16380);
	strcpy(szapipath, "[");
	char* ppp = szapipath + 1;
	//删除.exe
	strncpy(ppp, szExe, strlen(szExe) - 4);
	strcat(szapipath, "] ");
	//cout << szapipath << endl;//(TEST)
	strcat(szapipath, szMessageBuffer);

	OutputDebugString(szapipath);
	//死循环，暂时屏蔽
	HWND hTradeWnd = ::FindWindow(NULL, "自动化交易控制中心");
	if (hTradeWnd != NULL)
	{
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.lpData = szapipath;
		cds.cbData = strlen(szapipath) + 1; //字符串请记得把'\0'加上, 不然就错了, 这里是ANSI字符串

		::SendMessage(hTradeWnd, WM_COPYDATA, 0, (LPARAM)&cds);
	}

}


//遍历所有子窗口的子窗口 , Z序遍历
void print_window2(HWND parent, int level)
{
	HWND child = NULL;
	TCHAR buf[MAX_PATH];
	DWORD pid = 0, tid = 0;
	do {
		child = FindWindowEx(parent, child, NULL, NULL);
		int ret = GetWindowText(child, buf, MAX_PATH);
		buf[ret] = 0;
		tid = GetWindowThreadProcessId(child, &pid);
		for (int i = 0; i < level; ++i)
			_tprintf("\t");

		_tprintf("[[%s ,  pid:%d, tid:%d\n", buf, pid, tid);

		if (child)
			print_window2(child, level + 1);
	} while (child);
}

//遍历所有 explore 下的窗口 , Z序遍历
void print_window()
{
	HWND child = NULL;
	TCHAR buf[MAX_PATH];
	DWORD pid = 0, tid = 0;

	do {
		//“查找” Explore 下的一个窗口,如果能找到则根据 Explore 下的child 继续找
		child = FindWindowEx(NULL, child, NULL, NULL);
		int ret = GetWindowText(child, buf, MAX_PATH);
		buf[ret] = 0;
		tid = GetWindowThreadProcessId(child, &pid);
		_tprintf("%s ,  pid:%d, tid:%d\n", buf, pid, tid);

		//遍历子窗口们
		if (child)
			print_window2(child, 1);
	} while (child);
}

// CAuto_TDXBuyApp

BEGIN_MESSAGE_MAP(CAuto_TDXBuyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAuto_TDXBuyApp 构造

CAuto_TDXBuyApp::CAuto_TDXBuyApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

/// 回調函数:査找AP主窗口句柄
BOOL CALLBACK EnumWindowsPrcHAZQ(HWND hwnd, LPARAM lParam)
{
	char szCaption[512];
	char *szTemp;

	memset(szCaption, '\0', 512);
	szTemp = (char *)lParam;
	::GetWindowTextA(hwnd, szCaption, sizeof(szCaption));


	if (strstr(szCaption, szTemp))
	{
		::GetClassName(hwnd, szCaption, sizeof(szCaption));
		if (strstr(szCaption, "TdxW_MainFrame_Class"))
		{
			g_hMainWnd = hwnd;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CALLBACK EnumWindowsPrc(HWND hwnd, LPARAM lParam)
{
	char szCaption[512];
	char *szTemp;

	memset(szCaption, '\0', 512);
	szTemp = (char *)lParam;
	::GetWindowTextA(hwnd, szCaption, sizeof(szCaption));


	if (strstr(szCaption, szTemp))
	{
		::GetClassName(hwnd, szCaption, sizeof(szCaption));
		//if (strstr(szCaption, "TdxW_MainFrame_Class"))
		{
			g_hMainWnd = hwnd;
			return FALSE;
		}
	}

	return TRUE;
}


static char sMainClass[256] = "TdxW_MainFrame_Class";

void DoTrade(char* sRate, char * sDlgCaption, int iActionID, float fPriceOffset, char * buyCount_Default, char * sBuyBtnTxt)
{
	LogTrace16380("DoTrade sRate=%s iActionID=%d\n", sRate, iActionID);
	char sRefreshBtnTxt[256] = "刷";
	char sRateBtnTxt[256] = { 0 };

	if ((_tcsicmp(sRate, _T("1")) == 0) ||
		(_tcsicmp(sRate, _T("1/1")) == 0))
	{
		strcpy(sRateBtnTxt, "全部");
	}
	else
		strcpy(sRateBtnTxt, sRate);

	//华安证券V6.36 - [组合图-创业板]
	//通达信金融终端V7.46 - [分析图表-创业板指]
	g_hMainWnd = NULL;
	::EnumWindows(EnumWindowsPrcHAZQ, (LPARAM)"华安证券");

	HWND hTDX_MainWnd = g_hMainWnd/*::FindWindowA(sMainClass, NULL)*/;
	if (hTDX_MainWnd)
	{
		::SetWindowPos(hTDX_MainWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetWindowPos(hTDX_MainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetForegroundWindow(hTDX_MainWnd);

		HWND hTDX_QuickTradeWnd = ::FindWindowA("#32770", sDlgCaption);
		while (NULL != hTDX_QuickTradeWnd)
		{
			Sleep(10);
			SendMessage(hTDX_QuickTradeWnd, WM_CLOSE, 0, 0L);
			hTDX_QuickTradeWnd = ::FindWindowA("#32770", sDlgCaption);
		}
		
		::SendMessage(hTDX_MainWnd,WM_LBUTTONDOWN,0,0x770055);//鼠标按下消息 13 
		::SendMessage(hTDX_MainWnd,WM_LBUTTONUP,0,0x770055); //鼠标抬起消息
		
		/*iActionID=
		买一价闪买5081买二价闪买5082买三价闪买5083买四价闪买5084买五价闪买5085
		买六价闪买5086买七价闪买5087买八价闪买5088买九价闪买5089买十价闪买5090
		卖一价闪买5091卖二价闪买5092卖三价闪买5093卖四价闪买5094卖五价闪买5095
		卖六价闪买5096卖七价闪买5097卖八价闪买5098卖九价闪买5099卖十价闪买5100
		涨停价闪买5101跌停价闪买5102

		买一价闪卖5111买二价闪卖5112买三价闪卖5113买四价闪卖5114买五价闪卖5115
		买六价闪卖5116买七价闪卖5117买八价闪卖5118买九价闪卖5119买十价闪卖5120
		卖一价闪卖5121卖二价闪卖5122卖三价闪卖5123卖四价闪卖5124卖五价闪卖5125
		卖六价闪卖5126卖七价闪卖5127卖八价闪卖5128卖九价闪卖5129卖十价闪卖5130
		涨停价闪卖5131跌停价闪卖5132
		*/
		//弹出闪电窗：
		//::SendMessage((HWND)hTDX_MainWnd, WM_COMMAND, MAKEWPARAM(5085, 0), NULL);
		::SendMessage((HWND)hTDX_MainWnd, WM_COMMAND, MAKEWPARAM(iActionID, 0), NULL);
		LogTrace16380("弹出闪电窗\n");

		//获取闪电窗：
		hTDX_QuickTradeWnd = NULL;
		while (!hTDX_QuickTradeWnd)
		{
			Sleep(100);
			hTDX_QuickTradeWnd = ::FindWindowA("#32770", sDlgCaption);
		}
		LogTrace16380("获取闪电窗=0x%x\n", hTDX_QuickTradeWnd);
		Sleep(200);

		DWORD_PTR dwResult = 0;
		while (!::SendMessageTimeout(hTDX_QuickTradeWnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &dwResult))// 已经停止响应了（俗话说的窗口挂死了）  
		{
			LogTrace16380("窗口死了？？？。。。。。。。。。。\n");
			Sleep(50);
		}
		
		CWnd *pWnd = CWnd::FromHandle(hTDX_QuickTradeWnd)->GetWindow(GW_CHILD);

		CWnd *pCurRateBtnWnd = NULL;
		CWnd *pRefreshBtnWnd = NULL;
		TCHAR szBuf[256];
		while (pWnd != NULL)
		{
			GetClassName(pWnd->m_hWnd, szBuf, 256);
			if (_tcsicmp(szBuf, _T("Button")) == 0)
			{
				char temp1[256] = { 0 };
				pWnd->GetWindowText(temp1, 255);

				if (_tcsicmp(temp1, sRateBtnTxt) == 0)
				{
					pCurRateBtnWnd = pWnd;
					//SendMessage(pCurRateWnd->m_hWnd, BM_CLICK, 0, 0L);
					LogTrace16380("获取比例窗=0x%x\n", pCurRateBtnWnd);
				}
				else if (_tcsicmp(temp1, sRefreshBtnTxt) == 0)
				{
					pRefreshBtnWnd = pWnd;
					//SendMessage(pCurRateWnd->m_hWnd, BM_CLICK, 0, 0L);
					LogTrace16380("获取刷新窗=0x%x\n", pRefreshBtnWnd);
				}
			}

			pWnd = pWnd->GetNextWindow();
		}

		pWnd = CWnd::FromHandle(hTDX_QuickTradeWnd)->GetWindow(GW_CHILD);
		int idxEdit = 0;
		int idxButton = 0;
		while (pWnd != NULL)
		{
			GetClassName(pWnd->m_hWnd, szBuf, 256);
			if (_tcsicmp(szBuf, _T("Edit")) == 0)
			{
				idxEdit++;
				if (idxEdit == 1) // 1 价格
				{
					LogTrace16380("获取价格框文本。。。\n");
					char temp2[256] = { 0 };
					char tempNew[256] = { 0 };
					while (strlen(temp2) == 0)
					{
						Sleep(10);
						::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)temp2);//EDIT的句柄，消息，接收缓冲区大小，接收缓冲区指针
					}
					char str[255] = { 0 };
					// 价格 + - 0.001
					sprintf(str, "%.3f", atof(temp2) + fPriceOffset);
					::SendMessageA(pWnd->m_hWnd, WM_SETTEXT, 0, (LPARAM)str);

					while (_tcsicmp(tempNew, str) != 0)
					{
						Sleep(10);
						::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)tempNew);//EDIT的句柄，消息，接收缓冲区大小，接收缓冲区指针
					}

					if (NULL != pRefreshBtnWnd)
						SendMessage(pRefreshBtnWnd->m_hWnd, BM_CLICK, 0, 0L);

					//CWnd::FromHandle(hTDX_QuickTradeWnd)->UpdateWindow();
					Sleep(200);//问题：【+ 0.001】，不知道何时新的最大可买计算结束（值也可能不变）
					LogTrace16380("获取价格框文本结束\n");
				}
				if (idxEdit == 3) //  3 数量
				{
					LogTrace16380("比例窗=0x%x\n", pCurRateBtnWnd);

					if (NULL == pCurRateBtnWnd)
					{
						LogTrace16380("设置数量框文本=%s。。。\n", buyCount_Default);
						::SendMessageA(pWnd->m_hWnd, WM_SETTEXT, 0, (LPARAM)buyCount_Default);
						char temp2[256] = { 0 };
						while (strcmp(temp2, buyCount_Default) != 0)
						{
							Sleep(10);
							::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)temp2);//EDIT的句柄，消息，接收缓冲区大小，接收缓冲区指针
						}
						LogTrace16380("设置数量框文本=%s 结束\n", buyCount_Default);
					}
					else
					{
						LogTrace16380("【点击】数量比例 按钮。。。%08x %08x\n", pCurRateBtnWnd, pCurRateBtnWnd->m_hWnd);
						char temp2[256] = { 0 };
						while ((strcmp(temp2, "0") == 0) || (strlen(temp2) == 0))
						{
							//偶发死循环 BUG ,待查。。。
							SendMessage(pCurRateBtnWnd->m_hWnd, BM_CLICK, 0, 0L);
							Sleep(10);
							SendMessage(pCurRateBtnWnd->m_hWnd, BM_CLICK, 0, 0L);
							Sleep(10);
							::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)temp2);//EDIT的句柄，消息，接收缓冲区大小，接收缓冲区指针
						}
						LogTrace16380("【点击】数量比例 按钮结束\n");

					}
				}
			}
			//else if (_tcsicmp(szBuf, _T("Button")) == 0)
			//{
			//	char temp1[256] = { 0 };
			//	pWnd->GetWindowText(temp1, 255);

			//	if (_tcsicmp(temp1, sRateBtnTxt) == 0)
			//	{
			//		pCurRateBtnWnd = pWnd;
			//		//SendMessage(pCurRateWnd->m_hWnd, BM_CLICK, 0, 0L);
			//	}
			//	else if (_tcsicmp(temp1, sRefreshBtnTxt) == 0)
			//	{
			//		pRefreshBtnWnd = pWnd;
			//		//SendMessage(pCurRateWnd->m_hWnd, BM_CLICK, 0, 0L);
			//	}
			//}

			pWnd = pWnd->GetNextWindow();
		}

		{
			LogTrace16380("【点击】买卖 按钮\n");
			Sleep(200);
			HWND hWnd0 = NULL;
			while (NULL == hWnd0)
			{
				Sleep(10);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}
			if ((NULL != hWnd0) && ::IsWindowVisible(hTDX_QuickTradeWnd))
			{
				LogTrace16380("点了一次。。。\n");
				SendMessage(hWnd0, BM_CLICK, 0, 0L);
				Sleep(200);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}
			if ((NULL != hWnd0) && ::IsWindowVisible(hTDX_QuickTradeWnd))
			{
				LogTrace16380("点了一次。。。\n");
				SendMessage(hWnd0, BM_CLICK, 0, 0L);
				Sleep(200);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}
			if ((NULL != hWnd0) && ::IsWindowVisible(hTDX_QuickTradeWnd))
			{
				LogTrace16380("点了一次。。。\n");
				SendMessage(hWnd0, BM_CLICK, 0, 0L);
				Sleep(200);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}

			LogTrace16380("交易完成，打开委托撤单窗口看结果\n");
			//交易完成，打开委托撤单窗口看结果：
			//hTDX_MainWnd = ::FindWindowA(sMainClass, NULL);
			if (hTDX_MainWnd)
			{
				//新的华安会赢里，5333变成弹出了，待修改
				//::SendMessage((HWND)hTDX_MainWnd, WM_COMMAND, MAKEWPARAM(5333, 0), NULL);
			}
		}
	}
}

//无法更换股票，只能交易当前主图显示行情的票
void OnBuy(char* sCode, char* sRate, float fTotalRate)
{
	char buyCount_Default[256] = "19000";
	char sDlgCaption[256] = "闪电买入";
	char sBuyBtnTxt[256] = "买 入";

	//int iActionID = 5081;//买一价闪买
	//float fPriceOffset = 0.001;
	int iActionID = 5095;//卖五价闪买
	float fPriceOffset = 0.000;

	DoTrade(sRate, sDlgCaption, iActionID, fPriceOffset, buyCount_Default, sBuyBtnTxt);
}

//无法更换股票，只能交易当前主图显示行情的票
void OnSell(char* sCode, char* sRate, float fTotalRate)
{
	char buyCount_Default[256] = "19000";
	char sDlgCaption[256] = "闪电卖出";
	char sBuyBtnTxt[256] = "卖  出";

	//int iActionID = 5121;//卖一价闪卖
	//float fPriceOffset = -0.001;
	int iActionID = 5115; //买五价闪卖
	float fPriceOffset = -0.000;

	DoTrade(sRate, sDlgCaption, iActionID, fPriceOffset, buyCount_Default, sBuyBtnTxt);
}

// 唯一的一个 CAuto_TDXBuyApp 对象

CAuto_TDXBuyApp theApp;


// CAuto_TDXBuyApp 初始化

BOOL CAuto_TDXBuyApp::InitInstance()
{
	LogTrace16380("  \n");
	LogTrace16380("======“查找”主窗口 <<<<<=======\n");
	::EnumWindows(EnumWindowsPrc, (LPARAM)"核新网上交易系统");
	//WinExec("echo 0>>1.txt", SW_HIDE);
	//g_hMainWnd = g_hMainWnd;
	//hMain_Wnd = Find_ChildWindow(NULL, "核新网上交易系统");
	if (g_hMainWnd)
	{
		ShowWindow(g_hMainWnd, SW_SHOWNORMAL);
		SetForegroundWindow(g_hMainWnd);
		SetActiveWindow(g_hMainWnd); //父窗口置为活动窗口
		LogTrace16380("置为活动窗口\n");
		HWND hSubWnd = Find_ChildWindow(g_hMainWnd, "同时买卖");//先找到唯一的子窗口，再取其父窗口进行关键 按钮的“查找”
		if (hSubWnd)
		{
			LogTrace16380("【点击】 刷新 按钮\n");
			FindClickRefresh(hSubWnd);
// 			do 
// 			{
				Sleep(500);
			//} while (!::IsWindowEnabled(hSubWnd));

			//if (3 < __argc)
			{
				HWND hBtn_BS = NULL;
				LogTrace16380("“查找”买卖 按钮\n");
				if (strstr(__argv[0], "Auto_TDXBuy"))
				{
					hBtn_BS = Find_ChildWindow(::GetParent(hSubWnd), "买入[B]");
				}
				else if (strstr(__argv[0], "Auto_TDXSell"))
				{
					hBtn_BS = Find_ChildWindow(::GetParent(hSubWnd), "卖出[S]");
				}

				if (hBtn_BS)
				{

					LogTrace16380("【点击】 买/卖 按钮。。。\n");
					PostMessage(hBtn_BS, BM_CLICK, 0, 0L);

					//HWND hSubWnd0 = ::FindWindowA("#32770", "");//先找到唯一的子窗口，再取其父窗口进行关键 按钮的“查找”

					LogTrace16380("。。。等待弹出框3s\n");
					//等待弹出框3s
					HWND hPopWnd = NULL;
					for (int i = 0; i < 60; i++)
					{
						hPopWnd = ::GetLastActivePopup(g_hMainWnd);
						if (g_hMainWnd != hPopWnd)
						{
							//得到文字
							if (hPopWnd)
							{
								HWND child = NULL;
								TCHAR buf[MAX_PATH];
								DWORD pid = 0, tid = 0;

								do {
									child = FindWindowEx(hPopWnd, child, NULL, NULL);
									int dStyle = (int)GetWindowLong(child, GWL_STYLE);
									if ((0x50020000 == dStyle) || (0x5002000D == dStyle))
									{
										//得到结果文字
										TCHAR buf[1024];
										::SendMessage(child, WM_GETTEXT, sizeof(buf) / sizeof(char), (LPARAM)buf);//EDIT的句柄，消息，接收缓冲区大小，接收缓冲区指针
										LogTrace16380(buf);
										//送出文字。。。

										LogTrace16380("“查找”确定 按钮\n");
										//关闭(hPopWnd);
										child = Find_ChildWindow(hPopWnd, "确定");
										if (child)
										{
											SetForegroundWindow(hPopWnd);
											SetActiveWindow(hPopWnd); //父窗口置为活动窗口
											LogTrace16380("【点击】 确定 按钮\n");
											SendMessage(child, BM_CLICK, 0, 0);//单击

											ShowWindow(g_hMainWnd, SW_MINIMIZE);
										}
										break;
									}

								} while (child);

								child = NULL;
							}
							break;
						}
						Sleep(50);
					}

					LogTrace16380("======本次行动结束。 >>>>>=======\n");
					return TRUE;

				}
			}
		}
	}
	LogTrace16380("======本次行动【意外退出】。。。？？？？？？\n");
	return FALSE; //下面的代码是为HAZQ.TDX, 暂时不用了。。。

	if (3 < __argc)
	{
		if (strstr(__argv[0], "Auto_TDXBuy"))
		{
			OnBuy(__argv[1], __argv[2], atof(__argv[3]));
		}
		else if (strstr(__argv[0], "Auto_TDXSell"))
		{
			OnSell(__argv[1], __argv[2], atof(__argv[3]));
		}
	}
	//if ((m_lpCmdLine[0] == _T('/0')) || (lstrcmp(m_lpCmdLine, _T("b1")) != 0))
	//{
	//	m_bCmdRet = TRUE;
	//}
	//else
	//	m_bCmdRet = FALSE;
	//OnSell();
	return FALSE;
	//============================================

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CAuto_TDXBuyDlg dlg;
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
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

