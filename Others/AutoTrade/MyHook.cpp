#include "StdAfx.h"
#include "MyHook.h"

MyHook* MyHook::s_instance = NULL;
string MyHook::s_strWindowName = "";

MyHook::MyHook(void):_hwnd(0),_handle(0),_bSelf(FALSE),_state(MYHOOK_NULL)
{
}


MyHook::~MyHook(void)
{
}

MyHook& MyHook::instance()
{
	if (s_instance == NULL)
	{
		static MyHook defaultInstance;
		s_instance = &defaultInstance;
	}
	return *s_instance;
}

void MyHook::init(const HWND& hwnd, const HANDLE& handle, bool bSelf)
{
	_handle = handle;
	_hwnd = hwnd;
	_bSelf = bSelf;
	_state = MYHOOK_READY;
}

void MyHook::initByWindowName(const string& strName, bool isReal)
{
	// 进程初始化
	// strName 窗口名字符串
	if(isReal)
	{
		_initByWindowNameReal(strName);
	}
	else
	{
		_initByWindowNameSimilar(strName);
	}
}

void MyHook::_initByWindowNameReal(const string& strName)
{
	// 用实名初始化进程
	DWORD pid;
	_hwnd = ::FindWindowA(NULL, strName.c_str());
	::GetWindowThreadProcessId(_hwnd, &pid);							// 获得进程ID
	_handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);			// 最高权限打开
	init(_hwnd, _handle);
}

void MyHook::_initByWindowNameSimilar(const string& strName)
{
	// 用相似名初始话进程
	s_strWindowName = strName;
	EnumWindows(lpEnumFunc, (LPARAM)this);
}

BOOL CALLBACK MyHook::lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	// EnumWindows CALLBACK
	// lParam为类指针
	char strBuff[MAX_PATH];
	MyHook* pMyHook = (MyHook*)lParam;

	GetWindowTextA(hwnd, strBuff, MAX_PATH);
	string strName = strBuff;
	if (strName.find(s_strWindowName) != string::npos)
	{
		HWND hwnd = ::FindWindowA(NULL, strName.c_str());
		DWORD pid;
		::GetWindowThreadProcessId(hwnd, &pid);							// 获得进程ID
		HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);	// 最高权限打开
		pMyHook->init(hwnd, handle);
	}
	return TRUE;
}

void MyHook::initByCreateProcess(const string& strCmd, const string& strParameter)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if(CreateProcessA(strCmd.c_str(), const_cast<char*>(strParameter.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{

		HANDLE handle = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_CREATE_THREAD|
			PROCESS_VM_WRITE|PROCESS_QUERY_INFORMATION, FALSE, pi.dwProcessId);		// 打开进程

		init(NULL, handle);
	}
}

BOOL MyHook::readProcessMemory(LPVOID pBuff, LPVOID pAddr, int nLength)
{
	// 读取内存值
	// pBuff 内存位置
	// pAddr 地址
	// nLength 长度
	if(_state == MYHOOK_NULL)
	{	
		return FALSE;
	}
	if(_bSelf)
	{
		memcpy(pBuff, pAddr, nLength);
	}
	else
	{
		::ReadProcessMemory(_handle, pAddr, pBuff, nLength, NULL);
	}
	return TRUE;
}