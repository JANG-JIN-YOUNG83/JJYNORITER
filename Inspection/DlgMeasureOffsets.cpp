// DlgMeasureOffsets.cpp : implementation file
//

#include "stdafx.h"
#include "Inspection.h"
#include "DlgMeasureOffsets.h"
#include "afxdialogex.h"

#include "../Module/CustomControl/PropertyGridHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DlgMeasureOffsets dialog

IMPLEMENT_DYNAMIC(DlgMeasureOffsets, CDialogEx)

DlgMeasureOffsets::DlgMeasureOffsets(const Inspection::MeasureOffsets& measureOffset, CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgMeasureOffsets::IDD, pParent)
	, m_measureOffset(measureOffset)
{

}

DlgMeasureOffsets::~DlgMeasureOffsets()
{
}

void DlgMeasureOffsets::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgMeasureOffsets, CDialogEx)
END_MESSAGE_MAP()


// DlgMeasureOffsets message handlers


BOOL DlgMeasureOffsets::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rtProperty;
	GetDlgItem(IDC_PROPERTY)->GetWindowRect(rtProperty);
	ScreenToClient(rtProperty);

	m_property.Create(rtProperty, this, WS_CHILD | WS_VISIBLE);
	m_property.SetViewDivider(0.6);
	m_property.HighlightChangedItems(TRUE);
	m_property.SetBorderStyle(XTPPropertyGridBorderStyle::xtpGridBorderFlat);
	m_property.SetShowInplaceButtonsAlways(TRUE);

	if (auto* category = m_property.AddCategory(_T("Offset")))
	{
		PropertyGridHelper::AddLink(category, _T("Shoulder Line Top Offset [mm]"), _T("%.5f"), m_measureOffset.m_shoulderLineOffsetTop_mm);
		PropertyGridHelper::AddLink(category, _T("Shoulder Line Bot Offset [mm]"), _T("%.5f"), m_measureOffset.m_shoulderLineOffsetBot_mm);

		PropertyGridHelper::AddLink(category, _T("Cell Pitch Offset [mm]"), _T("%.5f"), m_measureOffset.m_tabPitch_mm);
		PropertyGridHelper::AddLink(category, _T("Cell Left Length Offset [mm]"), _T("%.5f"), m_measureOffset.m_cellLeftLength_mm);
		PropertyGridHelper::AddLink(category, _T("Cell Right Length Offset [mm]"), _T("%.5f"), m_measureOffset.m_cellRightLength_mm);//20220625 [jjh]
		PropertyGridHelper::AddLink(category, _T("Cell Top Length 2 Offset [mm]"), _T("%.5f"), m_measureOffset.m_cellTopLength2_mm);
		PropertyGridHelper::AddLink(category, _T("Cell Bottom Length 2 Offset [mm]"), _T("%.5f"), m_measureOffset.m_cellBottomLength2_mm);
		PropertyGridHelper::AddLink(category, _T("Misalign Offset [mm]"), _T("%.5f"), m_measureOffset.m_misalign_mm);
		PropertyGridHelper::AddLink(category, _T("Mismatch Offset [mm]"), _T("%.5f"), m_measureOffset.m_mismatch_mm);
		PropertyGridHelper::AddLink(category, _T("Tab Side Offset [mm]"), _T("%.5f"), m_measureOffset.m_tabSide_mm);
		PropertyGridHelper::AddLink(category, _T("Tab Width Offset [mm]"), _T("%.5f"), m_measureOffset.m_tabWidth_mm);
		PropertyGridHelper::AddLink(category, _T("Tab Height Offset [mm]"), _T("%.5f"), m_measureOffset.m_tabHeight_mm);
		PropertyGridHelper::AddLink(category, _T("Top Tab Height 2 Offset [mm]"), _T("%.5f"), m_measureOffset.m_topTabHeight2_mm);
		PropertyGridHelper::AddLink(category, _T("Bottom Tab Height 2 Offset [mm]"), _T("%.5f"), m_measureOffset.m_bottomTabHeight2_mm);

		PropertyGridHelper::AddLink(category, _T("V Home Height Offset [um]"), _T("%.5f"), m_measureOffset.m_vhomeHeight_mm);
		PropertyGridHelper::AddLink(category, _T("Bot V Home Height Offset [um]"), _T("%.5f"), m_measureOffset.m_vhomeHeightBot_mm);
		PropertyGridHelper::AddLink(category, _T("V Home Tilt Top Offset [mm]"), _T("%.5f"), m_measureOffset.m_vhomeTiltTop_mm);
		PropertyGridHelper::AddLink(category, _T("V Home Tilt Bot Offset [mm]"), _T("%.5f"), m_measureOffset.m_vhomeTiltBot_mm);

		PropertyGridHelper::AddLink(category, _T("Front Cutting Height Top Offset [mm]"), _T("%.5f"), m_measureOffset.m_frontCuttingHeightTop_mm);
		PropertyGridHelper::AddLink(category, _T("Front Cutting Height Bot Offset [mm]"), _T("%.5f"), m_measureOffset.m_frontCuttingHeightBot_mm);
		PropertyGridHelper::AddLink(category, _T("Back Cutting Height Top Offset [mm]"), _T("%.5f"), m_measureOffset.m_backCuttingHeightTop_mm);
		PropertyGridHelper::AddLink(category, _T("Back Cutting Height Bot Offset [mm]"), _T("%.5f"), m_measureOffset.m_backCuttingHeightBot_mm);

		PropertyGridHelper::AddLink(category, _T("Front Overlay Offset [mm]"), _T("%.5f"), m_measureOffset.m_frontoverlay_mm);
		PropertyGridHelper::AddLink(category, _T("Back Overlay Offset [mm]"), _T("%.5f"), m_measureOffset.m_backoverlay_mm);
		PropertyGridHelper::AddLink(category, _T("Front Insulation Offset [mm]"), _T("%.5f"), m_measureOffset.m_frontinsulation_mm);
		PropertyGridHelper::AddLink(category, _T("Back Insulation Offset [mm]"), _T("%.5f"), m_measureOffset.m_backinsulation_mm);
		PropertyGridHelper::AddLink(category, _T("Front Overlay Bot Offset [mm]"), _T("%.5f"), m_measureOffset.m_frontbotoverlay_mm);
		PropertyGridHelper::AddLink(category, _T("Back Overlay Bot Offset [mm]"), _T("%.5f"), m_measureOffset.m_backbotoverlay_mm);

		category->Expand();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
