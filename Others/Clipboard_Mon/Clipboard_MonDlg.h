
// Clipboard_MonDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CClipboard_MonDlg �Ի���
class CClipboard_MonDlg : public CDialogEx
{
// ����
public:
	CClipboard_MonDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIPBOARD_MON_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
};
