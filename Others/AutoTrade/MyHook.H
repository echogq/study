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

	BOOL readProcessMemory(LPVOID pBuff, LPVOID pAddr, int nLength = 4);	// ��ȡ�ڴ�ֵ

	void init(const HWND& hwnd, const HANDLE& handle, bool bSelf  = FALSE);	// ��ʼ��
	void initByWindowName(const string& strName, bool isReal);
	void initByCreateProcess(const string& strCmd, const string& strParameter = "");							

private:
	static BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam);				// EnumWindows CALLBACK
	static string s_strWindowName;											// ������

	void _initByWindowNameReal(const string& strName);						// ��ʵ����ʼ������
	void _initByWindowNameSimilar(const string& strName);					// ����������ʼ������

private:
	HWND _hwnd;																// ������
	HANDLE _handle;															// HANDLE
	bool _bSelf;															// �Ƿ�Ϊ�Լ�
	MYHOOK_STATE _state;													// ״̬
	static MyHook *s_instance;
};