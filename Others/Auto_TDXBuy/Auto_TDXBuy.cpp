
// Auto_TDXBuy.cpp : ����Ӧ�ó��������Ϊ��
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

	//��ȡӦ�ó���Ŀ¼
	char szapipath[16380];//��D:\Documents\Downloads\TEST.exe��
	memset(szapipath, 0, 16380);
	::GetModuleFileNameA(NULL, szapipath, 16380);

	//��ȡӦ�ó�������
	char szExe[16380] = "";//��TEST.exe��
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
	//ɾ��.exe
	strncpy(ppp, szExe, strlen(szExe) - 4);
	strcat(szapipath, "] ");
	//cout << szapipath << endl;//(TEST)
	strcat(szapipath, szMessageBuffer);

	OutputDebugString(szapipath);
	//��ѭ������ʱ����
	HWND hTradeWnd = ::FindWindow(NULL, "�Զ������׿�������");
	if (hTradeWnd != NULL)
	{
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.lpData = szapipath;
		cds.cbData = strlen(szapipath) + 1; //�ַ�����ǵð�'\0'����, ��Ȼ�ʹ���, ������ANSI�ַ���

		::SendMessage(hTradeWnd, WM_COPYDATA, 0, (LPARAM)&cds);
	}

}


//���������Ӵ��ڵ��Ӵ��� , Z�����
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

//�������� explore �µĴ��� , Z�����
void print_window()
{
	HWND child = NULL;
	TCHAR buf[MAX_PATH];
	DWORD pid = 0, tid = 0;

	do {
		//���� Explore �µ�һ������,������ҵ������ Explore �µ�child ������
		child = FindWindowEx(NULL, child, NULL, NULL);
		int ret = GetWindowText(child, buf, MAX_PATH);
		buf[ret] = 0;
		tid = GetWindowThreadProcessId(child, &pid);
		_tprintf("%s ,  pid:%d, tid:%d\n", buf, pid, tid);

		//�����Ӵ�����
		if (child)
			print_window2(child, 1);
	} while (child);
}

// CAuto_TDXBuyApp

BEGIN_MESSAGE_MAP(CAuto_TDXBuyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAuto_TDXBuyApp ����

CAuto_TDXBuyApp::CAuto_TDXBuyApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

/// ���{����:����AP�����ھ��
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
	char sRefreshBtnTxt[256] = "ˢ";
	char sRateBtnTxt[256] = { 0 };

	if ((_tcsicmp(sRate, _T("1")) == 0) ||
		(_tcsicmp(sRate, _T("1/1")) == 0))
	{
		strcpy(sRateBtnTxt, "ȫ��");
	}
	else
		strcpy(sRateBtnTxt, sRate);

	//����֤ȯV6.36 - [���ͼ-��ҵ��]
	//ͨ���Ž����ն�V7.46 - [����ͼ��-��ҵ��ָ]
	g_hMainWnd = NULL;
	::EnumWindows(EnumWindowsPrcHAZQ, (LPARAM)"����֤ȯ");

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
		
		::SendMessage(hTDX_MainWnd,WM_LBUTTONDOWN,0,0x770055);//��갴����Ϣ 13 
		::SendMessage(hTDX_MainWnd,WM_LBUTTONUP,0,0x770055); //���̧����Ϣ
		
		/*iActionID=
		��һ������5081���������5082����������5083���ļ�����5084���������5085
		����������5086���߼�����5087��˼�����5088��ż�����5089��ʮ������5090
		��һ������5091����������5092����������5093���ļ�����5094���������5095
		����������5096���߼�����5097���˼�����5098���ż�����5099��ʮ������5100
		��ͣ������5101��ͣ������5102

		��һ������5111���������5112����������5113���ļ�����5114���������5115
		����������5116���߼�����5117��˼�����5118��ż�����5119��ʮ������5120
		��һ������5121����������5122����������5123���ļ�����5124���������5125
		����������5126���߼�����5127���˼�����5128���ż�����5129��ʮ������5130
		��ͣ������5131��ͣ������5132
		*/
		//�������細��
		//::SendMessage((HWND)hTDX_MainWnd, WM_COMMAND, MAKEWPARAM(5085, 0), NULL);
		::SendMessage((HWND)hTDX_MainWnd, WM_COMMAND, MAKEWPARAM(iActionID, 0), NULL);
		LogTrace16380("�������細\n");

		//��ȡ���細��
		hTDX_QuickTradeWnd = NULL;
		while (!hTDX_QuickTradeWnd)
		{
			Sleep(100);
			hTDX_QuickTradeWnd = ::FindWindowA("#32770", sDlgCaption);
		}
		LogTrace16380("��ȡ���細=0x%x\n", hTDX_QuickTradeWnd);
		Sleep(200);

		DWORD_PTR dwResult = 0;
		while (!::SendMessageTimeout(hTDX_QuickTradeWnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &dwResult))// �Ѿ�ֹͣ��Ӧ�ˣ��׻�˵�Ĵ��ڹ����ˣ�  
		{
			LogTrace16380("�������ˣ�������������������������\n");
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
					LogTrace16380("��ȡ������=0x%x\n", pCurRateBtnWnd);
				}
				else if (_tcsicmp(temp1, sRefreshBtnTxt) == 0)
				{
					pRefreshBtnWnd = pWnd;
					//SendMessage(pCurRateWnd->m_hWnd, BM_CLICK, 0, 0L);
					LogTrace16380("��ȡˢ�´�=0x%x\n", pRefreshBtnWnd);
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
				if (idxEdit == 1) // 1 �۸�
				{
					LogTrace16380("��ȡ�۸���ı�������\n");
					char temp2[256] = { 0 };
					char tempNew[256] = { 0 };
					while (strlen(temp2) == 0)
					{
						Sleep(10);
						::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)temp2);//EDIT�ľ������Ϣ�����ջ�������С�����ջ�����ָ��
					}
					char str[255] = { 0 };
					// �۸� + - 0.001
					sprintf(str, "%.3f", atof(temp2) + fPriceOffset);
					::SendMessageA(pWnd->m_hWnd, WM_SETTEXT, 0, (LPARAM)str);

					while (_tcsicmp(tempNew, str) != 0)
					{
						Sleep(10);
						::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)tempNew);//EDIT�ľ������Ϣ�����ջ�������С�����ջ�����ָ��
					}

					if (NULL != pRefreshBtnWnd)
						SendMessage(pRefreshBtnWnd->m_hWnd, BM_CLICK, 0, 0L);

					//CWnd::FromHandle(hTDX_QuickTradeWnd)->UpdateWindow();
					Sleep(200);//���⣺��+ 0.001������֪����ʱ�µ���������������ֵҲ���ܲ��䣩
					LogTrace16380("��ȡ�۸���ı�����\n");
				}
				if (idxEdit == 3) //  3 ����
				{
					LogTrace16380("������=0x%x\n", pCurRateBtnWnd);

					if (NULL == pCurRateBtnWnd)
					{
						LogTrace16380("�����������ı�=%s������\n", buyCount_Default);
						::SendMessageA(pWnd->m_hWnd, WM_SETTEXT, 0, (LPARAM)buyCount_Default);
						char temp2[256] = { 0 };
						while (strcmp(temp2, buyCount_Default) != 0)
						{
							Sleep(10);
							::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)temp2);//EDIT�ľ������Ϣ�����ջ�������С�����ջ�����ָ��
						}
						LogTrace16380("�����������ı�=%s ����\n", buyCount_Default);
					}
					else
					{
						LogTrace16380("�������������ť������%08x %08x\n", pCurRateBtnWnd, pCurRateBtnWnd->m_hWnd);
						char temp2[256] = { 0 };
						while ((strcmp(temp2, "0") == 0) || (strlen(temp2) == 0))
						{
							//ż����ѭ�� BUG ,���顣����
							SendMessage(pCurRateBtnWnd->m_hWnd, BM_CLICK, 0, 0L);
							Sleep(10);
							SendMessage(pCurRateBtnWnd->m_hWnd, BM_CLICK, 0, 0L);
							Sleep(10);
							::SendMessageA(pWnd->m_hWnd, WM_GETTEXT, 256, (LPARAM)temp2);//EDIT�ľ������Ϣ�����ջ�������С�����ջ�����ָ��
						}
						LogTrace16380("�������������ť����\n");

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
			LogTrace16380("���������ť\n");
			Sleep(200);
			HWND hWnd0 = NULL;
			while (NULL == hWnd0)
			{
				Sleep(10);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}
			if ((NULL != hWnd0) && ::IsWindowVisible(hTDX_QuickTradeWnd))
			{
				LogTrace16380("����һ�Ρ�����\n");
				SendMessage(hWnd0, BM_CLICK, 0, 0L);
				Sleep(200);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}
			if ((NULL != hWnd0) && ::IsWindowVisible(hTDX_QuickTradeWnd))
			{
				LogTrace16380("����һ�Ρ�����\n");
				SendMessage(hWnd0, BM_CLICK, 0, 0L);
				Sleep(200);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}
			if ((NULL != hWnd0) && ::IsWindowVisible(hTDX_QuickTradeWnd))
			{
				LogTrace16380("����һ�Ρ�����\n");
				SendMessage(hWnd0, BM_CLICK, 0, 0L);
				Sleep(200);
				hWnd0 = ::FindWindowExA(hTDX_QuickTradeWnd, NULL, "Button", sBuyBtnTxt);
			}

			LogTrace16380("������ɣ���ί�г������ڿ����\n");
			//������ɣ���ί�г������ڿ������
			//hTDX_MainWnd = ::FindWindowA(sMainClass, NULL);
			if (hTDX_MainWnd)
			{
				//�µĻ�����Ӯ�5333��ɵ����ˣ����޸�
				//::SendMessage((HWND)hTDX_MainWnd, WM_COMMAND, MAKEWPARAM(5333, 0), NULL);
			}
		}
	}
}

//�޷�������Ʊ��ֻ�ܽ��׵�ǰ��ͼ��ʾ�����Ʊ
void OnBuy(char* sCode, char* sRate, float fTotalRate)
{
	char buyCount_Default[256] = "19000";
	char sDlgCaption[256] = "��������";
	char sBuyBtnTxt[256] = "�� ��";

	//int iActionID = 5081;//��һ������
	//float fPriceOffset = 0.001;
	int iActionID = 5095;//���������
	float fPriceOffset = 0.000;

	DoTrade(sRate, sDlgCaption, iActionID, fPriceOffset, buyCount_Default, sBuyBtnTxt);
}

//�޷�������Ʊ��ֻ�ܽ��׵�ǰ��ͼ��ʾ�����Ʊ
void OnSell(char* sCode, char* sRate, float fTotalRate)
{
	char buyCount_Default[256] = "19000";
	char sDlgCaption[256] = "��������";
	char sBuyBtnTxt[256] = "��  ��";

	//int iActionID = 5121;//��һ������
	//float fPriceOffset = -0.001;
	int iActionID = 5115; //���������
	float fPriceOffset = -0.000;

	DoTrade(sRate, sDlgCaption, iActionID, fPriceOffset, buyCount_Default, sBuyBtnTxt);
}

// Ψһ��һ�� CAuto_TDXBuyApp ����

CAuto_TDXBuyApp theApp;


// CAuto_TDXBuyApp ��ʼ��

BOOL CAuto_TDXBuyApp::InitInstance()
{
	::EnumWindows(EnumWindowsPrc, (LPARAM)"�������Ͻ���ϵͳ");
	//WinExec("echo 0>>1.txt", SW_HIDE);
	//g_hMainWnd = g_hMainWnd;
	//hMain_Wnd = Find_ChildWindow(NULL, "�������Ͻ���ϵͳ");
	if (g_hMainWnd)
	{
		ShowWindow(g_hMainWnd, SW_SHOWNORMAL);
		SetForegroundWindow(g_hMainWnd);
		SetActiveWindow(g_hMainWnd); //��������Ϊ�����
		HWND hSubWnd = Find_ChildWindow(g_hMainWnd, "ͬʱ����");//���ҵ�Ψһ���Ӵ��ڣ���ȡ�丸���ڽ��йؼ���ť�Ĳ���
		if (hSubWnd)
		{
			FindClickRefresh(hSubWnd);
			do 
			{
				Sleep(500);
			} while (!::IsWindowEnabled(hSubWnd));

			//if (3 < __argc)
			{
				HWND hBtn_BS = NULL;
				if (strstr(__argv[0], "Auto_TDXBuy"))
				{
					hBtn_BS = Find_ChildWindow(::GetParent(hSubWnd), "����[B]");
				}
				else if (strstr(__argv[0], "Auto_TDXSell"))
				{
					hBtn_BS = Find_ChildWindow(::GetParent(hSubWnd), "����[S]");
				}

				if (hBtn_BS)
				{

					LogTrace16380("����һ�ΰ�ť������\n");
					PostMessage(hBtn_BS, BM_CLICK, 0, 0L);

					//HWND hSubWnd0 = ::FindWindowA("#32770", "");//���ҵ�Ψһ���Ӵ��ڣ���ȡ�丸���ڽ��йؼ���ť�Ĳ���

					//�ȴ�������3s
					HWND hPopWnd = NULL;
					for (int i = 0; i < 60; i++)
					{
						hPopWnd = ::GetLastActivePopup(g_hMainWnd);
						if (g_hMainWnd != hPopWnd)
						{
							//�õ�����
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
										//�õ��������
										TCHAR buf[1024];
										::SendMessage(child, WM_GETTEXT, sizeof(buf) / sizeof(char), (LPARAM)buf);//EDIT�ľ������Ϣ�����ջ�������С�����ջ�����ָ��
										LogTrace16380(buf);
										//�ͳ����֡�����

										//�ر�(hPopWnd);
										child = Find_ChildWindow(hPopWnd, "ȷ��");
										if (child)
										{
											SetForegroundWindow(hPopWnd);
											SetActiveWindow(hPopWnd); //��������Ϊ�����
											SendMessage(child, BM_CLICK, 0, 0);//����

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

					return TRUE;

				}
			}
		}
	}
	return FALSE; //����Ĵ�����ΪHAZQ.TDX, ��ʱ�����ˡ�����

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

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CAuto_TDXBuyDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

