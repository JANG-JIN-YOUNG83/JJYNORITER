// DlgInspectionItemGrid.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "InspectionRoot.h"
#include "DlgInspectionMain.h"
#include "DlgCriteriaList.h"
#include "DlgMeasureOffsets.h"
#include "DlgMergeOptions.h"
#include "Accessor.h"
#include "afxdialogex.h"
#include "ImageViewEx.h"

#include "CDlgClassificationST.h"

#include "../Module/CustomControl/XTPPropertyGridItemButton.h"
#include "../Module/CustomControl/GridCtrl.h"
#include "../VisionModule/Language/LanguageText.h"
#include "../IBCI/SystemParameter.h"
#include "Types/Image_8u_C3.h"
#include "Types/Image_8u_C4.h"
#include "Algorithm/ImageProcessing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgInspectionMain 대화 상자입니다.

void CDlgInspectionMain::InitInspecitonItemGrid()
{
	CString strUpSide, strDownSide, strConfiguration, strAlign, strUncoat, strSurface, strTab, strScrab, strTape, strElecTorn, strShoulder, strTabReject, strNGmark, strMeasure, strBurr, strEtc, strISI, strBlob, strDeeplearning, strAbnormalInspection, strChattering;
	if (m_nLanguage == 1)
	{
		strUpSide = _T("상면");
		strDownSide = _T("하면");
		strConfiguration = _T("Configuration");
		strAlign = _T("Cell Alignment");
		strUncoat = _T("미코팅");
		strSurface = _T("표면");
		strTab = _T("탭");
		strScrab = _T("스크랩");
		strScrabROI = _T("Scrab ROI");
		strTape = _T("연결 테이프");
		strElecTorn = _T("전극 찢김");
		strShoulder = _T("어깨선");
		strTabReject = _T("탭 불량");
		strNGmark = _T("NG 마크");
		strMeasure = _T("치수 측정");
        strChattering = _T("Chattering");
		strBurr = _T("Burr");
		strEtc = _T("Etc");
		strISI = _T("ISI");
		strBlob = _T("Blob");
		strDeeplearning = _T("Deeplearning");
		strAbnormalInspection = _T("Abnormal Inspection");
	}
	else if (m_nLanguage == 2)
	{
		strUpSide = _T("Upside");
		strDownSide = _T("Downside");
		strConfiguration = _T("Configuration");
		strAlign = _T("Cell alignment");
		strUncoat = _T("Uncoating");
		strSurface = _T("Blob Criteria");
		strTab = _T("Tab");
		strScrab = _T("Scrab");
		strScrabROI = _T("Scrab ROI");
		strTape = _T("Tape");
		strElecTorn = _T("Electrode Torn");
		strShoulder = _T("Shoulder Line");
		strTabReject = _T("Tab");
		strNGmark = _T("NG Mark");
        strChattering = _T("Chattering");
		strMeasure = _T("Measure");
		strBurr = _T("Burr");
		strEtc = _T("Etc");
		strISI = _T("ISI");
		strBlob = _T("Blob");
		strDeeplearning = _T("Deeplearning");
		strAbnormalInspection = _T("Abnormal Inspection");
	}
	else
	{
		strUpSide = _T("上面");
		strDownSide = _T("下面");
		strConfiguration = _T("Configuration");
		strAlign = _T("基本设定");
		strUncoat = _T("未涂层");
		strSurface = _T("表面");
		strTab = _T("Tab");
		strScrab = _T("Scrab");
		strScrabROI = _T("Scrab ROI");
		strTape = _T("链接胶带");
		strElecTorn = _T("电极裂痕");
		strShoulder = _T("肩高");
		strTabReject = _T("Tab");
		strNGmark = _T("NG标志");
        strChattering = _T("Chattering");
		strMeasure = _T("综合测定");
		strBurr = _T("Burr");
		strEtc = _T("Etc");
		strISI = _T("ISI");
		strBlob = _T("Blob");
		strDeeplearning = _T("Deeplearning");
		strAbnormalInspection = _T("Abnormal Inspection");
	}

	if (auto* category = m_inspectionItemGrid->AddCategory(_T("Option")))
	{
		/*if (auto* button = category->AddChildItem(new XTPPropertyGridItemButton(_T("Criteria Option"))))
		{
			button->SetHeight(2 * button->GetHeight());
			button->SetID(IDC_BTN_CRITERIA_OPTIONS);
		}

		if (auto* button = category->AddChildItem(new XTPPropertyGridItemButton(_T("Measure Offset"))))
		{
			button->SetHeight(2 * button->GetHeight());
			button->SetID(IDC_BTN_MEASURE_OFFSET);
		}

		if (auto* button = category->AddChildItem(new XTPPropertyGridItemButton(_T("Merge Options"))))
		{
			button->SetHeight(2 * button->GetHeight());
			button->SetID(IDC_BTN_MERGE_OPTIONS);
		}*/



		if (auto* button = category->AddChildItem(new XTPPropertyGridItemButton(_T("ISI Criteria"))))
		{
			button->SetHeight(2 * button->GetHeight());
			button->SetID(IDC_BTN_ISI_CRITERIA);
		}


	

		if (auto* button = category->AddChildItem(new XTPPropertyGridItemButton(_T("Cell Design"))))
		{
			button->SetHeight(2 * button->GetHeight());
			button->SetID(IDC_BTN_CELL_DESIGN);
		}

		category->Expand();
	}
	auto *upside = m_inspectionItemGrid->AddCategory(_T("Inspection Item Category"));
	{
		auto *rollerMark = upside->AddChildItem(new XTPPropertyGridItemButton(strConfiguration));
		rollerMark->SetHeight(2 * rollerMark->GetHeight());
		rollerMark->SetID(IDC_BTN_UPSIDE_CONFIGURATION);
		rollerMark->SetHidden(TRUE);
		auto *cellAlign = upside->AddChildItem(new XTPPropertyGridItemButton(strAlign));
		cellAlign->SetHeight(2 * cellAlign->GetHeight());
		cellAlign->SetID(IDC_BTN_UPSIDE_CELL_ALIGN);
		auto *uncoating = upside->AddChildItem(new XTPPropertyGridItemButton(strUncoat));
		uncoating->SetHeight(2 * uncoating->GetHeight());
		uncoating->SetID(IDC_BTN_UPSIDE_UNCOATING);
		uncoating->SetHidden(TRUE);
		auto* tape = upside->AddChildItem(new XTPPropertyGridItemButton(strTape));
		tape->SetHeight(2 * tape->GetHeight());
		tape->SetID(IDC_BTN_UPSIDE_TAPE);
		tape->SetHidden(TRUE);

		auto* Scrab = upside->AddChildItem(new XTPPropertyGridItemButton(strScrab));
		Scrab->SetHeight(2 * Scrab->GetHeight());
		Scrab->SetID(IDC_BTN_UPSIDE_SCRAB);
		Scrab->SetHidden(TRUE);

		auto* Tab = upside->AddChildItem(new XTPPropertyGridItemButton(strTab));
		Tab->SetHeight(2 * Tab->GetHeight());
		Tab->SetID(IDC_BTN_UPSIDE_TAB);
		//Tab->SetHidden(TRUE);
		/// 
		
		auto *electrodetorn = upside->AddChildItem(new XTPPropertyGridItemButton(strElecTorn));
		electrodetorn->SetHeight(2 * electrodetorn->GetHeight());
		electrodetorn->SetID(IDC_BTN_UPSIDE_ELECTRODE_TORN);
		electrodetorn->SetHidden(TRUE);
		auto *shoulderline = upside->AddChildItem(new XTPPropertyGridItemButton(strShoulder));
		shoulderline->SetHeight(2 * shoulderline->GetHeight());
		shoulderline->SetID(IDC_BTN_UPSIDE_SHOULDERLINE);
		//auto *tabfold = upside->AddChildItem(new XTPPropertyGridItemButton(strTabReject));
		//tabfold->SetHeight(2 * tabfold->GetHeight());
		//tabfold->SetID(IDC_BTN_UPSIDE_FOLDING);
		//tabfold->SetHidden(TRUE);

		auto* Burr = upside->AddChildItem(new XTPPropertyGridItemButton(strBurr));
		Burr->SetHeight(2 * Burr->GetHeight());
		Burr->SetID(IDC_BTN_UPSIDE_BURR);


		auto *ngmark = upside->AddChildItem(new XTPPropertyGridItemButton(strNGmark));
		ngmark->SetHeight(2 * ngmark->GetHeight());
		ngmark->SetID(IDC_BTN_UPSIDE_NG_MARK);
		ngmark->SetHidden(TRUE);
		auto* abnormalInspection = upside->AddChildItem(new XTPPropertyGridItemButton(strAbnormalInspection));
		abnormalInspection->SetHeight(2 * abnormalInspection->GetHeight());
		abnormalInspection->SetID(IDC_BTN_UPSIDE_ABNORMAL_INSPECTION);
		abnormalInspection->SetHidden(FALSE);

		auto* chattering = upside->AddChildItem(new XTPPropertyGridItemButton(strChattering));
        chattering->SetHeight(2 * chattering->GetHeight());
        chattering->SetID(IDC_BTN_UPSIDE_CHATTERING);

		auto *measure = upside->AddChildItem(new XTPPropertyGridItemButton(strMeasure));
		measure->SetHeight(2 * measure->GetHeight());
        measure->SetID(IDC_BTN_UPSIDE_MEASURE);

		/// <UI 변경 >
		auto* Etc = upside->AddChildItem(new XTPPropertyGridItemButton(strEtc));
		Etc->SetHeight(2 * Etc->GetHeight());
		Etc->SetID(IDC_BTN_UPSIDE_ETC);
		Etc->SetHidden(TRUE);
		/// 
		/// <UI 변경 >
		auto* Surface = upside->AddChildItem(new XTPPropertyGridItemButton(strSurface));
		Surface->SetHeight(2 * Surface->GetHeight());
		Surface->SetID(IDC_BTN_UPSIDE_SURFACE);
		Surface->SetHidden(TRUE);

		CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
		if(parent->m_generationversion == Share::GenerationVersion::SplitterRemoved)
		{
			rollerMark->SetHidden(FALSE);
			uncoating->SetHidden(FALSE);
			tape->SetHidden(FALSE);
			Surface->SetHidden(FALSE);
			Scrab->SetHidden(FALSE);
			//Tab->SetHidden(FALSE);
			Etc->SetHidden(FALSE);
		}
		auto* Isi = upside->AddChildItem(new XTPPropertyGridItemButton(strISI));
		Isi->SetHeight(2 * Isi->GetHeight());
		Isi->SetID(IDC_BTN_UPSIDE_ISI);
#ifndef _ISI_
        Isi->SetHidden(TRUE);
		uncoating->SetHidden(TRUE);
		tape->SetHidden(TRUE);
		//Surface->SetHidden(TRUE);
		electrodetorn->SetHidden(TRUE);
		shoulderline->SetHidden(TRUE);
		Burr->SetHidden(TRUE);
		Scrab->SetHidden(TRUE);
		ngmark->SetHidden(TRUE);
		//measure->SetHidden(TRUE);
		Tab->SetHidden(TRUE);
#endif
        auto* blob = upside->AddChildItem(new XTPPropertyGridItemButton(strBlob));
        blob->SetHeight(2 * blob->GetHeight());
        blob->SetID(IDC_BTN_UPSIDE_BLOB);
        blob->SetHidden(FALSE);
		auto* deeplearning = upside->AddChildItem(new XTPPropertyGridItemButton(strDeeplearning));
		deeplearning->SetHeight(2 * deeplearning->GetHeight());
		deeplearning->SetID(IDC_BTN_UPSIDE_DEEPLEARNING);
		deeplearning->SetHidden(FALSE);
		
	}
	upside->Expand();
	auto *downside = m_inspectionItemGrid->AddCategory(strDownSide);
	{
		auto *rollerMark = downside->AddChildItem(new XTPPropertyGridItemButton(strConfiguration));
		rollerMark->SetHeight(2 * rollerMark->GetHeight());
		rollerMark->SetID(IDC_BTN_DOWNSIDE_CONFIGURATION);
		rollerMark->SetHidden(TRUE);
		auto *cellAlign = downside->AddChildItem(new XTPPropertyGridItemButton(strAlign));
		cellAlign->SetHeight(2 * cellAlign->GetHeight());
		cellAlign->SetID(IDC_BTN_DOWNSIDE_CELL_ALIGN);
		auto *uncoating = downside->AddChildItem(new XTPPropertyGridItemButton(strUncoat));
		uncoating->SetHeight(2 * uncoating->GetHeight());
		uncoating->SetID(IDC_BTN_DOWNSIDE_UNCOATING);
		uncoating->SetHidden(TRUE);
		auto* tape = downside->AddChildItem(new XTPPropertyGridItemButton(strTape));
		tape->SetHeight(2 * tape->GetHeight());
		tape->SetID(IDC_BTN_DOWNSIDE_TAPE);
		tape->SetHidden(TRUE);

		auto* Scrab = downside->AddChildItem(new XTPPropertyGridItemButton(strScrab));
		Scrab->SetHeight(2 * Scrab->GetHeight());
		Scrab->SetID(IDC_BTN_DOWNSIDE_SCRAB);
		Scrab->SetHidden(TRUE);

		auto* Tab = downside->AddChildItem(new XTPPropertyGridItemButton(strTab));
		Tab->SetHeight(2 * Tab->GetHeight());
		Tab->SetID(IDC_BTN_DOWNSIDE_TAB);
		//Tab->SetHidden(TRUE);
		/// 

		auto *electrodetorn = downside->AddChildItem(new XTPPropertyGridItemButton(strElecTorn));
		electrodetorn->SetHeight(2 * electrodetorn->GetHeight());
		electrodetorn->SetID(IDC_BTN_DOWNSIDE_ELECTRODE_TORN);
		electrodetorn->SetHidden(TRUE);
		auto *shoulderline = downside->AddChildItem(new XTPPropertyGridItemButton(strShoulder));
		shoulderline->SetHeight(2 * shoulderline->GetHeight());
		shoulderline->SetID(IDC_BTN_DOWNSIDE_SHOULDERLINE);
		//auto *tabfold = downside->AddChildItem(new XTPPropertyGridItemButton(strTabReject));
		//tabfold->SetHeight(2 * tabfold->GetHeight());
		//tabfold->SetID(IDC_BTN_DOWNSIDE_FOLDING);
		//tabfold->SetHidden(TRUE);
		auto* Burr = downside->AddChildItem(new XTPPropertyGridItemButton(strBurr));
		Burr->SetHeight(2 * Burr->GetHeight());
		Burr->SetID(IDC_BTN_DOWNSIDE_BURR);

		auto *ngmark = downside->AddChildItem(new XTPPropertyGridItemButton(strNGmark));
		ngmark->SetHeight(2 * ngmark->GetHeight());
		ngmark->SetID(IDC_BTN_DOWNSIDE_NG_MARK);
		ngmark->SetHidden(TRUE);

		auto* chattering = downside->AddChildItem(new XTPPropertyGridItemButton(strChattering));
        chattering->SetHeight(2 * chattering->GetHeight());
        chattering->SetID(IDC_BTN_DOWNSIDE_CHATTERING);

		auto *measure = downside->AddChildItem(new XTPPropertyGridItemButton(strMeasure));
		measure->SetHeight(2 * measure->GetHeight());
		measure->SetID(IDC_BTN_DOWNSIDE_MEASURE);

		/// <UI 변경 >
		auto* Etc = downside->AddChildItem(new XTPPropertyGridItemButton(strEtc));
		Etc->SetHeight(2 * Etc->GetHeight());
		Etc->SetID(IDC_BTN_DOWNSIDE_ETC);
		Etc->SetHidden(TRUE);
		///
		/// <UI 변경 >
		auto* Surface = downside->AddChildItem(new XTPPropertyGridItemButton(strSurface));
		Surface->SetHeight(2 * Surface->GetHeight());
		Surface->SetID(IDC_BTN_DOWNSIDE_SURFACE);
		Surface->SetHidden(TRUE);
		
		CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
		
		if (parent->m_generationversion == Share::GenerationVersion::SplitterRemoved)
		{
			rollerMark->SetHidden(FALSE);
			uncoating->SetHidden(FALSE);
			tape->SetHidden(FALSE);
			Surface->SetHidden(FALSE);
			Scrab->SetHidden(FALSE);
			//Tab->SetHidden(FALSE);
			Etc->SetHidden(FALSE);
		}
		auto* Isi = downside->AddChildItem(new XTPPropertyGridItemButton(strISI));
		Isi->SetHeight(2 * Isi->GetHeight());
		Isi->SetID(IDC_BTN_DOWNSIDE_ISI);
#ifndef _ISI_
		Isi->SetHidden(TRUE);
		uncoating->SetHidden(TRUE);
		tape->SetHidden(TRUE);
		//Surface->SetHidden(TRUE);
		electrodetorn->SetHidden(TRUE);
		shoulderline->SetHidden(TRUE);
		Burr->SetHidden(TRUE);

		ngmark->SetHidden(TRUE);
		//measure->SetHidden(TRUE);
		Scrab->SetHidden(TRUE);
		Tab->SetHidden(TRUE);
		//Etc->SetHidden(TRUE);
#endif
		auto* blob = downside->AddChildItem(new XTPPropertyGridItemButton(strBlob));
		blob->SetHeight(2 * blob->GetHeight());
		blob->SetID(IDC_BTN_DOWNSIDE_BLOB);
		blob->SetHidden(FALSE);
		auto* deeplearning = downside->AddChildItem(new XTPPropertyGridItemButton(strDeeplearning));
		deeplearning->SetHeight(2 * deeplearning->GetHeight());
		deeplearning->SetID(IDC_BTN_DOWNSIDE_DEEPLEARNING);
		deeplearning->SetHidden(TRUE);
		auto* abnormalInspection = downside->AddChildItem(new XTPPropertyGridItemButton(strAbnormalInspection));
		abnormalInspection->SetHeight(2 * abnormalInspection->GetHeight());
		abnormalInspection->SetID(IDC_BTN_DOWNSIDE_ABNORMAL_INSPECTION);
		abnormalInspection->SetHidden(FALSE);

	}
	downside->Expand();
	downside->SetHidden(TRUE);
}

LRESULT CDlgInspectionMain::OnGridNotify(WPARAM wparam, LPARAM lparam)
{
	//	if (wparam != XTP_PGN_CUSTOM_BUTTON_CLICK)
	//	{
	//		return 0;
	//	}
	if (XTP_PGN_INPLACEBUTTONDOWN == wparam)
	{
		CXTPPropertyGridInplaceButton* pButton = (CXTPPropertyGridInplaceButton*)lparam;
		if (pButton->GetID() == IDC_PARA_TILT_EDGE_SHOWCURRENTSETTING)
			//OnBnClickedButtonShowEdgeSetting();//현식 나중에 체크 할 것
		return 0;
		
	}
	if (wparam == XTP_PGN_SELECTION_CHANGED || wparam == XTP_PGN_ITEMVALUE_CHANGED || wparam == XTP_PGN_EDIT_CHANGED || wparam == XTP_PGN_CUSTOM_BUTTON_CLICK)
	{
		;
	}
	else
	{
		return 0;
	}

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	if (pItem == NULL)
	{
		return 0;
	}
 	auto index = pItem->GetID();
	
	CXTPPropertyGridItem* pItemParent = (CXTPPropertyGridItem*)pItem->GetParentItem();
	UINT indexParent = 0;
	if (pItemParent != NULL)
	{
		indexParent = pItemParent->GetID();;
	}
	
	if(((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0/*쓰레기값이라고 판단하여 리턴*/) 
		return 0;
	auto* insp = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	//insp->ResetImageBuffer(true);
	if (IDC_BTN_CRITERIA_OPTIONS/*인덱스 시작*/ > index || END/*인덱스 종료*/ < index)
	{
		if(IDC_BTN_CRITERIA_OPTIONS/*인덱스 시작*/ > indexParent || END/*인덱스 종료*/ < indexParent)
		{
			return 0; //정의된 enum이외의 값은 리턴
		}
		
		if(index == 0)
		{
			index = indexParent;
		}
	}
	if (index > IDC_PARA_BUTTON_LINEDETECT_PARA_MEASURE_00 && index <= IDC_PARA_BUTTON_LINEDETECT_PARA_MEASURE_20) index = IDC_PARA_BUTTON_LINEDETECT_PARA_MEASURE_00;
	if (index > IDC_PARA_BUTTON_LINEDETECT_PARA_BLOB_00 && index <= IDC_PARA_BUTTON_LINEDETECT_PARA_BLOB_20) index = IDC_PARA_BUTTON_LINEDETECT_PARA_BLOB_00;
	if (index > IDC_PARA_BLOB_USE_INSPECTION_000 && index <= IDC_PARA_BLOB_USE_INSPECTION_399) index = IDC_PARA_BLOB_USE_INSPECTION_000;
	if (index > IDC_PARA_BLOB_DELETE_INTERSECT_BLOB_000 && index <= IDC_PARA_BLOB_DELETE_INTERSECT_BLOB_399) index = IDC_PARA_BLOB_DELETE_INTERSECT_BLOB_000;
	if (index > IDC_PARA_BLOB_THRESHOLD_000 && index <= IDC_PARA_BLOB_THRESHOLD_399) index = IDC_PARA_BLOB_THRESHOLD_000;
	if (index > IDC_PARA_BLOB_THRESHOLD2_000 && index <= IDC_PARA_BLOB_THRESHOLD2_399) index = IDC_PARA_BLOB_THRESHOLD2_000;
    if (index > IDC_PARA_BLOB_USE_MEAN_RANGE_THRESHOLD_000 && index <= IDC_PARA_BLOB_USE_MEAN_RANGE_THRESHOLD_399) index = IDC_PARA_BLOB_USE_MEAN_RANGE_THRESHOLD_000;
	if (index > IDC_PARA_BLOB_BLOB_MIN_SIZE_000 && index <= IDC_PARA_BLOB_BLOB_MIN_SIZE_399) index = IDC_PARA_BLOB_BLOB_MIN_SIZE_000;
	if (index > IDC_PARA_BLOB_MCGVSIZE_000 && index <= IDC_PARA_BLOB_MCGVSIZE_399) index = IDC_PARA_BLOB_MCGVSIZE_000;
	if (index > IDC_PARA_BLOB_THRESHOL_HOR_DIV_000 && index <= IDC_PARA_BLOB_THRESHOL_HOR_DIV_399) index = IDC_PARA_BLOB_THRESHOL_HOR_DIV_000;
	if (index > IDC_PARA_BLOB_THRESHOL_VER_DIV_000 && index <= IDC_PARA_BLOB_THRESHOL_VER_DIV_399) index = IDC_PARA_BLOB_THRESHOL_VER_DIV_000;
	if (index > IDC_PARA_BLOB_BLOB_MERGE_DISTANCE_000 && index <= IDC_PARA_BLOB_BLOB_MERGE_DISTANCE_399) index = IDC_PARA_BLOB_BLOB_MERGE_DISTANCE_000;
    if (index > IDC_PARA_BLOB_BLOB_INTERSECT_MERGE_000 && index <= IDC_PARA_BLOB_BLOB_INTERSECT_MERGE_399)index = IDC_PARA_BLOB_BLOB_INTERSECT_MERGE_000;
	if (index > IDC_PARA_BLOB_BLOB_MAX_COUNT_000 && index <= IDC_PARA_BLOB_BLOB_MAX_COUNT_399) index = IDC_PARA_BLOB_BLOB_MAX_COUNT_000;
	if (index > IDC_PARA_BLOB_BLOB_TIMEOUT_000 && index <= IDC_PARA_BLOB_BLOB_TIMEOUT_399) index = IDC_PARA_BLOB_BLOB_TIMEOUT_000;
	if (index > IDC_PARA_BLOB_BLOB_MIN_LENGTH_000 && index <= IDC_PARA_BLOB_BLOB_MIN_LENGTH_399) index = IDC_PARA_BLOB_BLOB_MIN_LENGTH_000;
	if (index > IDC_PARA_BLOB_BLOB_TYPE_IMAGE_000 && index <= IDC_PARA_BLOB_BLOB_TYPE_IMAGE_399) index = IDC_PARA_BLOB_BLOB_TYPE_IMAGE_000;
	if (index > IDC_PARA_BLOB_BLOB_TYPE_THRESHOLD_BRIGHT_000 && index <= IDC_PARA_BLOB_BLOB_TYPE_THRESHOLD_BRIGHT_399) index = IDC_PARA_BLOB_BLOB_TYPE_THRESHOLD_BRIGHT_000;
	if (index > IDC_PARA_BLOB_BLOB_TYPE_INSPECTION_000 && index <= IDC_PARA_BLOB_BLOB_TYPE_INSPECTION_399) index = IDC_PARA_BLOB_BLOB_TYPE_INSPECTION_000;
	if (index > IDC_PARA_BLOB_INSP_AREA_ROI_000 && index <= IDC_PARA_BLOB_INSP_AREA_ROI_399) index = IDC_PARA_BLOB_INSP_AREA_ROI_000;
	if (index > IDC_PARA_BLOB_TYPE_SET_INSP_ROI_000 && index <= IDC_PARA_BLOB_TYPE_SET_INSP_ROI_399) index = IDC_PARA_BLOB_TYPE_SET_INSP_ROI_000;
	if (index > IDC_PARA_BLOB_OFFSET_ROI_000 && index <= IDC_PARA_BLOB_OFFSET_ROI_399) index = IDC_PARA_BLOB_OFFSET_ROI_000;
	if (index > IDC_PARA_BLOB_USE_EQUAL_000 && index <= IDC_PARA_BLOB_USE_EQUAL_399) index = IDC_PARA_BLOB_USE_EQUAL_000;
	
	
	switch (index)
	{
	case IDC_PARA_BLOB_USE_INSPECTION_000:
		OnChangedBlobUseInspection(lparam, index);
		break;
    case IDC_PARA_BLOB_DELETE_INTERSECT_BLOB_000:
        break;
	case IDC_PARA_BLOB_THRESHOLD_000:
		OnChangedBlobThreshold(lparam, index);
		break;
	case IDC_PARA_BLOB_THRESHOLD2_000:
		OnChangedBlobThreshold2(lparam, index);
		break;
    case IDC_PARA_BLOB_USE_MEAN_RANGE_THRESHOLD_000:
        break;
	case IDC_PARA_BLOB_INSP_AREA_ROI_000:
	case IDC_PARA_BLOB_TYPE_SET_INSP_ROI_000:
	case IDC_PARA_BLOB_OFFSET_ROI_000:
		OnChangedBlobRoi(lparam, index);
		break;
	case IDC_PARA_ABNORMAL_OFFSET_ROI:
		//OnChangedAbnormalRoi(lparam, index);
		break;
    case IDC_PARA_CHATTERING_SET_ALIGN_ROI:
        OnChangedChatteringlRoi(index);
        break;
    case IDC_PARA_CHATTERING_SET_USER_ROI:
        OnBnClickedButtonParaCellAlignVhomeROI(index);
        break;
    case IDC_PARA_ABNORMAL_SET_USER_ROI:
        OnBnClickedButtonParaCellAlignVhomeROI(index);
        //OnChangedAbnormalRoi(lparam, index);    
        break;
	case IDC_PARA_ABNORMAL_GV:
    case IDC_PARA_ABNORMAL_GV_2:
		OnChangedAbnormalGV(lparam, index);
		break;
	case IDC_PARA_ABNORMAL_USE_INSPECTION:
		OnChangedAbnormalUseInspection(lparam, index);
		break;
	case IDC_PARA_BLOB_THRESHOL_HOR_DIV_000:
	case IDC_PARA_BLOB_THRESHOL_VER_DIV_000:
		OnChangedBlobRoiDiv(lparam, index);
		break;
	case IDC_PARA_BLOB_USE_EQUAL_000:
	case IDC_PARA_BLOB_USE_EQUAL_THRESHOLD_000:
		OnChangedBlobEqualUse(lparam, index);
		break;

	
	case IDC_BTN_CRITERIA_OPTIONS:
		if (true)
		{
			auto& criteriaList = Inspection::Accessor::Get().m_criteriaList;

			DlgCriteriaList dlg(criteriaList, this);
			if (dlg.DoModal() == IDOK)
			{
				criteriaList = dlg.m_info;
			}
		}
		break;

	case IDC_BTN_MEASURE_OFFSET:
		if (true)
		{
			auto& measureOffset = Inspection::Accessor::Get().m_measureOffsets;

			DlgMeasureOffsets dlg(measureOffset, this);
			if(dlg.DoModal() == IDOK)
			{
				measureOffset = dlg.m_measureOffset;
			}
		}
		break;


	case IDC_BTN_MERGE_OPTIONS:
		if (true)
		{
			auto& mergeOptions = Inspection::Accessor::Get().m_inspMergeOptions;
			DlgMergeOptions dlg(mergeOptions, this);
			if (dlg.DoModal() == IDOK)
			{
				mergeOptions.m_mergeLength.m_type = (InspMergeType)dlg.m_length;
				mergeOptions.m_mergeTopTilt.m_type = (InspMergeType)dlg.m_topTilt;
				mergeOptions.m_mergeBottomTilt.m_type = (InspMergeType)dlg.m_bottomTilt;
				mergeOptions.m_mergeMisalign.m_type = (InspMergeType)dlg.m_misalign;
			}
		}
		break;

	case IDC_BTN_ISI_CRITERIA:
		if (true)
		{
			auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
			/*CInspectionBlob* blob = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
			blob->m_para.m_vecBlobParaCondition.size();*/
			////tab 아일랜드
			CISICellAllDefect cell_defect;
			CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
			auto* inspection = (CInspectionBlob*)parent->GetProcessingItem(ProcItemCategory::Blob);
			long nAreaNum = inspection->m_para.m_vecBlobParaCondition.size();
			CDlgClassificationST dlg(isi_spec, this, &parent->m_CellAllDefect, nAreaNum);
			if (dlg.DoModal() == IDOK)
			{
				isi_spec = dlg.m_ISI_Spec_table;
				m_inspectionRoot->SetSurfaceOption(isi_spec.m_bUse);
			}
		}
		break;

	case IDC_BTN_ISI_DIMENSION_CRITERIA:
		if (true)
		{

		}
		break;

	case IDC_BTN_CELL_DESIGN:
		OnBnClickedButtonCellDesign();
		break;

	case IDC_BTN_UPSIDE_CONFIGURATION:
		OnBnClickedButtonUpsideTabConfiguration();
		break;
	case IDC_BTN_UPSIDE_CELL_ALIGN:
		OnBnClickedButtonUpsideTabCellAlign();
		break;
	case IDC_PARA_USEINTEKTABALIGN:
		UpdateCheckBoxChilds(lparam);
		break;
	case IDC_BTN_UPSIDE_UNCOATING:
		OnBnClickedButtonUpsideTabUncoating();
		break;
	case IDC_BTN_UPSIDE_SURFACE:
		OnBnClickedButtonUpsideSurface();
		break;
	case IDC_BTN_UPSIDE_SCRAB:
		OnBnClickedButtonUpsideScrab();
		break;
	case IDC_BTN_UPSIDE_TAB:
		OnBnClickedButtonUpsideTab();
		break;
	case IDC_BTN_UPSIDE_ETC:
		OnBnClickedButtonUpsideEtc();
		break;
	case IDC_BTN_UPSIDE_SHOULDERLINE:
		OnBnClickedButtonUpsideTabShoulderLine();
		break;
	case IDC_BTN_UPSIDE_TAPE:
		OnBnClickedButtonUpsideTabTape();
		break;
	case IDC_BTN_UPSIDE_ISI:
		OnBnClickedButtonUpsideISI();
		break;
	case IDC_BTN_UPSIDE_BLOB:
		OnBnClickedButtonUpsideBlob();
		break;
	case IDC_BTN_UPSIDE_DEEPLEARNING:
		OnBnClickedButtonUpsideDeeplearning();
		break;
	case IDC_BTN_UPSIDE_ABNORMAL_INSPECTION:
		OnBnClickedButtonUpsideAbnormalInspection();
		break;
	case IDC_BTN_UPSIDE_BURR:
		OnBnClickedButtonUpsideBurr();
		break;
	case IDC_BTN_UPSIDE_ELECTRODE_TORN:
		OnBnClickedButtonUpsideTabElectrodeTorn();
		break;
	case IDC_BTN_UPSIDE_NG_MARK:
		OnBnClickedButtonUpsideNgMark();
		break;
	case IDC_BTN_UPSIDE_MEASURE:
		OnBnClickedButtonUpsideMeasure();
		break;
    case IDC_BTN_UPSIDE_CHATTERING:
        OnBnClickedButtonUpsideChattering();
        break;
	case IDC_BTN_DOWNSIDE_CONFIGURATION:
		OnBnClickedButtonDownsideTabConfiguration();
		break;
	case IDC_BTN_DOWNSIDE_CELL_ALIGN:
		OnBnClickedButtonDownsideTabCellAlign();
		break;
	case IDC_BTN_DOWNSIDE_UNCOATING:
		OnBnClickedButtonDownsideTabUncoating();
		break;
	case IDC_BTN_DOWNSIDE_SURFACE:
		OnBnClickedButtonDownsideSurface();
		break;
	case IDC_BTN_DOWNSIDE_SCRAB:
		OnBnClickedButtonDownsideScrab();
		break;
	case IDC_BTN_DOWNSIDE_TAB:
		OnBnClickedButtonDownsideTab();
		break;
	case IDC_BTN_DOWNSIDE_ETC:
		OnBnClickedButtonDownsideEtc();
		break;
	case IDC_BTN_DOWNSIDE_ISI:
		OnBnClickedButtonDownsideISI();
		break;
	case IDC_BTN_DOWNSIDE_BLOB:
		OnBnClickedButtonDownsideBlob();
		break;
	case IDC_BTN_DOWNSIDE_DEEPLEARNING:
		OnBnClickedButtonDownsideDeeplearning();
		break;
	case IDC_BTN_DOWNSIDE_ABNORMAL_INSPECTION:
		OnBnClickedButtonDownsideAbnormalInspection();
		break;
	case IDC_BTN_DOWNSIDE_SHOULDERLINE:
		OnBnClickedButtonDownsideTabShoulderLine();
		break;
	case IDC_BTN_DOWNSIDE_TAPE:
		OnBnClickedButtonDownsideTabTape();
		break;
	case IDC_BTN_DOWNSIDE_BURR:
		OnBnClickedButtonDownsideBurr();
		break;
	case IDC_BTN_DOWNSIDE_ELECTRODE_TORN:
		OnBnClickedButtonDownsideTabElectrodeTorn();
		break;
	case IDC_BTN_DOWNSIDE_NG_MARK:
		OnBnClickedButtonDownsideNgMark();
		break;
	case IDC_BTN_DOWNSIDE_MEASURE:
		OnBnClickedButtonDownsideMeasure();
		break;
    case IDC_BTN_DOWNSIDE_CHATTERING:
        OnBnClickedButtonDownsideChattering();
        break;
	case IDC_PARA_TEST_ROI:
		OnBnClickedButtonRuluer();
		break;
	case IDC_PARA_TEST_EDGEDETECT:
		OnBnClickedUseEdgeDetect();
		break;
#ifdef _DRAW_DEFECT_
	case IDC_PARA_TEST_COLOR_B:
	case IDC_PARA_TEST_COLOR_D:
		OnBnClickedColor(lparam);
		break;
#endif
	case IDC_PARA_TEST_TILTTEST:
		OnBnClickedUseVhomeTiltTest();
		break;
	case IDC_PARA_CELL_DESIGN_SHOW_OVERLAY:
		OnBnClickedButtonCellDesignShowOverlay();
		break;
	case IDC_PARA_CELL_DESIGN_OVERLAY_X_AXIS_MOVE:
		OnBnClickedButtonCellDesignShowOverlay();
		break;
	case IDC_PARA_CELL_DESIGN_OVERLAY_Y_AXIS_MOVE:
		OnBnClickedButtonCellDesignShowOverlay();
		break;
	case IDC_PARA_SHOW_ROI:
	case IDC_PARA_MANUAL_TAB_ROI:
	case IDC_PARA_SHOW_ROI_MEASURE:
	case IDC_PARA_SHOW_ROI_BLOB:
	case IDC_PARA_SHOW_ROI_MSA:
		OnBnClickedButtonParaCellAlignVhomeROI(index);
		break;
    case IDC_PARA_BUTTON_COPY_BLOB_ROI:
    case IDC_PARA_BUTTON_COPY_MEASURE_ROI:
        OnBnClickedButtonCopyROI(index);
        break;
    case IDC_PARA_BUTTON_MSA_ALIGN:
        OnBnClickedButtonMSAAlign(index);
        break;
    case IDC_PARA_ALIGN_SELECT_INSPTYPE:
        OnBnClickSelectInspType(index);
        break;
    case IDC_PARA_BUTTON_AUTO_FIND_EDGEDETECT_ROI:
        OnBnClickedButtonAutoFindEdgeDetectROI(index);
        break;
    case IDC_PARA_MEASURE_SELECT_IMAGE_TYPE:
    case IDC_PARA_ALIGN_SELECT_IMAGE_TYPE:
    case IDC_PARA_ALIGN_SELECT_IMAGE_MIRROR:
        OnSelectImageType(lparam, index);
        break;
	case IDC_PARA_ALIGN_VHOME_THRESHOLD_LEFT:
	case IDC_PARA_ALIGN_VHOME_THRESHOLD_RIGHT:
	case IDC_PARA_BUTTON_RIGHT_CELLBOUNDARY_EDGE_PARA:
	case IDC_PARA_BUTTON_LEFT_CELLBOUNDARY_EDGE_PARA:
	case IDC_PARA_BUTTON_LEFTVHOME_EDGE_PARA:
	case IDC_PARA_BUTTON_RIGHTVHOME_EDGE_PARA:
	case IDC_PARA_INSULATEWIDTH_EDGEPARA:
	case IDC_PARA_OVERLAYWIDTH_EDGEPARA:
	case IDC_PARA_BOTOVERLAYWIDTH_EDGEPARA:
	case IDC_PARA_BUTTON_TABRiGHT_EDGE_PARA:
	case IDC_PARA_BUTTON_LINEDETECT_PARA_MEASURE_00:
	case IDC_PARA_BUTTON_LINEDETECT_PARA_BLOB_00:
	case IDC_PARA_BUTTON_ELECTRODESHOULDER_EDGE_PARA:
		OnBnClickedShowEdgeDetectParas(lparam, index);
		break;
	case IDC_BTN_BLOB_ADD_INSP_AREA:
		OnBnClickedAddDetailInspSetting(lparam, index);
		break;
	case IDC_BTN_BLOB_DELETE_INSP_AREA:
		OnBnClickedDeleteDetailInspSetting(lparam, index);
		break;
	case IDC_PARA_ISI_LEFTNONCOATINGTHRESHOLD:
	case IDC_PARA_ALIGN_IMAGE_USE_INDEX:
		OnChangedCellAlignImageUseIndex();
		break;
    case IDC_PARA_SWAP_MIN_AREA_BOX_XY:
        OnBnClickedSwapMinAreaBoxXY();
        break;

		//20220916
	case IDC_PARA_TILT_EDGE_AUTOMANUAL:
	case IDC_PARA_USE_WHITEST_ROLLER_IN_THE_BRIGHT_IMAGE:
	case IDC_PARA_USE_NONECOATING_AUTODETECT:
	case IDC_PARA_USE_VHOME_Y_COMPENSATE:
	case IDC_PARA_USE_VHOME_HEIGHT:
	case IDC_PARA_USE_VHOME_MISALIGN_DETAIL:
	case IDC_PARA_SHOULDERLINE_RANGE:
	case IDC_PARA_TAB_NGTAG_TABPRESSED:
	case IDC_PARA_ISI_INSULATEINSP:
	case IDC_PARA_ISI_OVERLAYINSP:
	case IDC_PARA_ISI_BOTOVERLAYINSP:
	case IDC_PARA_TAPE_BARCODEMARK:
	case IDC_PARA_ISI_NONCOATINGINSP:
		UpdateCheckBoxChilds(lparam);
		break;
	case IDC_PARA_TAPE_USE_DETECT_BRIGHT_TAPE:
	case IDC_PARA_TAPE_USE_DETECT_DARK_TAPE:
	case IDC_PARA_TAB_PERPECTLYFOLD:
		UpdateCheckBoxChilds(lparam);
		
	case IDC_PARA_TAB_FOLDING_THRESHOLD:
		OnChangedTabFoldingThreshold();
		break;

	case IDC_PARA_MEASURE_IMAGE_USE_INDEX:
		OnBnClickedButtonParaMeasureImageUseIndex();
		break;
	}

	return 0;
}

bool CDlgInspectionMain::IsCurrentParaChanged()
{
	if (m_curItemCategory == ProcItemCategory::END) return false;

	auto inspectionBase = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	Database currentDB_Common;
	Database currentDB_Module;
	m_inspectionRoot->LinkDataBaseCommon(true, currentDB_Common);
	inspectionBase->LinkDataBase(true, currentDB_Module);

	if (m_dbEdit_Common != currentDB_Common) return true;
	if (m_dbEdit_Module != currentDB_Module) return true;

	return false;
}

bool CDlgInspectionMain::IsSameModule(bool isUp, ProcItemCategory categoryIndex)
{

	if (isUp == m_curIsUp && categoryIndex == m_curItemCategory)
	{
		return true;
	}

	return false;
}


void CDlgInspectionMain::ChangeEditInspectionItem(bool isUp, ProcItemCategory categoryIndex)
{
	if (IsSameModule(isUp, categoryIndex)) return;
	if (!ParameterApplied()) return;

	m_oldIsUp = m_curIsUp;
	m_oldItemCategory = m_curItemCategory;

	m_curIsUp = isUp;
	m_curItemCategory = categoryIndex;

	UpdateEditInspectionItem();
}

void CDlgInspectionMain::UpdateEditInspectionItem(long nSide, bool bUpdate)
{
    if (nSide == 1)
    {
		if (m_vecsetupImage_Area.size() != 0)
		{
            m_inspectionImageBright = m_vecsetupImage_Area[nSide - 1].m_imageBright[m_curIsUp ? 0 : 1];
            m_inspectionImageDark = m_vecsetupImage_Area[nSide - 1].m_imageDark[m_curIsUp ? 0 : 1];
		}
		else
		{
            m_inspectionImageBright = m_setupImage.m_imageBright[m_curIsUp ? 0 : 1];
            m_inspectionImageDark = m_setupImage.m_imageDark[m_curIsUp ? 0 : 1];
		}
        
	}
	else
	{
        if (m_vecsetupImage_Area.size() != 0 && nSide != 0 && !(nSide > m_vecsetupImage_Area.size()))
        {
            m_inspectionImageBright = m_vecsetupImage_Area[nSide - 1].m_imageBright[m_curIsUp ? 0 : 1];
            m_inspectionImageDark = m_vecsetupImage_Area[nSide - 1].m_imageDark[m_curIsUp ? 0 : 1];
		}
		else
		{
            return;
		}
	}
	

	m_imageView->ImageOverlayClear();
	m_imageView->ImageOverlayShow();

	m_editTextInformation.SetWindowText(_T(""));
	m_debugInfoGrid->SetRowCount(1);

	auto inspectionBase = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	m_dbEdit_Common.Reset();
	m_dbEdit_Module.Reset();
	m_inspectionRoot->LinkDataBaseCommon(true, m_dbEdit_Common);
	inspectionBase->LinkDataBase(true, m_dbEdit_Module);

	if (bUpdate)
    {
        UpdateInspectionDebugInfo();
        UpdateInspectionParaGrid();
    }
	
	ShowISIOption(true);
	long nImageUseIndex = -1;

	switch (m_curItemCategory)
	{
	//case ProcItemCategory::configuration:	
	//{
	//	ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_CELL_ALIGN : DOWNSIDE_CELL_ALIGN]);
	//	break;
	//}
	case ProcItemCategory::cellAlign:
	{
		auto cellAlign = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

		nImageUseIndex = cellAlign->m_para.m_imageUseIndex;

		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_CELL_ALIGN : DOWNSIDE_CELL_ALIGN]);
		break;
	}
	//case ProcItemCategory::uncoating:		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_UNCOATING : DOWNSIDE_UNCOATING]);				break;

	//case ProcItemCategory::scrab:		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_SCRAB : DOWNSIDE_SCRAB]);				break;
	//case ProcItemCategory::surface:		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_SURFACE : DOWNSIDE_SURFACE]);				break;
	//case ProcItemCategory::tab:			ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_TAB : DOWNSIDE_TAB]);				break;
	//case ProcItemCategory::ETC:			
	//{
	//	ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_ETC : DOWNSIDE_ETC]);
	//	break;
	//}
	//case ProcItemCategory::shoulder:
	//{
	//	auto shoulder = (CInspectionShoulderLine*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	//	nImageUseIndex = shoulder->m_para.m_imageUseIndex;

	//	ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_SHOULDER_LINE : DOWNSIDE_SHOULDER_LINE]);
	//	break;
	//}
	//case ProcItemCategory::tape:			ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_TAPE : DOWNSIDE_TAPE]);						break;
	//case ProcItemCategory::ISI:			
	//{
	//	ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_TAPE : DOWNSIDE_TAPE]);
	//	ShowISIOption(true);
	//}
	//break;
	///*case ProcItemCategory::tabFolding:
	//{
	//	auto tabFolding = (CInspectionTabFolding*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	//	nImageUseIndex = tabFolding->m_para.m_imageUseIndex;

	//	ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_TAB_FOLDING : DOWNSIDE_TAB_FOLDING]);
	//	break;
	//}*/
	//case ProcItemCategory::burr:
	//{
	//	auto burr = (CInspectionBurr*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	//	nImageUseIndex = burr->m_para.m_imageUseIndex;

	//	ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_BURR : DOWNSIDE_BURR]);
	//	break;
	//}
	//case ProcItemCategory::electrodeTorn:	ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_ELECTRODE_TORN : DOWNSIDE_ELECTRODE_TORN]);	break;
	//case ProcItemCategory::ngMark:			ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_NG_MARK : DOWNSIDE_NG_MARK]);					break;
	case ProcItemCategory::measure:
	{
		auto measure = (CInspectionMeasure*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
		nImageUseIndex = measure->m_para.m_imageUseIndex;
		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_MISMATCH : DOWNSIDE_MISMATCH]);
		break;
	}
	case ProcItemCategory::Blob:
	{
		auto blob = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
		nImageUseIndex = 0; //blob->m_para.m_imageUseIndex; blob도 넣어야 하나?
		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_MISMATCH : DOWNSIDE_MISMATCH]);
		break;
	}
	case ProcItemCategory::deeplearning:
	{
		auto deeplearning = (CInspectionDeeplearning*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_MISMATCH : DOWNSIDE_MISMATCH]);
		break;
	}
	// 예진
	case ProcItemCategory::abnormalInspection:
	{
		auto abnormalInspection = (CInspectionAbnormal*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
		ShowTestTime(m_editTextInformation, m_TestTime[m_curIsUp ? UPSIDE_MISMATCH : DOWNSIDE_MISMATCH]);
		break;
	}
	}

	if (nImageUseIndex > 0)
		m_imageView->SetImage(m_inspectionImageDark);
	else
		m_imageView->SetImage(m_inspectionImageBright);

	UpdateInspectionItemGrid();
}

void CDlgInspectionMain::UpdateInspectionParaGrid()
{
	switch (m_curItemCategory)
	{
	case ProcItemCategory::cellAlign:		UpdateInspectionParaGridCellAlign();		break;
	case ProcItemCategory::deeplearning:	UpdateInspectionParaGridDeeplearning();		break;
	case ProcItemCategory::Chattering:		UpdateInspectionParaGridChattering();		break;
	
	//case ProcItemCategory::surface:			UpdateInspectionParaGridSurface();			break;
	case ProcItemCategory::abnormalInspection: UpdateInspectionParaGridAbnormalInspection(); break;
	case ProcItemCategory::measure:			UpdateInspectionParaGridMeasure();			break;
	case ProcItemCategory::Blob:			UpdateInspectionParaGridBlob();				break;
	}
}

void CDlgInspectionMain::OnBnClickedButtonCellDesign()					{	UpdateInspectionParaGridCellDesign();								}

void CDlgInspectionMain::OnBnClickedButtonUpsideTabConfiguration()		{	ChangeEditInspectionItem(true, ProcItemCategory::configuration);	}
void CDlgInspectionMain::OnBnClickedButtonUpsideTabCellAlign()			{	ChangeEditInspectionItem(true, ProcItemCategory::cellAlign);		}
void CDlgInspectionMain::OnBnClickedButtonUpsideTabUncoating()			{	ChangeEditInspectionItem(true, ProcItemCategory::uncoating);		}
void CDlgInspectionMain::OnBnClickedButtonUpsideTabTape()				{	ChangeEditInspectionItem(true, ProcItemCategory::tape);				}
void CDlgInspectionMain::OnBnClickedButtonUpsideSurface()				{	ChangeEditInspectionItem(true, ProcItemCategory::surface);			}
void CDlgInspectionMain::OnBnClickedButtonUpsideScrab()					{	ChangeEditInspectionItem(true, ProcItemCategory::scrab);			}
void CDlgInspectionMain::OnBnClickedButtonUpsideTab()					{	ChangeEditInspectionItem(true, ProcItemCategory::tab);				}
void CDlgInspectionMain::OnBnClickedButtonUpsideTabShoulderLine()		{	ChangeEditInspectionItem(true, ProcItemCategory::shoulder);			}
//void CDlgInspectionMain::OnBnClickedButtonUpsideTabFolding()			{	ChangeEditInspectionItem(true, ProcItemCategory::tabFolding);		}
void CDlgInspectionMain::OnBnClickedButtonUpsideTabElectrodeTorn()		{	ChangeEditInspectionItem(true, ProcItemCategory::electrodeTorn);	}
void CDlgInspectionMain::OnBnClickedButtonUpsideNgMark()				{	ChangeEditInspectionItem(true, ProcItemCategory::ngMark);			}
void CDlgInspectionMain::OnBnClickedButtonUpsideBurr()					{	ChangeEditInspectionItem(true, ProcItemCategory::burr);				}
void CDlgInspectionMain::OnBnClickedButtonUpsideMeasure()				{   ChangeEditInspectionItem(true, ProcItemCategory::measure);			}
void CDlgInspectionMain::OnBnClickedButtonUpsideChattering()			{   ChangeEditInspectionItem(true, ProcItemCategory::Chattering);		}
void CDlgInspectionMain::OnBnClickedButtonUpsideEtc()					{	ChangeEditInspectionItem(true, ProcItemCategory::ETC);				}
void CDlgInspectionMain::OnBnClickedButtonUpsideISI()					{   ChangeEditInspectionItem(true, ProcItemCategory::ISI); }
void CDlgInspectionMain::OnBnClickedButtonUpsideBlob()					{	ChangeEditInspectionItem(true, ProcItemCategory::Blob); }
void CDlgInspectionMain::OnBnClickedButtonUpsideDeeplearning()			{   ChangeEditInspectionItem(true, ProcItemCategory::deeplearning); }
void CDlgInspectionMain::OnBnClickedButtonUpsideAbnormalInspection()	{   ChangeEditInspectionItem(true, ProcItemCategory::abnormalInspection); }


void CDlgInspectionMain::OnBnClickedButtonDownsideTabConfiguration()	{	ChangeEditInspectionItem(false, ProcItemCategory::configuration);	}
void CDlgInspectionMain::OnBnClickedButtonDownsideTabCellAlign()		{	ChangeEditInspectionItem(false, ProcItemCategory::cellAlign);		}
void CDlgInspectionMain::OnBnClickedButtonDownsideTabUncoating()		{	ChangeEditInspectionItem(false, ProcItemCategory::uncoating);		}
void CDlgInspectionMain::OnBnClickedButtonDownsideTabTape()				{	ChangeEditInspectionItem(false, ProcItemCategory::tape);			}
void CDlgInspectionMain::OnBnClickedButtonDownsideSurface()				{	ChangeEditInspectionItem(false, ProcItemCategory::surface);			}
void CDlgInspectionMain::OnBnClickedButtonDownsideScrab()				{	ChangeEditInspectionItem(false, ProcItemCategory::scrab);			}
void CDlgInspectionMain::OnBnClickedButtonDownsideTab()					{	ChangeEditInspectionItem(false, ProcItemCategory::tab);				}
void CDlgInspectionMain::OnBnClickedButtonDownsideTabShoulderLine()		{	ChangeEditInspectionItem(false, ProcItemCategory::shoulder);		}
//void CDlgInspectionMain::OnBnClickedButtonDownsideTabFolding()		{	ChangeEditInspectionItem(false, ProcItemCategory::tabFolding);		}
void CDlgInspectionMain::OnBnClickedButtonDownsideTabElectrodeTorn()	{	ChangeEditInspectionItem(false, ProcItemCategory::electrodeTorn);	}
void CDlgInspectionMain::OnBnClickedButtonDownsideNgMark()				{	ChangeEditInspectionItem(false, ProcItemCategory::ngMark);			}
void CDlgInspectionMain::OnBnClickedButtonDownsideBurr()				{	ChangeEditInspectionItem(false, ProcItemCategory::burr);			}
void CDlgInspectionMain::OnBnClickedButtonDownsideMeasure()				{   ChangeEditInspectionItem(false, ProcItemCategory::measure);			}
void CDlgInspectionMain::OnBnClickedButtonDownsideChattering()			{   ChangeEditInspectionItem(false, ProcItemCategory::Chattering);		}
void CDlgInspectionMain::OnBnClickedButtonDownsideEtc()					{	ChangeEditInspectionItem(false, ProcItemCategory::ETC);				}
void CDlgInspectionMain::OnBnClickedButtonDownsideISI()					{   ChangeEditInspectionItem(false, ProcItemCategory::ISI); }
void CDlgInspectionMain::OnBnClickedButtonDownsideBlob()				{	ChangeEditInspectionItem(false, ProcItemCategory::Blob); }
void CDlgInspectionMain::OnBnClickedButtonDownsideDeeplearning()		{ ChangeEditInspectionItem(true, ProcItemCategory::deeplearning); }
void CDlgInspectionMain::OnBnClickedButtonDownsideAbnormalInspection()	{ ChangeEditInspectionItem(true, ProcItemCategory::abnormalInspection); }


void CDlgInspectionMain::OnOK()
{
	if (!ParameterAppliedWhenQuit()) return;

	CDialog::OnCancel();
}

void CDlgInspectionMain::OnCancel()
{
	if (!ParameterAppliedWhenQuit()) return;

	CDialog::OnCancel();
}

void CDlgInspectionMain::OnDestroy()
{
	CDialog::OnDestroy();
}

void CDlgInspectionMain::UpdateInspectionItemGrid()
{
	long isUp, inspectionType;
	long buttonEnd;
	//{{ Set Default Button Text
	for (long buttonID = IDC_BTN_UPSIDE_CONFIGURATION; buttonID <= IDC_BTN_DOWNSIDE_BLOB; buttonID++)
	{
		auto *item = (XTPPropertyGridItemButton *)m_inspectionItemGrid->FindItem(buttonID);
		CString str, str2;
		item->GetButtonText(str);
		int strID = str.Find(_T(" - "));
		if (strID != -1)
		{
			str2 = str.Mid(0, strID);
			item->SetButtonText(str2);
		}
	}
	//}} Set Default Button Text


	long buttonID = GetCurrentInspectionItemID();
	long currentType = GetCurrentInspectionItemType();

	bool bPolarCheck = ((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_POLAR))->GetCheck();
	bool bUpdownCheck = ((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_UPDOWN))->GetCheck();
	bool bTabCheck = ((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_TAB))->GetCheck();

	//long nUpCount = (IDC_BTN_UPSIDE_MEASURE - IDC_BTN_UPSIDE_CONFIGURATION + 1);
	long nUpCount = (IDC_BTN_UPSIDE_BLOB - IDC_BTN_UPSIDE_CONFIGURATION + 1);
	for (long buttonID = IDC_BTN_UPSIDE_CONFIGURATION; buttonID <= IDC_BTN_DOWNSIDE_BLOB; buttonID++)
	{
		isUp = 1 - ((buttonID - IDC_BTN_UPSIDE_CONFIGURATION) / nUpCount);
		if (isUp)
		{
			inspectionType = buttonID - IDC_BTN_UPSIDE_CONFIGURATION;
		}
		else
		{
			inspectionType = buttonID - IDC_BTN_DOWNSIDE_CONFIGURATION;
		}

		if (currentType != inspectionType)
		{
			continue;
		}

		if (bUpdownCheck)
		{
			if (m_curIsUp != isUp)
			{
				continue;
			}
		}
		auto* item = (XTPPropertyGridItemButton*)m_inspectionItemGrid->FindItem(buttonID);
		CString str, str2;
		item->GetButtonText(str);
		str += _T(" - ") + LANG_TEXT(LanguageDef::Copy);

		item->SetButtonText(str);
	}

	// current item
	{
		long buttonID = GetCurrentInspectionItemID();
		auto *item = (XTPPropertyGridItemButton *)m_inspectionItemGrid->FindItem(buttonID);
		CString str, str2;
		item->GetButtonText(str);
		int strID = str.Find(_T(" - "));
		if (strID != -1)
		{
			str2 = str.Mid(0, strID) + _T(" - ") + LANG_TEXT(LanguageDef::Current);
		}
		else
		{
			str2 = str + _T(" - ") + LANG_TEXT(LanguageDef::Current);
		}
		item->SetButtonText(str2);
	}
}

int CDlgInspectionMain::GetCurrentInspectionItemID()
{
	long nUpCount = (IDC_BTN_UPSIDE_BLOB - IDC_BTN_UPSIDE_CONFIGURATION + 1);
	long buttonID = IDC_BTN_UPSIDE_CONFIGURATION + (1 - m_curIsUp) * nUpCount;

	switch (m_curItemCategory)
	{
	case ProcItemCategory::configuration:	buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::cellAlign:		buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::uncoating:		buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::tape:			buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::surface:			buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::scrab:			buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::tab:				buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::electrodeTorn:	buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::shoulder:		buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::burr:			buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::ngMark:			buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::measure:			buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::ETC:				buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::ISI:				buttonID += (int)m_curItemCategory;	break;
	case ProcItemCategory::Blob:			buttonID += (int)m_curItemCategory;	break;
	}

	return buttonID;
}

int CDlgInspectionMain::GetCurrentInspectionItemType()
{
	switch (m_curItemCategory)
	{
	case ProcItemCategory::configuration:	return (int)m_curItemCategory;
	case ProcItemCategory::cellAlign:		return (int)m_curItemCategory;
	case ProcItemCategory::uncoating:		return (int)m_curItemCategory;
	case ProcItemCategory::tape:			return (int)m_curItemCategory;
	case ProcItemCategory::surface:			return (int)m_curItemCategory;
	case ProcItemCategory::scrab:			return (int)m_curItemCategory;
	case ProcItemCategory::tab:				return (int)m_curItemCategory;
	case ProcItemCategory::electrodeTorn:	return (int)m_curItemCategory;
	case ProcItemCategory::shoulder:		return (int)m_curItemCategory;
	case ProcItemCategory::burr:			return (int)m_curItemCategory;
	case ProcItemCategory::ngMark:			return (int)m_curItemCategory;
	case ProcItemCategory::measure:			return (int)m_curItemCategory;
	case ProcItemCategory::ETC:				return (int)m_curItemCategory;
	case ProcItemCategory::ISI:				return (int)m_curItemCategory;
	case ProcItemCategory::Blob:			return (int)m_curItemCategory;
	}

	return -1;
}

void CDlgInspectionMain::OnBnClickedCheckCopySameUpdown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateInspectionItemGrid();
}


void CDlgInspectionMain::OnBnClickedCheckCopySameTab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateInspectionItemGrid();
}


void CDlgInspectionMain::OnBnClickedCheckCopySamePolar()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateInspectionItemGrid();
}

bool CDlgInspectionMain::ParameterAppliedWhenQuit()
{
	// 선택 포커스를 강제를 Grid 가 아닌 곳으로 변경한다.
	// Grid Control 이 편집중인 내용을 강제로 마감하기 위함이다.

	::SetFocus(m_imageView->GetSafeHwnd());
	return ParameterApplied();
}

bool CDlgInspectionMain::ParameterApplied()
{
	if (!IsCurrentParaChanged())
	{
		return true;
	}

	//		long nResult = (long)AfxMessageBox(_T("Apply changed parameters to recipe?"), MB_YESNOCANCEL);
	long nResult;
	if (m_nLanguage == 1)
	{
		if (m_bParamForceSave)
		{
			m_bParamForceSave = false;
			SetTimer(0, 500, NULL);
		}
		nResult = ::MessageBox(NULL, _T("수정된 파라미터들을 Recipe 에 적용하시겠습니까?"), _T("검사 파라미터 저장"), MB_ICONWARNING | MB_YESNOCANCEL);
	}
	else if (m_nLanguage == 3)
	{
		if (m_bParamForceSave)
		{
			m_bParamForceSave = false;
			SetTimer(0, 500, NULL);
		}
		nResult = ::MessageBox(NULL, _T("确定要将修改好的参数应用到Recipe中吗？"), _T("检察保存参数"), MB_ICONWARNING | MB_YESNOCANCEL);
	}
	else
	{
		if (m_bParamForceSave)
		{
			m_bParamForceSave = false;
			SetTimer(0, 500, NULL);
		}
		nResult = ::MessageBox(NULL, _T("Do you want to apply the modified parameters to Recipe?"), _T("Save Inspection Param"), MB_ICONWARNING | MB_YESNOCANCEL);
	}

	if (nResult == IDYES)
	{
		m_needToRecipeSave = true;	// 변경사항이 있었음
		// 임시 파일 지정 저장
		JobFileSave();
		return true;
	}
	else if (nResult == IDNO)
	{
		OnBnClickedButtonSettingUndo();
		return true;
	}

	return false;
}

void CDlgInspectionMain::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 0)
	{
		KillTimer(0);

		if (m_nLanguage == 1)
		{
			HWND hwndMsgBox = ::FindWindow(NULL, _T("검사 파라미터 저장"));
			::PostMessage(hwndMsgBox, WM_COMMAND, IDYES, NULL);
		}
		else if (m_nLanguage == 3)
		{
			HWND hwndMsgBox = ::FindWindow(NULL, _T("检察保存参数"));
			::PostMessage(hwndMsgBox, WM_COMMAND, IDYES, NULL);
		}
		else
		{
			HWND hwndMsgBox = ::FindWindow(NULL, _T("Save Inspection Param"));
			::PostMessage(hwndMsgBox, WM_COMMAND, IDYES, NULL);
		}
	}
	

	CDialog::OnTimer(nIDEvent);
}

afx_msg LRESULT CDlgInspectionMain::OnUserMessageBoxYes(WPARAM wParam, LPARAM lParam)
{
	m_bParamForceSave = true;

	return 0;
}


void CDlgInspectionMain::OnClickedUseIntekTabAlign()
{
	
}

bool CDlgInspectionMain::JobFileSave()
{
	CString strPath;
	const TCHAR szFilters[] = _T("Job Files (*.job)|*.job||");
	CFileDialog dlg(TRUE, _T("Job"), _T("*.job"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);
	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();

		// Job File을 Open한다.
		CFileFind find;
		if (!find.FindFile(filePath))
		{
			AfxMessageBox(_T("Can't find recipe!"));
			return false;
		}

		Database dbJob;
		Inspection::Accessor::GetRoot().LinkDataBase(true, dbJob);
		if (!dbJob.Save(filePath))
		{
			return false;
		}

		AfxMessageBox(_T("Job File Save 완료"));
	}
}

//BOOL CDlgInspectionMain::SaveROIImage(IPVM::Image_8u_C1 image, IPVM::Rect rtROI, IPVM::Rect rtArea, int nNum)
//{
//	rtROI.MoveToX(rtROI.m_left - rtArea.m_left);
//	IPVM::Image_8u_C1 dstImage;
//	dstImage.Create(rtArea.Width(), rtArea.Height());
//	IPVM::ImageProcessing::Copy(image, rtArea, IPVM::Rect(dstImage), dstImage);
//
//	const long nBpp = dstImage.GetBitCountPerPixel();
//
//	CDC* pDC = this->GetDC();
//
//	CBitmap bmp;
//	CBitmap* pOldBmp;
//	CDC memDC;
//	BITMAP bm;
//
//	memDC.CreateCompatibleDC(pDC);
//	bmp.CreateCompatibleBitmap(pDC, dstImage.GetSizeX(), dstImage.GetSizeY());
//	bmp.GetObject(sizeof(BITMAP), &bm);
//
//	pOldBmp = (CBitmap*)memDC.SelectObject(&bmp);
//
//	CPen pen, * oldPen;
//	pen.CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
//	oldPen = memDC.SelectObject(&pen);
//
//	memDC.SetBkMode(TRANSPARENT);
//	memDC.SetBkColor(TRANSPARENT);
//
//	memDC.SetTextColor(RGB(0, 255, 0));
//
//
//	BYTE* pSmallImg = dstImage.GetMem();
//
//	if (bm.bmBitsPixel == nBpp)
//	{
//		bmp.SetBitmapBits(bm.bmWidthBytes * bm.bmHeight, pSmallImg);
//	}
//	else
//	{
//		//Bpp가 다를 경우 임시 Buffer 생성
//		BYTE* pImg = new BYTE[bm.bmWidthBytes * bm.bmHeight];
//
//		for (long y = 0; y < bm.bmHeight; y++)
//		{
//			for (long x = 0; x < bm.bmWidth; x++)
//			{
//				long nIndex = y * bm.bmWidthBytes + x * bm.bmBitsPixel / 8;
//				long nIndex2 = y * dstImage.GetWidthBytes() + x;
//				for (long i = 0; i < 3; i++)
//				{
//					pImg[nIndex + i] = pSmallImg[nIndex2];
//				}
//			}
//		}
//
//		bmp.SetBitmapBits(bm.bmWidthBytes * bm.bmHeight, pImg);
//		delete[]pImg;
//	}
//
//	memDC.SetTextColor(RGB(0, 0, 255));
//
//	memDC.MoveTo(rtROI.m_left, rtROI.m_top);
//	memDC.LineTo(rtROI.m_left, rtROI.m_bottom);
//	memDC.LineTo(rtROI.m_right, rtROI.m_bottom);
//	memDC.LineTo(rtROI.m_right, rtROI.m_top);
//	memDC.LineTo(rtROI.m_left, rtROI.m_top);
//
//	IPVM::Image_8u_C4 saveImage;
//	saveImage.Create(dstImage.GetSizeX(), dstImage.GetSizeY());
//	bmp.GetBitmapBits(bm.bmWidthBytes * bm.bmHeight, saveImage.GetRawMem());
//
//	CString strFolder = _T("ROI Image"), strFileName;
//
//	strFileName.Format(_T("Tab_Area_%d.jpg"), nNum);
//
//	CreateDirectory(strFolder, NULL);
//
//	CString strFilePath;
//	strFilePath.Format(_T("%s\\%s"), (LPCTSTR)strFolder, (LPCTSTR)strFileName);
//
//	IPVM::Status ret = saveImage.SaveJpeg(strFilePath);
//	//IPVM::Status ret = saveImage.SaveJpeg(_T("Test.jpg"));
//
//	BOOL bRes = TRUE;
//
//	memDC.SelectObject(pOldBmp);
//	memDC.DeleteDC();
//	pen.DeleteObject();
//
//	return bRes;
//}