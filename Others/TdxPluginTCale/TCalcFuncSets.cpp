#include "stdafx.h"
#include "TCalcFuncSets.h"
#define WM_TRADEMSG WM_USER+1
#include <stdio.h>  
#include<math.h>
#include <stdarg.h>  
#include<string>
#include<sstream>
//#include "winsock.h"
#include "mysql.h"//ͷ�ļ�˳���ܵߵ�
#include <..\..\..\..\..\VS2013\VC\include\iostream>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <iptypes.h>
#include <iphlpapi.h>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

MYSQL mysql; //���ݿ����Ӿ��
BOOL bConnectMysql = FALSE;
using namespace std;

void TraceEx(const wchar_t *strOutputString, ...)  
{  
	va_list vlArgs = NULL;  
	va_start(vlArgs, strOutputString);  
	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;  
	wchar_t *strBuffer = new wchar_t[nLen];  
	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);  
	va_end(vlArgs);  
	OutputDebugStringW(strBuffer);  
	delete [] strBuffer;  
}  

void TraceEx(const char *strOutputString, ...)  
{  
	va_list vlArgs = NULL;  
	va_start(vlArgs, strOutputString);  
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1 + 6;  
	char *strBuffer = new char[nLen]; 
	strcpy(strBuffer, "[TDX] ");
	_vsnprintf_s(strBuffer+6, nLen, nLen, strOutputString, vlArgs);  
	va_end(vlArgs);  
	OutputDebugStringA(strBuffer);  
	delete [] strBuffer;  
}  
//test  
// 	TraceEx(L"DEBUG_INFO | %d %s\r\n", 1234, L"this is test by kandy(wchar_t*)");  
// 	TraceEx("DEBUG_INFO | %d %s\r\n", 5678, "this is test by kandy(char*)");  
// 	
double round(double r)
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}
void OnScreenfont(char* szBuffer, int x, int y, COLORREF newClr)
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
	clr = SetTextColor(hdc, newClr);
	TextOut(hdc, x, y, szBuffer, strlen(szBuffer));

	//��ԭ��ɫ
	SetTextColor(hdc, clr);

	//�ͷž��DC
	DeleteObject(font); 
	ReleaseDC(hwnd, hdc);
}


//1������mysql
int ConnectMysql(LPCTSTR sip,LPCTSTR suser,LPCTSTR spass,LPCTSTR sdbname,int sport)
{
	if( mysql_init(&mysql) == NULL )
	{
		::OutputDebugString("��ʼ��mysqlʧ��!");
		return FALSE;
	}
	//ʵ��mysql_real_connect(&mysql,"127.0.0.1","root","","zkeco_db",17770,NULL,0)
	if(!mysql_real_connect(&mysql,(const char *)sip,(const char *)suser,(const char *)spass,(const char *)sdbname,sport,NULL,0))
	{
		//TRACE("����mysqlʧ��:%s",mysql_error(&mysql)); 
		return FALSE;
	}
	else
	{
		//Trace2Logger("���ݿ����ӳɹ�");
		return TRUE;
	}
}

BOOL bLastData = FALSE;
void ShowTable(float* fOut, float a1,float a2,float a3,float a4 )
{
	//��ʾ����
	MYSQL_RES *result=NULL;
	//if(0==mysql_query(&mysql,"SELECT * FROM stock.399006_1min LIMIT 0, 100"))
/*
SELECT * FROM stock.399006_1min_result where 
min1a=floor(round(-0.12,2)*2)/2 and 
min2a=floor(round(-0.22,2)*2)/2 and 
min3a=floor(round(-0.32,2)*2)/2 and 
min4a=floor(round(-0.49,2)*2)/2;
 **/

	//OutputDebugString("111*********");
	TCHAR strSQL[1024] = {0};
	sprintf(strSQL, "SELECT * FROM stock.399006_1min_result where min1a=floor(round(%.2f,2)*2)/2 and min2a=floor(round(%.2f,2)*2)/2 and min3a=floor(round(%.2f,2)*2)/2 and min4a=floor(round(%.2f,2)*2)/2", a1, a2, a3, a4);
	if (bLastData)
	{
		::OutputDebugString("\r\n");
		::OutputDebugString(strSQL);
		::OutputDebugString("\r\n");
	}
	if(0==mysql_query(&mysql,strSQL))
	{
		//OutputDebugString("222");
		std::string loginfo;
		//loginfo="mysql_query() select data succeed";
		//MessageBox(loginfo);
		//һ����ȡ�����ݼ�
		result=mysql_store_result(&mysql);

		//ȡ�ò���ӡ����
		LONG64 rowcount=mysql_num_rows(result);
		/*loginfo.Format("row count=%d",rowcount);
		MessageBox(loginfo);       */

		//ȡ�ò���ӡ���ֶε�����
		unsigned int fieldcount=mysql_num_fields(result);
		MYSQL_FIELD *field=NULL;
		//loginfo=loginfo+"\r\n";
		//for(unsigned int i=0;i<fieldcount;i++)
		//{
		//	field=mysql_fetch_field_direct(result,i);
		//	loginfo=loginfo+(field->name)+",";
		//}
		//loginfo=loginfo+"\r\n";
		//::OutputDebugString(loginfo.c_str());       

		//OutputDebugString("333");
		//��ӡ����
		MYSQL_ROW row=NULL;
		row=mysql_fetch_row(result);
		//row=NULL;
		loginfo="\r\n[TDX]DB==> ";
		while(NULL!=row)
		{
			for(int i=0; i<fieldcount;i++)
			{
				//loginfo=loginfo+(row[i])+",";

				field=mysql_fetch_field_direct(result,i);
				if (strstr(field->name, "ZEROs") <= 0)
				{
					loginfo=loginfo+(field->name)+"= ["+(row[i])+"]";
					loginfo=loginfo+" ,";
					if ((i+1)==fieldcount)
					{
						loginfo=loginfo+"ӯռ�ȣ�";

						*fOut = (atof(row[4]))/((atof(row[4]) + atof(row[6])));
						TCHAR tmp[1024] = {0};
						sprintf(tmp, "%d��",(int)round((*fOut) * 100));

						if (bLastData)
						{
							OnScreenfont(row[4], 400, 10, RGB(255, 0, 0));
							OnScreenfont(row[6], 600, 10, RGB(0, 255, 0));
							OnScreenfont(tmp, 800, 10, RGB(0, 0, 255));
						}
						loginfo=loginfo+tmp;
					}
				}
			}
			loginfo=loginfo+";";
			//OutputDebugString("444");
			row=mysql_fetch_row(result);
		}
		loginfo=loginfo+"\r\n";
		if (bLastData)
		{
			::OutputDebugString(loginfo.c_str());
			bLastData = FALSE;
		}
	}   
	else {
		//MessageBox("mysql_query() select data failed");       
	}
	//OutputDebugString("111Done*********");

}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) 
{ 
	DWORD dwCurProcessId = *((DWORD*)lParam); 
	DWORD dwProcessId = 0; 

	GetWindowThreadProcessId(hwnd, &dwProcessId); 
	if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
	{ 
		*((HWND *)lParam) = hwnd;
		return FALSE; 
	} 
	return TRUE; 
} 


HWND GetMainWindow() 
{ 
	DWORD dwCurrentProcessId = GetCurrentProcessId();
	if(!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId)) 
	{     
		return (HWND)dwCurrentProcessId; 
	} 
	return NULL; 
} 

//���ɵ�dll���������dll�뿽����ͨ���Ű�װĿ¼��T0002/dlls/����,���ڹ�ʽ���������а�
float fMaxLost = 0;
float fWinRate = 0;
float fTotalAsset = 0;
float fStartPrice = 0;
float fEndPrice = 0;
int iSaleCount = 0;
int iWinCount = 0;
float fFirstDay = 0;;
float fEndDay = 0;;

void PostMsgToTradeWnd(LPARAM lParam)
{
	TCHAR sTradeWndTitle[256] = "�Զ������׿�������";
	HWND hTradeWnd = NULL;
	hTradeWnd = ::FindWindow(NULL, sTradeWndTitle);
	if( hTradeWnd != NULL) 
	{ 
		::PostMessage(hTradeWnd,WM_TRADEMSG,0,lParam); 
	}  
}
//��һ�汾��T+1 bug�޷��������������ڶ��汾
// void TestPlugin1(int DataLen,float* pfOUT,float* buyPrice,float* salePrice,float* closePrice)
// { /*TDXDLL2(1,BUYP,SALEP,CCC)*/
// 	//OutputDebugString("\r\nTestPlugin1");
// 	BOOL bBuy = FALSE;
// 	//float fFirstBuy = 0;
// 	float fLastSale = 0;
// 	//float fBuy = 0;
// 	//float fWin = 0;
// 	float fStockCounts = 0;
// 
// 	float fHighWin = 0;
// 
// 	fMaxLost = 0;
// 	fWinRate = 0;
// 	fTotalAsset = 0;
// 	fStartPrice = 0;
// 	iSaleCount = 0;
// 	iWinCount = 0;
// 
// 	if (DataLen>0)
// 	{
// 		fTotalAsset = closePrice[0];//��ʼ�ʲ�=��һ������̼�
// 		fStartPrice = closePrice[0];
// 
// 		for(int i=0;i<DataLen;i++)
// 		{
// 			pfOUT[i]=fTotalAsset;//��������ʲ�
// 			if (bBuy)//�ֹ���
// 			{
// 				pfOUT[i] = closePrice[i] * fStockCounts;
// 
// 				//�������س� fMaxLost
// 				if (pfOUT[i] > fHighWin)
// 				{
// 					fHighWin = pfOUT[i];
// 				}
// 				else
// 				{
// 					float ftempLost = (fHighWin - pfOUT[i])/ fHighWin * 100;
// 					if (ftempLost > fMaxLost)
// 					{
// 						fMaxLost = ftempLost;
// 					}
// 				}
// 				//�������س�����
// 			}
// 			if(buyPrice[i] > 0)//����
// 			{
// 				if (!bBuy)
// 				{
// 					bBuy = TRUE;
// 
// 					fStockCounts = fTotalAsset / buyPrice[i];
// 				}
// 			}
// 			if(salePrice[i] > 0)//����
// 			{
// 				if (bBuy)
// 				{
// 					bBuy = FALSE;
// 					fLastSale = salePrice[i];
// 					float fCurrMoney = 0;
// 					fCurrMoney = salePrice[i] * fStockCounts * 0.9995/*�۳� ETF���� ������2.5��������*/;
// 					if (fCurrMoney > fTotalAsset)
// 					{
// 						iWinCount++;
// 					}
// 					fTotalAsset = fCurrMoney;
// 					iSaleCount++;
// 
// // 					TCHAR bbb[256] = {0};
// // 					sprintf(bbb, "����,%.0f,����,%.0f,����,%.0f,С��,%.0f,���ʲ�,%.0f,", fBuy, fLastSale, fWin, fTotalAsset, fTotalAsset+fFirstBuy);
// // 					OutputDebugString(bbb);
// 				}
// 			}
// 
// 		}
// 
// 		fWinRate = (fTotalAsset - closePrice[0]) / closePrice[0] * 100; //����������
// 
// 	}
// }

//�ڶ��汾
void CalcWin(int DataLen,float* pfOUT,float* currPrice,float* fAction,float* currDay)
{
	/*TDXDLL2(1,Price,Action,DAY)*/
	//OutputDebugString("\r\nTestPlugin1");
	BOOL bBought = FALSE;
	BOOL bLastSale = FALSE;
	//float fFirstBuy = 0;
	//float fLastSale = 0;
	//float fBuy = 0;
	//float fWin = 0;
	float fStockCounts = 0;

	float fHighWin = 0;

	fMaxLost = 0;
	fWinRate = 0;
	fTotalAsset = 0;
	fStartPrice = 0;
	fEndPrice = 0;
	iSaleCount = 0;
	iWinCount = 0;
	float fBuyDay = 0; 
	fFirstDay = 0;
	fEndDay = 0;

	if (DataLen>0)
	{
		TraceEx("\r\n[TDX]==>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
		//fTotalAsset = currPrice[0];//��ʼ�ʲ�=��һ������̼�
		//fStartPrice = currPrice[0];
		fEndPrice = currPrice[DataLen-1];
		fFirstDay = currDay[0];
		fEndDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			pfOUT[i]=fTotalAsset;//��������ʲ�
			if (bBought)//�ֹ���
			{
				pfOUT[i] = currPrice[i] * fStockCounts;

				//�������س� fMaxLost
				if (pfOUT[i] > fHighWin)
				{
					fHighWin = pfOUT[i];
				}
				else
				{
					float ftempLost = (fHighWin - pfOUT[i])/ fHighWin * 100;
					if (ftempLost > fMaxLost)
					{
						fMaxLost = ftempLost;
					}
				}
				//�������س�����

				if ((fAction[i] == 0.0))
				{
					if ((fBuyDay != currDay[i]))
					{
						if (bLastSale == TRUE)
						{
							bLastSale = FALSE;
							fAction[i] = 2.0;//�ֹ��У����ڱ仯����һK����
						}
					}
				}
			}
			if(fAction[i] == 1.0)//����
			{
				if (!bBought)
				{
					bBought = TRUE;
					fBuyDay = currDay[i];

					if (fStartPrice == 0.0)
					{
						fTotalAsset = currPrice[i];//��ʼ�ʲ�=��һBUY�����̼�
						fStartPrice = currPrice[i];

						for (int xx = 0; xx <= i; xx++)
						{
							pfOUT[xx] = fTotalAsset;//�����һ��֮ǰ�����ʲ�
						}
					}

					fStockCounts = fTotalAsset / currPrice[i];
//#ifdef _DEBUG
					TCHAR bbb[256] = { 0 };
					sprintf(bbb, "+++����day,%.0f,�۸�,%.3f,���ʲ�,%.3f,", fBuyDay-1000000, currPrice[i], fTotalAsset);
					OutputDebugString(bbb);
//#endif
				}
				bLastSale = FALSE;
			}
			else if((fAction[i] == 2.0))
			{
				if ((fBuyDay != currDay[i]))//����
				{
					if (bBought)
					{
						bBought = FALSE;
						//fLastSale = fAction[i];
						float fCurrMoney = 0;
						fCurrMoney = currPrice[i] * fStockCounts * 0.9995/*�۳� ETF���� ������2.5��������*/;
						if (fCurrMoney > fTotalAsset)
						{
							iWinCount++;
						}
						fTotalAsset = fCurrMoney;
						iSaleCount++;

	//#ifdef _DEBUG
						TCHAR bbb[256] = { 0 };
						sprintf(bbb, "---����day,%.0f,�۸�,%.3f,���ʲ�,%.3f,[%d]��", currDay[i]-1000000, currPrice[i], fTotalAsset, iSaleCount);
						OutputDebugString(bbb);
	//#endif
					}
				}			
				else
				{
					bLastSale = TRUE;
				}

			}

		}

		if (fStartPrice > 0.0)
		{
			fWinRate = (fTotalAsset - fStartPrice) / fStartPrice * 100; //����������
		}

	}
}

void getReal_BS(int DataLen,float* pfOUT,float* currDay,float* currTime,float* fAction)
{
	//ʵ������:TDXDLL2(5,DATE,TIME,IF(ȫ���,1,IF(ȫ����,2,0)));

	float fBuyDay = 0; 
	//BOOL bBought = FALSE;
	BOOL bNextDaySale = FALSE;
	//float fFirstBuy = 0;
	//float fLastSale = 0;
	//float fBuy = 0;
	//float fWin = 0;
	fFirstDay = 0;
	fEndDay = 0;

	if (DataLen>0)
	{
		TraceEx("\r\n[TDX]==>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
		fFirstDay = currDay[0];
		fEndDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			pfOUT[i]=0;//���

			if (currTime[i] >= 1457) //����ǰ���Ͼ���ʱ��
			{
				if (fAction[i] > 0.0f)//�ж���
				{
					if ((i+1)<DataLen)//����һ����
					{
						if (fAction[i+1] == 0.0f)//��һ�����޶���
						{
							fAction[i+1] = fAction[i];//��ǰ����������һ����
						}
						fAction[i] = 0.0f;//��ǰ�������
					}
				}

				continue;
			}

			if (fBuyDay != 0.0f)//�ֹ���
			{
				if ((fAction[i] == 0.0f))
				{
					if ((fBuyDay != currDay[i]))
					{
						if (bNextDaySale == TRUE)
						{
							bNextDaySale = FALSE;
							fAction[i] = 2.0f;//�ֹ��У����������źţ����ڱ仯����һK����
						}
					}
				}
			}

			if(fAction[i] == 1.0f)//����
			{
				if ((fBuyDay == 0.0f) )
				{
					fBuyDay = currDay[i];
					pfOUT[i] = 1.0f;//���
				}
				bNextDaySale = FALSE;
			}
			else if((fAction[i] == 2.0f))
			{
				if ((fBuyDay != currDay[i]) && (fBuyDay != 0.0f))//����
				{
					fBuyDay = 0.0f;
					pfOUT[i] = 2.0f;//���
				}			
				else if ((fBuyDay != 0.0f))
				{
					bNextDaySale = TRUE;
				}

			}

		}

	}
}

//std::vector<float>
//Ema(std::vector<float> &X, int N)
//{
//	std::vector<float> vec;
//	int nLen = X.size();
//	if (nLen >= 1)
//	{
//		if (N > nLen) N = nLen;
//
//		vec.resize(nLen);
//		//vec.reserve(nLen);
//		vec[0] = X[0];
//		for (int i = 1; i < nLen; i++)
//		{
//			vec[i] = (2 * X[i] + (N - 1) * vec[i - 1]) / (N + 1);
//		}
//	}
//	return vec;
//}

//float* pfDIF1 = NULL;
//float lastBuyDate = 0;
//float lastBuyPrice = 0;
//float curMoney = 100000;
//float curStocks = 0;
//
//void testDea(int DataLen, float* pfOUT, float* pfINa, int iMultiple, float* pfDate, BOOL bTestCalc)
//{
//	int iShort = 10 * iMultiple;
//	int iLong = 12 * iMultiple;
//	int iMid = 9;
//	TraceEx("\r\n[TDX]======testDea \tiMultiple=%d ����=%d\tDataLen=%d\t", iMultiple, bTestCalc, DataLen);
//
//	if ((DataLen > 0))
//	{
//		if (iShort > DataLen) iShort = DataLen;
//		if (iMid > DataLen) iMid = DataLen;
//		if (iLong > DataLen) iLong = DataLen;
//
//		float* pfTmp1 = new float[DataLen];
//		float* pfTmp2 = new float[DataLen];
//		pfTmp1[0] = pfINa[0];
//		pfTmp2[0] = pfINa[0];
//
//		pfOUT[0] = 0;
//		if ((iMultiple == 1) || !bTestCalc)
//		{
//			pfDIF1[0] = 0;
//		}
//
//		int iBS = 0;
//		for (int i = 1; i < DataLen; i++)
//		{
//			pfTmp1[i] = (2 * pfINa[i] + (iShort - 1) * pfTmp1[i - 1]) / (iShort + 1);	//EMA���㹫ʽ
//			pfTmp2[i] = (2 * pfINa[i] + (iLong - 1) * pfTmp2[i - 1]) / (iLong + 1);		//EMA���㹫ʽ
//			if ((iMultiple == 1) /*|| !bTestCalc*/)
//			{
//				pfDIF1[i] = pfTmp1[i] - pfTmp2[i];
//			}
//			pfOUT[i] = (2 * (pfTmp1[i] - pfTmp2[i]) + (iMid - 1) * pfOUT[i - 1]) / (iMid + 1);	//EMA���㹫ʽ
//
//			//TraceEx("\r\n[TDX]======testDea \tiMultiple=%d ����=%d\t%.3f\t%.3f\t%.3f\t%.3f", iMultiple, bTestCalc, pfOUT[i], pfDIF1[i], pfTmp1[i], pfTmp2[i]);
//			if (bTestCalc)
//			{
////////////////////////////////////////Calc Win
//				if ((pfDIF1[i - 1] < pfOUT[i - 1]) && (pfDIF1[i] > pfOUT[i])) //CrossUp
//				{
//					if ((lastBuyDate < pfDate[i]) && (curMoney > 0))
//					{
//						curStocks = curMoney / pfINa[i];
//						lastBuyDate = pfDate[i];
//						lastBuyPrice = pfINa[i];
//						curMoney = 0;
//						//TraceEx("\r\n[TDX]======B \t%d\t��%.3f\t��%.3f\t%d\tdif=%.3f\tout=%.3f", iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000, pfDIF1[i], pfOUT[i]);
//					}
//
//				}
//				else if ((pfDIF1[i - 1] > pfOUT[i - 1]) && (pfDIF1[i] < pfOUT[i])) //CrossDown
//				{
//					if ((lastBuyDate < pfDate[i]) && (curStocks > 0))
//					{
//						iBS++;
//						curMoney = curStocks * pfINa[i];
//						lastBuyDate = 0;
//						lastBuyPrice = 0;
//						curStocks = 0;
////						TraceEx("\r\n[TDX]======S \t%d\t��%.3f\t��%.3f\t%d", iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000);
//						//TraceEx("\r\n[TDX]===SS \t%d\t��%.3f\t��%.3f\t%d\tdif=%.3f\tout=%.3f", iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000, pfDIF1[i], pfOUT[i]);
//					}
//				}
//
//				if (i == DataLen-1)
//				{
//					TraceEx("\r\n[TDX]===ӯ[%.3f%%] ��[%d] \t%d\t��%.3f\t��%.3f\t%d\tdif=%.3f\tout=%.3f", (curMoney + (curStocks * pfINa[i]) - 100000)/1000, iBS, iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000, pfDIF1[i], pfOUT[i]);
//				}
////////////////////////////////////////Calc Win DONE.
//			}
//		}
//		TraceEx("\r\n[TDX]=====End= \tiMultiple=%d\t%.3f", iMultiple, curMoney+(curStocks * pfINa[DataLen-1]));
//
//		delete[] pfTmp1;
//		delete[] pfTmp2;
//	}
//}

/*
DIF:EMA(CLOSE,SHORT)-EMA(CLOSE,LONG);
DEA:EMA(DIF,MID);
MACD:(DIF-DEA)*2,COLORSTICK;
*/
typedef void(*MyEMA)(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc);

void testEma(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	HINSTANCE hDll; //DLL���

	TCHAR chCurDir[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, chCurDir);
	//SetCurrentDirectory(_T(".\\"));

	hDll = LoadLibrary((".\\T0002\\dlls\\TdxPluginFuncs.dll"));

	//SetCurrentDirectory(chCurDir);

	if (hDll != NULL)
	{
		MyEMA MyEMA_p = (MyEMA)GetProcAddress(hDll, "MyEMA");

		if (MyEMA_p != NULL)
		{
			MyEMA_p(DataLen, pfOUT, pfINa, pfINb, pfINc);
		}
		FreeLibrary(hDll);
	}
	else
	{
		TraceEx("Load DLL Error or DLL not exist!\n");
	}

	//if ((DataLen > 0) )
	//{
	//	int N = pfINb[0];
	//	if (N > DataLen) N = DataLen;
	//	pfOUT[0] = pfINa[0];

	//	for (int i = 1; i < DataLen; i++)
	//	{
	//		pfOUT[i]= (2 * pfINa[i] + (N - 1) * pfOUT[i - 1]) / (N + 1);	//EMA���㹫ʽ
	//	}
	//}

	//if ((DataLen > 0))
	//{
	//	pfDIF1 = new float[DataLen];
	//	for (int i = 0; i < DataLen; i++)
	//	{
	//		pfDIF1[i] = 0.0f;
	//	}

	//	if (pfINb[0] >= 1.0f)
	//	{
	//		testDea(DataLen, pfOUT, pfINa, pfINb[0], pfINc, FALSE); //�����������
	//	}
	//	else {
	//		for (int i = 1; i < 30; i++)
	//		{
	//			lastBuyDate = 0;
	//			lastBuyPrice = 0;
	//			curMoney = 100000;
	//			curStocks = 0;

	//			testDea(DataLen, pfOUT, pfINa, i, pfINc, TRUE);//�������ڲ��Խ��
	//		}
	//	}
	//	delete[] pfDIF1;
	//	pfDIF1 = NULL;
	//}
}

void OutputN(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	if ((DataLen > 0) )
	{
		for (int i = 0; i < DataLen; i++)
		{
			if (pfINa[i] != 0.0f && pfINb[i] != 0.0f && pfINc[i] != 0.0f)
			{
				TraceEx("\r\n[TDX]=- \t%.3f\t%.3f\t%.3f", pfINa[i], pfINb[i], pfINc[i]);
			}
		}
	}
}

void OutputP(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	if ((DataLen > 0) )
	{
		for (int i = 0; i < DataLen; i++)
		{
			if (pfINa[i]!=0.0f && pfINb[i] != 0.0f && pfINc[i] != 0.0f )
			{
				TraceEx("\r\n[TDX]=+ \t%.3f\t%.3f\t%.3f", pfINa[i], pfINb[i], pfINc[i]);
			}
		}
	}
}

void returnMaxLost(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	if ((DataLen > 0) && (fMaxLost > 0))
	{
		//Log ���س� fMaxLost
		TCHAR bbb[256] = {0};
		TraceEx("\r\n[TDX]��(%.0f_%.0f_%.0f),%2.2f��%2.2f, ��(%.3f��%.3f), ��:%.3f, ӯ\t%.3f\t��|��-%.2f��|��%.2f��, %d��|%dӯ|��%.2f��", pfINa[0], pfINb[0], pfINc[0], 190000.0+(fFirstDay/100), 190000.0+(fEndDay/100), fStartPrice, fEndPrice, fTotalAsset, fWinRate,fMaxLost, 100*fWinRate/fMaxLost, iSaleCount,iWinCount,(100.0*iWinCount/iSaleCount));
		//OutputDebugString(bbb);
	}

	//�������س� fMaxLost
	for(int i=0;i<DataLen;i++)
	{
		pfOUT[i]=fMaxLost;
	}
}

void returnWinRate(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	//������������ fTotalAsset
	//OutputDebugString("TestPlugin3");
	for(int i=0;i<DataLen;i++)
	{
		pfOUT[i]=fWinRate;
	}
}


void WaitCorrectActiveWnd() 
{
	//��ҵ��ָ(399006) 2017��01��20�� ������ PageUp/Down:ǰ���� �ո��:���� ͨ����(R)
	char   chText[MAX_PATH];//�������洰�ڵ�Text   
	ZeroMemory(chText,MAX_PATH); // ZeroMemory����0�����һ���ڴ����� 
	while(!((strstr(chText, "399006") > 0) &&
		(strstr(chText, "PageUp/Down:ǰ����") > 0) &&
		(strstr(chText, "�ո��") > 0)))
	{
		Sleep(100);
		GetWindowText(::GetActiveWindow(),chText,MAX_PATH);//��ȡ���ڵı���
	}
}

#define SIZE 2

int m_iLastDate = 0;

int convertDateToFile( int curDate ) 
{
	int Date = curDate+19000000;
	int Y = Date/10000;
	int M = (Date % 10000)/100;
	int D = (Date % 100);
	int DateToFile = (Y-2004)*0x800+M*100+D;
	return DateToFile;
}

void GetminFilefullPath(TCHAR * pBuf) 
{
	GetModuleFileName(NULL,pBuf,MAX_PATH);
	TCHAR * tmp = pBuf;
	while (strstr(tmp, "\\"))
	{
		tmp = strstr(tmp, "\\")+1;
	}
	tmp[0] = '\0';
	//TraceEx(pBuf);
	strcat(pBuf, "vipdoc\\sz\\minline\\sz399006.lc1");
}

int readLastDateInFile() 
{
	FILE* pFile;  
	int DateInFile = 0;
	//���ļ�
	//pFile = fopen("N:\\Xmoney\\new_tdx\\vipdoc\\sz\\minline\\sz399006.lc1" , "rb"); // ���ļ�  
	TCHAR  pBuf[MAX_PATH];
	GetminFilefullPath(pBuf);
	pFile = fopen(pBuf, "rb"); // ���ļ�  
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int iFsize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		if(iFsize>=32)
		{
			//��ȡ���һ������
			fseek(pFile,iFsize-32,SEEK_SET);//��fpָ���˻ص����ļ���β32�ֽڴ���
			if(fread(&DateInFile,2,1,pFile)==1)
			{//��ȡ�ɹ�
			}

		}
		fclose(pFile); // �ر��ļ�  
	}
	return DateInFile;
}

void dataExport1Minute(int DataLen,float* pfOUT,float* currPrice,float* curDate,float* currTime)
{
//Todo:��ǰ��ҳ���߼������ж�
	if (DataLen > 0)
	{
		//ת�����ݸ�ʽ
		int DateToFile = convertDateToFile((int)(curDate[0]));
		int DateInFile = 0;
		TraceEx("[TDX]( DateToFile=%X curDate[0]=%X", DateToFile, (int)(curDate[0]));
		if (m_iLastDate == 0)
		{
			DateInFile = readLastDateInFile();

			if (DateInFile >= DateToFile)
			{	
				//�ϴ�����������ڵ�ǰ���ݵĵ�һ��
				//���ҳ
// 				TraceEx("[TDX](��ҳaaa DateInFile=%X, DateToFile=%X",DateInFile, DateToFile);
// 				WaitCorrectActiveWnd();
// 				::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
// 				::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
				return;
			}		
		}
		else if (m_iLastDate >= DateToFile)
		{
			//�ϴ�����������ڵ�ǰ���ݵĵ�һ��
			//���ҳ
// 			TraceEx("[TDX](��ҳbbb m_iLastDate=%X, DateToFile=%X",m_iLastDate, DateToFile);
// 			WaitCorrectActiveWnd();
// 			::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
// 			::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
			return;
		}
	}
// 	//

	FILE* pFile;  
	TCHAR  pBuf[MAX_PATH];
	GetminFilefullPath(pBuf);

	pFile = fopen(pBuf , "ab+"); // ���ļ�׷��д����  
	if (pFile)
	{
		int outDate = 0;
		fseek(pFile, 0, SEEK_END);
		for(int i=0;i<DataLen;i++)
		{
			TraceEx("[TDX]( ��������A�� %d, %d, %f, %x\r\n", (int)(curDate[i]+19000000), (int)(currTime[i]), currPrice[i], *(int *)&currPrice[i]);
			/*
			(int)(curDate[i]+19000000)	20170105	int
			(int)currTime[i]			93200	int
			currPrice[i]				1988.6200	float
			*/
			//ת�����ݸ�ʽ
			int Date = (int)(curDate[i]+19000000);
			int Y = Date/10000;
			int M = (Date % 10000)/100;
			int D = (Date % 100);
			outDate = (Y-2004)*0x800+M*100+D;
			int hm = (int)(currTime[i])/100;
			int h = hm/100;
			int m = hm % 100;
			int outTime = h*60+m;
			int outZero = 0;
			int outD = outTime * 0x10000 + outDate;
			int xPrice = *(int *)&currPrice[i];
			/*		 outDate outTime
			00000000h: FE 5B 3F 02	8F 32 13 45		A4 A0 14 45		8F 32 13 45 
			00000010h: 00 EC 13 45	4A F2 0F 50		CB 02 C2 05		28 01 6D 00 

			��2355.159912109375
			��2378.0400390625
			��2355.159912109375
			��2366.75
			��9660082176.0
			��96600779
			��7143720
			*/
			//׷��д��1����TDX�ѻ��ļ���
			TraceEx("%d",fwrite(&outD , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
		}
		fclose(pFile); // �ر��ļ�  
		if (DataLen > 0)
		{
			m_iLastDate = outDate;

			//WaitCorrectActiveWnd();
			::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
			::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
		}
	}
}
//��ʱֹͣд�˴��룬����TDX�ĵ�������Ϊcsv��������ָͬ��һ�𵼳�
//ֻ��3�������������ҪTDX��ε��ú��������д���ļ�
//"2013-06-05 16:34:18",1.562,0.5,0.6,0.7,0.71,0.81,0.82,0.9,0.91
//void dataExport2CSV1(int DataLen,float* pfOUT,float* o,float* h,float* l)
//{
// 	for(int i=0;i<DataLen;i++)
// 	{
// 		TraceEx("[TDX]( ��ʱ����B�� %.3f, %.3f, %.3f\r\n", o, h, l);
// 	}
//}

void SmoothBSCalc(int DataLen,float* pfOUT,float* currPrice,float* fAction,float* currDay)
{

	//�����λ���ƺ�Ľ��
/*
Dll�м���Ŀǰ��λ��
fAction>0 ��ʾ�����źż���λ�ٷֱ�  0.088015������λ0.88�������λ0.15��
fAction<0 ��ʾ�����źż���λ�ٷֱ� -0.088015������λ0.88��������λ0.15��
fAction=0 ��ʾ���ź�

*/
	float fMoney = 0;
	float fStockSellable = 0;
	float fStockLocked = 0;
	float fNewDay = 0;
	fStartPrice = 0;
	fEndPrice = 0;

	float fHighWin = 0;
	fTotalAsset = 0;
	fMaxLost = 0;
	fWinRate = 0;
	iSaleCount = 0;
	iWinCount = 0;
	fFirstDay = 0;
	fEndDay = 0;

	if (DataLen>0)
	{
		fTotalAsset = currPrice[0];//��ʼ�ʲ�=��һ������̼�
		fStartPrice = currPrice[0];
		fEndPrice = currPrice[DataLen-1];
		fFirstDay = currDay[0];
		fEndDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			if (fNewDay != currDay[i])
			{//��һ�죬T+1������
				fNewDay = currDay[i];
				fStockSellable += fStockLocked;
				fStockLocked = 0;
			}

			fTotalAsset = fMoney + (currPrice[i] * (fStockSellable + fStockLocked));
			pfOUT[i] = fTotalAsset;

			if (fAction[i] > 0)//�����ź�
			{
				if ((fMoney == 0) && (fStockSellable == 0) && (fStockLocked == 0)) 
				{//��һ�������źţ���ʼ�ʽ���Ϊ��ǰ�۸�
					fMoney = currPrice[i];
					fTotalAsset = fMoney;
				}
				float fMaxPercent = float(int(fAction[i]*1000))/100;//����λ�ٷֱ�

				float fBuyPercent = (fAction[i]*1000);//���β�λ�ٷֱ�
				fBuyPercent = (float)((int)(fBuyPercent*1000)%1000)/100;//���β�λ�ٷֱ�


				float fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
				TraceEx("\r\n[TDX]( ��λ=%.2f��B [%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fBuyPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				if ((fMaxPercent - fCurPercent) >= 0.01)
				{
					float fNeedMoney = 0;
					if ((fMaxPercent - fCurPercent) >= fBuyPercent)
					{
						//�����λ�ٷֱ�fBuyPercent
						fNeedMoney = (fBuyPercent * fTotalAsset);
					} 
					else//��λ�����ﵽ�������ֵ
					{
						//�����λ�ٷֱ�(fMaxPercent - fCurPercent)
						fNeedMoney = ((fMaxPercent - fCurPercent) * fTotalAsset);
					}
					if (fMoney >= fNeedMoney)
					{//��Ǯ��
						fStockLocked += (fNeedMoney/currPrice[i]);
						fMoney -= fNeedMoney;
					} 
					else if (fMoney > 0)
					{//��Ǯ����������
						fStockLocked += fMoney / currPrice[i];
						fMoney = 0;
					}//��Ǯ=0
				} 
				fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
				TraceEx("\r\n[TDX]( ��λ=%.2f B��[%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fBuyPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
			}
			else if (fAction[i] < 0)//�����ź�
			{
				if (fTotalAsset > 0)
				{
					float fMaxPercent = float(int(fAction[i]*(-1)*1000))/100;//����λ�ٷֱ�

					float fSellPercent = (fAction[i]*(-1)*1000);//���β�λ�ٷֱ�
					fSellPercent = (float)((int)(fSellPercent*1000)%1000)/100;//���β�λ�ٷֱ�

					float fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
					TraceEx("\r\n[TDX]( ��λ=%.2f��S [%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fSellPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);

					//������λ�ٷֱ�fSellPercent
					float fNeedStock = (fSellPercent * fTotalAsset)/currPrice[i];
					if (fStockSellable >= fNeedStock)
					{//����Ʊ��
						fStockSellable -= fNeedStock;
						fMoney += fNeedStock * currPrice[i] * 0.9995/*�۳� ETF���� ������2.5��������*/;;

						iSaleCount++;
					} 
					else if (fStockSellable > 0)
					{//����Ʊ����������
						fMoney += fStockSellable * currPrice[i] * 0.9995/*�۳� ETF���� ������2.5��������*/;;
						fStockSellable = 0;
						iSaleCount++;
					}//����Ʊ=0

					fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
					TraceEx("\r\n[TDX]( ��λ=%.2f S��[%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fSellPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				} 
				else
				{
					TraceEx("\r\n[TDX]( ��λ=0 [S][%.0f��| ��=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", currDay[i], currPrice[i], fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				}
			}
			else
			{
				//TraceEx("\r\n[TDX]( ��λ����[ ��=%f| Ǯ=%f| ����Ʊ=%f | ������=%f \r\n", fTotalAsset, fMoney, fStockSellable, fStockLocked);
				//�������س� fMaxLost
				if (pfOUT[i] > fHighWin)
				{
					fHighWin = pfOUT[i];
				}
				else
				{
					float ftempLost = (fHighWin - pfOUT[i])/ fHighWin * 100;
					if (ftempLost > fMaxLost)
					{
						fMaxLost = ftempLost;
					}
				}
				//�������س�����

			}
		}
		fWinRate = (fTotalAsset - currPrice[0]) / currPrice[0] * 100; //����������

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
const int MAX_BUF_LEN = 4096;
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
	//while (1)
	{
		string msg = "ping://.?A";
		if (sProjectName.length() > 0)
		{
			msg = "ping://" + sProjectName + ".?A";
		}
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
			char buff[MAX_BUF_LEN] = "";
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

			if (sProjectName.length() == 0)
			{
				//sProjectName = PROJ_NAME;
			}
			//if (strstr(buff, "pong://") && strstr(buff, Common::toLower(sProjectName).c_str()))
			//{
			//	printf("=====Recv: %s\n", buff);

			//	if (!strstr(buff, "&Server="))
			//	{
			//		strcat(buff, "&Server=");
			//		strcat(buff, NetCommon::UrlEncode(sock_ntop((SOCKADDR*)&addrto, 0)).c_str());

			//		closesocket(sock);
			//		//WSACleanup();
			//		return buff;
			//	}
			//}
			if (strstr(buff, "ping"))
			{
				printf("++++++++++Recv: %s\n", buff);
			}
			if (strstr(buff, "QMDnc.exe") == buff)
			{
				static char str[128];
				struct sockaddr_in *sin = (struct sockaddr_in *)((SOCKADDR*)&addrto);
				if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
					return "";
				else
					return str;
			}

		}
		//Sleep(200);
	}
	closesocket(sock);
	//WSACleanup();
	return "";
}
void Thread_UDP_PingPong(LPVOID Para)
{
	while (true)
	{
		TraceEx("TDX Thread_UDP_PingPong");
		//GoListenUDP();
		Sleep(200);
	}
	return;
}


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

//const int MAX_BUF_LEN = 4096;
////char        buf[MAX_BUF_LEN];
//char *sock_ntop(const struct sockaddr *sa, socklen_t salen);

//void GoListenUDP()
//{
//	char        buf[MAX_BUF_LEN];
//	//Common::g_sPong;
//	WSADATA wsd;
//	//SOCKET  s;
//	int     nRet;
//
//	// ��ʼ���׽��ֶ�̬��  
//	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
//	{
//		MyTraceA("WSAStartup failed !/n");
//		return;
//	}
//
//	//MyTraceA("\r\n+++++Thread_UDP_PingPong:%d\n", 888);
//	// �����׽���  
//	//s = socket(AF_INET, SOCK_DGRAM, 0);
//	//if (s == INVALID_SOCKET)
//	//{
//	//	MyTraceA("socket() failed, Error Code:%d/n", WSAGetLastError());
//	//	WSACleanup();
//	//	return;
//	//}
//	//MyTraceA("\r\n+++++Thread_UDP_PingPong: socket %d\n", 888);
//
//	SOCKET      sockLocal = socket(AF_INET, SOCK_DGRAM, 0);
//	if (sockLocal == INVALID_SOCKET)
//	{
//		MyTraceA("socket() failed, Error Code:%d/n", WSAGetLastError());
//		WSACleanup();
//		return;
//	}
//
//	/*
//	Windows UDP socket recvfrom����10054����Ľ���취
//	����
//	��Windows 7��ϵͳ�ϣ�Aʹ��UDP socket������sendto������һ��Ŀ���ַB�������ݣ�����Ŀ���ַBû�н������ݣ����A��ʱ��������recvfrom��ͼ����Ŀ���ַB���ص����ݵĻ���recvfrom����������-1��WSAGetLastError()����10045��
//
//	ԭ��
//	����������Windows socket��һ��bug����UDP Socket��ĳ�η��ͺ��յ�һ�����ɵ����ICMP��ʱ�������������һ�������з��أ�����������׽�������һ�εĽ����з�����SOCKET_ERROR,������10045��
//
//	����취��
//	ʹ��WSAIoctl����UDP socket�Ĺ���ģʽ���������������󡣾����������£�*/
//#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
//
//	BOOL bNewBehavior = FALSE;
//	DWORD dwBytesReturned = 0;
//	WSAIoctl(sockLocal, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);
//	/*
//	SIO_UDP_CONNRESTѡ�Controls whether UDP PORT_UNREACHABLE messages are reported. Set to TRUE to enable reporting. Set to FALSE to disable reporting.
//
//	��ע��
//	setsockopt���޸��׽ӿڵ����ԣ�ֻ�Ǹ��׽ӿ��ڹ����Ĺ�������Ҫ�õ���һЩ������
//	WSAIoctl�����޸��׽ӿڵĹ���ģʽ������Ķ���������׽ӿ�Ҫ����������ʽ���й������б��ʵ�����
//	*/
//
//	SOCKADDR_IN addrSrv;
//	SOCKADDR_IN addrClient;
//	int         len = sizeof(SOCKADDR);
//
//	// ���÷�������ַ  
//	addrSrv.sin_family = AF_INET;
//	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//	addrSrv.sin_port = htons(PORT_ASSIST);
//
//	int on = 0; //�Ѵ��׽���listenFd����Ϊ�������ַ���ã�on=0,���on=1�������������ˣ�debug��ʱ����
//	::setsockopt(sockLocal, SOL_SOCKET, SO_REUSEADDR,
//		(char*)&on, sizeof(on));
//
//	BOOL  bDontLinger = FALSE;
//	::setsockopt(sockLocal, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));
//
//	// ���׽���  
//	nRet = ::bind(sockLocal, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
//	if (SOCKET_ERROR == nRet)
//	{
//		MyTraceA("bind failed !/n");
//		closesocket(sockLocal);
//		WSACleanup();
//		return;
//	}
//	while (1)
//	{
//		//MyTraceA("\r\n+++++Start Recv From Client:%s\n", buf);
//		//Sleep(10000);
//		ZeroMemory(buf, MAX_BUF_LEN);
//		//MyTraceA("\r\n=====Start Recv From Client:%s\n", buf);
//		// �ӿͻ��˽�������  
//		nRet = recvfrom(sockLocal, buf, MAX_BUF_LEN, 0, (SOCKADDR*)&addrClient, &len);
//		//MyTraceA("<<Start Recv From Client: nRet = %d\n", nRet);
//		if (SOCKET_ERROR == nRet)
//		{
//			MyTraceA("recvfrom  [%s] failed ! WSAGetLastError=%d\n", inet_ntoa(addrClient.sin_addr), WSAGetLastError());
//			//closesocket(sockLocal);
//			//WSACleanup();
//			//return;
//			continue;
//		}
//		// ��ӡ���Կͻ��˷�����������  
//		MyTraceA("<<<Recv From [%s] data: %s", inet_ntoa(addrClient.sin_addr), buf);
//		string ssss = buf;
//		if (ssss.find("ping://") == string::npos)
//		{
//			MyTraceA(">>>\n");
//			continue;
//		}
//		ssss = ssss.substr(ssss.find("ping://") + 7);
//		ssss = ssss.substr(0, ssss.find("."));
//		MyTraceA("�յ�Ping: %s ���ԣ�%s ������>>>\n", ssss.c_str(), inet_ntoa(addrClient.sin_addr));
//
//		/*
//		pong://Server.PubwinNetcafePlatform.5.1018.408.172525/terminal?packet=V2.0.444
//
//		HomePage=http%3a%2f%2f111.231.106.196%3a38080%2fportal%2fdo%2fv3%2fmember%2fshowGoods%3fmerchantId%3d3108644708
//		Servers=portal-test.88plat.com%2cwww.88plat.com
//		Widget=web%2fwidget.003.html
//		Domain=portal-test.88plat.com%2cwww.88plat.com
//		Shortcut=%7c%25DeskTop%25%5c%e5%85%85%e5%80%bc%7c
//		Shopname=%e5%90%88%e8%82%a5%e5%b8%82%e5%a4%a7%e5%a4%a7%e5%a4%a7%e6%95%b0%e6%8d%ae%e7%bd%91%e5%90%a7
//		onButton=
//		btnList=%7c%e5%85%85%e5%80%bc%7c%e7%82%b9%e5%8d%95%7c%e5%91%bc%e5%8f%ab%e7%bd%91%e7%ae%a1%7c%e7%95%99%e8%a8%80%e6%9d%bf%7c%e8%a7%86%e9%a2%91%e7%9b%b4%e6%92%ad%7c%e7%82%b9%e6%ad%8c%7c
//		ShortcutNew=%7c%e7%bd%91%e8%b4%b9%2b%e5%85%85%e5%80%bc_F3%7c
//
//		Netcafe=3108644708
//		Platform=http%3a%2f%2f111.231.106.196%3a38080%2fportal%2fdo%2f
//		bs=BillingSystem
//		client=192.168.1.58:52471
//		ticks=1551687314636.51
//		Server=192.168.1.150%3A44380
//		*/
//
//		string ssPong = "pong://Server.PubwinNetcafePlatform."STR_PRODUCTVER"/";
//		ssPong += Common::toLower(ssss);
//		ssPong += "?packet=";
//		ssPong += mapConfig["packet"];
//
//		for (map<string, string>::iterator i = mapConfig.begin(); i != mapConfig.end(); i++)
//		{
//			if (i->first.find(ssss + ".") == 0)
//			{
//				ssPong += "&";
//				ssPong += i->first.substr((ssss + ".").length());
//				ssPong += "=";
//
//				MyTraceA((i->first + "=").c_str());
//				MyTraceA(i->second.c_str());
//				MyTraceA(NetCommon::UrlUTF8Decode(NetCommon::UTF8_URL_ENCODE((char*)i->second.c_str())).c_str());
//
//				//if (NetCommon::UrlUTF8Decode(NetCommon::UTF8_URL_ENCODE((char*)i->second.c_str())) == i->second)
//				if (NetCommon::UrlUTF8Decode(NetCommon::UrlEncode(i->second)) != i->second)
//				{
//					ssPong += NetCommon::UTF8_URL_ENCODE((char*)i->second.c_str());
//				}
//				else
//				{
//					ssPong += NetCommon::UrlEncode(i->second);
//				}
//			}
//		}
//		ssPong += "&Netcafe=";
//		ssPong += NetCommon::UrlEncode(mapConfig["netcafe"]);
//
//		ssPong += "&Platform=";
//		ssPong += NetCommon::UrlEncode(mapConfig["platform"]);
//
//		//ssPong += "&Server=";
//		////ssPong += NetCommon::UrlEncode(sock_ntop((SOCKADDR*)&addrSrv, 0));
//		//ssPong += NetCommon::UrlEncode(GetLocalIP() + ":" + to_string(PORT_ASSIST));
//
//		// ��ͻ��˷�������  
//		//strcpy(buf, "UDP Hello World ! [Server]\r\n");
//		//sendto(sockLocal, buf, strlen(buf), 0, (SOCKADDR*)&addrClient, len);
//		::sendto(sockLocal, ssPong.c_str(), ssPong.length(), 0, (SOCKADDR*)&addrClient, len);
//		MyTraceA("����pong...............to [%s]........................Done!\n", inet_ntoa(addrClient.sin_addr));
//	}
//
//	closesocket(sockLocal);
//	WSACleanup();
//	return;
//}
//

int TCP_RcvFile(int uPort, std::string sFName)
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
		return -1;
	}
	else {
		printf("WSAStartup() called successful!\n");
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		//������������İ汾��2.2,����ֹWinSock���ʹ��
		WSACleanup();
		return -1;
	}
	//�������ڼ������׽���
	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if (sockServer == INVALID_SOCKET) {
		printf("socket() called failed!, error code is: %d", WSAGetLastError());
		return -1;
	}
	else {
		printf("socket() called successful!\n");
	}
	//�����������׽��ֽṹ
	SOCKADDR_IN addrServer;
	//�������ֽ�˳��ת��ΪTCP/IP�����ֽ�˳��
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(uPort);
	//���׽��ְ󶨵�һ�����ص�ַ�Ͷ˿���
	err = ::bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (err == SOCKET_ERROR) {
		printf("bind() called failed! The error code is: %d\n", WSAGetLastError());
		return -1;
	}
	else {
		printf("bind() called successful\n");
	}
	//���׽�������Ϊ����ģʽ,׼�����տͻ�������
	err = listen(sockServer, 5);
	if (err == SOCKET_ERROR) {
		printf("listen() called failed! The error code is: %d\n", WSAGetLastError());
		return -1;
	}
	else {
		printf("listen() called successful!\n");
	}
	//���淢���������ӵĿͻ��˵��׽�����Ϣ
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	/*bool isStart = true;
	while (isStart)*/
	{
		string sOK = UDP_BC_Ping(uPort);

		if (sOK.length() == 0)
		{
			WSACleanup();
			return 0;
		}
		//�ȴ��ͻ���������
		TraceEx("LoaderX==> ����˴�һֱ�ȴ����ӣ����鱾������ǽ�� �ͻ���ip��%s", sOK.c_str());
		SOCKET sockConn = accept(sockServer, (SOCKADDR*)&addrClient, &len);
		if (sockConn == INVALID_SOCKET) {
			printf("accept() called failed! The error code is: %d\n", WSAGetLastError());
		}
		else {
			printf("The server receive a new client connection!\n");
		}

		TraceEx("LoaderX==> %s ���ԣ�%s ������", sFName.c_str(), inet_ntoa(addrClient.sin_addr));

		//char sendBuf[100];
		//sprintf_s(sendBuf, 100, "Welcome %s", inet_ntoa(addrClient.sin_addr));
		//��������
		//send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		//char recvBuf[100];
		//recv(sockConn, recvBuf, 100, 0);
		//��ӡ���յ�������
		//printf("receive data from client side [%s, %d] is: %s\n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port, recvBuf);
		FILE *fd = fopen(sFName.c_str(), "wb+");
		int RecvSize = 4096;
		int iTotalSize = 0;
		char recvBuf[4096];

		while (fd && (RecvSize > 0))
		{
			//��������
			RecvSize = recv(sockConn, recvBuf, RecvSize, 0);
			if (SOCKET_ERROR == RecvSize)
				break;;
			iTotalSize += RecvSize;

			if (fd) {
				fwrite(recvBuf, 1, RecvSize, fd);
			}
		}

		if (fd) {
			fclose(fd);
		}
		//�ر������׽���
		closesocket(sockConn);
	}
	WSACleanup();
	return 0;
}

HANDLE hThread_TCP2Trader = INVALID_HANDLE_VALUE;

void TCP2Trader(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	TraceEx("TCP2Trader");

	if (INVALID_HANDLE_VALUE == hThread_TCP2Trader)
	{
		hThread_TCP2Trader = (HANDLE)_beginthread(Thread_UDP_PingPong, 0, NULL);
	}

	//new thread of BC 40845
	//	:ping
	//	rcv pong
	//	get IP
	//	TCP to IP : 40845
	//	waitevent, send B / S
	//	waitevent, send B / S
	//	...
	//	if Broken!
	//		goto : ping
	//		loop : ping

}

void PostMsgOf_BS(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc) //���룬����������
{
/*	for(int i=0;i<DataLen;i++)
	{
		TCHAR bbb[256] = {0};
		sprintf(bbb, "\r\npfINa[%d]=%.0f,pfINb[%d]=%.0f,", i, pfINa[i], i, pfINb[i]);
		OutputDebugString(bbb);
	}
*/
	if (DataLen > 0) 
	{
		int iTrade=0;
		TCHAR aaa[512] = {0};
		TCHAR bbb[512] = {0};
		::GetWindowText(GetMainWindow(), bbb, 512);
		if(pfINa[DataLen-1] > 0)
		{
			iTrade=1;
			strcat(bbb, "  ָ�++++++++����++++++++");
		}
		else if(pfINb[DataLen-1] > 0)
		{
			iTrade=2;
			strcat(bbb, "  ָ�-------����------");
		}
		else 
		{
			iTrade=0;
			strcat(bbb, "  ָ�**********�޲���**********");
		}

		if (pfINc[DataLen-1] == 1)//{ֻ����5�������ڵ������źţ�PERIOD=1 }
		{
			PostMsgToTradeWnd(iTrade);
			sprintf(aaa, "\r\n[TDX]ָ��,(%.0f_%.0f_���ڣ�%.0f), %s,", pfINa[DataLen-1], pfINb[DataLen-1], pfINc[DataLen-1], bbb);
			OutputDebugString(aaa);
		}

	}
}
void BuyOne(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	OutputDebugString("BuyOne");
}

void Test99(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	OutputDebugString("Test99");
}

void QueryDB(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	if (bConnectMysql == FALSE)
	{
		bConnectMysql = ConnectMysql("127.0.0.1", "root", "admin777", "stock", 3306);
	}

	//OutputDebugString("QueryDB");
	/*for (int i = 0; i<DataLen; i++)
	{
		ShowTable(pfINa[i]/100.00, pfINb[i]/100.00, (int(pfINc[i]))/100.00, (pfINc[i]-(int(pfINc[i])))*100.00); 
	}*/
	if (DataLen > 0)
	{
		//int i = DataLen - 1;
		for (int i = 0; i<DataLen; i++)
		{
			if (i == DataLen - 1)
			{
				bLastData = TRUE;
			}
			ShowTable(&(pfOUT[i]), pfINa[i]/100.00, pfINb[i]/100.00, (round(pfINc[i]) )/100.00, (pfINc[i]-(round(pfINc[i])))*100.00); 
			bLastData = FALSE;

		}
	}	
	//mysql_close(&mysql);//���д��OnDestroy()������

}
//���صĺ���
PluginTCalcFuncInfo g_CalcFuncSets[] = 
{
	// 
	//{0,NULL},
	{1,(pPluginFUNC)&CalcWin},
	{2,(pPluginFUNC)&returnMaxLost},
	{3,(pPluginFUNC)&returnWinRate},
	{4,(pPluginFUNC)&SmoothBSCalc},
	//{5,(pPluginFUNC)&dataExport1Minute},
	{5,(pPluginFUNC)&getReal_BS},//���� ȫ����/ȫ��㣬���T+1��ʵ�ʵ���Ч�����㣬���ظ�TDX��ʽ
	{6,(pPluginFUNC)&QueryDB},

	{ 7,(pPluginFUNC)&TCP2Trader },//����ָ��ͨ��//
	{ 8,(pPluginFUNC)&PostMsgOf_BS },//�����̨��������ָ��//(���룬����������)
	{9,(pPluginFUNC)&testEma},//�µĶ�̬dll���ã�����ÿ���޸�Ҫ����TDX

	{ 10,(pPluginFUNC)&Test99 },
	{ 11,(pPluginFUNC)&Test99 },
	{ 12,(pPluginFUNC)&Test99 },
	{ 13,(pPluginFUNC)&Test99 },
	{ 88,(pPluginFUNC)&OutputN }, //Log������������3����ֵ or 3��0.0f
	{ 89,(pPluginFUNC)&OutputP }, //Log�������ӯ����3����ֵ or 3��0.0f
	{0,NULL}
};

//������TCalc��ע�ắ��
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if(*pFun==NULL)
	{
		//ConnectMysql("127.0.0.1","root","admin777","stock",3306);
		(*pFun)=g_CalcFuncSets;

		/*
		new thread of BC 40845
		:ping
		rcv pong
			get IP
			TCP to IP:40845
				waitevent, send B/S
				waitevent, send B/S
				...
				if Broken!
					goto :ping
		loop :ping
		*/

		return TRUE;
	}
	return FALSE;
}
