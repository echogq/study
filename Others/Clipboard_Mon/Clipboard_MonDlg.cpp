
// Clipboard_MonDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Clipboard_Mon.h"
#include "Clipboard_MonDlg.h"
#include "afxdialogex.h"
#include "..\common\Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClipboard_MonDlg 对话框



CClipboard_MonDlg::CClipboard_MonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIPBOARD_MON_DIALOG, pParent)
	, m_sCMD(_T(""))
	, m_sName(_T(""))
	, m_smmUrl(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClipboard_MonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sCMD);
	DDX_Control(pDX, IDC_STATIC2, m_InDoneList);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Text(pDX, IDC_EDIT2, m_sName);
	DDX_Text(pDX, IDC_EDIT3, m_smmUrl);
}

BEGIN_MESSAGE_MAP(CClipboard_MonDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DRAWCLIPBOARD()
	ON_BN_CLICKED(IDOK, &CClipboard_MonDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CClipboard_MonDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDABORT, &CClipboard_MonDlg::OnBnClickedAbort)
	ON_BN_CLICKED(IDOK2, &CClipboard_MonDlg::OnBnClickedOk2)
END_MESSAGE_MAP()


// CClipboard_MonDlg 消息处理程序

BOOL CClipboard_MonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	/*
	* 设置窗口为顶层窗口
	*/
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	this->ShowWindow(SW_HIDE);
	HWND m_hNext;
	m_hNext = ::SetClipboardViewer(this->GetSafeHwnd());  //监控链表

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClipboard_MonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClipboard_MonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClipboard_MonDlg::OnDrawClipboard()
{
	CDialogEx::OnDrawClipboard();

	// TODO: 在此处添加消息处理程序代码
	//Sleep(100);
	string sDone = readTxt("D:\\My_Data\\downX.ini");

	::OpenClipboard(GetSafeHwnd());

	//判断剪贴板的数据格式是否可以处理。
// 	if (IsClipboardFormatAvailable(CF_UNICODETEXT))
// 		Sleep(100);
// 	if (IsClipboardFormatAvailable(CF_TEXT))
// 		Sleep(100);

	HANDLE handleData = GetClipboardData(CF_TEXT);
	m_sCMD = ((char*)handleData);
	::CloseClipboard();
	//this->ShowWindow(SW_HIDE);

	if (strstr(m_sCMD, "://") && strstr(m_sCMD, "*"))
	{
		CString sUrl, sMp4Name;
		AfxExtractSubString(sUrl, m_sCMD, 0, '*');
		AfxExtractSubString(sMp4Name, m_sCMD, 1, '*');

		sUrl.Replace("240,360,480,720", "720");
		sUrl.Replace("720,1080", "1080");

		CString sSep = " Porn Video ";
		if (strstr(sMp4Name, sSep))
		{
			sMp4Name = strstr(sMp4Name, sSep) + strlen(sSep);
		}

		if (sDone.find(sMp4Name, 0) != sDone.npos)
		{
			m_InDoneList.ShowWindow(SW_SHOW);
			m_OK.SetWindowTextA("重新下载");
			//return;
		}
		else
		{
			m_InDoneList.ShowWindow(SW_HIDE);
			m_OK.SetWindowTextA("下载");
			//return;
		}

		m_sCMD = "ffmpeg -i \"" + sUrl + "\" -c copy \"z:\\" + sMp4Name + ".mkv\" ";
		m_sName = sMp4Name;
		m_smmUrl = sUrl;

		TRACE("\r\n%s\r\n", m_sCMD);
		UpdateData(FALSE);
		this->ShowWindow(SW_SHOWNORMAL);
	}
}


void CClipboard_MonDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();

	UpdateData(TRUE);

	std::ofstream fOut("z:\\FFmpegTMP.cmd");
	if (fOut)
	{
		//std::cout << "Open output file faild." << std::endl;
		fOut << m_sCMD << std::endl;
		fOut << "pause" << std::endl;
		fOut.close();

		WinExec("z:\\FFmpegTMP.cmd", SW_SHOWNORMAL);
		DWORD xx = ::GetLastError();
		this->ShowWindow(SW_HIDE);
	}

}


void CClipboard_MonDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CClipboard_MonDlg::OnBnClickedAbort()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ShowWindow(SW_HIDE);
}


void CClipboard_MonDlg::OnBnClickedOk2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	//UDP BC the Url and Name
}
