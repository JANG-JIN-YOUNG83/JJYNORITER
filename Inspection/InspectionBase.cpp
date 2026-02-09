#include "stdafx.h"
#include "InspectionBase.h"
#include "DebugInfo.h"

#include "Algorithm/AlgorithmObjectHandle.h"
#include "Algorithm/BlobDetection.h"
#include "Algorithm/ImageProcessing.h"
#include "Algorithm/Mathematics.h"
#include "Algorithm/Geometry.h"
#include "Base/basedef.h"
#include "Gadget/TimeCheck.h"
#include <Types/LineEq_32f.h>
#include <Algorithm/DataFitting.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ProcessingBase
CProcessingBase::CProcessingBase(CProcessingBase* parent, const ProcItemCategory itemCategory, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: m_logger(logger)
	, m_loggerResult(loggerResult)
	, m_itemCategory(itemCategory)
	, m_unionResult(parent->m_unionResult)
{
	m_imageSizeX = 0;
	m_imageSizeY = 0;

	m_resultValue = II_RESULT_VALUE::NOT_MEASURED;
    for (long nAngle=0; nAngle<180; nAngle++)
    {
        m_fCalcCos[nAngle] = cos(nAngle * (3.141592 / 180));
        m_fCalcSin[nAngle] = sin(nAngle * (3.141592 / 180));

		m_fCalcCosMin[nAngle] = cos(-nAngle * (3.141592 / 180));
        m_fCalcSinMin[nAngle] = sin(-nAngle * (3.141592 / 180));

    }
	
}

CProcessingBase::CProcessingBase(UnionCellResult& result, const ProcItemCategory itemCategory, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: m_logger(logger)
	, m_loggerResult(loggerResult)
	, m_itemCategory(itemCategory)
	, m_unionResult(result)
{
	m_imageSizeX = 0;
	m_imageSizeY = 0;

	m_resultValue = II_RESULT_VALUE::NOT_MEASURED;
}

CProcessingBase::~CProcessingBase()
{
}

bool CProcessingBase::DoInspection(Inspection::DebugInfo* debugInfo)
{
	if (debugInfo->GetModuleCategory() != ProcItemCategory::END &&
		m_itemCategory == debugInfo->GetModuleCategory())
	{
		InitDebugInfo(debugInfo);
	}
	//ResetSurface();
	bool success = ProcInspection(debugInfo);

	if (debugInfo->GetModuleCategory() != ProcItemCategory::END &&
		m_itemCategory == debugInfo->GetModuleCategory())
	{
		debugInfo->SetRejectInfo(m_rejectInfo);

		ProcInspectionEnd(debugInfo);
		
		auto* resultInfo = debugInfo->FindItem(LanguageDef::Result);
		if (resultInfo != nullptr && resultInfo->m_resultText.IsEmpty())
		{
			debugInfo->SetText(LanguageDef::Result, GetInspResultString());
			debugInfo->AddOverlay(LanguageDef::Result, m_rejectInfo);
			switch (m_resultValue)
			{
			case II_RESULT_VALUE::PASS:			debugInfo->AddOverlayText(LanguageDef::Result, IPVM::Point_32f_C2(50, 10), _T("Pass"), RGB(0, 255, 0), 300); break;
			case II_RESULT_VALUE::REWORK:		debugInfo->AddOverlayText(LanguageDef::Result, IPVM::Point_32f_C2(50, 10), _T("Rework"), RGB(255, 255, 0), 300); break;
			case II_RESULT_VALUE::REJECT:		debugInfo->AddOverlayText(LanguageDef::Result, IPVM::Point_32f_C2(50, 10), _T("Reject"), RGB(255, 0, 0), 300); break;
			case II_RESULT_VALUE::INVALID:		debugInfo->AddOverlayText(LanguageDef::Result, IPVM::Point_32f_C2(50, 10), _T("Invalid"), RGB(0, 0, 255), 300);  break;
			case II_RESULT_VALUE::NOT_MEASURED:	debugInfo->AddOverlayText(LanguageDef::Result, IPVM::Point_32f_C2(50, 10), _T("Not Measured"), RGB(100, 100,100), 300); break;
			}
			

		}
	}

	return success;
}

void CProcessingBase::InitDebugInfo(Inspection::DebugInfo* debugInfo)
{
	debugInfo->Reset();
	debugInfo->AddItem(LanguageDef::Result);

	PrepareDebugInfo(debugInfo);
}


void CProcessingBase::ResetImageBuffer(bool resetTapThreshold)
{
	ZeroMemory(m_bufferThreshold.GetMem(), m_bufferThreshold.GetSizeX() * m_bufferThreshold.GetSizeY());
	ZeroMemory(m_bufferThreshold2.GetMem(), m_bufferThreshold2.GetSizeX() * m_bufferThreshold2.GetSizeY());
	ZeroMemory(m_bufferThreshold3.GetMem(), m_bufferThreshold3.GetSizeX() * m_bufferThreshold3.GetSizeY());
	ZeroMemory(m_bufferThreshold4.GetMem(), m_bufferThreshold4.GetSizeX() * m_bufferThreshold4.GetSizeY());
	ZeroMemory(m_bufferThreshold5.GetMem(), m_bufferThreshold5.GetSizeX() * m_bufferThreshold5.GetSizeY());
	ZeroMemory(m_bufferThreshold6.GetMem(), m_bufferThreshold6.GetSizeX() * m_bufferThreshold6.GetSizeY());
	ZeroMemory(m_bufferLabel.GetMem(), m_bufferLabel.GetSizeX() * m_bufferLabel.GetSizeY());
	/*ZeroMemory(m_bufferLabelRoi1.GetMem(), m_bufferLabelRoi1.GetSizeX() * m_bufferLabelRoi1.GetSizeY());
	ZeroMemory(m_bufferLabelRoi2.GetMem(), m_bufferLabelRoi2.GetSizeX() * m_bufferLabelRoi2.GetSizeY());
	ZeroMemory(m_bufferLabelRoi3.GetMem(), m_bufferLabelRoi3.GetSizeX() * m_bufferLabelRoi3.GetSizeY());
	ZeroMemory(m_bufferLabelRoi4.GetMem(), m_bufferLabelRoi4.GetSizeX() * m_bufferLabelRoi4.GetSizeY());
	ZeroMemory(m_bufferLabelRoi1_Electrode.GetMem(), m_bufferLabelRoi1_Electrode.GetSizeX() * m_bufferLabelRoi1_Electrode.GetSizeY());
	ZeroMemory(m_bufferLabelRoi4_Electrode.GetMem(), m_bufferLabelRoi4_Electrode.GetSizeX() * m_bufferLabelRoi4_Electrode.GetSizeY());
	ZeroMemory(m_bufferLabelRoiRight.GetMem(), m_bufferLabelRoiRight.GetSizeX() * m_bufferLabelRoiRight.GetSizeY());
	ZeroMemory(m_bufferLabelRoiLeft.GetMem(), m_bufferLabelRoiLeft.GetSizeX() * m_bufferLabelRoiLeft.GetSizeY());*/
	if (resetTapThreshold)
		ZeroMemory(m_tapeThreshold.GetMem(), m_tapeThreshold.GetSizeX() * m_tapeThreshold.GetSizeY());
}

void CProcessingBase::SetUpDown(bool isUp)
{
	m_isUp = isUp;
}

void CProcessingBase::SetUpDown(long nSide)
{
    m_nSide = nSide;
    m_isUp = TRUE;
}

void CProcessingBase::SetCellID(long cellID) //이현민 수석님
{
	m_cellID = cellID;
}

void CProcessingBase::SetStartInspectionTime(float time_ms)
{
	m_inspectionStartTime_ms = time_ms;
}

void CProcessingBase::SetInspectionSplitImage(IPVM::Image_8u_C1 brightImage, IPVM::Image_8u_C1 darkImage)
{
	m_inspectionImageBright = brightImage;
	m_inspectionImageDark = darkImage;
	m_imageSizeX = brightImage.GetSizeX();
	m_imageSizeY = brightImage.GetSizeY();
}

void CProcessingBase::SetBufferThreshold(IPVM::Image_8u_C1 image)
{
	m_bufferThreshold = image;
}

void CProcessingBase::SetBufferThreshold2(IPVM::Image_8u_C1 image)
{
	m_bufferThreshold2 = image;
}

void CProcessingBase::SetBufferThreshold3(IPVM::Image_8u_C1 image)
{
	m_bufferThreshold3 = image;
}

void CProcessingBase::SetBufferThreshold4(IPVM::Image_8u_C1 image)
{
	m_bufferThreshold4 = image;
}

void CProcessingBase::SetBufferThreshold5(IPVM::Image_8u_C1 image)
{
	m_bufferThreshold5 = image;
}

void CProcessingBase::SetBufferThreshold6(IPVM::Image_8u_C1 image)
{
	m_bufferThreshold6 = image;
}

void CProcessingBase::SetBufferLabel(IPVM::Image_32s_C1 label)
{
	m_bufferLabel = label;
}

//void CProcessingBase::SetBufferLabel_Burr_Only(IPVM::Image_32s_C1 label1, IPVM::Image_32s_C1 label2, IPVM::Image_32s_C1 label3, IPVM::Image_32s_C1 label4, 
//	IPVM::Image_32s_C1 label5, IPVM::Image_32s_C1 label6, IPVM::Image_32s_C1 label7, IPVM::Image_32s_C1 label8)
//{
//	m_bufferLabelRoi1 = label1;
//	m_bufferLabelRoi2 = label2;
//	m_bufferLabelRoi3 = label3;
//	m_bufferLabelRoi4 = label4;
//	m_bufferLabelRoi1_Electrode = label5;
//	m_bufferLabelRoi4_Electrode = label6;
//	m_bufferLabelRoiRight = label7;
//	m_bufferLabelRoiLeft = label8;
//}

void CProcessingBase::SetBufferBlob(IPVM::BlobInfo* blob)
{
	m_bufferBlob = blob;
}

void CProcessingBase::SetTapeThreshold(IPVM::Image_8u_C1 image)
{
	m_tapeThreshold = image;
}

bool CProcessingBase::MakeThresholdImage(const IPVM::Image_8u_C1& i_GrayImage, BYTE threshold, const IPVM::Rect& rtROI, IPVM::Image_8u_C1& i_ThresImage, bool removeBackground, bool origBackground)
{
	if (removeBackground)
	{
		if (IPVM::ImageProcessing::Fill(IPVM::Rect(i_ThresImage), 0, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}

	if (origBackground)
	{
		if (IPVM::ImageProcessing::Copy(i_GrayImage, rtROI, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}

	threshold = max(1, threshold);
	if (IPVM::ImageProcessing::BinarizeGreater(i_GrayImage, rtROI, threshold, i_ThresImage) != IPVM::Status::success)
	{
		return false;
	}

	return true;
}

bool CProcessingBase::MakeReverseThresholdImage(const IPVM::Image_8u_C1& i_GrayImage, BYTE threshold, const IPVM::Rect& rtROI, IPVM::Image_8u_C1& i_ThresImage, bool removeBackground, bool origBackground)
{
	if (removeBackground)
	{
		if (IPVM::ImageProcessing::Fill(IPVM::Rect(i_ThresImage), 0, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}

	if (origBackground)
	{
		if (IPVM::ImageProcessing::Copy(i_GrayImage, rtROI, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}

	threshold = max(1, threshold);
	if (IPVM::ImageProcessing::BinarizeLess(i_GrayImage, rtROI, threshold, i_ThresImage) != IPVM::Status::success)
	{
		return false;
	}

	return true;
}

bool CProcessingBase::MakeBothThresholdImage(const IPVM::Image_8u_C1& i_GrayImage, BYTE lowThreshold, BYTE highThreshold, const IPVM::Rect& rtROI, IPVM::Image_8u_C1& i_ThresImage, bool removeBackground, bool origBackground)
{
	if (removeBackground)
	{
		if (IPVM::ImageProcessing::Fill(IPVM::Rect(i_ThresImage), 0, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}

	if (origBackground)
	{
		if (IPVM::ImageProcessing::Copy(i_GrayImage, rtROI, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}
	if (IPVM::ImageProcessing::BinarizeLess(i_GrayImage, rtROI, lowThreshold, i_ThresImage) != IPVM::Status::success) //2023.09.04 기존에 equal 이라서 무조건 검사 되게 되어있음 그래서 수정
	{
		return false;
	}
	if (IPVM::ImageProcessing::BinarizeGreater(i_GrayImage, rtROI, highThreshold, i_ThresImage) != IPVM::Status::success)
	{
		return false;
	}
	// 배경 GV가 0일 때 과검 발생하므로.. low th를 0으로 설정하면 흑색 Burr 검사하지 않도록 수정 (NA, 230818)
	/*if (lowThreshold == 0x00)
	{
		if (IPVM::ImageProcessing::BinarizeGreater(i_GrayImage, rtROI, highThreshold, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}
	else
	{
		if (IPVM::ImageProcessing::BinarizeLessEqualOrGreaterEqual(i_GrayImage, rtROI, lowThreshold, highThreshold, i_ThresImage) != IPVM::Status::success)
		{
			return false;
		}
	}*/

	/*if (IPVM::ImageProcessing::BinarizeLessEqualOrGreaterEqual(i_GrayImage, rtROI, lowThreshold, highThreshold, i_ThresImage) != IPVM::Status::success)
	{
		return false;
	}*/

	return true;
}

bool CProcessingBase::GetBlobs(const IPVM::Image_8u_C1& i_ThresImage,
	const IPVM::Rect& rtROI,
	const long minSize,
	const long maxSize,
	const long mergeDist,
	IPVM::Image_32s_C1& o_LabelImage,
	IPVM::BlobInfo* o_blobInfos,
	long& nBlobNum)
{
	//	TimeStack timeLog(__FUNCTIONW__);

	IPVM::AlgorithmObjectHandle<IPVM::BlobDetection> blobDetection;

	if (blobDetection->DetectBlob(i_ThresImage, rtROI, true, 0, BLOB_NUM_MAX, o_blobInfos, nBlobNum, o_LabelImage) != IPVM::Status::success)
	{
		nBlobNum = 0;
		return false;
	}

	IPVM::BlobDetection::BlobFiltering_SmallerThanSizeX(minSize, nBlobNum, o_blobInfos);
	IPVM::BlobDetection::BlobFiltering_SmallerThanSizeY(minSize, nBlobNum, o_blobInfos);
	IPVM::BlobDetection::BlobFiltering_LargerThanSizeX(maxSize, nBlobNum, o_blobInfos);
	IPVM::BlobDetection::BlobFiltering_LargerThanSizeY(maxSize, nBlobNum, o_blobInfos);

	if (mergeDist != 0)
	{
		blobDetection->BlobLink(o_LabelImage, nBlobNum, mergeDist, o_blobInfos);
	}

	// Filtering 된 BlobInfo 제거
	blobDetection->BlobMerge(nBlobNum, o_LabelImage, o_blobInfos, nBlobNum);

	return true;
}

bool CProcessingBase::GetBlobs_ISI(const IPVM::Image_8u_C1& i_ThresImage,
	const IPVM::Rect& rtROI,
	const long minSizeX,
	const long minSizeY,
	const long mergeDist,
	IPVM::Image_16u_C1& o_LabelImage,
	IPVM::BlobInfo* o_blobInfos,
	long& nBlobNum,
	const long& MaxBlobCount,
	float* fWidth,
	float* fLength,
	float* fLocus,
	float* fThickness,
	float* fMaxThickness,
	float* MinGV,
	float* MaxGV,
	float* AvgGV,
	IPVM::Point_32f_C2* massCenter,
	float* Compact,
	const IPVM::Image_8u_C1& origin,
	const bool bResize
	)
{
	
	return true;
}

const long constAngle = 180;
void CProcessingBase::RotatePoints(int num, float* i_pfx, float* i_pfy, float fangle, float* o_pfx, float* o_pfy)
{
    //fangle = fangle * (3.141592 / 180);
    //m_fCalcCos[long(fangle)] m_fCalcSin[long(fangle)]
    for (int i = 0; i < num; i++)
    {
        o_pfx[i] = m_fCalcCos[long(fangle)] * i_pfx[i] - m_fCalcSin[long(fangle)] * i_pfy[i];
        o_pfy[i] = m_fCalcSin[long(fangle)] * i_pfx[i] + m_fCalcCos[long(fangle)] * i_pfy[i];
    }
}

void CProcessingBase::RotatePoints(int num, float* i_pfx, float* i_pfy, float fangle, float* o_pfx, float* o_pfy, CPoint pt)
{
    //fangle = fangle * (3.141592 / 180);
    for (int i = 0; i < num; i++)
    {
        i_pfx[i] = i_pfx[i] - pt.x;
        i_pfy[i] = i_pfy[i] - pt.y;
        o_pfx[i] = m_fCalcCos[long(fangle)] * i_pfx[i] - m_fCalcSin[long(fangle)] * i_pfy[i];
        o_pfy[i] = m_fCalcSin[long(fangle)] * i_pfx[i] + m_fCalcCos[long(fangle)] * i_pfy[i];

        o_pfx[i] = o_pfx[i] + pt.x;
        o_pfy[i] = o_pfy[i] + pt.y;
    }
}

void CProcessingBase::RotatePoints_Min(int num, float* i_pfx, float* i_pfy, float fangle, float* o_pfx, float* o_pfy, CPoint pt)
{
    //fangle = fangle * (3.141592 / 180);
    for (int i = 0; i < num; i++)
    {
        i_pfx[i] = i_pfx[i] - pt.x;
        i_pfy[i] = i_pfy[i] - pt.y;
        o_pfx[i] = m_fCalcCosMin[long(fangle)] * i_pfx[i] - m_fCalcSinMin[long(fangle)] * i_pfy[i];
        o_pfy[i] = m_fCalcSinMin[long(fangle)] * i_pfx[i] + m_fCalcCosMin[long(fangle)] * i_pfy[i];

        o_pfx[i] = o_pfx[i] + pt.x;
        o_pfy[i] = o_pfy[i] + pt.y;
    }
}

IPVM::Point_32f_C2 IntersectionPoint1(const IPVM::Point_32f_C2* p1, const IPVM::Point_32f_C2* p2, const IPVM::Point_32f_C2* p3, const IPVM::Point_32f_C2* p4)
{
    IPVM::Point_32f_C2 ret;

    ret.m_x = ((p1->m_x * p2->m_y - p1->m_y * p2->m_x) * (p3->m_x - p4->m_x) - (p1->m_x - p2->m_x) * (p3->m_x * p4->m_y - p3->m_y * p4->m_x))
        / ((p1->m_x - p2->m_x) * (p3->m_y - p4->m_y) - (p1->m_y - p2->m_y) * (p3->m_x - p4->m_x));

    ret.m_y = ((p1->m_x * p2->m_y - p1->m_y * p2->m_x) * (p3->m_y - p4->m_y) - (p1->m_y - p2->m_y) * (p3->m_x * p4->m_y - p3->m_y * p4->m_x))
        / ((p1->m_x - p2->m_x) * (p3->m_y - p4->m_y) - (p1->m_y - p2->m_y) * (p3->m_x - p4->m_x));

    return ret;
}
#include <vector>
#include <algorithm>
bool compare(IPVM::Point_32s_C2 a, IPVM::Point_32s_C2 b)
{
    return a.m_x < b.m_x;
}
IPVM::Status CProcessingBase::GetBlobWidthLength(const IPVM::Image_16u_C1& labelImage, const IPVM::Image_8u_C1& BinaryImage, const IPVM::Image_8u_C1& DarkImage, const IPVM::BlobInfo* blobInfo, const long blobCount, const long dataByteInterval, float* blobWidth, float* blobLength, float* BoundaryLength,
    float* LTx, float* LTy, float* LBx, float* LBy, float* RTx, float* RTy, float* RBx, float* RBy,
    const IPVM::Image_8u_C1& srcImage, float* edgeEnergySharpness, const float fMag)
{
	IPVM::Status ret = IPVM::Status::success;
	std::vector<IPVM::Point_32s_C2> edgeList;
	size_t necessaryMaxBufferLength = 0;
	// 필요한 버퍼 크기 계산
	for (long blobIndex = 0; blobIndex < blobCount; blobIndex++)
	{
		const IPVM::BlobInfo& curBlob = blobInfo[blobIndex];

		if (curBlob.m_area <= 0 || curBlob.m_previousLinkedBlobIndex >= 0)
		{
			continue;
		}

		size_t necessaryBufferLength = 0;

		long nextLinkedBlobIndex = blobIndex;

		while (nextLinkedBlobIndex != -1)
		{
			const IPVM::BlobInfo& nextLinkedBlob = blobInfo[nextLinkedBlobIndex];

			// 무효화되지 않은 curBlob만 계산에 참여
			if (nextLinkedBlob.m_area > 0)
			{
				necessaryBufferLength += edgeList.size() + 4 * nextLinkedBlob.m_area;
			}

			nextLinkedBlobIndex = nextLinkedBlob.m_nextLinkedBlobIndex;
		}

		necessaryMaxBufferLength = max(necessaryMaxBufferLength, necessaryBufferLength);
	}

	edgeList.reserve(necessaryMaxBufferLength);
    /*std::vector<double> vecdResultTheta;

	std::vector<double> vecdResultTheta1;
    std::vector<double> vecdResultTheta2;*/
	for (long blobIndex = 0; blobIndex < blobCount; blobIndex++)
	{
		const IPVM::BlobInfo& curBlob = blobInfo[blobIndex];
		float& fWidth = *((float*)(((BYTE*)blobWidth) + dataByteInterval * blobIndex));
		float& fLength = *((float*)(((BYTE*)blobLength) + dataByteInterval * blobIndex));
		float& fBoundaryLength = *((float*)(((BYTE*)BoundaryLength) + dataByteInterval * blobIndex));

		float& ltx = *((float*)(((BYTE*)LTx) + dataByteInterval * blobIndex));
        float& lty = *((float*)(((BYTE*)LTy) + dataByteInterval * blobIndex));
        float& lbx = *((float*)(((BYTE*)LBx) + dataByteInterval * blobIndex));
        float& lby = *((float*)(((BYTE*)LBy) + dataByteInterval * blobIndex));
        float& rtx = *((float*)(((BYTE*)RTx) + dataByteInterval * blobIndex));
        float& rty = *((float*)(((BYTE*)RTy) + dataByteInterval * blobIndex));
        float& rbx = *((float*)(((BYTE*)RBx) + dataByteInterval * blobIndex));
        float& rby = *((float*)(((BYTE*)RBy) + dataByteInterval * blobIndex));

		float& fEnergy = *((float*)(((BYTE*)edgeEnergySharpness) + dataByteInterval * blobIndex));

        
		if (curBlob.m_area <= 0 || curBlob.m_previousLinkedBlobIndex >= 0)
		{
			fWidth = 0.f;
			fLength = 0.f;
			continue;
		}

		edgeList.clear();

		long nextLinkedBlobIndex = blobIndex;
        std::vector<IPVM::Point_32s_C2> edgeListCalc;
        long nCheckArea = 0;

		while (nextLinkedBlobIndex != -1)
		{
			const IPVM::BlobInfo& nextLinkedBlob = blobInfo[nextLinkedBlobIndex];

			// 무효화되지 않은 curBlob만 계산에 참여
			if (nextLinkedBlob.m_area > 0)
			{
				edgeList.reserve(edgeList.size() + 4 * nextLinkedBlob.m_area);
                std::vector<IPVM::Point_32s_C2> edgeList_White;
                std::vector<IPVM::Point_32s_C2> edgeList_Dark;
                edgeList_White.reserve(edgeList.size());
                edgeList_Dark.reserve(edgeList.size());
				GetObjectBoundary(labelImage, nextLinkedBlob.m_roi, nextLinkedBlob.m_label, edgeList);

#if INSPECTION_MACHINE_UC1

#else           
                for (long nEdge = 0; nEdge < edgeList.size(); nEdge++)
                {
                    const unsigned char* pImage = srcImage.GetMem(0, edgeList[nEdge].m_y);
                    //const unsigned char* pImage = srcImage.GetMem(edgeList[nEdge].m_x, edgeList[nEdge].m_y);
                    const unsigned char* pbyDownImage = pImage + srcImage.GetWidthBytes();
                    const unsigned char* pbyRightImage = pImage + 1;
                    const unsigned char* pbyUpImage = pImage - srcImage.GetWidthBytes();
                    const unsigned char* pbyLeftImage = pImage - 1;

                    auto* pLabelImageDown = labelImage.GetMem(0, edgeList[nEdge].m_y);
                    if (edgeList[nEdge].m_y + 1 < srcImage.GetSizeY())
                    {
                        pLabelImageDown = labelImage.GetMem(0, edgeList[nEdge].m_y + 1);
                    }

                    auto* pLabelImageUp = labelImage.GetMem(0, edgeList[nEdge].m_y);
                    if (edgeList[nEdge].m_y - 1 > 0)
                    {
                        pLabelImageUp = labelImage.GetMem(0, edgeList[nEdge].m_y - 1);
                    }

                    auto* pLabelImageLeft = labelImage.GetMem(0, edgeList[nEdge].m_y);
                    auto* pLabelImageRight = labelImage.GetMem(0, edgeList[nEdge].m_y);
                    //               auto* pLabelImageLeft = labelImage.GetMem(edgeList[nEdge].m_x, 0);
                    //               if (edgeList[nEdge].m_x - 1 > 0)
                    //{
                    //                   pLabelImageLeft = labelImage.GetMem(edgeList[nEdge].m_x - 1, 0);
                    //}

                    //               auto* pLabelImageRight = labelImage.GetMem(edgeList[nEdge].m_x, 0);
                    //               if (edgeList[nEdge].m_x + 1 < srcImage.GetSizeY())
                    //{
                    //                   pLabelImageRight = labelImage.GetMem(edgeList[nEdge].m_x + 1, 0);
                    //}
                    //const auto* pLabelImageDown = labelImage.GetMem(0, edgeList[nEdge].m_y + 1);
                    //const auto* pLabelImageUp = labelImage.GetMem(0, edgeList[nEdge].m_y - 1);
                    //const auto* pLabelImageLeft = labelImage.GetMem(edgeList[nEdge].m_x - 1, 0);
                    //const auto* pLabelImageRight = labelImage.GetMem(edgeList[nEdge].m_x + 1, 0);

                    //               const auto nValueImage = srcImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);
                    //               const auto pLabelImage = labelImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);

                    ////Down
                    //               auto nValueDownImage = srcImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);
                    //               auto pLabelDownImage = labelImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);

                    //               if (edgeList[nEdge].m_y + 1 < srcImage.GetSizeY())
                    //               {
                    //                   nValueDownImage = srcImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y + 1);
                    //                   pLabelDownImage = labelImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y + 1);
                    //               }

                    ////Right
                    //               auto nValueRightImage = srcImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);
                    //               auto pLabelRightImage = labelImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);

                    //if (edgeList[nEdge].m_x + 1 < srcImage.GetSizeX())
                    //               {
                    //                   nValueRightImage = srcImage.GetValue(edgeList[nEdge].m_x + 1, edgeList[nEdge].m_y);
                    //                   pLabelRightImage = labelImage.GetValue(edgeList[nEdge].m_x + 1, edgeList[nEdge].m_y);
                    //}

                    ////Up
                    //               auto nValueUpImage = srcImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);
                    //               auto pLabelUpImage = labelImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y);

                    //if (edgeList[nEdge].m_y - 1 > 0)
                    //{
                    //                   nValueUpImage = srcImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y - 1);
                    //                   pLabelUpImage = labelImage.GetValue(edgeList[nEdge].m_x, edgeList[nEdge].m_y - 1);
                    //}
                    //
                    ////Left
                    //               auto nValueLeftImage = srcImage.GetValue(edgeList[nEdge].m_x , edgeList[nEdge].m_y);
                    //               auto pLabelLeftImage = labelImage.GetValue(edgeList[nEdge].m_x , edgeList[nEdge].m_y);

                    //if (edgeList[nEdge].m_x - 1 > 0)
                    //{
                    //                   nValueLeftImage = srcImage.GetValue(edgeList[nEdge].m_x - 1, edgeList[nEdge].m_y);
                    //                   pLabelLeftImage = labelImage.GetValue(edgeList[nEdge].m_x - 1, edgeList[nEdge].m_y);
                    //}

                    float fTemp = -0.f;
                    long nTempCount = 0;

                    //if (long(pLabelDownImage) != curBlob.m_label)
                    //{
                    //    fTemp += abs(nValueImage - nValueDownImage);
                    //    nTempCount++;
                    //}
                    //if (long(pLabelRightImage) != curBlob.m_label)
                    //{
                    //    fTemp += abs(nValueImage - nValueRightImage);
                    //    nTempCount++;
                    //}
                    //if (long(pLabelUpImage) != curBlob.m_label)
                    //{
                    //    fTemp += abs(nValueImage - nValueUpImage);
                    //    nTempCount++;
                    //}
                    //if (long(pLabelLeftImage) != curBlob.m_label)
                    //{
                    //    fTemp += abs(nValueImage - nValueLeftImage);
                    //    nTempCount++;
                    //}

                    if (pLabelImageDown[edgeList[nEdge].m_x] != curBlob.m_label)
                    {
                        fTemp += abs(pImage[edgeList[nEdge].m_x] - pbyDownImage[edgeList[nEdge].m_x]);
                        nTempCount++;
                    }
                    if (pLabelImageUp[edgeList[nEdge].m_x] != curBlob.m_label)
                    {
                        fTemp += abs(pImage[edgeList[nEdge].m_x] - pbyUpImage[edgeList[nEdge].m_x]);
                        nTempCount++;
                    }
                    if (edgeList[nEdge].m_x - 1 > 0)
                    {
                        if (pLabelImageLeft[edgeList[nEdge].m_x - 1] != curBlob.m_label)
                        {
                            fTemp += abs(pImage[edgeList[nEdge].m_x] - pbyLeftImage[edgeList[nEdge].m_x]);
                            nTempCount++;
                        }
                    }

                    if (edgeList[nEdge].m_x + 1 < srcImage.GetSizeX())
                    {
                        if (pLabelImageRight[edgeList[nEdge].m_x + 1] != curBlob.m_label)
                        {
                            fTemp += abs(pImage[edgeList[nEdge].m_x] - pbyRightImage[edgeList[nEdge].m_x]);
                            nTempCount++;
                        }
                    }

                    if (nTempCount == 0)
                    {
                        nCheckArea++;
                        continue;
                    }

                    fTemp = fTemp / nTempCount;

                    fEnergy += fTemp;
                }
#endif
                //GetObjectLabel(labelImage, nextLinkedBlob.m_roi, nextLinkedBlob.m_label, edgeListCalc);
            }

            nextLinkedBlobIndex = nextLinkedBlob.m_nextLinkedBlobIndex;
        }

        fEnergy = fEnergy / (edgeList.size() - nCheckArea);


				

		if (edgeList.size() == 0)
		{
			// Edge를 못구할 경우는 ROI 전체가 풀로 차있을 경우일 것이다

			if (curBlob.m_roi.Width() > curBlob.m_roi.Height() * fMag)
			{
                fLength = (float)(curBlob.m_roi.Width() * m_pixelToUmX * 0.001f);
                fWidth = (float)(curBlob.m_roi.Height() * m_pixelToUmY * 0.001f);
			}
			else
			{
                fLength = (float)(curBlob.m_roi.Height() * m_pixelToUmY * 0.001f);
                fWidth = (float)(curBlob.m_roi.Width() * m_pixelToUmX * 0.001f);
			}

			continue;
		}
		//fBoundaryLength = 0;
		//for (int i = 0; i < edgeList.size(); i++)
		//{
		//	int nNextIndex = i + 1;
		//	if (nNextIndex >= edgeList.size())
		//		nNextIndex = 0;
		//	fBoundaryLength += sqrt(pow(edgeList[i].m_x - edgeList[nNextIndex].m_x, 2) + pow((edgeList[i].m_y - edgeList[nNextIndex].m_y) * fMag/*y분해능 배율*/, 2));
		//}
		//-----------------------------------------------------------------------------
		// Edge Mass Center를 구한다
		//-----------------------------------------------------------------------------

		const long edgeCount = (long)edgeList.size();
		const auto* edgeListPtr = &edgeList[0];

		IPVM::Point_32f_C2 ptMassCenter(0.f, 0.f);

		for (const auto* itr = edgeListPtr; itr != (edgeListPtr + edgeCount); ++itr)
		{
			ptMassCenter.m_x += itr->m_x;
			ptMassCenter.m_y += itr->m_y;
		}

		ptMassCenter.m_x /= edgeCount;
		ptMassCenter.m_y /= edgeCount;

		
        //////////////////////// 230720현식 수정 시작
        std::vector<float> fLx, fRx, fBy, fTy;
        float fMaxLength = -99999999999;
        float fMaxWidth = -99999999999;

       
        long nAngle;
        float fSelectL;
        float fSelectT;

        float fSelectR;
        float fSelectB;
        CPoint ptCenter;
        ptCenter.x = ptMassCenter.m_x;
        ptCenter.y = ptMassCenter.m_y;
       

        fLx.resize(constAngle);
        fRx.resize(constAngle);
        fTy.resize(constAngle);
        fBy.resize(constAngle);

  
        for (long i = 0; i < constAngle; i++)
        {
            fLx[i] = 99999999;
            fRx[i] = -99999999;

            fTy[i] = 99999999;
            fBy[i] = -99999999;
        }
        //BOOL bContrastImage = (pimgContrast == nullptr) ? FALSE : TRUE;
        // 비 최적화 버전
 
        for (const auto* itr = edgeListPtr; itr != (edgeListPtr + edgeCount); ++itr)
        {
            const auto x = itr->m_x;//            -ptMassCenter.m_x;
            const auto y = itr->m_y;//            -ptMassCenter.m_y;
            for (long i = 0; i < constAngle; i++)
            {
                float xOri = x;
                float yOri = y;
                float xTemp;
                float yTemp;
                xTemp = xOri;
                yTemp = yOri;
                if (i > 0)
                {
                    
                    RotatePoints(1, &xOri, &yOri, i, &xTemp, &yTemp, ptCenter);
                }

                if (xTemp < fLx[i])
                {
                    fLx[i] = xTemp;
                }
                if (xTemp > fRx[i])
                {
                    fRx[i] = xTemp;
                }
                if (yTemp < fTy[i])
                {
                    fTy[i] = yTemp;
                }
                if (yTemp > fBy[i])
                {
                    fBy[i] = yTemp;
                }
            }
        }

       
		float fMinArea = 999999999999999;
        for (long i = 0; i < constAngle; i++)
        {
            float fTempX = fabs(fLx[i] - fRx[i]);
            float fTempY = fabs(fTy[i] - fBy[i]);
            
            float fArea = fTempX * fTempY;
            if (fMinArea > fArea)
            {
                fMinArea = fArea;
                nAngle = i;
                fSelectL = fLx[i];
                fSelectT = fTy[i];

                fSelectR = fRx[i];
                fSelectB = fBy[i];
                /*if (fTempY > fTempX)
                {
                    fLength = fTempY;
                    fWidth = fTempX;
                }
                else
                {
                    fLength = fTempX;
                    fWidth = fTempY;
                }*/
            }

        }




        IPVM::Point_32f_C2 fptLT;
        IPVM::Point_32f_C2 fptRT;
        IPVM::Point_32f_C2 fptLB;
        IPVM::Point_32f_C2 fptRB;


		IPVM::Point_32f_C2 fptResultLT;
        IPVM::Point_32f_C2 fptResultRT;
        IPVM::Point_32f_C2 fptResultLB;
        IPVM::Point_32f_C2 fptResultRB;

		fptLT.m_x = fSelectL;
        fptLT.m_y = fSelectT;

        fptRT.m_x = fSelectR;
        fptRT.m_y = fSelectT;

        fptLB.m_x = fSelectL;
        fptLB.m_y = fSelectB;

        fptRB.m_x = fSelectR;
        fptRB.m_y = fSelectB;

		RotatePoints_Min(1, &fptLT.m_x, &fptLT.m_y, nAngle, &fptResultLT.m_x, &fptResultLT.m_y, ptCenter);
        RotatePoints_Min(1, &fptRT.m_x, &fptRT.m_y, nAngle, &fptResultRT.m_x, &fptResultRT.m_y, ptCenter);
        RotatePoints_Min(1, &fptLB.m_x, &fptLB.m_y, nAngle, &fptResultLB.m_x, &fptResultLB.m_y, ptCenter);
        RotatePoints_Min(1, &fptRB.m_x, &fptRB.m_y, nAngle, &fptResultRB.m_x, &fptResultRB.m_y, ptCenter);


		ltx = fptResultLT.m_x;
        lty = fptResultLT.m_y;
        lbx = fptResultLB.m_x;
        lby = fptResultLB.m_y;
        rtx = fptResultRT.m_x;
        rty = fptResultRT.m_y;
        rbx = fptResultRB.m_x;
        rby = fptResultRB.m_y;

		float fDisLR;
        float fDisTB;
        //IPVM::Geometry::GetDistance(fptResultLT, fptResultLB, fDisTB);
        //IPVM::Geometry::GetDistance(fptResultLT, fptResultRT, fDisLR);
		
		float fxTB = fabs(fptResultLT.m_x - fptResultLB.m_x) * m_pixelToUmX * 0.001f;
        float fyTB = fabs(fptResultLT.m_y - fptResultLB.m_y) * m_pixelToUmY * 0.001f;
        float fDisSqrtTB = sqrtf(fxTB * fxTB + fyTB * fyTB);

		float fxLR = fabs(fptResultLT.m_x - fptResultRT.m_x) * m_pixelToUmX * 0.001f;
        float fyLR = fabs(fptResultLT.m_y - fptResultRT.m_y) * m_pixelToUmY * 0.001f;
        float fDisSqrtLR = sqrtf(fxLR * fxLR + fyLR * fyLR);


		if (fDisSqrtLR > fDisSqrtTB)
        {
            fLength = fDisSqrtLR;
            fWidth = fDisSqrtTB;
		}
        else
        {
            fLength = fDisSqrtTB;
            fWidth = fDisSqrtLR;
		}
        
		
		/*fLength = (float)(curBlob.m_roi.Height() * m_pixelToUmY * 0.001f);
        fWidth = (float)(curBlob.m_roi.Width() * m_pixelToUmX * 0.001f);*/

        long n = 0;
		
	}

	return ret;
}
//void GatherBoundaryPixels(const LabelImageType& srcLabel, const Rect32s& srcROI, const int32_t checkLabel, std::vector<Point32s2>& edgeList)
//{
//    
//}

//
//static IPVM::Status ForEach3x3(const INPUT& in, const IPVM::Rect& roi, FUNC&& func)
//{
//    /* if (const auto status = ImageValidation::Check(in, roi); status != Status::k_ok)
//    {
//        return status;
//    }*/
//
//    const auto yBegin = roi.m_top + 1;
//    const auto yEnd = roi.m_bottom - 1;
//    const auto xBegin = roi.m_left + 1;
//    const auto xEnd = roi.m_right - 1;
//
//    if (yBegin >= yEnd || xBegin >= xEnd)
//    {
//        return IPVM::Status::success;
//    }
//
//    for (int32_t y = yBegin; y < yEnd; y++)
//    {
//        const auto itr_in_prev = in[y - 1];
//        const auto itr_in_curr = in[y];
//        const auto itr_in_next = in[y + 1];
//
//        auto val_0_0 = itr_in_prev[xBegin - 1];
//        auto val_1_0 = itr_in_curr[xBegin - 1];
//        auto val_2_0 = itr_in_next[xBegin - 1];
//        auto val_0_1 = itr_in_prev[xBegin];
//        auto val_1_1 = itr_in_curr[xBegin];
//        auto val_2_1 = itr_in_next[xBegin];
//
//        for (int32_t x = xBegin; x < xEnd; x++)
//        {
//            auto val_0_2 = itr_in_prev[x + 1];
//            auto val_1_2 = itr_in_curr[x + 1];
//            auto val_2_2 = itr_in_next[x + 1];
//
//            func(x, y, val_0_0, val_0_1, val_0_2, val_1_0, val_1_1, val_1_2, val_2_0, val_2_1, val_2_2);
//
//            val_0_0 = val_0_1;
//            val_0_1 = val_0_2;
//            val_1_0 = val_1_1;
//            val_1_1 = val_1_2;
//            val_2_0 = val_2_1;
//            val_2_1 = val_2_2;
//        }
//    }
//
//    return IPVM::Status::success;
//}


void CProcessingBase::GetObjectBoundary(const IPVM::Image_16u_C1& srcLabel, const IPVM::Rect& srcROI, const long checkLabel, std::vector<IPVM::Point_32s_C2>& edgeList)
{
    if (srcROI.IsRectEmpty())
        return;

    //------------------------------------------------------------------------------------------------
    // ROI 가장자리에 있는 Label 들을 Edge Point로 추가한다
    //------------------------------------------------------------------------------------------------
    {
        const int32_t y1 = srcROI.m_top;
        const int32_t y2 = srcROI.m_bottom - 1;

        const auto* labelY1 = srcLabel.GetMem(0, y1);
        const auto* labelY2 = srcLabel.GetMem(0, y2);

        for (int32_t x = srcROI.m_left; x < srcROI.m_right; x++)
        {
            if (labelY1[x] == checkLabel)
                edgeList.emplace_back(x, y1);
            if (labelY2[x] == checkLabel)
                edgeList.emplace_back(x, y2);
        }
    }

    {
        const int32_t x1 = srcROI.m_left;
        const int32_t x2 = srcROI.m_right - 1;

        for (int32_t y = srcROI.m_top + 1; y < srcROI.m_bottom - 1; y++)
        {
            const auto* labelY = srcLabel.GetMem(0, y);

            if (labelY[x1] == checkLabel)
                edgeList.emplace_back(x1, y);
            if (labelY[x2] == checkLabel)
                edgeList.emplace_back(x2, y);
        }
    }

    //------------------------------------------------------------------------------------------------
    // ROI 안쪽에 있는 Label 들중 경계에 있는 것을 Edge Point로 추가한다
    //------------------------------------------------------------------------------------------------

	const auto yBegin = srcROI.m_top + 1;
    const auto yEnd = srcROI.m_bottom - 1;
    const auto xBegin = srcROI.m_left + 1;
    const auto xEnd = srcROI.m_right - 1;
    
    if (yBegin >= yEnd || xBegin >= xEnd)
    {
        return;
    }
    
    for (int32_t y = yBegin; y < yEnd; y++)
    {
        
        const auto itr_in_prev =  srcLabel.GetMem(0, y-1);//in[y - 1];
        const auto itr_in_curr = srcLabel.GetMem(0, y ); //in[y];
        const auto itr_in_next = srcLabel.GetMem(0, y +1); //in[y + 1];
    
        auto val_0_0 = itr_in_prev[xBegin - 1];
        auto val_1_0 = itr_in_curr[xBegin - 1];
        auto val_2_0 = itr_in_next[xBegin - 1];
        auto val_0_1 = itr_in_prev[xBegin];
        auto val_1_1 = itr_in_curr[xBegin];
        auto val_2_1 = itr_in_next[xBegin];
    
        for (int32_t x = xBegin; x < xEnd; x++)
        {
            auto val_0_2 = itr_in_prev[x + 1];
            auto val_1_2 = itr_in_curr[x + 1];
            auto val_2_2 = itr_in_next[x + 1];
    
            //func(x, y, val_0_0, val_0_1, val_0_2, val_1_0, val_1_1, val_1_2, val_2_0, val_2_1, val_2_2);
            if (val_1_1 == checkLabel
                && (val_0_0 != checkLabel || val_0_1 != checkLabel || val_0_2 != checkLabel || val_1_0 != checkLabel || val_1_2 != checkLabel || val_2_0 != checkLabel
                    || val_2_1 != checkLabel || val_2_2 != checkLabel))
            {
                edgeList.emplace_back(x, y);
            }
            val_0_0 = val_0_1;
            val_0_1 = val_0_2;
            val_1_0 = val_1_1;
            val_1_1 = val_1_2;
            val_2_0 = val_2_1;
            val_2_1 = val_2_2;
        }
    }


	
}

void CProcessingBase::GetObjectLabel(const IPVM::Image_16u_C1& LabelImage, const IPVM::Rect& srcROI, const long checkLabel, std::vector<IPVM::Point_32s_C2>& vecEdgePoint)
{
    if (srcROI.IsRectEmpty())
        return;

    IPVM::Rect inROI = srcROI;

    if (inROI.IsRectEmpty())
        return;
    //long nSizeX = 0;//    inROI.Width() * .75f;
    //long nSizeY = 0;
    //inROI.Height() * .75f;
    for (long y = inROI.m_top; y < inROI.m_bottom; y++)
    {
        // const auto* labelUpY = LabelImage.GetMem(0, y - 1);
        const auto* label = LabelImage.GetMem(0, y);
        // const auto* labelDownY = LabelImage.GetMem(0, y + 1);

        for (long x = inROI.m_left; x < inROI.m_right; x++)
        {
            if (label[x] == checkLabel)
            {
                vecEdgePoint.emplace_back(x, y);
                //break;
            }
        }
    }

}


void CProcessingBase::GetObjectBoundary2(const IPVM::Image_16u_C1& LabelImage,const IPVM::Image_8u_C1& srcImage, const IPVM::Rect& srcROI, const long checkLabel, std::vector<IPVM::Point_32s_C2>& vecEdgePoint)
{
    if (srcROI.IsRectEmpty())
        return;

    IPVM::Rect inROI = srcROI;
   

    if (inROI.IsRectEmpty())
        return;
    //long nSizeX = 0;//    inROI.Width() * .75f;
    //long nSizeY = 0;
    //inROI.Height() * .75f;
    for (long y = inROI.m_top; y < inROI.m_bottom; y++)
    {
       // const auto* labelUpY = LabelImage.GetMem(0, y - 1);
        const auto* label = LabelImage.GetMem(0, y);
       // const auto* labelDownY = LabelImage.GetMem(0, y + 1);
        const auto* pSrc = srcImage.GetMem(0, y);
        for (long x = inROI.m_left; x < inROI.m_right ; x++)
        {
            if (label[x] == checkLabel)
            {
                if (pSrc[x] == 255)
                {
                    vecEdgePoint.emplace_back(x, y);
                    break;
                }
                
            }
        }
    }
    
	for (long y = inROI.m_top; y < inROI.m_bottom; y++)
    {
        // const auto* labelUpY = LabelImage.GetMem(0, y - 1);
        const auto* label = LabelImage.GetMem(0, y);
        // const auto* labelDownY = LabelImage.GetMem(0, y + 1);
        const auto* pSrc = srcImage.GetMem(0, y);
        for (long x = inROI.m_right - 1; x >= inROI.m_left ; x--)
        {
            if (label[x] == checkLabel)
            {
                if (pSrc[x] == 255)
                {
                    vecEdgePoint.emplace_back(x, y);
                    break;
                }
                
            }
        }
    }


	for (long x = inROI.m_left; x < inROI.m_right; x++)
    {
        // const auto* labelUpY = LabelImage.GetMem(0, y - 1);
        
        // const auto* labelDownY = LabelImage.GetMem(0, y + 1);

        for (long y = inROI.m_top; y < inROI.m_bottom ; y++)
        {
            if (*LabelImage.GetMem(x, y) == checkLabel)
            {
                if (*srcImage.GetMem(x, y) == 255)
                {
                    vecEdgePoint.emplace_back(x, y);
                    break;
                }
                
            }
        }
    }

	for (long x = inROI.m_left; x < inROI.m_right; x++)
    {
        // const auto* labelUpY = LabelImage.GetMem(0, y - 1);

        // const auto* labelDownY = LabelImage.GetMem(0, y + 1);

        for (long y = inROI.m_bottom - 1; y >= inROI.m_top ; y--)
        {
            if (*LabelImage.GetMem(x, y) == checkLabel)
            {
                if (*srcImage.GetMem(x, y) == 255)
                {
                    vecEdgePoint.emplace_back(x, y);
                    break;
                }
                
            }
        }
    }
}

IPVM::Status CProcessingBase::GetBlobEdgeEnergy(const IPVM::Image_16u_C1& label,  const IPVM::Image_8u_C1& DarkImage, const IPVM::BlobInfo* blobInfo, const long blobCount,
    const IPVM::Image_8u_C1& srcImage, const long dataByteInterval, float* edgeEnergy, float* edgeEnergyWhite, float* edgeEnergyBlack)
{
    for (long blobIndex = 0; blobIndex < blobCount; blobIndex++)
    {
        float& fEnergy = *((float*)(((BYTE*)edgeEnergy) + dataByteInterval * blobIndex));
        float& fWhiteEnergy = *((float*)(((BYTE*)edgeEnergyWhite) + dataByteInterval * blobIndex));
        float& fBlackEnergy = *((float*)(((BYTE*)edgeEnergyBlack) + dataByteInterval * blobIndex));
        fEnergy = 0.f;
        fBlackEnergy = 0.f;
        fWhiteEnergy = 0.f;

        if (blobInfo[blobIndex].m_area <= 0)
        {
            // 무효화된 Blob 무시
            return IPVM::Status::success;
        }

        if (blobInfo[blobIndex].m_previousLinkedBlobIndex >= 0)
        {
            // Link의 제일 첫번째 Item 만 Link을 쫓아가며 값을 계산한다
            continue;
        }

        long checkBlobIndex = blobIndex;
        long nRatioMixSum = 0.f;
        long nTotalArea = 0;

        while (checkBlobIndex != -1)
        {
            const IPVM::BlobInfo& curBlob = blobInfo[checkBlobIndex];

            if (curBlob.m_area >= 0)
            {
                // 무효화되지 않은 Blob만 계산에 참여
                IPVM::Rect rtROI = curBlob.m_roi;

                rtROI &= IPVM::Rect(rtROI.m_left, rtROI.m_top, min(rtROI.m_right, label.GetSizeX() - 1), min(rtROI.m_bottom, label.GetSizeY() - 1));

                for (long y = rtROI.m_top; y < rtROI.m_bottom; y++)
                {
                    const unsigned char* pImage = srcImage.GetMem(0, y);
                    const unsigned char* pbyDownImage = pImage + srcImage.GetWidthBytes();
                    const unsigned char* pbyRightImage = pImage + 1;
                    const auto* pLabelImage = label.GetMem(0, y);
                    const BYTE* pImageDark = DarkImage.GetMem(0, y);

                    for (long x = rtROI.m_left; x < rtROI.m_right; x++)
                    {
                        if (pLabelImage[x] != curBlob.m_label)
                            continue;

                        /*fEnergy += abs(pImage[x] - pbyDownImage[x]);
                        fEnergy += abs(pImage[x] - pbyRightImage[x]);*/
                        // 여기 에지 에너지 추가
                        float TempDown = abs(pImage[x] - pbyDownImage[x]);
                        float TempRight = abs(pImage[x] - pbyRightImage[x]);
                        fEnergy += TempDown;
                        fEnergy += TempRight;

                        if (pImageDark[x] == 255)
                        {
                            nRatioMixSum++;
                            fBlackEnergy += TempDown;
                            fBlackEnergy += TempRight;
                            
                        }
                        else
                        {
                            fWhiteEnergy += TempDown;
                            fWhiteEnergy += TempRight;
                        }
                    }
                }

                nTotalArea += curBlob.m_area;
            }

            checkBlobIndex = blobInfo[checkBlobIndex].m_nextLinkedBlobIndex;
        }

        if (nTotalArea > 0)
        {
            fEnergy /= nTotalArea;
            if (nRatioMixSum > 0)
            {
                fBlackEnergy /= nRatioMixSum;
			}
            else
            {
                fBlackEnergy = 0;
			}
            if (nTotalArea - nRatioMixSum > 0)
            {
                fWhiteEnergy /= (nTotalArea - nRatioMixSum);
			}
            else
            {
                fWhiteEnergy = 0;
			}
            
            
        }
    }

    return IPVM::Status::success;
}


IPVM::Status CProcessingBase::GetBlobContrast(const IPVM::Image_16u_C1& label, const IPVM::Image_8u_C1& DarkImage,
	const IPVM::BlobInfo* blobInfo, const long blobCount, const IPVM::Image_8u_C1& srcImage,  
	float fMinAreaContrast, const long dataByteInterval, float* keyContrast, 
	float* avgContrast, 
	float* WhiteavgContrast, 
	float* BlackavgContrast, float* minContrast, long nMCAvgSize, IPVM::Point_32f_C2* massCenter, float* massCenterAVG,
    float* ratioMix, long* over200GVpxCount, float* fMedianGV, float* fStdevGV)
{

	for (long blobIndex = 0; blobIndex < blobCount; blobIndex++)
	{

		float& fKey = *((float*)(((BYTE*)keyContrast) + dataByteInterval * blobIndex));
		float& fAvg = *((float*)(((BYTE*)avgContrast) + dataByteInterval * blobIndex));

		float& fAvgWhite = *((float*)(((BYTE*)WhiteavgContrast) + dataByteInterval * blobIndex));
		float& fAvgBlack = *((float*)(((BYTE*)BlackavgContrast) + dataByteInterval * blobIndex));

		float& fMin = *((float*)(((BYTE*)minContrast) + dataByteInterval * blobIndex));
		float& fMCAVG = *((float*)(((BYTE*)massCenterAVG) + dataByteInterval * blobIndex));
		float& fRatioMix = *((float*)(((BYTE*)ratioMix) + dataByteInterval * blobIndex));

		long& nOverPx = *((long*)(((BYTE*)over200GVpxCount) + dataByteInterval * blobIndex));
        
		float& fMedian = *((float*)(((BYTE*)fMedianGV) + dataByteInterval * blobIndex));
        float& fStdevv = *((float*)(((BYTE*)fStdevGV) + dataByteInterval * blobIndex));		
		//auto* blobMassCenter = (IPVM::Point_32f_C2*)(((BYTE*)massCenter) + dataByteInterval * blobIndex);

		auto blobMassCenter = massCenter[blobIndex];
		fKey = 0.f;
		fAvg = 0.f;
		fMin = 255.f;
		if (blobInfo[blobIndex].m_area <= 0)
		{
			// 무효화된 Blob 무시
			return IPVM::Status::success;
		}

		if (blobInfo[blobIndex].m_previousLinkedBlobIndex >= 0)
		{
			// Link의 제일 첫번째 Item 만 Link을 쫓아가며 값을 계산한다
			continue;
		}

		long checkBlobIndex = blobIndex;

		long nHistoDist[256];
		long nTotalArea = 0;

		long nHistoDistWhite[256];
		long nHistoDistBlack[256];

		// 초기화
		memset(nHistoDist, 0, sizeof(long) * 256);

		memset(nHistoDistWhite, 0, sizeof(long) * 256);
		memset(nHistoDistBlack, 0, sizeof(long) * 256);

		CRect rt;
        long nSizeTemp = 0;
        
		if (nMCAvgSize>0)
        {
            nSizeTemp = nMCAvgSize * 0.5f;

            rt.left = blobMassCenter.m_x - nSizeTemp;
            rt.top = blobMassCenter.m_y - nSizeTemp;
            rt.right = blobMassCenter.m_x + nSizeTemp;
            rt.bottom = blobMassCenter.m_y + nSizeTemp;
        }
		
		long nSizeMCAVG = 0;
		long fSumMCAVG= 0;
		fMCAVG = 0;
		long nRatioMixSum = 0.f;
        long nOver200GVpxCnt = 0;
        double dStdev = 0;
        std::vector<long> vecVal;
        long nGvMid = 0;
		//m_vecOver200GVPxCount.resize(blobCount);

		while (checkBlobIndex != -1)
		{
			const IPVM::BlobInfo& curBlob = blobInfo[checkBlobIndex];

			if (curBlob.m_area >= 0)
			{
				// 무효화되지 않은 Blob만 계산에 참여
				const IPVM::Rect& rtROI = curBlob.m_roi;
                double dAverage = 0;
                IPVM::ImageProcessing::GetMeanStdev(srcImage, rtROI, dAverage, dStdev);
                //IPVM::Rect temp = IPVM::Rect(rtROI.m_left, rtROI.m_top, min(rtROI.m_right, label.GetSizeX() - 1), min(rtROI.m_bottom, label.GetSizeY() - 1));
				for (long y = rtROI.m_top; y < rtROI.m_bottom; y++)
				{
					const auto* pLabelImage = label.GetMem(0, y);
					const BYTE* pImage = srcImage.GetMem(0, y);
					const BYTE* pImageDark = DarkImage.GetMem(0, y);
                    const BYTE* pbyDownImage = pImage + srcImage.GetWidthBytes();
                    const BYTE* pbyRightImage = pImage + 1;
					//const BYTE* pBg = srcBgImage.GetMem(0, y);

					for (long x = rtROI.m_left; x < rtROI.m_right; x++)
					{
						if (pLabelImage[x] == curBlob.m_label)
						{

							long nConstrast = abs(((long)pImage[x]));
							nHistoDist[nConstrast]++;
                            vecVal.push_back(nConstrast);
                            if (nMCAvgSize > 0)
                            {
                                if (PtInRect(rt, CPoint(x, y)))
                                {
                                    nSizeMCAVG++;
                                    fSumMCAVG += nConstrast;
                                }
                            }
                            if (nConstrast > 250)
                            {
                                nOver200GVpxCnt++;
							}
							/// <summary>
						
                           
							
							if (pImageDark[x] == 255)
							{
								nRatioMixSum++;
								nHistoDistBlack[nConstrast]++;
                              
							}
							else
							{
								nHistoDistWhite[nConstrast]++;
                                
							}
						}
					}
				}
                nGvMid = vecVal[vecVal.size() / 2];
				nTotalArea += curBlob.m_area;
				fMCAVG = nSizeMCAVG > 0 ? (fSumMCAVG / float(nSizeMCAVG)) : 0.f;
				/*if(nSizeMCAVG>0)
					fMCAVG = fSumMCAVG / nSizeMCAVG*/;
			}
            nOverPx = nOver200GVpxCnt;
			checkBlobIndex = blobInfo[checkBlobIndex].m_nextLinkedBlobIndex;
		}
		//m_over200GVPxCount = nOver200GVpxCnt;
        //m_vecOver200GVPxCount[checkBlobIndex] = nOver200GVpxCnt;
 

		long nAccHisto = 0;

		long nAccHistoWhite = 0;
		long nAccHistoBlack = 0;


		// Find Key Contrast
		/*for (long nIntensity = 255; nIntensity >= 0; nIntensity--)
		{
			nAccHisto += nHistoDist[nIntensity];
			if (nAccHisto >= fMinAreaContrast)
			{
				fKey = (float)nIntensity;
				break;
			}
		}*/

		for (long nIntensity = 0; nIntensity < 256; nIntensity++)
		{
			nAccHisto = nHistoDist[nIntensity];
			if (nHistoDist[nIntensity] > 0)
			{
				//fKey = (float)nIntensity;
				fKey = max(nIntensity, fKey);
				fMin = min(nIntensity, fMin);
			}
		}

		// Find Average Contrast
		nAccHisto = 0;
		for (long nIntensity = 255; nIntensity >= 0; nIntensity--)
		{
			nAccHisto += nIntensity * nHistoDist[nIntensity];
		}

		nAccHistoWhite = 0;
		for (long nIntensity = 255; nIntensity >= 0; nIntensity--)
		{
			nAccHistoWhite += nIntensity * nHistoDistWhite[nIntensity];
		}

		nAccHistoBlack = 0;
		for (long nIntensity = 255; nIntensity >= 0; nIntensity--)
		{
			nAccHistoBlack += nIntensity * nHistoDistBlack[nIntensity];
		}

		fRatioMix = nRatioMixSum > 0 ? (float(nRatioMixSum) / float(nTotalArea)) : 0.f; // Dark만 넣음

		long nRatioMixSumWhite;

		nRatioMixSumWhite = nTotalArea- nRatioMixSum;
		nRatioMixSum;

		fAvgWhite = nRatioMixSumWhite > 0 ? (nAccHistoWhite / float(nRatioMixSumWhite)) : 0.f;
		fAvgBlack = nRatioMixSum > 0 ? (nAccHistoBlack / float(nRatioMixSum)) : 0.f;
		
		fAvg = nTotalArea > 0 ? (nAccHisto / float(nTotalArea)) : 0.f;
        fMedian = nGvMid;
        fStdevv = dStdev;
	}

	return IPVM::Status::success;
}

IPVM::Status CProcessingBase::GetBlobEdgeSharpness(const IPVM::Image_16u_C1& labelImage,
    const IPVM::Image_8u_C1& BinaryImage,
    const IPVM::Image_8u_C1& DarkImage, const IPVM::BlobInfo* blobInfo, const long blobCount,
    const long dataByteInterval, const IPVM::Image_8u_C1& srcImage,
    float* edgeEnergySharpnessWhite, float* edgeEnergySharpnessBlack)
{
    IPVM::Status ret = IPVM::Status::success;
    std::vector<IPVM::Point_32s_C2> edgeList;
    std::vector<IPVM::Point_32s_C2> edgeList_White;
    std::vector<IPVM::Point_32s_C2> edgeList_Dark;
    size_t necessaryMaxBufferLength = 0;
    // 필요한 버퍼 크기 계산
    for (long blobIndex = 0; blobIndex < blobCount; blobIndex++)
    {
        const IPVM::BlobInfo& curBlob = blobInfo[blobIndex];

        if (curBlob.m_area <= 0 || curBlob.m_previousLinkedBlobIndex >= 0)
        {
            continue;
        }

        size_t necessaryBufferLength = 0;

        long nextLinkedBlobIndex = blobIndex;

        while (nextLinkedBlobIndex != -1)
        {
            const IPVM::BlobInfo& nextLinkedBlob = blobInfo[nextLinkedBlobIndex];

            // 무효화되지 않은 curBlob만 계산에 참여
            if (nextLinkedBlob.m_area > 0)
            {
                necessaryBufferLength += edgeList.size() + 4 * nextLinkedBlob.m_area;
            }

            nextLinkedBlobIndex = nextLinkedBlob.m_nextLinkedBlobIndex;
        }

        necessaryMaxBufferLength = max(necessaryMaxBufferLength, necessaryBufferLength);
    }

    edgeList.reserve(necessaryMaxBufferLength);

    for (long blobIndex = 0; blobIndex < blobCount; blobIndex++)
    {
        const IPVM::BlobInfo& curBlob = blobInfo[blobIndex];


        //float& fEnergy = *((float*)(((BYTE*)edgeEnergySharpness) + dataByteInterval * blobIndex));
        float& fEnergyWhite = *((float*)(((BYTE*)edgeEnergySharpnessWhite) + dataByteInterval * blobIndex));
        float& fEnergyBlack = *((float*)(((BYTE*)edgeEnergySharpnessBlack) + dataByteInterval * blobIndex));

        if (curBlob.m_area <= 0 || curBlob.m_previousLinkedBlobIndex >= 0)
        {
            continue;
        }

        edgeList.clear();

        long nextLinkedBlobIndex = blobIndex;
        std::vector<IPVM::Point_32s_C2> edgeListCalc;
        long nCheckArea = 0;
        long nRatioMixSum = 0.f;
        long nRatioMixSum2 = 0.f;
        //srcImage.SaveJpeg(_T("D:\\src.jpg"));
        //DarkImage.SaveJpeg(_T("D:\\Dark.jpg"));
        IPVM::Image_8u_C1 WhiteImage;
        WhiteImage.Create(srcImage.GetSizeX(), srcImage.GetSizeY());


        IPVM::ImageProcessing::Subtract(BinaryImage, DarkImage, IPVM::Rect(srcImage), 0, WhiteImage);
        //WhiteImage.SaveJpeg(_T("D:\\WhiteImage.jpg"));

        while (nextLinkedBlobIndex != -1)
        {
            const IPVM::BlobInfo& nextLinkedBlob = blobInfo[nextLinkedBlobIndex];

            edgeList_White.clear();
            edgeList_Dark.clear();

            // 무효화되지 않은 curBlob만 계산에 참여
            if (nextLinkedBlob.m_area > 0)
            {
                edgeList.reserve(edgeList.size() + 4 * nextLinkedBlob.m_area);

                edgeList_White.reserve(edgeList.size());
                edgeList_Dark.reserve(edgeList.size());

                GetObjectBoundary(labelImage, nextLinkedBlob.m_roi, nextLinkedBlob.m_label, edgeList);

                GetObjectBoundary2(labelImage, WhiteImage, nextLinkedBlob.m_roi, nextLinkedBlob.m_label, edgeList_White);
                GetObjectBoundary2(labelImage, DarkImage, nextLinkedBlob.m_roi, nextLinkedBlob.m_label, edgeList_Dark);

                sort(edgeList_White.begin(), edgeList_White.end(), compare);
                edgeList_White.erase(unique(edgeList_White.begin(), edgeList_White.end()), edgeList_White.end());

                sort(edgeList_Dark.begin(), edgeList_Dark.end(), compare);
                edgeList_Dark.erase(unique(edgeList_Dark.begin(), edgeList_Dark.end()), edgeList_Dark.end());

                nCheckArea = 0;
                if (edgeList_Dark.size() > 0)
                {
                    for (long nEdge = 0; nEdge < edgeList_Dark.size(); nEdge++)
                    {
                        const unsigned char* pImage = srcImage.GetMem(0, edgeList_Dark[nEdge].m_y);
                        //const unsigned char* pImage = srcImage.GetMem(edgeList[nEdge].m_x, edgeList[nEdge].m_y);
                        const unsigned char* pbyDownImage = pImage + srcImage.GetWidthBytes();
                        const unsigned char* pbyRightImage = pImage + 1;
                        const unsigned char* pbyUpImage = pImage - srcImage.GetWidthBytes();
                        const unsigned char* pbyLeftImage = pImage - 1;

                        const BYTE* pImageDark = DarkImage.GetMem(0, edgeList_Dark[nEdge].m_y);
                        const BYTE* pImageDarkDown = pImageDark;

                        const BYTE* pImageDarkUP = pImageDark;

                        auto* pLabelImageDown = labelImage.GetMem(0, edgeList_Dark[nEdge].m_y);
                        if (edgeList_Dark[nEdge].m_y + 1 < srcImage.GetSizeY())
                        {
                            pLabelImageDown = labelImage.GetMem(0, edgeList_Dark[nEdge].m_y + 1);
                            pImageDarkDown = pImageDark + DarkImage.GetWidthBytes();
                        }

                        auto* pLabelImageUp = labelImage.GetMem(0, edgeList_Dark[nEdge].m_y);
                        if (edgeList_Dark[nEdge].m_y - 1 > 0)
                        {
                            pLabelImageUp = labelImage.GetMem(0, edgeList_Dark[nEdge].m_y - 1);
                            pImageDarkUP = pImageDark - DarkImage.GetWidthBytes();
                        }

                        auto* pLabelImageLeft = labelImage.GetMem(0, edgeList_Dark[nEdge].m_y);
                        auto* pLabelImageRight = labelImage.GetMem(0, edgeList_Dark[nEdge].m_y);

                        float fTemp = -0.f;
                        long nTempCount = 0;

                        // DOWN
                        if (pLabelImageDown[edgeList_Dark[nEdge].m_x] != curBlob.m_label)
                        {
                            fTemp += abs(pImage[edgeList_Dark[nEdge].m_x] - pbyDownImage[edgeList_Dark[nEdge].m_x]);
                            nTempCount++;
                        }
                        else
                        {
                            if (pImageDarkDown[edgeList_Dark[nEdge].m_x] != 255)
                            {
                                fTemp += abs(pImage[edgeList_Dark[nEdge].m_x] - pbyLeftImage[edgeList_Dark[nEdge].m_x]);
                                nTempCount++;
                            }
                        }

                        //UP
                        if (pLabelImageUp[edgeList_Dark[nEdge].m_x] != curBlob.m_label)
                        {
                            fTemp += abs(pImage[edgeList_Dark[nEdge].m_x] - pbyUpImage[edgeList_Dark[nEdge].m_x]);
                            nTempCount++;
                        }
                        else
                        {
                            if (pImageDarkUP[edgeList_Dark[nEdge].m_x] != 255)
                            {
                                fTemp += abs(pImage[edgeList_Dark[nEdge].m_x] - pbyLeftImage[edgeList_Dark[nEdge].m_x]);
                                nTempCount++;
                            }
                        }

                        //LEFT
                        if (edgeList_Dark[nEdge].m_x - 1 > 0)
                        {
                            if (pLabelImageLeft[edgeList_Dark[nEdge].m_x - 1] != curBlob.m_label)
                            {
                                fTemp += abs(pImage[edgeList_Dark[nEdge].m_x] - pbyLeftImage[edgeList_Dark[nEdge].m_x]);
                                nTempCount++;
                            }
                            else
                            {
                                if (pImageDark[edgeList_Dark[nEdge].m_x - 1] != 255)
                                {
                                    fTemp += abs(
                                        pImage[edgeList_Dark[nEdge].m_x] - pbyLeftImage[edgeList_Dark[nEdge].m_x]);
                                    nTempCount++;
                                }
                            }
                        }

                        //RIGHT
                        if (edgeList_Dark[nEdge].m_x + 1 < srcImage.GetSizeX())
                        {
                            if (pLabelImageRight[edgeList_Dark[nEdge].m_x + 1] != curBlob.m_label)
                            {
                                fTemp
                                    += abs(pImage[edgeList_Dark[nEdge].m_x] - pbyRightImage[edgeList_Dark[nEdge].m_x]);
                                nTempCount++;
                            }
                            else
                            {
                                if (pImageDark[edgeList_Dark[nEdge].m_x + 1] != 255)
                                {
                                    fTemp += abs(
                                        pImage[edgeList_Dark[nEdge].m_x] - pbyLeftImage[edgeList_Dark[nEdge].m_x]);
                                    nTempCount++;
                                }
                            }
                        }

                        if (nTempCount == 0)
                        {
                            nCheckArea++;
                            continue;
                        }

                        fTemp = fTemp / nTempCount;
                        fEnergyBlack += fTemp;
                    }
                    fEnergyBlack = fEnergyBlack / (edgeList_Dark.size() - nCheckArea);

                }
                else
                {
                    fEnergyBlack = 0;
                }

                // White 
                nCheckArea = 0;
                if (edgeList_White.size() > 0)
                {
                    for (long nEdge = 0; nEdge < edgeList_White.size(); nEdge++)
                    {
                        const unsigned char* pImage = srcImage.GetMem(0, edgeList_White[nEdge].m_y);
                        //const unsigned char* pImage = srcImage.GetMem(edgeList_White[nEdge].m_x, edgeList_White[nEdge].m_y);
                        const unsigned char* pbyDownImage = pImage + srcImage.GetWidthBytes();
                        const unsigned char* pbyRightImage = pImage + 1;
                        const unsigned char* pbyUpImage = pImage - srcImage.GetWidthBytes();
                        const unsigned char* pbyLeftImage = pImage - 1;

                        const BYTE* pImageWhite = WhiteImage.GetMem(0, edgeList_White[nEdge].m_y);
                        const BYTE* pImageWhiteDown = pImageWhite;
                        const BYTE* pImageWhiteUP = pImageWhite;

                        auto* pLabelImageDown = labelImage.GetMem(0, edgeList_White[nEdge].m_y);
                        if (edgeList_White[nEdge].m_y + 1 < srcImage.GetSizeY())
                        {
                            pLabelImageDown = labelImage.GetMem(0, edgeList_White[nEdge].m_y + 1);
                            pImageWhiteDown = pImageWhite + WhiteImage.GetWidthBytes();
                        }

                        auto* pLabelImageUp = labelImage.GetMem(0, edgeList_White[nEdge].m_y);
                        if (edgeList_White[nEdge].m_y - 1 > 0)
                        {
                            pLabelImageUp = labelImage.GetMem(0, edgeList_White[nEdge].m_y - 1);
                            pImageWhiteUP = pImageWhite - WhiteImage.GetWidthBytes();
                        }

                        auto* pLabelImageLeft = labelImage.GetMem(0, edgeList_White[nEdge].m_y);
                        auto* pLabelImageRight = labelImage.GetMem(0, edgeList_White[nEdge].m_y);

                        float fTemp = -0.f;
                        long nTempCount = 0;

                        // DOWN
                        if (pLabelImageDown[edgeList_White[nEdge].m_x] != curBlob.m_label)
                        {
                            fTemp += abs(pImage[edgeList_White[nEdge].m_x] - pbyDownImage[edgeList_White[nEdge].m_x]);
                            nTempCount++;
                        }
                        else
                        {
                            if (pImageWhiteDown[edgeList_White[nEdge].m_x] != 255)
                            {
                                fTemp
                                    += abs(pImage[edgeList_White[nEdge].m_x] - pbyLeftImage[edgeList_White[nEdge].m_x]);
                                nTempCount++;
                            }
                        }

                        //UP
                        if (pLabelImageUp[edgeList_White[nEdge].m_x] != curBlob.m_label)
                        {
                            fTemp += abs(pImage[edgeList_White[nEdge].m_x] - pbyUpImage[edgeList_White[nEdge].m_x]);
                            nTempCount++;
                        }
                        else
                        {
                            if (pImageWhiteUP[edgeList_White[nEdge].m_x] != 255)
                            {
                                fTemp
                                    += abs(pImage[edgeList_White[nEdge].m_x] - pbyLeftImage[edgeList_White[nEdge].m_x]);
                                nTempCount++;
                            }
                        }

                        //LEFT
                        if (edgeList_White[nEdge].m_x - 1 > 0)
                        {
                            if (pLabelImageLeft[edgeList_White[nEdge].m_x - 1] != curBlob.m_label)
                            {
                                fTemp
                                    += abs(pImage[edgeList_White[nEdge].m_x] - pbyLeftImage[edgeList_White[nEdge].m_x]);
                                nTempCount++;
                            }
                            else
                            {
                                if (pImageWhite[edgeList_White[nEdge].m_x - 1] != 255)
                                {
                                    fTemp += abs(
                                        pImage[edgeList_White[nEdge].m_x] - pbyLeftImage[edgeList_White[nEdge].m_x]);
                                    nTempCount++;
                                }
                            }
                        }

                        //RIGHT
                        if (edgeList_White[nEdge].m_x + 1 < srcImage.GetSizeX())
                        {
                            if (pLabelImageRight[edgeList_White[nEdge].m_x + 1] != curBlob.m_label)
                            {
                                fTemp += abs(
                                    pImage[edgeList_White[nEdge].m_x] - pbyRightImage[edgeList_White[nEdge].m_x]);
                                nTempCount++;
                            }
                            else
                            {
                                if (pImageWhite[edgeList_White[nEdge].m_x + 1] != 255)
                                {
                                    fTemp += abs(
                                        pImage[edgeList_White[nEdge].m_x] - pbyLeftImage[edgeList_White[nEdge].m_x]);
                                    nTempCount++;
                                }
                            }
                        }

                        if (nTempCount == 0)
                        {
                            nCheckArea++;
                            continue;
                        }
                        fTemp = fTemp / nTempCount;
                        fEnergyWhite += fTemp;
                    }
                    fEnergyWhite = fEnergyWhite / (edgeList_White.size() - nCheckArea);

                }
                else
                {
                    fEnergyWhite = 0;
                }
                //GetObjectLabel(labelImage, nextLinkedBlob.m_roi, nextLinkedBlob.m_label, edgeListCalc);
            }

            nextLinkedBlobIndex = nextLinkedBlob.m_nextLinkedBlobIndex;
        }




    }

    return ret;
}
float CProcessingBase::Calcircularity(const IPVM::Point_32f_C2& massPoint, const IPVM::Image_16u_C1& labelimage, const IPVM::Rect roi, const int& LabelNum)
{
	int nDegree = 20;
	// 20도씩 돌면서 중심점을 기준으로 가장 먼 거리를 측정
	// 이 거리들의 표준편차를 통해서 얼마나 원에 가까운지 판단.
	// 0에 가까울수록 원이라고 판단.

	std::vector<float> vecDistance;
	IPVM::Rect ROI = roi;
	ROI.InflateRect(10, 10);
	ROI &= IPVM::Rect(labelimage);
	
	const auto& MassPoint_X = massPoint.m_x;
	const auto& MassPoint_Y = massPoint.m_y;
	const auto& image = labelimage;

	for (int degree = 1; degree < 360; degree += nDegree)
	{
		float fCos = cos(degree * (M_PI / 180));
		float fSin = sin(degree * (M_PI / 180));
		float fTan = tan(degree * (M_PI / 180));
		if ((degree > 0 && degree < 45) || (degree > 315 && degree < 360))
		{
			for (int x = ROI.m_right-1; x >= MassPoint_X; x--)
			{
				int zerobase_X = abs(MassPoint_X - x);
				int y;
				y = int(fTan * zerobase_X) + MassPoint_Y;
				if (0 > y || image.GetSizeY() <= y)
					continue;
				if (image.GetValue(x, y) == LabelNum)
				{
					
					vecDistance.push_back(sqrt(pow(MassPoint_X - x, 2) + pow(2 * (MassPoint_Y - y), 2)));
					break;
				}
				if (false == (x - 1 >= MassPoint_X))
					vecDistance.push_back(0.0f);
			}
		}
		else if (degree > 45 && degree < 135)
		{
			for (int y = ROI.m_top; y <= MassPoint_Y; y++)
			{
				int zerobase_Y = abs(MassPoint_Y - y);
				int x;
				if (0.0f == fTan || 0 == zerobase_Y)
					x = MassPoint_X;
				else
					x = int(1 / fTan * zerobase_Y) + MassPoint_X;
				if (0 > x || image.GetSizeX() <= x)
					continue;
				if (image.GetValue(x, y) == LabelNum)
				{
					vecDistance.push_back(sqrt(pow(MassPoint_X - x, 2) + pow(2 * (MassPoint_Y - y), 2)));
					break;
				}
				if (false == (y + 1 <= MassPoint_Y))
					vecDistance.push_back(0.0f);
			}
		}
		else if ((degree > 135 && degree < 225))
		{
			for (int x = ROI.m_left; x <= MassPoint_X; x++)
			{
				int zerobase_X = abs(MassPoint_X - x);
				int y;
				y = int(fTan * zerobase_X) + MassPoint_Y;
				if (0 > y || image.GetSizeY() <= y)
					continue;
				if (image.GetValue(x, y) == LabelNum)
				{
					vecDistance.push_back(sqrt(pow(MassPoint_X - x, 2) + pow(2 * (MassPoint_Y - y), 2)));
					break;
				}
				if (false == (x + 1 <= MassPoint_X))
					vecDistance.push_back(0.0f);
			}
		}
		else if (degree > 225 && degree < 315)
		{
			for (int y = ROI.m_bottom-1; y >= MassPoint_Y; y--)
			{
				int zerobase_Y = abs(MassPoint_Y - y);
				int x;
				if (0.0f == fTan)
					x = MassPoint_X;
				else
					x = int(1 / fTan * zerobase_Y) + MassPoint_X;
				if (0 > x || image.GetSizeX() <= x)
					continue;
				if (image.GetValue(x, y) == LabelNum)
				{
					vecDistance.push_back(sqrt(pow(MassPoint_X - x, 2) + pow(2 * (MassPoint_Y - y), 2)));
					break;
				}
				if (false == (y - 1 >= MassPoint_Y))
					vecDistance.push_back(0.0f);
			}
		}

	}

	float fAverage = 0.0f;
	float fDeviation = 0.0;
	int nCount = vecDistance.size();
	std::sort(vecDistance.begin(), vecDistance.end(), std::greater<float>());// 최대값을 100으로 만들어서 편차를 구함.
	float fMaxDsitance = 0.1f; // 분모 0되는거 방지
	float fMinDsitance = 0.0f;
	float* arrDistance = new float[nCount];
	
	if (0 != nCount)
	{
		fMaxDsitance = vecDistance[0];
		fMinDsitance = vecDistance[nCount - 1];
	}
	for (int i = 0; i < nCount; i++)
	{

		auto value = (vecDistance[i] - fMinDsitance) / (fMaxDsitance - fMinDsitance) * 100;//정규화 100으로
		arrDistance[i] = pow(value,2);
		/*if (0 == i)
			fMaxDsitance = vecDistance[i];

		if (0.0f != fMaxDsitance)
			arrDistance[i] = pow(vecDistance[i] / fMaxDsitance * 100, 2);
		else
			arrDistance[i] = 0;*/
	}
	IPVM::Mathematics::GetAverage(arrDistance, nCount, fAverage); //평균 구하기
	IPVM::Mathematics::GetStdDeviation(arrDistance, nCount, fAverage, fDeviation); //표준편차 구하기
	delete[] arrDistance;
	return fDeviation;
}

void CProcessingBase::ResetResult(II_RESULT_VALUE result)
{
	m_rejectInfo.Clear();
	m_resultValue = result;
}

void CProcessingBase::SetPixelToUmX(double x)
{
	m_pixelToUmX = x;
}

void CProcessingBase::SetPixelToUmY(double y)
{
	m_pixelToUmY = y;
}

void CProcessingBase::SetCellCuttingMargin(long nMargin)
{
	m_nCellCuttingMargin = nMargin;
}

void CProcessingBase::SetCellDesign(CELL_DESIGN design)
{
	m_cellDesign = design;
}

II_RESULT_VALUE	CProcessingBase::GetInspResult() const
{
	return m_resultValue;
}

LPCTSTR CProcessingBase::GetInspResultString() const
{
	CString strResult;
	switch (m_resultValue)
	{
	case II_RESULT_VALUE::PASS:			return _T("Pass\r\n");
	case II_RESULT_VALUE::REWORK:		return _T("Rework\r\n");
	case II_RESULT_VALUE::REJECT:		return _T("Reject\r\n");
	case II_RESULT_VALUE::INVALID:		return _T("Invalid\r\n");
	case II_RESULT_VALUE::NOT_MEASURED:	return _T("Not Measured\r\n");
	}

	return _T("");
}

void CProcessingBase::MakeLower(IPVM::Image_8u_C1& iImageSrc, IPVM::Rect rect, int threshold, IPVM::Image_8u_C1& iImageDst)
{
	rect = rect & IPVM::Rect(iImageSrc);
	BYTE* imageScr = iImageSrc.GetMem();
	BYTE* imageDst = iImageDst.GetMem();

	long width = iImageSrc.GetWidthBytes();
	long index;

	for (long y = rect.m_top; y < rect.m_bottom; y++)
	{
		for (long x = rect.m_left; x < rect.m_right; x++)
		{
			index = x + y * width;
			imageDst[index] = min(threshold, imageScr[index]);
		}
	}
}

void CProcessingBase::MakeUpper(IPVM::Image_8u_C1& iImageSrc, IPVM::Rect rect, int threshold, IPVM::Image_8u_C1& iImageDst, int nMakeUpperValue)
{
	int nOffset = 1;
	if (0 == nMakeUpperValue)
	{	
		nMakeUpperValue = threshold;
	}
	else
	{
		nOffset = 3;	// 엣지원래 기준은 2픽셀 이내 이기 때문에 3개면 충분함 
	}

	rect = rect & IPVM::Rect(iImageSrc);
	BYTE* imageScr = iImageSrc.GetMem();
	BYTE* imageDst = iImageDst.GetMem();

	long width = iImageSrc.GetWidthBytes();
	long index;

	for (long y = rect.m_top; y < rect.m_bottom; y++)
	{
		for (long x = rect.m_left; x < rect.m_right; x++)
		{
			index = x + y * width;
			if (threshold > imageScr[index])
			{
				imageDst[index] = nMakeUpperValue;
				
				if (1 < nOffset)
				{
					imageDst[index - 2] = nMakeUpperValue;
					imageDst[index -1] = nMakeUpperValue;
					imageDst[index + 1] = nMakeUpperValue;
					imageDst[index + 2] = nMakeUpperValue;
				}

			}
			else
			{
				imageDst[index] = imageScr[index];
			}
		}
	}
}

    // CInspectionBase
CInspectionBase::CInspectionBase(CProcessingBase* parent, const ProcItemCategory itemCategory, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: CProcessingBase(parent, itemCategory, logger, loggerResult)
{
}

CInspectionBase::~CInspectionBase()
{
}
