#include "stdafx.h"
#include "InspectionAbnormal.h"
#include "InspectionCell.h"
#include "InspectionCellAlign.h"
#include "DebugInfo.h"
#include "Accessor.h"
#include "UnionCellResult.h"

#include "Types/LineEq_32f.h"
#include "Algorithm/DataFitting.h"
#include "Types/LineEq_64f.h"
#include "Algorithm/Geometry.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "Algorithm/ImageProcessing.h"
#include <math.h>

#include "Algorithm/ImageProcessing.h"
#include "Algorithm/DataFitting.h"
#include "Algorithm/Geometry.h"
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/EdgeDetectionDebugInfo.h"
#include "Algorithm/Mathematics.h"
#include "Base/LoggerInterface.h"
#include "Types/Point_32f_C2.h"
#include "Types/Point_32f_C3.h"
#include "Types/Image_8u_C1.h"
#include "Types/Point_64f_C2.h"
#include "Types/LineEq_64f.h"
#include "Types/LineEq_32f.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionAbnormal::CInspectionAbnormal(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: CInspectionBase(parent, ProcItemCategory::abnormalInspection, logger, loggerResult)
	, m_parent((CInspectionCell*)parent)
{
}

CInspectionAbnormal::~CInspectionAbnormal()
{
}


bool CInspectionAbnormal::IsInspectionEnabled() const
{
	return true;
}

void CInspectionAbnormal::ResetInspection()
{
}

void CInspectionAbnormal::PrepareDebugInfo(Inspection::DebugInfo* debugInfo)
{
    debugInfo->AddItem(LanguageDef::Debug_Abnormal_GV_Result);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_EdgeDetectPoint);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_A_FullLength);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_B_LeftCoating);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_C_RightCoating);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_D_LeftFoil);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_E_MiddleFoil);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_E1_MiddleFoilLeftSlitting);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_E2_MiddleFoilRightSlitting);
	//debugInfo->AddItem(LanguageDef::Debug_Coater_F_RightFoil);



	//debugInfo->AddItem(LanguageDef::Debug_LinePoint);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeTiltPosition);
	//debugInfo->AddItem(LanguageDef::Debug_TabAreaForHeight);
	//debugInfo->AddItem(LanguageDef::Debug_CellPitch);
	//debugInfo->AddItem(LanguageDef::Debug_CellLength);
	//debugInfo->AddItem(LanguageDef::Debug_CellLength2);
	//debugInfo->AddItem(LanguageDef::Debug_TabWidth);
	//debugInfo->AddItem(LanguageDef::Debug_TabHeight);
	//debugInfo->AddItem(LanguageDef::Debug_TabHeight2);
	//debugInfo->AddItem(LanguageDef::Debug_Misalign);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeTilt);
	//debugInfo->AddItem(LanguageDef::Debug_TabSide);
	//debugInfo->AddItem(LanguageDef::Debug_CuttingHeightTop);
	//debugInfo->AddItem(LanguageDef::Debug_CuttingHeightBot);
	//debugInfo->AddItem(LanguageDef::Debug_VHomeHeight);
}
#include "Algorithm/FastFourierTransform1D.h"
//#include <math.h>
//#include <complex.h>

#include <algorithm>
#include <complex>
float CInspectionAbnormal::GetProfilePeriod_FFT(const std::vector<float>& profile)
{
	IPVM::FastFourierTransform1D fft;
	long profileLength = profile.size();

	std::vector<long> whiteCellLength;
	std::vector<long> blackCellLength;

	long fftLength = 0;

	if (IPVM::Status::success != fft.GetRequiredLength(long(profile.size()), fftLength))
	{
		return 0.f;
	}

	float dc = 0.f;

	if (IPVM::Status::success != IPVM::Mathematics::GetAverage(&profile[0], profile.size(), dc))
	{
		return 0.f;
	}

	std::vector<IPVM::Point_32f_C2> complexResult(fftLength, { 0.f, 0.f });

	for (long i = 0; i < profileLength; i++)
	{
		complexResult[i].m_x = profile[i] - dc;
	}

	if (IPVM::Status::success != fft.Forward(fftLength, &complexResult[0]))
	{
		return 0.f;
	}

	
	std::vector<float> absValue(fftLength);

	for (long i = 0; i < fftLength; i++)
	{
		absValue[i] = sqrt(complexResult[i].m_x * complexResult[i].m_x + complexResult[i].m_y * complexResult[i].m_y);
	}

	const long minFrequency = 3;

	float maxValue = 0;
	long maxID = 0;
	for (long i = minFrequency; i < fftLength / 2 - 1; i++)
	{
		if (absValue[i] >= absValue[i - 1] && absValue[i] >= absValue[i + 1] && absValue[i] > maxValue)
		{
			maxValue = absValue[i];
			maxID = i;
		}
	}
	if (maxID<1)
		return -1;

	float fMessCenter = 0;
	float fTotal = 0;
	for (long i = maxID - 1; i <= maxID + 1; i++)
	{
		fMessCenter += i * absValue[i];
		fTotal += absValue[i];
	}

	const float maxValueFloat = fMessCenter / fTotal;

	float period = fftLength / maxValueFloat;

	if (0)// 이거 풀면 진폭 계산은 됨..
	{
		std::vector<float> testAmp;
		long nperiod = period + 1;
		long nTempSize = profileLength / nperiod;
		long nTempDiv = profileLength / nTempSize;
		for (long n = 0; n < nTempSize; n++)
		{
			float max = 0;
			float min = 256;
			for (long m = 0; m < nperiod; m++)
			{
				float fTemp = profile[n * nperiod + m];
				if (max < fTemp)
					max = fTemp;
				if (min > fTemp)
					min = fTemp;


			}

			float temp = max - min;
			testAmp.push_back(temp);
			//fft 인버스 후 계산하여 진폭구하니 값이 이상해서 새로 만듬..불량이 있을수 있어 이중에 중간값 정도 쓰면 진폭 계산됨 
			// 검사위치를 더 다양하게 할시 더 잘 됨...
			//원본 데이터 스무딩 시 튀는게 더 적어 질 듯.. 
		}
		long n = 0;
		//long nFFTHalfLength = fftLength / 2;
		//// 2. dc, 음수 제거
		//complexResult[0].m_x = 0.f;
		//complexResult[0].m_y = 0.f;
		//for (int i = nFFTHalfLength; i < fftLength; i++)
		//{
		//	complexResult[i].m_x = 0.f;
		//	complexResult[i].m_y = 0.f;
		//}
		//long defaultcarrierFreq = 1;
		//long frequencyLowerBound = period;
		//long frequencyUpperBound = period*2;
		//// 3. C(f-fo) 영역만 남기고 제거 // 평행이동 안함
		//long nCarrierIndex = defaultcarrierFreq + 0.5f;
		//long nLowerIndex = max(0, nCarrierIndex * frequencyLowerBound + 0.5);
		//long nUpperIndex = min(nFFTHalfLength, nCarrierIndex * frequencyUpperBound + 0.5) + 1;

		//for (int i = 0; i < nLowerIndex; i++)
		//{
		//	complexResult[i].m_x = 0.f;
		//	complexResult[i].m_y = 0.f;
		//}
		//for (int i = nUpperIndex; i < fftLength / 2; i++)
		//{
		//	complexResult[i].m_x = 0.f;
		//	complexResult[i].m_y = 0.f;
		//}

		//// 4. IFFT
		//if (IPVM::Status::success != fft.Inverse(fftLength, &complexResult[0]))
		//{
		//	return 0;
		//}

		//// 5. Log 연산 -> phase, amplitude 계산
		//std::complex<float> cpm;
		//std::vector<float> dstAmp;
		//for (long i = 0; i < fftLength; i++)
		//{
		//	cpm._Val[0] = complexResult[i].m_x;
		//	cpm._Val[1] = complexResult[i].m_y;
		//	cpm = log(cpm);
		//	dstAmp.push_back( 2 * exp(cpm.real()));
		//	//dstPhase[i] = cpm.imag();
		//}
		//long n = 0;
	}

	return  period;
}

bool CInspectionAbnormal::ProcInspection(Inspection::DebugInfo* debugInfo)
{

	/*CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionMeasure* align = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::cellAlign);

    IPVM::Rect rect = align->m_unionResult.m_rectAreaBlob[1];
    m_para.m_rtSetInspArea = rect;*/
    CInspectionCellAlign* CellAlign = (CInspectionCellAlign*)m_parent->GetProcessingItem(ProcItemCategory::cellAlign);

	if (m_para.m_rtSetInspArea.IsRectNull())
    {
        if (CellAlign->m_para.m_nLineCountMeasure >= 6)
        {
            m_para.m_rtSetInspArea_2 = CellAlign->m_para.m_vecrtDetectLineMeasure[3];
            m_para.m_rtSetInspArea = CellAlign->m_para.m_vecrtDetectLineMeasure[1];
		}
        else if (CellAlign->m_para.m_nLineCountMeasure >= 4)
        {
            m_para.m_rtSetInspArea = CellAlign->m_para.m_vecrtDetectLineMeasure[1];
		}
	}

	double sum_bright = 0.;
    double sum_bright_2 = 0.;
    double favg_2 = 0.f;
	IPVM::ImageProcessing::GetSum(m_inspectionImageBright, m_para.m_rtSetInspArea, sum_bright);
	double favg = sum_bright / (m_para.m_rtSetInspArea.Width() * m_para.m_rtSetInspArea.Height());
	CString str;
	str.Format(_T("%.2f"), favg);

	CString str_2;
    if (CellAlign->m_para.m_nLineCountMeasure >= 6)
    {
        IPVM::ImageProcessing::GetSum(m_inspectionImageBright, m_para.m_rtSetInspArea_2, sum_bright_2);
        favg_2 = sum_bright_2 / (m_para.m_rtSetInspArea_2.Width() * m_para.m_rtSetInspArea_2.Height());
        str_2.Format(_T("%.2f"), favg_2);

	}
	// 주기 계산하는 코드입니다. 나중에 사용해서 계산할때 응용하세요!! to 남욱 예진 승만
	//if (m_para.m_rtSetInspArea.Height() > 10)
	//{
	//	
	//	
	//
	//	std::vector<float> fTemp;
	//
	//	fTemp.resize(m_para.m_rtSetInspArea.Height());
	//	
	//	for (long n = 0; n < m_para.m_rtSetInspArea.Height(); n++)
	//	{
	//		fTemp[n] = *m_inspectionImageBright.GetMem(m_para.m_rtSetInspArea.m_left, m_para.m_rtSetInspArea.m_top + n);
	//	}
	//	float fPeriod, b;
	//	
	//	fPeriod = GetProfilePeriod_FFT(fTemp);
	//	long n = 0;
	//	CString str;
	//	str.Format(_T("px Period = %f"),fPeriod);
	//	//AfxMessageBox(str);
	//}
	
	
	CString text = _T("(1) Average GV : ");
    debugInfo->AddText(LanguageDef::Debug_Abnormal_GV_Result, favg, text);

	if (CellAlign->m_para.m_nLineCountMeasure >= 6)
    {
        text = _T("(2) Average GV : ");
        debugInfo->AddText(LanguageDef::Debug_Abnormal_GV_Result, favg_2, text);
    }

	if (!m_para.m_bInspectionSkip)
	{
		return true;
	}

	if (favg > m_para.m_nGrayValue || favg_2 > m_para.m_nGrayValue_2)
	{
        m_resultValue = II_RESULT_VALUE::INVALID;
        return false;
	}
    else
    {
        m_resultValue = II_RESULT_VALUE::PASS;
        return true;
    }
}

void CInspectionAbnormal::ProcInspectionEnd(Inspection::DebugInfo* debugInfo)
{
	if (FALSE == m_parent->m_isTest) return;
}

void CInspectionAbnormal::LinkDataBase(bool bSave, Database& dbJob)
{
	m_para.LinkDataBase(bSave, m_isUp, dbJob);
}

bool CInspectionAbnormal::CopyDataBase(CProcessingBase* src)
{
	auto* ptr = dynamic_cast<CInspectionAbnormal*>(src);
	if (ptr == nullptr)
	{
		ASSERT(!_T("���� Ÿ���� Inspection�� ���簡 �����ϴ�"));
		return false;
	}

	auto& rhs = *ptr;

	m_pixelToUmX = rhs.m_pixelToUmX;
	m_pixelToUmY = rhs.m_pixelToUmY;

	m_para = rhs.m_para;

	m_isUp = rhs.m_isUp;

	return true;
}