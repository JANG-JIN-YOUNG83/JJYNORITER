#include "stdafx.h"
#include "InspectionRoot.h"
#include "DlgInspectionMain.h"
#include "Accessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

Root::Root(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: m_inspectionCellUp(logger, loggerResult)
	, m_inspectionCellDown(logger, loggerResult)
    , m_inspectionCell{CInspectionCell(logger, loggerResult), CInspectionCell(logger, loggerResult),
          CInspectionCell(logger, loggerResult), CInspectionCell(logger, loggerResult)}
	, m_pixelToUmXUp(48.)
	, m_pixelToUmXDown(48.)
	, m_pixelToUmY(100.)
{
	m_inspectionCellUp.SetUpDown(true);
	m_inspectionCellDown.SetUpDown(false);

	m_inspectionCellUp.Initialize();
	m_inspectionCellDown.Initialize();

	m_inspectionCell[0].SetUpDown((long)0);
    m_inspectionCell[0].Initialize();
    m_inspectionCell[1].SetUpDown((long)1);
    m_inspectionCell[1].Initialize();
    m_inspectionCell[2].SetUpDown((long)2);
    m_inspectionCell[2].Initialize();
    m_inspectionCell[3].SetUpDown((long)3);
    m_inspectionCell[3].Initialize();
}


Root::~Root()
{
}

bool Root::ShowDlg(int nLanguage, std::map<long, std::vector<CString>> mapExplainParamDimension, SetupImage& setupImage, long nType)
{
	// 이 코드가 없으면 Dialog 가 보이지 않음
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Dialog 생성
    CDlgInspectionMain Dlg(this, nLanguage, setupImage, nType);
	Dlg.m_loginMode = m_loginMode;
	Dlg.m_mapExplainParam = mapExplainParamDimension;
	Dlg.DoModal();
	return Dlg.m_needToRecipeSave;
}

void Root::LinkDataBase(bool bSave, Database &dbJob)
{
	m_inspectionCellUp	.LinkDataBase(bSave, dbJob[_T("Inspection")][_T("Up")]);
	m_inspectionCellDown.LinkDataBase(bSave, dbJob[_T("Inspection")][_T("Down")]);
    
	////일단 up으로 동작되서, 업으로 링크 시킴
	//m_inspectionCell[0].LinkDataBase(bSave, dbJob[_T("Inspection")][_T("Up")]);
 //   m_inspectionCell[1].LinkDataBase(bSave, dbJob[_T("Inspection")][_T("Up")]);
 //   m_inspectionCell[2].LinkDataBase(bSave, dbJob[_T("Inspection")][_T("Up")]);
 //   m_inspectionCell[3].LinkDataBase(bSave, dbJob[_T("Inspection")][_T("Up")]);

	LinkDataBaseCommon(bSave, dbJob);
}

void Root::LinkDataBaseCommon(bool bSave, Database &dbJob)
{
	long offsetVersion = 20220113;
	if (!dbJob[_T("Offset Version")].Link(bSave, offsetVersion)) offsetVersion = 0;

	auto& criteriaList = Inspection::Accessor::Get().m_criteriaList;
	auto& inspMergeOptions = Inspection::Accessor::Get().m_inspMergeOptions;
	auto& measureOffsets = Inspection::Accessor::Get().m_measureOffsets;
	auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;

	switch(offsetVersion)
	{
	case 0:
		if (true)
		{
			// 구버전 Recipe 호환용
			// 최신버전에 항목 추가시 내용을 추가할 필요 없습니다

			auto& dbUp = dbJob[_T("Inspection")][_T("Up")];
			auto& dbDn = dbJob[_T("Inspection")][_T("Down")];

			double shoulderLineOffsetTop_um = 0.;
			double shoulderLineOffsetBot_um = 0.;

			float cellLength_um = 0.f;
			float misalign_um = 0.f;
			float mismatch_um = 0.f;
			float tabHeight_um = 0.f;
			float tabPitch_um = 0.f;
			float tabSide_um = 0.f;
			float tabWidth_um = 0.f;
			float vhomeHeight_um = 0.f;
			float vhomeTiltTop_um = 0.f;
			float vhomeTiltBot_um = 0.f;

			float topFrontCuttingHeight_um = 0.f;
			float bottomFrontCuttingHeight_um = 0.f;
			float topBackCuttingHeight_um = 0.f;
			float bottomBackCuttingHeight_um = 0.f;

			// Up
			dbUp[_T("ShoulderLine")][_T("Shoulder Line Offset")].Link(bSave, shoulderLineOffsetTop_um);
			dbUp[_T("Measure")][_T("Cell Length Offset")].Link(bSave, cellLength_um);
			dbUp[_T("Measure")][_T("Cutting Height Top Offset")].Link(bSave, topFrontCuttingHeight_um);
			dbUp[_T("Measure")][_T("Cutting Height Bot Offset")].Link(bSave, bottomFrontCuttingHeight_um);
			dbUp[_T("Measure")][_T("Cell Pitch Offset")].Link(bSave, tabPitch_um);
			dbUp[_T("Measure")][_T("Tab Height Offset")].Link(bSave, tabHeight_um);
			dbUp[_T("Measure")][_T("Tab Side Offset")].Link(bSave, tabSide_um);
			dbUp[_T("Measure")][_T("Tab Width Offset")].Link(bSave, tabWidth_um);
			dbUp[_T("Measure")][_T("Misalign Offset")].Link(bSave, misalign_um);
			dbUp[_T("Measure")][_T("Mismatch Offset")].Link(bSave, mismatch_um);

			dbUp[_T("Measure")][_T("Vhome Height Offset")].Link(bSave, vhomeHeight_um);
			dbUp[_T("Measure")][_T("Vhome Tilt Top Offset")].Link(bSave, vhomeTiltTop_um);
			dbUp[_T("Measure")][_T("Vhome Tilt Bot Offset")].Link(bSave, vhomeTiltBot_um);

			// Down
			dbDn[_T("ShoulderLine")][_T("Shoulder Line Offset")].Link(bSave, shoulderLineOffsetBot_um);
			dbDn[_T("Measure")][_T("Cutting Height Top Offset")].Link(bSave, topBackCuttingHeight_um);
			dbDn[_T("Measure")][_T("Cutting Height Bot Offset")].Link(bSave, bottomBackCuttingHeight_um);

			measureOffsets.SetValue_mm(DefectType::TopCoating, shoulderLineOffsetTop_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::BottomCoating, shoulderLineOffsetBot_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::LeftLength, cellLength_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::RightLength, cellLength_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::Misalign, misalign_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::Mismatch, mismatch_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::TabHeight, tabHeight_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::TabSide, tabSide_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::TabPitch, tabPitch_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::TabWidth, tabWidth_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::TopVHomeHeight, vhomeHeight_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::TopTilt, vhomeTiltTop_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::BottomTilt, vhomeTiltBot_um / 1000.);

			measureOffsets.SetValue_mm(DefectType::TopFrontCuttingHeight, topFrontCuttingHeight_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::BottomFrontCuttingHeight, bottomFrontCuttingHeight_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::TopBackCuttingHeight, topBackCuttingHeight_um / 1000.);
			measureOffsets.SetValue_mm(DefectType::BottomBackCuttingHeight, bottomBackCuttingHeight_um / 1000.);
		}
		break;

	case 1:
		// 구버전 Recipe 호환용
		measureOffsets.LinkDataBase(bSave, dbJob);
		break;

	default:
		measureOffsets.LinkDataBase(bSave, dbJob[_T("Measure Offsets")]);
		break;
	}

	if (offsetVersion < 20211020)
	{
		// 구버전 Recipe 호환용
		auto& dbUp = dbJob[_T("Inspection")][_T("Up")];
		auto& dbDn = dbJob[_T("Inspection")][_T("Down")];

		auto& dbMeasureUp = dbUp[_T("Measure")];
		auto& dbMeasureDn = dbDn[_T("Measure")];

		criteriaList.m_tabPitch.LoadDataBaseOld(dbMeasureUp, _T("Cell Pitch Tolerance -"), _T("Cell Pitch Tolerance +"));
		criteriaList.m_tabWidth.LoadDataBaseOld(dbMeasureUp, _T("Tab Width Tolerance -"), _T("Tab Width Tolerance +"));
		criteriaList.m_tabHeight.LoadDataBaseOld(dbMeasureUp,_T("Tab Height Tolerance -"), _T("Tab Height Tolerance +"));
		criteriaList.m_tabSide.LoadDataBaseOld(dbMeasureUp, _T("Tab Side Tolerance -"), _T("Tab Side Tolerance +"));
		criteriaList.m_mismatch.LoadDataBaseOld(dbMeasureUp, _T("Mismatch Tolerance -"), _T("Mismatch Tolerance +"));
		criteriaList.m_cellLength.LoadDataBaseOld(dbMeasureUp,_T("Cell Length Tolerance -"), _T("Cell Length Tolerance +"));

		criteriaList.m_shoulderLineTop.LoadDataBaseOld(dbUp[_T("ShoulderLine")], _T("Spec Min Use"), _T("Spec Max Use"), _T("Spec Min Value"), _T("Spec Max Value"));
		criteriaList.m_shoulderLineBot.LoadDataBaseOld(dbDn[_T("ShoulderLine")], _T("Spec Min Use"), _T("Spec Max Use"), _T("Spec Min Value"), _T("Spec Max Value"));

		criteriaList.m_vhomeTiltTop.LoadDataBaseOld(dbMeasureUp, _T("Vhome Tilt Top Tolerance -"), _T("Vhome Tilt Top Tolerance +"));
		criteriaList.m_vhomeTiltBot.LoadDataBaseOld(dbMeasureUp, _T("Vhome Tilt Bot Tolerance -"), _T("Vhome Tilt Bot Tolerance +"));
		
		criteriaList.m_vhomeHeight.LoadDataBaseOld(dbMeasureUp,
			_T("V Home Height Standard"),
			_T("V Home Height Minus Tolerance"),
			_T("V Home Height Plus Tolerance"));

		criteriaList.m_misalign.LoadDataBaseOld(dbMeasureUp, _T("Misalign Tolerance -"), _T("Misalign Tolerance +"));

		criteriaList.m_frontCuttingHeightTop.LoadDataBaseOld(dbMeasureUp,
			_T("Cutting Height Top Standard"),
			_T("Cutting Height Top Minus Tolerance"),
			_T("Cutting Height Top Plus Tolerance"));

		criteriaList.m_frontCuttingHeightBot.LoadDataBaseOld(dbMeasureUp,
			_T("Cutting Height Bot Standard"),
			_T("Cutting Height Bot Minus Tolerance"),
			_T("Cutting Height Bot Plus Tolerance"));

		criteriaList.m_backCuttingHeightTop.LoadDataBaseOld(dbMeasureDn,
			_T("Cutting Height Top Standard"),
			_T("Cutting Height Top Minus Tolerance"),
			_T("Cutting Height Top Plus Tolerance"));

		criteriaList.m_backCuttingHeightBot.LoadDataBaseOld(dbMeasureDn,
			_T("Cutting Height Bot Standard"),
			_T("Cutting Height Bot Minus Tolerance"),
			_T("Cutting Height Bot Plus Tolerance"));
	}
	else
	{
		criteriaList.LinkDataBase(bSave, dbJob[_T("Criteria List")]);
	}

	inspMergeOptions.LinkDataBase(bSave, dbJob[_T("Merge Option")]);

	isi_spec.LinkDataBase(bSave, dbJob[_T("ISI spec")]);

	if (offsetVersion >= 20220113)
	{
		// 2022/01/13 부터 Option Criteria는 Accesor가 가지게 되었다
		Inspection::Accessor::Get().m_optionCriteriaList.LinkDataBase(bSave, dbJob[_T("Option Criteria List")]);
	}
}

bool Root::CopyDataBase(CProcessingBase* src)
{
	ASSERT(!_T("InspectionRoot는 복사를 지원하지 않는다."));
	return false;
}

CInspectionCell* Root::GetInspectionCell(bool isUp)
{
	if (isUp)
	{
		return &m_inspectionCellUp;
	}

	return &m_inspectionCellDown;
}
CInspectionCell* Root::GetInspectionCell(long nSide)
{
    return &m_inspectionCell[nSide];
}

CProcessingBase* Root::GetProcessingItem(bool isUp, ProcItemCategory categoryIndex)
{
	if (isUp)
	{
		return m_inspectionCellUp.GetProcessingItem(categoryIndex);
	}

	return m_inspectionCellDown.GetProcessingItem(categoryIndex);
}

CProcessingBase* Root::GetProcessingItem(long nSide, ProcItemCategory categoryIndex)
{
    return m_inspectionCell[nSide].GetProcessingItem(categoryIndex);
}

void Root::SetPixelToUmX(double xup, double xdown)
{
	m_pixelToUmXUp = xup;
	m_pixelToUmXDown = xdown;
	m_inspectionCellUp.SetPixelToUmX(xup);
	m_inspectionCellDown.SetPixelToUmX(xdown);
}

void Root::SetPixelToUmX(double x, long nSide)
{
    m_pixelToUmXUp = x;
    m_inspectionCell[nSide].SetPixelToUmX(x);
    
	//m_pixelToUmXUp = xup;
	//m_pixelToUmXDown = xdown;
    //m_inspectionCellUp.SetPixelToUmX(xup);
    //m_inspectionCellDown.SetPixelToUmX(xdown);
}

void Root::SetVersion(int nGenerationversion, int nSite)
{
	m_inspectionCellUp.SetVersion(nGenerationversion, nSite);
	m_inspectionCellDown.SetVersion(nGenerationversion, nSite);
}


void Root::SetPixelToUmY(double y)
{
	m_pixelToUmY = y;

	m_inspectionCellUp.SetPixelToUmY(y);
	m_inspectionCellDown.SetPixelToUmY(y);
}

void Root::SetPixelToUmY(double y, long nSide)
{
    m_pixelToUmY = y;
    m_inspectionCell[nSide].SetPixelToUmY(y);

    //m_inspectionCellUp.SetPixelToUmY(y);
    //m_inspectionCellDown.SetPixelToUmY(y);
}


void Root::SetCellCuttingMargin(long nMargin)
{
	m_nCellCuttingMargin = nMargin;

	m_inspectionCellUp.SetCellCuttingMargin(nMargin);
	m_inspectionCellDown.SetCellCuttingMargin(nMargin);

	m_inspectionCell[0].SetCellCuttingMargin(nMargin);
    m_inspectionCell[1].SetCellCuttingMargin(nMargin);
    m_inspectionCell[2].SetCellCuttingMargin(nMargin);
    m_inspectionCell[3].SetCellCuttingMargin(nMargin);
}

void Root::SetCellDesign(const CELL_DESIGN& design)
{
	m_cellDesign = design;

	m_inspectionCellUp.SetCellDesign(design);
	m_inspectionCellDown.SetCellDesign(design);

	m_inspectionCell[0].SetCellDesign(design);
    m_inspectionCell[1].SetCellDesign(design);
    m_inspectionCell[2].SetCellDesign(design);
    m_inspectionCell[3].SetCellDesign(design);
}

void Root::SetSurfaceOption(const bool& bUseISI)
{
	m_inspectionCellUp.SetSurfaceOption(bUseISI);
	m_inspectionCellDown.SetSurfaceOption(bUseISI);

	m_inspectionCell[0].SetSurfaceOption(bUseISI);
	m_inspectionCell[1].SetSurfaceOption(bUseISI);
	m_inspectionCell[2].SetSurfaceOption(bUseISI);
	m_inspectionCell[3].SetSurfaceOption(bUseISI);
}