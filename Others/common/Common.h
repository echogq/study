#pragma once
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <sstream>
#include<set>
#include <list>

using std::random_shuffle;
using std::vector;
using namespace std;
const int MAX_BUF_LEN = 4096;

void TraceEx(const wchar_t *strOutputString, ...)
{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;
	wchar_t *strBuffer = new wchar_t[nLen];
	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringW(strBuffer);
	delete[] strBuffer;
}

void TraceEx(const char *strOutputString, ...)
{
	char strBuffer[4096] = { 0 };
	memset(strBuffer, 0, 4096);
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1 + 8;
	strcpy(strBuffer, "\r\n[TDX] ");
	_vsnprintf_s(strBuffer + 8, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
	//delete[] strBuffer;
}

HWND Find_ChildWindowByClassWH(HWND hSubWnd, const char * sClass, int ww, int hh)
{
	HWND child = NULL;
	TCHAR buf[MAX_PATH];
	do {
		child = ::FindWindowEx(hSubWnd, child, NULL, NULL);
		int ret = ::GetClassName(child, buf, MAX_PATH);
		buf[ret] = 0;

		if (strlen(buf) > 0)
		{
			//LogTrace16380("0x%08X -> 0x%08X %s \n", parent, child, buf);
			if (strstr(buf, sClass))
			{
				RECT rect;
				::GetClientRect(child, &rect); //86 * 21
				if ((rect.right == ww) && (rect.bottom == hh))
				{
					if (::IsWindowVisible(child))
					{
						return child;
					}
					//break;
				}
			}
		}
	} while (child);
	return NULL;
}

//递归遍历所有子窗口的子窗口 , 查找
HWND Find_ChildWindow(HWND parent, char* sWnd/*, char* sClass = NULL*/)
{
	HWND child = NULL;
	HWND child000 = NULL;
	TCHAR buf[1024];
	TCHAR bufClass[1024];
	DWORD pid = 0, tid = 0;
	do {
		child = FindWindowEx(parent, child, NULL, NULL);
		int ret = GetWindowTextA(child, buf, 1024);
		buf[ret] = 0;

		if (strlen(buf) == 0)
		{
			::SendMessage(child, WM_GETTEXT, sizeof(buf) / sizeof(char), (LPARAM)buf);//EDIT的句柄，消息，接收缓冲区大小，接收缓冲区指针
		}

		::GetClassName(child, bufClass, 1024);
		//BOOL bClassOK = sClass ? FALSE:TRUE;

		//if (sClass && strstr(bufClass, sClass))
		//{
		//		if (::IsWindowVisible(child))
		//		{
		//			DWORD dwId = 0;
		//			GetWindowThreadProcessId(child, &dwId);

		//			if (hMain_Wnd == ::GetParent(child))
		//			{
		//				bClassOK = TRUE;
		//				hPop_Wnd = child;
		//				LogTrace16380("0x%08X -> 0x%08X p=0x%08X pid=%d %d %s \n", parent, child, ::GetParent(child), dwId, ::IsWindowVisible(child), buf);
		//			}
		//			
		//		}
		//}
		if (strlen(buf) > 0)
		{
			//LogTrace16380("0x%08X -> 0x%08X %d %s \n", parent, child, ::IsWindowVisible(parent), buf);
			if (strstr(buf, sWnd)/* && bClassOK*/)
			{
				return child;
			}
		}
		if (child)
			child000 = Find_ChildWindow(child, sWnd/*, sClass*/);
		if (child000)
		{
			return child000;
		}
	} while (child);

	return NULL;
}

void FindClickRefresh(HWND hSubWnd)
{
	string sVerfy = "";
	string sClass = "";
	HWND hwnd = NULL;

	sClass = "Button";
	hwnd = Find_ChildWindowByClassWH(::GetParent(hSubWnd), sClass.c_str(), 54, 21);
	if (hwnd)
	{
		if (::IsWindowEnabled(hwnd))
		{
			::PostMessage(hwnd, BM_CLICK, 0, 0L);
			do
			{
				Sleep(500);
			} while (!::IsWindowEnabled(hSubWnd));
		}
	}
}


