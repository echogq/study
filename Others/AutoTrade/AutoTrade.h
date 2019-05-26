// AutoTrade.h : main header file for the AUTOTRADE application
//

#if !defined(AFX_AUTOTRADE_H__B49AEF80_E2D0_4DB4_A619_70D0BCC43D25__INCLUDED_)
#define AFX_AUTOTRADE_H__B49AEF80_E2D0_4DB4_A619_70D0BCC43D25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAutoTradeApp:
// See AutoTrade.cpp for the implementation of this class
//

class CAutoTradeApp : public CWinApp
{
public:
	CAutoTradeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoTradeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAutoTradeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOTRADE_H__B49AEF80_E2D0_4DB4_A619_70D0BCC43D25__INCLUDED_)
