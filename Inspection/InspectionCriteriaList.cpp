#include "stdafx.h"
#include "InspectionCriteriaList.h"

#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../SharedModule/CommonShare/DefectDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

InspectionCriteriaList::InspectionCriteriaList()
	: m_tabPitch(false)
	, m_tabWidth(false)
	, m_tabHeight(false)
	, m_topTabHeight2(true)
	, m_bottomTabHeight2(true)
	, m_tabSide(false)
	, m_mismatch(true)
	, m_cellLength(false)
	, m_cellTopLength2(true)
	, m_cellBottomLength2(true)
	, m_shoulderLineTop(true)
	, m_shoulderLineBot(true)
	, m_vhomeHeight(true)
	, m_vhomeTiltTop(true)
	, m_vhomeTiltBot(true)
	, m_misalign(true)
	, m_frontCuttingHeightTop(true)
	, m_frontCuttingHeightBot(true)
	, m_backCuttingHeightTop(true)
	, m_backCuttingHeightBot(true)
	, m_frontOverlay(true)
	, m_backOverlay(true)
	, m_frontInsulation(true)
	, m_backInsulation(true)
	, m_frontBotOverlay(true)
	, m_backBotOverlay(true)
	//, m_frontBotInsulation(true)
	//, m_backBotInsulation(true)
{
}


InspectionCriteriaList::~InspectionCriteriaList()
{
}

void InspectionCriteriaList::LinkDataBase(bool bSave, Database &db)
{
	m_tabPitch.LinkDataBase(bSave, db[_T("Tab Pitch")]);
	m_tabWidth.LinkDataBase(bSave, db[_T("Tab Width")]);
	m_tabHeight.LinkDataBase(bSave, db[_T("Tab Height")]);
	m_topTabHeight2.LinkDataBase(bSave, db[_T("Top Tab Height 2")]);
	m_bottomTabHeight2.LinkDataBase(bSave, db[_T("Bottom Tab Height 2")]);
	m_tabSide.LinkDataBase(bSave, db[_T("Tab Side")]);
	m_mismatch.LinkDataBase(bSave, db[_T("Mismatch")]);
	m_cellLength.LinkDataBase(bSave, db[_T("Cell Length")]);
	m_cellTopLength2.LinkDataBase(bSave, db[_T("Cell Top Length 2")]);
	m_cellBottomLength2.LinkDataBase(bSave, db[_T("Cell Bottom Length 2")]);
	m_shoulderLineTop.LinkDataBase(bSave, db[_T("Shoulder Line Top")]);
	m_shoulderLineBot.LinkDataBase(bSave, db[_T("Shoulder Line Bot")]);
	m_vhomeHeight.LinkDataBase(bSave, db[_T("V Home Height")]);
	m_vhomeTiltTop.LinkDataBase(bSave, db[_T("V Home Tilt Top")]);
	m_vhomeTiltBot.LinkDataBase(bSave, db[_T("V Home Tilt Bot")]);
	m_misalign.LinkDataBase(bSave, db[_T("Misalign")]);
	m_frontCuttingHeightTop.LinkDataBase(bSave, db[_T("Front Cutting Height Top")]);
	m_frontCuttingHeightBot.LinkDataBase(bSave, db[_T("Front Cutting Height Bot")]);
	m_backCuttingHeightTop.LinkDataBase(bSave, db[_T("Back Cutting Height Top")]);
	m_backCuttingHeightBot.LinkDataBase(bSave, db[_T("Back Cutting Height Bot")]);

	m_frontOverlay.LinkDataBase(bSave, db[_T("Front Overlay")]);
	m_backOverlay.LinkDataBase(bSave, db[_T("Back Overlay")]);
	m_frontInsulation.LinkDataBase(bSave, db[_T("Front Insulation")]);
	m_backInsulation.LinkDataBase(bSave, db[_T("Back Insulation")]);
	m_frontBotOverlay.LinkDataBase(bSave, db[_T("Front Overlay Bot")]);
	m_backBotOverlay.LinkDataBase(bSave, db[_T("Back Overlay Bot")]);
	//m_frontBotInsulation.LinkDataBase(bSave, db[_T("Front Insulation Bot")]);
	//m_backBotInsulation.LinkDataBase(bSave, db[_T("Back Insulation Bot")]);

}

InspectionCriteria* InspectionCriteriaList::Find(DefectType defectType)
{
	switch (defectType)
	{
	case DefectType::TabPitch:					return &m_tabPitch;
	case DefectType::TabWidth:					return &m_tabWidth;
	case DefectType::TabHeight:					return &m_tabHeight;
	case DefectType::Top_TabHeight_2:			return &m_topTabHeight2;
	case DefectType::Bottom_TabHeight_2:		return &m_bottomTabHeight2;
	case DefectType::TabSide:					return &m_tabSide;
	case DefectType::Mismatch:					return &m_mismatch;
	case DefectType::LeftLength:				return &m_cellLength;
	case DefectType::RightLength:				return &m_cellLength;
	case DefectType::Top_Length_2:				return &m_cellTopLength2;
	case DefectType::Bottom_Length_2:			return &m_cellBottomLength2;
	case DefectType::TopCoating:				return &m_shoulderLineTop;
	case DefectType::BottomCoating:				return &m_shoulderLineBot;
	case DefectType::TopVHomeHeight:			return &m_vhomeHeight;
	case DefectType::BottomVHomeHeight:			return &m_vhomeHeight;
	case DefectType::TopTilt:					return &m_vhomeTiltTop;
	case DefectType::BottomTilt:				return &m_vhomeTiltBot;
	case DefectType::Misalign:					return &m_misalign;
	case DefectType::TopFrontCuttingHeight:		return &m_frontCuttingHeightTop;
	case DefectType::BottomFrontCuttingHeight:	return &m_frontCuttingHeightBot;
	case DefectType::TopBackCuttingHeight:		return &m_backCuttingHeightTop;
	case DefectType::BottomBackCuttingHeight:	return &m_backCuttingHeightBot;
	case DefectType::TopFrontOverlayWidth:			return &m_frontOverlay;
	case DefectType::TopBackOverlayWidth:			return &m_backOverlay;
	case DefectType::TopInsulationWidth:			return &m_frontInsulation;
	case DefectType::BottomInsulationWidth:			return &m_backInsulation;
	case DefectType::BottomFrontOverlayWidth:		return &m_frontBotOverlay;
	case DefectType::BottomBackOverlayWidth:		return &m_backBotOverlay;
	}

	return nullptr;
}
