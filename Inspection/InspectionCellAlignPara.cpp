#include "stdafx.h"
#include "InspectionCellAlignPara.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../SharedModule/CommonShare/DefectDef.h"
#include "InspectionConstantPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionCellAlignPara::CInspectionCellAlignPara()
	: m_bCellDesignShowOverlay(_T("Cell Design Show Overlay"), false)
	, m_imageUseIndex(_T("Image Use Index"), 0)//, (DefectType)10/*가변인자 개수*/, DefectType::LeftLength, DefectType::RightLength, DefectType::TopTilt, DefectType::BottomTilt, DefectType::TabSide, DefectType::TabPitch, DefectType::TabHeight, DefectType::Misalign, DefectType::TopVHomeHeight, DefectType::BottomVHomeHeight)
	, m_bUseInspection(_T("Use Inspection"), true)//, (DefectType)13/*가변인자 개수*/, DefectType::LeftLength, DefectType::RightLength, DefectType::TopTilt, DefectType::BottomTilt, DefectType::TopCoating, DefectType::BottomCoating, DefectType::TabSide, DefectType::TabPitch, DefectType::TabHeight, DefectType::Misalign, DefectType::TopVHomeHeight, DefectType::BottomVHomeHeight, DefectType::TabShape)
	, m_nLineCountMeasure(_T("Detect Line Count(Measure)"), 6)
	, m_nLineCountBlob(_T("Detect Line Count(Blob)"), 6)
    , m_nSelectImageType(_T("Select ImageType"), 0)
    , m_nInspectionType(_T("Inspection Type"), 0)
    , m_bUseCenterROISplitMeasure(_T("Add Center ROI Split Measure"), false)
    , m_bUseCenterROISplitBlob(_T("Add Center ROI Split Blob"), false)
{
        m_vecrtDetectLineMeasure.reserve(COUNT_CELL_ROI);
        m_vecDetectLineParaMeasure.reserve(COUNT_CELL_ROI);

		m_vecrtDetectLineBlob.reserve(COUNT_CELL_ROI);
		m_vecDetectLineParaBlob.reserve(COUNT_CELL_ROI);

		/*m_vecrtDetectLineMSA.reserve(COUNT_CELL_ROI);
		m_vecDetectLineParaMSA.reserve(COUNT_CELL_ROI);*/

        CString strLine, strDetectLinePara;
        for (long cnt = 0; cnt < COUNT_CELL_ROI; cnt++)
        {
			strLine.Format(_T("Measure Align Detect Line %02d"), cnt);
            strDetectLinePara.Format(_T("Measure Detect Line Para %02d"), cnt);
			IPVM::Rect rect;
			SetInitialDetectROI(cnt, rect);

            m_vecrtDetectLineMeasure.push_back(Base::ParaRect(strLine, rect));
            m_vecDetectLineParaMeasure.push_back(Base::ParaEdgeDetection(strDetectLinePara));

			strLine.Format(_T("Blob Align Detect Line %02d"), cnt);
			strDetectLinePara.Format(_T("Blob Detect Line Para %02d"), cnt);

			m_vecrtDetectLineBlob.push_back(Base::ParaRect(strLine, rect));
			m_vecDetectLineParaBlob.push_back(Base::ParaEdgeDetection(strDetectLinePara));

			/*strLine.Format(_T("MSA Align Detect Line %02d"), cnt);
			strDetectLinePara.Format(_T("MSA Detect Line Para %02d"), cnt);

			m_vecrtDetectLineMSA.push_back(Base::ParaRect(strLine, rect));
			m_vecDetectLineParaMSA.push_back(Base::ParaEdgeDetection(strDetectLinePara));*/
        }
}

CInspectionCellAlignPara::~CInspectionCellAlignPara()
{
}

void CInspectionCellAlignPara::SetInitialDetectROI(int nIndex, IPVM::Rect& rtDetect)
{
	switch (nIndex)
	{
	case 0:
	{
		rtDetect.SetRect((int)_enumROIStartPos1::ptX, (int)_enumROIStartPos1::ptY, (int)_enumROIStartPos1::ptX2, (int)_enumROIStartPos1::ptY2);
		break;
	}
	case 1:
	{
		rtDetect.SetRect((int)_enumROIStartPos2::ptX, (int)_enumROIStartPos2::ptY, (int)_enumROIStartPos2::ptX2, (int)_enumROIStartPos2::ptY2);
		break;
	}
	case 2:
	{
		rtDetect.SetRect((int)_enumROIStartPos3::ptX, (int)_enumROIStartPos3::ptY, (int)_enumROIStartPos3::ptX2, (int)_enumROIStartPos3::ptY2);
		break;
	}
	case 3:
	{
		rtDetect.SetRect((int)_enumROIStartPos4::ptX, (int)_enumROIStartPos4::ptY, (int)_enumROIStartPos4::ptX2, (int)_enumROIStartPos4::ptY2);
		break;
	}
	case 4:
	{
		rtDetect.SetRect((int)_enumROIStartPos5::ptX, (int)_enumROIStartPos5::ptY, (int)_enumROIStartPos5::ptX2, (int)_enumROIStartPos5::ptY2);
		break;
	}
	case 5:
	{
		rtDetect.SetRect((int)_enumROIStartPos6::ptX, (int)_enumROIStartPos6::ptY, (int)_enumROIStartPos6::ptX2, (int)_enumROIStartPos6::ptY2);
		break;
	}
	default:
		break;
	}
}
void CInspectionCellAlignPara::LinkDataBase(bool bSave, Database &db)
{
	long version = 20211203;

	if (!db[_T("Version")].Link(bSave, version))
	{
		version = 0;
	}

	m_nLineCountMeasure.Link(bSave, db[_T("AlignLineCount")]);
	m_nLineCountBlob.Link(bSave, db[_T("AlignLineCount_Blob")]);

	CString strLine, strDetectLinePara;
	for (long cnt = 0; cnt < COUNT_CELL_ROI; cnt++)
	{
		strLine.Format(_T("Measure Align Detect Line %02d"), cnt);
		strDetectLinePara.Format(_T("Measure Detect Line Para %02d"), cnt);
		m_vecrtDetectLineMeasure[cnt].Link(bSave, db[strLine]);
		m_vecDetectLineParaMeasure[cnt].Link(bSave, db[strDetectLinePara]);

		strLine.Format(_T("Blob Align Detect Line %02d"), cnt);
		strDetectLinePara.Format(_T("Blob Detect Line Para %02d"), cnt);
		m_vecrtDetectLineBlob[cnt].Link(bSave, db[strLine]);
		m_vecDetectLineParaBlob[cnt].Link(bSave, db[strDetectLinePara]);

		/*strLine.Format(_T("MSA Align Detect Line %02d"), cnt);
		strDetectLinePara.Format(_T("MSA Detect Line Para %02d"), cnt);
		m_vecrtDetectLineMSA[cnt].Link(bSave, db[strLine]);
		m_vecDetectLineParaMSA[cnt].Link(bSave, db[strDetectLinePara]);*/
	}

	m_bUseCenterROISplitMeasure.Link(bSave, db[_T("Add Center ROI Split Measure")]);
    m_bUseCenterROISplitBlob.Link(bSave, db[_T("Add Center ROI Split Blob")]);

	m_bCellDesignShowOverlay.Link(bSave, db[_T("Cell Design Show Overlay")]);
    m_nSelectImageType.Link(bSave, db[_T("Select ImageType")]);
    m_nInspectionType.Link(bSave, db[_T("Inspection Type")]);
}
