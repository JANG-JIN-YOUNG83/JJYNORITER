#include "stdafx.h"
#include "InspectionMeasurePara.h"
#include "Accessor.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "InspectionConstantPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionMeasurePara::CInspectionMeasurePara()
	: m_bSaveMeasureResult				(_T("Save Measure Result"), false)
	, m_fCellLengthCalcPosition			(_T("Cell Length Calc Position"), 5000.f, (DefectType)4, DefectType::LeftLength, DefectType::RightLength, DefectType::TopTilt, DefectType::BottomTilt)	// um
	, m_nFoundTopCuttingHeightStartPoint		(_T("Top Cutting Height Detection Start Point"), 5, (DefectType)2, DefectType::TopFrontCuttingHeight, DefectType::TopBackCuttingHeight)
	, m_nFoundBotCuttingHeightStartPoint		(_T("Bot Cutting Height Detection Start Point"), 25, (DefectType)2, DefectType::BottomFrontCuttingHeight, DefectType::BottomBackCuttingHeight)
	, m_imageUseIndex					(_T("Image Use Index"),0)
	, m_nCountMeasureItem				(_T("Count Measure Item"), 9)
	, m_nMSADistance					(_T("MSA Line To Point Distance"), 0)
    , m_bUseMSA(_T("Use MSA"), 0)
    , m_nImageDark(_T("Select ImageType"), 0)
    , m_bUseMSAAlign(_T("Use MSA Align"), 0)
{
	m_vecMeasureType.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureStart.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureEnd.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureStartPosX.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureEndPosX.reserve(COUNT_CELL_MEASURE);

	m_vecMeasureStandard.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureOffsetUSL.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureOffsetLSL.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureOffsetUCL.reserve(COUNT_CELL_MEASURE);
	m_vecMeasureOffsetLCL.reserve(COUNT_CELL_MEASURE);

	m_vecrtDetectLineMSA.reserve(COUNT_CELL_MEASURE);
    m_vecDetectLineParaMSA.reserve(COUNT_CELL_MEASURE);

	for (long cnt = 0; cnt < COUNT_CELL_MEASURE; cnt++)
	{
		CString strType, strStart, strEnd, strStartPosX, strEndPosX;
		CString strStandard, strOffsetUSL, strOffsetLSL, strOffsetUCL, strOffsetLCL;

		strType.Format(_T("measure_item_type_%d"), cnt);
		strStart.Format(_T("measure_item_start_index_%d"), cnt);
		strEnd.Format(_T("measure_item_end_index_%d"), cnt);
		strStartPosX.Format(_T("measure_item_start_posX_%d"), cnt);
		strEndPosX.Format(_T("measure_item_end_posX_%d"), cnt);

		strStandard.Format(_T("measure_item_spec_standard_%d"), cnt);
		strOffsetUSL.Format(_T("measure_item_spec_offset_USL_%d"), cnt);
		strOffsetLSL.Format(_T("measure_item_spec_offset_LSL_%d"), cnt);
		strOffsetUCL.Format(_T("measure_item_spec_offset_UCL_%d"), cnt);
		strOffsetLCL.Format(_T("measure_item_spec_offset_LCL_%d"), cnt);

		m_vecMeasureType.push_back(Base::ParaLong(strType, cnt));
		m_vecMeasureStart.push_back(Base::ParaLong(strStart, 0));
		m_vecMeasureEnd.push_back(Base::ParaLong(strEnd, 1));
		m_vecMeasureStartPosX.push_back(Base::ParaFloat(strStart, 0.));
		m_vecMeasureEndPosX.push_back(Base::ParaFloat(strEnd, 1.));

		m_vecMeasureStandard.push_back(Base::ParaFloat(strStandard, 100.5));
		m_vecMeasureOffsetUSL.push_back(Base::ParaFloat(strOffsetUSL, 0.5));
		m_vecMeasureOffsetLSL.push_back(Base::ParaFloat(strOffsetLSL, -0.5));
		m_vecMeasureOffsetUCL.push_back(Base::ParaFloat(strOffsetUCL, 0.5));
		m_vecMeasureOffsetLCL.push_back(Base::ParaFloat(strOffsetLCL, -0.5));

		CString strLine, strDetectLinePara;

		strLine.Format(_T("MSA Align Detect Line %02d"), cnt);
        strDetectLinePara.Format(_T("MSA Detect Line Para %02d"), cnt);

		IPVM::Rect rect;
        SetInitialDetectROI(cnt, rect);

        m_vecrtDetectLineMSA.push_back(Base::ParaRect(strLine, rect));
        m_vecDetectLineParaMSA.push_back(Base::ParaEdgeDetection(strDetectLinePara));
	}
}

void CInspectionMeasurePara::SetInitialDetectROI(int nIndex, IPVM::Rect& rtDetect)
{
    switch (nIndex)
    {
        case 0:
        {
            rtDetect.SetRect((int)_enumROIStartPos1::ptX, (int)_enumROIStartPos1::ptY, (int)_enumROIStartPos1::ptX2,
                (int)_enumROIStartPos1::ptY2);
            break;
        }
        case 1:
        {
            rtDetect.SetRect((int)_enumROIStartPos2::ptX, (int)_enumROIStartPos2::ptY, (int)_enumROIStartPos2::ptX2,
                (int)_enumROIStartPos2::ptY2);
            break;
        }
    }
}

CInspectionMeasurePara::~CInspectionMeasurePara()
{
}

void CInspectionMeasurePara::LinkDataBase(bool bSave, bool isUpCamera, Database &db)
{
	m_nFoundTopCuttingHeightStartPoint.Link(bSave, db[_T("Top Cutting Height Detection Start Point")]);
	m_nFoundBotCuttingHeightStartPoint.Link(bSave, db[_T("Bot Cutting Height Detection Start Point")]);
	m_imageUseIndex.Link(bSave, db[_T("Image Use Index")]);
	m_bSaveMeasureResult.Link(bSave, db[_T("Save Measure Result")]);
	m_fCellLengthCalcPosition.Link(bSave, db[_T("Cell Length Calc Position")]);

	// 코터
	m_nCountMeasureItem.Link(bSave, db[_T("Count Measure Item")]);
	m_nMSADistance.Link(bSave, db[_T("MSA Line To Point Distance")]);
    m_bUseMSA.Link(bSave, db[_T("Use MSA")]);
    m_bUseMSAAlign.Link(bSave, db[_T("Use MSA Align")]);


	if (bSave == false)
	{
		m_vecMeasureType.clear();
		m_vecMeasureStart.clear();
		m_vecMeasureStartPosX.clear();
		m_vecMeasureEnd.clear();
		m_vecMeasureEndPosX.clear();
		m_vecMeasureType.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureStart.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureStartPosX.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureEnd.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureEndPosX.reserve(COUNT_CELL_MEASURE);

		m_vecMeasureStandard.clear();
		m_vecMeasureOffsetUCL.clear();
		m_vecMeasureOffsetLCL.clear();
		m_vecMeasureOffsetUSL.clear();
		m_vecMeasureOffsetLSL.clear();
		m_vecMeasureStandard.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureOffsetUSL.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureOffsetLSL.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureOffsetUCL.reserve(COUNT_CELL_MEASURE);
		m_vecMeasureOffsetLCL.reserve(COUNT_CELL_MEASURE);
	}

	for (long cnt = 0; cnt < COUNT_CELL_MEASURE; cnt++)
	{
		CString strType, strStart, strStartPosX, strEnd, strEndPosX;
		CString strStandard, strOffsetUSL, strOffsetLSL, strOffsetUCL, strOffsetLCL;

		strType.Format(_T("measure_item_type_%d"), cnt);
		strStart.Format(_T("measure_item_start_index_%d"), cnt);
		strStartPosX.Format(_T("measure_item_start_posX_%d"), cnt);
		strEnd.Format(_T("measure_item_end_index_%d"), cnt);
		strEndPosX.Format(_T("measure_item_end_posX_%d"), cnt);

		strStandard.Format(_T("measure_item_spec_standard_%d"), cnt);
		strOffsetUSL.Format(_T("measure_item_spec_offset_USL_%d"), cnt);
		strOffsetLSL.Format(_T("measure_item_spec_offset_LSL_%d"), cnt);
		strOffsetUCL.Format(_T("measure_item_spec_offset_UCL_%d"), cnt);
		strOffsetLCL.Format(_T("measure_item_spec_offset_LCL_%d"), cnt);

		if (bSave == false)
		{
			m_vecMeasureType.push_back(Base::ParaLong(strType, cnt));
			m_vecMeasureStart.push_back(Base::ParaLong(strStart, 0));
			m_vecMeasureStartPosX.push_back(Base::ParaFloat(strStartPosX, 0.f));
			m_vecMeasureEnd.push_back(Base::ParaLong(strEnd, 1));
			m_vecMeasureEndPosX.push_back(Base::ParaFloat(strEndPosX, 0.f));

			m_vecMeasureStandard.push_back(Base::ParaFloat(strStandard, 100.5));
			m_vecMeasureOffsetUSL.push_back(Base::ParaFloat(strOffsetUSL, 0.5));
			m_vecMeasureOffsetLSL.push_back(Base::ParaFloat(strOffsetLSL, -0.5));
			m_vecMeasureOffsetUCL.push_back(Base::ParaFloat(strOffsetUCL, 0.5));
			m_vecMeasureOffsetLCL.push_back(Base::ParaFloat(strOffsetLCL, -0.5));
		}
		m_vecMeasureType[cnt].Link(bSave, db[strType]);
		m_vecMeasureStart[cnt].Link(bSave, db[strStart]);
		m_vecMeasureStartPosX[cnt].Link(bSave, db[strStartPosX]);
		m_vecMeasureEnd[cnt].Link(bSave, db[strEnd]);
		m_vecMeasureEndPosX[cnt].Link(bSave, db[strEndPosX]);

		m_vecMeasureStandard[cnt].Link(bSave, db[strStandard]);
		m_vecMeasureOffsetUSL[cnt].Link(bSave, db[strOffsetUSL]);
		m_vecMeasureOffsetLSL[cnt].Link(bSave, db[strOffsetLSL]);
		m_vecMeasureOffsetUCL[cnt].Link(bSave, db[strOffsetUCL]);
		m_vecMeasureOffsetLCL[cnt].Link(bSave, db[strOffsetLCL]);

		CString strLine, strDetectLinePara;

		strLine.Format(_T("MSA Align Detect Line %02d"), cnt);
        strDetectLinePara.Format(_T("MSA Detect Line Para %02d"), cnt);
        m_vecrtDetectLineMSA[cnt].Link(bSave, db[strLine]);
        m_vecDetectLineParaMSA[cnt].Link(bSave, db[strDetectLinePara]);
        m_nImageDark.Link(bSave, db[_T("Select ImageType")]);
	}
}