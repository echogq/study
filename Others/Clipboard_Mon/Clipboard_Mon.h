
// Clipboard_Mon.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CClipboard_MonApp: 
// �йش����ʵ�֣������ Clipboard_Mon.cpp
//

class CClipboard_MonApp : public CWinApp
{
public:
	CClipboard_MonApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CClipboard_MonApp theApp;