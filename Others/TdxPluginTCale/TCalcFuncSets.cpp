#include "stdafx.h"
#include "TCalcFuncSets.h"
#define WM_TRADEMSG WM_USER+1
#include <stdio.h>  
#include<math.h>
#include <stdarg.h>  
#include<string>
#include<sstream>
//#include "winsock.h"
#include "mysql.h"//头文件顺序不能颠倒
MYSQL mysql; //数据库连接句柄
BOOL bConnectMysql = FALSE;

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
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;  
	char *strBuffer = new char[nLen];  
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);  
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
	//TCHAR		szBuffer[] = TEXT("CPU温度超高...>89°C");

	//获取桌面窗口句柄
	hwnd = GetDesktopWindow();
	//获取桌面窗口DC
	hdc = GetWindowDC(hwnd);
	//背景色透明
	SetBkMode(hdc, TRANSPARENT);

	font=CreateFont(
		120,48,0,0, FW_BOLD,0,0,0, ANSI_CHARSET ,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,NULL,(LPCTSTR)"微软雅黑");
	SelectObject(hdc,font);

	//设置颜色并输出文字
	clr = SetTextColor(hdc, newClr);
	TextOut(hdc, x, y, szBuffer, strlen(szBuffer));

	//还原颜色
	SetTextColor(hdc, clr);

	//释放句柄DC
	DeleteObject(font); 
	ReleaseDC(hwnd, hdc);
}


//1、连接mysql
int ConnectMysql(LPCTSTR sip,LPCTSTR suser,LPCTSTR spass,LPCTSTR sdbname,int sport)
{
	if( mysql_init(&mysql) == NULL )
	{
		::OutputDebugString("初始化mysql失败!");
		return FALSE;
	}
	//实例mysql_real_connect(&mysql,"127.0.0.1","root","","zkeco_db",17770,NULL,0)
	if(!mysql_real_connect(&mysql,(const char *)sip,(const char *)suser,(const char *)spass,(const char *)sdbname,sport,NULL,0))
	{
		//TRACE("连接mysql失败:%s",mysql_error(&mysql)); 
		return FALSE;
	}
	else
	{
		//Trace2Logger("数据库连接成功");
		return TRUE;
	}
}

BOOL bLastData = FALSE;
void ShowTable(float* fOut, float a1,float a2,float a3,float a4 )
{
	//显示数据
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
		//一次性取得数据集
		result=mysql_store_result(&mysql);

		//取得并打印行数
		LONG64 rowcount=mysql_num_rows(result);
		/*loginfo.Format("row count=%d",rowcount);
		MessageBox(loginfo);       */

		//取得并打印各字段的名称
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
		//打印各行
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
						loginfo=loginfo+"盈占比：";

						*fOut = (atof(row[4]))/((atof(row[4]) + atof(row[6])));
						TCHAR tmp[1024] = {0};
						sprintf(tmp, "%d％",(int)round((*fOut) * 100));

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

//生成的dll及相关依赖dll请拷贝到通达信安装目录的T0002/dlls/下面,再在公式管理器进行绑定
float fMaxLost = 0;
float fWinRate = 0;
float fTotalAsset = 0;
float fStartPrice = 0;
float fEndPrice = 0;
int iSaleCount = 0;
int iWinCount = 0;
float fFirstDay = 0;;
float fLastDay = 0;;

void PostMsgToTradeWnd(LPARAM lParam)
{
	TCHAR sTradeWndTitle[256] = "自动化交易控制中心";
	HWND hTradeWnd = NULL;
	hTradeWnd = ::FindWindow(NULL, sTradeWndTitle);
	if( hTradeWnd != NULL) 
	{ 
		::PostMessage(hTradeWnd,WM_TRADEMSG,0,lParam); 
	}  
}
//第一版本，T+1 bug无法解决，改用下面第二版本
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
// 		fTotalAsset = closePrice[0];//初始资产=第一天的收盘价
// 		fStartPrice = closePrice[0];
// 
// 		for(int i=0;i<DataLen;i++)
// 		{
// 			pfOUT[i]=fTotalAsset;//输出的总资产
// 			if (bBuy)//持股中
// 			{
// 				pfOUT[i] = closePrice[i] * fStockCounts;
// 
// 				//计算最大回撤 fMaxLost
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
// 				//计算最大回撤结束
// 			}
// 			if(buyPrice[i] > 0)//买入
// 			{
// 				if (!bBuy)
// 				{
// 					bBuy = TRUE;
// 
// 					fStockCounts = fTotalAsset / buyPrice[i];
// 				}
// 			}
// 			if(salePrice[i] > 0)//卖出
// 			{
// 				if (bBuy)
// 				{
// 					bBuy = FALSE;
// 					fLastSale = salePrice[i];
// 					float fCurrMoney = 0;
// 					fCurrMoney = salePrice[i] * fStockCounts * 0.9995/*扣除 ETF买卖 各【万2.5】手续费*/;
// 					if (fCurrMoney > fTotalAsset)
// 					{
// 						iWinCount++;
// 					}
// 					fTotalAsset = fCurrMoney;
// 					iSaleCount++;
// 
// // 					TCHAR bbb[256] = {0};
// // 					sprintf(bbb, "买入,%.0f,卖出,%.0f,获利,%.0f,小计,%.0f,总资产,%.0f,", fBuy, fLastSale, fWin, fTotalAsset, fTotalAsset+fFirstBuy);
// // 					OutputDebugString(bbb);
// 				}
// 			}
// 
// 		}
// 
// 		fWinRate = (fTotalAsset - closePrice[0]) / closePrice[0] * 100; //总体收益率
// 
// 	}
// }

//第二版本
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
	fLastDay = 0;

	if (DataLen>0)
	{
		TraceEx("\r\n[TDX]==>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
		//fTotalAsset = currPrice[0];//初始资产=第一天的收盘价
		//fStartPrice = currPrice[0];
		fEndPrice = currPrice[DataLen-1];
		fFirstDay = currDay[0];
		fLastDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			pfOUT[i]=fTotalAsset;//输出的总资产
			if (bBought)//持股中
			{
				pfOUT[i] = currPrice[i] * fStockCounts;

				//计算最大回撤 fMaxLost
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
				//计算最大回撤结束

				if ((fAction[i] == 0.0))
				{
					if ((fBuyDay != currDay[i]))
					{
						if (bLastSale == TRUE)
						{
							bLastSale = FALSE;
							fAction[i] = 2.0;//持股中，日期变化，第一K卖出
						}
					}
				}
			}
			if(fAction[i] == 1.0)//买入
			{
				if (!bBought)
				{
					bBought = TRUE;
					fBuyDay = currDay[i];

					if (fStartPrice == 0.0)
					{
						fTotalAsset = currPrice[i];//初始资产=第一BUY的收盘价
						fStartPrice = currPrice[i];

						for (int xx = 0; xx <= i; xx++)
						{
							pfOUT[xx] = fTotalAsset;//补齐第一买之前的总资产
						}
					}

					fStockCounts = fTotalAsset / currPrice[i];
//#ifdef _DEBUG
					TCHAR bbb[256] = { 0 };
					sprintf(bbb, ">>买入day,%.0f,价格,%.3f,总资产,%.3f,", fBuyDay-1000000, currPrice[i], fTotalAsset);
					OutputDebugString(bbb);
//#endif
				}
				bLastSale = FALSE;
			}
			else if((fAction[i] == 2.0))
			{
				if ((fBuyDay != currDay[i]))//卖出
				{
					if (bBought)
					{
						bBought = FALSE;
						//fLastSale = fAction[i];
						float fCurrMoney = 0;
						fCurrMoney = currPrice[i] * fStockCounts * 0.9995/*扣除 ETF买卖 各【万2.5】手续费*/;
						if (fCurrMoney > fTotalAsset)
						{
							iWinCount++;
						}
						fTotalAsset = fCurrMoney;
						iSaleCount++;

	//#ifdef _DEBUG
						TCHAR bbb[256] = { 0 };
						sprintf(bbb, "====卖出day,%.0f,价格,%.3f,总资产,%.3f,[%d]次", currDay[i]-1000000, currPrice[i], fTotalAsset, iSaleCount);
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
			fWinRate = (fTotalAsset - fStartPrice) / fStartPrice * 100; //总体收益率
		}

	}
}

void getReal_BS(int DataLen,float* pfOUT,float* currDay,float* currTime,float* fAction)
{
	//实际买卖:TDXDLL2(5,DATE,TIME,IF(全买点,1,IF(全卖点,2,0)));

	BOOL bBought = FALSE;
	BOOL bNextDaySale = FALSE;
	//float fFirstBuy = 0;
	//float fLastSale = 0;
	//float fBuy = 0;
	//float fWin = 0;
	float fBuyDay = 0; 
	fFirstDay = 0;
	fLastDay = 0;

	if (DataLen>0)
	{
		TraceEx("\r\n[TDX]==>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
		fFirstDay = currDay[0];
		fLastDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			pfOUT[i]=0;//输出

			if (currTime[i] >= 1457) //收盘前集合竞价时间
			{
				if (fAction[i] > 0.0f)//有动作
				{
					if ((i+1)<DataLen)//有下一数据
					{
						if (fAction[i+1] == 0.0f)//下一数据无动作
						{
							fAction[i+1] = fAction[i];//当前动作移入下一数据
							fAction[i] = 0.0f;//当前动作清除
						}
					}
				}

				continue;
			}

			if (bBought)//持股中
			{
				if ((fAction[i] == 0.0f))
				{
					if ((fBuyDay != currDay[i]))
					{
						if (bNextDaySale == TRUE)
						{
							bNextDaySale = FALSE;
							fAction[i] = 2.0f;//持股中，已有卖出信号，日期变化，第一K卖出
						}
					}
				}
			}

			if(fAction[i] == 1.0f)//买入
			{
				if (!bBought)
				{
					bBought = TRUE;
					fBuyDay = currDay[i];
					pfOUT[i] = 1.0f;//输出
				}
				bNextDaySale = FALSE;
			}
			else if((fAction[i] == 2.0f))
			{
				if ((fBuyDay != currDay[i]))//卖出
				{
					if (bBought)
					{
						bBought = FALSE;
						pfOUT[i] = 2.0f;//输出
					}
				}			
				else
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

/*
DIF:EMA(CLOSE,SHORT)-EMA(CLOSE,LONG);
DEA:EMA(DIF,MID);
MACD:(DIF-DEA)*2,COLORSTICK;
*/

void testEma(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	if ((DataLen > 0) )
	{
		int N = pfINb[0];
		if (N > DataLen) N = DataLen;
		pfOUT[0] = pfINa[0];

		for (int i = 1; i < DataLen; i++)
		{
			pfOUT[i]= (2 * pfINa[i] + (N - 1) * pfOUT[i - 1]) / (N + 1);	//EMA计算公式
			//if (pfINa[i] != 0.0f && pfINb[i] != 0.0f && pfINc[i] != 0.0f)
			//{
			//	TraceEx("\r\n[TDX]=- \t%.3f\t%.3f\t%.3f", pfINa[i], pfINb[i], pfINc[i]);
			//}
		}
	}
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
		//Log 最大回撤 fMaxLost
		TCHAR bbb[256] = {0};
		TraceEx("\r\n[TDX]总(%.0f_%.0f_%.0f),%2.2f～%2.2f, 价(%.3f～%.3f), 资:%.3f, 盈\t%.3f\t％|撤-%.2f％|比%.2f％, %d卖|%d盈|比%.2f％", pfINa[0], pfINb[0], pfINc[0], 190000.0+(fFirstDay/100), 190000.0+(fLastDay/100), fStartPrice, fEndPrice, fTotalAsset, fWinRate,fMaxLost, 100*fWinRate/fMaxLost, iSaleCount,iWinCount,(100.0*iWinCount/iSaleCount));
		//OutputDebugString(bbb);
	}

	//返回最大回撤 fMaxLost
	for(int i=0;i<DataLen;i++)
	{
		pfOUT[i]=fMaxLost;
	}
}

void returnWinRate(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	//返回总收益率 fTotalAsset
	//OutputDebugString("TestPlugin3");
	for(int i=0;i<DataLen;i++)
	{
		pfOUT[i]=fWinRate;
	}
}


void WaitCorrectActiveWnd() 
{
	//创业板指(399006) 2017年01月20日 星期五 PageUp/Down:前后日 空格键:操作 通达信(R)
	char   chText[MAX_PATH];//用来储存窗口的Text   
	ZeroMemory(chText,MAX_PATH); // ZeroMemory宏用0来填充一块内存区域 
	while(!((strstr(chText, "399006") > 0) &&
		(strstr(chText, "PageUp/Down:前后日") > 0) &&
		(strstr(chText, "空格键") > 0)))
	{
		Sleep(100);
		GetWindowText(::GetActiveWindow(),chText,MAX_PATH);//获取窗口的标题
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
	//打开文件
	//pFile = fopen("N:\\Xmoney\\new_tdx\\vipdoc\\sz\\minline\\sz399006.lc1" , "rb"); // 打开文件  
	TCHAR  pBuf[MAX_PATH];
	GetminFilefullPath(pBuf);
	pFile = fopen(pBuf, "rb"); // 打开文件  
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int iFsize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		if(iFsize>=32)
		{
			//读取最后一天日期
			fseek(pFile,iFsize-32,SEEK_SET);//把fp指针退回到离文件结尾32字节处。
			if(fread(&DateInFile,2,1,pFile)==1)
			{//读取成功
			}

		}
		fclose(pFile); // 关闭文件  
	}
	return DateInFile;
}

void dataExport1Minute(int DataLen,float* pfOUT,float* currPrice,float* curDate,float* currTime)
{
//Todo:向前翻页，逻辑不好判断
	if (DataLen > 0)
	{
		//转换数据格式
		int DateToFile = convertDateToFile((int)(curDate[0]));
		int DateInFile = 0;
		TraceEx("[TDX]( DateToFile=%X curDate[0]=%X", DateToFile, (int)(curDate[0]));
		if (m_iLastDate == 0)
		{
			DateInFile = readLastDateInFile();

			if (DateInFile >= DateToFile)
			{	
				//上次最后日期晚于当前数据的第一天
				//向后翻页
// 				TraceEx("[TDX](翻页aaa DateInFile=%X, DateToFile=%X",DateInFile, DateToFile);
// 				WaitCorrectActiveWnd();
// 				::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
// 				::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
				return;
			}		
		}
		else if (m_iLastDate >= DateToFile)
		{
			//上次最后日期晚于当前数据的第一天
			//向后翻页
// 			TraceEx("[TDX](翻页bbb m_iLastDate=%X, DateToFile=%X",m_iLastDate, DateToFile);
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

	pFile = fopen(pBuf , "ab+"); // 打开文件追加写操作  
	if (pFile)
	{
		int outDate = 0;
		fseek(pFile, 0, SEEK_END);
		for(int i=0;i<DataLen;i++)
		{
			TraceEx("[TDX]( 分钟数据A： %d, %d, %f, %x\r\n", (int)(curDate[i]+19000000), (int)(currTime[i]), currPrice[i], *(int *)&currPrice[i]);
			/*
			(int)(curDate[i]+19000000)	20170105	int
			(int)currTime[i]			93200	int
			currPrice[i]				1988.6200	float
			*/
			//转换数据格式
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

			开2355.159912109375
			高2378.0400390625
			低2355.159912109375
			收2366.75
			额9660082176.0
			股96600779
			？7143720
			*/
			//追加写入1分钟TDX脱机文件中
			TraceEx("%d",fwrite(&outD , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
		}
		fclose(pFile); // 关闭文件  
		if (DataLen > 0)
		{
			m_iLastDate = outDate;

			//WaitCorrectActiveWnd();
			::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
			::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
		}
	}
}
//暂时停止写此代码，发现TDX的导出数据为csv，可以连同指标一起导出
//只能3个参数，因此需要TDX多次调用函数，最后写入文件
//"2013-06-05 16:34:18",1.562,0.5,0.6,0.7,0.71,0.81,0.82,0.9,0.91
//void dataExport2CSV1(int DataLen,float* pfOUT,float* o,float* h,float* l)
//{
// 	for(int i=0;i<DataLen;i++)
// 	{
// 		TraceEx("[TDX]( 分时数据B： %.3f, %.3f, %.3f\r\n", o, h, l);
// 	}
//}

void SmoothBSCalc(int DataLen,float* pfOUT,float* currPrice,float* fAction,float* currDay)
{

	//计算仓位控制后的结果
/*
Dll中计算目前仓位，
fAction>0 表示买入信号及仓位百分比  0.088015（最大仓位0.88本次买仓位0.15）
fAction<0 表示卖出信号及仓位百分比 -0.088015（最大仓位0.88本次卖仓位0.15）
fAction=0 表示无信号

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
	fLastDay = 0;

	if (DataLen>0)
	{
		fTotalAsset = currPrice[0];//初始资产=第一天的收盘价
		fStartPrice = currPrice[0];
		fEndPrice = currPrice[DataLen-1];
		fFirstDay = currDay[0];
		fLastDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			if (fNewDay != currDay[i])
			{//新一天，T+1可卖了
				fNewDay = currDay[i];
				fStockSellable += fStockLocked;
				fStockLocked = 0;
			}

			fTotalAsset = fMoney + (currPrice[i] * (fStockSellable + fStockLocked));
			pfOUT[i] = fTotalAsset;

			if (fAction[i] > 0)//买入信号
			{
				if ((fMoney == 0) && (fStockSellable == 0) && (fStockLocked == 0)) 
				{//第一个买入信号，初始资金设为当前价格
					fMoney = currPrice[i];
					fTotalAsset = fMoney;
				}
				float fMaxPercent = float(int(fAction[i]*1000))/100;//最大仓位百分比

				float fBuyPercent = (fAction[i]*1000);//本次仓位百分比
				fBuyPercent = (float)((int)(fBuyPercent*1000)%1000)/100;//本次仓位百分比


				float fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
				TraceEx("\r\n[TDX]( 仓位=%.2f《B [%.0f日| 价=%.2f| 限仓=%.2f| 加仓=%.2f| 资产=%.2f| 钱=%.2f| T0票=%.2f | T1票=%.2f  数据=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fBuyPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				if ((fMaxPercent - fCurPercent) >= 0.01)
				{
					float fNeedMoney = 0;
					if ((fMaxPercent - fCurPercent) >= fBuyPercent)
					{
						//买入仓位百分比fBuyPercent
						fNeedMoney = (fBuyPercent * fTotalAsset);
					} 
					else//仓位即将达到最大限制值
					{
						//买入仓位百分比(fMaxPercent - fCurPercent)
						fNeedMoney = ((fMaxPercent - fCurPercent) * fTotalAsset);
					}
					if (fMoney >= fNeedMoney)
					{//余钱够
						fStockLocked += (fNeedMoney/currPrice[i]);
						fMoney -= fNeedMoney;
					} 
					else if (fMoney > 0)
					{//余钱不够但还有
						fStockLocked += fMoney / currPrice[i];
						fMoney = 0;
					}//余钱=0
				} 
				fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
				TraceEx("\r\n[TDX]( 仓位=%.2f B》[%.0f日| 价=%.2f| 限仓=%.2f| 加仓=%.2f| 资产=%.2f| 钱=%.2f| T0票=%.2f | T1票=%.2f  数据=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fBuyPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
			}
			else if (fAction[i] < 0)//卖出信号
			{
				if (fTotalAsset > 0)
				{
					float fMaxPercent = float(int(fAction[i]*(-1)*1000))/100;//最大仓位百分比

					float fSellPercent = (fAction[i]*(-1)*1000);//本次仓位百分比
					fSellPercent = (float)((int)(fSellPercent*1000)%1000)/100;//本次仓位百分比

					float fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
					TraceEx("\r\n[TDX]( 仓位=%.2f【S [%.0f日| 价=%.2f| 限仓=%.2f| 加仓=%.2f| 资产=%.2f| 钱=%.2f| T0票=%.2f | T1票=%.2f  数据=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fSellPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);

					//卖出仓位百分比fSellPercent
					float fNeedStock = (fSellPercent * fTotalAsset)/currPrice[i];
					if (fStockSellable >= fNeedStock)
					{//可卖票够
						fStockSellable -= fNeedStock;
						fMoney += fNeedStock * currPrice[i] * 0.9995/*扣除 ETF买卖 各【万2.5】手续费*/;;

						iSaleCount++;
					} 
					else if (fStockSellable > 0)
					{//可卖票不够但还有
						fMoney += fStockSellable * currPrice[i] * 0.9995/*扣除 ETF买卖 各【万2.5】手续费*/;;
						fStockSellable = 0;
						iSaleCount++;
					}//可卖票=0

					fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
					TraceEx("\r\n[TDX]( 仓位=%.2f S】[%.0f日| 价=%.2f| 限仓=%.2f| 加仓=%.2f| 资产=%.2f| 钱=%.2f| T0票=%.2f | T1票=%.2f  数据=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fSellPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				} 
				else
				{
					TraceEx("\r\n[TDX]( 仓位=0 [S][%.0f日| 价=%.2f| 资产=%.2f| 钱=%.2f| T0票=%.2f | T1票=%.2f  数据=%d ]***\r\n", currDay[i], currPrice[i], fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				}
			}
			else
			{
				//TraceEx("\r\n[TDX]( 仓位控制[ 总=%f| 钱=%f| 可卖票=%f | 不可卖=%f \r\n", fTotalAsset, fMoney, fStockSellable, fStockLocked);
				//计算最大回撤 fMaxLost
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
				//计算最大回撤结束

			}
		}
		fWinRate = (fTotalAsset - currPrice[0]) / currPrice[0] * 100; //总体收益率

	}
}

void PostMsgOf_BS(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc) //买入，卖出，周期
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
			strcat(bbb, "  指令：++++++++买入++++++++");
		}
		else if(pfINb[DataLen-1] > 0)
		{
			iTrade=2;
			strcat(bbb, "  指令：-------卖出------");
		}
		else 
		{
			iTrade=0;
			strcat(bbb, "  指令：**********无操作**********");
		}

		if (pfINc[DataLen-1] == 1)//{只发出5分钟周期的买卖信号：PERIOD=1 }
		{
			PostMsgToTradeWnd(iTrade);
			sprintf(aaa, "\r\n[TDX]指令,(%.0f_%.0f_周期：%.0f), %s,", pfINa[DataLen-1], pfINb[DataLen-1], pfINc[DataLen-1], bbb);
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
	//mysql_close(&mysql);//最好写到OnDestroy()函数中

}
//加载的函数
PluginTCalcFuncInfo g_CalcFuncSets[] = 
{
	// 
	//{0,NULL},
	{1,(pPluginFUNC)&CalcWin},
	{2,(pPluginFUNC)&returnMaxLost},
	{3,(pPluginFUNC)&returnWinRate},
	{4,(pPluginFUNC)&SmoothBSCalc},
	//{5,(pPluginFUNC)&dataExport1Minute},
	{5,(pPluginFUNC)&getReal_BS},//根据 全卖点/全买点，算出T+1下实际的有效买卖点，返回给TDX公式
	{6,(pPluginFUNC)&QueryDB},

	{8,(pPluginFUNC)&PostMsgOf_BS},//向控制台发出买卖指令//(买入，卖出，周期)
	{9,(pPluginFUNC)&testEma},

	{ 10,(pPluginFUNC)&Test99 },
	{ 11,(pPluginFUNC)&Test99 },
	{ 12,(pPluginFUNC)&Test99 },
	{ 13,(pPluginFUNC)&Test99 },
	{ 88,(pPluginFUNC)&OutputN }, //Log输出卖出亏损的3个数值 or 3个0.0f
	{ 89,(pPluginFUNC)&OutputP }, //Log输出卖出盈利的3个数值 or 3个0.0f
	{0,NULL}
};

//导出给TCalc的注册函数
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if(*pFun==NULL)
	{
		//ConnectMysql("127.0.0.1","root","admin777","stock",3306);
		(*pFun)=g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}
