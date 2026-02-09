#pragma once

#include "../basedef.h"
#include "../InspectionBase.h"

#include "Types/Quadrangle_32f.h"
#include "Types/Point_32f_C2.h"
#include "Types/Point_32f_C3.h"
#include "Types/LineSeg_32f.h"
#include "Algorithm/AlgorithmObjectHandle.h"
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/EdgeDetectionPara.h"



struct sweepInfo
{
	float fLength_mm;
	IPVM::LineSeg_32f m_lsFindLine;

	sweepInfo()
	{
		fLength_mm = 0.0f;
		m_lsFindLine.m_ex = m_lsFindLine.m_ey = m_lsFindLine.m_sx = m_lsFindLine.m_sy = 0;
	}
};

struct ZipperInfo
{
public:
	std::vector<sweepInfo> m_vecSweepInfo;

	void init()
	{
		m_vecSweepInfo.clear();
	}

	ZipperInfo()
	{
		init();
	}

	ZipperInfo(std::vector<sweepInfo> &vecSweepInfo)
	{
		m_vecSweepInfo = vecSweepInfo;
	}

	ZipperInfo& ZipperInfo::operator+ (const ZipperInfo& rhs)
	{
		for (int i = 0; i < rhs.m_vecSweepInfo.size(); i++)
		{
			m_vecSweepInfo.push_back(rhs.m_vecSweepInfo[i]);
		}

		return *this;
	}

	ZipperInfo& ZipperInfo::operator+= (const sweepInfo& rhs)
	{
		m_vecSweepInfo.push_back(rhs);
		return *this;
	}


	float GetTotalLength_mm()
	{
		float fTotalLength = 0.0f;
		for (int i = 0; i < m_vecSweepInfo.size(); i++)
		{
			fTotalLength += m_vecSweepInfo[i].fLength_mm;
		}

		return fTotalLength;
	}

	CRect GetDefectROI(int x=0, int y=0)
	{
		IPVM::Rect rtRoi(999999, 999999, 0, 0);

		for (int i = 0; i < m_vecSweepInfo.size(); i++)
		{
			if (rtRoi.m_left > m_vecSweepInfo[i].m_lsFindLine.m_sx)
			{
				rtRoi.m_left = m_vecSweepInfo[i].m_lsFindLine.m_sx;
			}

			if (rtRoi.m_top > m_vecSweepInfo[i].m_lsFindLine.m_sy)
			{
				rtRoi.m_top = m_vecSweepInfo[i].m_lsFindLine.m_sy;
			}

			if (rtRoi.m_right < m_vecSweepInfo[i].m_lsFindLine.m_sx)
			{
				rtRoi.m_right = m_vecSweepInfo[i].m_lsFindLine.m_sx;
			}

			if (rtRoi.m_bottom < m_vecSweepInfo[i].m_lsFindLine.m_sy)
			{
				rtRoi.m_bottom = m_vecSweepInfo[i].m_lsFindLine.m_sy;
			}

			/**/
			if (rtRoi.m_left > m_vecSweepInfo[i].m_lsFindLine.m_ex)
			{
				rtRoi.m_left = m_vecSweepInfo[i].m_lsFindLine.m_ex;
			}

			if (rtRoi.m_top > m_vecSweepInfo[i].m_lsFindLine.m_ey)
			{
				rtRoi.m_top = m_vecSweepInfo[i].m_lsFindLine.m_ey;
			}

			if (rtRoi.m_right < m_vecSweepInfo[i].m_lsFindLine.m_ex)
			{
				rtRoi.m_right = m_vecSweepInfo[i].m_lsFindLine.m_ex;
			}

			if (rtRoi.m_bottom < m_vecSweepInfo[i].m_lsFindLine.m_ey)
			{
				rtRoi.m_bottom = m_vecSweepInfo[i].m_lsFindLine.m_ey;
			}
			/**/
		}

		CRect rt(rtRoi.m_left, rtRoi.m_top, rtRoi.m_right, rtRoi.m_bottom);
		rt.InflateRect(x, y);
		return rt;
	}
};


struct ZipperResult
{
private:
	std::vector<ZipperInfo> vecZipperInfo;

public:
	ZipperResult()
	{
		init();
	}

	void init()
	{
		vecZipperInfo.clear();
	}

	void Set(std::vector<sweepInfo> &vecSweepInfo)
	{
		ZipperInfo info(vecSweepInfo);
		vecZipperInfo.push_back(info);
	}

	void Set(ZipperInfo info)
	{
		vecZipperInfo.push_back(info);
	}

	long GetResultCount()
	{
		return vecZipperInfo.size();
	}

	std::vector<ZipperInfo>& GetResult()
	{
		return vecZipperInfo;
	}

	ZipperResult& ZipperResult::operator+= (const ZipperResult& rhs)
	{
		for (int i = 0; i < rhs.vecZipperInfo.size(); i++)
		{
			vecZipperInfo.push_back(rhs.vecZipperInfo[i]);
		}

		return *this;
	}
};




class __INSPECTOIN_CLASS__ Inspectionzipper
{
public:
	Inspectionzipper();
	~Inspectionzipper();
	
public:
	void initData();

	void SetParameter(long m_nSweepAngle, long m_nSweepCount, long m_nSweepLength, float fStartpointThreshold, float fTabAreaThreshold, float m_fAverageFilter, float m_fScale_X_um, float m_fScale_Y_um, long m_nSpecLength_mm, long nInsulationWidth, CRect rtInsulation/*절연오버레이검사에서 찾은 절연 roi*/, bool bUseROIfoundFromInsulationOverlayInspection, float fSetZipper2InspectTime, bool bCheckInspectionMiddleArea, float fInsulationStartPointThreshold, float fInsulationfTabAreaThreshold, bool bUseZipper2InsulationInspect, bool bUseZipper2AutoInsulationWidthPixel, long nDebugTitle);
	
	bool Doinspection(IPVM::Image_8u_C1 src, IPVM::Rect tab, bool m_bIstest, bool bUseInsulation, Inspection::DebugInfo* debugInfo = nullptr);
	bool inspection(IPVM::Image_8u_C1 src, IPVM::Rect tab, bool m_bIstest, long nTabThreshold, long nStartPointThresholdOffset, long nTabAreaThresholdOffset, ZipperResult &result, ZipperResult& result_candidate, IPVM::TimeCheck* ptc, Inspection::DebugInfo* debugInfo = nullptr);

	BYTE GetThreshold(const IPVM::Image_8u_C1 src, IPVM::Rect tab, int nOffset = 0, int nXOffset = 15, int nYOffset = 15, float fPosition = 0.5, Inspection::DebugInfo* debugInfo = nullptr);
	
	ZipperInfo FindZipper(const IPVM::Image_8u_C1 src, IPVM::Rect tab, bool bDown2Up, long nTabThreshold, IPVM::Point_32f_C2 pFindPoint, long nSweepAngle, long nSweepCount, long nSweepLength, float fAverageFilter, float fscaleX, float fscaleY, Inspection::DebugInfo* debugInfo = nullptr);
	bool SearchStartPoint(const IPVM::Image_8u_C1 src, IPVM::Rect roi, IPVM::Rect rttabthresholdArea, long nThresholdOffset,  std::vector<IPVM::Point_32f_C2> &vecFindPoint, Inspection::DebugInfo* debugInfo = nullptr);

	BOOL FindWireWidthEdge(IPVM::Image_8u_C1 src, IPVM::Point_32s_C2 cpPoint, float fLineRadian, long nSearchLength, float& fWireWidth);
	bool GetMeanStdev_BYTE(BYTE* i_pbData, long i_nSize, long nTabThreshold,float& o_fMean, float& o_fStdev);
	long GetLineData(BYTE* pByImage, long nWidth, long nHeight, float x0, float y0, float x1, float y1, float fCos, float fSin, BYTE* pLineOut);


	//debug
	BYTE m_byTabThreshold[2];
	std::vector<IPVM::LineSeg_32f> m_vecSweepLine;
	std::vector<IPVM::LineSeg_32f> m_vecSweepLineCandidate;
	std::vector<IPVM::Point_32f_C2> m_vecStartPoint;
	std::vector<IPVM::Point_32f_C2> m_vecStartPointCandidate;
	std::vector<IPVM::Point_32f_C3> m_vecFindWidth;
	std::vector<IPVM::LineSeg_32f> m_vecFindWidthLine;
	std::vector<IPVM::Point_32f_C2> m_vecMinMaxCha2Point;
	std::vector<IPVM::Point_32f_C2> m_vecLengthStartPoint;
	std::vector<IPVM::Point_32f_C2> m_vecOutOfTab;
	std::vector<float> m_vecMinMaxCha2;
	std::vector<float> m_vecLength;
	//std::vector<float> m_vecCandidateLength;
	IPVM::Rect m_rtTabSearch[2];
	IPVM::Rect m_rtStartPointThresholdGetArea;	//스타트 포인트에서 threshold를 가지고 와야함 


	bool FindDetialInsulation(const IPVM::Image_8u_C1 src, IPVM::Rect rtTab, IPVM::Rect& rtInsualtion, IPVM::Rect& rtAcTab);
	bool CombineResult(ZipperResult * InsuAreaCandidate, ZipperResult* TabAreaCandidate, ZipperResult &Result);
	

	ZipperResult m_ZipperResult;

private:


	bool m_bUseZipper2InspectionMiddleArea;
	long m_nDebug_title;

	float m_fScale_X_um;
	float m_fScale_Y_um;

	long m_nSpecLength_mm;

	long m_nSweepAngle;
	long m_nSweepCount;
	long m_nSweepLength;
	float m_fStartpointThresholdOffset;
	float m_fTabAreaThresholdOffset;
	float m_fSetZipper2InspectTime;
	float m_fInsulationStartPointThreshold;
	float m_fInsulationfTabAreaThreshold;
	bool m_bUseZipper2InsulationInspect;
	bool m_bUseZipper2AutoInsulationWidthPixel;
	bool m_bUseROIfoundFromInsulationOverlayInspection;

	CRect m_rtFoundFromInsulationOverlayInspection;

	long m_nInsulationWidth;

	float m_fAverageFilter;	//sweep 라인 평균들이 고르게 분포하고 있다면 라인이 없는걸로 판단한다. 

};

