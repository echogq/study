#pragma once

#include "stdafx.h"
#include "resource.h"
// CTradeDlg dialog

class CTradeDlg : public CDialog
{
	DECLARE_DYNAMIC(CTradeDlg)

public:
	CTradeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTradeDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
