#include "stdafx.h"
#include "InspectionBlobPara.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../SharedModule/CommonShare/DefectDef.h"
#include "InspectionConstantPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBlobParaRoi::CBlobParaRoi(CString name)
	: m_name(name)
	, m_bUseInspection(_T("Use Inspection"), true)
	, m_nThreshold(_T("Blob Threshold"), 50)
	, m_nThreshold2(_T("Blob Threshold2"), 0)
    , m_bUseMeanRangeTheshold(_T("Use Mean Range Threshold"), true)
	, m_fBlobMinSize_mm(_T("Blob Threshold"), 5.f)
	, m_nMassCenterGVsize(_T("MassCenterGV Size"), 0)
	, m_nThresholdHorDiv(_T("Threshold Hor Div"), 1)
	, m_nThresholdVerDiv(_T("Threshold Ver Div"), 1)
	, m_nBlobMergeDistance(_T("Blob Merge Distance"), 0)
    , m_bUseIntersectMerge(_T("Blob Intersect Merge"), true)
	, m_nBlobMaxCount(_T("Blob Threshold"), 500)
	, m_nTimeOut_ms(_T("Blob Threshold"), 500)
	, m_fBlobMinLength_mm(_T("Blob Threshold"), 50.f)
	, m_nImageType(_T("Source Image Type"), 0)
	, m_nBlobBrightType(_T("Blob Bright Type"), 0)
	, m_nInspectionType(_T("Inspection Type"), 0)
	, m_nInspectionArea(_T("Inspection Area"), 0)
	, m_nSetTypeInspArea(_T("Adjust Roi Set Type"), 0)
	, m_rtOffsetRoi(_T("Adjust Para OffsetRoi"), IPVM::Rect(0, 0, 0, 0))
	, m_rtSetInspArea(_T("Insp Area Set"), IPVM::Rect(0, 0, 0, 0))
	, m_bUseEqual(_T("Use Equalization"), 0)
	, m_bUseEqualThreshold(_T("Use_Equalization_Threshold"), 0)
	, m_nEqualThresholdMax(_T("Equal_Threshold_Max"), 50)
	, m_nEqualThresholdMin(_T("Equal_Threshold_Min"), 50)
    , m_nEqualThreadCount(_T("Equal_Thread_Count"), 5)
{
}

CInspectionBlobPara::CInspectionBlobPara()
	:m_bUseSwapXY(_T("Use Swap XY"), 0) 
	,m_bUseDeleteIntersectBlob(_T("Delete Intersect Blob"), false)
{
	const int nDefaultAlignBlobRoi = 5;
	
	m_nTotalInspCount = 0;
	m_vecnInspROICount.reserve(nDefaultAlignBlobRoi);
	m_vecBlobParaCondition.reserve(nDefaultAlignBlobRoi);

	CString strBlob;
	for (int nInspCnt = 0; nInspCnt < nDefaultAlignBlobRoi; nInspCnt++)
	{
		m_vecnInspROICount.push_back(Base::ParaLong(_T(""), 1));
		strBlob.Format(_T("Inspection %03d"), nInspCnt);
		m_vecBlobParaCondition.push_back(CBlobParaRoi(strBlob));
		m_nTotalInspCount++;
	}
}

CInspectionBlobPara::~CInspectionBlobPara()
{
}

void CInspectionBlobPara::LinkDataBase(bool bSave, Database& db)
{
	long version = 20211203;
	if (!db[_T("Version")].Link(bSave, version))
	{
		version = 0;
	}
	switch (version)
	{
	case 0:
		break;
	default:
		break;
	}

	if (!db[_T("Blob Total Inspection Count")].Link(bSave, m_nTotalInspCount))
	{
		version = 0;
	}

	long countParaRoi = m_vecBlobParaCondition.size();
	if (!db[_T("Blob Para Roi Count")].Link(bSave, countParaRoi))
	{
		version = 0;
	}

	if (!bSave)
    {
        m_vecBlobParaCondition.clear();
        m_vecnInspROICount.clear();
        
		m_vecBlobParaCondition.reserve(countParaRoi);
        m_vecnInspROICount.reserve(countParaRoi);

		CString strBlob;
        for (int nInspCnt = 0; nInspCnt < countParaRoi; nInspCnt++)
        {
            m_vecnInspROICount.push_back(Base::ParaLong(_T(""), 1));
            strBlob.Format(_T("Inspection %03d"), nInspCnt);
            m_vecBlobParaCondition.push_back(CBlobParaRoi(strBlob));
        }
    }
	
	for (long condition = 0; condition < countParaRoi; condition++)
	{
		CString strDbSub;
		strDbSub.Format(_T("Blob Para ROI %d"), condition);
		auto& dbSub = db[strDbSub];
		CBlobParaRoi& blobParaRoi = m_vecBlobParaCondition[condition];

		blobParaRoi.m_bUseInspection.Link(bSave, dbSub[_T("Use Inspection")]);
		blobParaRoi.m_nThreshold.Link(bSave, dbSub[_T("Blob Threshold")]);
		blobParaRoi.m_nThreshold2.Link(bSave, dbSub[_T("Blob Threshold2")]);
        blobParaRoi.m_bUseMeanRangeTheshold.Link(bSave, dbSub[_T("Use Mean Range Threshold")]);
		blobParaRoi.m_fBlobMinSize_mm.Link(bSave, dbSub[_T("Blob Min Pixel Size")]);
		blobParaRoi.m_nMassCenterGVsize.Link(bSave, dbSub[_T("MassCenterGV Size")]);
		blobParaRoi.m_nThresholdHorDiv.Link(bSave, dbSub[_T("Threshold Hor Div")]);
		blobParaRoi.m_nThresholdVerDiv.Link(bSave, dbSub[_T("Threshold Ver Div")]);
		blobParaRoi.m_nBlobMergeDistance.Link(bSave, dbSub[_T("Blob Merge Distance")]);
        blobParaRoi.m_bUseIntersectMerge.Link(bSave, dbSub[_T("Blob Intersect Merge")]);
		blobParaRoi.m_nBlobMaxCount.Link(bSave, dbSub[_T("Blob Max Count")]);
		blobParaRoi.m_nTimeOut_ms.Link(bSave, dbSub[_T("Blob Timeout")]);
		blobParaRoi.m_fBlobMinLength_mm.Link(bSave, dbSub[_T("Blob Min Length")]);
		blobParaRoi.m_nImageType.Link(bSave, dbSub[_T("Source Image Type")]);
		blobParaRoi.m_nBlobBrightType.Link(bSave, dbSub[_T("Blob Bright Type")]);
		blobParaRoi.m_nInspectionType.Link(bSave, dbSub[_T("Inspection Type")]);
		
		blobParaRoi.m_nInspectionArea.Link(bSave, dbSub[_T("Inspection Area")]);
		blobParaRoi.m_nSetTypeInspArea.Link(bSave, dbSub[_T("Adjust Type")]);
		blobParaRoi.m_rtOffsetRoi.Link(bSave, dbSub[_T("Adjust Para OffsetRoi")]);
		blobParaRoi.m_rtSetInspArea.Link(bSave, dbSub[_T("Insp Area Set")]);
        blobParaRoi.m_bUseEqual.Link(bSave, dbSub[_T("Use Equalization")]);

		blobParaRoi.m_bUseEqualThreshold.Link(bSave, dbSub[_T("Use_Equalization_Threshold")]);
		blobParaRoi.m_nEqualThresholdMax.Link(bSave, dbSub[_T("Equal_Threshold_Max")]);
		blobParaRoi.m_nEqualThresholdMin.Link(bSave, dbSub[_T("Equal_Threshold_Min")]);
		blobParaRoi.m_nEqualThreadCount.Link(bSave, dbSub[_T("Equal_Thread_Count")]);

		m_bUseSwapXY.Link(bSave, dbSub[_T("Use Swap XY")]);
        m_bUseDeleteIntersectBlob.Link(bSave, dbSub[_T("Delete Intersect Blob")]);
		Sleep(1);
	}
}