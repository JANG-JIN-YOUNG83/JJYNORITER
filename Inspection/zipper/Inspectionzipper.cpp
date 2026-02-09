#include "stdafx.h"

#include "Inspectionzipper.h"
#include "Gadget/TimeCheck.h"
#include "../DebugInfo.h"
#include <algorithm>
#include <Base/basedef.h>
#include "Algorithm/ImageProcessing.h"



Inspectionzipper::Inspectionzipper()
	: m_nSweepAngle(90)
	, m_nSweepCount(10)
	, m_nSweepLength(10)
	, m_fAverageFilter(5.5)
	, m_fScale_X_um(0.0f)
	, m_fScale_Y_um(0.0f)
	, m_nSpecLength_mm(10)
	, m_nInsulationWidth(0)
	,m_fSetZipper2InspectTime(5.0f)
	, m_bUseZipper2InspectionMiddleArea(true)
{

}


Inspectionzipper::~Inspectionzipper()
{

}


void Inspectionzipper::initData()
{
	m_rtTabSearch[0] = IPVM::Rect(0, 0, 0, 0);
	m_rtTabSearch[1] = IPVM::Rect(0, 0, 0, 0);
	m_rtStartPointThresholdGetArea = IPVM::Rect(99999, 99999, 0, 0);

	m_byTabThreshold[0] = 0;
	m_byTabThreshold[1] = 0;

	m_vecSweepLine.clear();
	m_vecSweepLineCandidate.clear();
	m_vecStartPoint.clear();
	m_vecStartPointCandidate.clear();
	m_vecFindWidth.clear();
	m_vecFindWidthLine.clear();
	m_vecMinMaxCha2.clear();
	m_vecMinMaxCha2Point.clear();
	m_vecLength.clear();
	m_vecLengthStartPoint.clear();
	m_vecOutOfTab.clear();
	/*m_vecCandidateLength.clear();*/

	m_ZipperResult.init();

}


void Inspectionzipper::SetParameter(long nSweepAngle, long nSweepCount, long nSweepLength, float fStartPointThresholdOffset, float fTabAreaThresholdOffset, float fAverageFilter, float fScale_X_um, float fScale_Y_um, long nSpecLength_mm, long nInsulationWidth, CRect rtInsulation/*절연오버레이검사에서 찾은 절연 roi*/, bool bUseROIfoundFromInsulationOverlayInspection, float fSetZipper2InspectTime, bool bUseZipper2InspectionMiddleArea, float fInsulationStartPointThreshold, float fInsulationfTabAreaThreshold,bool bUseZipper2InsulationInspect,bool bUseZipper2AutoInsulationWidthPixel, long nDebugTitle)
{
	m_nSweepAngle =		nSweepAngle;
	m_nSweepCount =		nSweepCount;
	m_nSweepLength =	nSweepLength;
	m_fStartpointThresholdOffset = fStartPointThresholdOffset;
	m_fTabAreaThresholdOffset = fTabAreaThresholdOffset;
	m_fAverageFilter =	fAverageFilter;
	m_fScale_X_um =		fScale_X_um;
	m_fScale_Y_um =		fScale_Y_um;
	m_nSpecLength_mm =	nSpecLength_mm;
	m_nInsulationWidth = nInsulationWidth;
	m_fSetZipper2InspectTime = fSetZipper2InspectTime;
	m_bUseZipper2InspectionMiddleArea = bUseZipper2InspectionMiddleArea;
	m_fInsulationStartPointThreshold = fInsulationStartPointThreshold;
	m_fInsulationfTabAreaThreshold = fInsulationfTabAreaThreshold;
	m_bUseZipper2InsulationInspect = bUseZipper2InsulationInspect;
	m_bUseZipper2AutoInsulationWidthPixel = bUseZipper2AutoInsulationWidthPixel;
	m_bUseROIfoundFromInsulationOverlayInspection = bUseROIfoundFromInsulationOverlayInspection;
	m_nDebug_title =	nDebugTitle;
	m_rtFoundFromInsulationOverlayInspection = rtInsulation;
}

BYTE Inspectionzipper::GetThreshold(const IPVM::Image_8u_C1 src, IPVM::Rect tab, int nOffset, int nXOffset, int nYOffset, float fPosition, Inspection::DebugInfo* debugInfo)
{
	if (tab.m_top + nYOffset > tab.m_bottom)
		tab.m_bottom = tab.m_top + nYOffset + 1;

	if (tab.m_left + nXOffset > tab.m_right)
		tab.m_right = tab.m_left + nXOffset + 1;

	std::vector<BYTE> vecTabintensity;
	for (int y = tab.m_top + nYOffset; y < tab.m_bottom; y = y + nYOffset)
	{
		for (int x = tab.m_left + nXOffset; x < tab.m_right; x = x + nXOffset)
		{
			if(0 < y && src.GetSizeY() > y && 0 < x && src.GetSizeX() > x)
				vecTabintensity.push_back(src.GetMem()[y * src.GetSizeX() + x]);
		}
	}

	if (0 == vecTabintensity.size())
		return 0;

	std::sort(vecTabintensity.begin(), vecTabintensity.end());


	//IPVM::ImageProcessing::GetThreshold_Otsu(src, const Image_8u_C1 & mask, const Rect & roi, unsigned char& byLowMean, unsigned char& byHighMean, unsigned char& byThreshold);

	//IPVM::imageprocess
	BYTE byThVal = vecTabintensity[vecTabintensity.size() * fPosition] + nOffset;

	return byThVal;	//50%정도에 있는 값을 탭 밝기로 쓴다.  
}


bool Inspectionzipper::Doinspection(IPVM::Image_8u_C1 src, IPVM::Rect tab, bool m_bIstest, bool bUseInsulation, Inspection::DebugInfo* debugInfo)
{
	IPVM::TimeCheck tc;
	initData();

	ZipperResult result_InsulationArea;
	ZipperResult result_NonInsulationArea;

	ZipperResult result_InsulationArea_candidate;
	ZipperResult result_NonInsulationArea_candidate;
	ZipperResult Result_Total;

	if (true == m_bUseZipper2InsulationInspect && true == bUseInsulation)
	{
		IPVM::Rect rtInsulationArea;
		IPVM::Rect rtNonInsulationArea;

		FindDetialInsulation(src, tab, rtInsulationArea, rtNonInsulationArea);

		m_rtTabSearch[0] = rtInsulationArea;
		m_rtTabSearch[1] = rtNonInsulationArea;

		m_byTabThreshold[0] =	GetThreshold(src, rtInsulationArea, m_fInsulationfTabAreaThreshold);//tabareathreshold
		inspection(src, rtInsulationArea, m_bIstest, m_byTabThreshold[0], m_fInsulationStartPointThreshold, m_fInsulationfTabAreaThreshold, result_InsulationArea, result_InsulationArea_candidate, &tc);
		
		m_byTabThreshold[1] = GetThreshold(src, rtNonInsulationArea, m_fTabAreaThresholdOffset);//tabareathreshold
		inspection(src, rtNonInsulationArea, m_bIstest, m_byTabThreshold[1], m_fStartpointThresholdOffset, m_fTabAreaThresholdOffset, result_NonInsulationArea, result_NonInsulationArea_candidate, &tc);

		if (0 == result_InsulationArea.GetResultCount() + result_NonInsulationArea.GetResultCount()  && 0 < result_InsulationArea_candidate.GetResultCount() && 0 < result_NonInsulationArea_candidate.GetResultCount())
		{
			CombineResult(&result_InsulationArea_candidate, &result_NonInsulationArea_candidate, m_ZipperResult);
		}
		else
		{
			m_ZipperResult += result_InsulationArea;
			m_ZipperResult += result_NonInsulationArea;
		}
	}
	else
	{
		ZipperResult result_candidate;
		m_byTabThreshold[1] = GetThreshold(src, tab, m_fTabAreaThresholdOffset);//tabareathreshold
		inspection(src, tab, m_bIstest, m_byTabThreshold[1], m_fStartpointThresholdOffset, m_fTabAreaThresholdOffset, m_ZipperResult, result_candidate, & tc);
		m_rtTabSearch[1] = tab;
	}


	return true;
}
bool Inspectionzipper::inspection(IPVM::Image_8u_C1 src, IPVM::Rect tab, bool m_bIstest, long nTabThreshold, long nStartPointThresholdOffset, long nTabAreaThresholdOffset, ZipperResult& result, ZipperResult& result_candidate, IPVM::TimeCheck *ptc, Inspection::DebugInfo* debugInfo)
{
	//위에서 아래로 아래에서 위로 2번 찾아보자

	IPVM::Rect rtThresholdGetArea = tab;

	BYTE threshold = nTabThreshold;
	
	float fSpanTime = 0.0f;
	

	BOOL bFindUD[2] = { false, false };
	for (int y = 0; y < 40; y = y + 2)
	{
		IPVM::Rect roi = tab;

		//ud : up & down
		for (int ud = 0; ud < 2; ud++)
		{
			if (true == bFindUD[ud])
				break;

				roi.m_top = roi.m_bottom = tab.m_top + y;
				rtThresholdGetArea.m_top = rtThresholdGetArea.m_bottom = tab.m_top + y;
			if (1 == ud)
			{
					roi.m_bottom = roi.m_top = tab.m_bottom - y;
					rtThresholdGetArea.m_bottom = rtThresholdGetArea.m_top = tab.m_bottom - y;
			}

			std::vector<IPVM::Point_32f_C2> vecStartPoint;
			if (true == SearchStartPoint(src, roi, rtThresholdGetArea, nStartPointThresholdOffset, vecStartPoint))
			{
				for (int i = 0; i < vecStartPoint.size(); i++)
				{

					ZipperInfo  info = FindZipper(src, tab, ud, threshold, vecStartPoint[i], m_nSweepAngle, m_nSweepCount, m_nSweepLength, m_fAverageFilter, m_fScale_X_um, m_fScale_Y_um);
					if (info.GetTotalLength_mm() > m_nSpecLength_mm)
					{
						result.Set(info);

						IPVM::Point_32f_C2 StartPoint(0, 0);
						StartPoint.m_x = info.m_vecSweepInfo[i].m_lsFindLine.m_sx;
						StartPoint.m_y = info.m_vecSweepInfo[i].m_lsFindLine.m_sy;
						m_vecLengthStartPoint.push_back(StartPoint);


						for (int i = 0; i < info.m_vecSweepInfo.size(); i++)
						{
							m_vecSweepLine.push_back(info.m_vecSweepInfo[i].m_lsFindLine);
						}
						m_vecLength.push_back(info.GetTotalLength_mm());

						bFindUD[ud] = true;

						if(false == m_bIstest)
							return true;

						break;
					}
					else
					{
						if (1 < info.m_vecSweepInfo.size())
							result_candidate.Set(info);//	m_vecCandidateLength.push_back(info.GetTotalLength());
					}
					//재성. 후보  result를 따로 푸시백
				}
			}

		}

		if (true == bFindUD[0] && true == bFindUD[1])
			break;

		fSpanTime = ptc->Elapsed_ms();

		if (m_fSetZipper2InspectTime < fSpanTime && false == m_bIstest)	// 물어보기
			return true;
	}

	
	if (true == m_bUseZipper2InspectionMiddleArea)
	{
		//중간에서 찾아보기. 
		IPVM::Rect MiddleRoiUp = tab;
		IPVM::Rect MiddleRoiDown = tab;
		std::vector<IPVM::Point_32f_C2> vecMiddleStartPoint;

		MiddleRoiDown.m_top = MiddleRoiUp.m_bottom = (tab.m_top + tab.m_bottom) * 0.5;

		for (int y = 0; y < 10; y++)
		{
			fSpanTime = ptc->Elapsed_ms();
			if (m_fSetZipper2InspectTime < fSpanTime && false == m_bIstest)	// 5ms 이상 걸렸다면 계산하지 않는다. 
				return true;

			MiddleRoiDown.m_top = MiddleRoiDown.m_top + 2;

			MiddleRoiUp.m_bottom = MiddleRoiDown.m_top;//식 다시 확인하기

			IPVM::Rect rtSSP = MiddleRoiDown;
			rtSSP.m_bottom = rtSSP.m_top;

			if (true == SearchStartPoint(src, rtSSP, rtThresholdGetArea, nStartPointThresholdOffset, vecMiddleStartPoint))
			{
				for (int i = 0; i < vecMiddleStartPoint.size(); i++)
				{
					ZipperInfo result_Up;
					ZipperInfo result_Down;

					result_Up = FindZipper(src, MiddleRoiUp, 1 /*middle 2 up*/, threshold, vecMiddleStartPoint[i], m_nSweepAngle, m_nSweepCount, m_nSweepLength, m_fAverageFilter, m_fScale_X_um, m_fScale_Y_um);
					result_Down = FindZipper(src, MiddleRoiDown, 0/*middle 2 down*/, threshold, vecMiddleStartPoint[i], m_nSweepAngle, m_nSweepCount, m_nSweepLength, m_fAverageFilter, m_fScale_X_um, m_fScale_Y_um);

					//  결과 합치기
					ZipperInfo result = result_Up + result_Down;
					if (result.GetTotalLength_mm() > m_nSpecLength_mm*1.3)
					{
						m_ZipperResult.Set(result);

						IPVM::Point_32f_C2 StartPoint(0, 0);
						if (result.m_vecSweepInfo.size() > 1)
						{
							StartPoint.m_x = result.m_vecSweepInfo[1].m_lsFindLine.m_sx;
							StartPoint.m_y = result.m_vecSweepInfo[1].m_lsFindLine.m_sy;
						}

						m_vecLengthStartPoint.push_back(StartPoint);

						for (int i = 0; i < result.m_vecSweepInfo.size(); i++)
						{
							m_vecSweepLine.push_back(result.m_vecSweepInfo[i].m_lsFindLine);
						}
						m_vecLength.push_back(result.GetTotalLength_mm());

						return true;
					}
					else
					{
						if(1 < result.m_vecSweepInfo.size())
							result_candidate.Set(result);//	m_vecCandidateLength.push_back(result.GetTotalLength());
					}
						//재성. 후보  result를 따로 푸시백

				}
			}

		}
	}
	return true;
}





ZipperInfo Inspectionzipper::FindZipper(const IPVM::Image_8u_C1 src, IPVM::Rect tab, bool bDown2Up, long nTabThreshold, IPVM::Point_32f_C2 pFindPoint, long nSweepAngle, long nSweepCount, long nSweepLength, float fAverageFilter, float fscaleX, float fscaleY, Inspection::DebugInfo* debugInfo)
{
	BOOL bNG = false;
	float fscale_X_um = fscaleX;
	float fscale_Y_um = fscaleY;

	BOOL bResultFind = false;

	IPVM::Rect rtEndPoint = tab;
	if (false == bDown2Up)
	{
		rtEndPoint.m_bottom = rtEndPoint.m_bottom - 20;
	}
	else
	{
		rtEndPoint.m_top = rtEndPoint.m_top + 20;
	}


		float nTotalLength_mm = 0.0f;

		long nSweepLineCount = nSweepCount;
		long nSweepLineLength = nSweepLength;

		float fSweepTotalAngle = nSweepAngle * 0.0175f /*(3.1416f / 180)*/;
		float fSweepAngle = fSweepTotalAngle / (float)nSweepLineCount;


		float fIntensity_Max = 0.f;
		float fIntensity_Sum = 0.f;
		float fIntensity_Avg = 0.f;
		float fIntensity = 0.f;
		float fIntensity_Stdev = 0.f;

		std::vector<IPVM::Point_32f_C2> vecfptFindPoint;
		std::vector<float> vecfIntensity;
		std::vector<float> vecfIntensity_stdev;
		std::vector<float> vecfWireWidth;

		float fAngle = 0.0f;
		float fRad = (90.0f - (nSweepAngle * 0.5)) * 0.0175f;

		if (true == bDown2Up)
			fRad = (90.0f - (nSweepAngle * 0.5)) * 0.0175f + 3.1416f;

		float fCosValue = 0.0f, fSinValue = 0.0f;

		IPVM::Point_32f_C2 fptObjectInnerPos, fptObjectOuterPos;

		fptObjectInnerPos = pFindPoint;

		IPVM::LineSeg_32f Line_debug;

		std::vector<IPVM::LineSeg_32f> vecSweepLine;

		//std::vector<IPVM::Point_32f_C2> vecsfPoint;

		std::vector<sweepInfo> vecSweepInfo;

		std::vector<BYTE> vecbyLineValue;

		long nContinusCnt = 3;

		while (TRUE)
		{
			fIntensity_Sum = 0.f;
			fIntensity_Avg = 0.f;
			fIntensity_Max = 255;
			vecfIntensity.clear();
			vecfptFindPoint.clear();
			vecfIntensity_stdev.clear();

			Line_debug.m_sx = fptObjectInnerPos.m_x;
			Line_debug.m_sy = fptObjectInnerPos.m_y;
			
			sweepInfo swInfo;
			swInfo.m_lsFindLine.m_sx = fptObjectInnerPos.m_x;
			swInfo.m_lsFindLine.m_sy = fptObjectInnerPos.m_y;
			//vecsfPoint.push_back(fptObjectInnerPos);

			int nContinuseLineCnt = 0;
			bool bCheck = false;
			bool bExist = false;
			for (long nSection = 0; nSection < nSweepLineCount; nSection++)
			{
				if (nContinusCnt > nSection || nSweepLineCount - 1 == nSection)
					bCheck = true;
				else
					bCheck = false;

				fAngle = fRad + (fSweepAngle * nSection);

				fCosValue = (float)cos(fAngle);
				fSinValue = (float)sin(fAngle);

				fptObjectOuterPos.m_x = fptObjectInnerPos.m_x + nSweepLineLength * fCosValue;
				fptObjectOuterPos.m_y = fptObjectInnerPos.m_y + nSweepLineLength * fSinValue;

				Line_debug.m_ex = fptObjectOuterPos.m_x;
				Line_debug.m_ey = fptObjectOuterPos.m_y;

				m_vecSweepLineCandidate.push_back(Line_debug);


				long nLineValue = GetLineData((BYTE*)src.GetRawMem(), src.GetSizeX(), src.GetSizeY(), fptObjectInnerPos.m_x, fptObjectInnerPos.m_y, fptObjectOuterPos.m_x, fptObjectOuterPos.m_y, fCosValue, fSinValue, NULL);
				if (0 == nLineValue)
					return ZipperInfo();

				vecbyLineValue.clear();
				vecbyLineValue.resize(nLineValue);

				GetLineData((BYTE*)src.GetMem(), src.GetSizeX(), src.GetSizeY(), fptObjectInnerPos.m_x, fptObjectInnerPos.m_y, fptObjectOuterPos.m_x, fptObjectOuterPos.m_y, fCosValue, fSinValue, &vecbyLineValue[0]);

				if (true/*전극쪽에 들어갔다고 판단되면*/ == GetMeanStdev_BYTE(&vecbyLineValue[0], nLineValue, nTabThreshold, fIntensity, fIntensity_Stdev))
				{
					nContinuseLineCnt++;
					m_vecOutOfTab.push_back(fptObjectOuterPos);

					if (true == bCheck && nContinusCnt/*요거 변수로 빼기*/ <= nContinuseLineCnt)
					{
						//경계로 판단하고 검색 종료 
						bExist = true;
						break;
					}
				}
				else
				{
					nContinuseLineCnt = 0;
				}

				// 찾은 정보 저장
				vecfIntensity.push_back(fIntensity);
				vecfptFindPoint.push_back(fptObjectOuterPos);
				vecfIntensity_stdev.push_back(fIntensity_Stdev);
			}
			if (true == bExist)
				break;

			//데이터 필터링
			float fgvMin = 99999;
			float fgvMax = -99999;
			for (long nG = 0; nG < vecfIntensity.size(); nG++)
			{
				if (fgvMin > vecfIntensity[nG])
					fgvMin = vecfIntensity[nG];

				if (fgvMax < vecfIntensity[nG])
					fgvMax = vecfIntensity[nG];
			}

			m_vecMinMaxCha2.push_back(fgvMax - fgvMin);
			m_vecMinMaxCha2Point.push_back(fptObjectInnerPos);

			if ((fgvMax - fgvMin) < fAverageFilter)   // 밝기평균차이가 0.5이상 안나면 
			{
				// 검색 평균 그레이값이 3을 넘지 않으면 아무것도 없는것으로 판단하고 검색을 중지시켜버린다. 
				// 나중에 어떻게 할건지 보고 판단하자.
				break;
			}


			fIntensity_Avg = fIntensity_Sum / (float)nSweepLineCount;

			long nSeletedIdx = 0;
			for (long nSection = 0; nSection < vecfIntensity.size(); nSection++)
			{
				//fIntensity = fabs(vecfIntensity[nSection] - fIntensity_Avg);
				fIntensity = vecfIntensity[nSection];

				if (fIntensity < fIntensity_Max)
				{
					fIntensity_Max = fIntensity;
					fptObjectInnerPos = vecfptFindPoint[nSection];
					nSeletedIdx = nSection;
				}
			}

			float fWidth = 0.0f;
			FindWireWidthEdge(src, fptObjectInnerPos, fAngle, 35, fWidth);

			swInfo.m_lsFindLine.m_ex = fptObjectInnerPos.m_x;
			swInfo.m_lsFindLine.m_ey = fptObjectInnerPos.m_y;

			//vecsfPoint.push_back(fptObjectInnerPos);


			IPVM::LineSeg_32f sfLine = swInfo.m_lsFindLine;
			/*sfLine.m_sx = vecsfPoint[vecsfPoint.size() - 1].m_x;
			sfLine.m_sy = vecsfPoint[vecsfPoint.size() - 1].m_y;
			sfLine.m_ex = vecsfPoint[vecsfPoint.size() - 2].m_x;
			sfLine.m_ey = vecsfPoint[vecsfPoint.size() - 2].m_y;*/

			vecSweepLine.push_back(sfLine);

			// 포인트 정리좀하고
			CPoint cpTemp;
			if (sfLine.m_sx > sfLine.m_ex)
			{
				cpTemp.x = sfLine.m_sx;
				cpTemp.y = sfLine.m_sy;

				sfLine.m_sx = sfLine.m_ex;
				sfLine.m_sy = sfLine.m_ey;

				sfLine.m_ex = cpTemp.x;
				sfLine.m_ey = cpTemp.y;
			}

			float fx = (sfLine.m_sx - sfLine.m_ex) * fscale_X_um;
			float fy = (sfLine.m_sy - sfLine.m_ey) * fscale_Y_um;
			float length = sqrt(pow(fx, 2) + pow(fy, 2));
			
			swInfo.fLength_mm = length;

			vecSweepInfo.push_back(swInfo);

			nTotalLength_mm += length;

			//탭영역을 벗어나면 exit 
			if (false == tab.PtInRect(fptObjectInnerPos))
			{
				break;
			}

			if (false == bDown2Up)
			{
				if (rtEndPoint.m_bottom < fptObjectInnerPos.m_y)
				{
					break;
				}

			}
			else
			{
				if (rtEndPoint.m_top > fptObjectInnerPos.m_y)
				{
					break;
				}
			}
		}

	return ZipperInfo(vecSweepInfo);
}


bool Inspectionzipper::SearchStartPoint(const IPVM::Image_8u_C1 src, IPVM::Rect roi, IPVM::Rect rttabthresholdArea, long nThresholdOffset, std::vector<IPVM::Point_32f_C2> &vecFindPoint,Inspection::DebugInfo* debugInfo)
{
	IPVM::TimeCheck tc;

	BYTE threshold = GetThreshold(src, rttabthresholdArea, nThresholdOffset, 1, 1, 0.3);//startPoint Threshold

	if (m_rtStartPointThresholdGetArea.m_left > rttabthresholdArea.m_left)
		m_rtStartPointThresholdGetArea.m_left = rttabthresholdArea.m_left;

	if (m_rtStartPointThresholdGetArea.m_right < rttabthresholdArea.m_right)
		m_rtStartPointThresholdGetArea.m_right = rttabthresholdArea.m_right;

	if (m_rtStartPointThresholdGetArea.m_top > rttabthresholdArea.m_top)
		m_rtStartPointThresholdGetArea.m_top = rttabthresholdArea.m_top;

	if (m_rtStartPointThresholdGetArea.m_bottom < rttabthresholdArea.m_bottom)
		m_rtStartPointThresholdGetArea.m_bottom = rttabthresholdArea.m_bottom;


	float f1 = tc.Elapsed_ms();
	tc.Reset();

	std::vector<long> vecPoint;
	for (int x = roi.m_left; x < roi.m_right; x++)
	{
		if (src.GetMem()[roi.m_top * src.GetSizeX() + x] < threshold)
		{
			m_vecStartPointCandidate.push_back(IPVM::Point_32f_C2(x, roi.m_top));
			vecPoint.push_back(x);
		}
	}

	float f2 = tc.Elapsed_ms();
	tc.Reset();


	long nSpec = 5;
	for (int p = 0; p < vecPoint.size(); )
	{
		int S = p;
		int E = p;
		for (int i = S; i < vecPoint.size()-1; i++)
		{
			if ((vecPoint[i + 1] - vecPoint[i]) < nSpec)
			{
				E = i + 1;/*+1을 하는 이유는 숫자 카운트라서 1부터 카운트해야함*/
				continue;
			}
			break;
		}

		p = E + 1;

		//카운트 해야함
		int count = 0;
		for (int cnt = S; cnt <= E; cnt++)
			count++;

		if (count > 3 && count < 20)	//3pixel 이상, 10pixel 이하는 되어야 라인으로 간주한다는  뜻
		{
			IPVM::Point_32f_C2 temp(((vecPoint[E] + vecPoint[S]) * 0.5), roi.m_top);
			vecFindPoint.push_back(temp);

		}
	}

	m_vecStartPoint = vecFindPoint;

	float f3 = tc.Elapsed_ms();

	return (vecFindPoint.size()) ? 1 : 0;
}


long Inspectionzipper::GetLineData(BYTE* pByImage, long nWidth, long nHeight, float x0, float y0, float x1, float y1, float fCos, float fSin, BYTE* pLineOut)
{
	if (0 == nWidth)
		return 0;

	if (nWidth <= x0 || nWidth <= x1 || nHeight <= y0 || nHeight <= y1)
		return 0;

	float fdx = x1 - x0;
	float fdy = y1 - y0;
	float fdist = float(sqrt(fdx * fdx + fdy * fdy));

	long   nDataNum = long(fdist + 0.5f);

	if (nDataNum == 0) return 0;
	if (pLineOut == NULL) return nDataNum;

	//{{ Get Data
	for (long i = 0; i < nDataNum; i++)
	{
		float x = x0 + i * fCos;
		float y = y0 + i * fSin;

		long x1 = long(x);
		long y1 = long(y);

		float h1 = y - y1;
		float h2 = y1 + 1 - y;
		float w1 = x - x1;
		float w2 = x1 + 1 - x;

		unsigned char p1 = pByImage[y1 * nWidth + x1];
		unsigned char p2 = pByImage[y1 * nWidth + x1 + 1];
		unsigned char p3 = pByImage[(y1 + 1) * nWidth + x1 + 1];
		unsigned char p4 = pByImage[(y1 + 1) * nWidth + x1];

		pLineOut[i] = unsigned char(w2 * h2 * p1 + w1 * h2 * p2 + w1 * h1 * p3 + w2 * h1 * p4 + 0.5);
	}

	return nDataNum;
}

//return  : sweep라인이 nTabThreshold 값 이하로 모두 동일하다면 전극쪽 영역에 들어갔다고 판단한다.
bool Inspectionzipper::GetMeanStdev_BYTE(BYTE* i_pbData, long i_nSize, long nTabThreshold, float& o_fMean, float& o_fStdev)
{
	BOOL bisLine = true;

	if (0 == i_nSize)
	{
		o_fMean = 0.0f;
		o_fStdev = 0.0f;
		bisLine = false;
		return bisLine;
	}

	float fAverage = 0.0f;
	for (int i = 0; i < i_nSize; i++)
	{
		fAverage += i_pbData[i];

		if (nTabThreshold < i_pbData[i])
			bisLine = false;
	}
	fAverage /= i_nSize;

	float fData = 0.0f;
	for (int i = 0; i < i_nSize; i++)
	{
		fData += pow((i_pbData[i] - fAverage), 2);
	}

	o_fMean = fAverage;
	o_fStdev = sqrt(fData /= i_nSize);

	return bisLine;
}




//  --------------
//   ^   ^   ^
//   |   |   |
// S--------------E   가로 선을 주면, 세로 방향(90도)으로 Seach 길이 만큼 step 만큼 edge를 찾아서 돌려준다.
//   |   |   |
//   v   V   v 
//  --------------
// 이건, 폭을 가지고 있는 선들의 두께를 찾는다. 이선들의 두께를 찾아서 가운데것을 찾아 이것이 선폭이라고 추축해보려고한다. 
// 위의 내용으로 만들려고했는데 ...  일단 생략
// BOOL CWireTrack_Inspection::FindWireWidthEdge(CPoint cpStartPos, CPoint cpEndPos, float fRadian, long nStep, long nSearchLength, std::vector<float> &vecfLeftLength, std::vector<float> &vecfRightLength)
// 

BOOL Inspectionzipper::FindWireWidthEdge(IPVM::Image_8u_C1 src, IPVM::Point_32s_C2 cpPoint, float fLineRadian, long nSearchLength, float& fWireWidth)
{
	if (0 == nSearchLength || 0 == cpPoint.m_x || 0 == cpPoint.m_y || cpPoint.m_x >= src.GetSizeX() || cpPoint.m_y >= src.GetSizeY())
		return FALSE;


	//90도 회전 ( 왼쪽 )
	float fRadian = fLineRadian - (3.141592 * 0.5);

	IPVM::Point_32s_C2 point90;
	point90.m_x = cos(fRadian) * 10/*pixel*/;
	point90.m_y = sin(fRadian) * 10/*pixel*/;

	point90.m_x += cpPoint.m_x;
	point90.m_y += cpPoint.m_y;

	IPVM::EdgeDetectionPara edgePara;
	IPVM::EdgeDetectionFilter edgeFilter;
	IPVM::EdgeDetection edge;
	IPVM::Point_32f_C3 findedge[2];

	findedge[0].Set(0, 0, 0);
	findedge[1].Set(0, 0, 0);

	edgePara.m_findFirstEdge = false;
	edgePara.m_edgeType = IPVM::EdgeType::Rising;
	edgePara.m_searchDirection = IPVM::SearchDirection::ToLeft;

	BOOL bRet = false;
	if (IPVM::Status::success == edge.DetectEdge(src, edgePara, edgeFilter, cpPoint, point90, 5, findedge[0]))
	{
		fWireWidth += sqrt(pow(cpPoint.m_x - findedge[0].m_x, 2) + pow(cpPoint.m_y - findedge[0].m_y, 2));

		bRet = true;
	}


	//90도 회전  + 180도 ( 오른쪽 )  
	fRadian = fLineRadian + (3.141592 * 0.5);

	IPVM::Point_32s_C2 point180;
	point180.m_x = cos(fRadian) * 10/*pixel*/;
	point180.m_y = sin(fRadian) * 10/*pixel*/;

	point180.m_x += cpPoint.m_x;
	point180.m_y += cpPoint.m_y;

	edgePara.m_searchDirection = IPVM::SearchDirection::ToRight;
	if (IPVM::Status::success == edge.DetectEdge(src, edgePara, edgeFilter, cpPoint, point180, 5, findedge[1]))
	{
		fWireWidth += sqrt(pow(cpPoint.m_x - findedge[1].m_x, 2) + pow(cpPoint.m_y - findedge[1].m_y, 2));
		bRet = true;
	}

	IPVM::LineSeg_32f line1;
	line1.m_sx = cpPoint.m_x;
	line1.m_sy = cpPoint.m_y;
	line1.m_ex = findedge[0].m_x;
	line1.m_ey = findedge[0].m_y;

	IPVM::LineSeg_32f line2;
	line2.m_sx = cpPoint.m_x;
	line2.m_sy = cpPoint.m_y;
	line2.m_ex = findedge[1].m_x;
	line2.m_ey = findedge[1].m_y;


	m_vecFindWidthLine.push_back(line1);
	m_vecFindWidthLine.push_back(line2);

	m_vecFindWidth.push_back(findedge[0]);
	m_vecFindWidth.push_back(findedge[1]);

	return TRUE;
}


bool Inspectionzipper::FindDetialInsulation(const IPVM::Image_8u_C1 src, IPVM::Rect rtTab, IPVM::Rect& rtInsualtion, IPVM::Rect& rtAcTab)
{
	if (true == m_bUseROIfoundFromInsulationOverlayInspection)
	{
		rtInsualtion = rtAcTab = rtTab;
		rtInsualtion.m_right = m_rtFoundFromInsulationOverlayInspection.right;
		rtAcTab.m_left = m_rtFoundFromInsulationOverlayInspection.right;
		return false;
	}

	if (false == m_bUseZipper2AutoInsulationWidthPixel)
	{
		rtInsualtion = rtAcTab = rtTab;
		rtInsualtion.m_right = rtTab.m_left + m_nInsulationWidth;
		rtAcTab.m_left = rtTab.m_left + m_nInsulationWidth;
		return false;
	}
	long nOffset = 30;

	IPVM::Rect rtSearch = rtTab;
	rtSearch.m_left = rtTab.m_left + m_nInsulationWidth - nOffset;
	rtSearch.m_right = rtTab.m_left + m_nInsulationWidth + nOffset;


	std::vector<long> vecYsum;
	vecYsum.resize(rtSearch.Width(), 0);

	long nStepX = 2;
	long nStepY = 5;

	long nCnt = 0;
	for (int y = rtTab.m_top; y < rtTab.m_bottom; y = y+ nStepY)
	{
		int idx = 0;
		for (int x = rtSearch.m_left; x < rtSearch.m_right; x=x+ nStepX)
		{
			vecYsum[idx] += src.GetMem()[y * src.GetSizeX() + x];
			idx += nStepX;
		}
		nCnt++;
	}

	long MIN = 255, MAX = 1;
	for (int i = 0; i < vecYsum.size(); i++)
	{
		if (0 < vecYsum[i])
			vecYsum[i] /= nCnt;

		if(vecYsum[i] > 0 && vecYsum[i] > MAX)
		{ 
			MAX = vecYsum[i];
		}

		if (vecYsum[i] > 0 &&  vecYsum[i] < MIN)
		{
			MIN = vecYsum[i];
		}
	}

	if (75 > MAX - MIN)
	{
		rtInsualtion = rtAcTab = rtTab;
		rtInsualtion.m_right = rtTab.m_left + m_nInsulationWidth;
		rtAcTab.m_left = rtTab.m_left + m_nInsulationWidth;
		return false;
	}

	BYTE threshold = (MAX + MIN) * 0.5;

	long nLeft2RightPoint = 0, nRight2LeftPoint = 0;
	for (int i = 0; i < vecYsum.size(); i++)
	{
		if (vecYsum[i] > 0 && vecYsum[i] > threshold)
		{
			nLeft2RightPoint = i;
			break;
		}
	}

	for (int i = vecYsum.size()-1; i >= 0; i--)
	{
		if (vecYsum[i] > 0 && vecYsum[i] < threshold)
		{
			nRight2LeftPoint = i;
			break;
		}
	}

	long nFindPoint = 0;
	if (0 < nLeft2RightPoint && 0 < nRight2LeftPoint)
	{
		nFindPoint = (nLeft2RightPoint + nRight2LeftPoint) * 0.5;
	}
	else
	{
		rtInsualtion = rtAcTab = rtTab;
		rtInsualtion.m_right = rtTab.m_left + m_nInsulationWidth;
		rtAcTab.m_left = rtTab.m_left + m_nInsulationWidth;
		return false;
	}

	rtInsualtion = rtAcTab = rtTab;
	rtInsualtion.m_right = rtSearch.m_left + nFindPoint;
	rtAcTab.m_left = rtSearch.m_left + nFindPoint;


	return true;
}

bool Inspectionzipper::CombineResult(ZipperResult* InsuAreaCandidate, ZipperResult* TabAreaCandidate, ZipperResult& Result)
{
	std::vector<ZipperInfo>& IAC = InsuAreaCandidate->GetResult();
	std::vector<ZipperInfo>& TAC = TabAreaCandidate->GetResult();

	bool bCombine = false;
	for (int i = 0; i < IAC.size(); i++)
	{
		CRect rtIZ= IAC[i].GetDefectROI();
		for (int t = 0; t < TAC.size(); t++)
		{
			CRect rtTZ = TAC[t].GetDefectROI();

			rtTZ.InflateRect(m_nSweepLength * 0.3, m_nSweepLength * 0.3);

			CRect overlapRect = CRect(0, 0, 0, 0);
			overlapRect.IntersectRect(rtIZ, rtTZ);

			if (overlapRect.IsRectEmpty() || overlapRect.IsRectNull())	//겹치지 않으면 패스
			{
				continue;
			}

			if ((rtIZ.top <= rtTZ.top && rtIZ.bottom >= rtTZ.bottom) ||	//어느 하나 ROI가 상대적으로 더 크다면 패스
				(rtIZ.top >= rtTZ.top && rtIZ.bottom <= rtTZ.bottom))
			{
				continue;
			}

			ZipperInfo combineR;
			if (rtIZ.top > rtTZ.top)
			{
				for (int n = 0; n < TAC[t].m_vecSweepInfo.size(); n++)
				{
					if (rtIZ.top > TAC[t].m_vecSweepInfo[n].m_lsFindLine.m_sy ||
						rtIZ.top > TAC[t].m_vecSweepInfo[n].m_lsFindLine.m_ey)
					{
						combineR += TAC[t].m_vecSweepInfo[n];
					}

				}
			}

			for (int k = 0; k < IAC[i].m_vecSweepInfo.size(); k++)
			{
				combineR += IAC[i].m_vecSweepInfo[k];
			}


			if (rtIZ.bottom < rtTZ.bottom)
			{
				for (int n = 0; n < TAC[t].m_vecSweepInfo.size(); n++)
				{
					if (rtIZ.bottom < TAC[t].m_vecSweepInfo[n].m_lsFindLine.m_sy ||
						rtIZ.bottom < TAC[t].m_vecSweepInfo[n].m_lsFindLine.m_ey)
					{
						combineR += TAC[t].m_vecSweepInfo[n];
					}

				}
			}

			if (combineR.GetTotalLength_mm() > m_nSpecLength_mm)
			{
				Result.Set(combineR);
				bCombine = true;
				break;
			}
		}

		if (true == bCombine)
			break;

	}

	return bCombine;
}

