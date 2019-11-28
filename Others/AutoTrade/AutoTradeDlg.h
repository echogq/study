// AutoTradeDlg.h : header file
//

#if !defined(AFX_AUTOTRADEDLG_H__E1607545_48DB_4E63_A209_766C9E09A272__INCLUDED_)
#define AFX_AUTOTRADEDLG_H__E1607545_48DB_4E63_A209_766C9E09A272__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxmt.h"
#include "psapi.h"  
#include "afxwin.h"
#include "myhook.h"

#pragma comment(lib,"psapi.lib")

#define WM_TRADEMSG WM_USER+1
#define MSGTITLE "倒计时结束后自动本窗口自动关闭"


/////////////////////////////////////////////////////////////////////////////
// CAutoTradeDlg dialog

class CAutoTradeDlg : public CDialog
{
// Construction
public:
	CAutoTradeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoTradeDlg)
	enum { IDD = IDD_AUTOTRADE_DIALOG };
	CListBox	m_lstInHand;
	CListBox	m_lstSale;
	CListBox	m_lstBuy;
	CListBox	m_lstRate;
	CListBox	m_lstNotSale;
	CString	m_buyCode;
	CString	m_buyCode2;
	CString	m_saleCode;
	BOOL	m_bAutoUpdate;
	BOOL	m_bLimitPercent;
	int		m_iLimitPercent;
	double	m_fCurPercent;
	CString	m_sInfo;
	int		m_iStocksInHand;
	int		m_iInBuyPoolCount;
	int m_iRadio01;
	//}}AFX_DATA
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoTradeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	HANDLE hReceiveThread;
	ULONG ThreadID;
	//余额:727040.50  可用:727040.50  可取:727040.50  参考市值:0.00  资产:727040.50  盈亏:0.00
	double m_fRemainMoney;
	double m_fUsableMoney;
	double m_fOutableMoney;
	double m_fValuableMoney;
	double m_fTotalMoney;
	double m_fWinMoney;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAutoTradeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnChange_Rcvbuycode();
	afx_msg void OnChangeRcvsalecode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnAutoupdate();
	afx_msg void OnChangeInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnReadLV();
	void Run_HuaAnTDX();
public:
	afx_msg void OnEnChange_RcvInHandStock();
	void GetInHandList();
	void SaleRate( int iRate =1 );
	CString m_stkCode;
	afx_msg void OnBnClickedSaleall();
	afx_msg void OnBnClickedBuyrandom();
	afx_msg void OnBnClickedCancel();
	void Lock();
	void Unlock();
	afx_msg void OnEnChangeWinPercent();
	CString m_strRate;
	afx_msg void OnBnClickedSaleall2();
	afx_msg void OnBnClickedSaleall4();
	afx_msg void OnLbnSelchangeListinhand();
	afx_msg void OnBnClickedNotsalelst();
	void ReadNotSaleListFromFile();
	void WriteNotSaleListToFile();
	afx_msg void OnLbnSelchangeListNotSale();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnEnChange_Rcvbuycode2();
	afx_msg void OnEnChangeRcv_ZTcode();
	CString m_ZTCode;
	int m_iSaleAbleCount;
	afx_msg void OnBnClickedSaleall3();
	CString m_buyCode3;
	MyHook m_myHook;
	afx_msg void OnEnChange_Rcvbuycode3();
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void AfterTrade();
	afx_msg void OnNMCustomdrawBuyrandom(NMHDR *pNMHDR, LRESULT *pResult);
	void ReadStkETFFromFile();
	CComboBox m_cmbETF;
	afx_msg void OnBnBuyAll();
	afx_msg void OnBnBuy1_2();
	afx_msg void OnBnBuy1_3();
	afx_msg void OnBnBuy1_4();
	void NewBuyRate(int iRate);
	afx_msg void OnBnSellAll();
	afx_msg void OnBnSell1_2();
	afx_msg void OnBnSell1_3();
	afx_msg void OnBnSell1_4();
	void NewSellRate( int iRate );

	afx_msg LRESULT OnTradeMsg(WPARAM wParam, LPARAM lParam);
	CListBox m_lstTradeMSG;
	LPARAM m_iLastAction;
	afx_msg void OnBnClickedRuntdx();
	afx_msg void OnBnClickedKilltdx();
	BOOL m_bAutoSell;
	BOOL m_bAutoBuy;
	afx_msg void OnBnClickedButtonrunhuaan();
	afx_msg void OnBnClickedButtonruntdx1();
	afx_msg void OnBnClickedButtonruntdx();
	int m_iRateS;
	int m_iRateB;
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	CString m_strCopyData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOTRADEDLG_H__E1607545_48DB_4E63_A209_766C9E09A272__INCLUDED_)
