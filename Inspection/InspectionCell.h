#pragma once

#include "basedef.h"
#include "ResultInfo.h"
//#include "../IBCI/ThreadControl.h"

#include "InspectionBase.h"
//#include "InspectionConfiguration.h"
#include "InspectionCellAlign.h"
//#include "InspectionUncoating.h"
//#include "InspectionUncoatingSurface.h"
//#include "InspectionScrab.h"
//#include "InspectionShoulderLine.h"
//#include "InspectionTape.h"
#include "InspectionMeasure.h"
//#include "InspectionTabFolding.h"
//#include "InspectionElectrodeTorn.h"
//#include "InspectionNgMark.h"
//#include "InspectionBurr.h"
//#include "InspectionEtc.h"
//#include "CInspectionISI.h"
#include "InspectionBlob.h"
#include "InspectionDeeplearning.h"
#include "CISIClassification.h"
#include "InspectionAbnormal.h"
#include "InspectionChattering.h"
#include "UnionCellResult.h"

class Database;
class SURFACE_RESULT_INFO;



class __INSPECTOIN_CLASS__ CInspectionCell : public CProcessingBase
{
public:
	CInspectionCell(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	~CInspectionCell();

public:
	void ResetInspection();
	void Initialize();
	void SetInspectionSplitImage(IPVM::Image_8u_C1 brightImage, IPVM::Image_8u_C1 darkImage);
	virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) override;

	virtual void PrepareDebugInfo(Inspection::DebugInfo* /*debugInfo*/) override {}		// 하는 일없음
	virtual void ProcInspectionEnd(Inspection::DebugInfo* /*debugInfo*/) override {}	// 하는 일없음

	virtual void LinkDataBase(bool bSave, Database &dbJob) override;
	virtual bool CopyDataBase(CProcessingBase* src) override;
	void ResetResult(II_RESULT_VALUE result = II_RESULT_VALUE::NOT_MEASURED);
	void ResetISI();
	void SetPixelToUmX(double x);
	void SetPixelToUmY(double y);
	void SetCellCuttingMargin(long nMargin);
	void SetImagePosition(long p);
	void SetCellDesign(const CELL_DESIGN& design);
	void SetTotalResult();
	SURFACE_RESULT_INFO GetSurfaceResult();
	SURFACE_RESULT_INFO GetSurfaceResult_ISI(BOOL bView = false);
	bool Filter_ISI(CISIDefectProperty& Defect, CString num, bool bisSkip);
	void SetVersion(int nGenerationversion, int nSite);
	void SetSurfaceOption(const bool& bUseISI);

	BOOL GetTotalResultInfo(RESULT_INFO& result);
	void MergeResultInfo();
	CProcessingBase* GetProcessingItem(ProcItemCategory categortIndex);
	CRect SetBoundary2(CRect rt, CRect rt2);
	void ImageSelectCopy(unsigned char* fmSrc, unsigned char* fmDest, int nSrcW, int nSrcH, CRect rtArea, int nDestW, int nDestH, int nDestSttX, int nDestSttY);
	int CheckColor(int nType);
	CString GetDefectName_G(int nCode);
	CString GetDefectNameWithCode(int nCode);
	int AreaAve1(BYTE* fm, CRect rt, int nw, int* nave);
	void PrePareInsp();

public:
	int						m_generationversion;
	int						m_site;
	bool					m_bUseISI;
	CISICellAllDefect		m_ISICellAllDefect;

	std::vector<CISIDefectProperty> m_vecResult_NG;
	std::vector<CISIDefectProperty> m_vecResult_OK;
	std::vector<CISIDefectProperty> m_vecResult_SKIP;

	int64_t m_imagePosition;
	long m_cellID;
	float				m_grabTime_ms;
	long				m_cellOffset;
	int					m_cellSizePixel;
	SYSTEMTIME			m_inputTime;
	int					m_nAnodeCathode;	// 양극 0, 음극 1
	int					m_nInsulateType;

	bool m_bInlineTest;
	BOOL m_isTest;

	CRect m_rtCell;
	CRect m_rtCellTab;
	double	m_dMiddleAve;

	BYTE* m_fmDefect[MAX_FULL_DEFECT_COUNT];//불량 이미지
	CRect	m_rtFullDefect[MAX_FULL_DEFECT_COUNT];//불량 영역
	
	IPVM::Rect_32f	m_rtRawDefect[MAX_FULL_DEFECT_COUNT][4];//불량 영역
	
	BOOL	m_bDefectColor[MAX_FULL_DEFECT_COUNT];//불량 color
	int		m_nFullDefectType[MAX_FULL_DEFECT_COUNT];//defect Code
	CString	m_strFullDefectName[MAX_FULL_DEFECT_COUNT];

	CRect	m_rtDetailRawDefect[MAX_FULL_DEFECT_COUNT];
	double m_dDetailFullArea[MAX_FULL_DEFECT_COUNT];
	double m_dDetailFullWidth[MAX_FULL_DEFECT_COUNT];
	double m_dDetailFullLength[MAX_FULL_DEFECT_COUNT];
	double	m_dDefectAreaSub[MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2];  // (0) pixel, (1) mm
	CRect	m_rtSubRawDefect[MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2];
	double	m_dDefectWidthSub[MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2];  // (0) pixel, (1) mm
	double	m_dDefectLengthSub[MOD_NUMBER][MAX_SPLIT_DEFECT][DT_TOTAL][2]; // (0) pixel, (1) mm

	RESULT_INFO m_cellResult;

private:
	IPVM::LoggerInterface& m_logger;

	UnionCellResult m_unionResultData;
	CProcessingBase* m_processingList[(long)ProcItemCategory::END];
};
