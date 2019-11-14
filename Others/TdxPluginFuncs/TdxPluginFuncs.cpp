// TdxPluginFuncs.cpp : 定义 DLL 应用程序的导出函数。
//

#include <windows.h>
#include "TdxPluginFuncs.h"

float* pfDIF1 = NULL;
float lastBuyDate = 0;
float lastBuyPrice = 0;
float curMoney = 100000;
float curStocks = 0;

void testDea(int DataLen, float* pfOUT, float* pfINa, int iMultiple, float* pfDate, BOOL bTestCalc)
{
	int iShort = 10 * iMultiple;
	int iLong = 12 * iMultiple;
	int iMid = 9;
	TraceEx("\r\n[TDX]======testDea \tiMultiple=%d 测算=%d\tDataLen=%d\t", iMultiple, bTestCalc, DataLen);

	if ((DataLen > 0))
	{
		if (iShort > DataLen) iShort = DataLen;
		if (iMid > DataLen) iMid = DataLen;
		if (iLong > DataLen) iLong = DataLen;

		float* pfTmp1 = new float[DataLen];
		float* pfTmp2 = new float[DataLen];
		pfTmp1[0] = pfINa[0];
		pfTmp2[0] = pfINa[0];

		pfOUT[0] = 0;
		if ((iMultiple == 1) || !bTestCalc)
		{
			pfDIF1[0] = 0;
		}

		int iBS = 0;
		for (int i = 1; i < DataLen; i++)
		{
			pfTmp1[i] = (2 * pfINa[i] + (iShort - 1) * pfTmp1[i - 1]) / (iShort + 1);	//EMA计算公式
			pfTmp2[i] = (2 * pfINa[i] + (iLong - 1) * pfTmp2[i - 1]) / (iLong + 1);		//EMA计算公式
			if ((iMultiple == 1) /*|| !bTestCalc*/)
			{
				pfDIF1[i] = pfTmp1[i] - pfTmp2[i];
			}
			pfOUT[i] = (2 * (pfTmp1[i] - pfTmp2[i]) + (iMid - 1) * pfOUT[i - 1]) / (iMid + 1);	//EMA计算公式

																								//TraceEx("\r\n[TDX]======testDea \tiMultiple=%d 测算=%d\t%.3f\t%.3f\t%.3f\t%.3f", iMultiple, bTestCalc, pfOUT[i], pfDIF1[i], pfTmp1[i], pfTmp2[i]);
			if (bTestCalc)
			{
				//////////////////////////////////////Calc Win
				if ((pfDIF1[i - 1] < pfOUT[i - 1]) && (pfDIF1[i] > pfOUT[i])) //CrossUp
				{
					if ((lastBuyDate < pfDate[i]) && (curMoney > 0))
					{
						curStocks = curMoney / pfINa[i];
						lastBuyDate = pfDate[i];
						lastBuyPrice = pfINa[i];
						curMoney = 0;
						//TraceEx("\r\n[TDX]======B \t%d\t可%.3f\t总%.3f\t%d\tdif=%.3f\tout=%.3f", iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000, pfDIF1[i], pfOUT[i]);
					}

				}
				else if ((pfDIF1[i - 1] > pfOUT[i - 1]) && (pfDIF1[i] < pfOUT[i])) //CrossDown
				{
					if ((lastBuyDate < pfDate[i]) && (curStocks > 0))
					{
						iBS++;
						curMoney = curStocks * pfINa[i];
						lastBuyDate = 0;
						lastBuyPrice = 0;
						curStocks = 0;
						//						TraceEx("\r\n[TDX]======S \t%d\t可%.3f\t总%.3f\t%d", iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000);
						//TraceEx("\r\n[TDX]===SS \t%d\t可%.3f\t总%.3f\t%d\tdif=%.3f\tout=%.3f", iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000, pfDIF1[i], pfOUT[i]);
					}
				}

				if (i == DataLen - 1)
				{
					TraceEx("\r\n[TDX]===盈[%.3f%%] 次[%d] \t%d\t可%.3f\t总%.3f\t%d\tdif=%.3f\tout=%.3f", (curMoney + (curStocks * pfINa[i]) - 100000) / 1000, iBS, iMultiple, curMoney, curMoney + (curStocks * pfINa[i]), (int)pfDate[i] + 19000000, pfDIF1[i], pfOUT[i]);
				}
				//////////////////////////////////////Calc Win DONE.
			}
		}
		TraceEx("\r\n[TDX]=====End= \tiMultiple=%d\t%.3f", iMultiple, curMoney + (curStocks * pfINa[DataLen - 1]));

		delete[] pfTmp1;
		delete[] pfTmp2;
	}
}

CDLL_ void MyEMA(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc) {
	if ((DataLen > 0))
	{
		pfDIF1 = new float[DataLen];
		for (int i = 0; i < DataLen; i++)
		{
			pfDIF1[i] = 0.0f;
		}

		if (pfINb[0] >= 1.0f)
		{
			testDea(DataLen, pfOUT, pfINa, pfINb[0], pfINc, FALSE); //数据正常输出
		}
		else {
			for (int i = 1; i < 30; i++)
			{
				lastBuyDate = 0;
				lastBuyPrice = 0;
				curMoney = 100000;
				curStocks = 0;

				testDea(DataLen, pfOUT, pfINa, i, pfINc, TRUE);//数据用于测试结果
			}
		}
		delete[] pfDIF1;
		pfDIF1 = NULL;
	}
}
