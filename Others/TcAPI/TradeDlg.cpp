// TradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TradeDlg.h"


// CTradeDlg dialog

IMPLEMENT_DYNAMIC(CTradeDlg, CDialog)

CTradeDlg::CTradeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTradeDlg::IDD, pParent)
{

}

CTradeDlg::~CTradeDlg()
{
}

void CTradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTradeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTradeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTradeDlg message handlers

void CTradeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
