
// Clipboard_MonDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Clipboard_Mon.h"
#include "Clipboard_MonDlg.h"
#include "afxdialogex.h"
#include "..\common\Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClipboard_MonDlg �Ի���



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


// CClipboard_MonDlg ��Ϣ�������

BOOL CClipboard_MonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	/*
	* ���ô���Ϊ���㴰��
	*/
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	this->ShowWindow(SW_HIDE);
	HWND m_hNext;
	m_hNext = ::SetClipboardViewer(this->GetSafeHwnd());  //�������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClipboard_MonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClipboard_MonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClipboard_MonDlg::OnDrawClipboard()
{
	CDialogEx::OnDrawClipboard();

	// TODO: �ڴ˴������Ϣ����������
	//Sleep(100);
	string sDone = readTxt("D:\\My_Data\\downX.ini");

	::OpenClipboard(GetSafeHwnd());

	//�жϼ���������ݸ�ʽ�Ƿ���Դ���
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
			m_OK.SetWindowTextA("��������");
			//return;
		}
		else
		{
			m_InDoneList.ShowWindow(SW_HIDE);
			m_OK.SetWindowTextA("����");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CClipboard_MonDlg::OnBnClickedAbort()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->ShowWindow(SW_HIDE);
}


void CClipboard_MonDlg::OnBnClickedOk2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	//UDP BC the Url and Name
}
