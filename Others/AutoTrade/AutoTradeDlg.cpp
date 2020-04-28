// AutoTradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoTrade.h"
#include "AutoTradeDlg.h"
#define PORT_UDP 17077
#define PORT_TCP 17078

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include<stdio.h>
#include<string.h>
#include<time.h>
#include <ws2tcpip.h>
#include <..\..\..\..\..\VS2013\VC\include\iostream>
#include "../Common/Common.h"

#define AUTO_OPEN_ALL "..\\Script\\Auto_OpenAll.exe"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

CString g_sCMD = "ping";
using namespace std;
list<HANDLE> g_Event_BS_Action;
int g_iTcpClientsCount = 0;
HBITMAP g_hBitmap20[20];


// ���ݴ��ھ����̨��ͼ���ұȶ���֤��ͼƬ
string GetBmp4OCR(HWND hwnd, int left, int top, int width, int height, CHAR* path = NULL)
{
	string sResult = "";
	HDC pDC;// ԴDC
			//�ж��ǲ��Ǵ��ھ������ǵĻ�����ʹ��GetDC����ȡDC ��Ȼ��ͼ���Ǻ���
	if (hwnd == ::GetDesktopWindow())
	{
		pDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
	}
	else
	{
		pDC = ::GetDC(hwnd);//��ȡ��ĻDC(0Ϊȫ���������Ϊ����)
	}
	int BitPerPixel = ::GetDeviceCaps(pDC, BITSPIXEL);//�����ɫģʽ
	if (width == 0 && height == 0)//Ĭ�Ͽ�Ⱥ͸߶�Ϊȫ��
	{
		width = ::GetDeviceCaps(pDC, HORZRES); //����ͼ����ȫ��
		height = ::GetDeviceCaps(pDC, VERTRES); //����ͼ��߶�ȫ��
	}
	HDC memDC;//�ڴ�DC
	memDC = ::CreateCompatibleDC(pDC);
	HBITMAP memBitmap, oldmemBitmap;//��������Ļ���ݵ�bitmap
	memBitmap = ::CreateCompatibleBitmap(pDC, width, height);
	oldmemBitmap = (HBITMAP)::SelectObject(memDC, memBitmap);//��memBitmapѡ���ڴ�DC
	if (hwnd == ::GetDesktopWindow())
	{
		BitBlt(memDC, 0, 0, width, height, pDC, left, top, SRCCOPY);//ͼ���ȸ߶Ⱥͽ�ȡλ��
	}
	else
	{
		bool bret = ::PrintWindow(hwnd, memDC, PW_CLIENTONLY);
		if (!bret)
		{
			BitBlt(memDC, 0, 0, width, height, pDC, left, top, SRCCOPY);//ͼ���ȸ߶Ⱥͽ�ȡλ��
		}
	}
	//���´��뱣��memDC�е�λͼ���ļ�
	BITMAP bmp;
	::GetObject(memBitmap, sizeof(BITMAP), &bmp);;//���λͼ��Ϣ
	FILE *fp = NULL;
	if (path != NULL)
	{
		fopen_s(&fp, path, "w+b");//ͼƬ����·���ͷ�ʽ
	}

	BITMAPINFOHEADER bih = { 0 };//λͼ��Ϣͷ
	bih.biBitCount = bmp.bmBitsPixel;//ÿ�������ֽڴ�С
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//�߶�
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//ͼ�����ݴ�С
	bih.biWidth = bmp.bmWidth;//���

	BITMAPFILEHEADER bfh = { 0 };//λͼ�ļ�ͷ
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//�ļ��ܵĴ�С
	bfh.bfType = (WORD)0x4d42;

	if (fp != NULL)
	{
		fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//д��λͼ�ļ�ͷ
		fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//д��λͼ��Ϣͷ
	}

	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//�����ڴ汣��λͼ����
	memset(p, 0, bmp.bmWidthBytes * bmp.bmHeight);
	GetDIBits(memDC, (HBITMAP)memBitmap, 0, height, p,
		(LPBITMAPINFO)&bih, DIB_RGB_COLORS);//��ȡλͼ����

	if (fp != NULL)
	{
		fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//д��λͼ����
		fclose(fp);
	}
	for (int i = 0; i < 20; i++)
	{
		byte * pppp = new byte[bmp.bmWidthBytes * bmp.bmHeight];//�����ڴ汣��λͼ����
		memset(pppp, 0, bmp.bmWidthBytes * bmp.bmHeight);
		GetDIBits(memDC, (HBITMAP)g_hBitmap20[i], 0, height, pppp,
			(LPBITMAPINFO)&bih, DIB_RGB_COLORS);//��ȡλͼ����

		if (0 == memcmp(pppp, p, bmp.bmWidthBytes * bmp.bmHeight))
		{
			sResult = std::to_string(i%10);
			break;
		}
	}
	delete[] p;
#if 0	
	HWND sBitHwnd = GetDlgItem(g_Hwnd, IDC_STATIC_IMG);
	/*�����ڴ��е�λͼ��� ��ԭԭ�����ڴ�DCλͼ��� ����ֱ���� memBitmap�Ҳ��Ժ����ǲ��в�֪��Ϊʲô*/
	HBITMAP oleImage = (HBITMAP)::SelectObject(memDC, oldmemBitmap);
	oleImage = (HBITMAP)SendMessage(sBitHwnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)oleImage);

	/*���ַ���Ҳ�ܰ�λͼ��ʾ��picture �ؼ���*/
	HDC bitDc = NULL;
	bitDc = ::GetDC(sBitHwnd);
	BitBlt(bitDc, 0, 0, bmp.bmWidth, bmp.bmHeight, memDC, 0, 0, SRCCOPY); //�ڴ�DCӳ�䵽��ĻDC
	ReleaseDC(sBitHwnd, bitDc);
	/*�����Ҫ��λͼת��*/
	/*
	CImage image;
	image.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(image.GetDC(), 0, 0, nWidth, nHeight, hdcSrc, 0, 0, SRCCOPY);
	::ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(path, Gdiplus::ImageFormatPNG);//ImageFormatJPEG
	*/
	DeleteObject(oleImage);
#endif
	DeleteObject(memBitmap);
	DeleteDC(memDC);
	ReleaseDC(hwnd, pDC);
	return sResult;
}

string GetNetCardIP()
{
	string sIP = "";
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen;
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);



	// ��һ�ε���GetAdapterInfo��ȡulOutBufLen��С 
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			PIP_ADDR_STRING pIPAddr;
			pIPAddr = &pAdapter->IpAddressList;
			while (pIPAddr)
			{
				if ((0 != strcmp(pIPAddr->IpAddress.String, "0.0.0.0")) &&
					(0 != strcmp(pAdapter->GatewayList.IpAddress.String, "0.0.0.0")))
				{
					cout << "IP:		" << pIPAddr->IpAddress.String << endl;
					cout << "Mask:		" << pIPAddr->IpMask.String << endl;
					TraceEx(pIPAddr->IpAddress.String);
					TraceEx(pAdapter->GatewayList.IpAddress.String);

					return pIPAddr->IpAddress.String;
					/*
					�㷨��
					1. ����������IP��ַ����λ�����㣬�ô������ַ
					2. �����ַ | (~��������)���ó��㲥��ַ
					*/
					//in_addr broadcast;
					//broadcast.S_un.S_addr = (
					//	inet_addr(pIPAddr->IpAddress.String)
					//	& inet_addr(pIPAddr->IpMask.String)
					//	)
					//	| (~inet_addr(pIPAddr->IpMask.String));
					////pAI->strBroadcastIp = inet_ntoa(broadcast);

					//cout << "BroadcastIp:	" << inet_ntoa(broadcast) << endl;
					//TraceEx("BroadcastPing:	%s	", inet_ntoa(broadcast));

					//sIP = BroadcastPing(inet_ntoa(broadcast), uPort);

					if (sIP.length() > 0)
					{
						{
							return sIP;
						}
					}
					cout << endl;
				}
				pIPAddr = pIPAddr->Next;
			}
			pAdapter = pAdapter->Next;
		}
	}
	return sIP;
}


set<SOCKET> sock_TCP_Connections;
//set<SOCKET> sock_TCP_Send;
//SOCKET* sock_TCP_Rcv = NULL;
//SOCKET* sock_TCP_Send = NULL;

void ThreadProcess_TCP_Rcv(LPVOID pSock)
{
	SOCKET sock_TCP_Rcv = *(SOCKET*)pSock;
	while (true)
	{
		//char recvBuf[100];
		char recvBuf[4096] = { 0 };
		int xxx = recv(sock_TCP_Rcv, recvBuf, 4096, 0);

		if (xxx == SOCKET_ERROR)
		{
			closesocket(sock_TCP_Rcv);
			return;
		}
		if (strlen(recvBuf)>0)
		{
			//��ӡ���յ�������
			TraceEx("���յ�������: [%s]\n", recvBuf);
		}

	}
}

void ThreadProcess_TCP_Send(LPVOID pSock)
{
	SOCKET sock_TCP_Send = *(SOCKET*)pSock;
	int tid = ::GetCurrentThreadId();
	HANDLE hhh = CreateEvent(NULL, FALSE, FALSE, NULL);//�Զ������ź�
	g_Event_BS_Action.push_back(hhh);

	g_iTcpClientsCount++;
	CString str; 
	str.Format(_T("%d"), g_iTcpClientsCount);
	AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_CLIENTS)->SetWindowText(str);
	
	while (true)
	{
		//extern HANDLE g_Event_BS_Action;

		DWORD res = WaitForSingleObject(hhh, 300);
		string msg = g_sCMD;
		char sendBuf[100] = { 0 };
		int xxx = 0;
		switch (res)
		{
		case WAIT_OBJECT_0:
			printf("\n��++++++++++++++++++++BS_Event����Event����------��������\n");
			//��������

			xxx = send(sock_TCP_Send, msg.c_str(), msg.length(), 0);
			TraceEx("\nTid=%d [%s],-------����BS %s, res = %d\n", tid, msg.c_str(), (xxx>0)?"�ɹ�":"ʧ��!!!", xxx);

			break;
		case WAIT_TIMEOUT:

			sprintf_s(sendBuf, 100, "%s", "AutoTrade...TCPPing");
			//����Ping
			xxx = send(sock_TCP_Send, sendBuf, strlen(sendBuf), 0);
			TraceEx("\nTid=%d [WS_WAIT_TIMEOUT],-------�������� %s, res = %d\n", tid, (xxx>0)?"�ɹ�":"ʧ��!!!", xxx);

			break;
		case WAIT_ABANDONED:
			printf("\n[WS_WAIT_ABANDONED]��hHandleΪmutexʱ�����ӵ��mutex���߳��ڽ���ʱû���ͷź��Ķ���������˷���ֵ\n");
			break;
		case WAIT_FAILED:
			printf("\n[WS_WAIT_FAILED]ʧ����, callGetLastError.\n");
			LPVOID lpMsgBuf;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
			);
			printf("\n[WS_WAIT_FAILED]ʧ��GetLastError is: %s\n", lpMsgBuf);
			// Process any inserts in lpMsgBuf.
			// ...
			// Display the string.
			//MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
			// Free the buffer.
			LocalFree(lpMsgBuf);
			break;
		default:
			break;

		}

		if (xxx == SOCKET_ERROR)//���ӳ������⣬�˳��߳�
		{
			//���ɾ��ǰ�潨����event���������Ҵ�list��ȥ��
			closesocket(sock_TCP_Send);

			g_iTcpClientsCount--;
			CString str;
			str.Format(_T("%d"), g_iTcpClientsCount);
			AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_CLIENTS)->SetWindowText(str);

			return;
		}
	}

}

void ThreadProcess_TCP_Listen(LPVOID Port)
{
	//��������WinSock��İ汾��
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	printf("This is a Server side application!\n");
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup() called failed!\n");
		return ;
	}
	else {
		printf("WSAStartup() called successful!\n");
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		//������������İ汾��2.2,����ֹWinSock���ʹ��
		WSACleanup();
		return ;
	}
	//�������ڼ������׽���
	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if (sockServer == INVALID_SOCKET) {
		printf("socket() called failed!, error code is: %d", WSAGetLastError());
		return ;
	}
	else {
		printf("socket() called successful!\n");
	}
	//�����������׽��ֽṹ
	SOCKADDR_IN addrServer;
	//�������ֽ�˳��ת��ΪTCP/IP�����ֽ�˳��
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons((int)Port);
	//���׽��ְ󶨵�һ�����ص�ַ�Ͷ˿���
	err = ::bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (err == SOCKET_ERROR) {
		printf("bind() called failed! The error code is: %d\n", WSAGetLastError());
		return ;
	}
	else {
		printf("bind() called successful\n");
	}
	//���׽�������Ϊ����ģʽ,׼�����տͻ�������
	err = listen(sockServer, 5);
	if (err == SOCKET_ERROR) {
		printf("listen() called failed! The error code is: %d\n", WSAGetLastError());
		return ;
	}
	else {
		printf("listen() called successful!\n");
	}
	//���淢���������ӵĿͻ��˵��׽�����Ϣ
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	bool isStart = true;
	SOCKET sockConn;

	while (isStart)
	{
		//�ȴ��ͻ���������
		TraceEx("==> ����˴�һֱ�ȴ����ӣ����鱾������ǽ�� ");
		sockConn = accept(sockServer, (SOCKADDR*)&addrClient, &len);
		if (sockConn == INVALID_SOCKET) {
			printf("accept() called failed! The error code is: %d\n", WSAGetLastError());
		}
		else {
			printf("The server receive a new client connection!\n");
		}

		TraceEx("socket=%d ���ԣ�%s ������" , sockConn, inet_ntoa(addrClient.sin_addr));

		int timeout = 200; //ms
		//setsockopt(sockConn, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		//if (sock_TCP_Connections.size() == 0)
		//{
			_beginthread(ThreadProcess_TCP_Rcv, 0, &sockConn);
			_beginthread(ThreadProcess_TCP_Send, 0, &sockConn);
		//}
		//else
		//	sock_TCP_Connections.insert(sockConn);


		////char sendBuf[100];
		////sprintf_s(sendBuf, 100, "Welcome %s", inet_ntoa(addrClient.sin_addr));
		////��������
		////send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		////char recvBuf[100];
		//char recvBuf[4096] = { 0 };
		//recv(sockConn, recvBuf, 100, 0);
		////��ӡ���յ�������
		//TraceEx("received data from client side [%s, %d] is: %s\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port, recvBuf);

		//int RecvSize = 4096;
		//int iTotalSize = 0;

		//while (fd && (RecvSize > 0))
		//{
		//	//��������
		//	RecvSize = recv(sockConn, recvBuf, RecvSize, 0);
		//	if (SOCKET_ERROR == RecvSize)
		//		break;;
		//	iTotalSize += RecvSize;

		//	if (fd) {
		//		fwrite(recvBuf, 1, RecvSize, fd);
		//	}
		//}
	}
		//�ر������׽���
		closesocket(sockConn);

	WSACleanup();
	return ;
}


void ThreadProcess_UDP_Listen(LPVOID Port)
{
	WSADATA wsd;
	//SOCKET  s;
	int     nRet;
	//WinExec("taskkill /F /IM QMDnc.exe", SW_HIDE);

	// ��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("WSAStartup failed !/n");
		return;
	}

	SOCKADDR_IN addrSrv;
	SOCKADDR_IN addrClient;
	char        buf[MAX_BUF_LEN];
	int         len = sizeof(SOCKADDR);

	// ���÷�������ַ  
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_port = htons((int)Port);

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	int iLastMin = 0;
	SOCKET      sockLocal;

		//GetLocalTime(&st);
		//if ((iLastMin == 0) || (((st.wMinute + 60 - iLastMin) % 60) >= 30)) //ÿ��30���Ӵ�Ngx����һ��zip
		//{
		//	iLastMin = st.wMinute;
		//	//Todo�� ׷�Ӱ汾�ж�
		//	//������
		//	//���ʱ��ȥ��ȡ���£�������Ҫ��������ʱ�䣬���Ӧ��ȷ�ϴ˴��Ƿ���յ���Щ�Ĺ�ʱ���ݣ���������
		//	if (PORT_TERMINAL_ZIP == (int)Port)
		//	{
		//		if (!UpdateZipFromNgx("Terminal.zip"))
		//		{
		//			break;;
		//		}
		//	}
		//}

		while (1)
		{
			// �����׽���  
			sockLocal = socket(AF_INET, SOCK_DGRAM, 0);
			if (sockLocal == INVALID_SOCKET)
			{
				printf("socket() failed, Error Code:%d/n", WSAGetLastError());
				Sleep(100);
				continue;
			}
			// ���׽���  
			nRet = ::bind(sockLocal, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
			if (SOCKET_ERROR == nRet)
			{
				printf("bind failed !/n");
				closesocket(sockLocal);
				Sleep(100);
				continue;
			}
			ZeroMemory(buf, MAX_BUF_LEN);

			// �ӿͻ��˽�������  
			int timeout = 0; //3s
			setsockopt(sockLocal, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

			while (1)
			{
				nRet = recvfrom(sockLocal, buf, MAX_BUF_LEN, 0, (SOCKADDR*)&addrClient, &len);
				if (SOCKET_ERROR == nRet)
				{
					printf("recvfrom failed !\n");
					closesocket(sockLocal);
					Sleep(100);
					break;
				}

				TraceEx(buf);

				// ��ӡ���Կͻ��˷�����������  
				//printf("Recv From Client:%s\n", buf);


				// ��ͻ��˷�������  
				//strcpy(buf, "UDP Hello World ! [Server]\r\n");
				//sendto(sockLocal, buf, strlen(buf), 0, (SOCKADDR*)&addrClient, len);
				{
				}
				//WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
				static char str[128];
				struct sockaddr_in *sin = (struct sockaddr_in *)((SOCKADDR*)&addrClient);
				if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) != NULL)
				{
					string sZipName = "";
					string sLocalIP = GetNetCardIP();

					if (sLocalIP.length() > 6)//1.1.1.1
					{
						::sendto(sockLocal, sLocalIP.c_str(), sLocalIP.length(), 0, (SOCKADDR*)&addrClient, len);
					}
					//if (PORT_TERMINAL_ZIP == (int)Port)
					//{
					//	sZipName = "Terminal.zip";
					//}
					//else if (PORT_COMMON_ZIP == (int)Port)
					//{
					//	sZipName = "common.zip";
					//}

					//if (Common::IsFileExist(".\\" + sZipName))
					//{
					//	::sendto(sockLocal, "QMDnc.exe", 9, 0, (SOCKADDR*)&addrClient, len);
					//	Sleep(100);
					//	TCP_ConnectAndSendFile(str, (int)Port, sZipName);
					//}
				}
			}
		}

		closesocket(sockLocal);

	WSACleanup();
	return;
}

std::vector<float>
Ema(std::vector<float> &X, int N)
{
	std::vector<float> vec;
	int nLen = X.size();
	if (nLen >= 1)
	{
		if (N > nLen) N = nLen;

		vec.resize(nLen);
		//vec.reserve(nLen);
		vec[0] = X[0];
		for (int i = 1; i < nLen; i++)
		{
			vec[i] = (2 * X[i] + (N - 1) * vec[i - 1]) / (N + 1);
		}
	}
	return vec;
}

char tmpbuf[128];
char* PrefixTimeStr( char* p )
{
	struct tm *newtime;
	time_t lt1;

	time( &lt1 );
	newtime=localtime(&lt1);

	//strftime( tmpbuf, 128, "Today is %A, the %d day of %B in the year %Y.\n", newtime);
	sprintf(tmpbuf, "��%d %2d:%2d:%2d %s", newtime->tm_wday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec, p);

	return tmpbuf;
}

void birthdaysong ()
{

	unsigned int frequency [] =
	{ 
		392,392,440,392,523,494, 
		392,392,440,392,587,523, 
		392,392,784,659,523,494,440, 
		698,698,659,523,587,523
	};

	unsigned int delay[] = 
	{
		375,125,500,500,500,1000, 
		375,125,500,500,500,1000, 
		375,125,500,500,500,500,1000, 
		375,125,500,500,500,1000,
	}; 

	int   i; 

	for ( i=0; i < 25; i++ ) 
	{ 
		Beep ( frequency[i] * 5, delay[i] );

		if ( i == 24 )
		{
			Sleep ( 500 );
			i = 0;
		}
	} 
}

#define ONE_BEEP 600
#define HALF_BEEP 300

#define NOTE_1 440
#define NOTE_2 495
#define NOTE_3 550
#define NOTE_4 587
#define NOTE_5 660
#define NOTE_6 733
#define NOTE_7 825
#define NOTE_NONE Sleep(ONE_BEEP);
void testsong ()
{
	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_1*2, ONE_BEEP*2);

	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP*2);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_1, HALF_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_3, ONE_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_1, HALF_BEEP);
	Beep(NOTE_2, ONE_BEEP*4);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_1*2, HALF_BEEP*3);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP*2);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_4, HALF_BEEP*3);
	Beep(NOTE_7/2, HALF_BEEP);
	Beep(NOTE_1, ONE_BEEP*4);

	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP*2);

	Beep(NOTE_7, ONE_BEEP);
	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_1*2, ONE_BEEP*2);

	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_1*2, HALF_BEEP);
	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_6, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_1, HALF_BEEP);
	Beep(NOTE_2, ONE_BEEP*4);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_5, HALF_BEEP);
	Beep(NOTE_1*2, HALF_BEEP*3);
	Beep(NOTE_7, HALF_BEEP);
	Beep(NOTE_6, ONE_BEEP);
	Beep(NOTE_1*2, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP*2);

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_2, HALF_BEEP);
	Beep(NOTE_3, HALF_BEEP);
	Beep(NOTE_4, HALF_BEEP*3);
	Beep(NOTE_7/2, HALF_BEEP);
	Beep(NOTE_1, ONE_BEEP*3);
}
void OnScreenfont(CString szBuffer)
{
	HFONT		font;
	HDC			hdc;
	HWND		hwnd;
	COLORREF	clr;
	//TCHAR		szBuffer[] = TEXT("CPU�¶ȳ���...>89��C");

	//��ȡ���洰�ھ��
	hwnd = GetDesktopWindow();
	//��ȡ���洰��DC
	hdc = GetWindowDC(hwnd);
	//����ɫ͸��
	SetBkMode(hdc, TRANSPARENT);

	font=CreateFont(
		120,48,0,0, FW_BOLD,0,0,0, ANSI_CHARSET ,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,NULL,(LPCTSTR)"΢���ź�");
	SelectObject(hdc,font);

	//������ɫ���������
	clr = SetTextColor(hdc, RGB(255, 0, 0));
	TextOut(hdc, 800,400, szBuffer.GetBuffer(), szBuffer.GetLength());

	//��ԭ��ɫ
	SetTextColor(hdc, clr);

	//�ͷž��DC
	DeleteObject(font); 
	ReleaseDC(hwnd, hdc);
}

//bug�����߳�����ס
CCriticalSection csInHand; 
CCriticalSection csBuy;
CCriticalSection csSale;
HANDLE hProcmonitorTDXpool = NULL;
HANDLE hProcAccountInfo = NULL;
HANDLE hProcGetMyStkList = NULL;
BOOL bSaling = FALSE;
int iLimitBuyOneMoney = 18000;

CFont fontTitle;

#define STRICT 

/***********************************************************************
*
*  Global variables
*
***********************************************************************/ 

HWND g_hwndTimedOwner; 
BOOL g_bTimedOut; 

/***********************************************************************
*
*  MessageBoxTimer
*
*      The timer callback function that posts the fake quit message.
*      This function causes the message box to exit because the message box
*      has determined that the application is exiting.
*
***********************************************************************/ 
void CALLBACK MessageBoxTimer(HWND hwnd,  
							  UINT uiMsg,  
							  UINT idEvent,  
							  DWORD dwTime) 
{ 
	g_bTimedOut = TRUE; 
// 	if (g_hwndTimedOwner) 
// 		EnableWindow(g_hwndTimedOwner, TRUE); 
// 	PostQuitMessage(0); 
	HWND hwnd2 = FindWindow(NULL, MSGTITLE);
	if (hwnd2)
	{
		//PostMessage(hwnd2, WM_CLOSE, 0, 0);
		::EndDialog((HWND)hwnd2, IDCLOSE);
	}
} 

/***********************************************************************
*
*  TimedMessageBox
*
*      The same as the standard MessageBox, except that TimedMessageBox
*      also accepts a timeout. If the user does not respond within the
*      specified timeout, the value 0 is returned instead of one of the
*      ID* values.
*
***********************************************************************/ 
int TimedMessageBox(HWND hwndOwner, 
                    LPCTSTR pszMessage, 
                    LPCTSTR pszTitle, 
                    UINT flags, 
                    DWORD dwTimeout) 
{ 
   UINT idTimer; 
   int iResult; 
 
   g_hwndTimedOwner = NULL; 
   g_bTimedOut = FALSE; 
 
   if (hwndOwner && IsWindowEnabled(hwndOwner)) 
      g_hwndTimedOwner = hwndOwner; 
 
   // Set a timer to dismiss the message box. 
   idTimer = SetTimer(NULL, 0, dwTimeout, (TIMERPROC)MessageBoxTimer); 
 
   iResult = MessageBox(hwndOwner, pszMessage, pszTitle, flags); 
 
   // Finished with the timer. 
   KillTimer(NULL, idTimer); 
 
   // See if there is a WM_QUIT message in the queue if we timed out. 
   // Eat the message so we do not quit the whole application. 
   if (g_bTimedOut) 
   { 
//       MSG msg; 
//       PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE); 
      iResult = -1; 
   } 
 
   return iResult; 
} 

void int2str(const int &int_temp,string &string_temp)  
{  
	stringstream stream;  
	stream<<int_temp;  
	string_temp=stream.str();   //�˴�Ҳ������ stream>>string_temp  
}  

int popTimedMessageBox(LPSTR pszMsgLine, int iSeconds) 
{ 

	UINT uiResult; 

	// Ask the user a question. Give the user five seconds to 
	// answer the question. 
	string scd="";
	int2str(iSeconds, scd);

	string szMsgLine=pszMsgLine;
	szMsgLine += "\r\n��ʱ������";
	szMsgLine += scd;

	scd = MSGTITLE;

	uiResult = TimedMessageBox(NULL,  
		szMsgLine.c_str(), 
		scd.c_str(),  
		MB_YESNO, 
		// NULL first parameter is important. 
		iSeconds*1000);  

// 	switch (uiResult) { 
// 	  case IDYES: 
// 		  MessageBox(NULL,  
// 			  "That's right!",  
// 			  "Result",  
// 			  MB_OK); 
// 		  break; 
// 
// 	  case IDNO: 
// 		  MessageBox(NULL,  
// 			  "Believe it or not, triangles " 
// 			  "really do have three sides.",  
// 			  "Result", 
// 			  MB_OK); 
// 		  break; 
// 
// 	  case -1: 
// 		  MessageBox(NULL,  
// 			  "I sensed some hesitation there.  " 
// 			  "The correct answer is Yes.",  
// 			  "Result",  
// 			  MB_OK); 
// 		  break; 
// 	} 

	return uiResult; 
} 

HMODULE fnGetProcessBase(DWORD PID);
DWORD GetLastErrorBox(HWND hWnd, LPSTR lpTitle) ;


HMODULE fnGetProcessBase(DWORD PID)
{
	//��ȡ���̻�ַ
	HANDLE hSnapShot;
	//ͨ��CreateToolhelp32Snapshot���߳�ID����ȡ���̿���
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		GetLastErrorBox(NULL,"�޷���������");
		return NULL;
	}
	MODULEENTRY32 ModuleEntry32;
	ModuleEntry32.dwSize = sizeof(ModuleEntry32);
	if (Module32First(hSnapShot, &ModuleEntry32))
	{
		do 
		{
			TCHAR szExt[5];
			strcpy(szExt, ModuleEntry32.szExePath + strlen(ModuleEntry32.szExePath) - 4);
			for (int i = 0;i < 4;i++)
			{
				if ((szExt[i] >= 'a')&&(szExt[i] <= 'z'))
				{
					szExt[i] = szExt[i] - 0x20;
				}
			}
			if (!strcmp(szExt, ".EXE"))
			{
				CloseHandle(hSnapShot);
				return ModuleEntry32.hModule;
			}
		} while (Module32Next(hSnapShot, &ModuleEntry32));
	}
	CloseHandle(hSnapShot);
	return NULL;

}


// ��ʾ������Ϣ  
DWORD GetLastErrorBox(HWND hWnd, LPSTR lpTitle)  
{  
	LPVOID lpv;  
	DWORD dwRv;  

	if (GetLastError() == 0) return 0;  

	dwRv = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |  
		FORMAT_MESSAGE_FROM_SYSTEM,  
		NULL,  
		GetLastError(),  
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),  
		(LPSTR)&lpv,  
		0,  
		NULL);  

	MessageBox(hWnd, (LPCSTR)lpv, lpTitle, MB_OK);  

	if(dwRv)  
		LocalFree(lpv);  

	SetLastError(0);  
	return dwRv;  
}
//
// FindProcess
// �������Ψһ�Ĳ�������ָ���Ľ���������:���Ŀ�����
// �� "Notepad.exe",����ֵ�Ǹý��̵�ID��ʧ�ܷ���0
//

DWORD FindProcess(CString strProcessName)
{
    DWORD aProcesses[1024], cbNeeded, cbMNeeded;
    HMODULE hMods[1024];
    HANDLE hProcess;
    char szProcessName[MAX_PATH];
	
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) ) return 0;
    for(int i=0; i< (int) (cbNeeded / sizeof(DWORD)); i++)
    {
        //_tprintf(_T("%d "), aProcesses[i]);
        hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, aProcesses[i]);
        EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
        GetModuleFileNameEx( hProcess, hMods[0], szProcessName,sizeof(szProcessName));
		
        if(strstr(szProcessName, strProcessName))
        {
            //_tprintf(_T("%s;"), szProcessName);
            return(aProcesses[i]);
        }
        //_tprintf(_T(" "));
    }
    return 0;
}

//
// KillProcess
// �˺������������ FindProcess ����������Ŀ����̵�ID
// ��WIN API OpenPorcess ��ô˽��̵ľ��������TerminateProcess
// ǿ�ƽ����������
//

VOID KillProcess(CString exeName)
{
    // When the all operation fail this function terminate the "winlogon" Process for force exit the system.
	while(TRUE)
	{
		HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE,
			FindProcess(exeName));
		
		if(hProcess == NULL)
		{
			return;
		}
		
		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
	}
    return;
}
HWND FindWindowX( LPCSTR lpClassName , LPCSTR sTitle)
{
	char strTitle[128];
	HWND hwnd=NULL;
	HWND AfterHwnd = NULL;
	while(true)
	{	
		hwnd=::FindWindowEx(NULL,AfterHwnd,lpClassName,NULL);
		if(!hwnd)
			break;
		else
		{
			if(::GetWindowText(hwnd,strTitle,80))
				if(strstr(strTitle, sTitle)!= NULL)
				{
					//�ҵ����ں�Ĳ���
					return hwnd;
				}
		}
		AfterHwnd = hwnd;
	}
	return NULL;
}

void OutDbgStr( CString m_outString)
{
	m_outString.Format("[AutoIT]Tool %s", m_outString);
	OutputDebugString(m_outString);
}

DWORD RunApp2End(CString sPath, CString sPara="", BOOL bWaitExit=TRUE)
{
	TRACE("***********************RunApp2End: %s\r\n", sPath);
	//����һ������ֱ�������н���
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = sPath;		
	ShExecInfo.lpParameters = sPara;	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;	
	ShellExecuteEx(&ShExecInfo);

	DWORD dwExitCode = 0;
	if (bWaitExit)
	{
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

		BOOL bOK = GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
		ASSERT(bOK);
		return dwExitCode;
	}
	else
		return (DWORD)ShExecInfo.hProcess;

}
void GetInfo()
{
	if(hProcAccountInfo)
	{
		TerminateProcess( hProcAccountInfo, 0);
	}
	hProcAccountInfo = (HANDLE)RunApp2End("Auto_AccountInfo.exe", "", FALSE); //�õ����ַ�����au3ֱ��set��staticText
}
void CAutoTradeDlg::GetInHandList()
{
	csInHand.Lock(); TRACE("csInHand.Lock()\r\n");
	if(hProcGetMyStkList)
	{
		TerminateProcess( hProcGetMyStkList, 0);
	}
	//m_lstInHand.ResetContent();
	hProcGetMyStkList = (HANDLE)RunApp2End("Auto_GetMyStkList.exe", "", FALSE); //�õ����б���au3ֱ��set��staticText���ٷָ��
	TRACE("csInHand.Unlock()\r\n");	csInHand.Unlock();
}
BOOL BuyIt(CString stkCode)
{
	return RunApp2End("Auto_TDXBuy.exe", stkCode, FALSE); //ֱ�ӷ���1��0����ʾ����ɹ���ʧ��
}
BOOL SellIt(CString stkCode)
{
	return RunApp2End("Auto_TDXSell.exe", stkCode, FALSE); //ֱ�ӷ���1��0����ʾ����ɹ���ʧ��
}

void SaleStk(CAutoTradeDlg * p)
{
	csSale.Lock(); 
	for (int i = 1; i <= p->m_lstSale.GetCount(); ++i )
	{
		CString text; 
		
		p->m_lstSale.GetText(i - 1, text);

		if(p->m_lstInHand.FindStringExact (0,text) >=0)
		{
			if(SellIt(text))
			{
				p->m_lstSale.DeleteString(i-1);
			}
		}
		else
		{
				p->m_lstSale.DeleteString(i-1);
		}
	}  	
	csSale.Unlock();
}
void BuyStk(CAutoTradeDlg * p)
{
	//��Ʊ����
	
	p->m_lstBuy;
	
	csBuy.Lock(); 
	csBuy.Unlock();
	
	for (int i = 1; i <= p->m_lstBuy.GetCount(); ++i )
	{
		CString text; 
		
		p->m_lstBuy.GetText(i - 1, text);
		if(BuyIt(text))
		{
			p->m_lstBuy.DeleteString(i-1);
		}
	}  	
}

//�߳���:
static DWORD WINAPI ThreadProc(LPVOID pParam)
{
	while(TRUE)
	{
		//Get:  ���:727040.50  ����:727040.50  ��ȡ:727040.50  �ο���ֵ:0.00  �ʲ�:727040.50  ӯ��:0.00  

		//GetInfo();
		//Get: ���й�Ʊ�б�
		//CAutoTradeDlg * p = pParam;
		//p->GetInHandList();
		
		//��Ʊ����
		//SaleStk((CAutoTradeDlg *)pParam);
				
		//��Ʊ����
		//BuyStk((CAutoTradeDlg *)pParam);
		Sleep(5000);
	}
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CAutoTradeDlg dialog

CAutoTradeDlg::CAutoTradeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoTradeDlg::IDD, pParent)
, m_stkCode(_T(""))
, m_strRate(_T(""))
, m_iRadio01(0)
, m_ZTCode(_T(""))
, m_iSaleAbleCount(0)
, m_buyCode3(_T(""))
, m_bAutoSell(TRUE)
, m_bAutoBuy(TRUE)
, m_bUDP(FALSE)
, m_iRateS(1)
, m_iRateB(2)
, m_strCopyData(_T(""))

{
	//{{AFX_DATA_INIT(CAutoTradeDlg)
	m_buyCode = _T("");
	m_buyCode2 = _T("");
	m_saleCode = _T("");
	m_bAutoUpdate = FALSE;
	m_bDrawRedTxt = FALSE;
	m_bLimitPercent = TRUE;
	m_iLimitPercent = 10;
	m_fCurPercent = 0.0f;
	m_sInfo = _T("");
	hReceiveThread = NULL;
	ThreadID = NULL;
	m_fRemainMoney = 0;
	m_fUsableMoney = 0;
	m_fOutableMoney = 0;
	m_fValuableMoney = 0;
	m_fTotalMoney = 0;
	m_fWinMoney = 0;
	m_iStocksInHand = 0;
	m_iInBuyPoolCount = 0;
	m_iLastAction = -1;
	g_iTcpClientsCount = 0;

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MF2);
}

void CAutoTradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoTradeDlg)
	DDX_Control(pDX, IDC_LISTINHAND, m_lstInHand);
	DDX_Control(pDX, IDC_LIST2, m_lstSale);
	DDX_Control(pDX, IDC_LIST1, m_lstBuy);
	DDX_Text(pDX, IDC_RCVBUYCODE, m_buyCode);
	DDX_Text(pDX, IDC_RCVBUYCODE2, m_buyCode2);
	DDX_Text(pDX, IDC_RCVSALECODE, m_saleCode);
	DDX_Check(pDX, IDC_AUTOUPDATE, m_bAutoUpdate);
	DDX_Check(pDX, IDC_REDTEXT_ONSCREEN, m_bDrawRedTxt);
	DDX_Check(pDX, IDC_CB_PERCENT, m_bLimitPercent);

	DDX_Check(pDX, IDC_CB_AUTOSELL, m_bAutoSell);
	DDX_Check(pDX, IDC_CB_AUTOBUY, m_bAutoBuy);
	DDX_Check(pDX, IDC_CB_UDP, m_bUDP);

	DDX_Text(pDX, IDC_EDIT_PERCENT, m_iLimitPercent);
	DDX_Text(pDX, IDC_CUR_PERCENT, m_fCurPercent);
	DDX_Text(pDX, IDC_INFO, m_sInfo);
	DDX_Text(pDX, IDC_STOCKCOUNT, m_iStocksInHand);
	DDX_Text(pDX, IDC_GETONESTOCK, m_stkCode);
	DDX_Text(pDX, IDC_BUYPOOLCOUNT, m_iInBuyPoolCount);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LISTRATE, m_lstRate);
	DDX_Text(pDX, IDC_WIN_PERCENT, m_strRate);
	DDX_Control(pDX, IDC_LISTNOTSALE, m_lstNotSale);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadio01);
	DDX_Text(pDX, IDC_RCVZTCODE, m_ZTCode);
	DDX_Text(pDX, IDC_SALEABLECOUNT, m_iSaleAbleCount);
	DDX_Text(pDX, IDC_RCVBUYCODE3, m_buyCode3);
	DDX_Control(pDX, IDC_COMBO1, m_cmbETF);
	DDX_Control(pDX, IDC_LIST3, m_lstTradeMSG);
	DDX_Text(pDX, IDC_RATES, m_iRateS);
	DDX_Text(pDX, IDC_RATEB, m_iRateB);
	DDX_Text(pDX, IDC_COPYDATA, m_strCopyData);
	//DDX_Text(pDX, IDC_CLIENTS, g_iTcpClientsCount);
}

BEGIN_MESSAGE_MAP(CAutoTradeDlg, CDialog)
//{{AFX_MSG_MAP(CAutoTradeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_RCVBUYCODE, OnEnChange_Rcvbuycode)
	ON_EN_CHANGE(IDC_RCVSALECODE, OnChangeRcvsalecode)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_AUTOUPDATE, OnAutoupdate)
	ON_EN_CHANGE(IDC_GETONESTOCK, OnEnChange_RcvInHandStock)
	ON_BN_CLICKED(IDSALEALL, OnBnClickedSaleall)
	ON_BN_CLICKED(IDBUYRANDOM, OnBnClickedBuyrandom)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_CHANGE(IDC_INFO, OnChangeInfo)
//}}AFX_MSG_MAP
ON_EN_CHANGE(IDC_WIN_PERCENT, &CAutoTradeDlg::OnEnChangeWinPercent)
ON_BN_CLICKED(IDSALEALL2, &CAutoTradeDlg::OnBnClickedSaleall2)
ON_BN_CLICKED(IDSALEALL3, &CAutoTradeDlg::OnBnClickedSaleall4)
ON_LBN_SELCHANGE(IDC_LISTINHAND, &CAutoTradeDlg::OnLbnSelchangeListinhand)
ON_BN_CLICKED(IDSALEALL4, &CAutoTradeDlg::OnBnClickedNotsalelst)
ON_LBN_SELCHANGE(IDC_LISTNOTSALE, &CAutoTradeDlg::OnLbnSelchangeListNotSale)
ON_BN_CLICKED(IDC_RADIO1, &CAutoTradeDlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, &CAutoTradeDlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO3, &CAutoTradeDlg::OnBnClickedRadio1)
ON_EN_CHANGE(IDC_RCVBUYCODE2, &CAutoTradeDlg::OnEnChange_Rcvbuycode2)
ON_EN_CHANGE(IDC_RCVZTCODE, &CAutoTradeDlg::OnEnChangeRcv_ZTcode)
ON_BN_CLICKED(IDSALEALL5, &CAutoTradeDlg::OnBnClickedSaleall3)
ON_BN_CLICKED(IDNOTSALELST, &CAutoTradeDlg::OnBnClickedNotsalelst)
ON_EN_CHANGE(IDC_RCVBUYCODE3, &CAutoTradeDlg::OnEnChange_Rcvbuycode3)
ON_BN_CLICKED(IDOK, &CAutoTradeDlg::OnBnClickedOk)
ON_WM_CTLCOLOR()
ON_NOTIFY(NM_CUSTOMDRAW, IDBUYRANDOM, &CAutoTradeDlg::OnNMCustomdrawBuyrandom)
ON_BN_CLICKED(IDC_BUTTON1, &CAutoTradeDlg::OnBnBuyAll)
ON_BN_CLICKED(IDC_BUTTON2, &CAutoTradeDlg::OnBnBuy1_2)
ON_BN_CLICKED(IDC_BUTTON3, &CAutoTradeDlg::OnBnBuy1_3)
ON_BN_CLICKED(IDC_BUTTON4, &CAutoTradeDlg::OnBnBuy1_4)
ON_BN_CLICKED(IDC_BUTTON5, &CAutoTradeDlg::OnBnSellAll)
ON_BN_CLICKED(IDC_BUTTON6, &CAutoTradeDlg::OnBnSell1_2)
ON_BN_CLICKED(IDC_BUTTON7, &CAutoTradeDlg::OnBnSell1_3)
ON_BN_CLICKED(IDC_BUTTON8, &CAutoTradeDlg::OnBnSell1_4)

ON_MESSAGE(WM_TRADEMSG,OnTradeMsg)
ON_BN_CLICKED(IDC_BUTTONRUNHUAAN, &CAutoTradeDlg::OnBnClickedButtonrunhuaan)
ON_BN_CLICKED(IDC_BUTTONRUNTDX1, &CAutoTradeDlg::OnBnClickedButtonruntdx1)
ON_BN_CLICKED(IDC_BUTTONRUNTDX, &CAutoTradeDlg::OnBnClickedButtonruntdx)
ON_BN_CLICKED(ID_KILLTDX, &CAutoTradeDlg::OnBnClickedKilltdx)
ON_BN_CLICKED(ID_RUNTDX, &CAutoTradeDlg::OnBnClickedRuntdx)
ON_WM_COPYDATA()
END_MESSAGE_MAP()

////�ݹ���������Ӵ��ڵ��Ӵ��� , ����
//HWND Find_ChildWindow(HWND parent, char* sWnd)
//{
//	HWND child = NULL;
//	HWND child000 = NULL;
//	TCHAR buf[MAX_PATH];
//	DWORD pid = 0, tid = 0;
//	do {
//		child = FindWindowEx(parent, child, NULL, NULL);
//		int ret = GetWindowText(child, buf, MAX_PATH);
//		buf[ret] = 0;
//
//		if (strlen(buf) > 0)
//		{
//			//LogTrace16380("0x%08X -> 0x%08X %s \n", parent, child, buf);
//			if (strstr(buf, sWnd))
//			{
//				return child;
//			}
//		}
//		if (child)
//			child000 = Find_ChildWindow(child, sWnd);
//		if (child000)
//		{
//			return child000;
//		}
//	} while (child);
//
//	return NULL;
//}

void KeyPress(char chr)
{
	Sleep(50);
	keybd_event(chr, 0, 0, 0);
	keybd_event(chr, 0, KEYEVENTF_KEYUP, 0);
}


void LoginGFZQ()
{
	for (int i = 0; i < 20; i++)
	{
		/*HBITMAP*/ g_hBitmap20[i] = (HBITMAP)::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(i + 8000));
		if (!g_hBitmap20[i])
		{
			int iii = GetLastError();
		}
	}

	HWND hSubWnd = Find_ChildWindow(NULL, "�������Ͻ���ϵͳ");
	if (hSubWnd)
	{
		hSubWnd = Find_ChildWindow(NULL, "�û���¼");//���ҵ�Ψһ���Ӵ��ڣ���ȡ�丸���ڽ��йؼ���ť�Ĳ���
		if (hSubWnd)
		{
			string sVerfy = "";
			string sClass = "";
			HWND hwnd = NULL;

			sClass = "Edit";
			hwnd = Find_ChildWindowByClassWH(hSubWnd, sClass.c_str(), 113, 16);
			if (hwnd)
			{
				//::SendMessage(hwnd, WM_SETTEXT, sizeof(sVerfy.c_str()) / sizeof(char), (LPARAM)sVerfy.c_str());//EDIT�ľ������Ϣ�����ջ�������С�����ջ�����ָ��
				::ShowWindow(hwnd, SW_SHOWNORMAL);
				::SetForegroundWindow(hwnd);
				::SetActiveWindow(hwnd); //��������Ϊ�����
				::SetFocus(hwnd);

				while (hSubWnd != ::GetForegroundWindow())
				{
					Sleep(50);
				}
				::SetFocus(hwnd);
				//hSubWnd = GetFocusEx();
				//while (GetFocusEx() != hwnd)
				//{
				//	Sleep(50);
				//}

				/*Password.ini
				[GFZQ]
				Password=******
				*/
				char buf[MAX_PATH] = { 0 };
				GetPrivateProfileStringA("GFZQ", "Password", "", buf, sizeof(buf), ".\\Password.ini");

				for (int i=0;i<strlen(buf);i++)
				{
					KeyPress(buf[i]);
				}

				if (strlen(buf)>0)//�ж�ȡ��Password
				{
					sClass = "Static";
					hwnd = Find_ChildWindowByClassWH(hSubWnd, sClass.c_str(), 86, 21);
					if (hwnd)
					{
						RECT rect;
						::GetClientRect(hwnd, &rect); //86 * 21
						char pN[256] = "00000.bmp";
						for (int i = 0; i < 5; i++)
						{
							pN[0] = 48 + i;
							sVerfy += (GetBmp4OCR(hwnd, i * 16, 3, 13, 15/*, pN*/));
							if (sVerfy.length() == 0)//not ��ȡ��Verfy
							{
								GetBmp4OCR(hwnd, i * 16, 3, 13, 15, pN);
							}
						}
						TRACE(sVerfy.c_str());
						if (sVerfy.length() > 0)//�ж�ȡ��Verfy
						{
							sClass = "Edit";
							hwnd = Find_ChildWindowByClassWH(hSubWnd, sClass.c_str(), 47, 16);
							if (hwnd)
							{
								TRACE(sVerfy.c_str());
								::SendMessage(hwnd, WM_SETTEXT, sizeof(sVerfy.c_str()) / sizeof(char), (LPARAM)sVerfy.c_str());//EDIT�ľ������Ϣ�����ջ�������С�����ջ�����ָ��
									
								//���ȷ����ť��¼
								hwnd = Find_ChildWindow(hSubWnd, "ȷ��(&Y)");//���ҵ�Ψһ���Ӵ��ڣ���ȡ�丸���ڽ��йؼ���ť�Ĳ���
								if (hwnd)
								{
									::PostMessage(hwnd, BM_CLICK, 0, 0L);
								}
							}
						}
					}
				}
			}
		}
	}
}


void Thread_ClosePopups(PVOID param)
{
	while (true)
	{
		HWND hPopWnd = NULL;
		if (hPopWnd = ::FindWindow("#32770", "ͨ������Ϣ"))
		{
			//::MessageBoxA(NULL, "ͨ�������", "�Ѻ���ʾ", MB_ICONEXCLAMATION);
			::PostMessage(hPopWnd, WM_CLOSE, 0, 0);
		}

		hPopWnd = NULL;
		if (hPopWnd = ::FindWindow("#32770", "ͨ�������"))
		{
			//::MessageBoxA(NULL, "ͨ�������", "�Ѻ���ʾ", MB_ICONEXCLAMATION);
			::PostMessage(hPopWnd, WM_CLOSE, 0, 0);
		}

		HWND hSubWnd = Find_ChildWindow(NULL, "�������Ͻ���ϵͳ");
		if (hSubWnd)
		{
			hSubWnd = Find_ChildWindow(hSubWnd, "ͬʱ����");//���ҵ�Ψһ���Ӵ��ڣ���ȡ�丸���ڽ��йؼ���ť�Ĳ���
			if (hSubWnd)
			{
				//FindClickRefresh(hSubWnd);
				::EnableWindow(hSubWnd, FALSE);//�����ť̫Σ�գ����ε���
			}
		}

		LoginGFZQ();

		Sleep(50);
	}

}

static const char *
inet_ntop_v4(const void *src, char *dst, size_t size)
{
	const char digits[] = "0123456789";
	int i;
	struct in_addr *addr = (struct in_addr *)src;
	u_long a = ntohl(addr->s_addr);
	const char *orig_dst = dst;

	if (size < INET_ADDRSTRLEN) {
		errno = ENOSPC;
		return NULL;
	}
	for (i = 0; i < 4; ++i) {
		int n = (a >> (24 - i * 8)) & 0xFF;
		int non_zerop = 0;

		if (non_zerop || n / 100 > 0) {
			*dst++ = digits[n / 100];
			n %= 100;
			non_zerop = 1;
		}
		if (non_zerop || n / 10 > 0) {
			*dst++ = digits[n / 10];
			n %= 10;
			non_zerop = 1;
		}
		*dst++ = digits[n];
		if (i != 3)
			*dst++ = '.';
	}
	*dst++ = '\0';
	return orig_dst;
}
const char * inet_ntop(int af, const void *src, char *dst, size_t size)
{
	switch (af) {
	case AF_INET:
		return inet_ntop_v4(src, dst, size);
#ifdef HAVE_IPV6
	case AF_INET6:
		return inet_ntop_v6(src, dst, size);
#endif
	default:
		errno = EAFNOSUPPORT;
		return NULL;
	}
}
char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char portstr[7];
	static char str[128];
	switch (sa->sa_family)
	{
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;
		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return NULL;
		if (ntohs(sin->sin_port) != 0)
		{
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return str;
	}
	break;
	case AF_INET6:
	{
		struct sockaddr_in6 *sin = (struct sockaddr_in6 *)sa;
		if (inet_ntop(AF_INET6, &sin->sin6_addr, str, sizeof(str)) == NULL)
			return NULL;
		if (ntohs(sin->sin6_port) != 0)
		{
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin6_port));
			strcat(str, portstr);
		}
		return str;
	}
	break;
	default:
		return NULL;
		break;
	}
}


WORD wVersionRequested = 0;
WSADATA wsaData;
int err = 0;
std::string BroadcastPing(const char* bcIP, u_short uPort, string sProjectName = "", int timeout = 200)
{
	////if (ZLOGP)
	//{
	//	//zlog_info(ZLOGP, "BroadcastPing IP=%s:%d", bcIP, uPort);
	//}

	if (wVersionRequested == 0)
	{
		// ����socket api  
		wVersionRequested = MAKEWORD(2, 2);
		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
		{
			wVersionRequested = 0;
			return  "";
		}

		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		{
			WSACleanup();
			wVersionRequested = 0;
			return  "";
		}
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	fflush(stdout);
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		cout << "sock error" << endl;
		//WSACleanup();
		return  "";
	}
	const int opt = -1;
	int nb = 0;
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));//�����׽�������
	if (nb == -1)
	{
		cout << "set socket error...\n" << endl;
		closesocket(sock);
		//WSACleanup();
		return  "";
	}

	//timeval tv = { 5, 0 };
	//nb = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(timeval));
	; //3s
	nb = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	if (nb == -1)
	{
		cout << "set socket SO_SNDTIMEO error...\n" << endl;
		closesocket(sock);
		////WSACleanup();
		return  "";
	}

	struct sockaddr_in addrto;
	//bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = inet_addr(bcIP);//�׽��ֵ�ַΪ�㲥��ַ
											 //addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);//�׽��ֵ�ַΪ�㲥��ַ
	addrto.sin_port = htons(uPort);//�׽��ֹ㲥�˿ں�Ϊ6000
	int nlen = sizeof(addrto);
	char buff[MAX_BUF_LEN] = "";

	//while (1)
	{
		string msg = g_sCMD;

		//if (sProjectName.length() > 0)
		//{
		//	msg = "ping://" + sProjectName + ".?A";
		//}
		//char msg[] = { "ping://.?A" };
		//if (ZLOGP)
		//zlog_info(ZLOGP, "BroadcastPing... sendto() IP=%s:%d", bcIP, uPort);
		int ret = sendto(sock, msg.c_str(), msg.length()/*strlen(msg)*/, 0, (sockaddr*)&addrto, nlen);//��㲥��ַ������Ϣ

		TraceEx("bcIP = \"%s\"\r\n", bcIP);

		if (ret < 0)
		{
			//if (ZLOGP)
			//zlog_info(ZLOGP, "BroadcastPing... sendto() error... IP=%s:%d", bcIP, uPort);
			cout << "send error...\n" << endl;
			closesocket(sock);
			//WSACleanup();
			return  "";
		}
		else
		{
			printf("\n\nUDP Send ok\n");
			g_sCMD = "ping";

			//if (ZLOGP)
			//zlog_info(ZLOGP, "BroadcastPing... sendto() Send ok IP=%s:%d", bcIP, uPort);
			//timeval tv2 = { 5, 0 };
			//nb = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv2, sizeof(timeval));

			//int timeout = 2000; //3s
			nb = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

			//if (ZLOGP)
			//zlog_info(ZLOGP, "BroadcastPing... setsockopt()  IP=%s:%d", bcIP, uPort);
			if (nb == -1)
			{
				cout << "set socket SO_RCVTIMEO error...\n" << endl;
				closesocket(sock);
				//WSACleanup();
				return  "";
			}

			int nAddrLen = sizeof(SOCKADDR);
			// ��������  
			int nRcvSize = recvfrom(sock, buff, MAX_BUF_LEN - 1, 0, (SOCKADDR*)&addrto, &nAddrLen);
			//if (ZLOGP)
			//zlog_info(ZLOGP, "BroadcastPing... recvfrom()  IP=%s:%d", bcIP, uPort);
			if (SOCKET_ERROR == nRcvSize)
			{
				err = WSAGetLastError();
				printf("\"recvfrom \" error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! error code is %d\n", err);
				//if (ZLOGP)
				//zlog_info(ZLOGP, "BroadcastPing... recvfrom() error... IP=%s:%d error code is %d", bcIP, uPort, err);
				//return  - 1;
				//Sleep(200);
				//continue;
				closesocket(sock);
				//WSACleanup();
				//if (ZLOGP)
				//zlog_info(ZLOGP, "BroadcastPing... recvfrom() error... return('') IP=%s:%d error code is %d", bcIP, uPort, err);
				return  "";

			}

			//if (ZLOGP)
			//zlog_info(ZLOGP, "BroadcastPing... recvfrom() Done... IP=%s:%d", bcIP, uPort);
			printf("\"recvfrom : %s\n", sock_ntop((SOCKADDR*)&addrto, 0));

			buff[nRcvSize] = '\0';

			if (strstr(buff + 4, "pong://"))//ֻ��һ��
			{
				strstr(buff + 4, "pong://")[0] = '\0';
			}

			//if (strstr(buff, "ping"))
			{
				printf("++++++++++RecvData: %s\n", buff);
			}

		}
		//Sleep(200);
	}
	closesocket(sock);
	//WSACleanup();
	return buff;
}
//void Thread_UDP_PingPong(LPVOID Para)
//{
//	while (true)
//	{
//		TraceEx("TDX Thread_UDP_PingPong");
//		//GoListenUDP();
//		Sleep(200);
//	}
//	return;
//}


string UDP_BC_Ping(int uPort)
{
	string sPong = "";
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen;
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);



	// ��һ�ε���GetAdapterInfo��ȡulOutBufLen��С 
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			PIP_ADDR_STRING pIPAddr;
			pIPAddr = &pAdapter->IpAddressList;
			while (pIPAddr)
			{
				if (0 != strcmp(pIPAddr->IpAddress.String, "0.0.0.0"))
				{
					cout << "IP:		" << pIPAddr->IpAddress.String << endl;
					cout << "Mask:		" << pIPAddr->IpMask.String << endl;

					/*
					�㷨��
					1. ����������IP��ַ����λ�����㣬�ô������ַ
					2. �����ַ | (~��������)���ó��㲥��ַ
					*/
					in_addr broadcast;
					broadcast.S_un.S_addr = (
						inet_addr(pIPAddr->IpAddress.String)
						& inet_addr(pIPAddr->IpMask.String)
						)
						| (~inet_addr(pIPAddr->IpMask.String));
					//pAI->strBroadcastIp = inet_ntoa(broadcast);

					cout << "BroadcastIp:	" << inet_ntoa(broadcast) << endl;
					TraceEx("BroadcastPing:	%s	", inet_ntoa(broadcast));

					sPong = BroadcastPing(inet_ntoa(broadcast), uPort);

					if (sPong.length() > 0)
					{
						{
							return sPong;
						}
					}
					cout << endl;
				}
				pIPAddr = pIPAddr->Next;
			}
			pAdapter = pAdapter->Next;
		}
	}
	return sPong;
}


//void Thread_UDP(PVOID param)
//{
//	while (true)
//	{
//		
//
//
//		while (true)
//		{
//			//BC by UDP to 17077
//			//rcv data
//			{
//				string sOK = UDP_BC_Ping(17077);
//
//				//if (sOK.length() == 0)
//				//{
//				//	WSACleanup();
//				//	return ;
//				//}
//			}
//
//			DWORD res = WaitForSingleObject(g_Event_BS_Action, 3000);
//			//MyTraceA("6>>>>>>>>");
//			switch (res)
//			{
//			case WAIT_OBJECT_0:
//				printf("\n��++++++++++++++++++++WS_Event����Event����------��������\n");
//				//G_Vars::g_cWebSocket->ping(hdl, WS_PINGSTR);
//				//((connection_metadata *)Para)->send2WS(lstData2WS);
//				//MyTraceA("44>>>>>>>>");
//
//				//time(&G_Vars::g_TimeA);
//				;
//
//				break;
//			case WAIT_TIMEOUT:
//				printf("\n[WS_WAIT_TIMEOUT],-------��������\n");
//				//G_Vars::g_cWebSocket->ping(hdl, WS_PINGSTR);
//				//time(&G_Vars::g_TimeA);
//				break;
//			case WAIT_ABANDONED:
//				printf("\n[WS_WAIT_ABANDONED]��hHandleΪmutexʱ�����ӵ��mutex���߳��ڽ���ʱû���ͷź��Ķ���������˷���ֵ\n");
//				break;
//			case WAIT_FAILED:
//				printf("\n[WS_WAIT_FAILED]ʧ����, callGetLastError.\n");
//				LPVOID lpMsgBuf;
//				FormatMessage(
//					FORMAT_MESSAGE_ALLOCATE_BUFFER
//					| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//					NULL,
//					GetLastError(),
//					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//					(LPTSTR)&lpMsgBuf,
//					0,
//					NULL
//				);
//				printf("\n[WS_WAIT_FAILED]ʧ��GetLastError is: %s\n", lpMsgBuf);
//				// Process any inserts in lpMsgBuf.
//				// ...
//				// Display the string.
//				//MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
//				// Free the buffer.
//				LocalFree(lpMsgBuf);
//				break;
//			default:
//				break;
//
//			}
//		}
//
////		Sleep(50);
//	}
//
//}

/////////////////////////////////////////////////////////////////////////////
// CAutoTradeDlg message handlers
void WriteDailyLog(CString sLog)
{
	FILETIME CreateTime;// �ļ�������ʱ��
	FILETIME VisitTime;//�ļ�����ʱ��
	FILETIME ModifyTime;// �ļ��޸�ʱ��
	FILETIME LocalTime;//����ʱ��
	BOOL bGotFileCreateTime;
	HANDLE hOriginal = CreateFile( _T("dailyLog.csv"), GENERIC_READ|GENERIC_WRITE ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL );
	if( INVALID_HANDLE_VALUE == hOriginal )
	{
		TRACE("���ļ�ʧ��\n");
	}
	else
	{
		bGotFileCreateTime = GetFileTime( hOriginal, &CreateTime, &VisitTime, &ModifyTime );
		SYSTEMTIME tModify;
		FileTimeToLocalFileTime(&ModifyTime,&LocalTime);//�ջ�ȡ����ʱ����0ʱ���ı�׼ʱ�䣬��ת��������ʱ��
		FileTimeToSystemTime(&LocalTime,&tModify);//ת��ΪSYSTEMTIME
		//TRACE("%d-%2d-%2d %d:%d:%d:%d\n",tModify.wYear,tModify.wMonth,tModify.wDay,tModify.wHour,tModify.wMinute,tModify.wSecond,tModify.wMilliseconds);
		CTime tm=CTime::GetCurrentTime();
		//
		//TRACE(str);
		
		if (tm.GetDay() != tModify.wDay)
		{
			if (tm.GetHour() >= 15)
			{
				DWORD Num;
				::SetFilePointer(hOriginal,0,0,FILE_END);
				CString str=tm.Format("%Y-%m-%d,%H:%M:%S,");

				//CString sLog = "���:852.06  ����:852.06  ��ȡ:852.06  �ο���ֵ:794831.00  �ʲ�:795683.06  ӯ��:-37970.09";
				sLog.Replace(' ',',');
				sLog.Replace(':',',');
				sLog.Replace(",,",",");

				str += sLog;
				str += "\r\n\r\n";
				
				::WriteFile(hOriginal,str,str.GetLength(),&Num,NULL);
			}
			
		}
		::CloseHandle(hOriginal); 
	}
}



static WNDPROC g_pOldWndProc = NULL;
static UINT g_uGetFocusMessage = RegisterWindowMessage(_T("SpecialGetFocusMessage"));
static LRESULT WINAPI GetFocusWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == g_uGetFocusMessage)
	{
		return (LRESULT)GetFocus();
	}
	else
	{
		return CallWindowProc(g_pOldWndProc, hWnd, uMsg, wParam, lParam);
	}
}
HWND GetFocusEx()
{
	HWND hWnd = GetForegroundWindow();
	if (!IsWindow(hWnd))
		return NULL;
	g_pOldWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)GetFocusWindowProc);
	HWND hResult = (HWND)SendMessage(hWnd, g_uGetFocusMessage, 0, 0);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)g_pOldWndProc);
	g_pOldWndProc = NULL;
	return hResult;
}


BOOL CAutoTradeDlg::OnInitDialog()
{
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() ");
	CDialog::OnInitDialog();

	//HWND hwnd = (HWND)0x002D2FEC;
	//RECT rect;
	////hdc = BeginPaint(hwnd, &ps);
	//::GetClientRect(hwnd, &rect); //86 * 21
	//char pN[256] = "111.bmp";
	//string sVerfy = "";
	//for (int i=0;i<5; i++)
	//{
	//	pN[0] = 48+i;
	//	sVerfy += (GetScreenBmp(hwnd, i*16, 3, 13, 15/*, pN*/));
	//}
	//TRACE(sVerfy.c_str());
	int pp = 0;

	//if (NULL == g_Event_BS_Action)
	//{
	//	g_Event_BS_Action = CreateEvent(NULL, TRUE, FALSE, NULL);//�ֶ������ź�
	//}


/*	//    LoadLibrary(_T("Kernel32.dll"));
	typedef BOOL(WINAPI *BeepEx)(DWORD dwFreq, DWORD dwDuration);

	BeepEx BeepX =(BeepEx)::GetProcAddress(
		::GetModuleHandle(_T("Kernel32.dll")), (LPCSTR)("Beep"));

	BOOL bbb=(*(BeepX))(NOTE_5, ONE_BEEP); 
	(*(BeepX))(2000, 500);   
*/
/*	float f = 2366.750f;
	TRACE("%f\n%x\n%f\n", f, *(int *)&f, f);
	//ת�����ݸ�ʽ
	int Date = (int)(20151022);
	int Y = Date/10000;
	int M = (Date % 10000)/100;
	int D = (Date % 100);
	int outDate = (Y-2004)*0x800+M*100+D;
	int hm = (int)(93500)/100;
	int h = hm/100;
	int m = hm % 100;
	int outTime = h*60+m;
	int outZero = 0;
	int outD = outTime * 0x10000 + outDate;
	int xx = 9;
	2015-10-22 09:35 023F5BFE
	float ff = 0.018055;
	CString ss="";
	CString s2="";
	ss.Format("%.6f", ff);
	if(ss.GetLength()>7)
	{
		s2="0."+ss.Mid(ss.Find('.')+1,3);
		ff = atof(s2);
		s2="0."+ss.Mid(ss.Find('.')+4,3);
		ff = atof(s2);
		s2=ss.Mid(ss.Find('.')+1,3);
	}
*/
//	HWND tdxWnd = ::FindWindow("TdxW_MainFrame_Class", NULL);
//int ii=popTimedMessageBox("�յ� **����** �źţ���Ҫ���������𣿣���", 3);
// 	BOOL fOk=FALSE; 
// 	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken)) 
// 	{ 
// 		TOKEN_PRIVILEGES tp; 
// 		tp.PrivilegeCount=1; 
// 		if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid)) 
// 			printf("Can't lookup privilege value.\n"); 
// 		tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED; 
// 		if(!AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL)) 
// 			printf("Can't adjust privilege value.\n"); 
// 		fOk=(GetLastError()==ERROR_SUCCESS); 
// 		CloseHandle(hToken); 
// 	}
// 
// 	DWORD pid;
// 	::GetWindowThreadProcessId(::FindWindowA(NULL, "CPUID Hardware Monitor PRO"), &pid);							// ��ý���ID
// 	HMODULE hModule = fnGetProcessBase(pid);
// 	printf("%X",hModule);

// 	//Զ��ִ�д���
	//iaddress  ��ַƫ�� ���㹫ʽ����ַ-ģ���ַ
// 	HWND hwnd=::FindWindow(NULL,_T("CPUID Hardware Monitor PRO"));
// 	::GetWindowThreadProcessId(hwnd,&pid);	
// 	DWORD dwBaseAddr;
// 
// 	HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid); 
// 	if (hModuleSnap==INVALID_HANDLE_VALUE) 
// 	{ 
// 		AfxMessageBox(_T("��������ģ��ʧ�ܣ�")); 
// 	} 

// 	MODULEENTRY32 me; 
// 	me.dwSize=sizeof(MODULEENTRY32); 
// 	Module32First(hModuleSnap,&me); 
// 	dwBaseAddr=(DWORD)me.modBaseAddr; 
// 	CloseHandle(hModuleSnap); 

// 	m_myHook.initByWindowName("CPUID Hardware Monitor PRO", true);
// 	INT xxx=0xCCCC;
// 	void *p = (LPVOID)0x0452F9F8;//52E9f8;
// 
// 	m_myHook.readProcessMemory((LPVOID)&xxx, p);
// 	INT CCC = GetLastError();

	//WinExec("cmd xxx.EXE monitorTDXpool", 0); 
	//WinExec( "cmd /c tskill monitorTDXpool",SW_NORMAL);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	fontTitle.CreatePointFont(90,"΢���ź�");
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() ");

// 	((CButton*)GetDlgItem(IDC_CHECKRUNTDX))->SetCheck(1);
// 	((CButton*)GetDlgItem(IDC_CHECKRUNTDX1))->SetCheck(1);
// 	((CButton*)GetDlgItem(IDC_CHECKRUNHUAAN))->SetCheck(1);


	KillProcess("Auto_monitorTDXpool.exe"); 
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() ");

	//SYSTEM("CMD /C TSKILL /f /im Auto_monitorTDXpool.exe");
//OnChangeInfo();
//		m_fCurPercent = 0.00;
//		UpdateData(FALSE);

	// TODO: Add extra initialization here

	//RunApp2End("Auto_InitTDXjy.exe"); //˳���������� ���� �ʽ�ɷ� �Գ�ʼ����ԴInstance
	ReadNotSaleListFromFile();
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() 1");
	ReadStkETFFromFile();

	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() 2");
	this->SetTimer(1,5000,NULL);
	
	//����BuyThread:
	hReceiveThread = CreateThread(NULL,0, ThreadProc,(LPVOID)this,0, &ThreadID);
	if ( hReceiveThread == NULL )
		return FALSE;
	// ���ȼ�Ϊ��ͨ
	SetThreadPriority(hReceiveThread,THREAD_PRIORITY_NORMAL);
	::OutputDebugString(" CAutoTradeDlg::OnInitDialog() 999");

	//hProcmonitorTDXpool = (HANDLE)RunApp2End("Auto_monitorTDXpool.exe", "", FALSE); 

	char buf[MAX_PATH] = { 0 };
	GetPrivateProfileStringA("LastTradeMsg", "Msg", "", buf, sizeof(buf), ".\\AutoTrade.ini");
	if (strlen(buf))
	{
		if (IDYES != popTimedMessageBox("�����ϴ��������źţ���Ҫ�ȴ�ini�������", 5))
		{
			m_iLastAction = atoi(buf);
			GetDlgItem(IDC_STATICCMD)->SetWindowText(PrefixTimeStr(buf));

		}
		else
			WritePrivateProfileStringA("LastTradeMsg", "Msg", "", ".\\AutoTrade.ini");
	}

	_beginthread(Thread_ClosePopups, 0, NULL);
	//_beginthread(Thread_UDP, 0, NULL);
	_beginthread(ThreadProcess_TCP_Listen, 0, (void*)PORT_TCP);
	_beginthread(ThreadProcess_UDP_Listen, 0, (void*)PORT_UDP);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoTradeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoTradeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAutoTradeDlg::OnChangeRcvsalecode() 
{
	if(bSaling)
		return;
	UpdateData(TRUE);
	//Lock();
	m_lstSale.InsertString(-1,m_saleCode);//ch1��CString���ַ���
	Unlock();
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
int num;
BOOL   CALLBACK   EnumChildProc(HWND hwnd,LPARAM lParam)
{
	if   (hwnd) //����Ӵ��ڴ��� 
	{ 
		num++;//��¼������Ŀ��num����
		char   chText[MAX_PATH];//�������洰�ڵ�Text   
		ZeroMemory(chText,MAX_PATH); // ZeroMemory����0�����һ���ڴ����� 
		GetWindowText(hwnd,chText,MAX_PATH);//��ȡ���ڵı���
		if(strstr(chText, "List") > 0)//������ڱ���ǿյĻ�
		{
			TRACE("�ҵ�����Ϊ�� %s   �Ĵ���һ�� HWND=%x������%d������\r\n",chText, hwnd, num);
			//��ʽ��chText
			//MessageBox(hwnd,chText,"�ҵ��Ӵ���",MB_OK);//��ʾ��Ϣ 
		}
		else
		{
			//TRACE("�ҵ�����Ϊ�յĴ���һ��������%d������\r\n",num);
			//��ʽ��chText
			//MessageBox(hwnd,chText,"�ҵ��Ӵ���",MB_OK);//��ʾ��Ϣ
		}
		return   TRUE;   
	}   
	return   FALSE;
}

void CAutoTradeDlg::OnReadLV() 
{

}
BOOL IsExistProcess(const char*  szProcessName)
{
	PROCESSENTRY32 processEntry32;
	HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (((int)toolHelp32Snapshot) != -1)
	{
		processEntry32.dwSize = sizeof(processEntry32);
		if (Process32First(toolHelp32Snapshot, &processEntry32))
		{
			do
			{
				int iLen = 2 * strlen(processEntry32.szExeFile);
				char* chRtn = new char[iLen + 1];
				//ת���ɹ�����Ϊ�Ǹ�ֵ
				//wcstombs(chRtn, processEntry32.szExeFile, iLen + 1);
				if (strcmp(szProcessName, processEntry32.szExeFile) == 0)
				{
					return TRUE;
				}
			} while (Process32Next(toolHelp32Snapshot, &processEntry32));
		}
		CloseHandle(toolHelp32Snapshot);
	}
	//
	return FALSE;
}


void CAutoTradeDlg::Run_HuaAnTDX()
{
	char buf[MAX_PATH] = { 0 };
	GetPrivateProfileStringA("LastTradeMsg", "HA_TDXPath", "", buf, sizeof(buf), ".\\AutoTrade.ini");
	if (strlen(buf) > 0)
	{
		//WinExec(buf, SW_NORMAL);
		this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(0);
		RunApp2End("Auto_OpenHuaAn.exe", buf);
		this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(1);
	}
}

void Run_TDX()
{
	char buf[MAX_PATH] = { 0 };
	GetPrivateProfileStringA("LastTradeMsg", "TDXPath", "", buf, sizeof(buf), ".\\AutoTrade.ini");
	if (strlen(buf) > 0)
	{
		//WinExec(buf, SW_NORMAL);
		RunApp2End("Auto_OpenTDX.exe", buf);
	}
}

void CAutoTradeDlg::OnTimer(UINT nIDEvent)
{
	TRACE("Tid=0x%X OnTimer ", ::GetCurrentThreadId());
	// TODO: Add your message handler code here and/or call default
	UpdateData(TRUE);
	if(m_bAutoUpdate && !bSaling)
	{
		Lock();
		GetInfo();
		GetInHandList();
		Unlock();
	}

	CTime tm=CTime::GetCurrentTime();
	//
	//TRACE(str);
	if (!IsExistProcess("TdxW.exe"))
	{
		if ((tm.GetHour() >= 9) && (tm.GetHour() < 15))
		{
			if (tm.GetDayOfWeek() > 1)
			{
				if (tm.GetDayOfWeek() <= 6)
				{
					if (!((tm.GetHour() == 9) && (tm.GetMinute() == 26)))
					{
						Run_HuaAnTDX();
					}
					//Run_TDX();
				}
			}
		}
	}

	if (((tm.GetHour() == 23 || tm.GetHour() == 8) && ((tm.GetMinute() == 58) || (tm.GetMinute() == 59) ))
		||((tm.GetHour() == 9) && (tm.GetMinute() == 26))) //���Ͼ��ۺ�ɱ����
	{
		if (::FindWindow("TdxW_MainFrame_Class", NULL))
		{
			if ((IDNO != popTimedMessageBox("0/9�㽫����ͨ������Ҫ��������Ҫ����kill�𣿣���", 4)))
			{
				TRACE("\r\nRunApp2End ...");
				RunApp2End("tskill", "/A *tdx*", FALSE);
				TRACE("\r\nRunApp2End >>>>>>>");
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CAutoTradeDlg::OnEnChange_RcvInHandStock()
{
	if(bSaling)
		return;
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	//m_stkCode = "xxx";
	Lock();
	m_lstInHand.ResetContent();
	//Split m_stkCode 
	m_stkCode.TrimLeft();
	m_stkCode.TrimRight();

	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_stkCode.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_stkCode, i,' '))
			break;
		TRACE(str1);
		if (i == 0)
		{
			m_iSaleAbleCount = atoi(str1);
		} 
		else
		{
			/////////////////////////////////
			if(m_lstInHand.FindString(0, str1) == -1)
			{
				m_lstInHand.InsertString(-1,str1);//ch1��CString���ַ���
			}
		}
	}
	m_iStocksInHand = m_lstInHand.GetCount();

	Unlock();
	UpdateData(FALSE);
}

void CAutoTradeDlg::OnBnClickedBuyrandom()
{
	Lock();
	vector<int> vi;
	for (int a = 0; a < m_lstBuy.GetCount(); a++)
		vi.push_back(a);
	/*�������������� 100 �� 0-99 ֮����������Ұ���������*/
	
	random_shuffle(vi.begin(), vi.end()); /* ����Ԫ�� */  
	for (int j = 0; j < m_lstBuy.GetCount(); j++)
		TRACE("\r\n %d",vi[j]); /* ��ʾ������˳���Ԫ�� */
	
	long tempRemainMoney = m_fUsableMoney;

	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_lstBuy.GetCount(); ++i )
	{
		CString text; 
		m_lstBuy.GetText(vi[i], text);

		if(m_lstInHand.FindStringExact (0,text) == -1)
		{
			//BuyIt(text);
			if(BuyIt(text))
			{
				//m_lstBuy.DeleteString(vi[i]);
				tempRemainMoney -= iLimitBuyOneMoney;
			}
		}

		//��λ����
		UpdateData(TRUE);

		float fCurPercent = 100 * (1-(tempRemainMoney/m_fTotalMoney));

		CString sOut;
		sOut.Format("�ٷֱ�=%d Ԥ���ٷֱ�=%.1f ʣ��=%d ���ʲ�=%.2f", m_iLimitPercent, fCurPercent, tempRemainMoney, m_fTotalMoney);

		OutDbgStr(sOut);
		if(tempRemainMoney <= 0)
		{
				break;
		}

		if(m_fTotalMoney <= 0)
		{
				break;
		}

		if(m_iLimitPercent <= 0)
		{
				break;
		}

		if(m_bLimitPercent)
		{
			if(m_iLimitPercent <= fCurPercent )
			{
				break;
			}
		}
	}
	this->AfterTrade();
	Unlock();
}

void CAutoTradeDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//WinExec( "cmd /c tskill monitorTDXpool",SW_NORMAL);
	//OnCancel();
	ExitProcess(0);
}


void CAutoTradeDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	TerminateProcess( hProcmonitorTDXpool, 0);
	TerminateProcess( hProcAccountInfo, 0);
	TerminateProcess( hProcGetMyStkList, 0);
	KillProcess("Auto_monitorTDXpool.exe");
}

void CAutoTradeDlg::OnAutoupdate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_bAutoUpdate)
	{
		GetInfo();
		GetInHandList();
	}
}

void CAutoTradeDlg::OnChangeInfo() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	::OutputDebugString(m_sInfo);
	
	//m_sInfo = " ���:727040.50  ����:727040.50  ��ȡ:727040.50  �ο���ֵ:0.00  �ʲ�:727040.50  ӯ��:0.00  ";
	//m_sInfo = "  ���:517737.81  ����:581689.95  ��ȡ:517737.81  �ο���ֵ:150555.00  �ʲ�:732223.95  ӯ��:4852.98  ";
	m_sInfo.TrimLeft();
	m_sInfo.TrimRight();
	CString str1;
	CString str2;
	CString strInfo = "";
	
	if(m_sInfo.GetLength() > 0)
	{
		WriteDailyLog(m_sInfo);

		for (int i = 0; ; i++)
		{
			
			if(!AfxExtractSubString(str1, m_sInfo, i,' '))
				break;
			TRACE(str1);
			AfxExtractSubString(str2, str1, 0,':');
			if(str2 == "���")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fRemainMoney = atof(str2);
			}
			else if(str2 == "����")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fUsableMoney = atof(str2);
			}
			else if(str2 == "��ȡ")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fOutableMoney = atof(str2);
			}
			else if(str2 == "�ο���ֵ")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fValuableMoney = atof(str2);
			}
			else if(str2 == "�ʲ�")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fTotalMoney = atof(str2);

				CString sOut;
				sOut.Format("���ʲ�=%.2f  %s", m_fTotalMoney, str2);
				OutDbgStr(sOut);
			}
			else if(str2 == "ӯ��")
			{
				AfxExtractSubString(str2, str1, 1,':');
				m_fWinMoney = atof(str2);
			}
		}
		if(m_fTotalMoney > 0)
		{
			m_fCurPercent = (m_fValuableMoney/m_fTotalMoney) * 100;
// 			m_iPercent = m_fCurPercent/10 + 1;
// 			m_iPercent *= 10;

		}
		UpdateData(FALSE);
	}
}

void CAutoTradeDlg::Lock()
{
	bSaling = TRUE;
	GetDlgItem(IDBUYRANDOM)->EnableWindow(FALSE);
/*	while(bSaling)
	{
		Sleep(50);
	}
*/}

void CAutoTradeDlg::Unlock()
{
	bSaling = FALSE;	
	GetDlgItem(IDSALEALL)->EnableWindow(TRUE);
	GetDlgItem(IDBUYRANDOM)->EnableWindow(TRUE);
}

//exe tskill


void CAutoTradeDlg::OnEnChangeWinPercent()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_lstRate.InsertString(0, m_strRate);
	UpdateData(FALSE);
}

afx_msg void CAutoTradeDlg::OnBnClickedSaleall()
{
	SaleRate(1);
}

void CAutoTradeDlg::OnBnClickedSaleall2()
{
	SaleRate(2);
}

void CAutoTradeDlg::OnBnClickedSaleall3()
{
	SaleRate(3);
}

void CAutoTradeDlg::OnBnClickedSaleall4()
{
	SaleRate(4);
}

void CAutoTradeDlg::SaleRate( int iRate/*=1*/ )
{
	Lock();
	CString l_text = "";
	//l_text = ListBox1.SelectedValue;
	//TextBox1.Text = l_text;

	int iSaleCount = m_iSaleAbleCount/iRate;
	for (int i = 0; i < iSaleCount; i++)
	{
		m_lstInHand.GetText(i, l_text);
		TRACE(l_text);

		int nIndex = 0;
		if ((nIndex=m_lstNotSale.FindString/*Exact*/(nIndex, l_text)) == LB_ERR)
		{
			SellIt(l_text);
		}
		else if(iSaleCount < m_iSaleAbleCount)
		{
			iSaleCount++;
		}
	}
	this->AfterTrade();
	Unlock();
}

void CAutoTradeDlg::OnLbnSelchangeListinhand()
{
	Lock();
	//UpdateData(TRUE);
	CString l_text = "";
	m_lstInHand.GetText(m_lstInHand.GetCurSel(), l_text);	
	TRACE(l_text);
	m_lstNotSale.InsertString(0, l_text);
	WriteNotSaleListToFile();
	//UpdateData(FALSE);
	Unlock();
}

void CAutoTradeDlg::OnBnClickedNotsalelst()
{
	m_lstNotSale.ResetContent();
	WriteNotSaleListToFile();
}

void CAutoTradeDlg::ReadStkETFFromFile() { 
	CStdioFile sfile;
	CString m_path;
	CString str;

	m_path = _T("./stockETF.txt");

	;

	if (!sfile.Open(m_path, CFile::modeRead))
	{
		m_cmbETF.AddString("159915");
		m_cmbETF.SetCurSel(0);
		return;
	}
	m_cmbETF.ResetContent();

	while(sfile.ReadString(str)/* !=false||str.GetLength() != 0*/)  
	{ 
		m_cmbETF.AddString(str);
	} 
	sfile.Close();
	m_cmbETF.SetCurSel(0);
} 

void CAutoTradeDlg::ReadNotSaleListFromFile() { 
	CStdioFile sfile;
	CString m_path;
	CString str;

	m_path = _T("./notSale.txt");

	if (!sfile.Open(m_path, CFile::modeRead))
	{
		return;
	}
	m_lstNotSale.ResetContent();

	while(sfile.ReadString(str)/* !=false||str.GetLength() != 0*/)  
	{ 
		m_lstNotSale.AddString(str);
	} 
	sfile.Close();
} 

void CAutoTradeDlg::WriteNotSaleListToFile() 
{ 
	CStdioFile sfile;
	CString m_path;
	CString str;
	int size,i,n;

	m_path = _T("./notSale.txt");

	sfile.Open(m_path,CFile::modeCreate|CFile::modeWrite);
	if (!sfile)
	{
		return;
	}
	//sfile.SeekToBegin();
	size = m_lstNotSale.GetCount();
	for(i=0; i<size; i++)
	{ 
		n = m_lstNotSale.GetTextLen(i);

		m_lstNotSale.GetText(i,str.GetBuffer(n));
		sfile.WriteString(str);
		sfile.WriteString("\r\n");
	} 
	sfile.Close();
}

void CAutoTradeDlg::OnLbnSelchangeListNotSale()
{
	Lock();
	//UpdateData(TRUE);
	CString l_text = "";
	m_lstNotSale.DeleteString(m_lstNotSale.GetCurSel());
	WriteNotSaleListToFile();
	//UpdateData(FALSE);
	Unlock();
}

void CAutoTradeDlg::OnBnClickedRadio1()
{
	UpdateData(TRUE);
	switch(m_iRadio01)
	{
	case 0:
		OnEnChange_Rcvbuycode();
		break;
	case 1:
		OnEnChange_Rcvbuycode2();
		break;
	case 2:
		OnEnChange_Rcvbuycode3();
		break;
	}
}
void CAutoTradeDlg::OnEnChange_Rcvbuycode() 
{
	if (m_iRadio01 != 0)
	{
		return;
	}

	if(bSaling)
		return;
	m_lstBuy.ResetContent();
	//todo: ȥ���ظ���
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_buyCode.TrimLeft();
	m_buyCode.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_buyCode.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_buyCode, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //ȥ����ͷ������
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstBuy.FindString(0, str1) == -1)
		{
			m_lstBuy.InsertString(-1,str1);//ch1��CString���ַ���
		}
	}
	m_iInBuyPoolCount = m_lstBuy.GetCount();

	Unlock();

	UpdateData(FALSE);
}


void CAutoTradeDlg::OnEnChange_Rcvbuycode2()
{
	if (m_iRadio01 != 1)
	{
		return;
	}

	if(bSaling)
		return;
	m_lstBuy.ResetContent();
	//todo: ȥ���ظ���
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_buyCode2.TrimLeft();
	m_buyCode2.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_buyCode2.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_buyCode2, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //ȥ����ͷ������
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstBuy.FindString(0, str1) == -1)
		{
			m_lstBuy.InsertString(-1,str1);//ch1��CString���ַ���
		}
	}
	m_iInBuyPoolCount = m_lstBuy.GetCount();

	Unlock();

	UpdateData(FALSE);
}

void CAutoTradeDlg::OnEnChangeRcv_ZTcode()
{
	if(bSaling)
		return;

	if(m_lstInHand.GetCount()<= 0)
		return;
	
	m_lstNotSale.ResetContent();
	ReadNotSaleListFromFile();
	//todo: ȥ���ظ���
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_ZTCode.TrimLeft();
	m_ZTCode.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_ZTCode.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_ZTCode, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //ȥ����ͷ������
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstInHand.FindString(0, str1) != -1)
		{
			if(m_lstNotSale.FindString(0, str1) == -1)
			{
				m_lstNotSale.InsertString(-1,str1);//ch1��CString���ַ���
			}
		}
	}
	//m_iZTCount = m_lstNotSale.GetCount();

	Unlock();

	UpdateData(FALSE);
}



void CAutoTradeDlg::OnEnChange_Rcvbuycode3()
{
	if (m_iRadio01 != 2)
	{
		return;
	}

	if(bSaling)
		return;
	m_lstBuy.ResetContent();
	//todo: ȥ���ظ���
	UpdateData(TRUE);
	//m_stkCode = "xxx";

	Lock();
	//Split m_stkCode 

	m_buyCode3.TrimLeft();
	m_buyCode3.TrimRight();
	CString str1 = "";

	for (int i = 0; ; i++)
	{
		if(m_buyCode3.GetLength() <= 0)
			break;

		if(!AfxExtractSubString(str1, m_buyCode3, i,' '))
			break;
		TRACE(str1);

		if (i == 0) //ȥ����ͷ������
		{
			continue;
		}
		/////////////////////////////////
		if(m_lstBuy.FindString(0, str1) == -1)
		{
			m_lstBuy.InsertString(-1,str1);//ch1��CString���ַ���
		}
	}
	m_iInBuyPoolCount = m_lstBuy.GetCount();

	Unlock();

	UpdateData(FALSE);
}

void CAutoTradeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

HBRUSH CAutoTradeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if((pWnd-> GetDlgCtrlID()==IDC_STATICBUY) /*|| (pWnd-> GetDlgCtrlID()==IDBUYRANDOM)*/)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //������޸ı���ģʽ
		//pDC-> SetBkColor(m_bkcolor); //������޸ı�����ɫ
		//hbr=CreateSolidBrush(m_bkcolor);  //������޸Ļ�ˢ
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(0,128,68));  //�㵱Ȼ�����޸�������ɫ
	} 
	else if((pWnd-> GetDlgCtrlID()==IDC_STATICSALE)  /*|| (pWnd-> GetDlgCtrlID()==IDSALEALL)*/)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //������޸ı���ģʽ
		//pDC-> SetBkColor(m_bkcolor); //������޸ı�����ɫ
		//hbr=CreateSolidBrush(m_bkcolor);  //������޸Ļ�ˢ
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(255,0,0));  //�㵱Ȼ�����޸�������ɫ
	} 
	else if((pWnd-> GetDlgCtrlID()==IDC_STATICCMD)  /*|| (pWnd-> GetDlgCtrlID()==IDSALEALL)*/)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //������޸ı���ģʽ
		//pDC-> SetBkColor(m_bkcolor); //������޸ı�����ɫ
		//hbr=CreateSolidBrush(m_bkcolor);  //������޸Ļ�ˢ
		pDC->SelectObject(&fontTitle);

		switch(m_iLastAction)
		{
			case 0:
				pDC->SetTextColor(RGB(128,120,120));  //�㵱Ȼ�����޸�������ɫ
				break;
			case 1:
				pDC->SetTextColor(RGB(255,0,0));  //�㵱Ȼ�����޸�������ɫ
				break;
			case 2:
				pDC->SetTextColor(RGB(0,128,68));  //�㵱Ȼ�����޸�������ɫ
				break;
			default:
				break;
		}
	} 
	else if(pWnd-> GetDlgCtrlID()==IDC_STATICSB) 
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //������޸ı���ģʽ
		//pDC-> SetBkColor(m_bkcolor); //������޸ı�����ɫ
		//hbr=CreateSolidBrush(m_bkcolor);  //������޸Ļ�ˢ
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(0,0,255));  //�㵱Ȼ�����޸�������ɫ
	} 
	else if(pWnd-> GetDlgCtrlID()== IDC_CB_AUTOSELL)
	{ 
		pDC-> SetBkMode(TRANSPARENT);   //������޸ı���ģʽ
		//pDC-> SetBkColor(m_bkcolor); //������޸ı�����ɫ
		//hbr=CreateSolidBrush(m_bkcolor);  //������޸Ļ�ˢ
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(0,188,0));  //�㵱Ȼ�����޸�������ɫ
	} 
	else if (pWnd->GetDlgCtrlID() == IDC_CB_AUTOBUY)
	{
		pDC->SetBkMode(TRANSPARENT);   //������޸ı���ģʽ
									   //pDC-> SetBkColor(m_bkcolor); //������޸ı�����ɫ
									   //hbr=CreateSolidBrush(m_bkcolor);  //������޸Ļ�ˢ
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(255, 0, 0));  //�㵱Ȼ�����޸�������ɫ
	}
	else if (pWnd->GetDlgCtrlID() == IDC_CLIENTS)
	{
		pDC->SetBkMode(TRANSPARENT);   //������޸ı���ģʽ
									   //pDC-> SetBkColor(m_bkcolor); //������޸ı�����ɫ
									   //hbr=CreateSolidBrush(m_bkcolor);  //������޸Ļ�ˢ
		pDC->SelectObject(&fontTitle);
		pDC->SetTextColor(RGB(255, 0, 0));  //�㵱Ȼ�����޸�������ɫ
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CAutoTradeDlg::AfterTrade()
{
	//RunApp2End("Auto_TDXAfterTrade.exe"); //����ʽ�ɷ�
}

void CAutoTradeDlg::OnNMCustomdrawBuyrandom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CAutoTradeDlg::NewBuyRate(int iRate)
{
	CString text; 
	UpdateData(TRUE);
	if (m_bUDP)
	{
		//extern HANDLE g_Event_BS_Action;
		g_sCMD.Format("%.2f", 1.0f/iRate);

		for (list<HANDLE>::iterator it = g_Event_BS_Action.begin(); it != g_Event_BS_Action.end(); ++it)
			SetEvent(*it);

		//ResetEvent(g_Event_BS_Action);
		return;
	}

	Lock();

	/*
	1ȡ��ǰ����
	((CComboBox*)GetDlgItem(IDC_COMBO_CF))->GetWindowText(strTemp);
	2ȡ����������
	((CComboBox*)GetDlgItem(IDC_COMBO_CF))->GetLBText(n,strTemp);
	*/
	m_cmbETF.GetWindowText(text);

	if (text.GetLength() > 5)
	{
		if (m_bLimitPercent)
		{
			text.Format("%s 1/%d %d", text, iRate, m_iLimitPercent); 
		} 
		else
		{
			text.Format("%s 1/%d %d", text, iRate, 100);
		}
		::OutputDebugString(text);

		BuyIt(text);
		this->AfterTrade();
	}

	Unlock();
}

void CAutoTradeDlg::NewSellRate( int iRate )
{
	UpdateData(TRUE);
	if (m_bUDP)
	{
		//extern HANDLE g_Event_BS_Action;
		g_sCMD.Format("-%.2f", 1.0f / iRate);

		for (list<HANDLE>::iterator it = g_Event_BS_Action.begin(); it != g_Event_BS_Action.end(); ++it)
			SetEvent(*it);

		return;
	}
	Lock();

	CString text; 
	m_cmbETF.GetWindowText(text);

	if (text.GetLength() > 5)
	{
		if (m_bLimitPercent)
		{
			text.Format("%s 1/%d %d", text, iRate, m_iLimitPercent);
		}
		else
		{
			text.Format("%s 1/%d %d", text, iRate, 100);
		}
		::OutputDebugString(text);

		SellIt(text);
		this->AfterTrade();
	}

	Unlock();
}

void CAutoTradeDlg::OnBnBuyAll()
{
	NewBuyRate(1);
}

void CAutoTradeDlg::OnBnBuy1_2()
{
	NewBuyRate(2);
}

void CAutoTradeDlg::OnBnBuy1_3()
{
	NewBuyRate(3);
}

void CAutoTradeDlg::OnBnBuy1_4()
{
	NewBuyRate(4);
}

void CAutoTradeDlg::OnBnSellAll()
{
	NewSellRate(1);
}

void CAutoTradeDlg::OnBnSell1_2()
{
	NewSellRate(2);
}

void CAutoTradeDlg::OnBnSell1_3()
{
	NewSellRate(3);
}

void CAutoTradeDlg::OnBnSell1_4()
{
	NewSellRate(4);
}

//int readLastTradeMsgFormIni()
//{
//
//	string lpPath = ".\\AutoTrade.ini";
//	if (sClientPathFile.length() == 0)
//	{
//		char buf[MAX_PATH] = { 0 };
//		GetPrivateProfileStringA("LastTradeMsg", "Msg", "", buf, sizeof(buf), ".\\AutoTrade.ini");
//		sClientPathFile = buf;
//	}
//	else
//	{
//		WritePrivateProfileStringA("LastTradeMsg", "Msg", sClientPathFile.c_str(), ".\\AutoTrade.ini");
//	}
//	return;
//}

LRESULT CAutoTradeDlg::OnTradeMsg( WPARAM wParam, LPARAM lParam )
{
	TRACE("Tid=0x%X OnTradeMsg ",::GetCurrentThreadId());
	CString sAct = ""; 

	CTime tm = CTime::GetCurrentTime();

	if (m_iLastAction != lParam)
	if (((tm.GetHour()*100+ tm.GetMinute()) >= 931)) //9:31֮���յ�ָ��Ŷ���
	{

		UpdateData(TRUE);

		switch(lParam)
		{
		case 0:
			sAct = "�޲���";
			break;
		case 1:
			sAct = "����";
			m_iLastAction = lParam;

			if (m_bAutoBuy)
			{
				NewBuyRate(m_iRateB);
			}
			else if (IDYES == popTimedMessageBox("�յ������롿�źţ���Ҫ���������𣿣���", 10))
			{
				NewBuyRate(m_iRateB);
			}
			
			//WritePrivateProfileStringA("LastTradeMsg", "Msg", "1", ".\\AutoTrade.ini");
			break;
		case 2:
			sAct = "����";
			m_iLastAction = lParam;

			if (m_bAutoSell) 
			{
				NewSellRate(m_iRateS);
			}
			else if (IDYES == popTimedMessageBox("�յ� **����** �źţ���Ҫ���������𣿣���", 10))
			{
				NewSellRate(m_iRateS);
			}

			//WritePrivateProfileStringA("LastTradeMsg", "Msg", "2", ".\\AutoTrade.ini");
			break;
		default:
			break;
		}

		if (lParam != 0)
		{
			m_lstTradeMSG.InsertString(0, PrefixTimeStr(sAct.GetBuffer()));
			::OutputDebugString("OnTradeMsg " + sAct);
			m_lstTradeMSG.SetCaretIndex(0, 1);

			//GetDlgItem(IDC_STATICCMD)->SetWindowText(PrefixTimeStr(sAct.GetBuffer()));

			if (m_bDrawRedTxt)
			{
				OnScreenfont(PrefixTimeStr(sAct.GetBuffer()));
			}
		}
	}

	GetDlgItem(IDC_STATICCMD)->SetWindowText(PrefixTimeStr(sAct.GetBuffer()));

	sAct.Format("OnTradeMsg lParam=%d", lParam);
	::OutputDebugString(sAct);

	return 0;
}

void CAutoTradeDlg::OnBnClickedRuntdx()
{
	GetDlgItem(IDOK2)->EnableWindow(FALSE);

	CString sPara = " 0 1 2 "; 
	RunApp2End(AUTO_OPEN_ALL, sPara);
}

void CAutoTradeDlg::OnBnClickedKilltdx()
{
	RunApp2End("tskill.exe", "TdxW", FALSE);
	GetDlgItem(IDOK2)->EnableWindow();
}

//bug�ʹ�����
//���Ͼ��۳�Ϊ��һ����--����֤ȯ
//��������
//ָ���Ż�

void CAutoTradeDlg::OnBnClickedButtonrunhuaan()
{
	//CString sPara = " 2 "; 
	//RunApp2End(AUTO_OPEN_ALL, sPara);
	//this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(0);
	Run_HuaAnTDX();
	//this->GetDlgItem(IDC_BUTTONRUNHUAAN)->EnableWindow(1);
}

void CAutoTradeDlg::OnBnClickedButtonruntdx1()
{
	CString sPara = " 1 "; 
	RunApp2End(AUTO_OPEN_ALL, sPara);
}

void CAutoTradeDlg::OnBnClickedButtonruntdx()
{
	//CString sPara = " 0 "; 
	//RunApp2End(AUTO_OPEN_ALL, sPara);
	Run_TDX();
}



BOOL CAutoTradeDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//m_strCopyData = (LPSTR)pCopyDataStruct->lpData;

	// ���ʵ�ʳ��ȵ��ַ���

	m_strCopyData = "\r\n" + m_strCopyData;
	//m_strCopyData = "\r\n" + CString(PrefixTimeStr(" ")) + m_strCopyData;
	m_strCopyData = CString(PrefixTimeStr(" ")) + ((CString)(LPSTR)(pCopyDataStruct->lpData)).Left(pCopyDataStruct->cbData) + m_strCopyData;

	// ��������

	UpdateData(FALSE);

	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}
