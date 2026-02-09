#include "stdafx.h"
#include "OptionCriteriaList.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../SharedModule/CommonShare/DefectDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

OptionCriteriaList::OptionCriteriaList()
{
}

OptionCriteriaList::~OptionCriteriaList()
{
}

ObjectCriteria* OptionCriteriaList::Find(DefectType defectType)
{
	switch (defectType)
	{
	case DefectType::TabWidth:					return &m_tabWidth;
	case DefectType::TabHeight:					return &m_tabHeight;
	case DefectType::Top_TabHeight_2:			return &m_topTabHeight2;
	case DefectType::Bottom_TabHeight_2:		return &m_bottomTabHeight2;
	case DefectType::LeftLength:				return &m_cellLeftLength;
	case DefectType::RightLength:				return &m_cellRightLength;
	case DefectType::Top_Length_2:				return &m_cellTopLength2;
	case DefectType::Bottom_Length_2:			return &m_cellBottomLength2;
	case DefectType::TabPitch:					return &m_cellPitch;
	case DefectType::TopTilt:					return &m_VHomeTiltTop;
	case DefectType::BottomTilt:				return &m_VHomeTiltBot;
	case DefectType::Mismatch:					return &m_mismatch;
	case DefectType::TopCoating:				return &m_shoulderLineTop;
	case DefectType::BottomCoating:				return &m_shoulderLineBot;
	case DefectType::TabShape:					return &m_tabShape;
	case DefectType::TabSide:					return &m_tabSide;
	case DefectType::Misalign:					return &m_mislalign;
	case DefectType::TopFrontCuttingHeight:		return &m_frontCuttingHeightTop;
	case DefectType::BottomFrontCuttingHeight:	return &m_frontCuttingHeightBot;
	case DefectType::TopBackCuttingHeight:		return &m_backCuttingHeightTop;
	case DefectType::BottomBackCuttingHeight:	return &m_backCuttingHeightBot;
	case DefectType::TopVHomeHeight:			return &m_topVHomeHeight;
	case DefectType::BottomVHomeHeight:			return &m_bottomVHomeHeight;
	case DefectType::TopFrontOverlayWidth:			return &m_frontoverlay;
	case DefectType::TopBackOverlayWidth:			return &m_backoverlay;
	case DefectType::TopInsulationWidth:			return &m_frontinsulation;
	case DefectType::BottomInsulationWidth:			return &m_backinsulation;
	case DefectType::BottomFrontOverlayWidth:		return &m_frontbotoverlay;
	case DefectType::BottomBackOverlayWidth:		return &m_backbotoverlay;
	}

	return nullptr;
}

void OptionCriteriaList::OldLinkDataBase(bool bSave, bool isUpCamera, Database& db)
{
	//----------------------------------------------------------------------------------------------------------------
	// *주의* 구버전 Recipe 호환용 코드 부분이다.
	// 해당 Version으로 저장은 하지 않는다.
	// 없던 항목이 추가되었을 때는 로드를 위해 Init함수만 호출해 주는 것을 추가하자
	// (예) m_optionXX.Init();
	//----------------------------------------------------------------------------------------------------------------

	long version = 1;
	if (!db[_T("Version")].Link(bSave, version)) version = 0;

	m_topVHomeHeight.Init();
	m_bottomVHomeHeight.Init();

	switch (version)
	{
	case 0:
		ASSERT(!bSave);

		if (isUpCamera)
		{
			m_tabWidth.m_ai.LinkOld(bSave, db, _T("AI Tab Width Tolerance -"), _T("AI Tab Width Tolerance +"));
			m_tabHeight.m_ai.LinkOld(bSave, db, _T("AI Tab Height Tolerance -"), _T("AI Tab Height Tolerance +"));
			m_cellLeftLength.m_ai.LinkOld(bSave, db, _T("AI Cell Length Tolerance -"), _T("AI Cell Length Tolerance +"));
			m_cellRightLength.m_ai.LinkOld(bSave, db, _T("AI Cell Length Tolerance -"), _T("AI Cell Length Tolerance +"));	// Right는 없었다
			m_cellPitch.m_ai.LinkOld(bSave, db, _T("AI Cell Pitch Tolerance -"), _T("AI Cell Pitch Tolerance +"));
			m_VHomeTiltTop.m_ai.LinkOld(bSave, db, _T("AI V Home Tilt Top Min"), _T("AI V Home Tilt Top Max"));
			m_VHomeTiltBot.m_ai.LinkOld(bSave, db, _T("AI V Home Tilt Bot Min"), _T("AI V Home Tilt Bot Max"));
			m_mismatch.m_ai.LinkOld(bSave, db, _T("AI Mismatch Min"), _T("AI Mismatch Max"));
			m_shoulderLineTop.m_ai.LinkOld(bSave, db, _T("AI ShoulderLine Top Tolerance -"), _T("AI ShoulderLine Top Tolerance +"));
			m_tabShape.m_ai.LinkOld(bSave, db, _T("AI Tab Shape Min"), _T("AI Tab Shape Max"));
			m_tabSide.m_ai.LinkOld(bSave, db, _T("AI Tab Side Tolerance -"), _T("AI Tab Side Tolerance +"));
			m_mislalign.m_ai.LinkOld(bSave, db, _T("AI Misalign Min"), _T("AI Misalign Max"));
			m_frontCuttingHeightTop.m_ai.LinkOld(bSave, db, _T("AI Cutting Height Top Minus Tolerance"), _T("AI Cutting Height Top Plus Tolerance"));
			m_frontCuttingHeightBot.m_ai.LinkOld(bSave, db, _T("AI Cutting Height Bottom Minus Tolerance"), _T("AI Cutting Height Bottom Plus Tolerance"));
			m_frontoverlay.m_ai.LinkOld(bSave, db, _T("AI Front Overlay Minus Tolerance"), _T("AI Front Overlay Plus Tolerance"));
			m_frontinsulation.m_ai.LinkOld(bSave, db, _T("AI Front Insulation Minus Tolerance"), _T("AI Front Insulation Plus Tolerance"));
			m_frontbotoverlay.m_ai.LinkOld(bSave, db, _T("AI Front Overlay Bot Minus Tolerance"), _T("AI Front Overlay Bot Plus Tolerance"));

			m_tabWidth.m_marking.LinkOld(bSave, db, _T("Marking Tab Width Tolerance -"), _T("Marking Tab Width Tolerance +"));
			m_tabHeight.m_marking.LinkOld(bSave, db, _T("Marking Tab Height Tolerance -"), _T("Marking Tab Height Tolerance +"));
			m_cellLeftLength.m_marking.LinkOld(bSave, db, _T("Marking Cell Left Length Tolerance -"), _T("Marking Cell Left Length Tolerance +"));
			m_cellRightLength.m_marking.LinkOld(bSave, db, _T("Marking Cell Right Length Tolerance -"), _T("Marking Cell Right Length Tolerance +"));
			m_cellPitch.m_marking.LinkOld(bSave, db, _T("Marking Cell Pitch Tolerance -"), _T("Marking Cell Pitch Tolerance +"));
			m_VHomeTiltTop.m_marking.LinkOld(bSave, db, _T("Marking V Home Tilt Top Min"), _T("Marking V Home Tilt Top Max"));
			m_VHomeTiltBot.m_marking.LinkOld(bSave, db, _T("Marking V Home Tilt Bot Min"), _T("Marking V Home Tilt Bot Max"));
			m_mismatch.m_marking.LinkOld(bSave, db, _T("Marking Mismatch Min"), _T("Marking Mismatch Max"));
			m_shoulderLineTop.m_marking.LinkOld(bSave, db, _T("Marking ShoulderLine Top Tolerance -"), _T("Marking ShoulderLine Top Tolerance +"));
			m_tabShape.m_marking.LinkOld(bSave, db, _T("Marking Tab Shape Min"), _T("Marking Tab Shape Max"));
			m_tabSide.m_marking.LinkOld(bSave, db, _T("Marking Tab Side Tolerance -"), _T("Marking Tab Side Tolerance +"));
			m_mislalign.m_marking.LinkOld(bSave, db, _T("Marking Misalign Min"), _T("Marking Misalign Max"));
			m_frontCuttingHeightTop.m_marking.LinkOld(bSave, db, _T("Marking Cutting Height Top Minus Tolerance"), _T("Marking Cutting Height Top Plus Tolerance"));
			m_frontCuttingHeightBot.m_marking.LinkOld(bSave, db, _T("Marking Cutting Height Bottom Minus Tolerance"), _T("Marking Cutting Height Bottom Plus Tolerance"));
			m_frontoverlay.m_marking.LinkOld(bSave, db, _T("Marking Front Overlay Minus Tolerance"), _T("Marking Front Overlay Plus Tolerance"));
			m_frontinsulation.m_marking.LinkOld(bSave, db, _T("Marking Front Insulation Minus Tolerance"), _T("Marking Front Insulation Plus Tolerance"));
			m_frontbotoverlay.m_marking.LinkOld(bSave, db, _T("Marking Front Overlay Bot Minus Tolerance"), _T("Marking Front Overlay Bot Plus Tolerance"));
		}
		else
		{
			m_shoulderLineBot.m_ai.LinkOld(bSave, db, _T("AI ShoulderLine Bot Tolerance -"), _T("AI ShoulderLine Bot Tolerance +"));
			m_backCuttingHeightTop.m_ai.LinkOld(bSave, db, _T("AI Cutting Height Top Minus Tolerance"), _T("AI Cutting Height Top Plus Tolerance"));
			m_backCuttingHeightBot.m_ai.LinkOld(bSave, db, _T("AI Cutting Height Bottom Minus Tolerance"), _T("AI Cutting Height Bottom Plus Tolerance"));
			m_backoverlay.m_ai.LinkOld(bSave, db, _T("AI Back Overlay Minus Tolerance"), _T("AI Back Overlay Plus Tolerance"));
			m_backinsulation.m_ai.LinkOld(bSave, db, _T("AI Back Insulation Minus Tolerance"), _T("AI Back Insulation Plus Tolerance"));
			m_backbotoverlay.m_ai.LinkOld(bSave, db, _T("AI Back Overlay Bot Minus Tolerance"), _T("AI Back Overlay Bot Plus Tolerance"));

			m_shoulderLineBot.m_marking.LinkOld(bSave, db, _T("Marking ShoulderLine Bot Tolerance -"), _T("Marking ShoulderLine Bot Tolerance +"));
			m_backCuttingHeightTop.m_marking.LinkOld(bSave, db, _T("Marking Cutting Height Top Minus Tolerance"), _T("Marking Cutting Height Top Plus Tolerance"));
			m_backCuttingHeightBot.m_marking.LinkOld(bSave, db, _T("Marking Cutting Height Bottom Minus Tolerance"), _T("Marking Cutting Height Bottom Plus Tolerance"));
			m_backoverlay.m_marking.LinkOld(bSave, db, _T("Marking Back Overlay Minus Tolerance"), _T("Marking Back Overlay Plus Tolerance"));
			m_backinsulation.m_marking.LinkOld(bSave, db, _T("Marking Back Insulation Minus Tolerance"), _T("Marking Back Insulation Plus Tolerance"));
			m_backbotoverlay.m_marking.LinkOld(bSave, db, _T("Marking Back Overlay Bot Minus Tolerance"), _T("Marking Back Overlay Bot Plus Tolerance"));
		}

		break;

	default:
		if (isUpCamera)
		{
			m_tabWidth.Link(bSave, db[_T("Option Tab Width")]);
			m_tabHeight.Link(bSave, db[_T("Option Tab Height")]);
			m_cellLeftLength.Link(bSave, db[_T("Option Cell Left Length")]);
			m_cellRightLength.Link(bSave, db[_T("Option Cell Right Length")]);
			m_cellPitch.Link(bSave, db[_T("Option Cell Pitch")]);
			m_VHomeTiltTop.Link(bSave, db[_T("Option V Home Tilt Top")]);
			m_VHomeTiltBot.Link(bSave, db[_T("Option V Home Tilt Bot")]);
			m_mismatch.Link(bSave, db[_T("Option Mismatch")]);
			m_shoulderLineTop.Link(bSave, db[_T("Option ShoulderLine Top")]);
			m_tabShape.Link(bSave, db[_T("Option Tab Shape")]);
			m_tabSide.Link(bSave, db[_T("Option Tab Side")]);
			m_mislalign.Link(bSave, db[_T("Option Misalign Min")]);
			m_frontCuttingHeightTop.Link(bSave, db[_T("Option Cutting Height Top")]);
			m_frontCuttingHeightBot.Link(bSave, db[_T("Option Cutting Height Bot")]);
			m_frontoverlay.Link(bSave, db[_T("Option Front Overlay")]);
			m_frontinsulation.Link(bSave, db[_T("Option Front Insulation")]);
			m_frontbotoverlay.Link(bSave, db[_T("Option Front Overlay Bot")]);
		}
		else
		{
			m_shoulderLineBot.Link(bSave, db[_T("Option ShoulderLine Bot")]);
			m_backCuttingHeightTop.Link(bSave, db[_T("Option Cutting Height Top")]);
			m_backCuttingHeightBot.Link(bSave, db[_T("Option Cutting Height Bot")]);
			m_backoverlay.Link(bSave, db[_T("Option Back Overlay")]);
			m_backinsulation.Link(bSave, db[_T("Option Back Insulation")]);
			m_backbotoverlay.Link(bSave, db[_T("Option Back Overlay Bot")]);
		}
		break;
	}
}

void OptionCriteriaList::LinkDataBase(bool bSave, Database& db)
{
	long version = 20220113;
	if (!db[_T("Version")].Link(bSave, version)) version = 0;

	m_tabWidth.Link(bSave, db[_T("Option Tab Width")]);
	m_tabHeight.Link(bSave, db[_T("Option Tab Height")]);
	m_topTabHeight2.Link(bSave, db[_T("Option Top Tab Height 2")]);
	m_bottomTabHeight2.Link(bSave, db[_T("Option Bottom Tab Height 2")]);
	m_cellLeftLength.Link(bSave, db[_T("Option Cell Left Length")]);
	m_cellRightLength.Link(bSave, db[_T("Option Cell Right Length")]);
	m_cellTopLength2.Link(bSave, db[_T("Option Cell Top Length 2")]);
	m_cellBottomLength2.Link(bSave, db[_T("Option Cell Bottom Length 2")]);
	m_cellPitch.Link(bSave, db[_T("Option Cell Pitch")]);
	m_VHomeTiltTop.Link(bSave, db[_T("Option V Home Tilt Top")]);
	m_VHomeTiltBot.Link(bSave, db[_T("Option V Home Tilt Bot")]);
	m_mismatch.Link(bSave, db[_T("Option Mismatch")]);
	m_shoulderLineTop.Link(bSave, db[_T("Option ShoulderLine Top")]);
	m_shoulderLineBot.Link(bSave, db[_T("Option ShoulderLine Bot")]);
	m_tabShape.Link(bSave, db[_T("Option Tab Shape")]);
	m_tabSide.Link(bSave, db[_T("Option Tab Side")]);
	m_mislalign.Link(bSave, db[_T("Option Misalign Min")]);
	m_frontCuttingHeightTop.Link(bSave, db[_T("Option Front Cutting Height Top")]);
	m_frontCuttingHeightBot.Link(bSave, db[_T("Option Front Cutting Height Bot")]);
	m_backCuttingHeightTop.Link(bSave, db[_T("Option Back Cutting Height Top")]);
	m_backCuttingHeightBot.Link(bSave, db[_T("Option Back Cutting Height Bot")]);
	m_topVHomeHeight.Link(bSave, db[_T("Option Top VHome Height")]);
	m_bottomVHomeHeight.Link(bSave, db[_T("Option Bottom VHome Height")]);
	m_frontoverlay.Link(bSave, db[_T("Option Front Overlay")]);
	m_backoverlay.Link(bSave, db[_T("Option Back Overlay")]);
	m_frontinsulation.Link(bSave, db[_T("Option Front Insulation")]);
	m_backinsulation.Link(bSave, db[_T("Option Back Insulation")]);
	m_frontbotoverlay.Link(bSave, db[_T("Option Front Overlay Bot")]);
	m_backbotoverlay.Link(bSave, db[_T("Option Back Overlay Bot")]);
}
