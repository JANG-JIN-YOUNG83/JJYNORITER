// DlgInspectionParaGrid.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "InspectionRoot.h"
#include "DlgInspectionMain.h"
#include "afxdialogex.h"
#include "ImageViewEx.h"
#include "Accessor.h"
//#include "Surface_Display.h"
#include "DlgCriteriaList.h"
#include "DlgMeasureOffsets.h"
#include "DlgMergeOptions.h"
#include "ImageViewEx.h"
#include "DlgEdgeDetectPara.h"

//#include "../IBCI/SystemParameter.h"
#include "../Module/CustomControl/XTPPropertyGridItemButton.h"
#include "../Module/CustomControl/PropertyGridHelper.h"
#include "../VisionModule/Language/PropertyGridHelperEx.h"
#include "../Module/CustomControl/CustomButton.h"
//#include "InspectionUncoating.h"
#include "Algorithm/BlobDetection.h"
#include "Algorithm/Mathematics.h"
#include "Algorithm/ImageProcessing.h"
#include "Base/basedef.h"
#include "Types/Triangle_32f.h"
#include "Types/Polyline_32f.h"
#include "Algorithm/DataFitting.h"
#include "Types/LineEq_32f.h"

#include "InspectionConstantPara.h"

//class CSurface_Display;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgInspectionMain 대화 상자입니다.

void CDlgInspectionMain::UpdateInspectionParaGridCellDesign()
{
	auto* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

	m_imageView->ROIClear();

	m_inspectionParaGrid->ResetContent();

	if (auto* category = m_inspectionParaGrid->AddCategory(_T("Cell Design")))
	{
		if (auto* item = PropertyGridHelper::AddLink(category, _T("Length"), _T("%.2f"), inspection->m_cellDesign.fCellHeight))
		{
			//item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink(category, _T("Pitch"), _T("%.2f"), inspection->m_cellDesign.fCellWidth))
		{
			//item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink(category, _T("Tab Side"), _T("%.2f"), inspection->m_cellDesign.fTabOffset))
		{
			//item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink(category, _T("Tab Height"), _T("%.2f"), inspection->m_cellDesign.fTabHeight))
		{
			//item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink(category, _T("Tab Width"), _T("%.2f"), inspection->m_cellDesign.fTabWidth))
		{
			//item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink(category, _T("V Home Height"), _T("%.2f"), Inspection::Accessor::Get().m_criteriaList.m_vhomeHeight.m_standardValue_mm))
		{
			//item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink_Bool(category, _T("Show Overlay"), inspection->m_para.m_bCellDesignShowOverlay))
		{
			item->SetID(IDC_PARA_CELL_DESIGN_SHOW_OVERLAY);
		}

		if (auto* item = category->AddChildItem(new CXTPPropertyGridItemNumber(_T("X Axis Move"))))
		{
			item->SetID(IDC_PARA_CELL_DESIGN_OVERLAY_X_AXIS_MOVE);
			auto* xMoveSlider = item->AddSliderControl();
			xMoveSlider->SetMin(-m_inspectionImageBright.GetSizeX() / 2);
			xMoveSlider->SetMax(m_inspectionImageBright.GetSizeX() / 2);
		}


		if (auto* item2 = category->AddChildItem(new CXTPPropertyGridItemNumber(_T("Y Axis Move"))))
		{
			item2->SetID(IDC_PARA_CELL_DESIGN_OVERLAY_Y_AXIS_MOVE);
			auto* yMoveSlider = item2->AddSliderControl();
			yMoveSlider->SetMin(-m_inspectionImageBright.GetSizeY() / 2);
			yMoveSlider->SetMax(m_inspectionImageBright.GetSizeY() / 2);
		}

		if (auto* Subitem = PropertyGridHelperEx::AddLink_CheckBox(category, _T("direction"), bcheckbox))
		{
			Subitem->SetID(IDC_PARA_TEST_ROI);
			CXTPPropertyGridInplaceButton* pButton = Subitem->GetInplaceButtons()->AddButton(new CXTPPropertyGridInplaceButton(IDC_PARA_TEST_ROI));
		}
		if (inspection->m_para.m_bCellDesignShowOverlay == true)
		{
			OnBnClickedButtonCellDesignShowOverlay();
		}

		category->Expand();
	}
	if (auto* category = m_inspectionParaGrid->AddCategory(_T("EdgeDetect Test")))
	{
		m_FilterLength = 6, m_Gaus = 1.5f, m_FindFirst = FALSE, m_DetectType = 0, m_Direction = 0, m_EdgeTh = 1.0, m_ShowProfile = 0, m_nFindEdge = 5, m_nSearchWidth = 5, m_isUseEdgeDetects = FALSE;
		if (auto* item = PropertyGridHelper::AddLink_Bool(category, _T("USE"), m_isUseEdgeDetect))
		{
			item->SetID(IDC_PARA_TEST_EDGEDETECT);
		}
		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Show Type"), m_ShowProfile))
		{
			item->GetConstraints()->AddConstraint(_T("Edge Threshold"), 0);
			item->GetConstraints()->AddConstraint(_T("Filter Values"), 1);

		}
		PropertyGridHelper::AddLink(category, _T("HalfFilter Length"), m_FilterLength);
		PropertyGridHelper::AddLink(category, _T("Gaussian Deviation"), _T("%.3f"), m_Gaus);
		PropertyGridHelper::AddLink_Bool(category, _T("Find First Edge"), m_FindFirst);

		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Direction"), m_Direction))
		{
			item->GetConstraints()->AddConstraint(_T("To Left"), (long)IPVM::SearchDirection::ToLeft);
			item->GetConstraints()->AddConstraint(_T("To Right"), (long)IPVM::SearchDirection::ToRight);
			item->GetConstraints()->AddConstraint(_T("To Up"), (long)IPVM::SearchDirection::ToUp);
			item->GetConstraints()->AddConstraint(_T("To Down"), (long)IPVM::SearchDirection::ToDown);

		}
		if (auto* Subitem = PropertyGridHelper::AddLink_Enum(category, _T("Type"), m_DetectType))
		{
			Subitem->GetConstraints()->AddConstraint(_T("Both"), (int)IPVM::EdgeType::Both);
			Subitem->GetConstraints()->AddConstraint(_T("Falling"), (int)IPVM::EdgeType::Falling);
			Subitem->GetConstraints()->AddConstraint(_T("Rising"), (int)IPVM::EdgeType::Rising);
		}
		PropertyGridHelper::AddLink(category, _T("Edge Threshold"), _T("%.3f"), m_EdgeTh);
		if (auto* item = PropertyGridHelper::AddLink_Bool(category, _T("Multiple Detection"), m_isUseEdgeDetects))
		{
			PropertyGridHelper::AddLink(item, _T("Find Edge Count"), m_nFindEdge);
			PropertyGridHelper::AddLink(item, _T("Search Width"), m_nSearchWidth);
		}
		category->Expand();
	}

#ifdef _DRAW_DEFECT_
	if (auto* category = m_inspectionParaGrid->AddCategory(_T("DrawDefect")))
	{
		PropertyGridHelper::AddLink_Bool(category, _T("USE"), m_isUseDrawDefect);
		category->AddChildItem(new CXTPPropertyGridItemColor(_T("BrightImageColor"), m_BrightColor))->SetID(IDC_PARA_TEST_COLOR_B);
		category->AddChildItem(new CXTPPropertyGridItemColor(_T("DarkImageColor"), m_DarkColor))->SetID(IDC_PARA_TEST_COLOR_D);
		PropertyGridHelper::AddLink(category,_T("DrawSize"), m_Size);
		if (auto* Subitem = PropertyGridHelper::AddLink_Enum(category, _T("Type"), m_Type))
		{
#if INSPECTION_MACHINE_UC1
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::END), (int)DefectType::END);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::White_Pinhole), (int)DefectType::White_Pinhole);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Crack), (int)DefectType::Crack);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::ConnectionTape), (int)DefectType::ConnectionTape);
    Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Crater), (int)DefectType::Crater);
    Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::White_Line), (int)DefectType::White_Line);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::DarkConnectionTape), (int)DefectType::DarkConnectionTape);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Seam), (int)DefectType::Seam);
#else
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::END), (int)DefectType::END);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Pinhole), (int)DefectType::Pinhole);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Crack), (int)DefectType::Crack);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::ConnectionTape), (int)DefectType::ConnectionTape);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Stain_Pinhole), (int)DefectType::Stain_Pinhole);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Line), (int)DefectType::Line);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::DarkConnectionTape), (int)DefectType::DarkConnectionTape);
	Subitem->GetConstraints()->AddConstraint(GetDefectName(DefectType::Seam), (int)DefectType::Seam);
#endif // INSPECTION_MACHINE_UC1

			
		}
		category->Expand();
	}
#endif
}

void CDlgInspectionMain::OnBnClickedButtonRuluer()
{
	if (false == bcheckbox)
	{
		m_imageView->ROIClear();
		m_imageView->ROIHide();
	}
	else
	{
		m_imageView->ROIClear();

		long nSizeX = m_inspectionImageBright.GetSizeX();
		long nSizeY = m_inspectionImageBright.GetSizeY();

		IPVM::Rect rt1( 1000, 100, 1100, 100);
		IPVM::Rect rt2(nSizeX - 1000, 100, nSizeX - 900, 100);


		m_imageView->ROISet(_T("point_1"), _T("point_1"), rt1, RGB(0, 255, 0), 100);
		m_imageView->ROISet(_T("point_2"), _T("point_2"), rt2, RGB(0, 255, 0), 100);
		
		
		m_imageView->ROIShow();
	}

}
#ifdef _DRAW_DEFECT_
void CDlgInspectionMain::OnBnClickedColor(const LPARAM & lparam)
{
	CXTPPropertyGridItemColor* pItem = (CXTPPropertyGridItemColor*)lparam;
	CString str = pItem->RGBToString(pItem->GetColor());
	switch (pItem->GetID())
	{
	case IDC_PARA_TEST_COLOR_B:
		m_BrightColor = pItem->GetColor();
		break;
	case IDC_PARA_TEST_COLOR_D:
		m_DarkColor = pItem->GetColor();
		break;
	default:
		break;
	}
	
}
#endif
void CDlgInspectionMain::OnBnClickedUseEdgeDetect()
{
	if (FALSE == m_isUseEdgeDetect)
	{
		m_imageView->ROIClear();
		m_imageView->ROIHide();
	}
	else
	{
		m_imageView->ROIClear();

		long nSizeX = m_inspectionImageBright.GetSizeX();
		long nSizeY = m_inspectionImageBright.GetSizeY();

		m_curSelImage = &m_inspectionImageBright;

		IPVM::Rect rt1(1000, 100, 1600, 600);

		m_imageView->ROISet(_T("EDGE_ROI"), _T("EDGE_ROI"), rt1, RGB(10, 255, 45), 100);
		m_imageView->ROIShow();
	}

}

void CDlgInspectionMain::OnBnClickedUseVhomeTiltTest()
{
	if (FALSE == m_isUseVhomeTiltTest)
	{
		m_imageView->ROIClear();
		m_imageView->ROIHide();
	}
	else
	{
		m_imageView->ROIClear();

		long nSizeX = m_inspectionImageBright.GetSizeX();
		long nSizeY = m_inspectionImageBright.GetSizeY();

		IPVM::Rect rtUp(7000, 100, 7100, 200);
		IPVM::Rect rtDown(7000, 300, 7100, 400);

		m_imageView->ROISet(_T("TILT_UP_ROI"), _T("TILT_UP_ROI"), rtUp, RGB(10, 0, 255), 10);
		//m_imageView->ROISet(_T("TILT_DOWN_ROI"), _T("TILT_DOWN_ROI"), rtDown, RGB(10, 100, 100), 10);
		m_imageView->ROIShow();
	}

}
void CDlgInspectionMain::OnBnClickedButtonCellDesignShowOverlay()
{
	auto* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

	m_imageView->ImageOverlayClear();

	auto* showOverlay = (CXTPPropertyGridItemBool*)m_inspectionParaGrid->FindItem(IDC_PARA_CELL_DESIGN_SHOW_OVERLAY);
	auto* xAxisItem = (CXTPPropertyGridItemNumber*)m_inspectionParaGrid->FindItem(IDC_PARA_CELL_DESIGN_OVERLAY_X_AXIS_MOVE);
	auto* yAxisItem = (CXTPPropertyGridItemNumber*)m_inspectionParaGrid->FindItem(IDC_PARA_CELL_DESIGN_OVERLAY_Y_AXIS_MOVE);

	CString value = showOverlay->GetValue();
	long nXAxisOffset = xAxisItem->GetNumber();
	long nYAxisOffset = yAxisItem->GetNumber();

	if (value == _T("True"))
		//if (inspection->m_para.m_bCellDesignShowOverlay == true)
	{
		DrawCellDesign(nXAxisOffset, nYAxisOffset);
	}
	m_imageView->ImageOverlayShow();
}

void CDlgInspectionMain::DrawCellDesign(long nXAxisOffset, long nYAxisOffset, IPVM::Point_32f_C2& ptLeftTopVhome, IPVM::Point_32f_C2& ptLeftBotVhome, IPVM::Point_32f_C2& ptRightTopVhome, IPVM::Point_32f_C2& ptRightBotVhome, long nTabOffsetX , long nTabOffsetY )
{
	auto* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

	if (m_inspectionImageBright.GetSizeX() > 0 && m_inspectionImageBright.GetSizeY() > 0)
	{
		long nWidth = m_inspectionImageBright.GetSizeX(), nHeight = m_inspectionImageBright.GetSizeY();

		float fCellLengthPx = inspection->m_cellDesign.fCellHeight / (inspection->m_pixelToUmX / 1000);
		float fCellPitchPx = inspection->m_cellDesign.fCellWidth / (inspection->m_pixelToUmY / 1000);
		float fTabSidePx = inspection->m_cellDesign.fTabOffset / (inspection->m_pixelToUmY / 1000);
		float fTabWidthPx = inspection->m_cellDesign.fTabWidth / (inspection->m_pixelToUmY / 1000);
		float fTabHeightPx = inspection->m_cellDesign.fTabHeight / (inspection->m_pixelToUmX / 1000);
		float fVHomeHeightPx = Inspection::Accessor::Get().m_criteriaList.m_vhomeHeight.m_standardValue_mm / (inspection->m_pixelToUmX / 1000);
		CString strSpecmm;

		//Cell 형태 그리기
		float fLeftStart = (nWidth / 2) - (fCellLengthPx / 2);
		float fRightEnd = (nWidth / 2) + (fCellLengthPx / 2);

		if (nTabOffsetX != 0 && nTabOffsetY != 0)
		{
			fRightEnd = nTabOffsetX - (fTabHeightPx / 2);
			fLeftStart = fRightEnd - fCellLengthPx;

			nYAxisOffset = nTabOffsetY - ((nHeight / 2) - (fCellPitchPx / 2) + fTabSidePx + fTabWidthPx/2) ;
		}
		IPVM::LineSeg_32f leftLineTop, leftLineCenter, leftLineBottom, rightLineTop, rightLineCenter, rightLineBottom;
		leftLineTop.m_sx = fLeftStart + nXAxisOffset;
		leftLineTop.m_ex = fLeftStart + nXAxisOffset;
		leftLineTop.m_sy = 0 + nYAxisOffset;
		leftLineTop.m_ey = (nHeight / 2) - (fCellPitchPx / 2) - 25 + nYAxisOffset;

		leftLineCenter.m_sx = fLeftStart + nXAxisOffset;
		leftLineCenter.m_ex = fLeftStart + nXAxisOffset;
		leftLineCenter.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + 25 + nYAxisOffset;
		leftLineCenter.m_ey = (nHeight / 2) + (fCellPitchPx / 2) - 25 + nYAxisOffset;

		leftLineBottom.m_sx = fLeftStart + nXAxisOffset;
		leftLineBottom.m_ex = fLeftStart + nXAxisOffset;
		leftLineBottom.m_sy = (nHeight / 2) + (fCellPitchPx / 2) + 25 + nYAxisOffset;
		leftLineBottom.m_ey = nHeight + nYAxisOffset;

		rightLineTop.m_sx = fRightEnd + nXAxisOffset;
		rightLineTop.m_ex = fRightEnd + nXAxisOffset;
		rightLineTop.m_sy = 0 + nYAxisOffset;
		rightLineTop.m_ey = (nHeight / 2) - (fCellPitchPx / 2) - 25 + nYAxisOffset;

		rightLineCenter.m_sx = fRightEnd + nXAxisOffset;
		rightLineCenter.m_ex = fRightEnd + nXAxisOffset;
		rightLineCenter.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + 25 + nYAxisOffset;
		rightLineCenter.m_ey = (nHeight / 2) + (fCellPitchPx / 2) - 25 + nYAxisOffset;

		rightLineBottom.m_sx = fRightEnd + nXAxisOffset;
		rightLineBottom.m_ex = fRightEnd + nXAxisOffset;
		rightLineBottom.m_sy = (nHeight / 2) + (fCellPitchPx / 2) + 25 + nYAxisOffset;
		rightLineBottom.m_ey = nHeight + nYAxisOffset;

		m_imageView->ImageOverlayAdd(leftLineTop, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(leftLineCenter, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(leftLineBottom, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(rightLineTop, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(rightLineCenter, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(rightLineBottom, RGB(0, 0, 255));

		IPVM::LineSeg_32f leftTopVHomeLine1, leftTopVHomeLine2, rightTopVHomeLine1, rightTopVHomeLine2;
		IPVM::LineSeg_32f leftBotVHomeLine1, leftBotVHomeLine2, rightBotVHomeLine1, rightBotVHomeLine2;

		leftTopVHomeLine1.m_sx = fLeftStart + nXAxisOffset;
		leftTopVHomeLine1.m_sy = (nHeight / 2) - (fCellPitchPx / 2) - 25 + nYAxisOffset;

		if (fVHomeHeightPx > 0)
		{
			leftTopVHomeLine1.m_ex = fLeftStart + fVHomeHeightPx + nXAxisOffset;
			leftTopVHomeLine1.m_ey = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			leftTopVHomeLine1.m_ex = fLeftStart + 50 + nXAxisOffset;
			leftTopVHomeLine1.m_ey = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}
		ptLeftTopVhome = IPVM::Point_32s_C2(fLeftStart + nXAxisOffset, (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset);

		if (fVHomeHeightPx > 0)
		{
			leftTopVHomeLine2.m_sx = fLeftStart + fVHomeHeightPx + nXAxisOffset;
			leftTopVHomeLine2.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			leftTopVHomeLine2.m_sx = fLeftStart + 50 + nXAxisOffset;
			leftTopVHomeLine2.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}

		leftTopVHomeLine2.m_ex = fLeftStart + nXAxisOffset;
		leftTopVHomeLine2.m_ey = (nHeight / 2) - (fCellPitchPx / 2) + 25 + nYAxisOffset;

		leftBotVHomeLine1.m_sx = fLeftStart + nXAxisOffset;
		leftBotVHomeLine1.m_sy = (nHeight / 2) + (fCellPitchPx / 2) - 25 + nYAxisOffset;

		if (fVHomeHeightPx > 0)
		{
			leftBotVHomeLine1.m_ex = fLeftStart + fVHomeHeightPx + nXAxisOffset;
			leftBotVHomeLine1.m_ey = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			leftBotVHomeLine1.m_ex = fLeftStart + 50 + nXAxisOffset;
			leftBotVHomeLine1.m_ey = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}
		ptLeftBotVhome = IPVM::Point_32s_C2(fLeftStart + nXAxisOffset, (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset);
		if (fVHomeHeightPx > 0)
		{
			leftBotVHomeLine2.m_sx = fLeftStart + fVHomeHeightPx + nXAxisOffset;
			leftBotVHomeLine2.m_sy = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			leftBotVHomeLine2.m_sx = fLeftStart + 50 + nXAxisOffset;
			leftBotVHomeLine2.m_sy = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}

		leftBotVHomeLine2.m_ex = fLeftStart + nXAxisOffset;
		leftBotVHomeLine2.m_ey = (nHeight / 2) + (fCellPitchPx / 2) + 25 + nYAxisOffset;

		rightTopVHomeLine1.m_sx = fRightEnd + nXAxisOffset;
		rightTopVHomeLine1.m_sy = (nHeight / 2) - (fCellPitchPx / 2) - 25 + nYAxisOffset;

		if (fVHomeHeightPx > 0)
		{
			rightTopVHomeLine1.m_ex = fRightEnd - fVHomeHeightPx + nXAxisOffset;
			rightTopVHomeLine1.m_ey = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			rightTopVHomeLine1.m_ex = fRightEnd - 50 + nXAxisOffset;
			rightTopVHomeLine1.m_ey = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}
		ptRightTopVhome = IPVM::Point_32s_C2(fRightEnd + nXAxisOffset, (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset);

		if (fVHomeHeightPx > 0)
		{
			rightTopVHomeLine2.m_sx = fRightEnd - fVHomeHeightPx + nXAxisOffset;
			rightTopVHomeLine2.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			rightTopVHomeLine2.m_sx = fRightEnd - 50 + nXAxisOffset;
			rightTopVHomeLine2.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		}

		rightTopVHomeLine2.m_ex = fRightEnd + nXAxisOffset;
		rightTopVHomeLine2.m_ey = (nHeight / 2) - (fCellPitchPx / 2) + 25 + nYAxisOffset;

		rightBotVHomeLine1.m_sx = fRightEnd + nXAxisOffset;
		rightBotVHomeLine1.m_sy = (nHeight / 2) + (fCellPitchPx / 2) - 25 + nYAxisOffset;

		if (fVHomeHeightPx > 0)
		{
			rightBotVHomeLine1.m_ex = fRightEnd - fVHomeHeightPx + nXAxisOffset;
			rightBotVHomeLine1.m_ey = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			rightBotVHomeLine1.m_ex = fRightEnd - 50 + nXAxisOffset;
			rightBotVHomeLine1.m_ey = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}
		ptRightBotVhome = IPVM::Point_32s_C2(fRightEnd + nXAxisOffset, (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset);

		if (fVHomeHeightPx > 0)
		{
			rightBotVHomeLine2.m_sx = fRightEnd - fVHomeHeightPx + nXAxisOffset;
			rightBotVHomeLine2.m_sy = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}
		else
		{
			rightBotVHomeLine2.m_sx = fRightEnd - 50 + nXAxisOffset;
			rightBotVHomeLine2.m_sy = (nHeight / 2) + (fCellPitchPx / 2) + nYAxisOffset;
		}
		rightBotVHomeLine2.m_ex = fRightEnd + nXAxisOffset;
		rightBotVHomeLine2.m_ey = (nHeight / 2) + (fCellPitchPx / 2) + 25 + nYAxisOffset;
		//Cell 형태 그리기

		//전장  라인 그리기
		IPVM::LineSeg_32f leftLengthLine, rightLengthLine;
		IPVM::Triangle_32f leftLengthTriangle1, leftLengthTriangle2, rightLengthTriangle1, rightLengthTriangle2;

		leftLengthLine.m_sx = fLeftStart + nXAxisOffset;
		leftLengthLine.m_ex = fRightEnd + nXAxisOffset;
		leftLengthLine.m_sy = leftBotVHomeLine1.m_sy - 50;
		leftLengthLine.m_ey = leftBotVHomeLine1.m_sy - 50;

		leftLengthTriangle1.m_x0 = fLeftStart + nXAxisOffset;
		leftLengthTriangle1.m_x1 = fLeftStart + nXAxisOffset + 20;
		leftLengthTriangle1.m_x2 = fLeftStart + nXAxisOffset + 20;
		leftLengthTriangle1.m_y0 = leftLengthLine.m_sy;
		leftLengthTriangle1.m_y1 = leftLengthLine.m_sy - 10;
		leftLengthTriangle1.m_y2 = leftLengthLine.m_sy + 10;

		leftLengthTriangle2.m_x0 = fRightEnd + nXAxisOffset;
		leftLengthTriangle2.m_x1 = fRightEnd + nXAxisOffset - 20;
		leftLengthTriangle2.m_x2 = fRightEnd + nXAxisOffset - 20;
		leftLengthTriangle2.m_y0 = leftLengthLine.m_sy;
		leftLengthTriangle2.m_y1 = leftLengthLine.m_sy - 10;
		leftLengthTriangle2.m_y2 = leftLengthLine.m_sy + 10;

		rightLengthLine.m_sx = fLeftStart + nXAxisOffset;
		rightLengthLine.m_ex = fRightEnd + nXAxisOffset;
		rightLengthLine.m_sy = leftTopVHomeLine2.m_ey + 50;
		rightLengthLine.m_ey = leftTopVHomeLine2.m_ey + 50;

		rightLengthTriangle1.m_x0 = fLeftStart + nXAxisOffset;
		rightLengthTriangle1.m_x1 = fLeftStart + nXAxisOffset + 20;
		rightLengthTriangle1.m_x2 = fLeftStart + nXAxisOffset + 20;
		rightLengthTriangle1.m_y0 = rightLengthLine.m_sy;
		rightLengthTriangle1.m_y1 = rightLengthLine.m_sy - 10;
		rightLengthTriangle1.m_y2 = rightLengthLine.m_sy + 10;

		rightLengthTriangle2.m_x0 = fRightEnd + nXAxisOffset;
		rightLengthTriangle2.m_x1 = fRightEnd + nXAxisOffset - 20;
		rightLengthTriangle2.m_x2 = fRightEnd + nXAxisOffset - 20;
		rightLengthTriangle2.m_y0 = rightLengthLine.m_sy;
		rightLengthTriangle2.m_y1 = rightLengthLine.m_sy - 10;
		rightLengthTriangle2.m_y2 = rightLengthLine.m_sy + 10;

		m_imageView->ImageOverlayAdd(leftTopVHomeLine1, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(leftTopVHomeLine2, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(leftBotVHomeLine1, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(leftBotVHomeLine2, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(rightTopVHomeLine1, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(rightTopVHomeLine2, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(rightBotVHomeLine1, RGB(0, 0, 255));
		m_imageView->ImageOverlayAdd(rightBotVHomeLine2, RGB(0, 0, 255));

		m_imageView->ImageOverlayAdd(leftLengthLine, RGB(255, 255, 0));
	
		strSpecmm.Format(_T("(%.2fmm)"), inspection->m_cellDesign.fCellHeight);
	
		m_imageView->ImageOverlayAdd(IPVM::Point_32s_C2(leftLengthLine.CenterPoint().m_x, leftLengthLine.CenterPoint().m_y), _T("Left Legnth") + strSpecmm, RGB(255, 255, 0), 70);
		m_imageView->ImageOverlayAdd(rightLengthLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(IPVM::Point_32s_C2(rightLengthLine.CenterPoint().m_x, rightLengthLine.CenterPoint().m_y), _T("Right Legnth") + strSpecmm, RGB(255, 255, 0), 70);
		m_imageView->ImageOverlayAdd(leftLengthTriangle1, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(leftLengthTriangle2, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(rightLengthTriangle1, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(rightLengthTriangle2, RGB(255, 255, 0));
		//전장  라인 그리기

		//Pitch 라인 그리기
		IPVM::LineSeg_32f pitchUpLine, pitchDownLine, pitchDistanceLine;
		IPVM::Triangle_32f pitchUpTriangle, pitchDownTriangle;

		pitchUpLine.m_sx = rightTopVHomeLine1.m_ex - 30;
		pitchUpLine.m_ex = rightTopVHomeLine1.m_ex;
		pitchUpLine.m_sy = rightTopVHomeLine1.m_ey;
		pitchUpLine.m_ey = rightTopVHomeLine1.m_ey;

		pitchDownLine.m_sx = rightBotVHomeLine1.m_ex - 30;
		pitchDownLine.m_ex = rightBotVHomeLine1.m_ex;
		pitchDownLine.m_sy = rightBotVHomeLine1.m_ey;
		pitchDownLine.m_ey = rightBotVHomeLine1.m_ey;

		pitchDistanceLine.m_sx = pitchUpLine.CenterPoint().m_x;
		pitchDistanceLine.m_ex = pitchUpLine.CenterPoint().m_x;
		pitchDistanceLine.m_sy = pitchUpLine.m_sy;
		pitchDistanceLine.m_ey = pitchDownLine.m_sy;

		pitchUpTriangle.m_x0 = pitchDistanceLine.m_sx;
		pitchUpTriangle.m_x1 = pitchDistanceLine.m_sx - 10;
		pitchUpTriangle.m_x2 = pitchDistanceLine.m_sx + 10;
		pitchUpTriangle.m_y0 = pitchDistanceLine.m_sy;
		pitchUpTriangle.m_y1 = pitchDistanceLine.m_sy + 20;
		pitchUpTriangle.m_y2 = pitchDistanceLine.m_sy + 20;

		pitchDownTriangle.m_x0 = pitchDistanceLine.m_sx;
		pitchDownTriangle.m_x1 = pitchDistanceLine.m_sx - 10;
		pitchDownTriangle.m_x2 = pitchDistanceLine.m_sx + 10;
		pitchDownTriangle.m_y0 = pitchDistanceLine.m_ey;
		pitchDownTriangle.m_y1 = pitchDistanceLine.m_ey - 20;
		pitchDownTriangle.m_y2 = pitchDistanceLine.m_ey - 20;

		m_imageView->ImageOverlayAdd(pitchUpLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(pitchDownLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(pitchDistanceLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(pitchUpTriangle, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(pitchDownTriangle, RGB(255, 255, 0));

		strSpecmm.Format(_T("(%.2fmm)"), inspection->m_cellDesign.fCellWidth);
		m_imageView->ImageOverlayAdd(IPVM::Point_32s_C2(pitchDistanceLine.CenterPoint().m_x - 300, pitchDistanceLine.CenterPoint().m_y), _T("Pitch") + strSpecmm, RGB(255, 255, 0), 50);
		//Pitch 라인 그리기

		//Tab 폭 라인 그리기
		IPVM::Rect tabRect;
		tabRect.m_left = fRightEnd + nXAxisOffset;
		tabRect.m_right = fRightEnd + fTabHeightPx + nXAxisOffset;
		tabRect.m_top = (nHeight / 2) - (fCellPitchPx / 2) + fTabSidePx + nYAxisOffset;
		tabRect.m_bottom = tabRect.m_top + fTabWidthPx;

		m_imageView->ImageOverlayAdd(tabRect, RGB(0, 0, 255));

		IPVM::LineSeg_32f tabWidthUpLine, tabWidthDownLine, tabWidthDistanceLine;
		IPVM::Triangle_32f tabWidthUpTriangle, tabWidthDownTriangle;

		tabWidthUpLine.m_sx = tabRect.m_right;
		tabWidthUpLine.m_ex = tabRect.m_right + 30;
		tabWidthUpLine.m_sy = tabRect.m_top;
		tabWidthUpLine.m_ey = tabRect.m_top;

		tabWidthDownLine.m_sx = tabRect.m_right;
		tabWidthDownLine.m_ex = tabRect.m_right + 30;
		tabWidthDownLine.m_sy = tabRect.m_bottom;
		tabWidthDownLine.m_ey = tabRect.m_bottom;

		tabWidthDistanceLine.m_sx = tabWidthUpLine.CenterPoint().m_x;
		tabWidthDistanceLine.m_ex = tabWidthUpLine.CenterPoint().m_x;
		tabWidthDistanceLine.m_sy = tabWidthUpLine.m_sy;
		tabWidthDistanceLine.m_ey = tabWidthDownLine.m_ey;

		tabWidthUpTriangle.m_x0 = tabWidthDistanceLine.m_sx;
		tabWidthUpTriangle.m_x1 = tabWidthDistanceLine.m_sx - 10;
		tabWidthUpTriangle.m_x2 = tabWidthDistanceLine.m_sx + 10;
		tabWidthUpTriangle.m_y0 = tabWidthDistanceLine.m_sy;
		tabWidthUpTriangle.m_y1 = tabWidthDistanceLine.m_sy + 20;
		tabWidthUpTriangle.m_y2 = tabWidthDistanceLine.m_sy + 20;

		tabWidthDownTriangle.m_x0 = tabWidthDistanceLine.m_sx;
		tabWidthDownTriangle.m_x1 = tabWidthDistanceLine.m_sx - 10;
		tabWidthDownTriangle.m_x2 = tabWidthDistanceLine.m_sx + 10;
		tabWidthDownTriangle.m_y0 = tabWidthDistanceLine.m_ey;
		tabWidthDownTriangle.m_y1 = tabWidthDistanceLine.m_ey - 20;
		tabWidthDownTriangle.m_y2 = tabWidthDistanceLine.m_ey - 20;

		m_imageView->ImageOverlayAdd(tabWidthUpLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabWidthDownLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabWidthDistanceLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabWidthUpTriangle, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabWidthDownTriangle, RGB(255, 255, 0));

		strSpecmm.Format(_T("(%.2fmm)"), inspection->m_cellDesign.fTabWidth);
		m_imageView->ImageOverlayAdd(IPVM::Point_32s_C2(tabWidthDistanceLine.CenterPoint().m_x + 10, tabWidthDistanceLine.CenterPoint().m_y), _T("Tab Width") + strSpecmm, RGB(255, 255, 0), 50);
		//Tab 폭 라인 그리기

		//Tab Side 라인 그리기
		IPVM::LineSeg_32f tabSideUpLine, tabSideDownLine, tabSideDistanceLine;
		IPVM::Triangle_32f tabSideUpTriangle, tabSideDownTriangle;

		tabSideUpLine.m_sx = fRightEnd + nXAxisOffset;
		tabSideUpLine.m_ex = fRightEnd + nXAxisOffset + 30;
		tabSideUpLine.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		tabSideUpLine.m_ey = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;

		tabSideDownLine.m_sx = fRightEnd + nXAxisOffset;
		tabSideDownLine.m_ex = fRightEnd + nXAxisOffset + 30;
		tabSideDownLine.m_sy = tabRect.m_top;
		tabSideDownLine.m_ey = tabRect.m_top;

		tabSideDistanceLine.m_sx = tabSideUpLine.CenterPoint().m_x;
		tabSideDistanceLine.m_ex = tabSideUpLine.CenterPoint().m_x;
		tabSideDistanceLine.m_sy = (nHeight / 2) - (fCellPitchPx / 2) + nYAxisOffset;
		tabSideDistanceLine.m_ey = tabRect.m_top;

		tabSideUpTriangle.m_x0 = tabSideDistanceLine.m_sx;
		tabSideUpTriangle.m_x1 = tabSideDistanceLine.m_sx - 10;
		tabSideUpTriangle.m_x2 = tabSideDistanceLine.m_sx + 10;
		tabSideUpTriangle.m_y0 = tabSideDistanceLine.m_sy;
		tabSideUpTriangle.m_y1 = tabSideDistanceLine.m_sy + 20;
		tabSideUpTriangle.m_y2 = tabSideDistanceLine.m_sy + 20;

		tabSideDownTriangle.m_x0 = tabSideDistanceLine.m_sx;
		tabSideDownTriangle.m_x1 = tabSideDistanceLine.m_sx - 10;
		tabSideDownTriangle.m_x2 = tabSideDistanceLine.m_sx + 10;
		tabSideDownTriangle.m_y0 = tabSideDistanceLine.m_ey;
		tabSideDownTriangle.m_y1 = tabSideDistanceLine.m_ey - 20;
		tabSideDownTriangle.m_y2 = tabSideDistanceLine.m_ey - 20;

		m_imageView->ImageOverlayAdd(tabSideUpLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabSideDownLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabSideDistanceLine, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabSideUpTriangle, RGB(255, 255, 0));
		m_imageView->ImageOverlayAdd(tabSideDownTriangle, RGB(255, 255, 0));

		strSpecmm.Format(_T("Tab Side(%.2fmm)\nTab Height(%.2fmm)"), inspection->m_cellDesign.fTabOffset, inspection->m_cellDesign.fTabHeight);
		m_imageView->ImageOverlayAdd(IPVM::Point_32s_C2(tabSideDistanceLine.CenterPoint().m_x + 10, tabSideDistanceLine.CenterPoint().m_y), strSpecmm, RGB(255, 255, 0), 50);
		//Tab Side 라인 그리기
	}
}

void CDlgInspectionMain::MoveROItoPoint(IPVM::Rect& rtRoi, const IPVM::Point_32f_C2& pt)
{
	auto& rect = rtRoi;
	rect.OffsetRect(pt);
	auto a = rect.CenterPoint() - pt;
	rect.m_left -= a.m_x;
	rect.m_right -= a.m_x;
	rect.m_top -= a.m_y;
	rect.m_bottom -= a.m_y;
}
void CDlgInspectionMain::UpdateInspectionParaGridChattering()
{
    auto* chattering = (CInspectionChattering*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
    m_imageView->ROIClear();
    m_inspectionParaGrid->ResetContent();

    if (auto* subcategory = m_inspectionParaGrid->AddCategory(_T("Chattering Para")))
    {
        if (auto* item = PropertyGridHelperEx::AddLink_Bool(subcategory, _T("Use Inspection"), chattering->m_para.m_bUseInspection))
        {
            item->SetID(IDC_PARA_CHATTERING_USE_INSPECTION);
        }
        
        if (auto* item = PropertyGridHelperEx::AddLink_Enum(subcategory, _T("Select ImageType (Bright/Dark)"), chattering->m_para.m_nSelectDarkImage))
        {
            item->GetConstraints()->AddConstraint(_T("Bright Image"), 0);
            item->GetConstraints()->AddConstraint(_T("Dark Image"), 1);

            item->SetID(IDC_PARA_CHATTERING_SELECT_DARK_IMAGE);
        }
        if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Div Count"), chattering->m_para.m_nDivCount))
        {
            item->SetID(IDC_PARA_CHATTERING_DIV_COUNT);
        }
        if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Standard"), chattering->m_para.m_nStandard))
        {
            item->SetID(IDC_PARA_CHATTERING_STANDARD);
        }
        if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Period_Max"), chattering->m_para.m_nPeriodMax))
        {
            item->SetID(IDC_PARA_CHATTERING_PERIOD_MAX);
        }
        if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Period_Min"), chattering->m_para.m_nPeriodMin))
        {
            item->SetID(IDC_PARA_CHATTERING_PERIOD_MIN);
        }
        if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Amp_Max"), chattering->m_para.m_nAmpMax))
        {
            item->SetID(IDC_PARA_CHATTERING_AMP_MAX);
        }
        if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Amp_Min"), chattering->m_para.m_nAmpMin))
        {
            item->SetID(IDC_PARA_CHATTERING_AMP_MIN);
        }
        if (auto* item = PropertyGridHelperEx::AddLink_Rect(subcategory, _T("Set Align ROI"), IPVM::ToMFC(chattering->m_para.m_rtOffsetRoi)))
        {
            item->SetID(IDC_PARA_CHATTERING_SET_ALIGN_ROI);
            OnChangedChatteringlRoi(IDC_PARA_CHATTERING_SET_ALIGN_ROI);
        }
        if (auto* showROI = subcategory->AddChildItem(new CXTPPropertyGridItemBool(_T("Set ROI"))))
        {
            showROI->SetID(IDC_PARA_CHATTERING_SET_USER_ROI);

            SetExplain(showROI, IDC_PARA_CHATTERING_SET_USER_ROI);
            showROI->SetValue(_T("False"));
            OnBnClickedButtonParaCellAlignVhomeROI(IDC_PARA_CHATTERING_SET_USER_ROI);
        }

        subcategory->Expand();
    }
    m_imageView->ImageOverlayShow();

}

void CDlgInspectionMain::UpdateInspectionParaGridDeeplearning()
{
	auto* deeplearning = (CInspectionDeeplearning*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	m_imageView->ROIClear();
	m_inspectionParaGrid->ResetContent();

	CString strInline;
	if (m_nLanguage == 3)
		strInline = _T("算法种类");
	else
		strInline = _T("Inline");
	m_inspectionParaGrid->GetImageManager()->SetMaskColor(0xC0C0C0);
	m_inspectionParaGrid->GetImageManager()->SetIcons(IDB_BITMAP_CIS1_IMAGE, 0, 8, CSize(50, 20));
	//if (auto* category = m_inspectionParaGrid->AddCategory(strInline))
	//{
	//	CString strValue;
	//	category->Expand();
	//}

	CString strCell;

	if (m_nLanguage == 3)
		strCell = _T("Deeplearning");
	else
		strCell = _T("Deeplearning");

	if (auto* category = m_inspectionParaGrid->AddCategory(strCell))
	{
		if (auto* item = PropertyGridHelperEx::AddLink_CheckBox(category, _T("Use Deeplearning"), deeplearning->m_para.m_bUseInspection))
		{
			//item->SetID(IDC_PARA_SHOULDER_LINE_GAUSSIAN_DEVIATION);
			//SetExplain(item, IDC_PARA_SHOULDER_LINE_GAUSSIAN_DEVIATION);
		}
		
		//(long)DefectType::END_Coater_Surface;
		for (long cnt = 0; cnt < deeplearning->m_para.m_nCountDefectType; cnt++)
		{
			long nType = (long)DefectType::START_Coater_Surface + cnt + 1;

			CString caption;
			caption.Format(_T("%s%s"), _T("Use Deeplearning for "), GetDefectName((DefectType)nType));
			
			if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, caption, deeplearning->m_para.m_vecbUseInspection[cnt]))
			{
				//item->SetID(IDC_PARA_SHOULDER_LINE_GAUSSIAN_DEVIATION);
				//SetExplain(item, IDC_PARA_SHOULDER_LINE_GAUSSIAN_DEVIATION);
			}
		}
		category->Expand();
	}

	m_imageView->ImageOverlayShow();
}

void CDlgInspectionMain::UpdateInspectionParaGridAbnormalInspection()
{
    auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
    auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::Blob);
	auto* abnormalInspection = (CInspectionAbnormal*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	//auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

	m_imageView->ROIClear();
	m_inspectionParaGrid->ResetContent();
	//long nTotalInspCount = inspection->m_para.m_vecBlobParaCondition.size();
	CString strNameInsp = _T("Abnormal Inspeciton ROI");
	if (auto* subcategory = m_inspectionParaGrid->AddCategory(strNameInsp))
	{
		if (auto* item = PropertyGridHelperEx::AddLink_Bool(subcategory, _T("Use Inspection"), abnormalInspection->m_para.m_bInspectionSkip))
		{
			item->SetID(IDC_PARA_ABNORMAL_USE_INSPECTION);
		}
		if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("GrayValue"), abnormalInspection->m_para.m_nGrayValue))
		{
			item->SetID(IDC_PARA_ABNORMAL_GV);
		}
		if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("GrayValue 2"), abnormalInspection->m_para.m_nGrayValue_2))
		{
            item->SetID(IDC_PARA_ABNORMAL_GV_2);
            if (cellAlign->m_para.m_nLineCountMeasure < 6)
            {
                item->SetReadOnly();
			}
            else
            {
                item->SetReadOnly(false);
			}
		}
		//if (auto* item = PropertyGridHelperEx::AddLink_Rect(subcategory, _T("  Adjust Blob Offset[+:Deflate]"), IPVM::ToMFC(abnormalInspection->m_para.m_rtOffsetRoi)))
		//{
		//	item->SetID(IDC_PARA_ABNORMAL_OFFSET_ROI);
  //          //OnChangedChatteringlRoi(IDC_PARA_CHATTERING_SET_ALIGN_ROI);
		//}
        if (auto* showROI = subcategory->AddChildItem(new CXTPPropertyGridItemBool(_T("Set ROI"))))
        {
            showROI->SetID(IDC_PARA_ABNORMAL_SET_USER_ROI);

            SetExplain(showROI, IDC_PARA_ABNORMAL_SET_USER_ROI);
            showROI->SetValue(_T("False"));
            OnBnClickedButtonParaCellAlignVhomeROI(IDC_PARA_ABNORMAL_SET_USER_ROI);
        }
		subcategory->Expand();
	}
	m_imageView->ImageOverlayShow();
}



void CDlgInspectionMain::UpdateInspectionParaGridCellAlign()
{
	auto* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	auto* inspBlob = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::Blob);

	m_imageView->ROIClear();

	m_inspectionParaGrid->ResetContent();

	CString strInline;
	if (m_nLanguage == 3)
		strInline = _T("算法种类");
	else
		strInline = _T("Inline");
	m_inspectionParaGrid->GetImageManager()->SetMaskColor(0xC0C0C0);
	m_inspectionParaGrid->GetImageManager()->SetIcons(IDB_BITMAP_CIS1_IMAGE, 0, 8, CSize(50, 20));
	//if (auto* category = m_inspectionParaGrid->AddCategory(strInline))
	//{
	//	CString strValue;
	//	category->Expand();
	//}
		
	CString strCell;
    strCell = _T("General");

	if (auto* categoryGeneral = m_inspectionParaGrid->AddCategory(strCell))
    {
        if (auto* item = PropertyGridHelperEx::AddLink_Enum(categoryGeneral,
                _T("Select ImageType (Bright/Dark/ROI별 선택)"), inspection->m_para.m_nSelectImageType))
        {
            item->GetConstraints()->AddConstraint(_T("Bright Image"), 0);
            item->GetConstraints()->AddConstraint(_T("Dark Image"), 1);
            item->GetConstraints()->AddConstraint(_T("ROI별 선택"), 2);

            item->SetID(IDC_PARA_ALIGN_SELECT_IMAGE_TYPE);

			item->SetDescription(_T("검사할 이미지 Bright(전체) / Dark(전체) / ROI별로 Bright or Dark 개별 선택(각 Rect Detect Line에서 설정 가능)"));
        }

		CString strLine = _T("Measure ROI -> Blob ROI");
        if (auto* item = PropertyGridHelper::AddLink_Button(categoryGeneral, strLine, TRUE, FALSE))
        {
			item->SetID(IDC_PARA_BUTTON_COPY_MEASURE_ROI);
        }
        strLine = _T("Blob ROI -> Measure ROI");
        if (auto* item = PropertyGridHelper::AddLink_Button(categoryGeneral, strLine, TRUE, FALSE))
        {
			item->SetID(IDC_PARA_BUTTON_COPY_BLOB_ROI);
        }
        strLine = _T("Auto Find Edge Detect Rect");
        if (auto* item = PropertyGridHelper::AddLink_Button(categoryGeneral, strLine, TRUE, FALSE))
        {
            item->SetID(IDC_PARA_BUTTON_AUTO_FIND_EDGEDETECT_ROI);
			// item->SetHidden(TRUE);
        } 
		strLine = _T("Inspection Type");
        if (auto* item = PropertyGridHelper::AddLink_Enum(categoryGeneral, strLine, inspection->m_para.m_nInspectionType))
        {
            item->SetID(IDC_PARA_ALIGN_SELECT_INSPTYPE);

            item->GetConstraints()->AddConstraint(_T("Defect #1"), 0);
            item->GetConstraints()->AddConstraint(_T("Defect #2"), 1);
            item->GetConstraints()->AddConstraint(_T("Defect #3"), 2);
            item->GetConstraints()->AddConstraint(_T("Defect #4"), 3);
        }
        categoryGeneral->Expand();
    }

	if (m_nLanguage == 3)
		strCell = _T("关于电极临界线(Measure)");
	else
		strCell = _T("Cell(Measure Align)");

	if (auto* categoryMeasure = m_inspectionParaGrid->AddCategory(strCell))
	{
		auto* showMeasureROI = categoryMeasure->AddChildItem(new CXTPPropertyGridItemBool(strShowVHomeROI));
		showMeasureROI->SetID(IDC_PARA_SHOW_ROI_MEASURE);

		SetExplain(showMeasureROI, IDC_PARA_SHOW_ROI_MEASURE);
		showMeasureROI->SetValue(_T("False"));

		OnBnClickedButtonParaCellAlignVhomeROI(IDC_PARA_SHOW_ROI_MEASURE);

		if (auto* item = PropertyGridHelper::AddLink(categoryMeasure, _T("    Detect Line Count(Measure)"), inspection->m_para.m_nLineCountMeasure))
		{
			item->SetID(IDC_PARA_DEFECT_LINE_COUNT_MEASURE);

			SetExplain(item, IDC_PARA_DEFECT_LINE_COUNT_MEASURE);
			item->SetDescription(_T("Detect Line Count(Measure)"));

			// 임시로 막음
			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}

		CString strLine;
		for (long cnt = 0; cnt < CONST_CELL_LINE; cnt++)
		{
			strLine.Format(_T("Measure Align Rect Detect Line %02d"), cnt);
			if (auto* item = PropertyGridHelper::AddLink_Button(categoryMeasure, strLine, TRUE, FALSE))
			{
				item->SetID(IDC_PARA_BUTTON_LINEDETECT_PARA_MEASURE_00 + cnt);
				item->SetHidden(!(inspection->m_para.m_nLineCountMeasure > cnt));
			}
		}

		if (auto* item = PropertyGridHelper::AddLink_Bool(categoryMeasure, _T("Add Center ROI Split Measure"), inspection->m_para.m_bUseCenterROISplitMeasure))
        {
            item->SetID(IDC_PARA_USE_CENTER_ROI_SPLIT);
        }

		categoryMeasure->Expand();
	}

	if (m_nLanguage == 3)
		strCell = _T("关于电极临界线(Blob)");
	else
		strCell = _T("Cell(Blob Align)");

	if (auto* categoryBlob = m_inspectionParaGrid->AddCategory(strCell))
	{
		auto* showROIBlob = categoryBlob->AddChildItem(new CXTPPropertyGridItemBool(strShowVHomeROI));
		showROIBlob->SetID(IDC_PARA_SHOW_ROI_BLOB);

		SetExplain(showROIBlob, IDC_PARA_SHOW_ROI_BLOB);
		showROIBlob->SetValue(_T("False"));

		OnBnClickedButtonParaCellAlignVhomeROI(IDC_PARA_SHOW_ROI_BLOB);

		if (auto* item = PropertyGridHelper::AddLink(categoryBlob, _T("    Detect Line Count(Blob)"), inspection->m_para.m_nLineCountBlob))
		{
			item->SetID(IDC_PARA_DEFECT_LINE_COUNT_BLOB);

			SetExplain(item, IDC_PARA_DEFECT_LINE_COUNT_BLOB);
			item->SetDescription(_T("Detect Line Count(Blob)"));

			// 임시로 막음
			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}

		CString strLine;
		for (long cnt = 0; cnt < CONST_CELL_LINE; cnt++)
		{
			strLine.Format(_T("Blob Align Rect Detect Line %02d"), cnt);
			if (auto* item = PropertyGridHelper::AddLink_Button(categoryBlob, strLine, TRUE, FALSE))
			{
				item->SetID(IDC_PARA_BUTTON_LINEDETECT_PARA_BLOB_00 + cnt);
				item->SetHidden(!(inspection->m_para.m_nLineCountBlob > cnt));
			}
		}

		if (auto* item = PropertyGridHelper::AddLink_Bool(categoryBlob, _T("Add Center ROI Split Blob"), inspection->m_para.m_bUseCenterROISplitBlob))
        {
            item->SetID(IDC_PARA_USE_CENTER_ROI_SPLIT);
        }
        categoryBlob->Expand();
	}
	m_imageView->ImageOverlayShow();

}

void CDlgInspectionMain::OnSelectImageType(LPARAM lparam, const int& nID)
{
    // 어디에 셋팅할지...??
    auto* inspection
        = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

    // 0 = Brightness
    // 1 = Dark
    //inspection->m_para.m_nImageDark
}
void CDlgInspectionMain::OnBnClickSelectInspType(const int& nID)
{
	auto* inspection
        = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
	
}
void CDlgInspectionMain::OnBnClickedButtonMSAAlign(const int& nID)
{
    auto* measure
        = (CInspectionMeasure*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::measure);

	if (nID == IDC_PARA_BUTTON_MSA_ALIGN)
    {
        measure->AlignMSA();
        m_imageView->SetImage(measure->m_inspectionImageBright);

        m_imageView->ImageOverlayAdd(measure->m_ptMSALeft, RGB(0, 255, 0), 7);
        m_imageView->ImageOverlayAdd(measure->m_ptMSARight, RGB(0, 255, 0), 7);
        m_imageView->ImageOverlayAdd(measure->m_ptMSALeftExpand, RGB(255, 0, 0), 7);

		m_imageView->ImageOverlayAdd(measure->m_MSAlineUp, RGB(255, 165, 0), 2);
        m_imageView->ImageOverlayAdd(measure->m_MSAlineLeft, RGB(255, 165, 0), 2);
        m_imageView->ImageOverlayAdd(measure->m_MSAlineRight, RGB(255, 165, 0), 2);

		m_imageView->ImageOverlayShow();
	}
}

void CDlgInspectionMain::OnBnClickedButtonCopyROI(const int& nID)
{
    auto* inspection
        = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
    CString strLine;

	
	
    // Measure ROI를 Blob ROI로 복사
    if (nID == IDC_PARA_BUTTON_COPY_MEASURE_ROI)
    {
        for (long cnt = 0; cnt < inspection->m_para.m_nLineCountMeasure; cnt++)
        {
            inspection->m_para.m_vecrtDetectLineBlob[cnt] = inspection->m_para.m_vecrtDetectLineMeasure[cnt];
            //inspection->m_para.m_vecDetectLineParaBlob[cnt] = inspection->m_para.m_vecDetectLineParaMeasure[cnt];
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_HalfFilterLength = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_HalfFilterLength;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_GaussianDeviation = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_GaussianDeviation;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_FindFirstEdge = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_FindFirstEdge;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_DetectType = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_DetectType;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_SearchDirection = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_SearchDirection;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_EdgeThreshold = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_EdgeThreshold;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_RansacThreshold = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_RansacThreshold;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_DetectCount = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_DetectCount;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_ImageTypeDark = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_ImageTypeDark;
            inspection->m_para.m_vecDetectLineParaBlob[cnt].m_EdgeDetectMode = inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_EdgeDetectMode;

		}
	}

    // Blob ROI를 Measure ROI로 복사
    else if (nID == IDC_PARA_BUTTON_COPY_BLOB_ROI)
    {
        for (long cnt = 0; cnt < inspection->m_para.m_nLineCountBlob; cnt++)
        {
            inspection->m_para.m_vecrtDetectLineMeasure[cnt] = inspection->m_para.m_vecrtDetectLineBlob[cnt];
            //inspection->m_para.m_vecDetectLineParaMeasure[cnt] = inspection->m_para.m_vecDetectLineParaBlob[cnt];
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_HalfFilterLength
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_HalfFilterLength;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_GaussianDeviation
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_GaussianDeviation;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_FindFirstEdge
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_FindFirstEdge;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_DetectType
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_DetectType;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_SearchDirection
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_SearchDirection;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_EdgeThreshold
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_EdgeThreshold;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_RansacThreshold
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_RansacThreshold;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_DetectCount
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_DetectCount;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_ImageTypeDark
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_ImageTypeDark;
            inspection->m_para.m_vecDetectLineParaMeasure[cnt].m_EdgeDetectMode
                = inspection->m_para.m_vecDetectLineParaBlob[cnt].m_EdgeDetectMode;
		}
	}
}
void CDlgInspectionMain::AutoFindEdgeDetectROI2()
{
    auto* inspection
        = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

    IPVM::Image_8u_C1 imgSource;
    if (inspection->m_para.m_nSelectImageType)
    {
        imgSource = m_inspectionImageDark;
    }
    else
    {
        imgSource = m_inspectionImageBright;
    }

    long imageSizeX = imgSource.GetSizeX();
    long imageSizeY = imgSource.GetSizeY();

    if (imageSizeX == 0)
    {
        return;
    }

    // Step #1. Average Line Profile
    std::vector<BYTE> vecByAvg;
    vecByAvg.resize(imageSizeX);

    for (int nX = 0; nX < imageSizeX; nX++)
    {
        long nSum = 0;
        for (int nY = imageSizeY / 2 - 50; nY < imageSizeY / 2 + 50; nY++)
        {
            BYTE* pbyCheckVal = imgSource.GetMem(nX, nY);
            nSum += *pbyCheckVal;
        }
        vecByAvg[nX] = nSum / 100;
        /*CString str;
        str.Format(_T("%d : %d \n"), nX, vecByAvg[nX]);
        TRACE(str);*/
    }

    // Step #2. Find Edge Line
    std::vector<IPVM::Point_32f_C2> vecPtCheckingPoint;
    vecPtCheckingPoint.resize(0);
    BYTE byStartValue;
    int nStartValue;
    IPVM::Point_32f_C2 ptCheck;
    int nForStart = 500;
    int nContinueCnt = 0;
    int nTabDiffPx = 893;
    nStartValue = nForStart;
    for (int nX = nForStart; nX < imageSizeX - nForStart, vecPtCheckingPoint.size() < 2; nX++)
    {
       
        if (abs(vecByAvg[nStartValue] - vecByAvg[nX]) > 100)
        {
            // 탭간 유의차일 경우
            if (abs(nStartValue - nX) > nTabDiffPx - 10 && abs(nStartValue - nX) < nTabDiffPx + 10)
                continue;

            // 1. average 값을 이용하면 좋을 듯

            // 2. 앞뒤 기준으로 연속된 값(차이가 얼마 나지 않는 부분을 찾아서 continue)
            //&& (nX - nStartValue < 100)
            vector<IPVM::Point_32f_C2> vecPtTempChecking;
            IPVM::LineEq_32f lineCheckingPos;
            long nCntCompareData = 3;

            for (int cnt = -2; cnt < nCntCompareData; cnt++)
            {
                IPVM::Point_32f_C2 ptTemp;
                ptTemp.m_x = cnt + 2;
                ptTemp.m_y = vecByAvg[nX + cnt];
                vecPtTempChecking.push_back(ptTemp);
            }

            if (IPVM::DataFitting::FitToLine(vecPtTempChecking.size(), &vecPtTempChecking[0], lineCheckingPos)
                != IPVM::Status::success)
            {
                // fittoline failed
            }

            float fSlope = abs(lineCheckingPos.m_a / lineCheckingPos.m_b);
            if (fSlope < 0.2)
            {
                nContinueCnt++;

                if (nContinueCnt > 50)
                {
                    nStartValue = nX;
                }
                continue;
            }

            //if (nX - nStartValue < 100)
            //continue;

            ptCheck.m_x = nX;
            ptCheck.m_y = imageSizeY / 2;
            vecPtCheckingPoint.push_back(ptCheck);
            nStartValue = nX;
        }
    }
    nStartValue = imageSizeX - nForStart;
    for (int nX = imageSizeX - nForStart; nX < nForStart, vecPtCheckingPoint.size() < 4 ; nX--)
    {
        
        if (abs(vecByAvg[nStartValue] - vecByAvg[nX]) > 100)
        {
            // 탭간 유의차일 경우
            if (abs(nStartValue - nX) > nTabDiffPx - 10 && abs(nStartValue - nX) < nTabDiffPx + 10)
                continue;

            // 1. average 값을 이용하면 좋을 듯

            // 2. 앞뒤 기준으로 연속된 값(차이가 얼마 나지 않는 부분을 찾아서 continue)
            //&& (nX - nStartValue < 100)
            vector<IPVM::Point_32f_C2> vecPtTempChecking;
            IPVM::LineEq_32f lineCheckingPos;
            long nCntCompareData = 3;

            for (int cnt = -2; cnt < nCntCompareData; cnt++)
            {
                IPVM::Point_32f_C2 ptTemp;
                ptTemp.m_x = cnt + 2;
                ptTemp.m_y = vecByAvg[nX + cnt];
                vecPtTempChecking.push_back(ptTemp);
            }

            if (IPVM::DataFitting::FitToLine(vecPtTempChecking.size(), &vecPtTempChecking[0], lineCheckingPos)
                != IPVM::Status::success)
            {
                // fittoline failed
            }

            float fSlope = abs(lineCheckingPos.m_a / lineCheckingPos.m_b);
            if (fSlope < 0.2)
            {
                nContinueCnt++;

                if (nContinueCnt > 50)
                {
                    nStartValue = nX;
                }
                continue;
            }

            //if (nX - nStartValue < 100)
            //continue;

            ptCheck.m_x = nX;
            ptCheck.m_y = imageSizeY / 2;
            vecPtCheckingPoint.push_back(ptCheck);
            nStartValue = nX;
        }
    }


    // Step #3. Find Point 기준으로 ROI 만들기 (좌/우 100 정도?)
    long nFindCheckPtCnt = vecPtCheckingPoint.size();

    if (nFindCheckPtCnt ==0)
    {
        AfxMessageBox(_T("Find Failed"));
        return;
    }
    if (vecPtCheckingPoint.size() == 4)
    {
        IPVM::Point_32f_C2 temp;
        temp = vecPtCheckingPoint[2];
        vecPtCheckingPoint[2] = vecPtCheckingPoint[3];
        vecPtCheckingPoint[3] = temp;

    }
        
    std::vector<IPVM::Rect> vecFindDetectLineROi;
    vecFindDetectLineROi.resize(0);

    for (int nIdx = 0; nIdx < nFindCheckPtCnt; nIdx++)
    {
        long nLeft = vecPtCheckingPoint[nIdx].m_x - 500;
        long nRight = vecPtCheckingPoint[nIdx].m_x + 500;
        if (nLeft < 0)
        {
            nLeft = 0;
        }

        if (nRight > imageSizeX)
        {
            nRight = imageSizeX;
        }

        IPVM::Rect rtMakeRoi(nLeft, vecPtCheckingPoint[nIdx].m_y - 1000, nRight, vecPtCheckingPoint[nIdx].m_y + 1000);

        vecFindDetectLineROi.push_back(rtMakeRoi);
    }

    // Step #4. Find Rect를 Detect Line Rect 값에 넣기
    //inspection->m_para.m_nLineCountMeasure = vecFindDetectLineROi.size();
    ///inspection->m_para.m_vecrtDetectLineMeasure.resize(inspection->m_para.m_nLineCountMeasure);
    long nVecSize = vecFindDetectLineROi.size();
    if (nVecSize > 21)
        nVecSize = 21;

    for (long cnt = 0; cnt < nVecSize; cnt++)
    {
        inspection->m_para.m_vecrtDetectLineMeasure[cnt] = vecFindDetectLineROi[cnt];
    }
}
void CDlgInspectionMain::OnBnClickedButtonAutoFindEdgeDetectROI(const int& nID)
{
    AutoFindEdgeDetectROI2();
    return;
    auto* inspection
        = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
    
	
	IPVM::Image_8u_C1 imgSource;
    if (inspection->m_para.m_nSelectImageType)
    {
        imgSource = m_inspectionImageDark;
    }
    else
    {
        imgSource = m_inspectionImageBright;
    }

    long imageSizeX = imgSource.GetSizeX();
    long imageSizeY = imgSource.GetSizeY();

    if (imageSizeX == 0)
    {
        return;
    }

	// Step #1. Average Line Profile 
	std::vector<BYTE> vecByAvg;
    vecByAvg.resize(imageSizeX);
	
	for (int nX = 0; nX < imageSizeX; nX++)
    {
        long nSum = 0;
        for (int nY = imageSizeY / 2 - 50; nY < imageSizeY / 2 + 50; nY++)
        {
            BYTE* pbyCheckVal = imgSource.GetMem(nX, nY);
            nSum += *pbyCheckVal;
		}
        vecByAvg[nX] = nSum / 100;
        /*CString str;
        str.Format(_T("%d : %d \n"), nX, vecByAvg[nX]);
        TRACE(str);*/
	}

	// Step #2. Find Edge Line 
    std::vector<IPVM::Point_32f_C2> vecPtCheckingPoint;
    vecPtCheckingPoint.resize(0);
    BYTE byStartValue;
    int nStartValue;
    IPVM::Point_32f_C2 ptCheck;
    int nForStart = 500;
    int nContinueCnt = 0;
    int nTabDiffPx = 893;

    for (int nX = nForStart; nX < imageSizeX - 1; nX++)
    {
        if (nX == nForStart)
        {
            nStartValue = nForStart;
            continue;
		}
        
        if (abs(vecByAvg[nStartValue] - vecByAvg[nX]) > 100)
        {
			// 탭간 유의차일 경우
            if (abs(nStartValue - nX) > nTabDiffPx - 10 && abs(nStartValue - nX) < nTabDiffPx + 10)
                continue;

			// 1. average 값을 이용하면 좋을 듯

			// 2. 앞뒤 기준으로 연속된 값(차이가 얼마 나지 않는 부분을 찾아서 continue)
            //&& (nX - nStartValue < 100)
            vector<IPVM::Point_32f_C2> vecPtTempChecking;
            IPVM::LineEq_32f lineCheckingPos;
            long nCntCompareData = 3;

			for (int cnt = -2; cnt < nCntCompareData; cnt++)
            {
                IPVM::Point_32f_C2 ptTemp;
                ptTemp.m_x = cnt + 2;
				ptTemp.m_y = vecByAvg[nX + cnt];
                vecPtTempChecking.push_back(ptTemp);
			}
            
			if (IPVM::DataFitting::FitToLine(vecPtTempChecking.size(), &vecPtTempChecking[0], lineCheckingPos)
                != IPVM::Status::success)
            {
				// fittoline failed
            }

			float fSlope = abs(lineCheckingPos.m_a / lineCheckingPos.m_b);
            if (fSlope < 0.2)
            {
                nContinueCnt++;

				if (nContinueCnt > 50)
                {
                    nStartValue = nX;
                }
                continue;
            }
            
			//if (nX - nStartValue < 100)
                //continue;

            ptCheck.m_x = nX;
            ptCheck.m_y = imageSizeY / 2;
            vecPtCheckingPoint.push_back(ptCheck);
            nStartValue = nX;
		}
	}

    // Step #3. Find Point 기준으로 ROI 만들기 (좌/우 100 정도?)
    long nFindCheckPtCnt = vecPtCheckingPoint.size();

    if (nFindCheckPtCnt == 0)
    {
        AfxMessageBox(_T("Find Failed"));
        return;
    }

    std::vector<IPVM::Rect> vecFindDetectLineROi;
    vecFindDetectLineROi.resize(0);

    for (int nIdx = 0; nIdx < nFindCheckPtCnt; nIdx++)
    {
        long nLeft = vecPtCheckingPoint[nIdx].m_x - 500;
        long nRight = vecPtCheckingPoint[nIdx].m_x + 500;
        if (nLeft < 0)
        {
            nLeft = 0;
        }

        if (nRight > imageSizeX)
        {
            nRight = imageSizeX;
        }

        IPVM::Rect rtMakeRoi(nLeft, vecPtCheckingPoint[nIdx].m_y - 1000, nRight, vecPtCheckingPoint[nIdx].m_y + 1000);

        vecFindDetectLineROi.push_back(rtMakeRoi);
    }

    // Step #4. Find Rect를 Detect Line Rect 값에 넣기
    //inspection->m_para.m_nLineCountMeasure = vecFindDetectLineROi.size();
    ///inspection->m_para.m_vecrtDetectLineMeasure.resize(inspection->m_para.m_nLineCountMeasure);
    long nVecSize = vecFindDetectLineROi.size();
    if (nVecSize > 21)
        nVecSize = 21;

    for (long cnt = 0; cnt < nVecSize; cnt++)
    {
        inspection->m_para.m_vecrtDetectLineMeasure[cnt] = vecFindDetectLineROi[cnt];
    }
}



void CDlgInspectionMain::OnBnClickedButtonParaCellAlignVhomeROI(const int& nID)
{
	long imageSizeX = m_inspectionImageBright.GetSizeX();
	if (imageSizeX == 0)
	{
		return;
	}

	//auto* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	auto* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
    auto* measure
        = (CInspectionMeasure*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::measure);
    auto* chattering = (CInspectionChattering*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::Chattering);
	auto* abnormal = (CInspectionAbnormal*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::abnormalInspection);

	CXTPPropertyGridItemBool* showROI;
	
	if (nID == IDC_PARA_SHOW_ROI_MEASURE)
	{
		showROI = (CXTPPropertyGridItemBool*)m_inspectionParaGrid->FindItem(IDC_PARA_SHOW_ROI_MEASURE);
	}
	else if (nID == IDC_PARA_SHOW_ROI_BLOB)
	{
		showROI = (CXTPPropertyGridItemBool*)m_inspectionParaGrid->FindItem(IDC_PARA_SHOW_ROI_BLOB);
	}
	else if (nID == IDC_PARA_SHOW_ROI_MSA)
	{	
		showROI = (CXTPPropertyGridItemBool*)m_inspectionParaGrid->FindItem(IDC_PARA_SHOW_ROI_MSA);
	}
    else if (nID == IDC_PARA_CHATTERING_SET_USER_ROI)
    {
        showROI = (CXTPPropertyGridItemBool*)m_inspectionParaGrid->FindItem(IDC_PARA_CHATTERING_SET_USER_ROI);
    }
    else if (nID == IDC_PARA_ABNORMAL_SET_USER_ROI)
    {
        showROI = (CXTPPropertyGridItemBool*)m_inspectionParaGrid->FindItem(IDC_PARA_ABNORMAL_SET_USER_ROI);
	}
	else
	{
		return;
	}
	
	CString value = showROI->GetValue();
	
	m_imageView->ImageOverlayClear();

	if (value == _T("False"))
	{
		m_imageView->ROIClear();
		m_imageView->ROIHide();
	}
	else
	{
		m_imageView->ROIClear();

		CString strLine;

		if (nID == IDC_PARA_SHOW_ROI_MEASURE)
		{
			for (long cnt = 0; cnt < inspection->m_para.m_nLineCountMeasure; cnt++)
			{
				strLine.Format(_T("Measure Align Detect Line %02d"), cnt);

				m_imageView->ROISet(strLine, strLine, inspection->m_para.m_vecrtDetectLineMeasure[cnt], RGB(0, 255, 0), 100);
			}
		}
		else if (nID == IDC_PARA_SHOW_ROI_BLOB)
		{
			for (long cnt = 0; cnt < inspection->m_para.m_nLineCountBlob; cnt++)
			{
				strLine.Format(_T("Blob Align Detect Line %02d"), cnt);

				m_imageView->ROISet(strLine, strLine, inspection->m_para.m_vecrtDetectLineBlob[cnt], RGB(0, 0, 255), 100);
			}
		}
		else if (nID == IDC_PARA_SHOW_ROI_MSA)
		{
            if (measure->m_para.m_bUseMSAAlign == TRUE)
            {
                for (long cnt = 0; cnt < 4; cnt++)
                {
                    strLine.Format(_T("MSA %02d"), cnt);

                    m_imageView->ROISet(
                        strLine, strLine, measure->m_para.m_vecrtDetectLineMSA[cnt], RGB(0, 0, 255), 100);
                }
			}
            else
            {
                for (long cnt = 0; cnt < 2; cnt++)
                {
                    strLine.Format(_T("MSA %02d"), cnt);

                    m_imageView->ROISet(
                        strLine, strLine, measure->m_para.m_vecrtDetectLineMSA[cnt], RGB(0, 0, 255), 100);
                }
			}
		}
        else if (nID == IDC_PARA_CHATTERING_SET_USER_ROI)
        {
            strLine.Format(_T("Chattering ROI"));
            m_imageView->ROISet(strLine, strLine, chattering->m_para.m_rtSetInspArea, RGB(0, 0, 255), 100);

        }
        else if (nID == IDC_PARA_ABNORMAL_SET_USER_ROI)
        {
            //strLine.Format(_T("Abnormal ROI"));
            //m_imageView->ROISet(strLine, strLine, abnormal->m_para.m_rtSetInspArea, RGB(0, 0, 255), 100);

			if (inspection->m_para.m_nLineCountMeasure >= 6)
			{
                strLine.Format(_T("Abnormal ROI 1"));
                m_imageView->ROISet(strLine, strLine, abnormal->m_para.m_rtSetInspArea, RGB(0, 0, 255), 100);

                strLine.Format(_T("Abnormal ROI 2"));
				m_imageView->ROISet(strLine, strLine, abnormal->m_para.m_rtSetInspArea_2, RGB(0, 0, 255), 100);
			}
            else if (inspection->m_para.m_nLineCountMeasure >= 4)
            {
                strLine.Format(_T("Abnormal ROI 1"));
                m_imageView->ROISet(strLine, strLine, abnormal->m_para.m_rtSetInspArea, RGB(0, 0, 255), 100);
			}
        }
		m_imageView->ROIShow();
	}
	m_imageView->ImageOverlayShow();
}

void CDlgInspectionMain::OnBnClickedSwapMinAreaBoxXY()
{
}

void CDlgInspectionMain::OnChangedCellAlignImageUseIndex()
{
	auto* inspection = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	if (inspection->m_para.m_imageUseIndex == 0)
	{
		m_imageView->SetImage(m_inspectionImageBright);
	}
	else
	{
		m_imageView->SetImage(m_inspectionImageDark);
	}

}

void CDlgInspectionMain::UpdateInspectionParaGridSurface()
{
	return;
}

void CDlgInspectionMain::OnChangedTabFoldingThreshold()
{
	m_imageView->ImageOverlayShow();
}

void CDlgInspectionMain::UpdateInspectionParaGridMeasure()
{
	//Measure
	CString strSaveMeasureResult;
	CString strPitchCalcMethod;
	CString strPitchCalcHome;
	CString strPitchCalcTab;
	CString strPitchCalcTabSide;
	CString strCellLengthCalcPosition;
	CString strDetailCellLengthCalc;
	CString strCuttingHeightTopDarkLowerThreshold;
	CString strCuttingHeightBotDarkLowerThreshold;
	CString	strDetectCuttingHeightContinuousPixelCount;
	CString	strFoundTopCuttingHeightStartPoint;
	CString	strFoundBotCuttingHeightStartPoint;
	CString	strFoundTopCuttingHeightAreaPixelCount;
	CString	strFoundBotCuttingHeightAreaPixelCount;
	CString strTabSideCalcPos;
	CString strUseMismatch;
	CString strUseCellPitch;
	CString strUseCellLength;
	CString strUseTabWidth;
	CString strUseTabHeight;
	CString strUseMisalign;
	CString strUseVhomeTiltTop;
	CString strUseVhomeTiltBot;
	CString strUseTabSide;
	CString strUseCuttingHeightTop;
	CString strUseCuttingHeightBot;

	if (m_nLanguage == 3)
	{
		strSaveMeasureResult = _T("Use Inspection");
		strPitchCalcMethod = _T("Pitch Calc Method");
		strPitchCalcHome = _T("Pitch Using V Home");
		strPitchCalcTab = _T("Pitch Using Tab Width");
		strPitchCalcTabSide = _T("Pitch Using Tab Width Spec + Both Tab Side");
		strCellLengthCalcPosition = _T("Inspection Range(1 Cycle) [mm]");
		strDetailCellLengthCalc = _T("Detail Cell Length Calc");
		strCuttingHeightTopDarkLowerThreshold = _T("Cutting Height Top Dark Threshold");
		strCuttingHeightBotDarkLowerThreshold = _T("Cutting Height Bot Dark Threshold");
		strDetectCuttingHeightContinuousPixelCount = _T("Cutting Height Continuous Pixel Count");
		strFoundTopCuttingHeightStartPoint = _T("Top Cutting Height Detection Start Point");
		strFoundBotCuttingHeightStartPoint = _T("Bot Cutting Height Detection Start Point");
		strFoundTopCuttingHeightAreaPixelCount = _T("Top Cutting Height Detection Area");
		strFoundBotCuttingHeightAreaPixelCount = _T("Bot Cutting Height Detection Area");
		strTabSideCalcPos = _T("Tab Side Calc Pos");
		strUseMismatch = _T("Use Mismatch");
		strUseCellPitch = _T("Use Cell Pitch");
		strUseCellLength = _T("Use Cell Length");
		strUseTabWidth = _T("Use Tab Width");
		strUseTabHeight = _T("Use Tab Height");
		strUseMisalign = _T("Use Misalign");
		strUseVhomeTiltTop = _T("Use V Home Tilt Top");
		strUseVhomeTiltBot = _T("Use V Home Tilt Bot");
		strUseTabSide = _T("Use Tab Side");
		strUseCuttingHeightTop = _T("Use Cutting Height Top");
		strUseCuttingHeightBot = _T("Use Cutting Height Bot");
	}
	else
	{
		strSaveMeasureResult = _T("Use Inspection");
		strPitchCalcMethod = _T("Pitch Calc Method");
		strPitchCalcHome = _T("Pitch Using V Home");
		strPitchCalcTab = _T("Pitch Using Tab Width");
		strPitchCalcTabSide = _T("Pitch Using Tab Width Spec + Both Tab Side");
		strCellLengthCalcPosition = _T("Inspection Range(1 Cycle) [mm]");
		strDetailCellLengthCalc = _T("Detail Cell Length Calc");
		strCuttingHeightTopDarkLowerThreshold = _T("Cutting Height Top Dark Threshold");
		strCuttingHeightBotDarkLowerThreshold = _T("Cutting Height Bot Dark Threshold");
		strDetectCuttingHeightContinuousPixelCount = _T("Cutting Height Continuous Pixel Count");
		strFoundTopCuttingHeightStartPoint = _T("Top Cutting Height Detection Start Point");
		strFoundBotCuttingHeightStartPoint = _T("Bot Cutting Height Detection Start Point");
		strFoundTopCuttingHeightAreaPixelCount = _T("Top Cutting Height Detection Area");
		strFoundBotCuttingHeightAreaPixelCount = _T("Bot Cutting Height Detection Area");
		strTabSideCalcPos = _T("Tab Side Calc Pos");
		strUseMismatch = _T("Use Mismatch");
		strUseCellPitch = _T("Use Cell Pitch");
		strUseCellLength = _T("Use Cell Length");
		strUseTabWidth = _T("Use Tab Width");
		strUseTabHeight = _T("Use Tab Height");
		strUseMisalign = _T("Use Misalign");
		strUseVhomeTiltTop = _T("Use V Home Tilt Top");
		strUseVhomeTiltBot = _T("Use V Home Tilt Bot");
		strUseTabSide = _T("Use Tab Side");
		strUseCuttingHeightTop = _T("Use Cutting Height Top");
		strUseCuttingHeightBot = _T("Use Cutting Height Bot");
	}

	auto* inspection = (CInspectionMeasure*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	//auto* shoulder = (CInspectionShoulderLine*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::shoulder);
	m_imageView->ROIClear();

	m_inspectionParaGrid->ResetContent();

	CString strGeneral;

	if (m_nLanguage == 3)
		strGeneral = _T("其他事项");
	else
		strGeneral = _T("General");

	if (auto* category = m_inspectionParaGrid->AddCategory(strGeneral))
	{
		if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, strSaveMeasureResult, inspection->m_para.m_bSaveMeasureResult))
		{
			item->SetID(IDC_PARA_SAVE_MEASURE_RESULT);
			SetExplain(item, IDC_PARA_SAVE_MEASURE_RESULT);
			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}

		if (auto* item = PropertyGridHelperEx::AddLink_Enum(
                category, _T("Select ImageType (Bright/Dark)"), inspection->m_para.m_nImageDark))
        {
            item->GetConstraints()->AddConstraint(_T("Bright Image"), 0);
            item->GetConstraints()->AddConstraint(_T("Dark Image"), 1);

            item->SetID(IDC_PARA_MEASURE_SELECT_IMAGE_TYPE);
        }

		if (auto* item = PropertyGridHelper::AddLink(category, strCellLengthCalcPosition, _T("%.2f"), inspection->m_para.m_fCellLengthCalcPosition))
		{
			item->SetID(IDC_PARA_CELL_LENGTH_TILT_CALC_POSITION);
			SetExplain(item, IDC_PARA_CELL_LENGTH_TILT_CALC_POSITION);
			/*if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();*/
		}

		if (auto* item = PropertyGridHelper::AddLink(category, _T("Measure Item Count"), inspection->m_para.m_nCountMeasureItem))
		{
			item->SetID(IDC_PARA_MEASURE_ITEM_COUNT);
			SetExplain(item, IDC_PARA_MEASURE_ITEM_COUNT);
			item->SetDescription(_T("Measure Item Count"));
		}
        auto* itemUseMSA = PropertyGridHelperEx::AddLink_Bool(category, _T("Use MSA"), inspection->m_para.m_bUseMSA);
		//if (auto* itemUseMSA = PropertyGridHelperEx::AddLink_Bool(category, _T("Use MSA"), inspection->m_para.m_bUseMSA))
  //      {
  //      }

		if (auto* showMSAROI = itemUseMSA->AddChildItem(new CXTPPropertyGridItemBool(_T("MSA ROI"))))
		{
			showMSAROI->SetID(IDC_PARA_SHOW_ROI_MSA);

			SetExplain(showMSAROI, IDC_PARA_SHOW_ROI_MSA);
			showMSAROI->SetValue(_T("False"));

			OnBnClickedButtonParaCellAlignVhomeROI(IDC_PARA_SHOW_ROI_MSA);

			if (inspection->m_para.m_bUseMSA == 0)
                showMSAROI->SetReadOnly();
		}

		if (auto* item = PropertyGridHelper::AddLink_Enum(itemUseMSA, _T("MSA Line To Point Distance"), inspection->m_para.m_nMSADistance))
		{
			item->GetConstraints()->AddConstraint(_T("None"), 0);
			item->GetConstraints()->AddConstraint(_T("Start"), 1);
			item->GetConstraints()->AddConstraint(_T("End"), 2);

			if (inspection->m_para.m_bUseMSA == 0)
                item->SetReadOnly();
		}

        if (auto* item
            = PropertyGridHelperEx::AddLink_Bool(itemUseMSA, _T("Use MSA Align"), inspection->m_para.m_bUseMSAAlign))
        {
        }

        CString strLine = _T("MSA Align");
        if (auto* item = PropertyGridHelper::AddLink_Button(itemUseMSA, strLine, TRUE, FALSE))
        {
            item->SetID(IDC_PARA_BUTTON_MSA_ALIGN);
        }

		itemUseMSA->Expand();

		category->Expand();
	}

	auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
	
	long nCountLine = cellAlign->m_para.m_nLineCountMeasure.GetValue();

	CString strMeasureOrder;
    strMeasureOrder = _T("Measure Order");

	CString strType, strStart, strEnd;
	CString strSpecStandard, strSpecMin, strSpecMax, strSpecMin_C, strSpecMax_C;
    for (long measure = 0; measure < COUNT_CELL_MEASURE; measure++)
    {
        CString strMeasureItemCategory;
        strMeasureItemCategory.Format(_T("Measure Item %d"), measure + 1);
        if (auto* category = m_inspectionParaGrid->AddCategory(strMeasureItemCategory))
        {
            strType.Format(_T("%d Type"), measure + 1);
            strStart.Format(_T("%d Start Boundary"), measure + 1);
            strEnd.Format(_T("%d End Boundary"), measure + 1);

			strSpecStandard.Format(_T("%d Spec Standard"), measure + 1);
			strSpecMax.Format(_T("%d USL"), measure + 1);
			strSpecMin.Format(_T("%d LSL"), measure + 1);

			strSpecMax_C.Format(_T("%d UCL"), measure + 1);
			strSpecMin_C.Format(_T("%d LCL"), measure + 1);

			if (auto* item = PropertyGridHelper::AddLink_Enum(category, strType, inspection->m_para.m_vecMeasureType[measure]))
            {
                for (long measure2 = (long)DefectType::START_Coater_Dimension + 1; measure2 < (long)DefectType::End_Coater_Dimension; measure2++)
                {
					item->GetConstraints()->AddConstraint(GetDefectName((DefectType)measure2), measure2 - (long)DefectType::START_Coater_Dimension - 1);
                }
                item->SetID(IDC_PARA_MEASURE_ITEM_TYPE + measure);
                item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink_Enum(category, strStart, inspection->m_para.m_vecMeasureStart[measure]))
            {
				long constraint = 0;
				for (constraint = 0; constraint < nCountLine; constraint++)
				{
					CString strRoi;
					strRoi.Format(_T("%d"), constraint + 1);
					item->GetConstraints()->AddConstraint(strRoi, constraint);
				}
				item->GetConstraints()->AddConstraint(_T("none"), constraint++);
				item->GetConstraints()->AddConstraint(_T("Half Point"), constraint++);

                item->SetID(IDC_PARA_MEASURE_ITEM_START + measure);
                item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink_Enum(category, strEnd, inspection->m_para.m_vecMeasureEnd[measure]))
            {
				long constraint = 0;
				for (constraint = 0; constraint < nCountLine; constraint++)
				{
					CString strRoi;
					strRoi.Format(_T("%d"), constraint + 1);
					item->GetConstraints()->AddConstraint(strRoi, constraint);
				}
				item->GetConstraints()->AddConstraint(_T("none"), constraint++);
				item->GetConstraints()->AddConstraint(_T("Half Point"), constraint++);

                item->SetID(IDC_PARA_MEASURE_ITEM_END + measure);
                item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
            }

			if (auto* item = PropertyGridHelper::AddLink(category, strSpecStandard, _T("%.1f"), inspection->m_para.m_vecMeasureStandard[measure]))
			{
				item->SetID(IDC_PARA_MEASURE_ITEM_STANDARD + measure);
				item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
			}
			
			if (auto* item = PropertyGridHelper::AddLink(category, strSpecMax_C, _T("%.1f"), inspection->m_para.m_vecMeasureOffsetUCL[measure]))
			{
				item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETUCL + measure);
				item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
			}
			if (auto* item = PropertyGridHelper::AddLink(category, strSpecMin_C, _T("%.1f"), inspection->m_para.m_vecMeasureOffsetLCL[measure]))
			{
				item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETLCL + measure);
				item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
			}
            if (auto* item = PropertyGridHelper::AddLink(category, strSpecMax, _T("%.1f"), inspection->m_para.m_vecMeasureOffsetUSL[measure]))
            {
                item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETUSL + measure);
                item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink(category, strSpecMin, _T("%.1f"), inspection->m_para.m_vecMeasureOffsetLSL[measure]))
            {
                item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETLSL + measure);
                item->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
            }
			category->SetHidden(!(inspection->m_para.m_nCountMeasureItem > measure));
			category->Expand();
		}
	}
}

void CDlgInspectionMain::OnBnClickedButtonParaMeasureImageUseIndex()
{
	auto* inspection = (CInspectionMeasure*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	if (inspection->m_para.m_imageUseIndex == 0)
	{
		m_imageView->SetImage(m_inspectionImageBright);
	}
	else
	{
		m_imageView->SetImage(m_inspectionImageDark);
	}
}

CXTPPropertyGridItem* CDlgInspectionMain::PropertyAdd_AndOr(CXTPPropertyGridItem* parent, LanguageDef def, bool& value)
{
	if (auto* item = PropertyGridHelperEx::AddLink_Bool(parent, def, value))
	{
		item->SetTrueFalseText(_T("AND"), _T("OR"));
		return item;
	}

	return NULL;
}

bool CDlgInspectionMain::SetExplain(CXTPPropertyGridItem* item, long nID)
{
	if (m_nLanguage == 1)
	{
		if (m_mapExplainParam.find(nID) != m_mapExplainParam.end())
			item->SetDescription(m_mapExplainParam.find(nID)->second[0]);
	}
	else if (m_nLanguage == 2 || m_nLanguage == 4)
	{
		if (m_mapExplainParam.find(nID) != m_mapExplainParam.end())
			item->SetDescription(m_mapExplainParam.find(nID)->second[1]);
	}
	else if (m_nLanguage == 3)
	{
		if (m_mapExplainParam.find(nID) != m_mapExplainParam.end())
			item->SetDescription(m_mapExplainParam.find(nID)->second[2]);
	}
	return true;
}

void CDlgInspectionMain::UpdateInspectionParaGridBlob()
{
	auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

	m_imageView->ROIClear();
	m_inspectionParaGrid->ResetContent();
    if (auto* category = m_inspectionParaGrid->AddCategory(_T("General")))
    {
        if (auto* item = PropertyGridHelper::AddLink_Bool(category, _T("Use Min AreaBox X/Y"), inspection->m_para.m_bUseSwapXY))
        {
            item->SetID(IDC_PARA_SWAP_MIN_AREA_BOX_XY);
        }
        if (auto* item = PropertyGridHelper::AddLink_Bool(category, _T("Use Delete Intersect Blob"), inspection->m_para.m_bUseDeleteIntersectBlob))
        {
            item->SetID(IDC_PARA_BLOB_DELETE_INTERSECT_BLOB_000);
            item->SetDescription(_T("True - 영역 내 중복 ROI 제거 기능 사용 \nFalse - 영역 내 중복 ROI 기능 미사용"));
        }
        category->Expand(); 
    }

	if (auto* category = m_inspectionParaGrid->AddCategory(_T("Inspection")))
	{
		category->AddChildItem(new XTPPropertyGridItemButton(_T("Add Insp Blob")));
		category->SetHeight(2 * category->GetHeight());
		category->SetID(IDC_BTN_BLOB_ADD_INSP_AREA);

		long nTotalInspCount = inspection->m_para.m_vecBlobParaCondition.size();
		for (long nInsp = 0; nInsp < nTotalInspCount; nInsp++)
		{
			CString strNameInsp;
			strNameInsp.Format(_T("Insp Blob %03d"), nInsp);
			if (auto* subcategory = m_inspectionParaGrid->AddCategory(strNameInsp))
			{
				if (auto* item = PropertyGridHelper::AddLink_Bool(subcategory, _T("Use Inspection"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_bUseInspection))
				{
					item->SetID(IDC_PARA_BLOB_USE_INSPECTION_000 + nInsp);
                    item->SetDescription(_T("True - 해당 Insp Blob 사용 \nFalse - 해당 Insp Blob 미사용"));

				}
				if (auto* item = PropertyGridHelper::AddLink_Threshold(subcategory, _T("Blob Threshold White"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nThreshold))
				{
					item->SetID(IDC_PARA_BLOB_THRESHOLD_000 + nInsp);
                    item->SetDescription(_T("White 불량 0 ~ 255 설정"));

				}
				/*if (inspection->m_para.m_vecBlobParaCondition[nInsp].m_nBlobBrightType == (int)ENUM_BLOB_TYPE_THBRIGHT::Mix)
				{*/
				if (auto* item = PropertyGridHelper::AddLink_Threshold(subcategory, _T("Blob Threshold Black"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nThreshold2)) 
				{
					item->SetID(IDC_PARA_BLOB_THRESHOLD2_000 + nInsp);
                    item->SetDescription(_T("Black 불량 0 ~ 255 설정"));
						
				}
                if (auto* item = PropertyGridHelper::AddLink_Bool(subcategory, _T("Use Mean Range Threshold"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_bUseMeanRangeTheshold))
                {
                    item->SetID(IDC_PARA_BLOB_USE_MEAN_RANGE_THRESHOLD_000 + nInsp);
                    item->SetDescription(_T("True - 10 ~ 245 범위 내 중간 밝기값 설정 \nFalse - 전체 범위 중간 밝기값 구한 후 설정"));
                }
				//}
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Blob Min Pixel Size"), _T("%.2f"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_fBlobMinSize_mm))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_MIN_SIZE_000 + nInsp);
                    item->SetDescription(_T("최소 Blob의 Pixel Size 설정"));
				}
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("MassCenterGV Size"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nMassCenterGVsize))
				{
					item->SetID(IDC_PARA_BLOB_MCGVSIZE_000 + nInsp);
                    item->SetDescription(_T("Mass Center 기준으로 GV 계산할 Pixel Size 설정"));
				}
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Threshold Hor Div"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nThresholdHorDiv))
				{
					item->SetID(IDC_PARA_BLOB_THRESHOL_HOR_DIV_000 + nInsp);
                    item->SetDescription(_T("Insp Area를 수평 방향으로 설정 개수만큼 분할"));
				}
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Threshold Ver Div"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nThresholdVerDiv))
				{
					item->SetID(IDC_PARA_BLOB_THRESHOL_VER_DIV_000 + nInsp);
                    item->SetDescription(_T("Insp Area를 수직 방향으로 설정 개수만큼 분할"));
				}
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Blob Merge Distance"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nBlobMergeDistance))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_MERGE_DISTANCE_000 + nInsp);
                    item->SetDescription(_T("인접 Blob Merge 거리 설정"));
				}
                if (auto* item = PropertyGridHelper::AddLink_Bool(subcategory, _T("Blob Intersect Merge"),inspection->m_para.m_vecBlobParaCondition[nInsp].m_bUseIntersectMerge))
                {
                    item->SetID(IDC_PARA_BLOB_BLOB_INTERSECT_MERGE_000 + nInsp);
                    item->SetDescription(_T("True - 검출된 Blob 영역이 겹치지 않게 설정 \nFalse - 검출된 Blob 영역이 겹치도록 설정"));
                }
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Blob Max Count"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nBlobMaxCount))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_MAX_COUNT_000 + nInsp);
                    item->SetDescription(_T("최대 Blob 검출 개수 설정"));
				}
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Blob Timeout[ms]"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nTimeOut_ms))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_TIMEOUT_000 + nInsp);
                    item->SetDescription(_T("Blob 검사 TimeOut 설정"));
				}
				if (auto* item = PropertyGridHelper::AddLink(subcategory, _T("Blob Min Length[mm]"), _T("%.3f"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_fBlobMinLength_mm))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_MIN_LENGTH_000 + nInsp);
                    item->SetDescription(_T("검출 Blob의 최소 길이 설정"));
				}
				if (auto* item = PropertyGridHelper::AddLink_Enum(subcategory, _T("Source Image Type"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nImageType))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_TYPE_IMAGE_000 + nInsp);
					item->GetConstraints()->AddConstraint(_T("Brightness"), (int)ENUM_BLOB_TYPE_SRCIMAGE::Brightness);
					item->GetConstraints()->AddConstraint(_T("Darkness"), (int)ENUM_BLOB_TYPE_SRCIMAGE::Darkness);
                    item->GetConstraints()->AddConstraint(_T("MAX"), (int)ENUM_BLOB_TYPE_SRCIMAGE::MAX);
                    item->SetDescription(_T("Blob 검사를 진행할 영상 선택(Bright/Dark)"));
				}
				if (auto* item = PropertyGridHelper::AddLink_Enum(subcategory, _T("Threshold Type"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nBlobBrightType))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_TYPE_THRESHOLD_BRIGHT_000 + nInsp);
					item->GetConstraints()->AddConstraint(_T("White"), (int)ENUM_BLOB_TYPE_THBRIGHT::White);
					item->GetConstraints()->AddConstraint(_T("Black"), (int)ENUM_BLOB_TYPE_THBRIGHT::Black);
					item->GetConstraints()->AddConstraint(_T("Mix"), (int)ENUM_BLOB_TYPE_THBRIGHT::Mix);
                    item->SetDescription(_T("Blob 검출 Threshold Type 설정(White/Black/Mix)"));
				}
				if (auto* item = PropertyGridHelper::AddLink_Enum(subcategory, _T("Inspection Type"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nInspectionType))
				{
					item->SetID(IDC_PARA_BLOB_BLOB_TYPE_INSPECTION_000 + nInsp);
					item->GetConstraints()->AddConstraint(_T("Defect #1"), (int)ENUM_BLOB_TYPE_INSPTYPE::Defect1);
					item->GetConstraints()->AddConstraint(_T("Defect #2"), (int)ENUM_BLOB_TYPE_INSPTYPE::Defect2);
					item->GetConstraints()->AddConstraint(_T("Defect #3"), (int)ENUM_BLOB_TYPE_INSPTYPE::Defect3);
					item->GetConstraints()->AddConstraint(_T("Defect #4"), (int)ENUM_BLOB_TYPE_INSPTYPE::Defect4);
                    item->SetDescription(_T("Defect Type 설정"));
				}

				// equal
				auto* itemUseEqual = PropertyGridHelper::AddLink_Bool(subcategory, _T("Use Equalization"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_bUseEqual);
                if (itemUseEqual)
                {
					itemUseEqual->SetID(IDC_PARA_BLOB_USE_EQUAL_000 + nInsp);
                    itemUseEqual->SetDescription(_T("True - 노이즈 보정(전처리) 기능 On \nFalse - 노이즈 보정(전처리) 기능 Off"));
				}
				/*auto* itemUseEqualThreshold = PropertyGridHelper::AddLink_Bool(itemUseEqual, _T("Use_Equalization_Threshold"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_bUseEqualThreshold);
                if (itemUseEqualThreshold)
                {
					itemUseEqualThreshold->SetID(IDC_PARA_BLOB_USE_EQUAL_THRESHOLD_000 + nInsp);
					if (itemUseEqual->GetValue() == _T("False"))
                        itemUseEqualThreshold->SetReadOnly();
                }*/
                if (auto* item = PropertyGridHelper::AddLink(itemUseEqual, _T("Equal_Threshold_Max"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nEqualThresholdMax))
                {
					item->SetID(IDC_PARA_BLOB_EQUAL_THRESHOLD_MAX_000 + nInsp);
                    //if (itemUseEqual->GetValue() == _T("False") || itemUseEqualThreshold->GetValue() == _T("False"))
                    if (itemUseEqual->GetValue() == _T("False"))
                        item->SetReadOnly();
                }
                if (auto* item = PropertyGridHelper::AddLink(itemUseEqual, _T("Equal_Threshold_Min"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nEqualThresholdMin))
                {
					item->SetID(IDC_PARA_BLOB_EQUAL_THRESHOLD_MIN_000 + nInsp);
                    //if (itemUseEqual->GetValue() == _T("False") || itemUseEqualThreshold->GetValue() == _T("False"))
                    if (itemUseEqual->GetValue() == _T("False"))
                        item->SetReadOnly();
                }
                if (auto* item = PropertyGridHelper::AddLink(itemUseEqual, _T("Thread Divide Count"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nEqualThreadCount))
                {
                    item->SetID(IDC_PARA_BLOB_EQUAL_THREAD_COUNT_000 + nInsp);
                    //if (itemUseEqual->GetValue() == _T("False") || itemUseEqualThreshold->GetValue() == _T("False"))
                    if (itemUseEqual->GetValue() == _T("False"))
                        item->SetReadOnly();
                }

                // end equal

				if (auto* item = PropertyGridHelper::AddLink_Enum(subcategory, _T("Select Origin Area"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nInspectionArea))
				{
					item->SetID(IDC_PARA_BLOB_INSP_AREA_ROI_000 + nInsp);
                    item->SetDescription(_T("Cell Align에서 나온 결과를 기준으로 검사 영역 설정"));
					CString strTitle;
					long nItemCount = 6;
					for (int idx = 0; idx < nItemCount; idx++)
					{
						strTitle.Format(_T("Insp Area #%d"), idx);
						item->GetConstraints()->AddConstraint(strTitle, idx);
					}
				}
				if (auto* item = PropertyGridHelper::AddLink_Enum(subcategory, _T("Adjust Roi Set Type"), inspection->m_para.m_vecBlobParaCondition[nInsp].m_nSetTypeInspArea))
				{
					item->SetID(IDC_PARA_BLOB_TYPE_SET_INSP_ROI_000 + nInsp);
					item->GetConstraints()->AddConstraint(_T("Offset Roi"), (int)ENUM_BLOB_TYPE_SET_INSPAREA::OffsetRoi);
					item->GetConstraints()->AddConstraint(_T("From Left"), (int)ENUM_BLOB_TYPE_SET_INSPAREA::From_Left);
					item->GetConstraints()->AddConstraint(_T("From Right"), (int)ENUM_BLOB_TYPE_SET_INSPAREA::From_Right);
                    item->SetDescription(_T("Insp Area의 ROI를 조정하는 기능"));
				}
				if (auto* item = PropertyGridHelperEx::AddLink_Rect(subcategory, _T("  Adjust Blob Offset[+:Deflate]"), IPVM::ToMFC(inspection->m_para.m_vecBlobParaCondition[nInsp].m_rtOffsetRoi)))
				{
					item->SetID(IDC_PARA_BLOB_OFFSET_ROI_000 + nInsp);
                    item->SetDescription(_T("Adjust Roi Set Type에 적용 될 Offset 값 설정"));
				}
				if (auto* item = subcategory->AddChildItem(new XTPPropertyGridItemButton(_T("Delete Item"))))
				{
					item->SetID(IDC_BTN_BLOB_DELETE_INSP_AREA);
				}
				subcategory->Expand();
			}
			category->Expand();
		}
	}
}

//void CDlgInspectionMain::OnChangedBlobRoi(LPARAM lparam, int nID)
//{
//	auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
//	auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
//
//	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
//		return;
//	
//	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
//	if (pItem == nullptr)
//		return;
//
//	int id = pItem->GetID();
//
//	if (id == 0)
//	{
//		if(pItem->GetParentItem() != NULL)
//			id = pItem->GetParentItem()->GetID();
//	}
//
//	int condition = (id - nID) / COUNT_CELL_ROI;
//	int roi = (id - nID) % COUNT_CELL_ROI;
//	
//	CString strRectValue;
//	CString strLeft, strTop, strRight, strBottom;
//	IPVM::Rect rect;
//
//	CString strItemCaption = pItem->GetCaption();
//	if (strItemCaption.Find(_T("Adjust Blob Offset")) > 0)
//	{
//		strRectValue = pItem->GetValue();
//	}
//	else
//	{
//		if (pItem->GetParentItem() != NULL)
//		{
//			strRectValue = pItem->GetParentItem()->GetValue();
//		}
//		else
//		{
//			strRectValue = pItem->GetValue();
//		}
//	}
//	
//	// refresh roi information
//	if (nID == IDC_PARA_BLOB_TYPE_SET_INSP_ROI_000 
//		|| nID == IDC_PARA_BLOB_INSP_AREA_ROI_000
//		|| nID == IDC_PARA_BLOB_OFFSET_ROI_000 )
//	{
//		long index = inspection->m_para.m_vecBlobParaCondition[roi].m_nInspectionArea;
//		rect = cellAlign->m_unionResult.m_rectAreaBlob[index];
//
//		switch (inspection->m_para.m_vecBlobParaCondition[roi].m_nSetTypeInspArea)
//		{
//		case (long)ENUM_BLOB_TYPE_SET_INSPAREA::OffsetRoi:
//		{
//		}
//		break;
//		case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Left:
//		{
//			rect.m_right = rect.m_left;
//		}
//		break;
//		case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Right:
//		{
//			rect.m_left = rect.m_right;
//		}
//		break;
//		}
//	}
//
//	AfxExtractSubString(strLeft, strRectValue, 0, ';');
//	AfxExtractSubString(strTop, strRectValue, 1, ';');
//	AfxExtractSubString(strRight, strRectValue, 2, ';');
//	AfxExtractSubString(strBottom, strRectValue, 3, ';');
//
//	IPVM::Rect offset(_wtoi(strLeft), _wtoi(strTop), _wtoi(strRight), _wtoi(strBottom));
//	inspection->m_para.m_vecBlobParaCondition[roi].m_rtOffsetRoi = offset;
//	rect.DeflateRect(offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);
//
//	inspection->m_para.m_vecBlobParaCondition[roi].m_rtSetInspArea = rect;
//	
//	//show roi
//	{
//		m_imageView->ImageOverlayClear();
//		//debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);
//		m_imageView->ImageOverlayAdd(rect, RGB(0, 255, 0), 1);
//		m_imageView->ImageOverlayShow();
//	}
//}

void CDlgInspectionMain::OnChangedBlobRoi(LPARAM lparam, int nID)
{
	auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	int id = pItem->GetID();
	if (id == 0)	// ID 지정하지 않은 하위 Category
	{
		pItem = (CXTPPropertyGridItem*)pItem->GetParentItem();
		if (pItem == nullptr)
			return;
	}
	
	id = pItem->GetID();
	int condition = (id - nID) / COUNT_CELL_ROI;
	int roi = (id - nID) % COUNT_CELL_ROI;

	IPVM::Rect rect;
	
	// refresh roi information
	if (nID == IDC_PARA_BLOB_TYPE_SET_INSP_ROI_000
		|| nID == IDC_PARA_BLOB_INSP_AREA_ROI_000
		|| nID == IDC_PARA_BLOB_OFFSET_ROI_000)
	{
		long index = inspection->m_para.m_vecBlobParaCondition[roi].m_nInspectionArea;
		rect = cellAlign->m_unionResult.m_rectAreaBlob[index];

		switch (inspection->m_para.m_vecBlobParaCondition[roi].m_nSetTypeInspArea)
		{
		case (long)ENUM_BLOB_TYPE_SET_INSPAREA::OffsetRoi:
		{
		}
		break;
		case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Left:
		{
			rect.m_right = rect.m_left;
		}
		break;
		case (long)ENUM_BLOB_TYPE_SET_INSPAREA::From_Right:
		{
			rect.m_left = rect.m_right;
		}
		break;
		}

		CString strRectValue;
		CString strLeft, strTop, strRight, strBottom;
		if (nID == IDC_PARA_BLOB_OFFSET_ROI_000)
		{
			strRectValue = pItem->GetValue();
		}
		else
		{
			IPVM::Rect offset = inspection->m_para.m_vecBlobParaCondition[roi].m_rtOffsetRoi;
            strRectValue.Format(_T("%d;%d;%d;%d"), offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);
		}

		AfxExtractSubString(strLeft, strRectValue, 0, ';');
		AfxExtractSubString(strTop, strRectValue, 1, ';');
		AfxExtractSubString(strRight, strRectValue, 2, ';');
		AfxExtractSubString(strBottom, strRectValue, 3, ';');

		IPVM::Rect offset(_wtoi(strLeft), _wtoi(strTop), _wtoi(strRight), _wtoi(strBottom));
		inspection->m_para.m_vecBlobParaCondition[roi].m_rtOffsetRoi = offset;
	}

	IPVM::Rect offset = inspection->m_para.m_vecBlobParaCondition[roi].m_rtOffsetRoi;
	rect.DeflateRect(offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);

	inspection->m_para.m_vecBlobParaCondition[roi].m_rtSetInspArea = rect;

	//show roi
	{
		m_imageView->ImageOverlayClear();
		//debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);
		m_imageView->ImageOverlayAdd(rect, RGB(0, 255, 0), 1);
		m_imageView->ImageOverlayShow();
	}
}
void CDlgInspectionMain::OnChangedChatteringlRoi(int nID)
{
    auto* chattering = (CInspectionChattering*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
    auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);
    

	IPVM::Rect rect;
    rect = cellAlign->m_unionResult.m_rectAreaMeasure[1];

    CString strRectValue;
    CString strLeft, strTop, strRight, strBottom;

	if (nID == IDC_PARA_CHATTERING_SET_ALIGN_ROI)
    {
        strRectValue = m_inspectionParaGrid->FindItem(IDC_PARA_CHATTERING_SET_ALIGN_ROI)->GetValue();
    }

    AfxExtractSubString(strLeft, strRectValue, 0, ';');
    AfxExtractSubString(strTop, strRectValue, 1, ';');
    AfxExtractSubString(strRight, strRectValue, 2, ';');
    AfxExtractSubString(strBottom, strRectValue, 3, ';');

	IPVM::Rect offset(_wtoi(strLeft), _wtoi(strTop), _wtoi(strRight), _wtoi(strBottom));
    chattering->m_para.m_rtOffsetRoi = offset;

    rect.DeflateRect(offset.m_left, offset.m_top, offset.m_right, offset.m_bottom);

    chattering->m_para.m_rtSetInspArea = rect;

	//show roi
    {
        m_imageView->ImageOverlayClear();
        m_imageView->ROIClear();
        //debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);
        m_imageView->ImageOverlayAdd(rect, RGB(0, 255, 0), 1);
        m_imageView->ImageOverlayShow();
    }

}
void CDlgInspectionMain::OnChangedAbnormalRoi(LPARAM lparam, int nID)
{
	//auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	//auto* abnormalInspection = (CInspectionAbnormal*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	//


	//if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
	//	return;

	//CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	//if (pItem == nullptr)
	//	return;

	//int id = pItem->GetID();
	//if (id == 0)	// ID 지정하지 않은 하위 Category
	//{
	//	pItem = (CXTPPropertyGridItem*)pItem->GetParentItem();
	//	if (pItem == nullptr)
	//		return;
	//}

	////show roi
	//{
	//	m_imageView->ImageOverlayClear();
	//	//debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);

	//	m_imageView->ImageOverlayAdd(rect, RGB(255, 0, 0), 1);
	//	m_imageView->ImageOverlayShow();
	//}
}

void CDlgInspectionMain::OnChangedAbnormalGV(LPARAM lparam, int nID)
{
	//IPVM::ImageProcessing::GetSum();
	// Sum / x*y 로 해준 값이 roi의 avg
	auto* abnormalInspection = (CInspectionAbnormal*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	CString strGV;

	if (nID == IDC_PARA_ABNORMAL_GV)
	{
		strGV = pItem->GetValue();
        abnormalInspection->m_para.m_nGrayValue = _wtoi(strGV);
	}
    if (nID == IDC_PARA_ABNORMAL_GV_2)
    {
        strGV = pItem->GetValue();
        abnormalInspection->m_para.m_nGrayValue_2 = _wtoi(strGV);
	}
}

void CDlgInspectionMain::OnChangedAbnormalUseInspection(LPARAM lparam, int nID)
{
	auto* abnormalInspection = (CInspectionAbnormal*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	bool bUse;
	CString strUse;

	if (nID == IDC_PARA_ABNORMAL_USE_INSPECTION)
	{
		strUse = pItem->GetValue();
	}

	if (strUse == _T("Not Use"))
	{
		abnormalInspection->m_para.m_bInspectionSkip = false;
	}
	else
	{
		abnormalInspection->m_para.m_bInspectionSkip = true;
	}
}

void CDlgInspectionMain::OnChangedBlobEqualUse(LPARAM lparam, int nID)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	int id = pItem->GetID();
	if (id == 0)	// ID 지정하지 않은 하위 Category
	{
		pItem = (CXTPPropertyGridItem*)pItem->GetParentItem();
		if (pItem == nullptr)
			return;
	}
	int index = (id - nID);

	auto* itemUseEqual = ((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_USE_EQUAL_000 + index));
	//auto* itemUseEqualThreshold = ((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_USE_EQUAL_THRESHOLD_000 + index));

	//if (itemUseEqual->GetValue() == _T("False") || itemUseEqualThreshold->GetValue() == _T("False"))
    if (itemUseEqual->GetValue() == _T("False"))
	{
		((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_EQUAL_THRESHOLD_MAX_000 + index))->SetReadOnly();
		((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_EQUAL_THRESHOLD_MIN_000 + index))->SetReadOnly();
		((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_EQUAL_THREAD_COUNT_000 + index))->SetReadOnly();
	}
	else
	{
		((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_EQUAL_THRESHOLD_MAX_000 + index))->SetReadOnly(FALSE);
		((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_EQUAL_THRESHOLD_MIN_000 + index))->SetReadOnly(FALSE);
        ((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_EQUAL_THREAD_COUNT_000 + index))->SetReadOnly(FALSE);
	}

	/*if (nID == IDC_PARA_BLOB_USE_EQUAL_000)
	{
		if (itemUseEqual->GetValue() == _T("False"))
			((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_USE_EQUAL_THRESHOLD_000 + index))->SetReadOnly();
		else
			((CXTPPropertyGridItem*)m_inspectionParaGrid->FindItem(IDC_PARA_BLOB_USE_EQUAL_THRESHOLD_000 + index))->SetReadOnly(FALSE);
	}*/
	
	//auto* item = (CXTPPropertyGridItemNumber*)m_surfaceInspectionPara->FindItem(IDC_PARA_BLOB_USE_EQUAL_THRESHOLD_000);
	//	item->SetReadOnly(FALSE);
}

void CDlgInspectionMain::OnChangedBlobRoiDiv(LPARAM lparam, int nID)
{
	auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	auto* cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign);

	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	int id = pItem->GetID();
	if (id == 0)	// ID 지정하지 않은 하위 Category
	{
		pItem = (CXTPPropertyGridItem*)pItem->GetParentItem();
		if (pItem == nullptr)
			return;
	}


	// refresh roi information
	if (nID == IDC_PARA_BLOB_THRESHOL_HOR_DIV_000
		|| nID == IDC_PARA_BLOB_THRESHOL_VER_DIV_000)
	{
		
		int id = pItem->GetID();
		int condition = (id - nID) / COUNT_CELL_ROI;
		int roi = (id - nID) % COUNT_CELL_ROI;

		CBlobParaRoi& blobPara = inspection->m_para.m_vecBlobParaCondition[roi];
		bool bCheck = blobPara.m_bUseInspection;

		if (bCheck)
		{
			IPVM::Rect rectCal = blobPara.m_rtSetInspArea;

			IPVM::Image_8u_C1 ImageThreshold;
			ImageThreshold.Create(m_inspectionImageBright.GetSizeX(), m_inspectionImageBright.GetSizeY());

			IPVM::Image_8u_C1* ImageSrc;
			if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)//bright
			{
				ImageSrc = &m_inspectionImageBright;
			}
            else //if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Darkness) //bright
			{
				ImageSrc = &m_inspectionImageDark;
			}
            
			//계산상 이름이 반대로...
			long nVerDivSave =  blobPara.m_nThresholdHorDiv;
			long nHorDivSave =  blobPara.m_nThresholdVerDiv;

			IPVM::Rect rtOrig = blobPara.m_rtSetInspArea;

			std::vector<IPVM::Rect> rtDiv;


			long nSizeOrigX = rtOrig.Width();
			long nSizeOrigY = rtOrig.Height();

			float fHorzSize = rtOrig.Height() / (float)nHorDivSave;
			float fVertSize = rtOrig.Width() / (float)nVerDivSave;

			int x, y, k;

			if (nVerDivSave == 999)
            {
                float fSensorPosOffset = ImageSrc->GetSizeX() / 32;
                float fPosCheckY = rtOrig.CenterPoint().m_y;

                IPVM::Point_32f_C2 PtCheckingSensor;
                IPVM::Rect rtValidRect;
                
				long nValidCnt = 0;
                for (int cnt = 0; cnt < 32; cnt++)
                {
                    PtCheckingSensor.m_x = fSensorPosOffset * cnt;
                    PtCheckingSensor.m_y = fPosCheckY;
                    if (rtOrig.PtInRect(PtCheckingSensor))
                    {
                        // Left 맞추기
                        if (nValidCnt == 0)
                        {
                            rtValidRect.m_left = rtOrig.m_left;
                        }
                        else
                        {
                            rtValidRect.m_left = fSensorPosOffset * (cnt - 1);
                        }

                        rtValidRect.m_top = rtOrig.m_top;
                        rtValidRect.m_bottom = rtOrig.m_bottom;
                        rtValidRect.m_right = PtCheckingSensor.m_x;

                        rtDiv.push_back(rtValidRect);
                        nValidCnt++;
                    }
                    // Right 맞추기
                    if (fSensorPosOffset * (cnt -1) < rtOrig.m_right && rtOrig.m_right <= fSensorPosOffset * cnt)
                    {
                        rtValidRect.m_left = rtDiv[nValidCnt-1].m_right;
                        rtValidRect.m_top = rtOrig.m_top;
                        rtValidRect.m_bottom = rtOrig.m_bottom;
                        rtValidRect.m_right = rtOrig.m_right;
                        rtDiv.push_back(rtValidRect);
                        break;
                    }
                }
            }
            else
            {
                for (y = 0; y < nHorDivSave; y++)
                {
                    for (x = 0; x < nVerDivSave; x++)
                    {
                        k = y * nVerDivSave + x;
                        IPVM::Rect temp = IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f),
                            (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f),
                            (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
                        rtDiv.push_back(temp);
                    }
                }
            }

			//show roi
			{
				
				m_imageView->ImageOverlayClear();
				//debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);
				for (long n = 0; n < rtDiv.size(); n++)
				{
					m_imageView->ImageOverlayAdd(rtDiv[n], RGB(0, 255, 0), 1);
				}
				m_imageView->ImageOverlayShow();
			}

		}

		
	}

	

	
}

void CDlgInspectionMain::OnChangedBlobThreshold(LPARAM lparam, int nID)
{
	auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	int id = pItem->GetID();
	int condition = (id - nID) / COUNT_CELL_ROI;
	int roi = (id - nID) % COUNT_CELL_ROI;

	CBlobParaRoi& blobPara = inspection->m_para.m_vecBlobParaCondition[roi];
	bool bCheck = blobPara.m_bUseInspection;

	if (bCheck)
	{
		IPVM::Rect rectCal = blobPara.m_rtSetInspArea;

		IPVM::Image_8u_C1 ImageThreshold;
		ImageThreshold.Create(m_inspectionImageBright.GetSizeX(), m_inspectionImageBright.GetSizeY());

		IPVM::Image_8u_C1* ImageSrc;
		if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)//bright
		{
			ImageSrc = &m_inspectionImageBright;
           // m_bufferThreshold;
		}
        else if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Darkness) //bright
		{
			ImageSrc = &m_inspectionImageDark;
		}
        else
        {
            IPVM::ImageProcessing::Max(m_inspectionImageDark, m_inspectionImageBright,
                IPVM::Rect(m_inspectionImageBright), inspection->m_bufferThreshold6);
            ImageSrc = &inspection->m_bufferThreshold6;
        }

        if (blobPara.m_bUseEqual)
        {
            ImageSrc = &inspection->m_bufferThreshold;
        }

		
        //IPVM::ImageProcessing::Copy(ImageSrc, rectCal, rectCal,
        //    m_bufferThreshold); //각 Rect영역에 대한 이미지 복사
        //if (blobPara.m_bUseEqual)
        //{
        //    Equalization_TargetWeighted(m_bufferThreshold, rectCal, 128, 1.5f);
        //}

		//계산상 이름이 반대로...
		long nVerDivSave =  blobPara.m_nThresholdHorDiv;
		long nHorDivSave =  blobPara.m_nThresholdVerDiv;

		IPVM::Rect rtOrig = blobPara.m_rtSetInspArea;

		std::vector<IPVM::Rect> rtDiv;

		long nSizeOrigX = rtOrig.Width();
		long nSizeOrigY = rtOrig.Height();

		float fHorzSize = rtOrig.Height() / (float)nHorDivSave;
		float fVertSize = rtOrig.Width() / (float)nVerDivSave;

		int x, y, k;
		/*for (y = 0; y < nHorDivSave; y++)
		{
			for (x = 0; x < nVerDivSave; x++)
			{
				k = y * nVerDivSave + x;
				IPVM::Rect temp = IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f), (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f), (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
				rtDiv.push_back(temp);
			}
		}*/
        if (nVerDivSave == 999)
        {
            float fSensorPosOffset = m_inspectionImageBright.GetSizeX() / 32;
            float fPosCheckY = rtOrig.CenterPoint().m_y;

            IPVM::Point_32f_C2 PtCheckingSensor;
            IPVM::Rect rtValidRect;

            long nValidCnt = 0;
            for (int cnt = 0; cnt < 32; cnt++)
            {
                PtCheckingSensor.m_x = fSensorPosOffset * cnt;
                PtCheckingSensor.m_y = fPosCheckY;
                if (rtOrig.PtInRect(PtCheckingSensor))
                {
                    // Left 맞추기
                    if (nValidCnt == 0)
                    {
                        rtValidRect.m_left = rtOrig.m_left;
                    }
                    else
                    {
                        rtValidRect.m_left = fSensorPosOffset * (cnt - 1);
                    }

                    rtValidRect.m_top = rtOrig.m_top;
                    rtValidRect.m_bottom = rtOrig.m_bottom;
                    rtValidRect.m_right = PtCheckingSensor.m_x;

                    rtDiv.push_back(rtValidRect);
                    nValidCnt++;
                }
                // Right 맞추기
                if (fSensorPosOffset * (cnt - 1) < rtOrig.m_right && rtOrig.m_right <= fSensorPosOffset * cnt)
                {
                    rtValidRect.m_left = rtDiv[nValidCnt - 1].m_right;
                    rtValidRect.m_top = rtOrig.m_top;
                    rtValidRect.m_bottom = rtOrig.m_bottom;
                    rtValidRect.m_right = rtOrig.m_right;
                    rtDiv.push_back(rtValidRect);
                    break;
                }
            }
        }
        else
        {
            for (y = 0; y < nHorDivSave; y++)
            {
                for (x = 0; x < nVerDivSave; x++)
                {
                    k = y * nVerDivSave + x;
                    IPVM::Rect temp = IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f),
                        (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f),
                        (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
                    rtDiv.push_back(temp);
                }
            }
        }

		float fAverage;
		//if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::White)
		{
			//		BinarizationGreater_Mean(CopyImage, IPVM::Rect(InspImage), m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
			for (long n = 0; n < rtDiv.size(); n++)
			{
				//inspection->BinarizationGreater_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
				inspection->BinarizationGreater_Mean(*ImageSrc, rtDiv[n], blobPara.m_nThreshold, ImageThreshold, fAverage);
			}
		}
		//else if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::Black)
		
		//{
		//	//BinarizationLess_Mean(CopyImage, IPVM::Rect(InspImage), m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
		//	for (long n = 0; n < rtDiv.size(); n++)
		//	{
		//		//inspection->BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
		//		inspection->BinarizationLess_Mean(*ImageSrc, rtDiv[n], blobPara.m_nThreshold, ImageThreshold, fAverage);
		//	}
		//}
		


		//float fAverage;
		//if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::White)
		//{
		//	inspection->BinarizationGreater_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold, ImageThreshold, fAverage);
		//}
		//else if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::Black)
		//{
		//	inspection->BinarizationLess_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold, ImageThreshold, fAverage);
		//}
		//else
		//{
		//	//float fAverage;
		//	inspection->BinarizationGreater_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold, ImageThreshold, fAverage);
		//	inspection->BinarizationLess_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold2, ImageThreshold, fAverage);
		//}

		m_imageView->ImageOverlayClear();
		//debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);
		m_imageView->SetImage(ImageThreshold);
		m_imageView->ImageOverlayAdd(rectCal, RGB(0, 255, 0), 1);
		m_imageView->ImageOverlayShow();
	}
}

void CDlgInspectionMain::OnChangedBlobThreshold2(LPARAM lparam, int nID)
{
	auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	int id = pItem->GetID();
	int condition = (id - nID) / COUNT_CELL_ROI;
	int roi = (id - nID) % COUNT_CELL_ROI;

	CBlobParaRoi& blobPara = inspection->m_para.m_vecBlobParaCondition[roi];
	bool bCheck = blobPara.m_bUseInspection;

	if (bCheck)
	{
		IPVM::Rect rectCal = blobPara.m_rtSetInspArea;

		IPVM::Image_8u_C1 ImageThreshold;
		ImageThreshold.Create(m_inspectionImageBright.GetSizeX(), m_inspectionImageBright.GetSizeY());

		IPVM::Image_8u_C1* ImageSrc;
		if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)//bright
		{
			ImageSrc = &m_inspectionImageBright;
		}
        else if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Darkness) //bright
        {
            ImageSrc = &m_inspectionImageDark;
        }
        else
        {
            IPVM::ImageProcessing::Max(m_inspectionImageDark, m_inspectionImageBright,
                IPVM::Rect(m_inspectionImageBright), inspection->m_bufferThreshold6);
            ImageSrc = &inspection->m_bufferThreshold6;
        }
        if (blobPara.m_bUseEqual)
        {
            ImageSrc = &inspection->m_bufferThreshold;
        }
		//계산상 이름이 반대로...
		long nVerDivSave = blobPara.m_nThresholdHorDiv;
		long nHorDivSave = blobPara.m_nThresholdVerDiv;

		IPVM::Rect rtOrig = blobPara.m_rtSetInspArea;

		std::vector<IPVM::Rect> rtDiv;

		long nSizeOrigX = rtOrig.Width();
		long nSizeOrigY = rtOrig.Height();

		float fHorzSize = rtOrig.Height() / (float)nHorDivSave;
		float fVertSize = rtOrig.Width() / (float)nVerDivSave;

		int x, y, k;
		/*for (y = 0; y < nHorDivSave; y++)
		{
			for (x = 0; x < nVerDivSave; x++)
			{
				k = y * nVerDivSave + x;
				IPVM::Rect temp = IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f), (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f), (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
				rtDiv.push_back(temp);
			}
		}*/

		if (nVerDivSave == 999)
        {
            float fSensorPosOffset = m_inspectionImageBright.GetSizeX() / 32;
            float fPosCheckY = rtOrig.CenterPoint().m_y;

            IPVM::Point_32f_C2 PtCheckingSensor;
            IPVM::Rect rtValidRect;

            long nValidCnt = 0;
            for (int cnt = 0; cnt < 32; cnt++)
            {
                PtCheckingSensor.m_x = fSensorPosOffset * cnt;
                PtCheckingSensor.m_y = fPosCheckY;
                if (rtOrig.PtInRect(PtCheckingSensor))
                {
                    // Left 맞추기
                    if (nValidCnt == 0)
                    {
                        rtValidRect.m_left = rtOrig.m_left;
                    }
                    else
                    {
                        rtValidRect.m_left = fSensorPosOffset * (cnt - 1);
                    }

                    rtValidRect.m_top = rtOrig.m_top;
                    rtValidRect.m_bottom = rtOrig.m_bottom;
                    rtValidRect.m_right = PtCheckingSensor.m_x;

                    rtDiv.push_back(rtValidRect);
                    nValidCnt++;
                }
                // Right 맞추기
                if (fSensorPosOffset * (cnt - 1) < rtOrig.m_right && rtOrig.m_right <= fSensorPosOffset * cnt)
                {
                    rtValidRect.m_left = rtDiv[nValidCnt - 1].m_right;
                    rtValidRect.m_top = rtOrig.m_top;
                    rtValidRect.m_bottom = rtOrig.m_bottom;
                    rtValidRect.m_right = rtOrig.m_right;
                    rtDiv.push_back(rtValidRect);
                    break;
                }
            }
        }
        else
        {
            for (y = 0; y < nHorDivSave; y++)
            {
                for (x = 0; x < nVerDivSave; x++)
                {
                    k = y * nVerDivSave + x;
                    IPVM::Rect temp = IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f),
                        (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f),
                        (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
                    rtDiv.push_back(temp);
                }
            }
        }
		float fAverage;
		//if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::White)
		//{
		//	//		BinarizationGreater_Mean(CopyImage, IPVM::Rect(InspImage), m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
		//	for (long n = 0; n < rtDiv.size(); n++)
		//	{
		//		//inspection->BinarizationGreater_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
		//		inspection->BinarizationGreater_Mean(*ImageSrc, rtDiv[n], blobPara.m_nThreshold, ImageThreshold, fAverage);
		//	}
		//}
		//else if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::Black)

		{
			//BinarizationLess_Mean(CopyImage, IPVM::Rect(InspImage), m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
            if (inspection->m_para.m_vecBlobParaCondition[roi].m_bUseMeanRangeTheshold == true)
            {
                for (long n = 0; n < rtDiv.size(); n++)
                {
                    //inspection->BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
                    inspection->BinarizationLess_Mean(*ImageSrc, rtDiv[n], blobPara.m_nThreshold2, ImageThreshold, fAverage);
                }
			}
            else
            {
                for (long n = 0; n < rtDiv.size(); n++)
                {
                    //inspection->BinarizationLess_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold2, InspImage, fAverage);
                    inspection->BinarizationLess_Mean_noRange(*ImageSrc, rtDiv[n], blobPara.m_nThreshold2, ImageThreshold, fAverage);
                }
			}
		}



		//float fAverage;
		//if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::White)
		//{
		//	inspection->BinarizationGreater_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold, ImageThreshold, fAverage);
		//}
		//else if (blobPara.m_nBlobBrightType == (long)ENUM_BLOB_TYPE_THBRIGHT::Black)
		//{
		//	inspection->BinarizationLess_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold, ImageThreshold, fAverage);
		//}
		//else
		//{
		//	//float fAverage;
		//	inspection->BinarizationGreater_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold, ImageThreshold, fAverage);
		//	inspection->BinarizationLess_Mean(*ImageSrc, rectCal, blobPara.m_nThreshold2, ImageThreshold, fAverage);
		//}

		m_imageView->ImageOverlayClear();
		//debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);
		m_imageView->SetImage(ImageThreshold);
		m_imageView->ImageOverlayAdd(rectCal, RGB(0, 255, 0), 1);
		m_imageView->ImageOverlayShow();
	}
}

void CDlgInspectionMain::OnChangedBlobUseInspection(LPARAM lparam, int nID)
{
	auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	int id = pItem->GetID();
	int condition = (id - nID) / COUNT_CELL_ROI;
	int roi = (id - nID) % COUNT_CELL_ROI;

	CBlobParaRoi& blobPara = inspection->m_para.m_vecBlobParaCondition[roi];
	bool& bCheck = blobPara.m_bUseInspection;

	if (bCheck)
	{
		IPVM::Rect rectCal = blobPara.m_rtSetInspArea;
		
		IPVM::Image_8u_C1* ImageSrc;
		if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness)//bright
		{
			ImageSrc = &m_inspectionImageBright;
		}
		else
		{
			ImageSrc = &m_inspectionImageDark;
		}

		m_imageView->ImageOverlayClear();
		//debug->AddOverlay(LanguageDef::Debug_CellBoundaryROI, m_unionResult.m_rectArea[i]);
		m_imageView->SetImage(*ImageSrc);
		m_imageView->ImageOverlayAdd(rectCal, RGB(0, 255, 0), 1);
		m_imageView->ImageOverlayShow();
	}
}

void  CDlgInspectionMain::UpdateCheckBoxChilds(const LPARAM& lparam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;

	CXTPPropertyGridItems* pSiblingItems = pItem->GetParentItem()->GetChilds();

	for (int i = 0; i < pSiblingItems->GetCount(); i++)
	{
		if (pSiblingItems->GetAt(i) != pItem)
		{
			pSiblingItems->GetAt(i)->SetHidden(!((CXTPPropertyGridItemBool*)pItem)->GetBool());
		}
	}
}

void CDlgInspectionMain::ShowBotOverlayInspArea()
{
	m_imageView->ImageOverlayShow();
}

void CDlgInspectionMain::OnBnClickedShowEdgeDetectParas(LPARAM lparam, const int& nID)
{
	auto& insp = *((CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory));
	//auto& ISI = *((CInspectionISI*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::ISI));

	Base::ParaEdgeDetection* pParaEdgeDetection;
	pParaEdgeDetection = nullptr;

	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;
	int id = pItem->GetID();
	int cnt_para = (id - nID);

	switch (nID)
	{

	case IDC_PARA_BUTTON_LINEDETECT_PARA_MEASURE_00:
		pParaEdgeDetection = &insp.m_para.m_vecDetectLineParaMeasure[cnt_para];
		break;
	case IDC_PARA_BUTTON_LINEDETECT_PARA_BLOB_00:
		pParaEdgeDetection = &insp.m_para.m_vecDetectLineParaBlob[cnt_para];
		break;

	default:
		break;
	}

	DlgEdgeDetectPara dlg(*pParaEdgeDetection, 0);
	if (dlg.DoModal() == IDOK)
	{

	}
}

void CDlgInspectionMain::OnBnClickedAddDetailInspSetting(LPARAM lparam, const int& nID)
{
	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	// Step #1. BlobParaCondition 정보 준비
	auto& pBlobPara = *((CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory));
	auto& pCellAlign = *((CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::cellAlign));
	
	long nTotalInspectionNum = pBlobPara.m_para.m_vecBlobParaCondition.size();
	long nAddConditionIdx = nTotalInspectionNum;
	
	CString strAddConditionName;
	strAddConditionName.Format(_T("Insp Blob %03d"), nAddConditionIdx);
	
	IPVM::Rect rect(pCellAlign.m_unionResult.m_rectAreaMeasure[0]);

	// Step #2. Insert는 Pushback으로 항상 끝에(Use Inspection Value는 Fasle/ Inspection Area는 Align 첫번째 영역으로)
	CBlobParaRoi itemBlobPara(strAddConditionName);
	itemBlobPara.m_bUseInspection = FALSE;
	itemBlobPara.m_nInspectionArea = 0;
	itemBlobPara.m_rtSetInspArea = rect;
	pBlobPara.m_para.m_vecBlobParaCondition.push_back(itemBlobPara);
    pBlobPara.m_para.m_nTotalInspCount++;
	
	UpdateInspectionParaGridBlob();
}

void CDlgInspectionMain::OnBnClickedDeleteDetailInspSetting(LPARAM lparam, const int& nID)
{
	if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
		return;

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == nullptr)
		return;

	CXTPPropertyGridItem* pItemParent = (CXTPPropertyGridItem*)pItem->GetParentItem();
	if (pItemParent == nullptr)
		return;

	auto& pBlobPara = *((CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory));

	//Step #1. 삭제되는 BlobParaCondition Index 찾기
	CString strDeleteConditionName = pItemParent->GetCaption();
	long nDeleteConditionIdx = _ttoi(strDeleteConditionName.Right(3));

	//Step #2. BlobParaCondition Item 삭제 (중간에 있을 수도 있으니 erase로)
	//예외처리 Insp Item이 1개일 때, 지우지말고 Use Inspection으로 처리
	long nTotalInspectionNum = pBlobPara.m_para.m_vecBlobParaCondition.size();
	if (nTotalInspectionNum > 1)
	{
		CString strParaConditionName;
		for (int index = 0; index < nTotalInspectionNum; index++)
		{
			if (index == nDeleteConditionIdx)
			{
				pBlobPara.m_para.m_vecBlobParaCondition.erase(pBlobPara.m_para.m_vecBlobParaCondition.begin() + nDeleteConditionIdx);
                pBlobPara.m_para.m_nTotalInspCount--;
			}
			else if (nDeleteConditionIdx < index)
			{
				strParaConditionName.Format(_T("Insp Blob %03d"), index - 1);
				pBlobPara.m_para.m_vecBlobParaCondition[index - 1].m_name = strParaConditionName;
			}
			else
			{
				strParaConditionName.Format(_T("Insp Blob %03d"), index);
				pBlobPara.m_para.m_vecBlobParaCondition[index].m_name = strParaConditionName;
			}
		}
	}
	else
	{
		AfxMessageBox(_T("Item이 1개일 때는 삭제할 수 없습니다."));
	}

	UpdateInspectionParaGridBlob();
}
