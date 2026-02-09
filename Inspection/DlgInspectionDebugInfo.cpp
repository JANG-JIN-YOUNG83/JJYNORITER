// DlgInspectionDebugInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "InspectionRoot.h"
#include "DlgInspectionMain.h"
#include "ImageViewEx.h"

#include "Types/LineSeg_32f.h"
#include "afxdialogex.h"

#include "../Module/CustomControl/GridCtrl.h"
#include "../VisionModule/Language/LanguageText.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	GRID_NUM_X					1
#define	GRID_HEIGHT					25

// CDlgInspectionMain 대화 상자입니다.

void CDlgInspectionMain::OnSelChangedGridDebugInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto* selectInfo = (NM_GRIDVIEW *)pNMHDR;
	auto rowIndex = selectInfo->iRow;
	
	long index = rowIndex - 1;

	if (index < 0 || index >= m_debugInfo.GetItemCount())
	{
		return;
	}

	auto& debugItem = m_debugInfo.GetAt(index);

	debugItem.UpdateImageView(m_imageView);
	debugItem.UpdateProfileView(m_profileView);
	m_editTextInformation.SetWindowText(debugItem.m_resultText);
}

void CDlgInspectionMain::InitInspecitonDebugInfo()
{
	// Grid 가로 세로 개수 설정
//	long nDebugInfoNum = m_pDebugGroup->GetCount();
	long nDebugInfoNum = 9;
	m_debugInfoGrid->SetColumnCount(GRID_NUM_X);
	m_debugInfoGrid->SetRowCount(nDebugInfoNum + 1);
	m_debugInfoGrid->SetFixedRowCount(1);
	m_debugInfoGrid->SetColumnWidth(0, 445);

//	m_debugInfoGrid->SetColumnWidth(0, 425);


	// 0 번째 Row 에 각각의 Title 을 정의
	m_debugInfoGrid->SetItemText(0, 0, LANG_TEXT(LanguageDef::Name));
	m_debugInfoGrid->SetItemText(0, 1, LANG_TEXT(LanguageDef::Type));
	m_debugInfoGrid->SetRowHeight(0, GRID_HEIGHT);

	// Debug Info Grid 내용 채우기
	for (long i = 0; i < nDebugInfoNum; i++)
	{
		m_debugInfoGrid->SetRowHeight(i + 1, GRID_HEIGHT);
//		m_debugInfoGrid->SetItemText(i + 1, 0, m_pDebugGroup->Get(i)->GetName());
//		m_debugInfoGrid->SetItemText(i + 1, 1, m_pDebugGroup->Get(i)->GetDataType2String());
	}
}

void CDlgInspectionMain::UpdateInspectionDebugInfo()
{
	auto* inspection = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	m_debugInfo.Init(inspection->m_itemCategory);
	inspection->InitDebugInfo(&m_debugInfo);

	m_debugInfoGrid->SetRowCount(m_debugInfo.GetItemCount() + 1);

	// Debug Info Grid 내용 채우기
	for (long index = 0; index < m_debugInfo.GetItemCount(); index++)
	{
		m_debugInfoGrid->SetRowHeight(index + 1, GRID_HEIGHT);
		m_debugInfoGrid->SetItemText(index + 1, 0, m_debugInfo.GetAt(index).m_key);
		m_debugInfoGrid->SetItemBkColour(index + 1, 0, RGB(128, 128, 128));
	}
}

void CDlgInspectionMain::UpdateInspectionDebugInfoColor()
{
	for (long i = 0; i < m_debugInfo.GetItemCount(); i++)
	{
		if (m_debugInfo.GetAt(i).IsEmpty())
		{
			m_debugInfoGrid->SetItemBkColour(i + 1, 0, RGB(128, 128, 128));
		}
		else
		{
			m_debugInfoGrid->SetItemBkColour(i + 1, 0, RGB(255, 255, 255));
		}

		m_debugInfoGrid->RedrawCell(i + 1, 0);
	}
}

// Test 시간을 TextInfo Edit에 띄움
void CDlgInspectionMain::UpdateInspectionTextInfoTestTime(float elapTime, int timeUnit ,bool bTimeout) 
{
	CString strElapTime;

	if (timeUnit == UNIT_SECOND)
	{
		elapTime /= 1000;
		strElapTime.Format(_T("[Test Time] %.3f sec"), elapTime);
	}
	else if (timeUnit == UNIT_MILLI_SECOND)
	{
		strElapTime.Format(_T("[Test Time] %.3f ms"), elapTime);
	}
	else
	{
		elapTime *= 1000;
		strElapTime.Format(_T("[Test Time] %.3f μs"), elapTime);
	}

	/*if (true == bTimeout)
		strElapTime.Append(_T(" - TIMEOUT"));*/
	m_editTextInformation.SetWindowText(strElapTime);
	// 여기 아래 해줘야 할건 같은데 왜 하는지 모르겠다..
	if (m_curIsUp)
	{
		switch (m_curItemCategory)
		{
		case ProcItemCategory::cellAlign:		m_TestTime[UPSIDE_CELL_ALIGN] = strElapTime;		break;
		case ProcItemCategory::uncoating:		m_TestTime[UPSIDE_UNCOATING] = strElapTime;			break;
		case ProcItemCategory::shoulder:		m_TestTime[UPSIDE_SHOULDER_LINE] = strElapTime;		break;
		case ProcItemCategory::tape:			m_TestTime[UPSIDE_TAPE] = strElapTime;				break;
		case ProcItemCategory::tab:				m_TestTime[UPSIDE_TAB] = strElapTime;				break;
		//case ProcItemCategory::tabFolding:	m_TestTime[UPSIDE_TAB_FOLDING] = strElapTime;		break;
		case ProcItemCategory::burr:			m_TestTime[UPSIDE_BURR] = strElapTime;				break;
		case ProcItemCategory::electrodeTorn:	m_TestTime[UPSIDE_ELECTRODE_TORN] = strElapTime;	break;
		case ProcItemCategory::ngMark:			m_TestTime[UPSIDE_NG_MARK] = strElapTime;			break;
		case ProcItemCategory::measure:			m_TestTime[UPSIDE_MISMATCH] = strElapTime;			break;
		}			
	}
	else
	{
		switch (m_curItemCategory)
		{
		case ProcItemCategory::cellAlign:		m_TestTime[DOWNSIDE_CELL_ALIGN] = strElapTime;		break;
		case ProcItemCategory::uncoating:		m_TestTime[DOWNSIDE_UNCOATING] = strElapTime;		break;
		case ProcItemCategory::shoulder:		m_TestTime[DOWNSIDE_SHOULDER_LINE] = strElapTime;	break;
		case ProcItemCategory::tape:			m_TestTime[DOWNSIDE_TAPE] = strElapTime;			break;
		case ProcItemCategory::tab:				m_TestTime[DOWNSIDE_TAB] = strElapTime;				break;
		//case ProcItemCategory::tabFolding:		m_TestTime[DOWNSIDE_TAB_FOLDING] = strElapTime;		break;
		case ProcItemCategory::burr:			m_TestTime[DOWNSIDE_BURR] = strElapTime;			break;
		case ProcItemCategory::electrodeTorn:	m_TestTime[DOWNSIDE_ELECTRODE_TORN] = strElapTime;	break;
		case ProcItemCategory::ngMark:			m_TestTime[DOWNSIDE_NG_MARK] = strElapTime;			break;
		}
	}
}