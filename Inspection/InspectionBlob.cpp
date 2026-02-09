#include "stdafx.h"
#include "InspectionBlob.h"
#include "InspectionCell.h"
//#include "InspectionShoulderLine.h"
#include "Accessor.h"
#include "DebugInfo.h"
#include "UnionCellResult.h"

#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../IBCI/SystemParameter.h"

#include "../CompanySharedLibrary/Persistence/Database.h"
#include "Algorithm/BlobDetection.h"

#include "Base/basedef.h"

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

#include "Gadget/TimeCheck.h"

#include "InspectionCell.h"

#include <cmath>
#include <algorithm>
#include <numeric>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionBlob::CInspectionBlob(CInspectionCell* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: CProcessingBase(parent, ProcItemCategory::cellAlign, logger, loggerResult)
	, m_parent(parent)
{

	for (int i = 0; i < BLOB_COUNT; i++)
	{
		m_Width[i] = new float[MAX_DEFECT_COUNT];
		m_Length[i] = new float[MAX_DEFECT_COUNT];
		m_Locus[i] = new float[MAX_DEFECT_COUNT];
		m_Thickness[i] = new float[MAX_DEFECT_COUNT];
		m_MaxThickness[i] = new float[MAX_DEFECT_COUNT];
		m_MaxGV[i] = new float[MAX_DEFECT_COUNT];
		m_MinGV[i] = new float[MAX_DEFECT_COUNT];
		m_AvgGV[i] = new float[MAX_DEFECT_COUNT];
		m_massPoint[i] = new IPVM::Point_32f_C2[MAX_DEFECT_COUNT];
		m_Compact[i] = new float[MAX_DEFECT_COUNT];
		//	m_BlobDatas[i].m_bufferBlobInfo = new IPVM::BlobInfo[BLOB_NUM_MAX];
		m_BlobDatas[i].SetMap(&m_parent->m_ISICellAllDefect.m_mapInspAreaInfo);
	}

}

CInspectionBlob::~CInspectionBlob()
{
	for (int i = 0; i < BLOB_COUNT; i++)
	{
		delete m_Width[i];
		delete m_Length[i];
		delete m_Locus[i];
		delete m_Thickness[i];
		delete m_MaxThickness[i];
		delete m_MaxGV[i];
		delete m_MinGV[i];
		delete m_AvgGV[i];
		delete m_massPoint[i];
		delete m_Compact[i];
	//	delete m_BlobDatas[i].m_bufferBlobInfo;
		
	}

	
	
}

void CInspectionBlob::PrepareDebugInfo(Inspection::DebugInfo* debugInfo)
{
	debugInfo->AddItem(LanguageDef::Debug_SurfaceResult);
    debugInfo->AddItem(LanguageDef::Debug_Equlization);
    //debugInfo->AddItem(LanguageDef::Debug_TestImage1);
    debugInfo->AddItem(LanguageDef::Debug_TestImage2);
    debugInfo->AddItem(LanguageDef::Debug_TestImage3);
    debugInfo->AddItem(LanguageDef::Debug_TestImage4);
    debugInfo->AddItem(LanguageDef::Debug_TestImage5);
    debugInfo->AddItem(LanguageDef::Debug_InspTime);

}

void CInspectionBlob::BinarizationLess_Mean(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimage, float& fAverage)
{
    IPVM::Rect rtRoi = roi;
    rtRoi &= IPVM::Rect(srcimage);
    int nWidth = srcimage.GetSizeX();
    int nHeight = srcimage.GetSizeY();
    double dAverage = 0.0f;

    if (IPVM::Status::success != IPVM::ImageProcessing::GetMeanInRange(srcimage, rtRoi, 10, 245, dAverage))
        ASSERT(1);
    fAverage = dAverage;
    int nTH = dAverage - Th;
    if (0 > nTH)
        nTH = 1;
    else if (255 < nTH)
        nTH = 255;

    IPVM::ImageProcessing::BinarizeLess(srcimage, roi, nTH, dstimage);
}

void CInspectionBlob::BinarizationLess_Mean_noRange(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimag, float& fAverage)
{
    IPVM::Rect rtRoi = roi;
    rtRoi &= IPVM::Rect(srcimage);
    int nWidth = srcimage.GetSizeX();
    int nHeight = srcimage.GetSizeY();
    double dAverage = 0.0f;
    

    double dMean = 0.f;
    if (IPVM::Status::success != IPVM::ImageProcessing::GetMean(srcimage, rtRoi, dMean))
        ASSERT(1);

    fAverage = dMean;
    int nTH = dMean - Th;
    if (0 > nTH)
        nTH = 1;
    else if (255 < nTH)
        nTH = 255;

    IPVM::ImageProcessing::BinarizeLess(srcimage, roi, nTH, dstimag);

    
}

void CInspectionBlob::BinarizationGreater_Mean(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimage, float& fAverage)
{
	IPVM::Rect rtRoi = roi;
	rtRoi &= IPVM::Rect(srcimage);
	int nWidth = srcimage.GetSizeX();
	int nHeight = srcimage.GetSizeY();
	double dAverage = 0.0f;

	if (IPVM::Status::success != IPVM::ImageProcessing::GetMeanInRange(srcimage, rtRoi, 10, 245, dAverage))
		ASSERT(1);
	fAverage = dAverage;
	int nTH = dAverage + Th;
	if (0 > nTH)
		nTH = 1;
	else if (255 < nTH)
		nTH = 255;
	IPVM::ImageProcessing::BinarizeGreater(srcimage, roi, dAverage + Th, dstimage);

}

void CInspectionBlob::BinarizationGreater_Mean_noRange(const IPVM::Image_8u_C1& srcimage, const IPVM::Rect& roi, const int& Th, IPVM::Image_8u_C1& dstimag, float& fAverage)
{
    IPVM::Rect rtRoi = roi;
    rtRoi &= IPVM::Rect(srcimage);
    int nWidth = srcimage.GetSizeX();
    int nHeight = srcimage.GetSizeY();
    double dAverage = 0.0f;

    double dMean = 0.f;
    if (IPVM::Status::success != IPVM::ImageProcessing::GetMean(srcimage, rtRoi, dMean))
        ASSERT(1);

    fAverage = dMean;
    int nTH = dMean + Th;
    if (0 > nTH)
        nTH = 1;
    else if (254 < nTH)
        nTH = 254;

    IPVM::ImageProcessing::BinarizeLess(srcimage, roi, nTH, dstimag);
}



bool CInspectionBlob::ProcInspection(Inspection::DebugInfo* debugInfo)
{
    //	debugInfo->SetText(LanguageDef::Result, _T("Cell Align : Pass"));

#ifdef RESET_IMAGE_BUFFER_WHEN_MODULE_INSPECTION
    ResetImageBuffer(false);
#endif
    m_DebugInfo = debugInfo;
    CString strDir;
    if (m_isUp)
    {
        strDir = _T("Up");
    }
    else
    {
        strDir = _T("Down");
    }

    long nNumInspectionBlob = m_para.m_vecBlobParaCondition.size();

    auto* Align = (CInspectionCellAlign*)m_parent->GetProcessingItem(ProcItemCategory::cellAlign);

    long nBlobCnt = Align->m_para.m_nLineCountBlob;
    m_unionResult.m_rectResult.clear();

    ////
    /*
	조명종류
	다크 브라이트
	설정
	ROI
	*/
    long nSumCheckCnt = 0;
    for (long n = 0; n < nNumInspectionBlob; n++)
    {
        bool bCheck = m_para.m_vecBlobParaCondition[n].m_bUseInspection;
        if (bCheck)
        {
            nSumCheckCnt++;
        }
    }

    m_pvecsObjInfo.clear();
    m_vecnBlobNum.clear();
    m_vecrtCal.clear();
    m_item.clear();
    m_EqualItem.clear();

    m_pvecsObjInfo.resize(nNumInspectionBlob);
    m_vecnBlobNum.resize(nNumInspectionBlob);
    m_vecrtCal.resize(nNumInspectionBlob);
    m_item.resize(nNumInspectionBlob);

#if INSPECTION_MACHINE_UC1
    // IPVM::Rect rectSub[120];

    //for (int n = 0; n < 100; n++)
    //{
    //    m_bEqualImageFlag_Bright[n] = FALSE;
    //    m_bEqualImageFlag_Dark[n] = FALSE;
    //}

    //for (long n = 0; n < 160; n++)
    //{
    //    m_Flag_InspectionItemsReady[n] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    //    //::ResetEvent(m_Flag_InspectionItemsReady[n]);
    //}

    //for (long n = 0; n < 160; n++)
    //{
    //    m_Flag_EqualizationItemsReady[n] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    //}

    /* long nEqualCheck = 0;
    IPVM::TimeCheck tc;
    tc.Reset();
    BOOL bEqualThreadCheck = FALSE;*/

    //for (long index = 0; index < 5; index++)
    //{
    //    IPVM::Rect rect;
    //   // rect = m_unionResult.m_rectAreaBlob[index];

    //    for (long n = 0; n < nNumInspectionBlob; n++)
    //    {
    //        bool bCheckInsp = m_para.m_vecBlobParaCondition[n].m_bUseInspection;
    //        if (bCheckInsp)
    //        {
    //            if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
    //            {
    //                long nSubIndex = m_para.m_vecBlobParaCondition[n].m_nInspectionArea;
    //                if (index != nSubIndex)
    //                {
    //                    continue;
    //                }
    //                if (index == nSubIndex && (m_bEqualImageFlag_Bright[index] == TRUE || m_bEqualImageFlag_Dark[index] == TRUE))
    //                {
    //                    continue;
    //                }
    //                long nDivThreadSize = m_para.m_vecBlobParaCondition[n].m_nEqualThreadCount;
    //                m_EqualItem.resize(nNumInspectionBlob * nDivThreadSize);
    //                for (long nDiv = 0; nDiv < nDivThreadSize; nDiv++)
    //                {
    //                    rectSub[nDiv] = rect;
    //                }
    //                if (nDivThreadSize > 1)
    //                {
    //                    long nWidthSubSize = rect.Width() / nDivThreadSize;
    //                    for (long nDiv = 0; nDiv < nDivThreadSize; nDiv++)
    //                    {
    //                        rectSub[nDiv] = rect;
    //                    }
    //                    rectSub[0].m_right = rect.m_left + nWidthSubSize;
    //                    for (long nDiv = 1; nDiv < nDivThreadSize - 1; nDiv++)
    //                    {
    //                        rectSub[nDiv].m_left = rectSub[nDiv - 1].m_right;
    //                        rectSub[nDiv].m_right = rectSub[nDiv].m_left + nWidthSubSize;
    //                    }
    //                    rectSub[nDivThreadSize - 1].m_left = rectSub[nDivThreadSize - 2].m_right;
    //                }
    //                if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)
    //                {
    //                    m_EqualImage_Bright[index].Create(
    //                        IPVM::Rect(m_inspectionImageBright).Width(), IPVM::Rect(m_inspectionImageBright).Height());

    //                    if (m_bEqualImageFlag_Bright[index] == FALSE)
    //                    {
    //                        IPVM::ImageProcessing::Copy(m_inspectionImageBright, rect, rect,
    //                            m_EqualImage_Bright[index]); //각 Rect영역에 대한 이미지 복사
    //                        if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
    //                        {
    //                            /*Equalization_TargetWeighted(m_EqualImage_Bright[index], rect, 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                        m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);*/
    //                            for (long nSub = 0; nSub < nDivThreadSize; nSub++)
    //                            {
    //                                m_EqualItem[n * nDivThreadSize + nSub]
    //                                    = new SEqualizationItems(this, m_EqualImage_Bright[index], rectSub[nSub], 128,
    //                                        1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                                        m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
    //                                AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n * nDivThreadSize + nSub]);
    //                                nEqualCheck++;
    //                            }
    //                            bEqualThreadCheck = TRUE;
    //                            m_bEqualImageFlag_Bright[index] = TRUE;
    //                        }
    //                    }
    //                }
    //                else
    //                {
    //                    m_EqualImage_Dark[index].Create(
    //                        IPVM::Rect(m_inspectionImageDark).Width(), IPVM::Rect(m_inspectionImageDark).Height());
    //                    if (m_bEqualImageFlag_Dark[index] == FALSE)
    //                    {
    //                        IPVM::ImageProcessing::Copy(m_inspectionImageDark, rect, rect,
    //                            m_EqualImage_Dark[index]); //각 Rect영역에 대한 이미지 복사
    //                        if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
    //                        {
    //
    //                            for (long nSub = 0; nSub < nDivThreadSize; nSub++)
    //                            {
    //                                m_EqualItem[n * nDivThreadSize + nSub]
    //                                    = new SEqualizationItems(this, m_EqualImage_Dark[index], rectSub[nSub], 128,
    //                                        1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                                        m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
    //                                AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n * nDivThreadSize + nSub]);
    //                                nEqualCheck++;
    //                            }

    //
    //                            bEqualThreadCheck = TRUE;
    //                            m_bEqualImageFlag_Dark[index] = TRUE;
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}

    /* if (bEqualThreadCheck)
    {
        DWORD dwWaitEqualResult = ::WaitForMultipleObjects(nEqualCheck, m_Flag_EqualizationItemsReady, TRUE, INFINITE);
    }*/

    // float fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    // debugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[pre process(equalization)](ms)"));

    long nSumCheck = 0;
    for (long n = 0; n < nNumInspectionBlob; n++)
    {
        m_vecnBlobNum[n] = 0;
        bool bCheckInsp = m_para.m_vecBlobParaCondition[n].m_bUseInspection;

        if (bCheckInsp)
        {
            // refresh roi information
            IPVM::Rect rect;
            long index = m_para.m_vecBlobParaCondition[n].m_nInspectionArea;
            rect = m_unionResult.m_rectAreaBlob[index];

            switch (m_para.m_vecBlobParaCondition[n].m_nSetTypeInspArea)
            {
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::OffsetRoi:
                {
                }
                break;
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Left:
                {
                    rect.m_right = rect.m_left;
                }
                break;
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Right:
                {
                    rect.m_left = rect.m_right;
                }
                break;
            }

            CString strRectValue;
            CString strLeft, strTop, strRight, strBottom;

            IPVM::Rect offset = m_para.m_vecBlobParaCondition[n].m_rtOffsetRoi;
            rect.DeflateRect(offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);

            m_vecrtCal[n] = rect;

            //m_CropImage[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());

            /*
			m_CropImage2[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());

			m_CropImage3[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());
			m_CropImage4[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());*/

            m_BlobDatas[n].m_bufferimage = &m_bufferThreshold5;

            if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)
            {
                {
                    IPVM::ImageProcessing::Copy(m_inspectionImageBright, m_vecrtCal[n], m_vecrtCal[n],
                        m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                }
                //IPVM::ImageProcessing::Copy(m_EqualImage_Bright[index], m_vecrtCal[n], m_vecrtCal[n],
                //    m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
                {
                    Equalization_TargetWeighted(m_bufferThreshold, m_vecrtCal[n], 128, 1.5f,
                        m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
                        m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);
                }
                if (m_para.m_vecBlobParaCondition[n].m_nInspectionType == (long)ENUM_BLOB_TYPE_INSPTYPE::Defect2)
                {
                    //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);

                    //IPVM::ImageProcessing::FilterBox(
                    //    m_bufferThreshold2, m_vecrtCal[n], 3, 3, TRUE, m_bufferThreshold);
                    //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);

                    //IPVM::ImageProcessing::FilterBox(m_bufferThreshold2, m_vecrtCal[n], 3, 3, TRUE, m_bufferThreshold);

                    IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);
                    ImageProcessing_BoxFilter(m_bufferThreshold2, m_vecrtCal[n], m_bufferThreshold);
                }
                //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]), m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::Mix)
                {
                    //IPVM::ImageProcessing::Copy(m_inspectionImageBright, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]), m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                    IPVM::ImageProcessing::Copy(
                        m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold3); //각 Rect영역에 대한 이미지 복사
                }
            }
            else
            {
                {
                    IPVM::ImageProcessing::Copy(m_inspectionImageDark, m_vecrtCal[n], m_vecrtCal[n],
                        m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                }
                if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::MAX)
                {
                    //m_bufferLabel
                    IPVM::ImageProcessing::Max(
                        m_inspectionImageDark, m_inspectionImageBright, m_vecrtCal[n], m_bufferThreshold);
                }
                //IPVM::ImageProcessing::Copy(m_EqualImage_Dark[index], m_vecrtCal[n], m_vecrtCal[n],
                //    m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
                {
                    Equalization_TargetWeighted(m_bufferThreshold, m_vecrtCal[n], 128, 1.5f,
                        m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
                        m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);
                }
                if (m_para.m_vecBlobParaCondition[n].m_nInspectionType == (long)ENUM_BLOB_TYPE_INSPTYPE::Defect2)
                {
                    IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);
                    ImageProcessing_BoxFilter(m_bufferThreshold2, m_vecrtCal[n], m_bufferThreshold);
                }
                //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]),
                //    m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::Mix)
                {
                    //IPVM::ImageProcessing::Copy(m_inspectionImageBright, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]), m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                    IPVM::ImageProcessing::Copy(
                        m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold3); //각 Rect영역에 대한 이미지 복사
                }
            }

            //  여기까지는 쓰레드로 돌리면 안된다. ROI 가 겹칠 위험이 있음.
            long nMaxBlobCnt = m_para.m_vecBlobParaCondition[n].m_nBlobMaxCount;

            m_pvecsObjInfo[n] = new CInspectionCustomBlobInfo[nMaxBlobCnt];
            // 이거는 처음에 한버난 만들어 놓자.
            long nInspIllum = (long)m_para.m_vecBlobParaCondition[n].m_nImageType;

            m_vecnBlobNum[n] = InspBlob(m_bufferThreshold, m_bufferThreshold5, m_bufferThreshold3, m_bufferThreshold4,
                m_pvecsObjInfo[n], n, nInspIllum, m_vecrtCal[n]);

            nSumCheck++;
        }
    }

    for (long n = 0; n < nNumInspectionBlob; n++)
    {
        ISICF::enInspectionImageType ImageType;
        ISICF::enDefectBrightness DefectBrigtness;

        if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)
        {
            ImageType = ISICF::enInspectionImageType::Bright;
        }
        else
        {
            ImageType = ISICF::enInspectionImageType::Dark;
        }

        if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::White)
        {
            DefectBrigtness = ISICF::enDefectBrightness::DFB_WHITE;
        }
        else if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::Black)
        {
            DefectBrigtness = ISICF::enDefectBrightness::DFB_BLACK;
        }
        else
        {
            DefectBrigtness = ISICF::enDefectBrightness::DFB_MIX;
        }

        auto& Defect = m_parent->m_ISICellAllDefect;
        Defect.Init(n, &m_inspectionImageBright, &m_inspectionImageDark);
        auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
        long nTempArea = 0;
        if (isi_spec.m_vecInspAreaSet.size() > n)
        {
            nTempArea = isi_spec.m_vecInspAreaSet[n];
        }
        m_BlobDatas[n].SetBlobDataInfo(m_unionResult.m_rectAreaBlob[n], nTempArea, DefectBrigtness, ImageType);
        m_BlobDatas[n].m_bufferBlobInfo = m_pvecsObjInfo[n]->sObjInfo;
        m_BlobDatas[n].m_BlobNum = m_vecnBlobNum[n];
        for (int m = 0; m < m_vecnBlobNum[n]; m++)
        {
            {
                m_unionResult.m_rectResult.push_back(m_pvecsObjInfo[n]->sObjInfo[m].m_roi);
                debugInfo->AddOverlay(
                    LanguageDef::Debug_SurfaceResult, m_pvecsObjInfo[n]->sObjInfo[m].m_roi, RGB(255, 0, 0));
            }
        }
    }
    GetDefectProperty(BLOB_COUNT);

    for (long n = 0; n < m_pvecsObjInfo.size(); n++)
    {
        delete m_pvecsObjInfo[n];
    }
    m_resultValue = II_RESULT_VALUE::PASS;

    // fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[all Thread Calc Time](ms)"));

    // fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[all Thread Calc Time](ms)"));
#else
    // 임시니 나중에 바꿔야 함.
    /*long nDivThreadSize = 5;
    IPVM::Rect rectSub[5];
    m_EqualItem.resize(nNumInspectionBlob * nDivThreadSize);*/
    IPVM::Rect rectSub[120];

    for (int n = 0; n < 100; n++)
    {
        m_bEqualImageFlag_Bright[n] = FALSE;
        m_bEqualImageFlag_Dark[n] = FALSE;
    }

    for (long n = 0; n < 160; n++)
    {
        m_Flag_InspectionItemsReady[n] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        //::ResetEvent(m_Flag_InspectionItemsReady[n]);
    }

    for (long n = 0; n < 160; n++)
    {
        m_Flag_EqualizationItemsReady[n] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    long nEqualCheck = 0;
    IPVM::TimeCheck tc;
    tc.Reset();
    BOOL bEqualThreadCheck = FALSE;

    for (long index = 0; index < m_para.m_nTotalInspCount; index++)
    {
        IPVM::Rect rect;
        rect = m_unionResult.m_rectAreaBlob[index];

        for (long n = 0; n < nNumInspectionBlob; n++)
        {
            bool bCheckInsp = m_para.m_vecBlobParaCondition[n].m_bUseInspection;
            if (bCheckInsp)
            {
                if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
                {
                    long nSubIndex = m_para.m_vecBlobParaCondition[n].m_nInspectionArea;
                    if (index != nSubIndex)
                    {
                        continue;
                    }
                    if (index == nSubIndex
                        && (m_bEqualImageFlag_Bright[index] == TRUE || m_bEqualImageFlag_Dark[index] == TRUE))
                    {
                        continue;
                    }
                    long nDivThreadSize = m_para.m_vecBlobParaCondition[n].m_nEqualThreadCount;
                    m_EqualItem.resize(nNumInspectionBlob * nDivThreadSize);
                    for (long nDiv = 0; nDiv < nDivThreadSize; nDiv++)
                    {
                        rectSub[nDiv] = rect;
                    }
                    if (nDivThreadSize > 1)
                    {
                        long nWidthSubSize = rect.Width() / nDivThreadSize;
                        for (long nDiv = 0; nDiv < nDivThreadSize; nDiv++)
                        {
                            rectSub[nDiv] = rect;
                        }
                        rectSub[0].m_right = rect.m_left + nWidthSubSize;
                        for (long nDiv = 1; nDiv < nDivThreadSize - 1; nDiv++)
                        {
                            rectSub[nDiv].m_left = rectSub[nDiv - 1].m_right;
                            rectSub[nDiv].m_right = rectSub[nDiv].m_left + nWidthSubSize;
                        }
                        rectSub[nDivThreadSize - 1].m_left = rectSub[nDivThreadSize - 2].m_right;
                    }
                    if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)
                    {
                        m_EqualImage_Bright[index].Create(
                            IPVM::Rect(m_inspectionImageBright).Width(), IPVM::Rect(m_inspectionImageBright).Height());

                        if (m_bEqualImageFlag_Bright[index] == FALSE)
                        {
                            IPVM::ImageProcessing::Copy(m_inspectionImageBright, rect, rect,
                                m_EqualImage_Bright[index]); //각 Rect영역에 대한 이미지 복사
                            if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
                            {
                                /*Equalization_TargetWeighted(m_EqualImage_Bright[index], rect, 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
                            m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);*/
                                for (long nSub = 0; nSub < nDivThreadSize; nSub++)
                                {
                                    m_EqualItem[n * nDivThreadSize + nSub]
                                        = new SEqualizationItems(this, m_EqualImage_Bright[index], rectSub[nSub], 128,
                                            1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
                                            m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
                                    AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n * nDivThreadSize + nSub]);
                                    nEqualCheck++;
                                }
                                bEqualThreadCheck = TRUE;
                                m_bEqualImageFlag_Bright[index] = TRUE;
                            }
                        }
                    }
                    else
                    {
                        m_EqualImage_Dark[index].Create(
                            IPVM::Rect(m_inspectionImageDark).Width(), IPVM::Rect(m_inspectionImageDark).Height());
                        if (m_bEqualImageFlag_Dark[index] == FALSE)
                        {
                            IPVM::ImageProcessing::Copy(m_inspectionImageDark, rect, rect,
                                m_EqualImage_Dark[index]); //각 Rect영역에 대한 이미지 복사
                            if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
                            {
                                /*Equalization_TargetWeighted(m_EqualImage_Dark[index], rect, 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
                            m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);*/
                                for (long nSub = 0; nSub < nDivThreadSize; nSub++)
                                {
                                    m_EqualItem[n * nDivThreadSize + nSub]
                                        = new SEqualizationItems(this, m_EqualImage_Dark[index], rectSub[nSub], 128,
                                            1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
                                            m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
                                    AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n * nDivThreadSize + nSub]);
                                    nEqualCheck++;
                                }

                                /*m_EqualItem[n] = new SEqualizationItems(this, m_EqualImage_Dark[index], rect, 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
                            m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
                        AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n]);*/
                                bEqualThreadCheck = TRUE;
                                m_bEqualImageFlag_Dark[index] = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }

    //for (long n = 0; n < nNumInspectionBlob; n++)
    //   {
    //       bool bCheckInsp = m_para.m_vecBlobParaCondition[n].m_bUseInspection;
    //       if (bCheckInsp)
    //       {
    //           if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
    //           {
    //               long nDivThreadSize = m_para.m_vecBlobParaCondition[n].m_nEqualThreadCount;
    //               m_EqualItem.resize(nNumInspectionBlob * nDivThreadSize);

    //               // refresh roi information
    //               IPVM::Rect rect;
    //               long index = m_para.m_vecBlobParaCondition[n].m_nInspectionArea;
    //               rect = m_unionResult.m_rectAreaBlob[index];
    //               for (long nDiv = 0; nDiv < nDivThreadSize; nDiv++)
    //               {
    //                   rectSub[nDiv] = rect;
    //               }
    //               if (nDivThreadSize > 1)
    //               {
    //                   long nWidthSubSize = rect.Width() / nDivThreadSize;
    //                   for (long nDiv = 0; nDiv < nDivThreadSize; nDiv++)
    //                   {
    //                       rectSub[nDiv] = rect;
    //                   }
    //                   rectSub[0].m_right = rect.m_left + nWidthSubSize;
    //                   for (long nDiv = 1; nDiv < nDivThreadSize - 1; nDiv++)
    //                   {
    //                       rectSub[nDiv].m_left = rectSub[nDiv - 1].m_right;
    //                       rectSub[nDiv].m_right = rectSub[nDiv].m_left + nWidthSubSize;
    //                       //rectSub[nDiv].m_right = rectSub[nDiv].m_left + nWidthSubSize;
    //                   }

    //                   rectSub[nDivThreadSize - 1].m_left = rectSub[nDivThreadSize - 2].m_right;
    //               }

    //               if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)
    //               {
    //                   m_EqualImage_Bright[index].Create(
    //                       IPVM::Rect(m_inspectionImageBright).Width(), IPVM::Rect(m_inspectionImageBright).Height());

    //                   if (m_bEqualImageFlag_Bright[index] == FALSE)
    //                   {
    //                       IPVM::ImageProcessing::Copy(m_inspectionImageBright, rect, rect,
    //                           m_EqualImage_Bright[index]); //각 Rect영역에 대한 이미지 복사
    //                       if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
    //                       {
    //                           /*Equalization_TargetWeighted(m_EqualImage_Bright[index], rect, 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                           m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);*/
    //                           for (long nSub = 0; nSub < nDivThreadSize; nSub++)
    //                           {
    //                               m_EqualItem[n * nDivThreadSize + nSub]
    //                                   = new SEqualizationItems(this, m_EqualImage_Bright[index], rectSub[nSub], 128, 1.5f,
    //                                       m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                                       m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
    //                               AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n * nDivThreadSize + nSub]);
    //                               nEqualCheck++;
    //                           }
    //                           bEqualThreadCheck = TRUE;
    //                           m_bEqualImageFlag_Bright[index] = TRUE;
    //                       }
    //                   }
    //               }
    //               else
    //               {
    //                   m_EqualImage_Dark[index].Create(
    //                       IPVM::Rect(m_inspectionImageDark).Width(), IPVM::Rect(m_inspectionImageDark).Height());
    //                   if (m_bEqualImageFlag_Dark[index] == FALSE)
    //                   {
    //                       IPVM::ImageProcessing::Copy(m_inspectionImageDark, rect, rect,
    //                           m_EqualImage_Dark[index]); //각 Rect영역에 대한 이미지 복사
    //                       if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
    //                       {
    //                           /*Equalization_TargetWeighted(m_EqualImage_Dark[index], rect, 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                           m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);*/
    //                           for (long nSub = 0; nSub < nDivThreadSize; nSub++)
    //                           {
    //                               m_EqualItem[n * nDivThreadSize + nSub]
    //                                   = new SEqualizationItems(this, m_EqualImage_Dark[index], rectSub[nSub], 128, 1.5f,
    //                                       m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                                       m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
    //                               AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n * nDivThreadSize + nSub]);
    //                               nEqualCheck++;
    //                           }

    //                           /*m_EqualItem[n] = new SEqualizationItems(this, m_EqualImage_Dark[index], rect, 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax,
    //                           m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin, nEqualCheck);
    //                       AfxBeginThread(ThreadEqualizationSubFunc, m_EqualItem[n]);*/
    //                           bEqualThreadCheck = TRUE;
    //                           m_bEqualImageFlag_Dark[index] = TRUE;
    //                       }
    //                   }
    //               }
    //           }
    //       }
    //   }

    if (bEqualThreadCheck)
    {
        DWORD dwWaitEqualResult = ::WaitForMultipleObjects(nEqualCheck, m_Flag_EqualizationItemsReady, TRUE, INFINITE);
    }

    float fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    debugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[pre process(equalization)](ms)"));

    long nSumCheck = 0;
    for (long n = 0; n < nNumInspectionBlob; n++)
    {
        m_vecnBlobNum[n] = 0;
        bool bCheckInsp = m_para.m_vecBlobParaCondition[n].m_bUseInspection;

        if (bCheckInsp)
        {
            // refresh roi information
            IPVM::Rect rect;
            long index = m_para.m_vecBlobParaCondition[n].m_nInspectionArea;
            rect = m_unionResult.m_rectAreaBlob[index];

            switch (m_para.m_vecBlobParaCondition[n].m_nSetTypeInspArea)
            {
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::OffsetRoi:
                {
                }
                break;
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Left:
                {
                    rect.m_right = rect.m_left;
                }
                break;
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Right:
                {
                    rect.m_left = rect.m_right;
                }
                break;
            }

            CString strRectValue;
            CString strLeft, strTop, strRight, strBottom;

            IPVM::Rect offset = m_para.m_vecBlobParaCondition[n].m_rtOffsetRoi;
            rect.DeflateRect(offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);

            m_vecrtCal[n] = rect;

            /*IPVM::Image_8u_C1 CropImage;
			IPVM::Image_8u_C1 CropImage2;*/

            m_CropImage[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());

            m_CropImage2[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());

            m_CropImage3[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());
            m_CropImage4[n].Create(m_vecrtCal[n].Width(), m_vecrtCal[n].Height());

            m_BlobDatas[n].m_bufferimage = &m_CropImage[n];

            if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)
            {
                if (m_bEqualImageFlag_Bright[index] == TRUE)
                {
                    IPVM::ImageProcessing::Copy(m_EqualImage_Bright[index], m_vecrtCal[n], m_vecrtCal[n],
                        m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                }
                else
                {
                    IPVM::ImageProcessing::Copy(m_inspectionImageBright, m_vecrtCal[n], m_vecrtCal[n],
                        m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                }
                //IPVM::ImageProcessing::Copy(m_EqualImage_Bright[index], m_vecrtCal[n], m_vecrtCal[n],
                //    m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                /*if(m_para.m_vecBlobParaCondition[n].m_bUseEqual)
                {
					if(!m_para.m_vecBlobParaCondition[n].m_bUseEqualThreshold)
						Equalization_TargetWeighted(m_bufferThreshold, m_vecrtCal[n], 128, 1.5f); 
					else
						Equalization_TargetWeighted(m_bufferThreshold, m_vecrtCal[n], 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);
				}*/
                if (m_para.m_vecBlobParaCondition[n].m_nInspectionType == (long)ENUM_BLOB_TYPE_INSPTYPE::Defect2)
                {
                    //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);

                    //IPVM::ImageProcessing::FilterBox(
                    //    m_bufferThreshold2, m_vecrtCal[n], 3, 3, TRUE, m_bufferThreshold);
                    //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);

                    //IPVM::ImageProcessing::FilterBox(m_bufferThreshold2, m_vecrtCal[n], 3, 3, TRUE, m_bufferThreshold);

                    IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);
                    ImageProcessing_BoxFilter(m_bufferThreshold2, m_vecrtCal[n], m_bufferThreshold);
                }
                IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]),
                    m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::Mix)
                {
                    //IPVM::ImageProcessing::Copy(m_inspectionImageBright, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]), m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                    IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], IPVM::Rect(m_CropImage3[n]),
                        m_CropImage3[n]); //각 Rect영역에 대한 이미지 복사
                }
            }
            else
            {
                if (m_bEqualImageFlag_Dark[index] == TRUE)
                {
                    IPVM::ImageProcessing::Copy(m_EqualImage_Dark[index], m_vecrtCal[n], m_vecrtCal[n],
                        m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                }
                else
                {
                    IPVM::ImageProcessing::Copy(m_inspectionImageDark, m_vecrtCal[n], m_vecrtCal[n],
                        m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                }
                //IPVM::ImageProcessing::Copy(m_EqualImage_Dark[index], m_vecrtCal[n], m_vecrtCal[n],
                //    m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
                /*if (m_para.m_vecBlobParaCondition[n].m_bUseEqual)
                {
					if (!m_para.m_vecBlobParaCondition[n].m_bUseEqualThreshold)
						Equalization_TargetWeighted(m_bufferThreshold, m_vecrtCal[n], 128, 1.5f);
					else
						Equalization_TargetWeighted(m_bufferThreshold, m_vecrtCal[n], 128, 1.5f, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMax, m_para.m_vecBlobParaCondition[n].m_nEqualThresholdMin);
                }*/
                if (m_para.m_vecBlobParaCondition[n].m_nInspectionType == (long)ENUM_BLOB_TYPE_INSPTYPE::Defect2)
                {
                    //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);

                    //IPVM::ImageProcessing::FilterBox(
                    //    m_bufferThreshold2, m_vecrtCal[n], 3, 3, TRUE, m_bufferThreshold);
                    //IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);

                    //IPVM::ImageProcessing::FilterBox(m_bufferThreshold2, m_vecrtCal[n], 3, 3, TRUE, m_bufferThreshold);

                    IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], m_bufferThreshold2);
                    ImageProcessing_BoxFilter(m_bufferThreshold2, m_vecrtCal[n], m_bufferThreshold);
                }
                IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]),
                    m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::Mix)
                {
                    //IPVM::ImageProcessing::Copy(m_inspectionImageBright, m_vecrtCal[n], IPVM::Rect(m_CropImage2[n]), m_CropImage2[n]); //각 Rect영역에 대한 이미지 복사
                    IPVM::ImageProcessing::Copy(m_bufferThreshold, m_vecrtCal[n], IPVM::Rect(m_CropImage3[n]),
                        m_CropImage3[n]); //각 Rect영역에 대한 이미지 복사
                }
            }

            //  여기까지는 쓰레드로 돌리면 안된다. ROI 가 겹칠 위험이 있음.
            long nMaxBlobCnt = m_para.m_vecBlobParaCondition[n].m_nBlobMaxCount;
            //m_pvecsObjInfo[n] = new IPVM::BlobInfo[nMaxBlobCnt];
            m_pvecsObjInfo[n] = new CInspectionCustomBlobInfo[nMaxBlobCnt];

            long nInspIllum = (long)m_para.m_vecBlobParaCondition[n].m_nImageType;
            m_item[n] = new SInspectionItemsItem(this, m_CropImage2[n], m_CropImage[n], m_CropImage3[n],
                m_CropImage4[n], m_pvecsObjInfo[n], n, nInspIllum, nSumCheck, m_vecrtCal[n]);

            // ¾²·¡µå¿¡¼­ ÀÎ½ºÅÏÆ®ÀÎ½ÀÀ» ½ÇÇà
            AfxBeginThread(ThreadInspectionSubFunc, m_item[n]);
            nSumCheck++;

            //쓰레드 마무리 아래쪽은 웨이트 아래로 가야 한다... 그럴려면 블랍인포를 차례대로...
            /// /////////////
        }
    }
    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();

    debugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[pre process(etc)](ms)"));

    tc.Reset();
    DWORD dwWaitResult = ::WaitForMultipleObjects(nSumCheckCnt, m_Flag_InspectionItemsReady, TRUE, INFINITE);
    for (long n = 0; n < nNumInspectionBlob; n++)
    {
        ISICF::enInspectionImageType ImageType;
        ISICF::enDefectBrightness DefectBrigtness;

        if (m_para.m_vecBlobParaCondition[n].m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)
        {
            ImageType = ISICF::enInspectionImageType::Bright;
        }
        else
        {
            ImageType = ISICF::enInspectionImageType::Dark;
        }

        if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::White)
        {
            DefectBrigtness = ISICF::enDefectBrightness::DFB_WHITE;
        }
        else if (m_para.m_vecBlobParaCondition[n].m_nBlobBrightType == (long)_enumBlobThresholdBright::Black)
        {
            DefectBrigtness = ISICF::enDefectBrightness::DFB_BLACK;
        }
        else
        {
            DefectBrigtness = ISICF::enDefectBrightness::DFB_MIX;
        }

        auto& Defect = m_parent->m_ISICellAllDefect;
        Defect.Init(n, &m_inspectionImageBright, &m_inspectionImageDark);
        auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
        long nTempArea = 0;
        if (isi_spec.m_vecInspAreaSet.size() > n)
        {
            nTempArea = isi_spec.m_vecInspAreaSet[n];
        }
        m_BlobDatas[n].SetBlobDataInfo(m_unionResult.m_rectAreaBlob[n], nTempArea, DefectBrigtness, ImageType);
        m_BlobDatas[n].m_bufferBlobInfo = m_pvecsObjInfo[n]->sObjInfo;
        m_BlobDatas[n].m_BlobNum = m_vecnBlobNum[n];
        for (int m = 0; m < m_vecnBlobNum[n]; m++)
        {
            // 이거 있음 큰거 일때 이상하게 넣어져서 제거...
            /*if (m_pvecsObjInfo[n][m].m_roi.Width() > m_para.m_vecBlobParaCondition[n].m_fBlobMinLength_mm * 1000 / m_pixelToUmX
				|| m_pvecsObjInfo[n][m].m_roi.Height() > m_para.m_vecBlobParaCondition[n].m_fBlobMinLength_mm * 1000 / m_pixelToUmY)*/
            {
                m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_left
                    = m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_left + m_vecrtCal[n].m_left;
                m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_right
                    = m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_right + m_vecrtCal[n].m_left;
                m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_top
                    = m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_top + m_vecrtCal[n].m_top;
                m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_bottom
                    = m_pvecsObjInfo[n]->sObjInfo[m].m_roi.m_bottom + m_vecrtCal[n].m_top;

                m_pvecsObjInfo[n]->quardROI[m].m_fLBx = m_pvecsObjInfo[n]->quardROI[m].m_fLBx + m_vecrtCal[n].m_left;
                m_pvecsObjInfo[n]->quardROI[m].m_fLBy = m_pvecsObjInfo[n]->quardROI[m].m_fLBy + m_vecrtCal[n].m_top;
                m_pvecsObjInfo[n]->quardROI[m].m_fLTx = m_pvecsObjInfo[n]->quardROI[m].m_fLTx + m_vecrtCal[n].m_left;
                m_pvecsObjInfo[n]->quardROI[m].m_fLTy = m_pvecsObjInfo[n]->quardROI[m].m_fLTy + m_vecrtCal[n].m_top;
                m_pvecsObjInfo[n]->quardROI[m].m_fRTx = m_pvecsObjInfo[n]->quardROI[m].m_fRTx + m_vecrtCal[n].m_left;
                m_pvecsObjInfo[n]->quardROI[m].m_fRTy = m_pvecsObjInfo[n]->quardROI[m].m_fRTy + m_vecrtCal[n].m_top;
                m_pvecsObjInfo[n]->quardROI[m].m_fRBx = m_pvecsObjInfo[n]->quardROI[m].m_fRBx + m_vecrtCal[n].m_left;
                m_pvecsObjInfo[n]->quardROI[m].m_fRBy = m_pvecsObjInfo[n]->quardROI[m].m_fRBy + m_vecrtCal[n].m_top;

                m_unionResult.m_rectResult.push_back(m_pvecsObjInfo[n]->sObjInfo[m].m_roi);
                debugInfo->AddOverlay(
                    LanguageDef::Debug_SurfaceResult, m_pvecsObjInfo[n]->sObjInfo[m].m_roi, RGB(255, 0, 0));
            }
        }
    }
    GetDefectProperty(BLOB_COUNT);

    for (long n = 0; n < m_pvecsObjInfo.size(); n++)
    {
        delete m_pvecsObjInfo[n];
    }
    m_resultValue = II_RESULT_VALUE::PASS;

    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[all Thread Calc Time](ms)"));
#endif 

	return true;
}

void CInspectionBlob::GetDefectProperty(const int& nCount, const bool bResize, const int nRate, const IPVM::Image_8u_C1& ResizeImage)
{
	auto& Defect = m_parent->m_ISICellAllDefect;
	
	//IPVM::TimeCheck timeck;
	std::vector<float> ftime;
	CISIDefectProperty ISITemp;

    
    // Blob ROI 중복 제거
    // 1. Check Option
    if (m_para.m_bUseDeleteIntersectBlob == true)
    {
        std::vector<IPVM::Rect> vecROI1;
        std::vector<IPVM::Rect> vecROI2;

        long nParaSize = m_para.m_vecBlobParaCondition.size();

        for (long nIndex = 0; nIndex < nParaSize; nIndex++)
        {
            int nblobcount = m_BlobDatas[nIndex].m_BlobNum;
            long nInspArea = m_para.m_vecBlobParaCondition[nIndex].m_nInspectionArea;
            
            for (long nSub = nIndex + 1; nSub < nParaSize; nSub++)
            {
                int nblobcountSub = m_BlobDatas[nSub].m_BlobNum;
                long nInspAreaSub = m_para.m_vecBlobParaCondition[nSub].m_nInspectionArea;

                // 2. Compare to Inspection Area
                if (nInspArea == nInspAreaSub)
                {
                    vecROI1.clear();
                    vecROI2.clear();
                    
                    // 3. ROI vector 만들기
                    for (int nRoi = 0; nRoi < nblobcount; nRoi++)
                    {
                        IPVM::Rect InspRoi = m_BlobDatas[nIndex].m_bufferBlobInfo[nRoi].m_roi;
                        vecROI1.push_back(InspRoi);
                    }
                    for (int nRoi = 0; nRoi < nblobcountSub; nRoi++)
                    {
                        IPVM::Rect InspRoi = m_BlobDatas[nSub].m_bufferBlobInfo[nRoi].m_roi;
                        vecROI2.push_back(InspRoi);
                    }

                    // 4. 같은 ROI인지 비교
                    for (int nCmp1 = 0; nCmp1 < vecROI1.size(); nCmp1++)
                    {
                        for (int nCmp2 = 0; nCmp2 < vecROI2.size(); nCmp2++)
                        {
                            IPVM::Rect overlapRect = IPVM::Rect(0, 0, 0, 0);
                            if (overlapRect.IntersectRect(vecROI1[nCmp1], vecROI2[nCmp2]))
                            {
                                long CurArea = vecROI1[nCmp1].Width() * vecROI1[nCmp1].Height();
                                long SubArea = vecROI2[nCmp2].Width() * vecROI2[nCmp2].Height();
                                long nOverlapArea = overlapRect.Width() * overlapRect.Height();

                                float fIntersectArea = nOverlapArea / SubArea;

                                if (fIntersectArea > 0.8)
                                {
                                    m_BlobDatas[nSub].m_bufferBlobInfo[nCmp2].m_roi.SetRectEmpty();
                                    m_pvecsObjInfo[nSub]->quardROI[nCmp2] = {
                                        IPVM::Quadrangle_32f(IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0)),
                                    };
                                    m_BlobDatas[nSub].m_bufferBlobInfo[nCmp2].m_label = 0;
                                }
                                else
                                {
                                    if (vecROI1[nCmp1].m_left <= vecROI2[nCmp2].m_left && vecROI1[nCmp1].m_right >= vecROI2[nCmp2].m_right
                                        && vecROI1[nCmp1].m_top <= vecROI2[nCmp2].m_top && vecROI1[nCmp1].m_bottom <= vecROI2[nCmp2].m_bottom)
                                    {
                                        m_BlobDatas[nSub].m_bufferBlobInfo[nCmp2].m_roi.SetRectEmpty();
                                        
                                        m_pvecsObjInfo[nSub]->quardROI[nCmp2] = {
                                            IPVM::Quadrangle_32f(IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0), IPVM::Point_32f_C2(0, 0)),
                                        };
                                        m_BlobDatas[nSub].m_bufferBlobInfo[nCmp2].m_label = 0;
                                        
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    


	for (int blob = 0; blob < nCount; blob++)
	{
		IPVM::Image_8u_C1* image;
		if (ISICF::enInspectionImageType::Bright == m_BlobDatas[blob].m_ImageType)
		{
			image = &m_inspectionImageBright;
		}
		else
		{
			image = &m_inspectionImageDark;
		}

		int nblobcount = m_BlobDatas[blob].m_BlobNum;
		auto InspectionArea = m_BlobDatas[blob].m_InspectionArea;
        auto Rect = m_BlobDatas[blob].m_Rect;
		for (int index = 0; index < nblobcount; index++)
		{
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Step #1. Prepare Calculate to Blob Information Value
			const auto& LabelNum = m_BlobDatas[blob].m_bufferBlobInfo[index].m_label;
			IPVM::Rect InspRoi = m_BlobDatas[blob].m_bufferBlobInfo[index].m_roi;
			
			IPVM::Rect rtTemp = m_BlobDatas[blob].m_bufferBlobInfo[index].m_roi;
			auto& rtBlobRect = IPVM::ToMFC(m_BlobDatas[blob].m_bufferBlobInfo[index].m_roi);
			rtTemp.m_left = rtTemp.m_left - m_vecrtCal[blob].m_left;
			rtTemp.m_right = rtTemp.m_right - m_vecrtCal[blob].m_left;
			rtTemp.m_top = rtTemp.m_top - m_vecrtCal[blob].m_top;
			rtTemp.m_bottom = rtTemp.m_bottom - m_vecrtCal[blob].m_top;
			

#if INSPECTION_MACHINE_UC1
            rtTemp = m_BlobDatas[blob].m_bufferBlobInfo[index].m_roi;
            IPVM::Image_8u_C1 cbinaryimage(rtTemp.Width(), rtTemp.Height());
            IPVM::ImageProcessing::Copy(
                *m_BlobDatas[blob].m_bufferimage, rtTemp, IPVM::Rect(cbinaryimage), cbinaryimage);

            ISITemp.srcImage = cbinaryimage;

#else
            if (TRUE == m_parent->m_isTest)
            {
                IPVM::Image_8u_C1 cbinaryimage(rtTemp.Width(), rtTemp.Height());
                IPVM::ImageProcessing::Copy(
                    *m_BlobDatas[blob].m_bufferimage, rtTemp, IPVM::Rect(cbinaryimage), cbinaryimage);

                ISITemp.srcImage = cbinaryimage;
            }
#endif // INSPECTION_MACHINE_UC1

			CString strtemp;
			float fcontour = 0.0f;
		
			float	fSizeH = (long)rtBlobRect.Width() * m_pixelToUmX * 0.001f;
			float	fSizeV = (long)rtBlobRect.Height() * m_pixelToUmY * 0.001f;
            
            if (fSizeH <= 0 || fSizeV <= 0)
                continue;

			// 각도 계산상의 문제로 앞단으로 옮김..
            float fWidth = m_pvecsObjInfo[blob]->fWidth[index];
            //*m_pixelToUmX * 0.001f;
            float fLength = m_pvecsObjInfo[blob]->fLength[index];
            //*m_pixelToUmX * 0.001f;
			
			//IPVM::Rect rtArea = m_para.m_vecBlobParaCondition[blob].m_rtSetInspArea;

			
			//-- work      

            IPVM::Rect rect;
            long nAreaIndex = m_para.m_vecBlobParaCondition[blob].m_nInspectionArea;
            rect = m_unionResult.m_rectAreaBlob[nAreaIndex];

            switch (m_para.m_vecBlobParaCondition[blob].m_nSetTypeInspArea)
            {
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::OffsetRoi:
                {
                }
                break;
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Left:
                {
                    rect.m_right = rect.m_left;
                }
                break;
                case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Right:
                {
                    rect.m_left = rect.m_right;
                }
                break;
            }

            CString strRectValue;
            CString strLeft, strTop, strRight, strBottom;

            IPVM::Rect offset = m_para.m_vecBlobParaCondition[blob].m_rtOffsetRoi;
            rect.DeflateRect(offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);

            IPVM::Rect CalcRect = rect; 

            auto& align = *((CInspectionCellAlign*)m_parent->GetProcessingItem(ProcItemCategory::cellAlign));
            long nROISize = align.m_para.m_nLineCountBlob;
            //nAreaIndex
                        
           
            if (abs(InspRoi.m_left - CalcRect.m_left) <= 2.0f && nAreaIndex>0) // 2픽셀
            {
                ISITemp.IS_NEAR_EDGE = 1;
            }
            else if (abs(CalcRect.m_right - InspRoi.m_right) <= 2.0f && nAreaIndex < nROISize-2) // 2픽셀
            {
                ISITemp.IS_NEAR_EDGE = 1;
            }
            else
            {
                ISITemp.IS_NEAR_EDGE = 0;
            }

            float fLeftDis2Edge_mm = abs(InspRoi.m_left - CalcRect.m_left) * m_pixelToUmX * 0.001f;
            float fRightDis2Edge_mm = abs(CalcRect.m_right - InspRoi.m_right) * m_pixelToUmX * 0.001f;
            float fSmallerDis2Edge_mm = min(fLeftDis2Edge_mm, fRightDis2Edge_mm);


			long	nGVMin = (long)m_pvecsObjInfo[blob]->fGVMin[index];
			long	nGVMax = (long)m_pvecsObjInfo[blob]->fGVMax[index];
			long	nGVAvg = (long)m_pvecsObjInfo[blob]->fGVAvg[index];
			float	fRatioWhitePx = 0.f;
			float	fRatioBlackPx = 0.f;
			float	fRatioMixPx = 0.f;
			float	fRatioContrast = m_pvecsObjInfo[blob]->fEdgeEnergy[index];
			float	fedgeBlack = m_pvecsObjInfo[blob]->fEdgeEnergyBlack[index];
			float	fedgeWhite = m_pvecsObjInfo[blob]->fEdgeEnergyWhite[index];
			
            //2024.10.21 JJS EdgeSharpness 추가
            float fEdgeSharpness = m_pvecsObjInfo[blob]->fEdgeEnergySharpness[index];
			long nOver200GVPxCnt = (long)m_pvecsObjInfo[blob]->nOver200GVPxCnt[index];
            //long nOver200GVPxCnt = m_vecOver200GVPxCount[index];
            float fEdgeSharpnessWhite = m_pvecsObjInfo[blob]->fEdgeEnergySharpnessWhite[index];
            float fEdgeSharpnessBlack = m_pvecsObjInfo[blob]->fEdgeEnergySharpnessBlack[index];
			float	fAreaRatio = ((float)m_BlobDatas[blob].m_bufferBlobInfo[index].m_area / (float)(rtBlobRect.Width() * rtBlobRect.Height())) * 100.0f;

            float fGVMedian = m_pvecsObjInfo[blob]->fGVMedian[index];
            float fGVStdev = m_pvecsObjInfo[blob]->fGVStdev[index];
			if ((fSizeH * fSizeV) == 0)
			{
				fAreaRatio = 100.f;
			}

			
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Step #3. Blob Information Value Setting (to CISIDefectProperty)
			ISITemp.AREA = m_BlobDatas[blob].m_bufferBlobInfo[index].m_area * m_pixelToUmX * m_pixelToUmY * 0.001f * 0.001f;
			ISITemp.AREA_Ratio = fAreaRatio;
			//ISITemp.RATIO_AREA = fAreaRatio;
			ISITemp.MAX_GV = nGVMax;
			ISITemp.MIN_GV = nGVMin;
			ISITemp.rtRect_pixel = IPVM::FromMFC(rtBlobRect);
			ISITemp.AVG_GV = nGVAvg;
			ISITemp.EDGE_ENERGY = fRatioContrast; 
            ISITemp.EDGE_ENERGY_BLACK = fedgeBlack;
            ISITemp.EDGE_ENERGY_WHITE = fedgeWhite;
            ISITemp.EDGE_ENERGY_SHARPNESS = fEdgeSharpness;
            ISITemp.EDGE_ENERGY_SHARPNESS_WHITE = fEdgeSharpnessWhite;
            ISITemp.EDGE_ENERGY_SHARPNESS_BLACK = fEdgeSharpnessBlack;
            
            ISITemp.MEDIAN_GV = fGVMedian;
            ISITemp.STDEV_GV = fGVStdev;
            if (m_para.m_bUseSwapXY)
            {
                ISITemp.SIZE_X_mm = fWidth;
                ISITemp.SIZE_Y_mm = fLength;
			}
            else
            {
                ISITemp.SIZE_X_mm = fSizeH;
                ISITemp.SIZE_Y_mm = fSizeV;
			}

			ISITemp.HOR = fSizeH;
			ISITemp.VER = fSizeV;
			ISITemp.RATIO_HV = fSizeH/ fSizeV;
			ISITemp.RATIO_VH = fSizeV/ fSizeH;
            ISITemp.OVER_200_GV_PIXEL_COUNT = nOver200GVPxCnt;
            ISITemp.DISTANCE_TO_EDGE_mm = fSmallerDis2Edge_mm;

			if (0 != ISITemp.SIZE_X_mm)//비율 최대 100으로 한정
				ISITemp.RATIO_YX = ISITemp.SIZE_Y_mm / ISITemp.SIZE_X_mm;
			else
				ISITemp.RATIO_YX = 100.0f;

			if (0 != ISITemp.SIZE_Y_mm)//비율 최대 100으로 한정
				ISITemp.RATIO_XY = ISITemp.SIZE_X_mm / ISITemp.SIZE_Y_mm;
			else
				ISITemp.RATIO_XY = 100.0f;

			ISITemp.BRIGHT_AVG_GV = (long)m_pvecsObjInfo[blob]->fWhiteGVAvg[index]; //0;				//계산 로직 추가 필요
			ISITemp.DARK_AVG_GV = (long)m_pvecsObjInfo[blob]->fBlackGVAvg[index]; //0;				//계산 로직 추가 필요


			ISITemp.MASS_CENTER_GV_GV = m_pvecsObjInfo[blob]->fGVMassCenterAvg[index];

			ISITemp.LENGTH_mm = fLength;
            ISITemp.WIDTH_mm = fWidth;

			ISITemp.RATIO_WL = ISITemp.WIDTH_mm / ISITemp.LENGTH_mm;
			ISITemp.RATIO_LW = ISITemp.LENGTH_mm / ISITemp.WIDTH_mm;

			//float fHalfimageAngle = 0.0f;	
			//float ftemp3 = m_BlobDatas[blob].m_bufferBlobInfo[index].m_area;
			//float fTemp4 = m_pvecsObjInfo[blob]->fBoundaryDis[index];
			//float ftemp2 = m_pvecsObjInfo[blob]->fBoundaryDis[index] * m_pixelToUmX * 0.001f ;
			//float ftemp1 = (PI * 4 * ftemp3);
			////float ftemp2 = ((ISITemp.LENGTH_mm * ISITemp.WIDTH_mm) / 2 * (ISITemp.LENGTH_mm * ISITemp.WIDTH_mm) / 2);
			//float compact = ftemp1 / (fTemp4 * fTemp4)*100.f; //진원도 측정법
			//if (compact >= 100.0f)
			//{
			//	compact = 100.0f;
			//}
			ISITemp.COMPACT = 0;
			
			ISITemp.nCategory = m_BlobDatas[blob].m_category;
			ISITemp.nInspectionArea = m_BlobDatas[blob].m_InspectionArea;
			ISITemp.nInspectImage = (long)m_BlobDatas[blob].m_ImageType;
			ISITemp.nDefectBrightness = m_BlobDatas[blob].m_DefectColor;
			if (ISICF::enDefectBrightness::DFB_WHITE == ISITemp.nDefectBrightness)
			{
				ISITemp.SECTION_THRESHOLD = ISITemp.MAX_GV - ISITemp.AVG_GV;
				ISITemp.RATIO_WHITE = 100;
				ISITemp.RATIO_DARK = 0;
			}
			else if (ISICF::enDefectBrightness::DFB_BLACK == ISITemp.nDefectBrightness)
			{
				ISITemp.SECTION_THRESHOLD = ISITemp.AVG_GV - ISITemp.MIN_GV;
				ISITemp.RATIO_WHITE = 0;
				ISITemp.RATIO_DARK = 100;
			}
			else
			{
				ISITemp.SECTION_THRESHOLD = 0;

				ISITemp.RATIO_DARK = m_pvecsObjInfo[blob]->fRatioMix[index]*100;
				ISITemp.RATIO_WHITE = 100- ISITemp.RATIO_DARK;
				// 계산해서 들어가야 함
				/*ISITemp.RATIO_WHITE = 0;
				ISITemp.RATIO_DARK = 0;*/
			}

			ISITemp.quadROI = m_pvecsObjInfo[blob]->quardROI[index];
		
			Defect.m_vecCandidate.push_back(ISITemp);
		}
	}
	
	for (int i = 0; i < nCount; i++) // 다음 검사를 위해 blob 개수 초기화
	{
		m_BlobDatas[i].m_BlobNum = 0;
		IPVM::ImageProcessing::Fill(m_BlobDatas[i].m_Rect, 0, *m_BlobDatas[i].m_bufferimage);//사용한 부분만 0으로 초기화
	}
	//ftime.push_back(timeck.Elapsed_ms());
	//timeck.Reset();
}
//


float CInspectionBlob::GetSTD(const IPVM::Image_8u_C1& OriginImage, const IPVM::Rect& rtElectrode, const IPVM::Rect& Rect)
{
	IPVM::Rect rt = Rect;
	rt.InflateRect(15, 15);
	rt &= rtElectrode;

	int nArea = rt.Width() * rt.Height();

	if (0 == nArea)
		return 0;

	int nAroundSum = 0;
	std::vector<long> fvecAround;
	std::vector<long> fvecGVData;
	for (int y = rt.m_top; y <= rt.m_bottom; y++)
	{
		for (int x = rt.m_left; x <= rt.m_right; x++)
		{
			auto value = *OriginImage.GetMem(x, y);
			if (y == rt.m_top || y == rt.m_bottom || x == rt.m_left || x == rt.m_right)
			{
				fvecAround.push_back(value);

				nAroundSum += value;
			}
			fvecGVData.push_back(value);
		}
	}

	std::sort(fvecAround.begin(), fvecAround.end());
	float fAroundGV = fvecAround[fvecAround.size() * 0.5];

	std::sort(fvecGVData.begin(), fvecGVData.end());
	float fTopCnt = fvecGVData.size() * 0.01; //3%
	if (0 == fTopCnt)
		return 0;


	int nSum = 0;
	for (int i = 0; i <= fTopCnt; i++)
	{
		nSum += fvecGVData[i];
	}

	float fValidDefectGV = nSum / fTopCnt;

	return abs(fAroundGV - fValidDefectGV);


}

void CInspectionBlob::CalcAngle(const long SizeX_px, const long SizeY_px, const float fWidth_px, const float fLength_px, float& fRealAngle, float& fHalfImageAngle)
{
	bool bBiggerX = false; // y 가 길면 90 - angle
	float fCos = SizeY_px / fLength_px;

	if (SizeX_px > SizeY_px)
	{
		fCos = SizeX_px / fLength_px;
		bBiggerX = true;
	}

	if (fCos >= -1.0 && fCos <= 1.0) {

		if (true == bBiggerX)
			fHalfImageAngle = acos(fCos) * 180 / PI;
		else
			fHalfImageAngle = 90 - (acos(fCos) * 180 / PI);
	}
	else
		fHalfImageAngle = 0.0f;


	float fTan = SizeX_px / (SizeY_px * 2);

	fRealAngle = 90 - atan2(SizeX_px, SizeY_px * 2) * 180 / PI;

}

void CInspectionBlob::ImageProcessing_BoxFilter(IPVM::Image_8u_C1& imgSrc, IPVM::Rect& Rect, IPVM::Image_8u_C1& imgDst)
{
    IPVM::Rect rtCopyRect(Rect);

    IPVM::Rect rtExpandRoi(Rect);
    rtExpandRoi.m_left = 0;
    rtExpandRoi.m_top = 0;
    rtExpandRoi.m_right = Rect.Width() + 10;
    rtExpandRoi.m_bottom = Rect.Height() + 10;

    IPVM::Image_8u_C1 imgExpand, imgTempExpand;
    imgExpand.Create(rtExpandRoi.Width(), rtExpandRoi.Height());
    imgTempExpand.Create(rtExpandRoi.Width(), rtExpandRoi.Height());

    // Center Origin
    IPVM::Rect rtCopyRoi(Rect);
    rtCopyRoi.m_left = 5;
    rtCopyRoi.m_top = 5;
    rtCopyRoi.m_right = Rect.Width() + 5;
    rtCopyRoi.m_bottom = Rect.Height() + 5;

    IPVM::ImageProcessing::Copy(imgSrc, rtCopyRect, rtCopyRoi, imgExpand);

    // Left Expand
    rtCopyRect = Rect;
    rtCopyRect.m_right = Rect.m_left + 5;

    IPVM::Rect rtCopyLeftExpand(rtCopyRoi);
    rtCopyLeftExpand.m_left = 0;
    rtCopyLeftExpand.m_top = 5;
    rtCopyLeftExpand.m_right = 5;
    rtCopyLeftExpand.m_bottom = Rect.Height() + 5;

    IPVM::ImageProcessing::Copy(imgSrc, rtCopyRect, rtCopyLeftExpand, imgExpand);

    // Top Expand
    rtCopyRect = Rect;
    rtCopyRect.m_bottom = Rect.m_top + 5;

    IPVM::Rect rtCopyTopExpand(rtCopyRoi);
    rtCopyTopExpand.m_left = 5;
    rtCopyTopExpand.m_top = 0;
    rtCopyTopExpand.m_right = Rect.Width() + 5;
    rtCopyTopExpand.m_bottom = 5;

    IPVM::ImageProcessing::Copy(imgSrc, rtCopyRect, rtCopyTopExpand, imgExpand);

    // Right Expand
    rtCopyRect = Rect;
    rtCopyRect.m_left = rtCopyRect.m_right - 5;

    IPVM::Rect rtCopyRightExpand(rtCopyRoi);
    rtCopyRightExpand.m_left = Rect.Width() + 5;
    rtCopyRightExpand.m_top = 5;
    rtCopyRightExpand.m_right = Rect.Width() + 10;
    rtCopyRightExpand.m_bottom = Rect.Height() + 5;

    IPVM::ImageProcessing::Copy(imgSrc, rtCopyRect, rtCopyRightExpand, imgExpand);

    // Bottom Expand
    rtCopyRect = Rect;
    rtCopyRect.m_top = rtCopyRect.m_bottom - 5;

    IPVM::Rect rtCopyBottomExpand(rtCopyRoi);
    rtCopyBottomExpand.m_left = 5;
    rtCopyBottomExpand.m_top = Rect.Height() + 5;
    rtCopyBottomExpand.m_right = Rect.Width() + 5;
    rtCopyBottomExpand.m_bottom = Rect.Height() + 10;

    IPVM::ImageProcessing::Copy(imgSrc, rtCopyRect, rtCopyBottomExpand, imgExpand);

    IPVM::ImageProcessing::Copy(imgExpand, rtExpandRoi, imgTempExpand);
    IPVM::ImageProcessing::FilterBox(imgTempExpand, Rect, 9, 9, TRUE, imgExpand);
    IPVM::ImageProcessing::Copy(imgExpand, rtCopyRoi, Rect, imgSrc);

    imgExpand.Free();
    imgTempExpand.Free();
}


void CInspectionBlob::Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea,
    IPVM::Image_8u_C1& maskimage, const unsigned char& target, const float& weight, const unsigned char* arAverage)
{
    int nWidth = srcimage.GetSizeX();
    int nHeight = srcimage.GetSizeY();

    const unsigned char* arrAverageX = arAverage;
    for (int y = 0; y < nHeight; y++)
    {
        long nSize = y * srcimage.GetWidthBytes();
        long nSizeMask = y * maskimage.GetWidthBytes();
        for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
        {
            auto& Value = srcimage.GetMem()[nSize + x];
            auto& MaskValue = maskimage.GetMem()[nSizeMask + x];
            auto value = ((Value - arrAverageX[x]) * weight) + target;

            if (MaskValue != 255)
            {
                Value = 0;
                continue;
            }

            if (arrAverageX[x] == 0)
            {
                continue;
            }
            if (255 <= value)
                Value = 255;
            else if (0 >= value)
                Value = 0;
            else
                Value = value;
        }
    }
}

void CInspectionBlob::Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea,
    const unsigned char& target, const float& weight, const unsigned char* arAverage)
{
    int nWidth = srcimage.GetSizeX();
    int nHeight = srcimage.GetSizeY();
    const unsigned char* arrAverageX = arAverage;
    std::vector<float> temp;
    temp.resize(rtInspArea.m_right);
    for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
    {
        temp[x] = target / arrAverageX[x];
	}
    for (int y = 0; y < nHeight; y++)
    {
        long nSize = y * srcimage.GetWidthBytes();
        for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
        {
            auto& Value = srcimage.GetMem()[nSize + x];
            //auto value = ((Value - arrAverageX[x]) * weight) + target;
            auto value = Value * temp[x];
            if (255 <= value)
                Value = 255;
            else if (30 >= value)
                Value = 0;
            else
                Value = value;
        }
    }
}

void CInspectionBlob::Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea, const unsigned char& target, const float& weight)
{
    int nWidth = srcimage.GetSizeX();
    int nHeight = srcimage.GetSizeY();
   
   // std::vector<float> fTemp;
    std::vector < double> dTemp;
    std::vector<long> nTemp;
   // fTemp.resize(rtInspArea.m_right);
    dTemp.resize(rtInspArea.m_right,0.f);
    nTemp.resize(rtInspArea.m_right, 0);
    float fMean;
    //IPVM::ImageProcessing::GetMean(srcimage, rtInspArea, &fMean);
    for (int y = 0; y < nHeight; y++)
    {
        long nSize = y * srcimage.GetWidthBytes();
        
        for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
        {
            auto& Value = srcimage.GetMem()[nSize + x];
            if (Value)
            {
                dTemp[x] += Value;
                nTemp[x]++;
            }
            
        }
       
    }

    for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
    {
        dTemp[x] = dTemp[x] / nTemp[x];

        //temp[x] = target / dTemp[x];
    }
    for (int y = 0; y < nHeight; y++)
    {
        long nSize = y * srcimage.GetWidthBytes();
        for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
        {
            auto& Value = srcimage.GetMem()[nSize + x];
            auto value = ((Value - dTemp[x]) * weight) + target;
            //auto value = Value * temp[x];
            if (255 <= value)
                Value = 255;
            else if (30 >= value)
                Value = 0;
            else
                Value = value;
        }
    }
}

void CInspectionBlob::Equalization_TargetWeighted(IPVM::Image_8u_C1& srcimage, IPVM::Rect& rtInspArea, const unsigned char& target, const float& weight, const long thresholdMax, const long thresholdMin)
{
    // find horizontal modeGv 2ea
    vector<long> vHist(256);
    for (long& hist : vHist)
        hist = 0;
    long modeGv0 = -1;
    long modeGv1 = -1;
    {
        long y = (rtInspArea.m_bottom + rtInspArea.m_top) / 2;
        auto vHist0 = vHist;
        auto vHist1 = vHist;

        BYTE* pSrc = srcimage.GetMem(0, y);
        for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
        {
            BYTE src = pSrc[x];
            if (x / 893 % 2 == 0)
                vHist0[src]++;
            else
                vHist1[src]++;
        }

        long max0 = LONG_MIN;
        long max1 = LONG_MIN;
        for (long hist = 10; hist < 220; hist++) // 10미만 & 220이상 무시
        {
            if (vHist0[hist] > max0)
            {
                max0 = vHist0[hist];
                modeGv0 = hist; // peak1
            }
            if (vHist1[hist] > max1)
            {
                max1 = vHist1[hist];
                modeGv1 = hist; // peak2
            }
        }
    }

    int nWidth = srcimage.GetSizeX();
    int nHeight = srcimage.GetSizeY();

    // std::vector<float> fTemp;
    std::vector<double> dTemp;
    std::vector<long> nTemp;
    // fTemp.resize(rtInspArea.m_right);
    dTemp.resize(rtInspArea.m_right, 0.f);
    nTemp.resize(rtInspArea.m_right, 0);
    float fMean;
    //IPVM::ImageProcessing::GetMean(srcimage, rtInspArea, &fMean);
    for (int y = 0; y < nHeight; y++)
    {
        long nSize = y * srcimage.GetWidthBytes();

        for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
        {
            auto& Value = srcimage.GetMem()[nSize + x];
            if (Value)
            {
                // modeGv
                long modeGv = -1;
                if (x / 893 % 2 == 0)
                    modeGv = modeGv0;
                else
                    modeGv = modeGv1;

                if (modeGv == -1 || (Value < modeGv + thresholdMax && Value > modeGv - thresholdMin))
                {
                    dTemp[x] += Value;
                    nTemp[x]++;
                }
            }
        }
    }

    for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
    {
        dTemp[x] = dTemp[x] / nTemp[x];

        if (nTemp[x] == 0)
        {
            long modeGv = -1;
            if (x / 893 % 2 == 0)
                modeGv = modeGv0;
            else
                modeGv = modeGv1;
            dTemp[x] = modeGv;
        }
    }

    for (int y = 0; y < nHeight; y++)
    {
        long nSize = y * srcimage.GetWidthBytes();
        for (int x = rtInspArea.m_left; x < rtInspArea.m_right; x++)
        {
            auto& Value = srcimage.GetMem()[nSize + x];
            auto value = ((Value - dTemp[x]) * weight) + target;
            //auto value = Value * temp[x];
            if (255 <= value)
                Value = 255;
            else if (30 >= value)
                Value = 0;
            else
                Value = value;
        }
    }
}

void CInspectionBlob::CalcPixel2mm(const IPVM::Rect& Rect, const float fHalfImageAngle, const float fLocus, const float fThickness, float& SIZE_X_mm, float& SIZE_Y_mm, float& WIDTH_mm, float& LENGTH_mm, float& RATIO_YX, float& RATIO_XY, float& RATIO_LW, float& RATIO_WL)
{
	//비율 계산
	float fCos = fabs(cos(fHalfImageAngle));
	float fSin = fabs(sin(fHalfImageAngle));
	float fXRatio_ = (fCos) / (fCos + fSin);
	float fYRatio_ = (fSin) / (fCos + fSin);

	float bigger = max(fXRatio_, fYRatio_);
	float smaller = min(fXRatio_, fYRatio_);
	float fLengthResolution_ = 0.0f;
	float fWidthResolution_ = 0.0f;
	if (fHalfImageAngle > 45)
	{
		fLengthResolution_ = (m_pixelToUmY * bigger) + (m_pixelToUmX * smaller);
		fWidthResolution_ = (m_pixelToUmY * smaller) + (m_pixelToUmX * bigger);
	}
	else
	{
		fLengthResolution_ = (m_pixelToUmY * smaller) + (m_pixelToUmX * bigger);
		fWidthResolution_ = (m_pixelToUmY * bigger) + (m_pixelToUmX * smaller);
	}


	SIZE_X_mm = Rect.Width() * m_pixelToUmX * 0.001f;
	SIZE_Y_mm = Rect.Height() * m_pixelToUmY * 0.001f;

	if (0 != SIZE_X_mm)//비율 최대 100으로 한정
		RATIO_YX = SIZE_Y_mm / SIZE_X_mm;
	else
		RATIO_YX = 100.0f;

	if (0 != SIZE_Y_mm)//비율 최대 100으로 한정
		RATIO_XY = SIZE_X_mm / SIZE_Y_mm;
	else
		RATIO_XY = 100.0f;

	WIDTH_mm = fThickness * fWidthResolution_ * 0.001f;
	LENGTH_mm = fLocus * fLengthResolution_ * 0.001f;

	if (0 != WIDTH_mm)//비율 최대 100으로 한정
		RATIO_LW = LENGTH_mm / WIDTH_mm;
	else
		RATIO_LW = 100.0f;

	if (0 != LENGTH_mm)//비율 최대 100으로 한정
		RATIO_WL = WIDTH_mm / LENGTH_mm;
	else
		RATIO_WL = 100.0f;

}

UINT CInspectionBlob::ThreadEqualizationSubFunc(LPVOID pParam)
{
    SEqualizationItems* pData = (SEqualizationItems*)pParam;

    pData->pMain->ThreadEqualizationSub(pData);

    ::SetEvent(pData->pMain->m_Flag_EqualizationItemsReady[pData->nCheckNum]);

    delete pData;

    return 0;
}


UINT CInspectionBlob::ThreadInspectionSubFunc(LPVOID pParam)
{
	SInspectionItemsItem* pData = (SInspectionItemsItem*)pParam;

	pData->pMain->ThreadInspectionSub(pData);
	
	::SetEvent(pData->pMain->m_Flag_InspectionItemsReady[pData->nCheckNum]);

	//TRACE(_T(">>>>> ThreadInspectionSubFunc End : %d\n"), pData->nInspItemsThread);

	delete pData;

	return 0;
}

void CInspectionBlob::ThreadEqualizationSub(SEqualizationItems* pEqualItem)
{
    Equalization_TargetWeighted(pEqualItem->srcImage, pEqualItem->rtInspArea, pEqualItem->target, pEqualItem->weight, pEqualItem->thesholdMax, pEqualItem->thesholdMin);
}

void CInspectionBlob::ThreadInspectionSub(SInspectionItemsItem* pInspItem)
{
	//m_vecnBlobNum[pInspItem->nInspBlob] = InspBlob(pInspItem->CopyImage, pInspItem->InspImage, pInspItem->psObjInfo, pInspItem->nInspBlob, pInspItem->nInspIllum);
    m_vecnBlobNum[pInspItem->nInspBlob] = InspBlob(
        pInspItem->CopyImage, pInspItem->InspImage, pInspItem->CopyImage2, pInspItem->InspImage2, pInspItem->psObjinfo, pInspItem->nInspBlob, pInspItem->nInspIllum, pInspItem->rtROI);
}

//long CInspectionBlob::InspBlob(IPVM::Image_8u_C1 CopyImage, IPVM::Image_8u_C1 InspImage, IPVM::BlobInfo* psObjInfo, long nInspBlob, long nInspIllum)
long CInspectionBlob::InspBlob(IPVM::Image_8u_C1 CopyImage, IPVM::Image_8u_C1 InspImage, IPVM::Image_8u_C1 CopyImage2, IPVM::Image_8u_C1 InspImage2, CInspectionCustomBlobInfo* psObjInfo, long nInspBlob, long nInspIllum, IPVM::Rect rtROI)
{

	//쓰레스 홀드 하기전에 필터 적용할지?

	// make 이미지 같은거 해서? 
	// 원하는 필터 및 이런거 넣을수 있도록 수정? 

	// ROI 쪼개서 쓰레스홀드 할 내용
	// 쓰레스 홀드 종류따라 어떻게 할지

	//계산상 이름이 반대로...
    CString strtcName;
	IPVM::TimeCheck tc;
    float fElapsedTime;
    tc.Reset();

	long nVerDivSave = m_para.m_vecBlobParaCondition[nInspBlob].m_nThresholdHorDiv; 
	long nHorDivSave = m_para.m_vecBlobParaCondition[nInspBlob].m_nThresholdVerDiv;
	

#if INSPECTION_MACHINE_UC1
    IPVM::Rect rtOrig = rtROI;
#else
    IPVM::Rect rtOrig = IPVM::Rect(InspImage);
#endif // INSPECTION_MACHINE_UC1

	

	std::vector<IPVM::Rect> rtDiv;

	long nSizeOrigX = rtOrig.Width();
	long nSizeOrigY = rtOrig.Height();

	float fHorzSize = rtOrig.Height() / (float)nHorDivSave;
	float fVertSize = rtOrig.Width() / (float)nVerDivSave;

	int x, y, k;
	/*for (y = 0; y < nHorDivSave; y++)
	{
		for (x = 0; x < nVerDivSave; x++)
		{
			k = y * nVerDivSave + x;
			IPVM::Rect temp= IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f), (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f), (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
			rtDiv.push_back(temp);
		}
	}*/
    if (nVerDivSave == 999)
    {
        rtOrig = rtROI;
        float fSensorPosOffset = m_inspectionImageBright.GetSizeX() / 32;
        float fPosCheckY = rtOrig.CenterPoint().m_y;

        IPVM::Point_32f_C2 PtCheckingSensor;
        IPVM::Rect rtValidRect;
        long nValidCnt = 0;
        for (int cnt = 0; cnt < 32; cnt++)
        {
            PtCheckingSensor.m_x = fSensorPosOffset * cnt;
            PtCheckingSensor.m_y = fPosCheckY;
            if (rtOrig.PtInRect(PtCheckingSensor))
            {
                // Left 맞추기
                if (nValidCnt == 0)
                {
                    rtValidRect.m_left = rtOrig.m_left;
                }
                else
                {
                    rtValidRect.m_left = fSensorPosOffset * (cnt - 1);
                }

                rtValidRect.m_top = rtOrig.m_top;
                rtValidRect.m_bottom = rtOrig.m_bottom;
                rtValidRect.m_right = PtCheckingSensor.m_x;
                rtDiv.push_back(rtValidRect);
                nValidCnt++;
            }
            // Right 맞추기
            if (rtDiv.size()>1)
            {
                if (fSensorPosOffset * (cnt - 1) < rtOrig.m_right && rtOrig.m_right <= fSensorPosOffset * cnt)
                {
                    rtValidRect.m_left = rtDiv[nValidCnt - 1].m_right;
                    rtValidRect.m_top = rtOrig.m_top;
                    rtValidRect.m_bottom = rtOrig.m_bottom;
                    rtValidRect.m_right = rtOrig.m_right;
                    rtDiv.push_back(rtValidRect);
                    break;
                }
            }
           
        }

		for (long n = 0; n < rtDiv.size(); n++)
        {
            rtDiv[n].m_left = rtDiv[n].m_left - rtROI.m_left;
            rtDiv[n].m_right = rtDiv[n].m_right - rtROI.m_left;
            rtDiv[n].m_top = rtDiv[n].m_top - rtROI.m_top;
            rtDiv[n].m_bottom = rtDiv[n].m_bottom - rtROI.m_top;
		}
    }
    else
    {
        for (y = 0; y < nHorDivSave; y++)
        {
            for (x = 0; x < nVerDivSave; x++)
            {
                k = y * nVerDivSave + x;
                IPVM::Rect temp = IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f),
                    (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f),
                    (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
                rtDiv.push_back(temp);
            }
        }
    }
	
	float fAverage;
    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    //m_vecBlobTime.push_back(fElapsedTime);
    //nInspBlob
    strtcName.Format(_T("%d_[ROI seperate](ms)"), nInspBlob);
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
	
	if (m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::White)
	{
        if (m_para.m_vecBlobParaCondition[nInspBlob].m_bUseMeanRangeTheshold == true)
        {
            //		BinarizationGreater_Mean(CopyImage, IPVM::Rect(InspImage), m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
            for (long n = 0; n < rtDiv.size(); n++)
            {
                BinarizationGreater_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
            }
        }
        else
        {
            for (long n = 0; n < rtDiv.size(); n++)
            {
                BinarizationGreater_Mean_noRange(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
            }
        }
		
	}
	else if (m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::Black)
	{
		//BinarizationLess_Mean(CopyImage, IPVM::Rect(InspImage), m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
		/*for (long n = 0; n < rtDiv.size(); n++)
		{
			BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
			
		}*/
        if (m_para.m_vecBlobParaCondition[nInspBlob].m_bUseMeanRangeTheshold == true)
        {
            for (long n = 0; n < rtDiv.size(); n++)
            {
                //inspection->BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
                BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
            }
        }
        else
        {
            for (long n = 0; n < rtDiv.size(); n++)
            {
                //inspection->BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
                BinarizationLess_Mean_noRange(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
            }
        }     
	}
	else
	{
		//for (long n = 0; n < rtDiv.size(); n++)
		//{
		//	BinarizationLess_Mean(CopyImage2, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage2, fAverage);
		//	// 2 만들어서threshold 
		//}

        if (m_para.m_vecBlobParaCondition[nInspBlob].m_bUseMeanRangeTheshold == true)
        {
            for (long n = 0; n < rtDiv.size(); n++)
            {
                BinarizationGreater_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
            }

            for (long n = 0; n < rtDiv.size(); n++)
            {
                //inspection->BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
                BinarizationLess_Mean(CopyImage2, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage2, fAverage);
            }
        }
        else
        {
            for (long n = 0; n < rtDiv.size(); n++)
            {
                BinarizationGreater_Mean_noRange(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
            }

            for (long n = 0; n < rtDiv.size(); n++)
            {
                //inspection->BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
                BinarizationLess_Mean_noRange(CopyImage2, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage2, fAverage);
            }
        }
        
#if INSPECTION_MACHINE_UC1
        IPVM::ImageProcessing::BitwiseOr(InspImage, InspImage2, rtROI, CopyImage2);
        IPVM::ImageProcessing::Copy(CopyImage2, rtROI, InspImage);
#else
        IPVM::ImageProcessing::BitwiseOr(InspImage, InspImage2, IPVM::Rect(InspImage), CopyImage2);
        IPVM::ImageProcessing::Copy(CopyImage2, IPVM::Rect(InspImage), IPVM::Rect(InspImage), InspImage);
#endif // INSPECTION_MACHINE_UC1




		//for (long n = 0; n < rtDiv.size(); n++)
  //      {
  //          IPVM::Rect rtRoi = rtDiv[n];
  //          rtRoi &= IPVM::Rect(CopyImage);
  //          int nWidth = CopyImage.GetSizeX();
  //          int nHeight = CopyImage.GetSizeY();
  //          double dAverage = 0.0f;

  //          if (IPVM::Status::success != IPVM::ImageProcessing::GetMeanInRange(CopyImage, rtRoi, 10, 245, dAverage))
  //              ASSERT(1);

  //          long tempBlack = dAverage - m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2;
  //          long tempWhite = dAverage + m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold;
  //          if (tempBlack < 0)
  //              tempBlack = 1;
  //          else if (tempBlack > 254)
  //              tempBlack = 254;

  //          if (tempWhite < 0)
  //              tempWhite = 1;
  //          else if (tempWhite > 254)
  //              tempWhite = 254;
  //          int pnLutvalues[3] = {255, 0, 255};
  //          int pnLutLevels[4] = {0, tempBlack + 1, tempWhite, 256};
  //          int nLutLevels = 4;

  //          IPVM::ImageProcessing::ApplyLUT(CopyImage, rtRoi, pnLutvalues, pnLutLevels, nLutLevels, InspImage);
  //          
		//	
		//	/*for (long ny = rtRoi.m_top; ny < rtRoi.m_bottom; ny++)
  //          {
  //              BYTE* src_y = CopyImage.GetMem(0, ny);
  //              BYTE* dst_y = InspImage.GetMem(0, ny);
  //              
  //              for (long nx = rtRoi.m_left; nx < rtRoi.m_right; nx++)
  //          	{
  //          		if(src_y[nx] > tempReverse || src_y[nx] < temp)
  //          		{
  //          			dst_y[nx] = 255;                        
  //          		}
  //                  else
  //                  {
  //                      dst_y[nx] = 0;
  //                  }
  //  
  //          	}
  //          }*/
  //        
  //      }
       

		
		
	}
 
    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    //m_vecBlobTime.push_back(fElapsedTime);
    strtcName.Format(_T("%d_[threshold](ms)"), nInspBlob);
    m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
	long nBlobNum;
	long nMaxBlobCnt = m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMaxCount;
	
	m_BlobDatas[nInspBlob].m_bloblabelimage.Create(IPVM::Rect(InspImage).Width(), IPVM::Rect(InspImage).Height());
	long nBolbMinSizepx = m_para.m_vecBlobParaCondition[nInspBlob].m_fBlobMinSize_mm;
	if (-1 == nMaxBlobCnt)
	{
		nMaxBlobCnt = BLOB_NUM_MAX;
		m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMaxCount = nMaxBlobCnt;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step #1. Detect Blob
#if INSPECTION_MACHINE_UC1
    IPVM::AlgorithmObjectHandle<IPVM::BlobDetection> blobDetection;
    if (blobDetection->DetectBlob(InspImage, rtROI, true, nBolbMinSizepx, nMaxBlobCnt, psObjInfo->sObjInfo, nBlobNum,
            m_BlobDatas[nInspBlob].m_bloblabelimage)
        != IPVM::Status::success)
    {
        // fElapsedTime = tc.Elapsed_ms();
        // tc.Reset();
        //m_vecBlobTime.push_back(fElapsedTime);
        // strtcName.Format(_T("%d_[blob](ms)"), nInspBlob);
        // m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
        nBlobNum = 0;
        return nBlobNum;
    }
    // fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    //m_vecBlobTime.push_back(fElapsedTime);
    // strtcName.Format(_T("%d_[blob](ms)"), nInspBlob);
    //_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    if (nBlobNum == 0)
        return nBlobNum;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #2. Merge Blob (사용 유무 및 Link Distance는 외부 입력으로)
    // 머지를 거리로 하기 위험하다 가로세로 비율이 달라....
    //float fLinkDistance = 0.f;
    //
    //

    //m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance * 1000 / m_pixelToUmX; // 이거 픽셀로 해야 하나?
    //if (m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance != 0)
    //{
    //	blobDetection->BlobLink(InspImage, nBlobNum, m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance, psObjInfo->sObjInfo);
    //
    //}
    //
    //

    //// Filtering 된 BlobInfo 제거
    //blobDetection->BlobMerge(nBlobNum, m_BlobDatas[nInspBlob].m_bloblabelimage, psObjInfo->sObjInfo, nBlobNum);
    //   fElapsedTime = tc.Elapsed_ms();
    //   tc.Reset();
    //
    //  m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[머지 1](ms)"));

    // 옵션으로 겹치는거 머지하기 넣을때
    if (m_para.m_vecBlobParaCondition[nInspBlob].m_bUseIntersectMerge
        || m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance != 0)
    {
        long nSize = m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance * .5f;
        std::vector<long> vecrt1;
        std::vector<long> vecrt2;
        std::vector<std::vector<long>> vecvecInterSectROI;

        vecvecInterSectROI.resize(nBlobNum);
        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            vecvecInterSectROI[blob1].push_back(blob1);
            long backID = blob1;
            for (long blob2 = blob1 + 1; blob2 < nBlobNum; blob2++)
            {
                IPVM::Rect overlapRect = IPVM::Rect(0, 0, 0, 0);
                IPVM::Rect ROI1 = psObjInfo->sObjInfo[blob1].m_roi;
                IPVM::Rect ROI2 = psObjInfo->sObjInfo[blob2].m_roi;
                ROI1.InflateRect(nSize, nSize);
                ROI2.InflateRect(nSize, nSize);
                if (overlapRect.IntersectRect(ROI1, ROI2))
                {
                    vecrt1.push_back(blob1);
                    vecrt2.push_back(blob2);
                }
            }
        }
        if (vecrt1.size() > 0)
        {
            vecvecInterSectROI[vecrt1[0]].push_back(vecrt2[0]);
            for (long nIntersectSize = 1; nIntersectSize < vecrt1.size(); nIntersectSize++)
            {
                for (long blob1 = 0; blob1 < nBlobNum; blob1++)
                {
                    const long Size = vecvecInterSectROI[blob1].size();
                    for (long nInner = 0; nInner < Size; nInner++)
                    {
                        if (vecrt1[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                        {
                            BOOL bNotCheck = true;
                            for (long nInner = 0; nInner < Size; nInner++)
                            {
                                if (vecrt2[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                                    bNotCheck = false;
                            }
                            if (bNotCheck)
                            {
                                vecvecInterSectROI[blob1].push_back(vecrt2[nIntersectSize]);
                            }
                        }
                        if (vecrt2[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                        {
                            BOOL bNotCheck = true;
                            for (long nInner = 0; nInner < Size; nInner++)
                            {
                                if (vecrt1[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                                    bNotCheck = false;
                            }
                            if (bNotCheck)
                            {
                                vecvecInterSectROI[blob1].push_back(vecrt1[nIntersectSize]);
                            }
                        }
                    }
                }
            }
        }

        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            const long Size = vecvecInterSectROI[blob1].size();
            for (long nInner = 1; nInner < Size; nInner++)
            {
                vecvecInterSectROI[vecvecInterSectROI[blob1][nInner]].resize(0);
            }
            if (vecvecInterSectROI[blob1].size() < 2)
            {
                vecvecInterSectROI[blob1].resize(0);
            }
        }
        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            if (vecvecInterSectROI[blob1].size() > 0)
            {
                std::sort(vecvecInterSectROI[blob1].begin(), vecvecInterSectROI[blob1].end());
            }
        }

        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            if (vecvecInterSectROI[blob1].size() > 0)
            {
                long backID = vecvecInterSectROI[blob1][0];
                const long Size = vecvecInterSectROI[blob1].size();
                for (long nInner = 1; nInner < Size; nInner++)
                {
                    long blob2 = vecvecInterSectROI[blob1][nInner];
                    psObjInfo->sObjInfo[backID].m_nextLinkedBlobIndex = blob2;
                    psObjInfo->sObjInfo[blob2].m_previousLinkedBlobIndex = backID;
                    psObjInfo->sObjInfo[blob2].m_nextLinkedBlobIndex = -1;

                    backID = blob2;
                    //vecvecInterSectROI[vecvecInterSectROI[blob1][nInner]].resize(0);
                }
            }
        }

        blobDetection->BlobMerge(nBlobNum, m_BlobDatas[nInspBlob].m_bloblabelimage, psObjInfo->sObjInfo, nBlobNum);
        // fElapsedTime = tc.Elapsed_ms();
        // tc.Reset();
        //strtcName.Format(_T("%d_[fast merge](ms)"), nInspBlob);
        //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #3. Calculate Edge Energy
    // 아래 오른쪽 차이값 더한값 / 토탈 에어리어
    if (CProcessingBase::GetBlobEdgeEnergy(m_BlobDatas[nInspBlob].m_bloblabelimage, InspImage2, psObjInfo->sObjInfo,
            nBlobNum, CopyImage, sizeof(float), psObjInfo->fEdgeEnergy, psObjInfo->fEdgeEnergyWhite,
            psObjInfo->fEdgeEnergyBlack)
        != IPVM::Status::success)
    {
    }

    /*if (blobDetection->GetBlobEdgeEnergy(m_BlobDatas[nInspBlob].m_bloblabelimage,
		psObjInfo->sObjInfo,
		nBlobNum,
		InspImage,
		psObjInfo->fEdgeEnergy) != IPVM::Status::success)
	{
        *psObjInfo->fEdgeEnergy = 0.f;
	}*/
    // fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    //strtcName.Format(_T("%d_[edge energy](ms)"), nInspBlob);
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #4. Calculate Width/Length
    float fMag = m_pixelToUmY / m_pixelToUmX;

    //InspImage2//
    std::vector<float> fLTx;
    std::vector<float> fLTy;
    std::vector<float> fLBx;
    std::vector<float> fLBy;
    std::vector<float> fRTx;
    std::vector<float> fRTy;
    std::vector<float> fRBx;
    std::vector<float> fRBy;
    fLTx.resize(nBlobNum);
    fLTy.resize(nBlobNum);
    fLBx.resize(nBlobNum);
    fLBy.resize(nBlobNum);
    fRTx.resize(nBlobNum);
    fRTy.resize(nBlobNum);
    fRBx.resize(nBlobNum);
    fRBy.resize(nBlobNum);

    if (CProcessingBase::GetBlobWidthLength(m_BlobDatas[nInspBlob].m_bloblabelimage, InspImage, InspImage2,
            psObjInfo->sObjInfo, nBlobNum, sizeof(float), psObjInfo->fWidth, psObjInfo->fLength,
            psObjInfo->fBoundaryDis, &fLTx[0], &fLTy[0], &fLBx[0], &fLBy[0], &fRTx[0], &fRTy[0], &fRBx[0], &fRBy[0],
            CopyImage, psObjInfo->fEdgeEnergySharpness, fMag)
        != IPVM::Status::success)
    {
        *psObjInfo->fWidth = 0;
        *psObjInfo->fLength = 0;
    }
    for (long blob1 = 0; blob1 < nBlobNum; blob1++)
    {
        psObjInfo->quardROI[blob1].m_fLTx = fLTx[blob1];
        psObjInfo->quardROI[blob1].m_fLTy = fLTy[blob1];
        psObjInfo->quardROI[blob1].m_fLBx = fLBx[blob1];
        psObjInfo->quardROI[blob1].m_fLBy = fLBy[blob1];
        psObjInfo->quardROI[blob1].m_fRTx = fRTx[blob1];
        psObjInfo->quardROI[blob1].m_fRTy = fRTy[blob1];
        psObjInfo->quardROI[blob1].m_fRBx = fRBx[blob1];
        psObjInfo->quardROI[blob1].m_fRBy = fRBy[blob1];
    }

    // fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    //strtcName.Format(_T("%d_[Width Length](ms)"), nInspBlob);
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);

    /*
	if (blobDetection->GetBlobWidthLength(m_BlobDatas[nInspBlob].m_bloblabelimage,
		psObjInfo->sObjInfo,
		nBlobNum,
		psObjInfo->fWidth,
		psObjInfo->fLength) != IPVM::Status::success)
	{
		*psObjInfo->fWidth = 0;
		*psObjInfo->fLength = 0;
	}*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #5. Calculate Thickness & Locus
    /*if (blobDetection->GetBlobThicknessLocus(m_BlobDatas[nInspBlob].m_bloblabelimage,
		psObjInfo->sObjInfo,
		nBlobNum,
		psObjInfo->fAverageThickness,vp
		psObjInfo->fThickness,
		psObjInfo->fLocus) != IPVM::Status::success)
	{
		*psObjInfo->fAverageThickness = 0;
		*psObjInfo->fThickness = 0;
		*psObjInfo->fLocus = 0;
	}*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #6. Calculate BlobMassCenter
    if (m_para.m_vecBlobParaCondition[nInspBlob].m_nMassCenterGVsize > 0)
    {
        if (blobDetection->GetBlobMassCenter(m_BlobDatas[nInspBlob].m_bloblabelimage, psObjInfo->sObjInfo, nBlobNum,
                sizeof(m_BlobDatas[nInspBlob].m_bloblabelimage.GetMem()), psObjInfo->ptMassCenter)
            != IPVM::Status::success)
        {
            *psObjInfo->ptMassCenter = IPVM::Point_32f_C2(0, 0);
        }
    }

    // fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    //strtcName.Format(_T("%d_[mass center](ms)"), nInspBlob);
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #7. Calculate Gray Value (Max, Min, Avg) in Defect Area
    //int nLabelNum;
    //for (int idx = 0; idx < nBlobNum; idx++)
    //{
    //	CString str;
    //	str.Format(_T("d:temp%d.bmp"),idx);
    //	CopyImage.SaveBmp(str);
    //	str.Format(_T("d:tempL%d.bmp"), idx);
    //	m_BlobDatas[nInspBlob].m_bloblabelimage.SaveBmp(str);
    //	nLabelNum = psObjInfo->sObjInfo[idx].m_label;
    //	CProcessingBase::GetGrayValue(CopyImage,
    //		m_BlobDatas[nInspBlob].m_bloblabelimage,
    //		IPVM::Rect(m_BlobDatas[nInspBlob].m_bloblabelimage),
    //		//psObjInfo->sObjInfo[idx].m_roi,
    //		nLabelNum,
    //		psObjInfo->fGVMin[idx],
    //		psObjInfo->fGVMax[idx],
    //		psObjInfo->fGVAvg[idx],
    //		IPVM::Rect(psObjInfo->sObjInfo[idx].m_roi));

    if (CProcessingBase::GetBlobContrast(m_BlobDatas[nInspBlob].m_bloblabelimage, InspImage2, psObjInfo->sObjInfo,
            nBlobNum, CopyImage, 0, sizeof(float), psObjInfo->fGVMax, psObjInfo->fGVAvg, psObjInfo->fWhiteGVAvg,
            psObjInfo->fBlackGVAvg, psObjInfo->fGVMin, m_para.m_vecBlobParaCondition[nInspBlob].m_nMassCenterGVsize,
            psObjInfo->ptMassCenter, psObjInfo->fGVMassCenterAvg, psObjInfo->fRatioMix, psObjInfo->nOver200GVPxCnt,
            psObjInfo->fGVMedian, psObjInfo->fGVStdev)
        != IPVM::Status::success)
    {
    }
    // fElapsedTime = tc.Elapsed_ms();
    // tc.Reset();
    // strtcName.Format(_T("%d_[contrast](ms)"), nInspBlob);
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
#else
    IPVM::AlgorithmObjectHandle<IPVM::BlobDetection> blobDetection;
    if (blobDetection->DetectBlob(InspImage, IPVM::Rect(InspImage), true, nBolbMinSizepx, nMaxBlobCnt,
            psObjInfo->sObjInfo, nBlobNum, m_BlobDatas[nInspBlob].m_bloblabelimage)
        != IPVM::Status::success)
    {
        fElapsedTime = tc.Elapsed_ms();
        tc.Reset();
        //m_vecBlobTime.push_back(fElapsedTime);
        strtcName.Format(_T("%d_[blob](ms)"), nInspBlob);
        m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
        nBlobNum = 0;
        return nBlobNum;
    }

    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    //m_vecBlobTime.push_back(fElapsedTime);
    strtcName.Format(_T("%d_[blob](ms)"), nInspBlob);
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    if (nBlobNum == 0)
        return nBlobNum;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #2. Merge Blob (사용 유무 및 Link Distance는 외부 입력으로)
    // 머지를 거리로 하기 위험하다 가로세로 비율이 달라....
    //float fLinkDistance = 0.f;
    //
    //

    //m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance * 1000 / m_pixelToUmX; // 이거 픽셀로 해야 하나?
    //if (m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance != 0)
    //{
    //	blobDetection->BlobLink(InspImage, nBlobNum, m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance, psObjInfo->sObjInfo);
    //
    //}
    //
    //

    //// Filtering 된 BlobInfo 제거
    //blobDetection->BlobMerge(nBlobNum, m_BlobDatas[nInspBlob].m_bloblabelimage, psObjInfo->sObjInfo, nBlobNum);
    //   fElapsedTime = tc.Elapsed_ms();
    //   tc.Reset();
    //
    //  m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, _T("[머지 1](ms)"));
    // 옵션으로 겹치는거 머지하기 넣을때
    if (m_para.m_vecBlobParaCondition[nInspBlob].m_bUseIntersectMerge
        || m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance != 0)
    {
        long nSize = m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobMergeDistance * .5f;
        std::vector<long> vecrt1;
        std::vector<long> vecrt2;
        std::vector<std::vector<long>> vecvecInterSectROI;

        vecvecInterSectROI.resize(nBlobNum);
        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            vecvecInterSectROI[blob1].push_back(blob1);
            long backID = blob1;
            for (long blob2 = blob1 + 1; blob2 < nBlobNum; blob2++)
            {
                IPVM::Rect overlapRect = IPVM::Rect(0, 0, 0, 0);
                IPVM::Rect ROI1 = psObjInfo->sObjInfo[blob1].m_roi;
                IPVM::Rect ROI2 = psObjInfo->sObjInfo[blob2].m_roi;
                ROI1.InflateRect(nSize, nSize);
                ROI2.InflateRect(nSize, nSize);
                if (overlapRect.IntersectRect(ROI1, ROI2))
                {
                    vecrt1.push_back(blob1);
                    vecrt2.push_back(blob2);
                }
            }
        }
        if (vecrt1.size() > 0)
        {
            vecvecInterSectROI[vecrt1[0]].push_back(vecrt2[0]);
            for (long nIntersectSize = 1; nIntersectSize < vecrt1.size(); nIntersectSize++)
            {
                for (long blob1 = 0; blob1 < nBlobNum; blob1++)
                {
                    const long Size = vecvecInterSectROI[blob1].size();
                    for (long nInner = 0; nInner < Size; nInner++)
                    {
                        if (vecrt1[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                        {
                            BOOL bNotCheck = true;
                            for (long nInner = 0; nInner < Size; nInner++)
                            {
                                if (vecrt2[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                                    bNotCheck = false;
                            }
                            if (bNotCheck)
                            {
                                vecvecInterSectROI[blob1].push_back(vecrt2[nIntersectSize]);
                            }
                        }
                        if (vecrt2[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                        {
                            BOOL bNotCheck = true;
                            for (long nInner = 0; nInner < Size; nInner++)
                            {
                                if (vecrt1[nIntersectSize] == vecvecInterSectROI[blob1][nInner])
                                    bNotCheck = false;
                            }
                            if (bNotCheck)
                            {
                                vecvecInterSectROI[blob1].push_back(vecrt1[nIntersectSize]);
                            }
                        }
                    }
                }
            }
        }

        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            const long Size = vecvecInterSectROI[blob1].size();
            for (long nInner = 1; nInner < Size; nInner++)
            {
                vecvecInterSectROI[vecvecInterSectROI[blob1][nInner]].resize(0);
            }
            if (vecvecInterSectROI[blob1].size() < 2)
            {
                vecvecInterSectROI[blob1].resize(0);
            }
        }
        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            if (vecvecInterSectROI[blob1].size() > 0)
            {
                std::sort(vecvecInterSectROI[blob1].begin(), vecvecInterSectROI[blob1].end());
            }
        }

        for (long blob1 = 0; blob1 < nBlobNum; blob1++)
        {
            if (vecvecInterSectROI[blob1].size() > 0)
            {
                long backID = vecvecInterSectROI[blob1][0];
                const long Size = vecvecInterSectROI[blob1].size();
                for (long nInner = 1; nInner < Size; nInner++)
                {
                    long blob2 = vecvecInterSectROI[blob1][nInner];
                    psObjInfo->sObjInfo[backID].m_nextLinkedBlobIndex = blob2;
                    psObjInfo->sObjInfo[blob2].m_previousLinkedBlobIndex = backID;
                    psObjInfo->sObjInfo[blob2].m_nextLinkedBlobIndex = -1;

                    backID = blob2;
                    //vecvecInterSectROI[vecvecInterSectROI[blob1][nInner]].resize(0);
                }
            }
        }

        blobDetection->BlobMerge(nBlobNum, m_BlobDatas[nInspBlob].m_bloblabelimage, psObjInfo->sObjInfo, nBlobNum);
        fElapsedTime = tc.Elapsed_ms();
        tc.Reset();
        strtcName.Format(_T("%d_[fast merge](ms)"), nInspBlob);
        m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #3. Calculate Edge Energy
    // 아래 오른쪽 차이값 더한값 / 토탈 에어리어
    if (CProcessingBase::GetBlobEdgeEnergy(m_BlobDatas[nInspBlob].m_bloblabelimage, InspImage2, psObjInfo->sObjInfo,
            nBlobNum, CopyImage, sizeof(float), psObjInfo->fEdgeEnergy, psObjInfo->fEdgeEnergyWhite,
            psObjInfo->fEdgeEnergyBlack)
        != IPVM::Status::success)
    {
    }

    /*if (blobDetection->GetBlobEdgeEnergy(m_BlobDatas[nInspBlob].m_bloblabelimage,
		psObjInfo->sObjInfo,
		nBlobNum,
		InspImage,
		psObjInfo->fEdgeEnergy) != IPVM::Status::success)
	{
        *psObjInfo->fEdgeEnergy = 0.f;
	}*/
    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    strtcName.Format(_T("%d_[edge energy](ms)"), nInspBlob);
    m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #4. Calculate Width/Length
    float fMag = m_pixelToUmY / m_pixelToUmX;

    //InspImage2//
    std::vector<float> fLTx;
    std::vector<float> fLTy;
    std::vector<float> fLBx;
    std::vector<float> fLBy;
    std::vector<float> fRTx;
    std::vector<float> fRTy;
    std::vector<float> fRBx;
    std::vector<float> fRBy;
    fLTx.resize(nBlobNum);
    fLTy.resize(nBlobNum);
    fLBx.resize(nBlobNum);
    fLBy.resize(nBlobNum);
    fRTx.resize(nBlobNum);
    fRTy.resize(nBlobNum);
    fRBx.resize(nBlobNum);
    fRBy.resize(nBlobNum);

    if (CProcessingBase::GetBlobWidthLength(m_BlobDatas[nInspBlob].m_bloblabelimage, InspImage, InspImage2,
            psObjInfo->sObjInfo, nBlobNum, sizeof(float), psObjInfo->fWidth, psObjInfo->fLength,
            psObjInfo->fBoundaryDis, &fLTx[0], &fLTy[0], &fLBx[0], &fLBy[0], &fRTx[0], &fRTy[0], &fRBx[0], &fRBy[0],
            CopyImage, psObjInfo->fEdgeEnergySharpness, fMag)
        != IPVM::Status::success)
    {
        *psObjInfo->fWidth = 0;
        *psObjInfo->fLength = 0;
    }
    for (long blob1 = 0; blob1 < nBlobNum; blob1++)
    {
        psObjInfo->quardROI[blob1].m_fLTx = fLTx[blob1];
        psObjInfo->quardROI[blob1].m_fLTy = fLTy[blob1];
        psObjInfo->quardROI[blob1].m_fLBx = fLBx[blob1];
        psObjInfo->quardROI[blob1].m_fLBy = fLBy[blob1];
        psObjInfo->quardROI[blob1].m_fRTx = fRTx[blob1];
        psObjInfo->quardROI[blob1].m_fRTy = fRTy[blob1];
        psObjInfo->quardROI[blob1].m_fRBx = fRBx[blob1];
        psObjInfo->quardROI[blob1].m_fRBy = fRBy[blob1];
    }
    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    strtcName.Format(_T("%d_[Width Length](ms)"), nInspBlob);
    m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);

    if (m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::Mix)
    {
        if (CProcessingBase::GetBlobEdgeSharpness(m_BlobDatas[nInspBlob].m_bloblabelimage, InspImage, InspImage2,
                psObjInfo->sObjInfo, nBlobNum, sizeof(float), CopyImage, psObjInfo->fEdgeEnergySharpnessWhite,
                psObjInfo->fEdgeEnergySharpnessBlack)
            != IPVM::Status::success)
        {
            *psObjInfo->fEdgeEnergySharpness = 0;
            *psObjInfo->fEdgeEnergySharpnessWhite = 0;
            *psObjInfo->fEdgeEnergySharpnessBlack = 0;
        }
    }
    else if (m_para.m_vecBlobParaCondition[nInspBlob].m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::White)
    {
        {
            //*psObjInfo->fEdgeEnergySharpness = 0;
            *psObjInfo->fEdgeEnergySharpnessWhite = *psObjInfo->fEdgeEnergySharpness;
            //*psObjInfo->fEdgeEnergySharpnessBlack = 0;
        }
    }
    else
    {
        *psObjInfo->fEdgeEnergySharpnessBlack = *psObjInfo->fEdgeEnergySharpness;
    }
    //fElapsedTime = tc.Elapsed_ms();
    //tc.Reset();
    //strtcName.Format(_T("%d_[Edge Sharpness](ms)"), nInspBlob);
    //m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);

    /*
	if (blobDetection->GetBlobWidthLength(m_BlobDatas[nInspBlob].m_bloblabelimage,
		psObjInfo->sObjInfo,
		nBlobNum,
		psObjInfo->fWidth,
		psObjInfo->fLength) != IPVM::Status::success)
	{
		*psObjInfo->fWidth = 0;
		*psObjInfo->fLength = 0;
	}*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #5. Calculate Thickness & Locus
    /*if (blobDetection->GetBlobThicknessLocus(m_BlobDatas[nInspBlob].m_bloblabelimage,
		psObjInfo->sObjInfo,
		nBlobNum,
		psObjInfo->fAverageThickness,vp
		psObjInfo->fThickness,
		psObjInfo->fLocus) != IPVM::Status::success)
	{
		*psObjInfo->fAverageThickness = 0;
		*psObjInfo->fThickness = 0;
		*psObjInfo->fLocus = 0;
	}*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #6. Calculate BlobMassCenter
    if (m_para.m_vecBlobParaCondition[nInspBlob].m_nMassCenterGVsize > 0)
    {
        if (blobDetection->GetBlobMassCenter(m_BlobDatas[nInspBlob].m_bloblabelimage, psObjInfo->sObjInfo, nBlobNum,
                sizeof(m_BlobDatas[nInspBlob].m_bloblabelimage.GetMem()), psObjInfo->ptMassCenter)
            != IPVM::Status::success)
        {
            *psObjInfo->ptMassCenter = IPVM::Point_32f_C2(0, 0);
        }
    }

    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    strtcName.Format(_T("%d_[mass center](ms)"), nInspBlob);
    m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step #7. Calculate Gray Value (Max, Min, Avg) in Defect Area
    //int nLabelNum;
    //for (int idx = 0; idx < nBlobNum; idx++)
    //{
    //	CString str;
    //	str.Format(_T("d:temp%d.bmp"),idx);
    //	CopyImage.SaveBmp(str);
    //	str.Format(_T("d:tempL%d.bmp"), idx);
    //	m_BlobDatas[nInspBlob].m_bloblabelimage.SaveBmp(str);
    //	nLabelNum = psObjInfo->sObjInfo[idx].m_label;
    //	CProcessingBase::GetGrayValue(CopyImage,
    //		m_BlobDatas[nInspBlob].m_bloblabelimage,
    //		IPVM::Rect(m_BlobDatas[nInspBlob].m_bloblabelimage),
    //		//psObjInfo->sObjInfo[idx].m_roi,
    //		nLabelNum,
    //		psObjInfo->fGVMin[idx],
    //		psObjInfo->fGVMax[idx],
    //		psObjInfo->fGVAvg[idx],
    //		IPVM::Rect(psObjInfo->sObjInfo[idx].m_roi));

    if (CProcessingBase::GetBlobContrast(m_BlobDatas[nInspBlob].m_bloblabelimage, InspImage2, psObjInfo->sObjInfo,
            nBlobNum, CopyImage, 0, sizeof(float), psObjInfo->fGVMax, psObjInfo->fGVAvg, psObjInfo->fWhiteGVAvg,
            psObjInfo->fBlackGVAvg, psObjInfo->fGVMin, m_para.m_vecBlobParaCondition[nInspBlob].m_nMassCenterGVsize,
            psObjInfo->ptMassCenter, psObjInfo->fGVMassCenterAvg, psObjInfo->fRatioMix, psObjInfo->nOver200GVPxCnt, psObjInfo->fGVMedian, psObjInfo->fGVStdev)
        != IPVM::Status::success)
    {
    }
    fElapsedTime = tc.Elapsed_ms();
    tc.Reset();
    strtcName.Format(_T("%d_[contrast](ms)"), nInspBlob);
    m_DebugInfo->AddText(LanguageDef::Debug_InspTime, fElapsedTime, strtcName);
#endif // INSPECTION_MACHINE_UC1

	
	return nBlobNum;
}

void CInspectionBlob::ProcInspectionEnd(Inspection::DebugInfo* debugInfo)
{
    if (FALSE == m_parent->m_isTest)
        return;

    debugInfo->SetImage(LanguageDef::Debug_Equlization, m_bufferThreshold);
    //debugInfo->SetImage(LanguageDef::Debug_TestImage1, m_bufferThreshold);
    debugInfo->SetImage(LanguageDef::Debug_TestImage2, m_bufferThreshold2);
    debugInfo->SetImage(LanguageDef::Debug_TestImage3, m_bufferThreshold3);
    debugInfo->SetImage(LanguageDef::Debug_TestImage4, m_bufferThreshold4);
    debugInfo->SetImage(LanguageDef::Debug_TestImage5, m_bufferThreshold5);







}
void CInspectionBlob::LinkDataBase(bool bSave, Database& dbJob)
{
	m_para.LinkDataBase(bSave, dbJob);
}

bool CInspectionBlob::CopyDataBase(CProcessingBase* src)
{
	auto* ptr = dynamic_cast<CInspectionBlob*>(src);
	if (ptr == nullptr)
	{
		ASSERT(!_T("같은 타입의 Inspection만 복사가 가능하다"));
		return false;
	}

	auto& rhs = *ptr;

	m_pixelToUmX = rhs.m_pixelToUmX;
	m_pixelToUmY = rhs.m_pixelToUmY;

	m_para = rhs.m_para;

	m_isUp = rhs.m_isUp;

	return true;
}

