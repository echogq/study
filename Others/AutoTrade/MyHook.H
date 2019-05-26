#pragma once
#include <string>
using namespace std;

class MyHook
{
	enum MYHOOK_STATE
	{
		MYHOOK_NULL,
		MYHOOK_READY,
	};
public:
	MyHook(void);
	virtual ~MyHook(void);
public:
	static MyHook& instance();

	BOOL readProcessMemory(LPVOID pBuff, LPVOID pAddr, int nLength = 4);	// 读取内存值

	void init(const HWND& hwnd, const HANDLE& handle, bool bSelf  = FALSE);	// 初始化
	void initByWindowName(const string& strName, bool isReal);
	void initByCreateProcess(const string& strCmd, const string& strParameter = "");							

private:
	static BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam);				// EnumWindows CALLBACK
	static string s_strWindowName;											// 窗体名

	void _initByWindowNameReal(const string& strName);						// 用实名初始化进程
	void _initByWindowNameSimilar(const string& strName);					// 用相似名初始话进程

private:
	HWND _hwnd;																// 窗体句柄
	HANDLE _handle;															// HANDLE
	bool _bSelf;															// 是否为自己
	MYHOOK_STATE _state;													// 状态
	static MyHook *s_instance;
};
