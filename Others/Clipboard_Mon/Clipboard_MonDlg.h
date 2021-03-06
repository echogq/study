
// Clipboard_MonDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CClipboard_MonDlg 对话框
class CClipboard_MonDlg : public CDialogEx
{
// 构造
public:
	CClipboard_MonDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIPBOARD_MON_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawClipboard();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedAbort();
	CString m_sCMD;
	CStatic m_InDoneList;
	CButton m_OK;
	CString m_sName;
	CString m_smmUrl;
	afx_msg void OnBnClickedOk2();
};
