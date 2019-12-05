// AutoTradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoTrade.h"
#include "AutoTradeDlg.h"
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <sstream>

using std::random_shuffle;
using std::vector;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include<stdio.h>
#include<string.h>
#include<time.h>

#define AUTO_OPEN_ALL "..\\Script\\Auto_OpenAll.exe"

std::vector<float>
Ema(std::vector<float> &X, int N)
{
	std::vector<float> vec;
	int nLen = X.size();
	if (nLen >= 1)
	{
		if (N > nLen) N = nLen;

		vec.resize(nLen);
		//vec.reserve(nLen);
		vec[0] = X[0];
		for (int i = 1; i < nLen; i++)
		{
			vec[i] = (2 * X[i] + (N - 1) * vec[i - 1]) / (N + 1);
		}
	}
	return vec;
}

char tmpbuf[128];
char* PrefixTimeStr( char* p )
{
	struct tm *newtime;
	time_t lt1;

	time( &lt1 );
	newtime=localtime(&lt1);

	//strftime( tmpbuf, 128, "Today is %A, the %d day of %B in the year %Y.\n", newtime);
	sprintf(tmpbuf, "周%d %2d:%2d:%2d %s", newtime->tm_wday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec, p);

	return tmpbuf;
}

void birthdaysong ()
{

	unsigned int frequency [] =
	{ 
		392,392,440,392,523,494, 
		392,392,440,392,587,523, 
		392,392,784,659,523,494,440, 
		698,698,659,523,587,523
	};

	unsigned int delay[] = 
	{
		375,125,500,500,500,1000, 
		375,125,500,500,500,1000, 
		375,125,500,500,500,500,1000, 
		375,125,500,500,500,1000,
	}; 

	int   i; 

	for ( i=0; i < 25; i++ ) 
	{ 
		Beep ( frequency[i] * 5, delay[i] );

		if ( i == 24 )
		{
			Sleep ( 500 );
			i = 0;
		}
	} 
}

#define ONE_BEEP 600
#define HALF_BEEP 300

#define NOTE_1 440
#define NOTE_2 495
#define NOTE_3 550
#define NOTE_4 587
#define NOTE_5 660
#define NOTE_6 733
#define NOTE_7 825
#define NOTE_NONE Sleep(ONE_BEEP);
void testsong ()
{
	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_1*2, ONE_BEEP*2);

	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP*2);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_1, HALF_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_3, ONE_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_1, HALF_BEEP);
	Beep(NOTE_2, ONE_BEEP*4);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_1*2, HALF_BEEP*3);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP*2);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_4, HALF_BEEP*3);
	Beep(NOTE_7/2, HALF_BEEP);
	Beep(NOTE_1, ONE_BEEP*4);

	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP*2);

	Beep(NOTE_7, ONE_BEEP);
	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_1*2, ONE_BEEP*2);

	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_1*2, HALF_BEEP);
	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_1, HALF_BEEP);
	Beep(NOTE_2, ONE_BEEP*4);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_1*2, HALF_BEEP*3);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP*2);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_4, HALF_BEEP*3);
	Beep(NOTE_7/2, HALF_BEEP);
	Beep(NOTE_1, ONE_BEEP*3);
}
void OnScreenfont(CString szBuffer)
{
	HFONT		font;
	HDC			hdc;
	HWND		hwnd;
	COLORREF	clr;
	//TCHAR		szBuffer[] = TEXT("CPU温度超高...>89°C");

	//获取桌面窗口句柄
	hwnd = GetDesktopWindow();
	//获取桌面窗口DC
	hdc = GetWindowDC(hwnd);
	//背景色透明
	SetBkMode(hdc, TRANSPARENT);

	font=CreateFont(
		120,48,0,0, FW_BOLD,0,0,0, ANSI_CHARSET ,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,NULL,(LPCTSTR)"微软雅黑");
	SelectObject(hdc,font);

	//设置颜色并输出文字
	clr = SetTextColor(hdc, RGB(255, 0, 0));
	TextOut(hdc, 800,400, szBuffer.GetBuffer(), szBuffer.GetLength());

	//还原颜色
	SetTextColor(hdc, clr);

	//释放句柄DC
	DeleteObject(font); 
	ReleaseDC(hwnd, hdc);
}

//bug：单线程锁不住
CCriticalSection csInHand; 
CCriticalSection csBuy;
CCriticalSection csSale;
HANDLE hProcmonitorTDXpool = NULL;
HANDLE hProcAccountInfo = NULL;
HANDLE hProcGetMyStkList = NULL;
BOOL bSaling = FALSE;
int iLimitBuyOneMoney = 18000;

CFont fontTitle;

#define STRICT 

/***********************************************************************
*
*  Global variables
*
***********************************************************************/ 

HWND g_hwndTimedOwner; 
BOOL g_bTimedOut; 

/***********************************************************************
*
*  MessageBoxTimer
*
*      The timer callback function that posts the fake quit message.
*      This function causes the message box to exit because the message box
*      has determined that the application is exiting.
*
***********************************************************************/ 
void CALLBACK MessageBoxTimer(HWND hwnd,  
							  UINT uiMsg,  
							  UINT idEvent,  
							  DWORD dwTime) 
{ 
	g_bTimedOut = TRUE; 
// 	if (g_hwndTimedOwner) 
// 		EnableWindow(g_hwndTimedOwner, TRUE); 
// 	PostQuitMessage(0); 
	HWND hwnd2 = FindWindow(NULL, MSGTITLE);
	if (hwnd2)
	{
		//PostMessage(hwnd2, WM_CLOSE, 0, 0);
		::EndDialog((HWND)hwnd2, IDCLOSE);
	}
} 

/***********************************************************************
*
*  TimedMessageBox
*
*      The same as the standard MessageBox, except that TimedMessageBox
*      also accepts a timeout. If the user does not respond within the
*      specified timeout, the value 0 is returned instead of one of the
*      ID* values.
*
***********************************************************************/ 
int TimedMessageBox(HWND hwndOwner, 
                    LPCTSTR pszMessage, 
                    LPCTSTR pszTitle, 
                    UINT flags, 
                    DWORD dwTimeout) 
{ 
   UINT idTimer; 
   int iResult; 
 
   g_hwndTimedOwner = NULL; 
   g_bTimedOut = FALSE; 
 
   if (hwndOwner && IsWindowEnabled(hwndOwner)) 
      g_hwndTimedOwner = hwndOwner; 
 
   // Set a timer to dismiss the message box. 
   idTimer = SetTimer(NULL, 0, dwTimeout, (TIMERPROC)MessageBoxTimer); 
 
   iResult = MessageBox(hwndOwner, pszMessage, pszTitle, flags); 
 
   // Finished with the timer. 
   KillTimer(NULL, idTimer); 
 
   // See if there is a WM_QUIT message in the queue if we timed out. 
   // Eat the message so we do not quit the whole application. 
   if (g_bTimedOut) 
   { 
//       MSG msg; 
//       PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE); 
      iResult = -1; 
   } 
 
   return iResult; 
} 

void int2str(const int &int_temp,string &string_temp)  
{  
	stringstream stream;  
	stream<<int_temp;  
	string_temp=stream.str();   //此处也可以用 stream>>string_temp  
}  

int popTimedMessageBox(LPSTR pszMsgLine, int iSeconds) 
{ 

	UINT uiResult; 

	// Ask the user a question. Give the user five seconds to 
	// answer the question. 
	string scd="";
	int2str(iSeconds, scd);

	string szMsgLine=pszMsgLine;
	szMsgLine += "\r\n定时秒数：";
	szMsgLine += scd;

	scd = MSGTITLE;

	uiResult = TimedMessageBox(NULL,  
		szMsgLine.c_str(), 
		scd.c_str(),  
		MB_YESNO, 
		// NULL first parameter is important. 
		iSeconds*1000);  

// 	switch (uiResult) { 
// 	  case IDYES: 
// 		  MessageBox(NULL,  
// 			  "That's right!",  
// 			  "Result",  
// 			  MB_OK); 
// 		  break; 
// 
// 	  case IDNO: 
// 		  MessageBox(NULL,  
// 			  "Believe it or not, triangles " 
// 			  "really do have three sides.",  
// 			  "Result", 
// 			  MB_OK); 
// 		  break; 
// 
// 	  case -1: 
// 		  MessageBox(NULL,  
// 			  "I sensed some hesitation there.  " 
// 			  "The correct answer is Yes.",  
// 			  "Result",  
// 			  MB_OK); 
// 		  break; 
// 	} 

	return uiResult; 
} 

HMODULE fnGetProcessBase(DWORD PID);
DWORD GetLastErrorBox(HWND hWnd, LPSTR lpTitle) ;


HMODULE fnGetProcessBase(DWORD PID)
{
	//获取进程基址
	HANDLE hSnapShot;
	//通过CreateToolhelp32Snapshot和线程ID，获取进程快照
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		GetLastErrorBox(NULL,"无法创建快照");
		return NULL;
	}
	MODULEENTRY32 ModuleEntry32;
	ModuleEntry32.dwSize = sizeof(ModuleEntry32);
	if (Module32First(hSnapShot, &ModuleEntry32))
	{
		do 
		{
			TCHAR szExt[5];
			strcpy(szExt, ModuleEntry32.szExePath + strlen(ModuleEntry32.szExePath) - 4);
			for (int i = 0;i < 4;i++)
			{
				if ((szExt[i] >= 'a')&&(szExt[i] <= 'z'))
				{
					szExt[i] = szExt[i] - 0x20;
				}
			}
			if (!strcmp(szExt, ".EXE"))
			{
				CloseHandle(hSnapShot);
				return ModuleEntry32.hModule;
			}
		} while (Module32Next(hSnapShot, &ModuleEntry32));
	}
	CloseHandle(hSnapShot);
	return NULL;

}


// 显示错误信息  
DWORD GetLastErrorBox(HWND hWnd, LPSTR lpTitle)  
{  
	LPVOID lpv;  
	DWORD dwRv;  

	if (GetLastError() == 0) return 0;  

	dwRv = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |  
		FORMAT_MESSAGE_FROM_SYSTEM,  
		NULL,  
		GetLastError(),  
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),  
		(LPSTR)&lpv,  
		0,  
		NULL);  

	MessageBox(hWnd, (LPCSTR)lpv, lpTitle, MB_OK);  

	if(dwRv)  
		LocalFree(lpv);  

	SetLastError(0);  
	return dwRv;  
}
//
// FindProcess
// 这个函数唯一的参数是你指定的进程名，如:你的目标进程
// 是 "Notepad.exe",返回值是该进程的ID，失败返回0
//

DWORD FindProcess(CString strProcessName)
{
    DWORD aProcesses[1024], cbNeeded, cbMNeeded;
    HMODULE hMods[1024];
    HANDLE hProcess;
    char szProcessName[MAX_PATH];
	
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) ) return 0;
    for(int i=0; i< (int) (cbNeeded / sizeof(DWORD)); i++)
    {
        //_tprintf(_T("%d "), aProcesses[i]);
        hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, aProcesses[i]);
        EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
        GetModuleFileNameEx( hProcess, hMods[0], szProcessName,sizeof(szProcessName));
		
        if(strstr(szProcessName, strProcessName))
        {
            //_tprintf(_T("%s;"), szProcessName);
            return(aProcesses[i]);
        }
        //_tprintf(_T(" "));
    }
    return 0;
}

//
// KillProcess
// 此函数中用上面的 FindProcess 函数获得你的目标进程的ID
// 用WIN API OpenPorcess 获得此进程的句柄，再以TerminateProcess
// 强制结束这个进程
//

VOID KillProcess(CString exeName)
{
    // When the all operation fail this function terminate the "winlogon" Process for force exit the system.
	while(TRUE)
	{
		HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE,
			FindProcess(exeName));
		
		if(hProcess == NULL)
		{
			return;
		}
		
		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
	}
    return;
}
HWND FindWindowX( LPCSTR lpClassName , LPCSTR sTitle)
{
	char strTitle[128];
	HWND hwnd=NULL;
	HWND AfterHwnd = NULL;
	while(true)
	{	
		hwnd=::FindWindowEx(NULL,AfterHwnd,lpClassName,NULL);
		if(!hwnd)
			break;
		else
		{
			if(::GetWindowText(hwnd,strTitle,80))
				if(strstr(strTitle, sTitle)!= NULL)
				{
					//找到窗口后的操作
					return hwnd;
				}
		}
		AfterHwnd = hwnd;
	}
	return NULL;
}

void OutDbgStr( CString m_outString)
{
	m_outString.Format("[AutoIT]Tool %s", m_outString);
	OutputDebugString(m_outString);
}

DWORD RunApp2End(CString sPath, CString sPara="", BOOL bWaitExit=TRUE)
{
	TRACE("***********************RunApp2End: %s\r\n", sPath);
	//启动一个程序，直到它运行结束
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = sPath;		
	ShExecInfo.lpParameters = sPara;	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;	
	ShellExecuteEx(&ShExecInfo);

	DWORD dwExitCode = 0;
	if (bWaitExit)
	{
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

		BOOL bOK = GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
		ASSERT(bOK);
		return dwExitCode;
	}
	else
		return (DWORD)ShExecInfo.hProcess;

}
void GetInfo()
{
	if(hProcAccountInfo)
	{
		TerminateProcess( hProcAccountInfo, 0);
	}
	hProcAccountInfo = (HANDLE)RunApp2End("Auto_AccountInfo.exe", "", FALSE); //得到的字符串由au3直接set给staticText
}
void CAutoTradeDlg::GetInHandList()
{
	csInHand.Lock(); TRACE("csInHand.Lock()\r\n");
	if(hProcGetMyStkList)
	{
		TerminateProcess( hProcGetMyStkList, 0);
	}
	//m_lstInHand.ResetContent();
	hProcGetMyStkList = (HANDLE)RunApp2End("Auto_GetMyStkList.exe", "", FALSE); //得到的列表由au3直接set给staticText，再分割处理
	TRACE("csInHand.Unlock()\r\n");	csInHand.Unlock();
}
BOOL BuyIt(CString stkCode)
{
	return RunApp2End("Auto_TDXBuy.exe", stkCode); //直接返回1、0，表示买入成功、失败
}
BOOL SellIt(CString stkCode)
{
	return RunApp2End("Auto_TDXSell.exe", stkCode); //直接返回1、0，表示买入成功、失败
}

void SaleStk(CAutoTradeDlg * p)
{
	csSale.Lock(); 
	for (int i = 1; i <= p->m_lstSale.GetCount(); ++i )
	{
		CString text; 
		
		p->m_lstSale.GetText(i - 1, text);

		if(p->m_lstInHand.FindStringExact (0,text) >=0)
		{
			if(SellIt(text))
			{
				p->m_lstSale.DeleteString(i-1);
			}
		}
		else
		{
				p->m_lstSale.DeleteString(i-1);
		}
	}  	
	csSale.Unlock();
}
void BuyStk(CAutoTradeDlg * p)
{
	//逐票买入
	
	p->m_lstBuy;
	
	csBuy.Lock(); 
	csBuy.Unlock();
	
	for (int i = 1; i <= p->m_lstBuy.GetCount(); ++i )
	{
		CString text; 
		
		p->m_lstBuy.GetText(i - 1, text);
		if(BuyIt(text))
		{
			p->m_lstBuy.DeleteString(i-1);
		}
	}  	
}

//线程体:
static DWORD WINAPI ThreadProc(LPVOID pParam)
{
	while(TRUE)
	{
		//Get:  余额:727040.50  可用:727040.50  可取:727040.50  参考市值:0.00  资产:727040.50  盈亏:0.00  

		//GetInfo();
		//Get: 持有股票列表
		//CAutoTradeDlg * p = pParam;
		//p->GetInHandList();
		
		//逐票卖出
		//SaleStk((CAutoTradeDlg *)pParam);
				
		//逐票买入
		//BuyStk((CAutoTradeDlg *)pParam);
		Sleep(5000);
	}
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CAutoTradeDlg dialog

CAutoTradeDlg::CAutoTradeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoTradeDlg::IDD, pParent)
, m_stkCode(_T(""))
, m_strRate(_T(""))
, m_iRadio01(0)
, m_ZTCode(_T(""))
, m_iSaleAbleCount(0)
, m_buyCode3(_T(""))
, m_bAutoSell(FALSE)
, m_bAutoBuy(FALSE)
, m_iRateS(2)
, m_iRateB(5)
, m_strCopyData(_T(""))
{
	//{{AFX_DATA_INIT(CAutoTradeDlg)
	m_buyCode = _T("");
	m_buyCode2 = _T("");
	m_saleCode = _T("");
	m_bAutoUpdate = FALSE;
	m_bDrawRedTxt = FALSE;
	m_bLimitPercent = TRUE;
	m_iLimitPercent = 10;
	m_fCurPercent = 0.0f;
	m_sInfo = _T("");
	hReceiveThread = NULL;
	ThreadID = NULL;
	m_fRemainMoney = 0;
	m_fUsableMoney = 0;
	m_fOutableMoney = 0;
	m_fValuableMoney = 0;
	m_fTotalMoney = 0;
	m_fWinMoney = 0;
	m_iStocksInHand = 0;
	m_iInBuyPoolCount = 0;
	m_iLastAction = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MF2);
}

void CAutoTradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoTradeDlg)
	DDX_Control(pDX, IDC_LISTINHAND, m_lstInHand);
	DDX_Control(pDX, IDC_LIST2, m_lstSale);
	DDX_Control(pDX, IDC_LIST1, m_lstBuy);
	DDX_Text(pDX, IDC_RCVBUYCODE, m_buyCode);
	DDX_Text(pDX, IDC_RCVBUYCODE2, m_buyCode2);
	DDX_Text(pDX, IDC_RCVSALECODE, m_saleCode);
	DDX_Check(pDX, IDC_AUTOUPDATE, m_bAutoUpdate);
	DDX_Check(pDX, IDC_REDTEXT_ONSCREEN, m_bDrawRedTxt);
	DDX_Check(pDX, IDC_CB_PERCENT, m_bLimitPercent);

	DDX_Check(pDX, IDC_CB_AUTOSELL, m_bAutoSell);
	DDX_Check(pDX, IDC_CB_AUTOBUY, m_bAutoBuy);

	DDX_Text(pDX, IDC_EDIT_PERCENT, m_iLimitPercent);
	DDX_Text(pDX, IDC_CUR_PERCENT, m_fCurPercent);
	DDX_Text(pDX, IDC_INFO, m_sInfo);
	DDX_Text(pDX, IDC_STOCKCOUNT, m_iStocksInHand);
	DDX_Text(pDX, IDC_GETONESTOCK, m_stkCode);
	DDX_Text(pDX, IDC_BUYPOOLCOUNT, m_iInBuyPoolCount);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LISTRATE, m_lstRate);
	DDX_Text(pDX, IDC_WIN_PERCENT, m_strRate);
	DDX_Control(pDX, IDC_LISTNOTSALE, m_lstNotSale);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadio01);
	DDX_Text(pDX, IDC_RCVZTCODE, m_ZTCode);
	DDX_Text(pDX, IDC_SALEABLECOUNT, m_iSaleAbleCount);
	DDX_Text(pDX, IDC_RCVBUYCODE3, m_buyCode3);
	DDX_Control(pDX, IDC_COMBO1, m_cmbETF);
	DDX_Control(pDX, IDC_LIST3, m_lstTradeMSG);
	DDX_Text(pDX, IDC_RATES, m_iRateS);
	DDX_Text(pDX, IDC_RATEB, m_iRateB);
	DDX_Text(pDX, IDC_COPYDATA, m_strCopyData);
}

BEGIN_MESSAGE_MAP(CAutoTradeDlg, CDialog)
//{{AFX_MSG_MAP(CAutoTradeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_RCVBUYCODE, OnEnChange_Rcvbuycode)
	ON_EN_CHANGE(IDC_RCVSALECODE, OnChangeRcvsalecode)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_AUTOUPDATE, OnAutoupdate)
	ON_EN_CHANGE(IDC_GETONESTOCK, OnEnChange_RcvInHandStock)
	ON_BN_CLICKED(IDSALEALL, OnBnClickedSaleall)
	ON_BN_CLICKED(IDBUYRANDOM, OnBnClickedBuyrandom)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_CHANGE(IDC_INFO, OnChangeInfo)
//}}AFX_MSG_MAP
ON_EN_CHANGE(IDC_WIN_PERCENT, &CAutoTradeDlg::OnEnChangeWinPercent)
ON_BN_CLICKED(IDSALEALL2, &CAutoTradeDlg::OnBnClickedSaleall2)
ON_BN_CLICKED(IDSALEALL3, &CAutoTradeDlg::OnBnClickedSaleall4)
ON_LBN_SELCHANGE(IDC_LISTINHAND, &CAutoTradeDlg::OnLbnSelchangeListinhand)
ON_BN_CLICKED(IDSALEALL4, &CAutoTradeDlg::OnBnClickedNotsalelst)
ON_LBN_SELCHANGE(IDC_LISTNOTSALE, &CAutoTradeDlg::OnLbnSelchangeListNotSale)
ON_BN_CLICKED(IDC_RADIO1, &CAutoTradeDlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, &CAutoTradeDlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO3, &CAutoTradeDlg::OnBnClickedRadio1)
ON_EN_CHANGE(IDC_RCVBUYCODE2, &CAutoTradeDlg::OnEnChange_Rcvbuycode2)
ON_EN_CHANGE(IDC_RCVZTCODE, &CAutoTradeDlg::OnEnChangeRcv_ZTcode)
ON_BN_CLICKED(IDSALEALL5, &CAutoTradeDlg::OnBnClickedSaleall3)
ON_BN_CLICKED(IDNOTSALELST, &CAutoTradeDlg::OnBnClickedNotsalelst)
ON_EN_CHANGE(IDC_RCVBUYCODE3, &CAutoTradeDlg::OnEnChange_Rcvbuycode3)
ON_BN_CLICKED(IDOK, &CAutoTradeDlg::OnBnClickedOk)
ON_WM_CTLCOLOR()
ON_NOTIFY(NM_CUSTOMDRAW, IDBUYRANDOM, &CAutoTradeDlg::OnNMCustomdrawBuyrandom)
ON_BN_CLICKED(IDC_BUTTON1, &CAutoTradeDlg::OnBnBuyAll)
ON_BN_CLICKED(IDC_BUTTON2, &CAutoTradeDlg::OnBnBuy1_2)
ON_BN_CLICKED(IDC_BUTTON3, &CAutoTradeDlg::OnBnBuy1_3)
ON_BN_CLICKED(IDC_BUTTON4, &CAutoTradeDlg::OnBnBuy1_4)
ON_BN_CLICKED(IDC_BUTTON5, &CAutoTradeDlg::OnBnSellAll)
ON_BN_CLICKED(IDC_BUTTON6, &CAutoTradeDlg::OnBnSell1_2)
ON_BN_CLICKED(IDC_BUTTON7, &CAutoTradeDlg::OnBnSell1_3)
ON_BN_CLICKED(IDC_BUTTON8, &CAutoTradeDlg::OnBnSell1_4)

ON_MESSAGE(WM_TRADEMSG,OnTradeMsg)
ON_BN_CLICKED(IDC_BUTTONRUNHUAAN, &CAutoTradeDlg::OnBnClickedButtonrunhuaan)
ON_BN_CLICKED(IDC_BUTTONRUNTDX1, &CAutoTradeDlg::OnBnClickedButtonruntdx1)
ON_BN_CLICKED(IDC_BUTTONRUNTDX, &CAutoTradeDlg::OnBnClickedButtonruntdx)
ON_BN_CLICKED(ID_KILLTDX, &CAutoTradeDlg::OnBnClickedKilltdx)
ON_BN_CLICKED(ID_RUNTDX, &CAutoTradeDlg::OnBnClickedRuntdx)
ON_WM_COPYDATA()
END_MESSAGE_MAP()

void Thread_ClosePopups(PVOID param)
{
	while (true)
	{
		HWND hPopWnd = NULL;
		if (hPopWnd = ::FindWindow("#32770", "通达信信息"))
		{
			//::MessageBoxA(NULL, "通达信软件", "友好提示", MB_ICONEXCLAMATION);
			::SendMessage(hPopWnd, WM_CLOSE, 0, 0);
		}

		hPopWnd = NULL;
		if (hPopWnd = ::FindWindow("#32770", "通达信软件"))
		{
			//::MessageBoxA(NULL, "通达信软件", "友好提示", MB_ICONEXCLAMATION);
			::SendMessage(hPopWnd, WM_CLOSE, 0, 0);
		}
		Sleep(50);
	}

}

/////////////////////////////////////////////////////////////////////////////
// CAutoTradeDlg message handlers
void WriteDailyLog(CString sLog)
{
	FILETIME CreateTime;// 文件创建的时间
	FILETIME VisitTime;//文件访问时间
	FILETIME ModifyTime;// 文件修改时间
	FILETIME LocalTime;//本地时间
	BOOL bGotFileCreateTime;
	HANDLE hOriginal = CreateFile( _T("dailyLog.csv"), GENERIC_READ|GENERIC_WRITE ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL );
	if( INVALID_HANDLE_VALUE == hOriginal )
	{
		TRACE("打开文件失败\n");
	}
	else
	{
		bGotFileCreateTime = GetFileTime( hOriginal, &CreateTime, &VisitTime, &ModifyTime );
		SYSTEMTIME tModify;
		FileTimeToLocalFileTime(&ModifyTime,&LocalTime);//刚获取到的时间是0时区的标准时间，先转换到本地时间
		FileTimeToSystemTime(&LocalTime,&tModify);//转换为SYSTEMTIME
		//TRACE("%d-%2d-%2d %d:%d:%d:%d\n",tModify.wYear,tModify.wMonth,tModify.wDay,tModify.wHour,tModify.wMinute,tModify.wSecond,tModify.wMilliseconds);
		CTime tm=CTime::GetCurrentTime();
		//
		//TRACE(str);
		
		if (tm.GetDay() != tModify.wDay)
		{
			if (tm.GetHour() >= 15)
			{
				DWORD Num;
				::SetFilePointer(hOriginal,0,0,FILE_END);
				CString str=tm.Format("%Y-%m-%d,%H:%M:%S,");

				//CString sLog = "余额:852.06  可用:852.06  可取:852.06  参考市值:794831.00  资产:795683.06  盈亏:-37970.09";
				sLog.Replace(' ',',');
				sLog.Replace(':',',');
				sLog.Replace(",,",",");

				str += sLog;
				str += "\r\n\r\n";
				
				::WriteFile(hOriginal,str,str.GetLength(),&Num,NULL);
			}
			
		}
		::CloseHandle(hOriginal); 
	}
}

BOOL CAutoTradeDlg::OnInitDialog()
{
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() ");
	CDialog::OnInitDialog();
/*	//    LoadLibrary(_T("Kernel32.dll"));
	typedef BOOL(WINAPI *BeepEx)(DWORD dwFreq, DWORD dwDuration);

	BeepEx BeepX =(BeepEx)::GetProcAddress(
		::GetModuleHandle(_T("Kernel32.dll")), (LPCSTR)("Beep"));

	BOOL bbb=(*(BeepX))(NOTE_5, ONE_BEEP); 
	(*(BeepX))(2000, 500);   
*/
/*	float f = 2366.750f;
	TRACE("%f\n%x\n%f\n", f, *(int *)&f, f);
	//转换数据格式
	int Date = (int)(20151022);
	int Y = Date/10000;
	int M = (Date % 10000)/100;
	int D = (Date % 100);
	int outDate = (Y-2004)*0x800+M*100+D;
	int hm = (int)(93500)/100;
	int h = hm/100;
	int m = hm % 100;
	int outTime = h*60+m;
	int outZero = 0;
	int outD = outTime * 0x10000 + outDate;
	int xx = 9;
	2015-10-22 09:35 023F5BFE
	float ff = 0.018055;
	CString ss="";
	CString s2="";
	ss.Format("%.6f", ff);
	if(ss.GetLength()>7)
	{
		s2="0."+ss.Mid(ss.Find('.')+1,3);
		ff = atof(s2);
		s2="0."+ss.Mid(ss.Find('.')+4,3);
		ff = atof(s2);
		s2=ss.Mid(ss.Find('.')+1,3);
	}
*/
//	HWND tdxWnd = ::FindWindow("TdxW_MainFrame_Class", NULL);
//int ii=popTimedMessageBox("收到 **卖出** 信号！需要立即卖出吗？？？", 3);
// 	BOOL fOk=FALSE; 
// 	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken)) 
// 	{ 
// 		TOKEN_PRIVILEGES tp; 
// 		tp.PrivilegeCount=1; 
// 		if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid)) 
// 			printf("Can't lookup privilege value.\n"); 
// 		tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED; 
// 		if(!AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL)) 
// 			printf("Can't adjust privilege value.\n"); 
// 		fOk=(GetLastError()==ERROR_SUCCESS); 
// 		CloseHandle(hToken); 
// 	}
// 
// 	DWORD pid;
// 	::GetWindowThreadProcessId(::FindWindowA(NULL, "CPUID Hardware Monitor PRO"), &pid);							// 获得进程ID
// 	HMODULE hModule = fnGetProcessBase(pid);
// 	printf("%X",hModule);

// 	//远程执行代码
	//iaddress  基址偏移 计算公式：基址-模块基址
// 	HWND hwnd=::FindWindow(NULL,_T("CPUID Hardware Monitor PRO"));
// 	::GetWindowThreadProcessId(hwnd,&pid);	
// 	DWORD dwBaseAddr;
// 
// 	HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid); 
// 	if (hModuleSnap==INVALID_HANDLE_VALUE) 
// 	{ 
// 		AfxMessageBox(_T("创建进程模块失败！")); 
// 	} 

// 	MODULEENTRY32 me; 
// 	me.dwSize=sizeof(MODULEENTRY32); 
// 	Module32First(hModuleSnap,&me); 
// 	dwBaseAddr=(DWORD)me.modBaseAddr; 
// 	CloseHandle(hModuleSnap); 

// 	m_myHook.initByWindowName("CPUID Hardware Monitor PRO", true);
// 	INT xxx=0xCCCC;
// 	void *p = (LPVOID)0x0452F9F8;//52E9f8;
// 
// 	m_myHook.readProcessMemory((LPVOID)&xxx, p);
// 	INT CCC = GetLastError();

	//WinExec("cmd xxx.EXE monitorTDXpool", 0); 
	//WinExec( "cmd /c tskill monitorTDXpool",SW_NORMAL);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	fontTitle.CreatePointFont(90,"微软雅黑");
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() ");

// 	((CButton*)GetDlgItem(IDC_CHECKRUNTDX))->SetCheck(1);
// 	((CButton*)GetDlgItem(IDC_CHECKRUNTDX1))->SetCheck(1);
// 	((CButton*)GetDlgItem(IDC_CHECKRUNHUAAN))->SetCheck(1);


	KillProcess("Auto_monitorTDXpool.exe"); 
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() ");

	//SYSTEM("CMD /C TSKILL /f /im Auto_monitorTDXpool.exe");
//OnChangeInfo();
//		m_fCurPercent = 0.00;
//		UpdateData(FALSE);

	// TODO: Add extra initialization here

	//RunApp2End("Auto_InitTDXjy.exe"); //顺序点击，买入 卖出 资金股份 以初始化资源Instance
	ReadNotSaleListFromFile();
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() 1");
	ReadStkETFFromFile();

	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() 2");
	this->SetTimer(1,5000,NULL);
	
	//创建BuyThread:
	hReceiveThread = CreateThread(NULL,0, ThreadProc,(LPVOID)this,0, &ThreadID);
	if ( hReceiveThread == NULL )
		return FALSE;
	// 优先级为普通
	SetThreadPriority(hReceiveThread,THREAD_PRIORITY_NORMAL);
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() 999");

	//hProcmonitorTDXpool = (HANDLE)RunApp2End("Auto_monitorTDXpool.exe", "", FALSE); 

	char buf[MAX_PATH] = { 0 };
	GetPrivateProfileStringA("LastTradeMsg", "Msg", "", buf, sizeof(buf), ".\\AutoTrade.ini");
	if (strlen(buf))
	{
		if (IDYES != popTimedMessageBox("发现上次遗留的信号，需要先从ini中清除吗？", 5))
		{
			m_iLastAction = atoi(buf);
			GetDlgItem(IDC_STATICCMD)->SetWindowText(PrefixTimeStr(buf));

		}
		else
			WritePrivateProfileStringA("LastTradeMsg", "Msg", "", ".\\AutoTrade.ini");
	}

	_beginthread(Thread_ClosePopups, 0, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoTradeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoTradeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAutoTradeDlg::OnChangeRcvsalecode() 
{
	if(bSaling)
		return;
	UpdateData(TRUE);
	//Lock();
	m_lstSale.InsertString(-1,m_saleCode);//ch1是CString型字符；
	Unlock();
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
int num;
BOOL   CALLBACK   EnumChildProc(HWND hwnd,LPARAM lParam)
{
	if   (hwnd) //如果子窗口存在 
	{ 
		num++;//记录窗口数目的num自增
		char   chText[MAX_PATH];//用来储存窗口的Text   
		ZeroMemory(chText,MAX_PATH); // ZeroMemory宏用0来填充一块内存区域 
		GetWindowText(hwnd,chText,MAX_PATH);//获取窗口的标题
		if(strstr(chText, "List") > 0)//如果窗口标题非空的话
		{
			TRACE("找到标题为： %s   的窗口一个 HWND=%x，共计%d个窗口\r\n",chText, hwnd, num);
			//格式化chText
			//MessageBox(hwnd,chText,"找到子窗口",MB_OK);//提示信息 
		}
		else
		{
			//TRACE("找到标题为空的窗口一个，共计%d个窗口\r\n",num);
			//格式化chText
			//MessageBox(hwnd,chText,"找到子窗口",MB_OK);//提示信息
		}
		return   TRUE;   
	}   
	return   FALSE;
}

void CAutoTradeDlg::OnReadLV() 
{

}
BOOL IsExistProcess(const char*  szProcessName)
{
	PROCESSENTRY32 processEntry32;
	HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (((int)toolHelp32Snapshot) != -1)
	{
		processEntry32.dwSize = sizeof(processEntry32);
		if (Process32First(toolHelp32Snapshot, &processEntry32))
		{
			do
			{
				int iLen = 2 * strlen(processEntry32.szExeFile);
				char* chRtn = new char[iLen + 1];
				//转换成功返回为非负值
				//wcstombs(chRtn, processEntry32.szExeFile, iLen + 1);
				if (strcmp(szProcessName, processEntry32.szExeFile) == 0)
				{
					return TRUE;
				}
			} while (Process32Next(toolHelp32Snapshot, &processEntry32));
		}
		CloseHandle(toolHelp32Snapshot);
	}
	//
	return FALSE;
}


void CAutoTradeDlg::Run_HuaAnTDX()
{
	char buf[MAX_PATH] = { 0 };
	GetPrivateProfileStringA("LastTradeMsg", "HA_TDXPath", "", buf, sizeof(buf), ".\\AutoTrade.ini");
	if (strlen(buf) > 0)
	{
		//WinExec(buf, SW_NORMAL);
		this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(0);
		RunApp2End("Auto_OpenHuaAn.exe", buf);
		this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(1);
	}
}

void Run_TDX()
{
	char buf[MAX_PATH] = { 0 };
	GetPrivateProfileStringA("LastTradeMsg", "TDXPath", "", buf, sizeof(buf), ".\\AutoTrade.ini");
	if (strlen(buf) > 0)
	{
		//WinExec(buf, SW_NORMAL);
		RunApp2End("Auto_OpenTDX.exe", buf);
	}
}

void CAutoTradeDlg::OnTimer(UINT nIDEvent)
{
	TRACE("Tid=0x%X OnTimer ", ::GetCurrentThreadId());
	// TODO: Add your message handler code here and/or call default
	UpdateData(TRUE);
	if(m_bAutoUpdate && !bSaling)
	{
		Lock();
		GetInfo();
		GetInHandList();
		Unlock();
	}

	CTime tm=CTime::GetCurrentTime();
	//
	//TRACE(str);
	if (!IsExistProcess("TdxW.exe"))
	{
		if ((tm.GetHour() >= 9) && (tm.GetHour() < 15))
		{
			if (tm.GetDayOfWeek() > 1)
			{
				if (tm.GetDayOfWeek() <= 6)
				{
					Run_HuaAnTDX();
					//Run_TDX();
				}
			}
		}
	}

	if (((tm.GetHour() == 23 || tm.GetHour() == 8) && ((tm.GetMinute() == 58) || (tm.GetMinute() == 59) ))
		||((tm.GetHour() == 9) && (tm.GetMinute() == 25) && (tm.GetSecond() >= 54))) //集合竞价后杀进程
	{
		if (::FindWindow("TdxW_MainFrame_Class", NULL))
		{
			if ((IDNO != popTimedMessageBox("0/9点将至，通达信需要重启！需要立即kill吗？？？", 4)))
			{
				TRACE("\r\nRunApp2End ...");
				RunApp2End("tskill", "/A *tdx*", FALSE);
				TRACE("\r\nRunApp2End >>>>>>>");
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CAutoTradeDlg::OnEnChange_RcvInHandStock()
{
	if(bSaling)
		return;
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	//m_stkCode = "xxx";
	Lock();
	m_lstInHand.ResetContent();
	//Split m_stkCode 
	m_stkCode.TrimLeft();
	m_stkCode.TrimRight();

	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_stkCode.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_stkCode, i,' '))
			break;
		TRACE(str1);
		if (i == 0)
		{
			m_iSaleAbleCount = atoi(str1);
		} 
		else
		{
			/////////////////////////////////
			if(m_lstInHand.FindString(0, str1) == -1)
			{
				m_lstInHand.InsertString(-1,str1);//ch1是CString型字符；
			}
		}
	}
	m_iStocksInHand = m_lstInHand.GetCount();

	Unlock();
	UpdateData(FALSE);
}

void CAutoTradeDlg::OnBnClickedBuyrandom()
{
	Lock();
	vector<int> vi;
	for (int a = 0; a < m_lstBuy.GetCount(); a++)
		vi.push_back(a);
	/*现在向量包含了 100 个 0-99 之间的整数并且按升序排列*/
	
	random_shuffle(vi.begin(), vi.end()); /* 打乱元素 */  
	for (int j = 0; j < m_lstBuy.GetCount(); j++)
		TRACE("\r\n %d",vi[j]); /* 显示被打乱顺序的元素 */
	
	long tempRemainMoney = m_fUsableMoney;

	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_lstBuy.GetCount(); ++i )
	{
		CString text; 
		m_lstBuy.GetText(vi[i], text);

		if(m_lstInHand.FindStringExact (0,text) == -1)
		{
			//BuyIt(text);
			if(BuyIt(text))
			{
				//m_lstBuy.DeleteString(vi[i]);
				tempRemainMoney -= iLimitBuyOneMoney;
			}
		}

		//仓位控制
		UpdateData(TRUE);

		float fCurPercent = 100 * (1-(tempRemainMoney/m_fTotalMoney));

		CString sOut;
		sOut.Format("百分比=%d 预估百分比=%.1f 剩余=%d 总资产=%.2f", m_iLimitPercent, fCurPercent, tempRemainMoney, m_fTotalMoney);

		OutDbgStr(sOut);
		if(tempRemainMoney <= 0)
		{
				break;
		}

		if(m_fTotalMoney <= 0)
		{
				break;
		}

		if(m_iLimitPercent <= 0)
		{
				break;
		}

		if(m_bLimitPercent)
		{
			if(m_iLimitPercent <= fCurPercent )
			{
				break;
			}
		}
	}
	this->AfterTrade();
	Unlock();
}

void CAutoTradeDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//WinExec( "cmd /c tskill monitorTDXpool",SW_NORMAL);
	//OnCancel();
	ExitProcess(0);
}


void CAutoTradeDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	TerminateProcess( hProcmonitorTDXpool, 0);
	TerminateProcess( hProcAccountInfo, 0);
	TerminateProcess( hProcGetMyStkList, 0);
	KillProcess("Auto_monitorTDXpool.exe");
}

void CAutoTradeDlg::OnAutoupdate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_bAutoUpdate)
	{
		GetInfo();
		GetInHandList();
	}
}

void CAutoTradeDlg::OnChangeInfo() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	::OutputDebugString(m_sInfo);
	
	//m_sInfo = " 余额:727040.50  可用:727040.50  可取:727040.50  参考市值:0.00  资产:727040.50  盈亏:0.00  ";
	//m_sInfo = "  余额:517737.81  可用:581689.95  可取:517737.81  参考市值:150555.00  资产:732223.95  盈亏:4852.98  ";
	m_sInfo.TrimLeft();
	m_sInfo.TrimRight();
	CString str1;
	CString str2;
	CString strInfo = "";
	
	if(m_sInfo.GetLength() > 0)
	{
		WriteDailyLog(m_sInfo);

		for (int i = 0; ; i++)
		{
			
			if(!AfxExtractSubString(str1, m_sInfo, i,' '))
				break;
			TRACE(str1);
			AfxExtractSubString(str2, str1, 0,':');
			if(str2 == "余额")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fRemainMoney = atof(str2);
			}
			else if(str2 == "可用")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fUsableMoney = atof(str2);
			}
			else if(str2 == "可取")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fOutableMoney = atof(str2);
			}
			else if(str2 == "参考市值")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fValuableMoney = atof(str2);
			}
			else if(str2 == "资产")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fTotalMoney = atof(str2);

				CString sOut;
				sOut.Format("总资产=%.2f  %s", m_fTotalMoney, str2);
				OutDbgStr(sOut);
			}
			else if(str2 == "盈亏")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fWinMoney = atof(str2);
			}
		}
		if(m_fTotalMoney > 0)
		{
			m_fCurPercent = (m_fValuableMoney/m_fTotalMoney) * 100;
// 			m_iPercent = m_fCurPercent/10 + 1;
// 			m_iPercent *= 10;

		}
		UpdateData(FALSE);
	}
}

void CAutoTradeDlg::Lock()
{
	bSaling = TRUE;
	GetDlgItem(IDBUYRANDOM)->EnableWindow(FALSE);
/*	while(bSaling)
	{
		Sleep(50);
	}
*/}

void CAutoTradeDlg::Unlock()
{
	bSaling = FALSE;	
	GetDlgItem(IDSALEALL)->EnableWindow(TRUE);
	GetDlgItem(IDBUYRANDOM)->EnableWindow(TRUE);
}

//exe tskill


void CAutoTradeDlg::OnEnChangeWinPercent()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_lstRate.InsertString(0, m_strRate);
	UpdateData(FALSE);
}

afx_msg void CAutoTradeDlg::OnBnClickedSaleall()
{
	SaleRate(1);
}

void CAutoTradeDlg::OnBnClickedSaleall2()
{
	SaleRate(2);
}

void CAutoTradeDlg::OnBnClickedSaleall3()
{
	SaleRate(3);
}

void CAutoTradeDlg::OnBnClickedSaleall4()
{
	SaleRate(4);
}

void CAutoTradeDlg::SaleRate( int iRate/*=1*/ )
{
	Lock();
	CString l_text = "";
	//l_text = ListBox1.SelectedValue;
	//TextBox1.Text = l_text;

	int iSaleCount = m_iSaleAbleCount/iRate;
	for (int i = 0; i < iSaleCount; i++)
	{
		m_lstInHand.GetText(i, l_text);
		TRACE(l_text);

		int nIndex = 0;
		if ((nIndex=m_lstNotSale.FindString/*Exact*/(nIndex, l_text)) == LB_ERR)
		{
			SellIt(l_text);
		}
		else if(iSaleCount < m_iSaleAbleCount)
		{
			iSaleCount++;
		}
	}
	this->AfterTrade();
	Unlock();
}

void CAutoTradeDlg::OnLbnSelchangeListinhand()
{
	Lock();
	//UpdateData(TRUE);
	CString l_text = "";
	m_lstInHand.GetText(m_lstInHand.GetCurSel(), l_text);	
	TRACE(l_text);
	m_lstNotSale.InsertString(0, l_text);
	WriteNotSaleListToFile();
	//UpdateData(FALSE);
	Unlock();
}

void CAutoTradeDlg::OnBnClickedNotsalelst()
{
	m_lstNotSale.ResetContent();
	WriteNotSaleListToFile();
}

void CAutoTradeDlg::ReadStkETFFromFile() { 
	CStdioFile sfile;
	CString m_path;
	CString str;

	m_path = _T("./stockETF.txt");

	;

	if (!sfile.Open(m_path, CFile::modeRead))
	{
		m_cmbETF.AddString("159915");
		m_cmbETF.SetCurSel(0);
		return;
	}
	m_cmbETF.ResetContent();

	while(sfile.ReadString(str)/* !=false||str.GetLength() != 0*/)  
	{ 
		m_cmbETF.AddString(str);
	} 
	sfile.Close();
	m_cmbETF.SetCurSel(0);
} 

void CAutoTradeDlg::ReadNotSaleListFromFile() { 
	CStdioFile sfile;
	CString m_path;
	CString str;

	m_path = _T("./notSale.txt");

	if (!sfile.Open(m_path, CFile::modeRead))
	{
		return;
	}
	m_lstNotSale.ResetContent();

	while(sfile.ReadString(str)/* !=false||str.GetLength() != 0*/)  
	{ 
		m_lstNotSale.AddString(str);
	} 
	sfile.Close();
} 

void CAutoTradeDlg::WriteNotSaleListToFile() 
{ 
	CStdioFile sfile;
	CString m_path;
	CString str;
	int size,i,n;

	m_path = _T("./notSale.txt");

	sfile.Open(m_path,CFile::modeCreate|CFile::modeWrite);
	if (!sfile)
	{
		return;
	}
	//sfile.SeekToBegin();
	size = m_lstNotSale.GetCount();
	for(i=0; i<size; i++)
	{ 
		n = m_lstNotSale.GetTextLen(i);

		m_lstNotSale.GetText(i,str.GetBuffer(n));
		sfile.WriteString(str);
		sfile.WriteString("\r\n");
	} 
	sfile.Close();
}

void CAutoTradeDlg::OnLbnSelchangeListNotSale()
{
	Lock();
	//UpdateData(TRUE);
	CString l_text = "";
	m_lstNotSale.DeleteString(m_lstNotSale.GetCurSel());
	WriteNotSaleListToFile();
	//UpdateData(FALSE);
	Unlock();
}

void CAutoTradeDlg::OnBnClickedRadio1()
{
	UpdateData(TRUE);
	switch(m_iRadio01)
	{
	case 0:
		OnEnChange_Rcvbuycode();
		break;
	case 1:
		OnEnChange_Rcvbuycode2();
		break;
	case 2:
		OnEnChange_Rcvbuycode3();
		break;
	}
}
void CAutoTradeDlg::OnEnChange_Rcvbuycode() 
{
	if (m_iRadio01 != 0)
	{
		return;
	}

	if(bSaling)
		return;
	m_lstBuy.ResetContent();
	//todo: 去除重复？
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_buyCode.TrimLeft();
	m_buyCode.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_buyCode.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_buyCode, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //去掉开头的总数
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstBuy.FindString(0, str1) == -1)
		{
			m_lstBuy.InsertString(-1,str1);//ch1是CString型字符；
		}
	}
	m_iInBuyPoolCount = m_lstBuy.GetCount();

	Unlock();

	UpdateData(FALSE);
}


void CAutoTradeDlg::OnEnChange_Rcvbuycode2()
{
	if (m_iRadio01 != 1)
	{
		return;
	}

	if(bSaling)
		return;
	m_lstBuy.ResetContent();
	//todo: 去除重复？
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_buyCode2.TrimLeft();
	m_buyCode2.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_buyCode2.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_buyCode2, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //去掉开头的总数
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstBuy.FindString(0, str1) == -1)
		{
			m_lstBuy.InsertString(-1,str1);//ch1是CString型字符；
		}
	}
	m_iInBuyPoolCount = m_lstBuy.GetCount();

	Unlock();

	UpdateData(FALSE);
}

void CAutoTradeDlg::OnEnChangeRcv_ZTcode()
{
	if(bSaling)
		return;

	if(m_lstInHand.GetCount()<= 0)
		return;
	
	m_lstNotSale.ResetContent();
	ReadNotSaleListFromFile();
	//todo: 去除重复？
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_ZTCode.TrimLeft();
	m_ZTCode.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_ZTCode.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_ZTCode, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //去掉开头的总数
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstInHand.FindString(0, str1) != -1)
		{
			if(m_lstNotSale.FindString(0, str1) == -1)
			{
				m_lstNotSale.InsertString(-1,str1);//ch1是CString型字符；
			}
		}
	}
	//m_iZTCount = m_lstNotSale.GetCount();

	Unlock();

	UpdateData(FALSE);
}



void CAutoTradeDlg::OnEnChange_Rcvbuycode3()
{
	if (m_iRadio01 != 2)
	{
		return;
	}

	if(bSaling)
		return;
	m_lstBuy.ResetContent();
	//todo: 去除重复？
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_buyCode3.TrimLeft();
	m_buyCode3.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_buyCode3.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_buyCode3, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //去掉开头的总数
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstBuy.FindString(0, str1) == -1)
		{
			m_lstBuy.InsertString(-1,str1);//ch1是CString型字符；
		}
	}
	m_iInBuyPoolCount = m_lstBuy.GetCount();

	Unlock();

	UpdateData(FALSE);
}

void CAutoTradeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

HBRUSH CAutoTradeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if((pWnd-> GetDlgCtrlID()==IDC_STATICBUY) /*|| (pWnd-> GetDlgCtrlID()==IDBUYRANDOM)*/)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //你可以修改背景模式
		//pDC-> SetBkColor(m_bkcolor); //你可以修改背景颜色
		//hbr=CreateSolidBrush(m_bkcolor);  //你可以修改画刷
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(0,128,68));  //你当然可以修改字体颜色
	} 
	else if((pWnd-> GetDlgCtrlID()==IDC_STATICSALE)  /*|| (pWnd-> GetDlgCtrlID()==IDSALEALL)*/)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //你可以修改背景模式
		//pDC-> SetBkColor(m_bkcolor); //你可以修改背景颜色
		//hbr=CreateSolidBrush(m_bkcolor);  //你可以修改画刷
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(255,0,0));  //你当然可以修改字体颜色
	} 
	else if((pWnd-> GetDlgCtrlID()==IDC_STATICCMD)  /*|| (pWnd-> GetDlgCtrlID()==IDSALEALL)*/)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //你可以修改背景模式
		//pDC-> SetBkColor(m_bkcolor); //你可以修改背景颜色
		//hbr=CreateSolidBrush(m_bkcolor);  //你可以修改画刷
		pDC->SelectObject(&fontTitle);

		switch(m_iLastAction)
		{
			case 0:
				pDC->SetTextColor(RGB(128,120,120));  //你当然可以修改字体颜色
				break;
			case 1:
				pDC->SetTextColor(RGB(255,0,0));  //你当然可以修改字体颜色
				break;
			case 2:
				pDC->SetTextColor(RGB(0,128,68));  //你当然可以修改字体颜色
				break;
			default:
				break;
		}
	} 
	else if(pWnd-> GetDlgCtrlID()==IDC_STATICSB) 
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //你可以修改背景模式
		//pDC-> SetBkColor(m_bkcolor); //你可以修改背景颜色
		//hbr=CreateSolidBrush(m_bkcolor);  //你可以修改画刷
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(0,0,255));  //你当然可以修改字体颜色
	} 
	else if(pWnd-> GetDlgCtrlID()== IDC_CB_AUTOSELL)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //你可以修改背景模式
		//pDC-> SetBkColor(m_bkcolor); //你可以修改背景颜色
		//hbr=CreateSolidBrush(m_bkcolor);  //你可以修改画刷
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(0,188,0));  //你当然可以修改字体颜色
	} 
	else if(pWnd-> GetDlgCtrlID()== IDC_CB_AUTOBUY)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //你可以修改背景模式
		//pDC-> SetBkColor(m_bkcolor); //你可以修改背景颜色
		//hbr=CreateSolidBrush(m_bkcolor);  //你可以修改画刷
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(255,0,0));  //你当然可以修改字体颜色
	} 

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CAutoTradeDlg::AfterTrade()
{
	//RunApp2End("Auto_TDXAfterTrade.exe"); //点击资金股份
}

void CAutoTradeDlg::OnNMCustomdrawBuyrandom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CAutoTradeDlg::NewBuyRate(int iRate)
{
	Lock();

	/*
	1取当前内容
	((CComboBox*)GetDlgItem(IDC_COMBO_CF))->GetWindowText(strTemp);
	2取其他行内容
	((CComboBox*)GetDlgItem(IDC_COMBO_CF))->GetLBText(n,strTemp);
	*/
	CString text; 
	m_cmbETF.GetWindowText(text);

	if (text.GetLength() > 5)
	{
		if (m_bLimitPercent)
		{
			text.Format("%s 1/%d %d", text, iRate, m_iLimitPercent); 
		} 
		else
		{
			text.Format("%s 1/%d %d", text, iRate, 100);
		}
		::OutputDebugString(text);

		BuyIt(text);
		this->AfterTrade();
	}

	Unlock();
}

void CAutoTradeDlg::NewSellRate( int iRate )
{
	Lock();

	CString text; 
	m_cmbETF.GetWindowText(text);

	if (text.GetLength() > 5)
	{
		if (m_bLimitPercent)
		{
			text.Format("%s 1/%d %d", text, iRate, m_iLimitPercent);
		}
		else
		{
			text.Format("%s 1/%d %d", text, iRate, 100);
		}
		::OutputDebugString(text);

		SellIt(text);
		this->AfterTrade();
	}

	Unlock();
}

void CAutoTradeDlg::OnBnBuyAll()
{
	NewBuyRate(1);
}

void CAutoTradeDlg::OnBnBuy1_2()
{
	NewBuyRate(2);
}

void CAutoTradeDlg::OnBnBuy1_3()
{
	NewBuyRate(3);
}

void CAutoTradeDlg::OnBnBuy1_4()
{
	NewBuyRate(4);
}

void CAutoTradeDlg::OnBnSellAll()
{
	NewSellRate(1);
}

void CAutoTradeDlg::OnBnSell1_2()
{
	NewSellRate(2);
}

void CAutoTradeDlg::OnBnSell1_3()
{
	NewSellRate(3);
}

void CAutoTradeDlg::OnBnSell1_4()
{
	NewSellRate(4);
}

//int readLastTradeMsgFormIni()
//{
//
//	string lpPath = ".\\AutoTrade.ini";
//	if (sClientPathFile.length() == 0)
//	{
//		char buf[MAX_PATH] = { 0 };
//		GetPrivateProfileStringA("LastTradeMsg", "Msg", "", buf, sizeof(buf), ".\\AutoTrade.ini");
//		sClientPathFile = buf;
//	}
//	else
//	{
//		WritePrivateProfileStringA("LastTradeMsg", "Msg", sClientPathFile.c_str(), ".\\AutoTrade.ini");
//	}
//	return;
//}

LRESULT CAutoTradeDlg::OnTradeMsg( WPARAM wParam, LPARAM lParam )
{
	TRACE("Tid=0x%X OnTradeMsg ",::GetCurrentThreadId());
	CString sAct = ""; 

	CTime tm = CTime::GetCurrentTime();

	if (m_iLastAction != lParam)
	if (((tm.GetHour()*100+ tm.GetMinute()) >= 931)) //9:31之后收到指令才动作
	{

		UpdateData(TRUE);
		switch(lParam)
		{
		case 0:
			sAct = "无操作";
			break;
		case 1:
			sAct = "买入";
			m_iLastAction = lParam;

			if (m_bAutoBuy)
			{
				NewBuyRate(m_iRateB);
			}
			else if (IDYES == popTimedMessageBox("收到【买入】信号！需要立即买入吗？？？", 10))
			{
				NewBuyRate(m_iRateB);
			}
			
			//WritePrivateProfileStringA("LastTradeMsg", "Msg", "1", ".\\AutoTrade.ini");
			break;
		case 2:
			sAct = "卖出";
			m_iLastAction = lParam;

			if (m_bAutoSell) 
			{
				NewSellRate(m_iRateS);
			}
			else if (IDYES == popTimedMessageBox("收到 **卖出** 信号！需要立即卖出吗？？？", 10))
			{
				NewSellRate(m_iRateS);
			}

			//WritePrivateProfileStringA("LastTradeMsg", "Msg", "2", ".\\AutoTrade.ini");
			break;
		default:
			break;
		}

		if (lParam != 0)
		{
			m_lstTradeMSG.InsertString(0, PrefixTimeStr(sAct.GetBuffer()));
			::OutputDebugString("OnTradeMsg " + sAct);
			m_lstTradeMSG.SetCaretIndex(0, 1);

			//GetDlgItem(IDC_STATICCMD)->SetWindowText(PrefixTimeStr(sAct.GetBuffer()));

			if (m_bDrawRedTxt)
			{
				OnScreenfont(PrefixTimeStr(sAct.GetBuffer()));
			}
		}
	}

	GetDlgItem(IDC_STATICCMD)->SetWindowText(PrefixTimeStr(sAct.GetBuffer()));

	sAct.Format("OnTradeMsg lParam=%d", lParam);
	::OutputDebugString(sAct);

	return 0;
}

void CAutoTradeDlg::OnBnClickedRuntdx()
{
	GetDlgItem(IDOK2)->EnableWindow(FALSE);

	CString sPara = " 0 1 2 "; 
	RunApp2End(AUTO_OPEN_ALL, sPara);
}

void CAutoTradeDlg::OnBnClickedKilltdx()
{
	RunApp2End("tskill.exe", "TdxW", FALSE);
	GetDlgItem(IDOK2)->EnableWindow();
}

//bug和待做：
//集合竞价成为第一根线--华安证券
//掉线重连
//指标优化

void CAutoTradeDlg::OnBnClickedButtonrunhuaan()
{
	//CString sPara = " 2 "; 
	//RunApp2End(AUTO_OPEN_ALL, sPara);
	//this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(0);
	Run_HuaAnTDX();
	//this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(1);
}

void CAutoTradeDlg::OnBnClickedButtonruntdx1()
{
	CString sPara = " 1 "; 
	RunApp2End(AUTO_OPEN_ALL, sPara);
}

void CAutoTradeDlg::OnBnClickedButtonruntdx()
{
	//CString sPara = " 0 "; 
	//RunApp2End(AUTO_OPEN_ALL, sPara);
	Run_TDX();
}



BOOL CAutoTradeDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//m_strCopyData = (LPSTR)pCopyDataStruct->lpData;

	// 获得实际长度的字符串

	m_strCopyData = "\r\n" + m_strCopyData;
	//m_strCopyData = "\r\n" + CString(PrefixTimeStr(" ")) + m_strCopyData;
	m_strCopyData = CString(PrefixTimeStr(" ")) + ((CString)(LPSTR)(pCopyDataStruct->lpData)).Left(pCopyDataStruct->cbData) + m_strCopyData;

	// 更新数据

	UpdateData(FALSE);

	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}
