#pragma once

#include "basedef.h"

#include "InspectionBase.h"
#include "InspectionMeasurePara.h"
#include "InspectionCell.h"
#include "MeasureValue.h"
#include "../VisionModule/Language/LanguageDef.h"
#include "Types/LineEq_32f.h"
class Database;

struct stMeasurementDraw
{
	IPVM::LineSeg_32f m_lineSegLeft;
	IPVM::LineSeg_32f m_lineSegRight;

	IPVM::Point_32f_C2 m_crosspointLeft;
	IPVM::Point_32f_C2 m_crosspointRight;
	stMeasurementDraw()
	{
		m_lineSegLeft = IPVM::LineSeg_32f(0, 0, 0, 0);
		m_lineSegRight = IPVM::LineSeg_32f(0, 0, 0, 0);
		m_crosspointLeft = IPVM::Point_32f_C2(0, 0);
		m_crosspointRight = IPVM::Point_32f_C2(0, 0);
	}
	void SetPoints(IPVM::Point_32f_C2& Left, IPVM::Point_32f_C2& Right)
	{
		m_crosspointLeft = Left;
		m_crosspointRight = Right;
	}
	void SetLines(IPVM::LineSeg_32f& Left, IPVM::LineSeg_32f& Right)
	{
		m_lineSegLeft = Left;
		m_lineSegRight = Right;
	}
};

class CInspectionMeasureEdgeDetectPara
{
public:
    std::vector<IPVM::EdgeDetectionPara> m_vecedgePara;
    std::vector<IPVM::Rect> m_vecrtDetect;
    std::vector<long> m_vecDetecCount;
    std::vector<float> m_vecRansacThreshold;
    std::vector<IPVM::EdgeDetectionFilter> m_vecedgeFilter;
    IPVM::AlgorithmObjectHandle<IPVM::EdgeDetection> m_edgeDetection;
};

class __INSPECTOIN_CLASS__ CInspectionMeasure : public CInspectionBase
{
public:
	CInspectionMeasure(CProcessingBase* parent, IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult);
	virtual ~CInspectionMeasure();

public:
	virtual bool IsInspectionEnabled() const override;
	virtual void ResetInspection();

	virtual void PrepareDebugInfo(Inspection::DebugInfo* debugInfo) override;
	virtual bool ProcInspection(Inspection::DebugInfo* debugInfo) override;
	void DrawMeasurePoints(Inspection::DebugInfo* debugInfo, stMeasurementDraw& Data, LanguageDef Def, bool bPointHead ,int inputsize);
	virtual void ProcInspectionEnd(Inspection::DebugInfo* debugInfo) override;

	virtual void LinkDataBase(bool bSave, Database &dbJob) override;
	virtual bool CopyDataBase(CProcessingBase* src) override;

	void SetMeasureUIDatas(const IPVM::Rect& InspROI, stMeasurementDraw& Datas);
    void AlignMSA(); //이름을 MSA


public:
	CInspectionMeasurePara m_para;
	
	Inspection::MeasureValue m_measureTabHeight;
	Inspection::MeasureValue m_measureTabHeight2;
	Inspection::MeasureValue m_measureTabPitch;
	Inspection::MeasureValue m_measureTabSide;
	Inspection::MeasureValue m_measureTabSide2;
	Inspection::MeasureValue m_measureTabWidth;
	Inspection::MeasureValue m_measureCellLengthTop;
	Inspection::MeasureValue m_measureCellLengthBot;
	Inspection::MeasureValue m_measureCellLength2_Top;
	Inspection::MeasureValue m_measureCellLength2_Bot;
	Inspection::MeasureValue m_measureCuttingHeightTop;
	Inspection::MeasureValue m_measureCuttingHeightBot;
	Inspection::MeasureValue m_measureMisalign;
	Inspection::MeasureValue m_measureVHomeTiltTop;
	Inspection::MeasureValue m_measureVHomeTiltBot;

	float m_fCellLengthTopUmOffset;	// um
	float m_fCellLengthBotUmOffset;	// um
	float m_fCellLengthTop2UmOffset;	// um
	float m_fCellLengthBot2UmOffset;	// um
	float m_fVhomeHeightLeftUm;
	float m_fVhomeHeightRightUm;

	IPVM::Rect_32f m_rectCuttingHeightTop;
	IPVM::Rect_32f m_rectCuttingHeightTopDebug;
	IPVM::Rect_32f m_rectCuttingHeightBot;

	IPVM::Rect_32f m_rectVhomeLeftTop;
	IPVM::Rect_32f m_rectVhomeRightTop;

	float m_fCellPitchErrorUm;	// um
	float m_fCellLengthTopErrorUm;	// um
	float m_fCellLengthBotErrorUm;	// um
	float m_fCellLength2ErrorUm;	// um
	float m_fTabWidthErrorUm;		// um
	float m_fTabHeightErrorUm;	// um
	float m_fTabHeight2ErrorUm;	// um
	float m_fMisalignErrorUm;		// um
	float m_fMisalignNormal;		// um
	float m_fMisalignDetail;		// um
	float m_fVhomeTiltBotErrorUm;	// um
	float m_fVhomeTiltTopErrorUm;	// um
	float m_fTabSideErrorUm;		// um
	
	std::vector<std::vector <float>> m_vecMSALength;
	std::vector<CString> m_vecMSAType;

    CInspectionMeasureEdgeDetectPara m_paraEdgeDetectMSA;

    IPVM::Point_32f_C2 m_ptMSALeft;
    IPVM::Point_32f_C2 m_ptMSARight;
    IPVM::Point_32f_C2 m_ptMSALeftExpand;
    
	IPVM::LineEq_32f m_MSAlineUp;
	IPVM::LineEq_32f m_MSAlineLeft;
	IPVM::LineEq_32f m_MSAlineRight;


	//debug
	/*IPVM::LineSeg_32f Debug_Length_lineSegLeft;
	IPVM::LineSeg_32f Debug_Length_lineSegRight;

	IPVM::Point_32f_C2 Debug_Length_crosspoint1;
	IPVM::Point_32f_C2 Debug_Length_crosspoint2;*/

	//UI 변수
	stMeasurementDraw m_Length;
	stMeasurementDraw m_Length2;
	stMeasurementDraw m_Height;
	stMeasurementDraw m_Height2;
	stMeasurementDraw m_CuttingHeightTop;
	stMeasurementDraw m_CuttingHeightBot;
	stMeasurementDraw m_VhomeHeightTop;
	stMeasurementDraw m_VhomeHeightBot;

private:
	CInspectionCell* m_parent;
};
