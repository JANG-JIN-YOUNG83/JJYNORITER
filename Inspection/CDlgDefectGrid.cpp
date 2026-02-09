// CDlgDefectGrid.cpp: 구현 파일
//

#include "stdafx.h"
#include "Inspection.h"
#include "CDlgDefectGrid.h"
#include "afxdialogex.h"

#include "CDlgClassificationST.h"

#define	MESSAGE_DEFECT_GRID	(WM_USER + 401)

// CDlgDefectGrid 대화 상자


BOOL virtualDefectGrid(GV_DISPINFO* pDisplay, LPARAM lparam)
{
	CDlgDefectGrid* pParent = (CDlgDefectGrid*)lparam;

	return pParent->CallBackDefectGrid(pDisplay);
}


IMPLEMENT_DYNAMIC(CDlgDefectGrid, CDialogEx)

CDlgDefectGrid::CDlgDefectGrid(CWnd* pParent, CISICellAllDefect* pDefectInfo)
	: CDialogEx(IDD_DIALOG_DEFECT_GRID, pParent)
	, m_DefectInfo(*pDefectInfo)
{
	m_pDlgClassifcationST = (CDlgClassificationST*)pParent;
	m_ctrDefectGrid.SetVirtualMode(true);
	m_ctrDefectGrid.SetCallbackFunc(virtualDefectGrid, (LPARAM)this);
}

CDlgDefectGrid::~CDlgDefectGrid()
{
}

void CDlgDefectGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

enum enDefectColTitle
{
	DC_DEFECT = 0,
	DC_RESULT,
	DC_SIZE_X,
	DC_SIZE_Y,
	DC_AREA,
	DC_AREA_RATIO,
	
	DC_INSPECTION_AREA,
	DC_INSPECTION_IMAGE_TYPE,
	DC_RATIO_YX,
	DC_RATIO_XY,
	DC_THICKNESS,
	DC_LENGTH,
	DC_RATIO_LW,
	DC_RATIO_WL,
	DC_MIN,
	DC_AVG,
	DC_MAX,
    DC_BRIGHT_AVG,
    DC_DARK_AVG,
	DC_MASS_CENTER_GV_GV,
	DC_EDGE_ENERGY,
	DC_EDGE_ENERGY_WHITE,
    DC_EDGE_ENERGY_BLACK,
	DC_SECTION_THRESHOLD,
	DC_HOR,
	DC_VER,
	DC_RATIO_HV,
	DC_RATIO_VH,
	DC_RATIO_WHITE, // [sm]
	DC_RATIO_DARK,
    DC_OVER_200_GV_PX_COUNT,
	DC_DISTANCE_TO_EDGE,
    DC_IS_NEAR_EDGE,


	DC_DEFECT_BRIGHTNESS,
	DC_BLOB_THRESHOLD,
	DC_CATEGORY,
	
	DC_COMPACT,
	DC_ANGLE,
	DC_STD,

	
	DC_RATIO_AREA,
	
	//2024.10.21 JJS Edge_Sharpness 추가
    DC_EDGE_SHARPNESS,
    DC_EDGE_SHARPNESS_WHITE,
    DC_EDGE_SHARPNESS_BLACK,


};


BEGIN_MESSAGE_MAP(CDlgDefectGrid, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgDefectGrid 메시지 처리기


BOOL CDlgDefectGrid::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CRect rtDlg;
	GetWindowRect(rtDlg);
	m_ctrDefectGrid.Create(rtDlg, this, MESSAGE_DEFECT_GRID);
	initDefectGrid();
	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgDefectGrid::initDefectGrid()
{
	vecDefectTitle.push_back(_T("         DEFECT         "));
	vecDefectTitle.push_back(_T("Result        "));
	vecDefectTitle.push_back(_T("SIZE X(mm)"));
	vecDefectTitle.push_back(_T("SIZE Y(mm)"));
	vecDefectTitle.push_back(_T("Area(㎟)"));
	vecDefectTitle.push_back(_T("Area(%)"));
	vecDefectTitle.push_back(_T("Inspection Area"));
	vecDefectTitle.push_back(_T("InspectionImageType"));
	vecDefectTitle.push_back(_T("RATIO_YX"));
	vecDefectTitle.push_back(_T("RATIO_XY"));
	vecDefectTitle.push_back(_T("Width(mm)"));
	vecDefectTitle.push_back(_T("Length(mm)"));
	vecDefectTitle.push_back(_T("RATIO_T"));
	vecDefectTitle.push_back(_T("RATIO_L"));
	vecDefectTitle.push_back(_T("MIN(GV)"));
	vecDefectTitle.push_back(_T("AVG(GV)"));
	vecDefectTitle.push_back(_T("MAX(GV)"));
    vecDefectTitle.push_back(_T("BRIGHT AVG(GV)"));
    vecDefectTitle.push_back(_T("DARK AVG(GV)"));
	vecDefectTitle.push_back(_T("MASS_CENTER_GV_GV"));
	vecDefectTitle.push_back(_T("EDGE_ENERGY"));
	vecDefectTitle.push_back(_T("EDGE_ENERGY_WHITE"));
	vecDefectTitle.push_back(_T("EDGE_ENERGY_BLACK"));
	vecDefectTitle.push_back(_T("Section Th"));
	vecDefectTitle.push_back(_T("HOR"));
	vecDefectTitle.push_back(_T("VER"));
	vecDefectTitle.push_back(_T("RATIO_HV"));
	vecDefectTitle.push_back(_T("RATIO_VH"));
	vecDefectTitle.push_back(_T("RATIO_WHITE"));	// [sm]
	vecDefectTitle.push_back(_T("RATIO_DARK"));
	vecDefectTitle.push_back(_T("250 > CNT"));
	vecDefectTitle.push_back(_T("Dis 2 Edge(mm)"));
    vecDefectTitle.push_back(_T("Is Near Edge"));

	vecDefectTitle.push_back(_T("Defect Brightness"));
	vecDefectTitle.push_back(_T("Blob Threshold"));
	vecDefectTitle.push_back(_T("Category"));
	vecDefectTitle.push_back(_T("CMAPACT"));
	vecDefectTitle.push_back(_T("ANGLE"));
	vecDefectTitle.push_back(_T("STD"));
	vecDefectTitle.push_back(_T("RATIO_AREA"));
    vecDefectTitle.push_back(_T("EDGE_SHARPNESS"));
    vecDefectTitle.push_back(_T("EDGE_SHARPNESS_WHITE"));
    vecDefectTitle.push_back(_T("EDGE_SHARPNESS_BLACK"));




	long nRawNum = m_DefectInfo.m_vecCandidate.size() + 1/*TITLE*/;
	long nColNum = vecDefectTitle.size();

	m_ctrDefectGrid.SetColumnCount(nColNum);
	m_ctrDefectGrid.SetRowCount(nRawNum);
	m_ctrDefectGrid.SetEditable(FALSE);
	m_ctrDefectGrid.SetSingleColSelection(FALSE);
	m_ctrDefectGrid.SetSingleRowSelection(FALSE);
	m_ctrDefectGrid.SetFixedColumnSelection(FALSE);
	m_ctrDefectGrid.SetFixedRowSelection(TRUE);
	m_ctrDefectGrid.SetColumnResize(TRUE);
	m_ctrDefectGrid.SetRowResize(TRUE);
	m_ctrDefectGrid.SetFixedRowCount();
	m_ctrDefectGrid.SetFixedColumnCount();

	for (int col = 0; col < nColNum; col++)
	{
		if (vecDefectTitle.size() > col)
			m_ctrDefectGrid.SetItemText(0, col, vecDefectTitle[col]);
	}
	m_ctrDefectGrid.AutoSizeColumns();

	m_ctrDefectGrid.SetColumnWidth((enDefectColTitle)DC_DEFECT_BRIGHTNESS, 0);
	m_ctrDefectGrid.SetColumnWidth((enDefectColTitle)DC_BLOB_THRESHOLD, 0);
	m_ctrDefectGrid.SetColumnWidth((enDefectColTitle)DC_CATEGORY, 0);
	m_ctrDefectGrid.SetColumnWidth((enDefectColTitle)DC_COMPACT, 0);
	m_ctrDefectGrid.SetColumnWidth((enDefectColTitle)DC_ANGLE, 0);
	m_ctrDefectGrid.SetColumnWidth((enDefectColTitle)DC_STD, 0);
	m_ctrDefectGrid.SetColumnWidth((enDefectColTitle)DC_RATIO_AREA, 0);


}

void CDlgDefectGrid::SelectDefectIndex(long nIndex)
{
	m_nSelectDefectIndex = nIndex;
    //if (m_nSelectDefectIndex>0)
    {
        m_ctrDefectGrid.EnsureVisible(m_nSelectDefectIndex+1, TRUE);
        m_ctrDefectGrid.SetFocus();
    }
    
    
}

void CDlgDefectGrid::ReFresh()
{
	m_ctrDefectGrid.Invalidate();
}


BOOL CDlgDefectGrid::CallBackDefectGrid(GV_DISPINFO* pDisplay)
{
	int nrow = pDisplay->item.row - 1;
	int ncor = pDisplay->item.col;

	switch (pDisplay->item.row)
	{
	case 0:	//row : title 영역
		if (vecDefectTitle.size() > ncor)
			pDisplay->item.strText = vecDefectTitle[ncor];
		break;

	default: //row : data 영역 n개
		if (true)
		{
			if (m_DefectInfo.m_vecCandidate.size() <= nrow)
				break;

			DisplayDefectData(pDisplay, nrow, ncor);
		}
	}
	return true;
}

//void CDlgDefectGrid::UpdateDefectGrid()
//{
//	for (int row = 1; row < m_DefectInfoOrg.m_vecCandidate.size(); row++)
//	{
//		for (int col = 1; col < vecDefectTitle.size(); col++)
//		{
//
//			m_ctrDefectGrid.SetItemText(row, col, _T("TEST"));
//		}
//	}
//}

void CDlgDefectGrid::DisplayDefectData(GV_DISPINFO* pDisplay, long nrow, long ncor)
{

	if (II_RESULT_VALUE::REJECT == m_DefectInfo.m_vecCandidate[nrow].GetJudge())
	{
		pDisplay->item.crBkClr = RGB(230, 160, 160);
	}
	else
	{
		pDisplay->item.crBkClr = RGB(255, 255, 255);
	}

	if (0/*item*/ == ncor)
	{
		CString strName;
		strName.Format(_T("Defect[%d]"), nrow);
        DefectType dt = m_DefectInfo.m_vecCandidate[nrow].GetDefectResultCode();
        CString strdtName = GetDefectName(dt);
        strName = strName + _T("_") + strdtName;
		pDisplay->item.strText = strName;

		if (m_nSelectDefectIndex < m_DefectInfo.m_vecCandidate.size() && nrow == m_nSelectDefectIndex)
			pDisplay->item.crBkClr = RGB(128, 128, 255);
		else
			pDisplay->item.crBkClr = RGB(255, 255, 255);
	}
    else if (enDefectColTitle::DC_RESULT == ncor)
    {
        //m_vecLogicStep[index].strDefectName;
        DefectType defect = m_DefectInfo.m_vecCandidate[nrow].GetDefectResultCode();
        CString strDefect = GetDefectName(defect);
        pDisplay->item.strText = strDefect;
	}
	else if (enDefectColTitle::DC_SIZE_X/*SIZE X mm*/ == ncor)
	{
		pDisplay->item.strText = m_pDlgClassifcationST->Numeral2String(_T("%.3f"), m_DefectInfo.m_vecCandidate[nrow].SIZE_X_mm.GetValue());
	}
	else if (enDefectColTitle::DC_SIZE_Y/*SIZE Y mm*/ == ncor)
	{
		pDisplay->item.strText = m_pDlgClassifcationST->Numeral2String(_T("%.3f"), m_DefectInfo.m_vecCandidate[nrow].SIZE_Y_mm.GetValue());
	}
	else if (enDefectColTitle::DC_THICKNESS/*thiness*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].WIDTH_mm.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_LENGTH/*length*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].LENGTH_mm.GetValue(), _T("%.3f"), true, nrow);
	}
    else if (enDefectColTitle::DC_OVER_200_GV_PX_COUNT /*over 200GV px Count*/ == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<long>(
            pDisplay, m_DefectInfo.m_vecCandidate[nrow].OVER_200_GV_PIXEL_COUNT.GetValue(), _T("%d"), true, nrow);
	}
    else if (enDefectColTitle::DC_BRIGHT_AVG == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<long>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].BRIGHT_AVG_GV.GetValue(), _T("%d"), true, nrow);
	}
    else if (enDefectColTitle::DC_DARK_AVG == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<long>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].DARK_AVG_GV.GetValue(), _T("%d"), true, nrow);
    }
    else if (enDefectColTitle::DC_DISTANCE_TO_EDGE /*distance to edge*/ == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<float>(
            pDisplay, m_DefectInfo.m_vecCandidate[nrow].DISTANCE_TO_EDGE_mm.GetValue(), _T("%.3f"), true, nrow);
    }
    else if (enDefectColTitle::DC_IS_NEAR_EDGE /*is there edge*/ == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<long>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].IS_NEAR_EDGE.GetValue(), _T("%d"), true, nrow);
    }
    //else if (enDefectColTitle::DC_EDGE_ENERGY_SHARPNESS /*is there edge*/ == ncor)
    //{
    //    m_pDlgClassifcationST->DisplayItemNumber<long>(
    //        pDisplay, m_DefectInfo.m_vecCandidate[nrow].IS_NEAR_EDGE.GetValue(), _T("%d"), true, nrow);
    //}
	else if (enDefectColTitle::DC_INSPECTION_IMAGE_TYPE/*InspectionImageType*/ == ncor)
	{
		pDisplay->item.strText = enumtoString().InspectionImageType2String((enInspectionImageType)m_DefectInfo.m_vecCandidate[nrow].nInspectImage.GetValue());
		//pDisplay->item.crBkClr = BackGroundColor(nrow);
	}
	else if (enDefectColTitle::DC_DEFECT_BRIGHTNESS/*Defect Brightness*/ == ncor)
	{
		pDisplay->item.strText = enumtoString().DefectBrightness2String((enDefectBrightness)m_DefectInfo.m_vecCandidate[nrow].nDefectBrightness.GetValue());
	}
	else if (enDefectColTitle::DC_BLOB_THRESHOLD/*Blob Threshold*/ == ncor)
	{
	}
	else if (enDefectColTitle::DC_MIN/*MIN GV*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].MIN_GV.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_AVG/*AVG GV*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].AVG_GV.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_MAX/*MAX GV*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].MAX_GV.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_AREA_RATIO/*AREA*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].AREA_Ratio.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_AREA/*AREA*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber(pDisplay, m_DefectInfo.m_vecCandidate[nrow].AREA.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_LW/*RATIO YX*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_LW.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_WL/*RATIO XY*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_WL.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_COMPACT/*COMAPCT*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].COMPACT.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_ANGLE/*ANGLE*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].ANGLE.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_STD/*STD*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].STD.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_CATEGORY/*STD COMPARE OPERATION*/ == ncor)
	{
		pDisplay->item.strText = enumtoString().Category((enCategory)m_DefectInfo.m_vecCandidate[nrow].nCategory.GetValue());
		//pDisplay->item.crBkClr = BackGroundColor(nrow);
	}
	else if (enDefectColTitle::DC_INSPECTION_AREA/**/ == ncor)
	{
		pDisplay->item.strText = m_pDlgClassifcationST->InspectionArea2String((enInspectionArea)m_DefectInfo.m_vecCandidate[nrow].nInspectionArea.GetValue());
		//pDisplay->item.crBkClr = BackGroundColor(nrow);
	}
	else if (enDefectColTitle::DC_SECTION_THRESHOLD/*Blob Threshold*/ == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber(pDisplay, m_DefectInfo.m_vecCandidate[nrow].SECTION_THRESHOLD.GetValue(), _T("%d"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_YX == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_YX.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_XY == ncor)
	{
		m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_XY.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_WHITE == ncor) // [sm]
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_WHITE.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_DARK == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_DARK.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_EDGE_ENERGY == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].EDGE_ENERGY.GetValue(), _T("%.3f"), true, nrow);
	}
    else if (enDefectColTitle::DC_EDGE_ENERGY_WHITE == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].EDGE_ENERGY_WHITE.GetValue(), _T("%.3f"), true, nrow);
    }
    else if (enDefectColTitle::DC_EDGE_ENERGY_BLACK == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].EDGE_ENERGY_BLACK.GetValue(), _T("%.3f"), true, nrow);
    }
	else if (enDefectColTitle::DC_RATIO_AREA == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].AREA_Ratio.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_MASS_CENTER_GV_GV == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].MASS_CENTER_GV_GV.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_HOR == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].HOR.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_VER == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].VER.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_HV == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_HV.GetValue(), _T("%.3f"), true, nrow);
	}
	else if (enDefectColTitle::DC_RATIO_VH == ncor)
	{
	m_pDlgClassifcationST->DisplayItemNumber<float>(pDisplay, m_DefectInfo.m_vecCandidate[nrow].RATIO_VH.GetValue(), _T("%.3f"), true, nrow);
	}

    else if (enDefectColTitle::DC_EDGE_SHARPNESS == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<float>( pDisplay, m_DefectInfo.m_vecCandidate[nrow].EDGE_ENERGY_SHARPNESS.GetValue(), _T("%.3f"), true, nrow);
    }
    else if (enDefectColTitle::DC_EDGE_SHARPNESS_WHITE == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<float>(
            pDisplay, m_DefectInfo.m_vecCandidate[nrow].EDGE_ENERGY_SHARPNESS_WHITE.GetValue(), _T("%.3f"), true, nrow);
    }
    else if (enDefectColTitle::DC_EDGE_SHARPNESS_BLACK == ncor)
    {
        m_pDlgClassifcationST->DisplayItemNumber<float>(
            pDisplay, m_DefectInfo.m_vecCandidate[nrow].EDGE_ENERGY_SHARPNESS_BLACK.GetValue(), _T("%.3f"), true, nrow);
    }
	////defect info
	//if (nrow == m_nSelectDefectIndex)
	//{
	//	if (m_nSelectDefectIndex < m_DefectInfoProcess.m_vecCandidate.size())
	//		pDisplay->item.crBkClr = DefectBackGroundColor(m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetJudge());
	//}
}

void CDlgDefectGrid::MoveWindows()
{
	CRect rtDlg;
	GetWindowRect(rtDlg);
	m_ctrDefectGrid.MoveWindow(0, 0, rtDlg.Width(), rtDlg.Height());
}


void CDlgDefectGrid::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


BOOL CDlgDefectGrid::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (MESSAGE_DEFECT_GRID == wParam)
	{
		//grid event
		NM_GRIDVIEW* nmgv = (NM_GRIDVIEW*)lParam;

		int nCol = nmgv->iColumn;
		int nRaw = nmgv->iRow - 1;

		switch (nmgv->hdr.code)
		{
		case GVN_SELCHANGING:
			break;

		case GVN_SELCHANGED:
			break;

		case NM_DBLCLK:
		{
			m_nSelectDefectIndex = nRaw;
			m_pDlgClassifcationST->m_nSelectDefectIndex = m_nSelectDefectIndex;
			m_pDlgClassifcationST->OnBnClickedBtnReFresh();
			m_ctrDefectGrid.Invalidate();
		}
		break;

		case NM_CLICK:
			break;

		case NM_RCLICK:
			break;

		case GVN_BEGINLABELEDIT:
			break;

		case GVN_ENDLABELEDIT:
			break;

		default:
			break;
		}

	}

	return CDialogEx::OnNotify(wParam, lParam, pResult);
}
