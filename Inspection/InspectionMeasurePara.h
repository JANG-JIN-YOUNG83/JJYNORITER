#pragma once
#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaLong.h"
#include "../VisionModule/Base/ParaEdgeDetection.h"

class Database;

class CInspectionMeasurePara
{
public:
	CInspectionMeasurePara();
	~CInspectionMeasurePara();

public:
	void LinkDataBase(bool bSave, bool isUpCamera, Database &db);
    void SetInitialDetectROI(int nIndex, IPVM::Rect& rtDetect);

public:
	Base::ParaBool	m_bSaveMeasureResult;
	Base::ParaFloat	m_fCellLengthCalcPosition;	// um
	Base::ParaLong	m_imageUseIndex;

	Base::ParaLong	m_nFoundTopCuttingHeightStartPoint;
	Base::ParaLong	m_nFoundBotCuttingHeightStartPoint;

	// 코터
	std::vector< Base::ParaLong> m_vecMeasureType;
	std::vector< Base::ParaLong> m_vecMeasureStart;
	std::vector< Base::ParaLong> m_vecMeasureEnd;
	std::vector< Base::ParaFloat> m_vecMeasureStartPosX;
	std::vector< Base::ParaFloat> m_vecMeasureEndPosX;

	std::vector< Base::ParaFloat> m_vecMeasureStandard;
	std::vector< Base::ParaFloat> m_vecMeasureOffsetUSL;
	std::vector< Base::ParaFloat> m_vecMeasureOffsetLSL;
	std::vector< Base::ParaFloat> m_vecMeasureOffsetUCL;
	std::vector< Base::ParaFloat> m_vecMeasureOffsetLCL;

	Base::ParaLong	m_nCountMeasureItem;   // dimension 검사항목 개수

	Base::ParaLong m_nMSADistance;
    Base::ParaBool m_bUseMSA;
    Base::ParaBool m_bUseMSAAlign;

	std::vector<IPVM::Point_32f_C2> m_vecPtMSA;

	std::vector<Base::ParaRect> m_vecrtDetectLineMSA;
    std::vector<Base::ParaEdgeDetection> m_vecDetectLineParaMSA;

	Base::ParaLong m_nImageDark;
};
