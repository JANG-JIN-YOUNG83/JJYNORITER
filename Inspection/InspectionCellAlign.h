#pragma once

#include "basedef.h"

#include "InspectionBase.h"
#include "InspectionCellAlignPara.h"

#include "Types/Point_32f_C2.h"
#include "Types//LineSeg_32f.h"
#include "Algorithm/AlgorithmObjectHandle.h"
#include "Algorithm/EdgeDetection.h"
#include "Algorithm/EdgeDetectionFilter.h"
#include "Algorithm/EdgeDetectionPara.h"

class CInspectionCellAlignDebugInfo
{
public:
	std::vector<IPVM::Point_32f_C2> m_vecSearch;
	std::vector<IPVM::Rect> m_vecrtBoundrySearch;
	std::vector<IPVM::Rect> m_vecrtvHomeMakeUpper;
    std::vector<long> m_nMeanValue_Bright;
    std::vector<long> m_nMeanValue_Dark;
};

class CInspectionCellAlignEdgeDetectPara
{
public:
	std::vector<IPVM::EdgeDetectionPara> m_vecedgePara;
	std::vector<IPVM::Rect> m_vecrtDetect;
	std::vector<long> m_vecDetecCount;
	std::vector<float> m_vecRansacThreshold;
	std::vector < IPVM::EdgeDetectionFilter> m_vecedgeFilter;
	IPVM::AlgorithmObjectHandle<IPVM::EdgeDetection> m_edgeDetection;
};
typedef struct _ImageGrayValue
{
    std::vector<long> vecGV_Bright;
    std::vector<long> vecGV_Dark;
} ImageGrayValue;

class CInspectionCell;
class Database;

class __INSPECTOIN_CLASS__ CInspectionCellAlign : public CProcessingBase
{
public:
	CInspectionCellAlign(CInspectionCell* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	virtual ~CInspectionCellAlign();

public:
	virtual void PrepareDebugInfo(Inspection::DebugInfo* debugInfo) override;
	virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) override;
	virtual void ProcInspectionEnd(Inspection::DebugInfo* debugInfo) override;
    void AutoFindEdgeDetectROI2();
	bool ProcInspection(bool isLeft);
	virtual void LinkDataBase(bool bSave, Database &dbJob) override;
	virtual bool CopyDataBase(CProcessingBase* src) override;

public:

	CInspectionCellAlignPara m_para;
	CInspectionCellAlignDebugInfo	m_debugInfo;

	CInspectionCellAlignEdgeDetectPara m_paraEdgeDetectMeasure;
	CInspectionCellAlignEdgeDetectPara m_paraEdgeDetectBlob;
	//CInspectionCellAlignEdgeDetectPara m_paraEdgeDetectMSA;
    IPVM::Rect m_rectImageValid;
    ImageGrayValue m_AlignGrayValue;

private:
	CInspectionCell* m_parent;
	//IPVM::Rect m_rectImageValid;
};
