// DlgSpecTable.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgSpecTable.h"
#include "afxdialogex.h"
#include "../Inspection/Accessor.h"
#include "DlgEditGrid.h"

// CDlgSpecTable 대화 상자
#define MESSAGE_GRID (WM_USER + 400)

CMutex m_mutex;

BOOL virtualGrid(GV_DISPINFO* pDisplay, LPARAM lparam)
{
    CDlgSpecTable* pParent = (CDlgSpecTable*)lparam;

    return pParent->CallBackGrid(pDisplay);
}


IMPLEMENT_DYNAMIC(CDlgSpecTable, CDialogEx)

CDlgSpecTable::CDlgSpecTable(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_SPEC_TABLE, pParent)
    , m_nRow(0)
    , m_nCol(0)
    , m_nSizeRow(0)
    , m_vecTogeter(nullptr)
    , m_use_mode(0)
{
    m_ctrGrid.SetVirtualMode(true);
    m_ctrGrid.SetCallbackFunc(virtualGrid, (LPARAM)this);

    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel5, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel5, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel5, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel5, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel5, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel5, true));

    m_mapSDCol.insert(std::make_pair(enColTitle::USE, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::ENABLE, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_X, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_X_CO, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_Y, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_Y_CO, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::ORAND, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::MINIMUM_SIZE_X, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::MINIMUM_SIZE_Y, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::COUNT, true));
    m_mapSDCol.insert(std::make_pair(enColTitle::PARENT, true));

    
    auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
    m_ISI_Spec_table = isi_spec;
}

CDlgSpecTable::~CDlgSpecTable()
{
}

void CDlgSpecTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB_SPEC_TABLE, m_ctrTab);
    DDX_Control(pDX, IDC_BUTTON_USE_ADMIN, m_btn_useAdmin);
    DDX_Control(pDX, IDC_BUTTON_USE_NORMAL, m_btn_useNormal);
}


BEGIN_MESSAGE_MAP(CDlgSpecTable, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_USE_NORMAL, &CDlgSpecTable::OnBnClickedButtonUseNormal)
ON_BN_CLICKED(IDC_BUTTON_USE_ADMIN, &CDlgSpecTable::OnBnClickedButtonUseAdmin)
ON_BN_CLICKED(IDC_BUTTON_INIT_SPEC, &CDlgSpecTable::OnBnClickedButtonInitSpec)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SPEC_TABLE, &CDlgSpecTable::OnTcnSelchangeTab)
ON_BN_CLICKED(IDOK, &CDlgSpecTable::OnBnClickedOk)
ON_BN_CLICKED(IDCANCEL, &CDlgSpecTable::OnBnClickedCancel)
ON_WM_SIZE()
ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CDlgSpecTable 메시지 처리기

BOOL CDlgSpecTable::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.

    GetClientRect(&m_dlgSize);

    /*CRect rtTab;
    m_ctrTab.MoveWindow(m_dlgSize.left, m_dlgSize.top, m_dlgSize.right * 0.2, 22);*/
    //m_ctrTab.GetWindowRect(rtTab);
    //ScreenToClient(rtTab);

    
    CRect rtGrid(m_dlgSize.left, m_dlgSize.top + 50, m_dlgSize.right, m_dlgSize.bottom * 0.9);

    m_ctrGrid.Create(rtGrid, this, MESSAGE_GRID);

    CRect rtTab(rtGrid.left, rtGrid.top - 30, rtGrid.left + 500, rtGrid.top);
    m_ctrTab.MoveWindow(rtTab);

    initGrid();

    
    m_ctrTab.InsertItem(0, _T("Normal Spec Table"));
    m_ctrTab.InsertItem(1, _T("Small Spec Table"));

    SetGridType(0);

    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgSpecTable::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (MESSAGE_GRID == wParam)
    {
        //grid event
        NM_GRIDVIEW* nmgv = (NM_GRIDVIEW*)lParam;

        int nCol = nmgv->iColumn;
        int nRaw = nmgv->iRow;

        nRaw = nRaw - 1;

        switch (nmgv->hdr.code)
        {
            case GVN_SELCHANGING:
                break;

            case GVN_SELCHANGED:
                break;

            case NM_DBLCLK:
            {
                NextOrAnd(nRaw, nCol);
                NextCompareOperator(nRaw, nCol);
                EditGridText(nRaw, nCol);
                NextInspectionImgType(nRaw, nCol);
                NextAreaOperator(nRaw, nCol);
                m_ctrGrid.Invalidate();
            }
            break;

            case NM_CLICK:
                TogeterValue(nRaw, nCol);
                m_ctrGrid.Invalidate();
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

void CDlgSpecTable::initGrid()
{
    //auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;

    long nRawNum = m_ISI_Spec_table.GetSpecCount() + 1;

    long nColNum = 66; // 예진 -- 컬럼 넣을 때 수정

    m_ctrGrid.SetColumnCount(nColNum);
    m_ctrGrid.SetRowCount(nRawNum);
    m_ctrGrid.SetEditable(FALSE);
    m_ctrGrid.SetSingleColSelection(FALSE);
    m_ctrGrid.SetSingleRowSelection(FALSE);
    m_ctrGrid.SetFixedColumnSelection(FALSE);
    m_ctrGrid.SetFixedRowSelection(TRUE);
    m_ctrGrid.SetColumnResize(TRUE);
    m_ctrGrid.SetRowResize(TRUE);
    m_ctrGrid.SetFixedRowCount();
    m_ctrGrid.SetFixedColumnCount();

    /*0*/ vecTitle.push_back(_T("         ITEM         "));
    /*1*/ vecTitle.push_back(_T("USE"));
    /*2*/ vecTitle.push_back(_T("ENABLE"));
    /*3*/ vecTitle.push_back(_T("SIZE X(mm)"));
    /*4*/ vecTitle.push_back(_T(">,<   "));
    /*5*/ vecTitle.push_back(_T("SIZE Y(mm)"));
    /*6*/ vecTitle.push_back(_T(">,<   "));
    /*7*/ vecTitle.push_back(_T("OR/AND"));
    /*8*/ vecTitle.push_back(_T("Area(㎟)"));
    /*9*/ vecTitle.push_back(_T(">,<   "));
    /*10*/ vecTitle.push_back(_T("Area(%)"));

    /*11*/ vecTitle.push_back(_T(">,<   "));
    /*12*/ vecTitle.push_back(_T("Small Insp"));
    /*12*/ vecTitle.push_back(_T("Level"));
    /*12*/ vecTitle.push_back(_T("Inspection Area"));
    /*13*/ vecTitle.push_back(_T("InspectionImageType"));
    /*14*/ vecTitle.push_back(_T("RATIO_XY(%)"));
    /*15*/ vecTitle.push_back(_T(">,<   "));
    /*16*/ vecTitle.push_back(_T("RATIO_YX(%)"));
    /*17*/ vecTitle.push_back(_T(">,<   "));
    /*18*/ vecTitle.push_back(_T("Width(mm)"));
    /*19*/ vecTitle.push_back(_T(">,<   "));
    /*20*/ vecTitle.push_back(_T("Length(mm)"));

    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("RATIO_WL"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("RATIO_LW"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("MIN(GV)"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("AVG(GV)"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("MAX(GV)"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Mass center GV"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Edge Energy"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Section Threshold"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("HOR"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("VER"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("RATIO HV"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("RATIO VH"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Ratio White"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Ratio Black"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Defect Brightness"));
    /**/ vecTitle.push_back(_T("Blob Threshold"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Category"));
    /**/ vecTitle.push_back(_T("CMAPACT"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("ANGLE"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("STD"));
    /**/ vecTitle.push_back(_T(">,<   "));
    /**/ vecTitle.push_back(_T("Minimum Size X"));
    /**/ vecTitle.push_back(_T("Minimum Size Y"));
    /**/ vecTitle.push_back(_T("COUNT"));
    /**/ vecTitle.push_back(_T("Parent              "));
    /**/ vecTitle.push_back(_T("                         "));
    /**/ vecTitle.push_back(_T("                         "));
    /**/ vecTitle.push_back(_T("                         "));
    /**/ vecTitle.push_back(_T("                         "));
    /**/ vecTitle.push_back(_T("                         "));

    m_ctrGrid.SetColumnWidth((enColTitle)DEFECT_BRIGHTNESS, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)BLOB_THRESHOLD, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)BLOB_THRESHOLD_CO, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)CATEGORY, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)COMPACT, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)COMPACT_CO, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)ANGLE, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)ANGLE_CO, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)STD, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)STD_CO, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)SMALL_INSP, 0);
    m_ctrGrid.SetColumnWidth((enColTitle)SMALL_INSP_LEVEL, 0);

    LinkSpecAndTable();

    for (int col = 1; col < nColNum; col++)
    {
        if (vecTitle.size() > col)
        {
            m_ctrGrid.SetItemText(0, col, vecTitle[col]);
        }
    }

    m_ctrGrid.AutoSizeColumns();

    for (int col = 0; col < nColNum; col++)
    {
        m_vecnSizeCol.push_back(m_ctrGrid.GetColumnWidth(col));
    }
    m_nSizeRow = m_ctrGrid.GetRowHeight(0);

    
}

BOOL CDlgSpecTable::CallBackGrid(GV_DISPINFO* pDisplay)
{ // 현식 색 넣기
    int nrow = pDisplay->item.row - 1;
    int ncor = pDisplay->item.col;

    switch (pDisplay->item.row)
    {
        case 0: //row : title 영역
            if (vecTitle.size() > ncor)
                pDisplay->item.strText = vecTitle[ncor];
            break;

        default: //row : data 영역 n개
            if (true)
            {
                if (vecItem.size() <= nrow)
                    break;

                DisplayData(pDisplay, nrow, ncor);
            }
    }

    if (0 < ncor)
    {
        // 0 : item 은 건들지 않는다.
        if (_T("") == pDisplay->item.strText)
        {
            pDisplay->item.crBkClr = RGB(240, 240, 240);
        }

        if (enColTitle::USE == ncor || enColTitle::ENABLE == ncor || enColTitle::ORAND == ncor
            || enColTitle::SIZE_X_CO == ncor || enColTitle::SIZE_Y_CO == ncor || enColTitle::RATIO_LW_CO == ncor
            || enColTitle::COMPACT_CO == ncor || enColTitle::SIZE_X == ncor || enColTitle::SIZE_Y == ncor
            || enColTitle::COMPACT == ncor || enColTitle::RATIO_LW == ncor || enColTitle::RATIO_WL == ncor
            || enColTitle::RATIO_YX == ncor || enColTitle::RATIO_XY == ncor || enColTitle::AREA_Ratio == ncor
            || enColTitle::LENGTH == ncor || enColTitle::WIDTH == ncor || enColTitle::WIDTH_CO == ncor ||
            //예진아 CO 넣어서 색 나오게 바꿔 주삼
            enColTitle::LENGTH_CO == ncor || enColTitle::AREA_Ratio_CO == ncor || enColTitle::RATIO_WL_CO == ncor
            || enColTitle::RATIO_XY_CO == ncor || enColTitle::RATIO_YX_CO == ncor || enColTitle::ANGLE_CO == ncor
            || enColTitle::STD_CO == ncor || enColTitle::ANGLE == ncor || enColTitle::STD == ncor
            || enColTitle::INSPECTION_IMAGE_TYPE == ncor || enColTitle::DEFECT_BRIGHTNESS == ncor
            || enColTitle::BLOB_THRESHOLD == ncor || enColTitle::BLOB_THRESHOLD_CO == ncor
            || enColTitle::CATEGORY == ncor || enColTitle::MIN == ncor || enColTitle::MIN_CO == ncor
            || enColTitle::AVG == ncor || enColTitle::AVG_CO == ncor || enColTitle::MAX == ncor 
            || enColTitle::MAX_CO == ncor || enColTitle::BRIGHT_AVG == ncor || enColTitle::BRIGHT_AVG_CO == ncor || enColTitle::DARK_AVG == ncor
            || enColTitle::DARK_AVG_CO == ncor || enColTitle::MINIMUM_SIZE_X == ncor || enColTitle::MINIMUM_SIZE_Y == ncor
            || enColTitle::INSPECTION_AREA == ncor || enColTitle::COUNT == ncor || enColTitle::RATIO_WHITE == ncor
            || enColTitle::RATIO_WHITE_CO == ncor || enColTitle::RATIO_DARK == ncor || enColTitle::RATIO_DARK_CO == ncor
            || enColTitle::EDGE_ENERGY == ncor || enColTitle::EDGE_ENERGY_CO == ncor
            || enColTitle::SECTION_THRESHOLD == ncor || enColTitle::SECTION_THRESHOLD_CO == ncor
            || enColTitle::MASS_CENTERGV == ncor || enColTitle::MASS_CENTERGV_CO == ncor || enColTitle::HOR == ncor
            || enColTitle::HOR_CO == ncor || enColTitle::VER == ncor || enColTitle::VER_CO == ncor
            || enColTitle::RATIO_HV == ncor || enColTitle::RATIO_HV_CO == ncor || enColTitle::AREA == ncor
            || enColTitle::AREA_CO == ncor || enColTitle::RATIO_VH == ncor || enColTitle::RATIO_VH_CO == ncor
            || enColTitle::SMALL_INSP == ncor || enColTitle::SMALL_INSP_LEVEL == ncor || enColTitle::PARENT == ncor || enColTitle::OVER_GV_PX_COUNT == ncor || enColTitle::OVER_GV_PX_COUNT_CO == ncor 
            || enColTitle::DIS_TO_EDGE_mm == ncor || enColTitle::DIS_TO_EDGE_mm_CO == ncor 
            || enColTitle::DIS_TO_EDGE_2_mm == ncor || enColTitle::DIS_TO_EDGE_2_mm_CO == ncor
            || enColTitle::IS_NEAR_EDGE==ncor||enColTitle::IS_NEAR_EDGE_CO==ncor)

        {
        }
        else
        {
            pDisplay->item.crBkClr = RGB(240, 240, 240);
        }
    }

    return true;
}


void CDlgSpecTable::DisplayData(GV_DISPINFO* pDisplay, long nrow, long ncor)
{
    if (0 /*item*/ == ncor)
    {
        CString strName = GetDefectName(vecItem[nrow].first);
        pDisplay->item.strText = strName;

        /*COLORREF RGB_BK;
        BOOL binclude = BackGroundColor(nrow, RGB_BK);
        pDisplay->item.crBkClr = RGB_BK;*/

        ////defect info
        //if (0 <= m_nSelectDefectIndex && true == binclude)
        //{
        //    if (m_nSelectDefectIndex < m_DefectInfoProcess.m_vecCandidate.size()
        //        && vecItem[nrow].first
        //            == m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetDefectResultCode())
        //        pDisplay->item.crBkClr
        //            = DefectBackGroundColor(m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetJudge());
        //}
    }
    // --
    else if (enColTitle::USE == ncor)
    {
        pDisplay->item.strText = Enable2String(vecItem[nrow].second->USE.GetValue());
    }
    else if (enColTitle::ENABLE == ncor)
    {
        pDisplay->item.strText = Enable2String(vecItem[nrow].second->ENABLE.GetValue());
        //pDisplay->item.crBkClr = BackGroundColor(nrow);
    }
    else if (enColTitle::SIZE_X == ncor)
    {
        DisplayItemNumber<float>(
            pDisplay, vecItem[nrow].second->SIZE_X_mm.GetValue(), _T("%.2f"), vecItem[nrow].second->GetUseSize(), nrow);
    }
    else if (enColTitle::SIZE_X_CO /**/ == ncor && DefectType::Defect_Info != vecItem[nrow].first)
    {
        DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->SIZEX_COMPARE_OPERATOR.GetValue(),
            vecItem[nrow].second->GetUseSize(), nrow);
    }
    else if (enColTitle::SIZE_Y /*SIZE Y mm*/ == ncor)
    {
        DisplayItemNumber<float>(
            pDisplay, vecItem[nrow].second->SIZE_Y_mm.GetValue(), _T("%.2f"), vecItem[nrow].second->GetUseSize(), nrow);
    }
    else if (enColTitle::SIZE_Y_CO /**/ == ncor && DefectType::Defect_Info != vecItem[nrow].first)
    {
        DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->SIZEY_COMPARE_OPERATOR.GetValue(),
            vecItem[nrow].second->GetUseSize(), nrow);
    }
    else if (enColTitle::ORAND /**/ == ncor && DefectType::Defect_Info != vecItem[nrow].first)
    {
        if (vecItem[nrow].second->GetUseSize())
        {
            pDisplay->item.strText = enumtoString().OrAnd2String(vecItem[nrow].second->ORAND.GetValue());
        }
    }
    //else if (enColTitle::WIDTH /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->WIDTH_mm.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseWidth(), nrow);
    //}
    //else if (enColTitle::WIDTH_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->WIDTH_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseWidth(), nrow);
    //}
    //else if (enColTitle::LENGTH /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->LENGTH_mm.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseLength(), nrow);
    //}
    //else if (enColTitle::LENGTH_CO /**/ == ncor && DefectType::Defect_Info != vecItem[nrow].first)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->LENGTH_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseLength(), nrow);
    //}
    //else if (enColTitle::SMALL_INSP == ncor)
    //{
    //    pDisplay->item.strText = Enable2String(vecItem[nrow].second->SMALL_INSP.GetValue());
    //}
    //else if (enColTitle::SMALL_INSP_LEVEL == ncor)
    //{
    //    if (vecItem[nrow].second->SMALL_INSP)
    //    {
    //        pDisplay->item.strText
    //            = enumtoString().SmallInsp2String((enSmallInsplevel)vecItem[nrow].second->SMALL_INSP_LEVEL.GetValue());
    //    }
    //}
    else if (enColTitle::INSPECTION_IMAGE_TYPE /*InspectionImageType*/ == ncor)
    {
        pDisplay->item.strText = enumtoString().InspectionImageType2String(
            (enInspectionImageType)vecItem[nrow].second->INSPECTION_IMAGE_TYPE.GetValue());
        //pDisplay->item.crBkClr = BackGroundColor(nrow);
    }
    else if (enColTitle::DEFECT_BRIGHTNESS /**/ == ncor)
    {
        pDisplay->item.strText = enumtoString().DefectBrightness2String(
            (enDefectBrightness)vecItem[nrow].second->nDefectBrightness.GetValue());
        //pDisplay->item.crBkClr = BackGroundColor(nrow);
    }
    else if (enColTitle::BLOB_THRESHOLD /*Blob Threshold*/ == ncor)
    {
        //pDisplay->item.crBkClr = BackGroundColor(nrow);
    }
    else if (enColTitle::BLOB_THRESHOLD_CO /**/ == ncor)
    {
        //pDisplay->item.strText = CompareOperator2String((enCompareOperator)vecItem[nrow].second->MIN_GV_COMPARE_OPERATOR.GetValue());
        //pDisplay->item.crBkClr = BackGroundColor(nrow);
    }
    //else if (enColTitle::MIN /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->MIN_GV.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseMinGV(), nrow);
    //}
    //else if (enColTitle::MIN_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->MIN_GV_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseMinGV(), nrow);
    //}
    //else if (enColTitle::AVG /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->AVG_GV.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseAvgGV(), nrow);
    //}
    //else if (enColTitle::AVG_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->AVG_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseAvgGV(), nrow);
    //}
    //else if (enColTitle::MAX /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->MAX_GV.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseMaxGV(), nrow);
    //}
    //else if (enColTitle::MAX_CO /**/ == ncor && DefectType::Defect_Info != vecItem[nrow].first)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->MAX_GV_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseMaxGV(), nrow);
    //}
    //else if (enColTitle::AREA_Ratio /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->AREA_Ratio.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseArea_RAtio(), nrow);
    //}
    //else if (enColTitle::AREA_Ratio_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->AREA_RATIO_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseArea_RAtio(), nrow);
    //}
    //else if (enColTitle::RATIO_LW /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_LW.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioYX(), nrow);
    //}
    //else if (enColTitle::RATIO_LW_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_LW_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioYX(), nrow);
    //}
    //else if (enColTitle::RATIO_WL /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_WL.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioXY(), nrow);
    //}
    //else if (enColTitle::RATIO_WL_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_WL_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioXY(), nrow);
    //}
    //else if (enColTitle::RATIO_YX /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_YX.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioYX(), nrow);
    //}
    //else if (enColTitle::RATIO_YX_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_YX_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioYX(), nrow);
    //}
    //else if (enColTitle::RATIO_XY /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_XY.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioXY(), nrow);
    //}
    //else if (enColTitle::RATIO_XY_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_XY_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioXY(), nrow);
    //}
    //else if (enColTitle::COMPACT /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->COMPACT.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseCompact(), nrow);
    //}
    //else if (enColTitle::COMPACT_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->COMPACT_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseCompact(), nrow);
    //}
    ////else if (enColTitle::RATIO_WHITE == ncor)
    ////{
    ////	DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_WHITE.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseRatioXY(), nrow);
    ////}
    //else if (enColTitle::ANGLE /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->ANGLE.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseAngle(), nrow);
    //}
    //else if (enColTitle::ANGLE_CO == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->ANGLE_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseAngle(), nrow);
    //}
    //else if (enColTitle::STD == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->STD.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseStd(), nrow);
    //}
    //else if (enColTitle::STD_CO == ncor && DefectType::Defect_Info != vecItem[nrow].first)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->STD_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseStd(), nrow);
    //}
    //else if (enColTitle::CATEGORY == ncor)
    //{
    //    pDisplay->item.strText = enumtoString().Category((enCategory)vecItem[nrow].second->nCategory.GetValue());
    //    //pDisplay->item.crBkClr = BackGroundColor(nrow);
    //}
    //// 예진
    //else if (enColTitle::OVER_GV_PX_COUNT /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->OVER_GV_PX_COUNT.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseOverGVPxCount(), nrow);
    //}
    //else if (enColTitle::DIS_TO_EDGE_mm /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->DISTANCE_TO_EDGE_mm.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseDis2Edge(), nrow);
    //}
    //else if (enColTitle::DIS_TO_EDGE_2_mm /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->DISTANCE_TO_EDGE_2_mm.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseDis2Edge_2(), nrow);
    //}
    //else if (enColTitle::IS_NEAR_EDGE /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->IS_NEAR_EDGE.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseIsNearEdge(), nrow);
    //}
    //else if (enColTitle::INSPECTION_AREA /**/ == ncor)
    //{
    //    pDisplay->item.strText
    //        = InspectionArea2String((enInspectionArea)vecItem[nrow].second->nInspectionArea.GetValue());
    //    //pDisplay->item.crBkClr = BackGroundColor(nrow);
    //}
    //else if (enColTitle::SECTION_THRESHOLD == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->SECTION_THRESHOLD.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseSectionThreshold(), nrow);
    //}
    //else if (enColTitle::SECTION_THRESHOLD_CO == ncor)
    //{
    //    DisplayItemCompare(pDisplay,
    //        (enCompareOperator)vecItem[nrow].second->SECTION_THRESHOLD_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseSectionThreshold(), nrow);
    //}
    //else if (enColTitle::MINIMUM_SIZE_X == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->MINIMUM_SIZEX_mm.GetValue(), _T("%.1f"), true, nrow);
    //}
    //else if (enColTitle::MINIMUM_SIZE_Y == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->MINIMUM_SIZEY_mm.GetValue(), _T("%.1f"), true, nrow);
    //}
    //else if (enColTitle::COUNT == ncor)
    //{
    //    DisplayItemNumber<long>(
    //        pDisplay, vecItem[nrow].second->COUNT.GetValue(), _T("%d"), vecItem[nrow].second->GetUseCount(), nrow);
    //}
    //else if (enColTitle::RATIO_WHITE == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_WHITE.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioWhite(), nrow);
    //}
    //else if (enColTitle::RATIO_DARK == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_DARK.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioDark(), nrow);
    //}
    //else if (enColTitle::RATIO_WHITE_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_WHITE_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioWhite(), nrow);
    //}
    //else if (enColTitle::RATIO_DARK_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_DARK_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioDark(), nrow);
    //}
    //else if (enColTitle::EDGE_ENERGY /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->EDGE_ENERGY.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseEdgeEnergy(), nrow);
    //}
    //else if (enColTitle::EDGE_ENERGY_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->EDGE_ENERGY_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseEdgeEnergy(), nrow);
    //}
    //else if (enColTitle::AREA /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->AREA.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseArea(), nrow);
    //}
    //else if (enColTitle::AREA_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->AREA_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseArea(), nrow);
    //}
    //else if (enColTitle::MASS_CENTERGV /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->MASS_CENTER_GV_GV.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseMassCenterGV(), nrow);
    //}
    //else if (enColTitle::MASS_CENTERGV_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay,
    //        (enCompareOperator)vecItem[nrow].second->MASS_CENTER_GV_GV_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseMassCenterGV(), nrow);
    //}

    //// [sm]
    //else if (enColTitle::HOR /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->HOR.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseHor(), nrow);
    //}
    //else if (enColTitle::HOR_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->HOR_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseHor(), nrow);
    //}

    //else if (enColTitle::VER /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(
    //        pDisplay, vecItem[nrow].second->VER.GetValue(), _T("%.1f"), vecItem[nrow].second->GetUseVer(), nrow);
    //}
    //else if (enColTitle::VER_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->VER_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseVer(), nrow);
    //}
    //else if (enColTitle::RATIO_HV /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_HV.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioHV(), nrow);
    //}
    //else if (enColTitle::OVER_GV_PX_COUNT_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->OVER_GV_PX_COUNT_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseOverGVPxCount(), nrow);
    //}
    //else if (enColTitle::DIS_TO_EDGE_mm_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay,
    //        (enCompareOperator)vecItem[nrow].second->DIS_TO_EDGE_mm_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseDis2Edge(), nrow);
    //}
    //else if (enColTitle::DIS_TO_EDGE_2_mm_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->DIS_TO_EDGE_2_mm_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseDis2Edge_2(), nrow);
    //}
    //else if (enColTitle::IS_NEAR_EDGE_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->IS_NEAR_EDGE_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseIsNearEdge(), nrow);
    //}
    //else if (enColTitle::RATIO_HV_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_HV_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioHV(), nrow);
    //}
    //else if (enColTitle::RATIO_VH /**/ == ncor)
    //{
    //    DisplayItemNumber<float>(pDisplay, vecItem[nrow].second->RATIO_VH.GetValue(), _T("%.1f"),
    //        vecItem[nrow].second->GetUseRatioVH(), nrow);
    //}
    //else if (enColTitle::RATIO_VH_CO /**/ == ncor)
    //{
    //    DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->RATIO_VH_COMPARE_OPERATOR.GetValue(),
    //        vecItem[nrow].second->GetUseRatioVH(), nrow);
    //}
    else if (enColTitle::PARENT == ncor)
    {
        if (vecItem[nrow].second->GetUseParent())
        {
            pDisplay->item.strText
                = enumtoString().Parent2String((enParent)vecItem[nrow].second->PARENT_COMPARE_OPERATOR.GetValue());
        }
    }
    {
        m_mutex.Lock();
        if (nullptr != m_vecTogeter)
        {
            for (int i = 0; i < (*m_vecTogeter).size(); i++)
            {
                if ((*m_vecTogeter)[i] == vecItem[nrow].first && m_nSelectCol == ncor)
                {
                    pDisplay->item.crBkClr = RGB(189, 179, 0);
                }
            }
        }
        m_mutex.Unlock();
    }

}

bool CDlgSpecTable::DisplayItemCompare(GV_DISPINFO* pDisplay, enCompareOperator compare, bool bUse, long nrow)
{
    if (true == bUse)
    {
        pDisplay->item.strText = enumtoString().CompareOperator2String(compare);
        //pDisplay->item.crBkClr = BackGroundColor(nrow);
    }

    return bUse;
}

void CDlgSpecTable::NextOrAnd(int nRow, int nCol)
{
    if (enColTitle::USE == nCol)
    {
        if (vecItem.size() > nRow)
        {
            bool bUse = vecItem[nRow].second->USE.GetValue();
            vecItem[nRow].second->USE = !bUse;
        }
    }
    if (enColTitle::ENABLE == nCol)
    {
        if (vecItem.size() > nRow)
        {
            bool bEnable = vecItem[nRow].second->ENABLE.GetValue();
            vecItem[nRow].second->ENABLE = !bEnable;
        }
    }

    if (enColTitle::ORAND == nCol)
    {
        if (vecItem.size() > nRow)
        {
            bool bOrAnd = vecItem[nRow].second->ORAND.GetValue();
            vecItem[nRow].second->ORAND = !bOrAnd;
        }
    }
    if (enColTitle::SMALL_INSP == nCol)
    {
        //if (vecItem.size() > nRow)
        //{
        //    bool bSmallInsp = vecItem[nRow].second->SMALL_INSP.GetValue();
        //    vecItem[nRow].second->SMALL_INSP = !bSmallInsp;
        //}
    }
}

void CDlgSpecTable::NextInspectionImgType(int nRow, int nCol)
{
    BOOL bUse = false;
    if (vecItem.size() <= nRow)
    {
        ASSERT(1);
        return;
    }
    Base::ParaLong* pValue = nullptr;
    switch (nCol)
    {
        case enColTitle::INSPECTION_IMAGE_TYPE:
        {
            pValue = &vecItem[nRow].second->INSPECTION_IMAGE_TYPE;
            bUse = true;
        }
            if (nullptr != pValue && true == bUse)
            {
                *pValue = (pValue->GetValue() + 1) % enInspectionImageType::INS_END;
            }
            break;

        case enColTitle::DEFECT_BRIGHTNESS:
        {
            pValue = &vecItem[nRow].second->nDefectBrightness;
            bUse = true;
        }
            if (nullptr != pValue && true == bUse)
            {
                *pValue = (pValue->GetValue() + 1) % enDefectBrightness::DFB_NO_USE;
            }
            break;
    }
}
void CDlgSpecTable::NextCompareOperator(int nRow, int nCol)
{
    BOOL bUse = false;
    if (vecItem.size() <= nRow)
    {
        ASSERT(1);
        return;
    }

    Base::ParaLong* pValue = nullptr;
    //현식 동작을 원한다면 이걸 바꿔야 한다.
    // 예진아 이거 넣어서 동작 되도록 바꿔야 한다...
    switch (nCol)
    {
        case enColTitle::SIZE_X_CO:
        {
            pValue = &vecItem[nRow].second->SIZEX_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::SIZE_Y_CO:
        {
            pValue = &vecItem[nRow].second->SIZEY_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
      /*  case enColTitle::SECTION_THRESHOLD_CO:
        {
            pValue = &vecItem[nRow].second->SECTION_THRESHOLD_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::WIDTH_CO:
        {
            pValue = &vecItem[nRow].second->WIDTH_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::LENGTH_CO:
        {
            pValue = &vecItem[nRow].second->LENGTH_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::AREA_Ratio_CO:
        {
            pValue = &vecItem[nRow].second->AREA_RATIO_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::RATIO_WL_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_WL_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::RATIO_XY_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_XY_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::RATIO_YX_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_YX_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::COMPACT_CO:
        {
            pValue = &vecItem[nRow].second->COMPACT_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::ANGLE_CO:
        {
            pValue = &vecItem[nRow].second->ANGLE_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::STD_CO:
        {
            pValue = &vecItem[nRow].second->STD_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::MIN_CO:
        {
            pValue = &vecItem[nRow].second->MIN_GV_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::AVG_CO:
        {
            pValue = &vecItem[nRow].second->AVG_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::MAX_CO:
        {
            pValue = &vecItem[nRow].second->MAX_GV_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::BRIGHT_AVG_CO:
        {
            pValue = &vecItem[nRow].second->BRIGHT_AVG_GV_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::DARK_AVG_CO:
        {
            pValue = &vecItem[nRow].second->DARK_AVG_GV_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::RATIO_WHITE_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_WHITE_COMPARE_OPERATOR;
            bUse = true;
        }
        break;

        case enColTitle::RATIO_DARK_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_DARK_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::AREA_CO:
        {
            pValue = &vecItem[nRow].second->AREA_COMPARE_OPERATOR;
            bUse = true;
        }
        break;

        case enColTitle::EDGE_ENERGY_CO:
        {
            pValue = &vecItem[nRow].second->EDGE_ENERGY_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::MASS_CENTERGV_CO:
        {
            pValue = &vecItem[nRow].second->MASS_CENTER_GV_GV_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::HOR_CO:
        {
            pValue = &vecItem[nRow].second->HOR_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::VER_CO:
        {
            pValue = &vecItem[nRow].second->VER_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::OVER_GV_PX_COUNT_CO:
        {
            pValue = &vecItem[nRow].second->OVER_GV_PX_COUNT_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::DIS_TO_EDGE_mm_CO:
        {
            pValue = &vecItem[nRow].second->DIS_TO_EDGE_mm_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::DIS_TO_EDGE_2_mm_CO:
        {
            pValue = &vecItem[nRow].second->DIS_TO_EDGE_2_mm_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::IS_NEAR_EDGE_CO:
        {
            pValue = &vecItem[nRow].second->IS_NEAR_EDGE_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::RATIO_HV_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_HV_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::RATIO_VH_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_VH_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
        case enColTitle::RATIO_LW_CO:
        {
            pValue = &vecItem[nRow].second->RATIO_LW_COMPARE_OPERATOR;
            bUse = vecItem[nRow].second->GetUseRatioYX();
        }
        break;*/
        //case enColTitle::COMPACT_CO:
        //{
        //	//	pValue = &vecItem[nRow].second->COMPACT_COMPARE_OPERATOR;
        //	//	bUse = vecItem[nRow].second->GetUseCompact();
        //}
        //break;
        default:
            break;
    }

    if (nullptr != pValue && true == bUse)
    {
        *pValue = (pValue->GetValue() + 1) % enCompareOperator::CO_END;
    }
}

void CDlgSpecTable::EditGridText(int nRow, int nCol)
{
    if (vecItem.size() <= nRow)
    {
        ASSERT(1);
        return;
    }

    BOOL bUse = false;
    {
        Base::ParaFloat* pValue = nullptr;
        switch (nCol)
        {
            case enColTitle::SIZE_X:
                pValue = &vecItem[nRow].second->SIZE_X_mm;
                bUse = true;
                break;

            case enColTitle::SIZE_Y:
                pValue = &vecItem[nRow].second->SIZE_Y_mm;
                bUse = true;
                break;

         /*   case enColTitle::COMPACT:
                pValue = &vecItem[nRow].second->COMPACT;
                bUse = vecItem[nRow].second->GetUseCompact();
                break;

            case enColTitle::RATIO_XY:
                pValue = &vecItem[nRow].second->RATIO_XY;
                bUse = vecItem[nRow].second->GetUseRatioXY();
                break;

            case enColTitle::RATIO_YX:
                pValue = &vecItem[nRow].second->RATIO_YX;
                bUse = vecItem[nRow].second->GetUseRatioYX();
                break;

            case enColTitle::RATIO_LW:
                pValue = &vecItem[nRow].second->RATIO_LW;
                bUse = vecItem[nRow].second->GetUseRatioLW();
                break;

            case enColTitle::RATIO_WL:
                pValue = &vecItem[nRow].second->RATIO_WL;
                bUse = vecItem[nRow].second->GetUseRatioWL();
                break;

            case enColTitle::AREA_Ratio:
                pValue = &vecItem[nRow].second->AREA_Ratio;
                bUse = vecItem[nRow].second->GetUseArea_RAtio();
                break;

            case enColTitle::LENGTH:
                pValue = &vecItem[nRow].second->LENGTH_mm;
                bUse = vecItem[nRow].second->GetUseLength();
                break;

            case enColTitle::WIDTH:
                pValue = &vecItem[nRow].second->WIDTH_mm;
                bUse = vecItem[nRow].second->GetUseWidth();
                break;

            case enColTitle::ANGLE:
                pValue = &vecItem[nRow].second->ANGLE;
                bUse = vecItem[nRow].second->GetUseAngle();
                break;

            case enColTitle::STD:
                pValue = &vecItem[nRow].second->STD;
                bUse = vecItem[nRow].second->GetUseStd();
                break;

            case enColTitle::MINIMUM_SIZE_X:
                pValue = &vecItem[nRow].second->MINIMUM_SIZEX_mm;
                bUse = true;
                break;

            case enColTitle::MINIMUM_SIZE_Y:
                pValue = &vecItem[nRow].second->MINIMUM_SIZEY_mm;
                bUse = true;
                break;

            case enColTitle::RATIO_WHITE:
                pValue = &vecItem[nRow].second->RATIO_WHITE;
                bUse = true;
                break;

            case enColTitle::RATIO_DARK:
                pValue = &vecItem[nRow].second->RATIO_DARK;
                bUse = true;
                break;

            case enColTitle::EDGE_ENERGY:
                pValue = &vecItem[nRow].second->EDGE_ENERGY;
                bUse = true;
                break;

            case enColTitle::AREA:
                pValue = &vecItem[nRow].second->AREA;
                bUse = true;
                break;

            case enColTitle::MASS_CENTERGV:
                pValue = &vecItem[nRow].second->MASS_CENTER_GV_GV;
                bUse = true;
                break;

            case enColTitle::VER:
                pValue = &vecItem[nRow].second->VER;
                bUse = true;
                break;
            case enColTitle::HOR:
                pValue = &vecItem[nRow].second->HOR;
                bUse = true;
                break;
            case enColTitle::RATIO_HV:
                pValue = &vecItem[nRow].second->RATIO_HV;
                bUse = true;
                break;
            case enColTitle::RATIO_VH:
                pValue = &vecItem[nRow].second->RATIO_VH;
                bUse = true;
                break;
            case enColTitle::DIS_TO_EDGE_mm:
                pValue = &vecItem[nRow].second->DISTANCE_TO_EDGE_mm;
                bUse = true;
                break;
            case enColTitle::DIS_TO_EDGE_2_mm:
                pValue = &vecItem[nRow].second->DISTANCE_TO_EDGE_2_mm;
                bUse = true;
                break;*/
            default:
                break;
        }

        if (nullptr != pValue && true == bUse)
        {
            CDlgEditGrid dlg(GetDefectName(vecItem[nRow].first), Numeral2String(_T("%.1f"), pValue->GetValue()),
                GetCellPoint(nRow, nCol), this);
            if (dlg.DoModal() == IDOK)
            {
                *pValue = _wtof(dlg.m_strValue);
                //auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
                m_ISI_Spec_table.SetValueTogether(vecItem[nRow].first, (enColTitle)nCol, _wtof(dlg.m_strValue));
            }
        }
    }

    {
        Base::ParaLong* pValue = nullptr;
        switch (nCol)
        {
        /*    case enColTitle::SECTION_THRESHOLD:
                pValue = &vecItem[nRow].second->SECTION_THRESHOLD;
                bUse = vecItem[nRow].second->GetUseSectionThreshold();
                break;
            case enColTitle::OVER_GV_PX_COUNT:
                pValue = &vecItem[nRow].second->OVER_GV_PX_COUNT;
                bUse = vecItem[nRow].second->GetUseOverGVPxCount();
                break;
            case enColTitle::AVG:
                pValue = &vecItem[nRow].second->AVG_GV;
                bUse = vecItem[nRow].second->GetUseAvgGV();
                break;

            case enColTitle::MIN:
                pValue = &vecItem[nRow].second->MIN_GV;
                bUse = vecItem[nRow].second->GetUseMinGV();
                break;

            case enColTitle::MAX:
                pValue = &vecItem[nRow].second->MAX_GV;
                bUse = vecItem[nRow].second->GetUseMaxGV();
                break;*/

            case enColTitle::COUNT:
                pValue = &vecItem[nRow].second->COUNT;
                bUse = vecItem[nRow].second->GetUseCount();
                break;
            default:
                break;
        }

        if (nullptr != pValue && true == bUse)
        {
            CDlgEditGrid dlg(GetDefectName(vecItem[nRow].first), Numeral2String(_T("%d"), pValue->GetValue()),
                GetCellPoint(nRow, nCol), this);
            if (dlg.DoModal() == IDOK)
            {
                *pValue = _wtoi(dlg.m_strValue);
                //auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
                m_ISI_Spec_table.SetValueTogether(vecItem[nRow].first, (enColTitle)nCol, _wtoi(dlg.m_strValue));
            }
        }
    }
}

void CDlgSpecTable::NextAreaOperator(int nRow, int nCol)
{
    BOOL bUse = false;
    if (vecItem.size() <= nRow)
    {
        ASSERT(1);
        return;
    }

    Base::ParaLong* pValue = nullptr;
    //현식 동작을 원한다면 이걸 바꿔야 한다.
    // 예진아 이거 넣어서 동작 되도록 바꿔야 한다...
    switch (nCol)
    {
        case enColTitle::INSPECTION_AREA:
        {
            pValue = &vecItem[nRow].second->nInspectionArea;
            bUse = true;
        }
        break;
        //case enColTitle::SMALL_INSP_LEVEL:
        //{
        //    pValue = &vecItem[nRow].second->SMALL_INSP_LEVEL;
        //    bUse = true;
        //}
        break;
        case enColTitle::PARENT:
        {
            pValue = &vecItem[nRow].second->PARENT_COMPARE_OPERATOR;
            bUse = true;
        }
        break;
    }

    if (nCol == enColTitle::INSPECTION_AREA && nullptr != pValue && true == bUse)
    {
        *pValue = (pValue->GetValue() + 1) % enInspectionArea::IA_TAB_END;
    }

    if (nCol == enColTitle::SMALL_INSP_LEVEL && nullptr != pValue && true == bUse)
    {
        *pValue = (pValue->GetValue() + 1) % enSmallInsplevel::end;
    }

    if (nCol == enColTitle::PARENT && nullptr != pValue && true == bUse)
    {
        *pValue = (pValue->GetValue() + 1) % enParent::End;
    }
}

void CDlgSpecTable::TogeterValue(int nRow, int nCol)
{
    if (vecItem.size() <= nRow)
        return;

    m_mutex.Lock();
    m_vecTogeter = m_ISI_Spec_table.GetValueTogether(vecItem[nRow].first, (enColTitle)nCol);
    m_nSelectCol = nCol;
    m_mutex.Unlock();
}

CPoint CDlgSpecTable::GetCellPoint(int nRow, int nCol)
{
    int nScrollPosX = m_ctrGrid.GetScrollPos(0);

    CRect roi;

    long nWidth = 0;
    for (int c = 0; c < nCol; c++)
    {
        m_ctrGrid.GetCellRect(nRow, c, LPRECT(roi));

        nWidth += roi.Width();
    }

    long nHeight = 0;
    for (int h = 0; h < nRow; h++)
    {
        m_ctrGrid.GetCellRect(h, 0, LPRECT(roi));
        nHeight += roi.Height();
    }

    CRect gridrect;
    m_ctrGrid.GetWindowRect(gridrect);

    CPoint cp(nWidth - nScrollPosX, nHeight + m_rtGrid.top);
    cp.x += gridrect.left;
    return cp;
}




void CDlgSpecTable::LinkSpecAndTable()
{
    vecItem.clear();
    for (int i = 0; i < (long)DefectType::ISI_END; i++)
    {
        //auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
        CISISpecInfo* spec = m_ISI_Spec_table.GetSpecInfo((DefectType)i);
        if (nullptr != spec)
        {
            vecItem.push_back(std::make_pair((DefectType)i, spec));
        }
    }
}

void CDlgSpecTable::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    SetGridType(m_ctrTab.GetCurSel());
    *pResult = 0;
}

void CDlgSpecTable::OnBnClickedButtonUseNormal()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_use_mode = 0;
    m_ctrGrid.SetColumnWidth(1, 0);
    for (int row = 0; row < m_ctrGrid.GetRowCount(); row++)
    {
        if (m_ctrGrid.GetItemText(row, 1) == _T("x"))
        {
            m_ctrGrid.SetRowHeight(row, 0);
        }
    }
    m_ctrGrid.Invalidate();
}

void CDlgSpecTable::OnBnClickedButtonUseAdmin()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_use_mode = 1;
    SetGridType(m_ctrTab.GetCurSel());
}

void CDlgSpecTable::OnBnClickedButtonInitSpec()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    m_vecTogeter = nullptr;

    if (IDYES == AfxMessageBox(_T("Do you want to reset? (YES/NO)"), MB_YESNO | MB_ICONSTOP))
    {
        //auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
        m_ISI_Spec_table.initSpec(); 

        LinkSpecAndTable();
    }
    m_ctrGrid.Invalidate();
}

void CDlgSpecTable::SetGridType(bool bSmallDefect)
{
    switch (m_use_mode)
    {
        case 0:
            for (int col = 1; col < enColTitle::CT_END; col++)
            {
                if (true == bSmallDefect)
                {
                    m_ctrGrid.SetColumnWidth(0, 200);
                    if (true == isSmallDefectUsingCol((enColTitle)col))
                    {
                        m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
                        if ((enColTitle)col == (enColTitle)ITEM)
                        {
                            m_ctrGrid.SetColumnWidth(col, 200);
                        }
                    }
                    else
                        m_ctrGrid.SetColumnWidth(col, 0);
                }
                else
                {
                    m_ctrGrid.SetColumnWidth(0, m_vecnSizeCol[col]);
                    m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
                    if (col == (enColTitle)MINIMUM_SIZE_X || col == (enColTitle)MINIMUM_SIZE_Y
                        || col == (enColTitle)COUNT || col == (enColTitle)PARENT)
                    {
                        m_ctrGrid.SetColumnWidth(col, 0);
                    }
                }
            }
            m_ctrGrid.SetColumnWidth(1, 0);
            for (int row = 0; row < vecItem.size(); row++)
            {
                if (!bSmallDefect == isSmallDefectType(vecItem[row].first))
                    m_ctrGrid.SetRowHeight(row + 1, 0);
                else
                {
                    m_ctrGrid.SetRowHeight(row + 1, m_nSizeRow);
                    if (vecItem[row].first >= DefectType::SmallDefect_ProtrutionLevel1
                        && vecItem[row].first <= DefectType::SmallDefect_ProtrutionLevel5)
                    {
                        m_ctrGrid.SetRowHeight(row + 1, 0);
                    }

                    if (vecItem[row].first >= DefectType::SmallDefect_ScratchLevel1
                        && vecItem[row].first <= DefectType::SmallDefect_ScratchLevel5)
                    {
                        m_ctrGrid.SetRowHeight(row + 1, 0);
                    }
                }
            }

            for (int row = 0; row < m_ctrGrid.GetRowCount(); row++)
            {
                if (m_ctrGrid.GetItemText(row, 1) == _T("x"))
                {
                    m_ctrGrid.SetRowHeight(row, 0);
                }
            }
            break;
        case 1:
            for (int col = 1; col < enColTitle::CT_END; col++)
            {
                if (true == bSmallDefect)
                {
                    if (true == isSmallDefectUsingCol((enColTitle)col))
                        m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
                    else
                        m_ctrGrid.SetColumnWidth(col, 0);
                }
                else
                {
                    m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
                    if (col == (enColTitle)MINIMUM_SIZE_X || col == (enColTitle)MINIMUM_SIZE_Y
                        || col == (enColTitle)COUNT || col == (enColTitle)PARENT)
                    {
                        m_ctrGrid.SetColumnWidth(col, 0);
                    }
                }
            }

            for (int row = 0; row < vecItem.size(); row++)
            {
                if (!bSmallDefect == isSmallDefectType(vecItem[row].first))
                    m_ctrGrid.SetRowHeight(row + 1, 0);
                else
                    m_ctrGrid.SetRowHeight(row + 1, m_nSizeRow);
            }
            break;
    }
    m_ctrGrid.Invalidate();
}

bool CDlgSpecTable::isSmallDefectUsingCol(enColTitle ct)
{
    return (m_mapSDCol.end() == m_mapSDCol.find(ct)) ? false : true;
}

bool CDlgSpecTable::isSmallDefectType(DefectType dt)
{
    return (m_mapSD.end() == m_mapSD.find(dt)) ? false : true;
}

void CDlgSpecTable::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    JobFileSave();
    CDialogEx::OnOK();
}

void CDlgSpecTable::OnBnClickedCancel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_ISI_Spec_table = Inspection::Accessor::Get().m_isi_spec;
    CDialogEx::OnCancel();
}

bool CDlgSpecTable::JobFileSave()
{
    CString strPath;
    const TCHAR szFilters[] = _T("Job Files (*.job)|*.job||");
    CFileDialog dlg(TRUE, _T("Job"), _T("*.job"),
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);
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

void CDlgSpecTable::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    GetClientRect(&m_dlgSize);

    // static
    ControlSizeChange(MESSAGE_GRID);

    // Btn
    ControlSizeChange(IDC_BUTTON_USE_NORMAL);
    ControlSizeChange(IDC_BUTTON_USE_ADMIN);
    ControlSizeChange(IDC_BUTTON_INIT_SPEC);
    ControlSizeChange(IDOK);
    ControlSizeChange(IDCANCEL);
    
    
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
void CDlgSpecTable::ControlSizeChange(int nID)
{
    CWnd* pCtl;

    long nStartPointX = m_dlgSize.right;
    long nStartPointY = m_dlgSize.top;

    long nStaticSizeX = m_dlgSize.Width() * 0.49;
    long nTitleSizeX = nStaticSizeX;

    long nBtnSizeX = m_dlgSize.Width() * 0.05;
    long nBtnSizeY = m_dlgSize.Height() * 0.04;

    long nBtnSizeX2 = m_dlgSize.Width() * 0.1;
    long nBtnSizeY2 = m_dlgSize.Height() * 0.04;

    long nGridSizeX = m_dlgSize.Width();
    long nGridSizeY = m_dlgSize.Height() * 0.7;
    
    long nStaticSizeY = m_dlgSize.Height() * 0.9;
    long nTitleSizeY = m_dlgSize.Height() * 0.05;

    //m_nDefectMapSizeX = nStaticSizeX;
    //m_nDefectMapSizeY = nStaticSizeY;

    // Btn
    pCtl = GetDlgItem(IDC_BUTTON_INIT_SPEC);
    if (pCtl != NULL)
    {
        pCtl->MoveWindow(nStartPointX - nBtnSizeX, nStartPointY, nBtnSizeX, nBtnSizeY);
    }

    pCtl = GetDlgItem(IDC_BUTTON_USE_ADMIN);
    if (pCtl != NULL)
    {
        pCtl->MoveWindow(nStartPointX - (nBtnSizeX *2), nStartPointY, nBtnSizeX, nBtnSizeY);
    }

    pCtl = GetDlgItem(IDC_BUTTON_USE_NORMAL);
    if (pCtl != NULL)
    {
        pCtl->MoveWindow(nStartPointX - (nBtnSizeX * 3), nStartPointY, nBtnSizeX, nBtnSizeY);
    }

    pCtl = GetDlgItem(IDCANCEL);
    if (pCtl != NULL)
    {
        pCtl->MoveWindow(nStartPointX - nBtnSizeX2, m_dlgSize.bottom - nBtnSizeY2, nBtnSizeX2, nBtnSizeY2);
    }

    pCtl = GetDlgItem(IDOK);
    if (pCtl != NULL)
    {
        pCtl->MoveWindow(nStartPointX - (nBtnSizeX2 * 2), m_dlgSize.bottom - nBtnSizeY2, nBtnSizeX2, nBtnSizeY2);
    }

    //Grid
    pCtl = GetDlgItem(MESSAGE_GRID);
    if (pCtl != NULL)
    {
        pCtl->MoveWindow(m_dlgSize.left, m_dlgSize.top + nBtnSizeY + 50, nGridSizeX, nGridSizeY);
    }
   
}

void CDlgSpecTable::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    lpMMI->ptMinTrackSize.x = 1200;
    lpMMI->ptMinTrackSize.y = 600;

    lpMMI->ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
    lpMMI->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);

    CDialogEx::OnGetMinMaxInfo(lpMMI);
}
