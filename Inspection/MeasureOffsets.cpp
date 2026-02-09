#include "stdafx.h"
#include "MeasureOffsets.h"

#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../SharedModule/CommonShare/DefectDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

MeasureOffsets::MeasureOffsets()
	: m_shoulderLineOffsetTop_mm(0.)
	, m_shoulderLineOffsetBot_mm(0.)
	, m_cellLeftLength_mm(0.)
	, m_cellRightLength_mm(0.)//20220625 [jjh]
	, m_misalign_mm(0.)
	, m_mismatch_mm(0.)
	, m_tabHeight_mm(0.)
	, m_tabPitch_mm(0.)
	, m_tabSide_mm(0.)
	, m_tabWidth_mm(0.)
	, m_vhomeHeight_mm(0.)
	, m_vhomeHeightBot_mm(0.)
	, m_vhomeTiltTop_mm(0.)
	, m_vhomeTiltBot_mm(0.)
	, m_frontCuttingHeightTop_mm(0.)
	, m_frontCuttingHeightBot_mm(0.)
	, m_backCuttingHeightTop_mm(0.)
	, m_backCuttingHeightBot_mm(0.)
	, m_frontoverlay_mm(0.)
	, m_backoverlay_mm(0.)
	, m_frontinsulation_mm(0.)
	, m_backinsulation_mm(0.)
	, m_frontbotoverlay_mm(0.)
	, m_backbotoverlay_mm(0.)
{
}

MeasureOffsets::~MeasureOffsets()
{
}

double MeasureOffsets::GetValue_mm(bool isUp, MeasureOffsetType type)
{
	return Get(isUp, type);
}

double MeasureOffsets::GetValue_mm(DefectType defectType)
{
	if (auto* variable = Get(defectType))
	{
		return *variable;
	}

	return 0.;
}

double& MeasureOffsets::Get(bool isUp, MeasureOffsetType type)
{
	switch (type)
	{
	case MeasureOffsetType::CellLeftLength:		return m_cellLeftLength_mm;
	case MeasureOffsetType::CellRightLength:	return m_cellRightLength_mm; //20220625 [jjh]
	case MeasureOffsetType::CellLength2:		return isUp ? m_cellTopLength2_mm : m_cellBottomLength2_mm;
	case MeasureOffsetType::CuttingHeightTop:	return isUp ? m_frontCuttingHeightTop_mm : m_backCuttingHeightTop_mm;
	case MeasureOffsetType::CuttingHeightBot:	return isUp ? m_frontCuttingHeightBot_mm : m_backCuttingHeightBot_mm;
	case MeasureOffsetType::Misalign:			return m_misalign_mm;
	case MeasureOffsetType::Mismatch:			return m_mismatch_mm;
	case MeasureOffsetType::ShoulderLine:		return isUp ? m_shoulderLineOffsetTop_mm : m_shoulderLineOffsetBot_mm;
	case MeasureOffsetType::TabHeight:			return m_tabHeight_mm;
	case MeasureOffsetType::TabHeight2:			return isUp ? m_topTabHeight2_mm : m_bottomTabHeight2_mm;
	case MeasureOffsetType::TabPitch:			return m_tabPitch_mm;
	case MeasureOffsetType::TabSide:			return m_tabSide_mm;
	case MeasureOffsetType::TabWidth:			return m_tabWidth_mm;
	case MeasureOffsetType::VHomeTiltTop:		return m_vhomeTiltTop_mm;
	case MeasureOffsetType::VHomeTiltBot:		return m_vhomeTiltBot_mm;
	case MeasureOffsetType::Overlay:		return isUp ? m_frontoverlay_mm : m_backoverlay_mm;
	case MeasureOffsetType::Insulation:		return isUp ? m_frontinsulation_mm : m_backinsulation_mm;
	case MeasureOffsetType::BotOverlay:		return isUp ? m_frontbotoverlay_mm : m_backbotoverlay_mm;
	}

	static double errorValue = 0.;
	return errorValue;
}

double* MeasureOffsets::Get(DefectType defectType)
{
	switch (defectType)
	{
	case DefectType::BottomBackCuttingHeight:	return &m_backCuttingHeightBot_mm;
	case DefectType::BottomCoating:				return &m_shoulderLineOffsetBot_mm;
	case DefectType::BottomFrontCuttingHeight:	return &m_frontCuttingHeightBot_mm;
	case DefectType::Misalign:					return &m_misalign_mm;
	case DefectType::Mismatch:					return &m_mismatch_mm;
	case DefectType::TabPitch:					return &m_tabPitch_mm;
	case DefectType::TabWidth:					return &m_tabWidth_mm;
	case DefectType::TabHeight:					return &m_tabHeight_mm;
	case DefectType::Top_TabHeight_2:			return &m_topTabHeight2_mm;
	case DefectType::Bottom_TabHeight_2:		return &m_bottomTabHeight2_mm;
	case DefectType::TabSide:					return &m_tabSide_mm;
	case DefectType::TopBackCuttingHeight:		return &m_backCuttingHeightTop_mm;
	case DefectType::TopCoating:				return &m_shoulderLineOffsetTop_mm;
	case DefectType::TopFrontCuttingHeight:		return &m_frontCuttingHeightTop_mm;
	case DefectType::TopVHomeHeight:			return &m_vhomeHeight_mm;
	case DefectType::BottomVHomeHeight:			return &m_vhomeHeightBot_mm;
	case DefectType::LeftLength:				return &m_cellLeftLength_mm;	//Offset이 Plus Minus 두개 있으나 하나만 써도 될듯
	case DefectType::RightLength:				return &m_cellRightLength_mm; //20220625 [jjh]
	case DefectType::Top_Length_2:				return &m_cellTopLength2_mm;
	case DefectType::Bottom_Length_2:			return &m_cellBottomLength2_mm;
	case DefectType::TopTilt:					return &m_vhomeTiltTop_mm;
	case DefectType::BottomTilt:				return &m_vhomeTiltBot_mm;
	case DefectType::TopFrontOverlayWidth:			return &m_frontoverlay_mm;
	case DefectType::TopBackOverlayWidth:			return &m_backoverlay_mm;
	case DefectType::TopInsulationWidth:			return &m_frontinsulation_mm;
	case DefectType::BottomInsulationWidth:			return &m_backinsulation_mm;
	case DefectType::BottomFrontOverlayWidth:		return &m_frontbotoverlay_mm;
	case DefectType::BottomBackOverlayWidth:		return &m_backbotoverlay_mm;
	}

	return nullptr;
}

void MeasureOffsets::Reset()
{
	m_shoulderLineOffsetTop_mm = 0.;
	m_shoulderLineOffsetBot_mm = 0.;

	m_cellLeftLength_mm = 0;
	m_cellRightLength_mm = 0;//20220625 [jjh]
	m_cellTopLength2_mm = 0.;
	m_cellBottomLength2_mm = 0.;
	m_misalign_mm = 0.;
	m_mismatch_mm = 0.;
	m_tabHeight_mm = 0.;
	m_topTabHeight2_mm = 0.;
	m_bottomTabHeight2_mm = 0.;
	m_tabPitch_mm = 0.;
	m_tabSide_mm = 0.;
	m_tabWidth_mm = 0.;
	m_vhomeHeight_mm = 0.;
	m_vhomeHeightBot_mm = 0.;
	m_vhomeTiltTop_mm = 0.;
	m_vhomeTiltBot_mm = 0.;

	m_frontCuttingHeightTop_mm = 0.;
	m_frontCuttingHeightBot_mm = 0.;
	m_backCuttingHeightTop_mm = 0.;
	m_backCuttingHeightBot_mm = 0.;

	m_frontoverlay_mm = 0.;
	m_backoverlay_mm   = 0.;
	m_frontinsulation_mm = 0.;
	m_backinsulation_mm= 0.;
	m_frontbotoverlay_mm = 0.;
	m_backbotoverlay_mm= 0.;
}

void MeasureOffsets::LinkDataBase(bool bSave, Database &db)
{
	if (!bSave) Reset();

	db[_T("Shoulder Line Offset Top")].Link(bSave, m_shoulderLineOffsetTop_mm);
	db[_T("Shoulder Line Offset Bot")].Link(bSave, m_shoulderLineOffsetBot_mm);

	if (!db[_T("Cell Left Length")].Link(bSave, m_cellLeftLength_mm))
		db[_T("Cell Length")].Link(bSave, m_cellLeftLength_mm);
	if(!db[_T("Cell Right Length")].Link(bSave, m_cellRightLength_mm))//20220625 [jjh]
		db[_T("Cell Length")].Link(bSave, m_cellRightLength_mm);

	db[_T("Cell Top Length 2")].Link(bSave, m_cellTopLength2_mm);
	db[_T("Cell Bottom Length 2")].Link(bSave, m_cellBottomLength2_mm);

	db[_T("Misalign")].Link(bSave, m_misalign_mm);
	db[_T("Mismatch")].Link(bSave, m_mismatch_mm);
	db[_T("Tab Height")].Link(bSave, m_tabHeight_mm);
	db[_T("Top Tab Height 2")].Link(bSave, m_topTabHeight2_mm);
	db[_T("Bottom Tab Height 2")].Link(bSave, m_bottomTabHeight2_mm);
	db[_T("Tab Pitch")].Link(bSave, m_tabPitch_mm);
	db[_T("Tab Side")].Link(bSave, m_tabSide_mm);
	db[_T("Tab Width")].Link(bSave, m_tabWidth_mm);
	db[_T("V Home Height")].Link(bSave, m_vhomeHeight_mm);
	db[_T("Bot V Home Height")].Link(bSave, m_vhomeHeightBot_mm);
	db[_T("V Home Tilt Top")].Link(bSave, m_vhomeTiltTop_mm);
	db[_T("V Home Tilt Bot")].Link(bSave, m_vhomeTiltBot_mm);

	db[_T("Front Cutting Height Top")].Link(bSave, m_frontCuttingHeightTop_mm);
	db[_T("Front Cutting Height Bot")].Link(bSave, m_frontCuttingHeightBot_mm);
	db[_T("Back Cutting Height Top")].Link(bSave, m_backCuttingHeightTop_mm);
	db[_T("Back Cutting Height Bot")].Link(bSave, m_backCuttingHeightBot_mm);

	db[_T("Front Overlay")].Link(bSave, m_frontoverlay_mm);
	db[_T("Back Overlay")].Link(bSave, m_backoverlay_mm);
	db[_T("Front Insulation")].Link(bSave, m_frontinsulation_mm);
	db[_T("Back Insulation")].Link(bSave, m_backinsulation_mm);
	db[_T("Front Overlay Bot")].Link(bSave, m_frontbotoverlay_mm);
	db[_T("Back Overlay Bot")].Link(bSave, m_backbotoverlay_mm);

}

void MeasureOffsets::SetValue_mm(DefectType defectType, double value)
{
	if (auto* variable = Get(defectType))
	{
		*variable = value;
	}
}
