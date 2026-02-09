// DlgDataSearch.cpp: 구현 파일
//

#include "DlgDataSearch.h"
#include "DataSearch.h"
#include "afxdialogex.h"
#include "pch.h"
#include "resource.h"

#include "..\\SharedModule\CommonShare\DefectDef.h"
#include <../../iVisionMadangSDK/include/Algorithm/ImageProcessing.h>
#include <../../iVisionMadangSDK/include/Types/Image_8u_C1.h>
#include <../../iVisionMadangSDK/include/Types/Rect.h>
#include <../../iVisionMadangSDK/include/Widget/ImageView.h>
#include <ipvmbasedef.h>
#include <../../iVisionMadangSDK/include/Gadget/TimeCheck.h>
#include <afxtempl.h>
#include <algorithm>
#include <vector>





// CDlgDataSearch 대화 상자

IMPLEMENT_DYNAMIC(CDlgDataSearch, CDialogEx)

CDlgDataSearch::CDlgDataSearch(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_DATA_SEARCH, pParent)
    , m_nRadioID(0)
    , m_bInputCheck(FALSE)
{
    
}

CDlgDataSearch::~CDlgDataSearch()
{
    delete m_pDlgDefectRawSearch;
    delete m_pDlgLotSearch;
    delete m_pDlgDefectTimeTable;
    for (int i = 0; i < 2; i++)
        delete m_pImageView[i];
}

void CDlgDataSearch::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DATETIMEPICKER_START_DAY, m_dateStartMonth);
    DDX_Control(pDX, IDC_DATETIMEPICKER_END_DAY, m_dateEndMonth);
    DDX_Control(pDX, IDC_DATETIMEPICKER_START_TIME, m_dateStartTime);
    DDX_Control(pDX, IDC_DATETIMEPICKER_END_TIME, m_dateEndTime);
    DDX_Control(pDX, IDC_RADIO_RAW, m_radioBtnRaw);
    DDX_Control(pDX, IDC_RADIO_LOT, m_radioBtnLot);
    DDX_Control(pDX, IDC_LIST_CSV_FIND_FILE, m_listCtrlCsvList);
    DDX_Control(pDX, IDC_LIST_CSV_INFO, m_listCtrlCsvInfo);
    DDX_Control(pDX, IDC_BUTTON_FIND, m_btnFind);
    DDX_Control(pDX, IDC_EDIT_SEARCH_BLANK, m_editSearchBlank);
    DDX_Radio(pDX, IDC_RADIO_1, m_nRadioSelect);
    DDX_Control(pDX, IDC_STATIC_DEFECT_INFO, m_staticDefectInfo);
    DDX_Control(pDX, IDC_LIST_SELECT_INFO, m_listCtrlExcelSelect);
    DDX_Control(pDX, IDC_CHECK_CROP, m_checkCropSave);
    DDX_Control(pDX, IDC_LIST_DEFECTTIMETABLE, m_list_DefectTimeTable);
    DDX_Control(pDX, IDC_CHECK_DEFECTPOSIMAGEADD, m_chk_DefectPosAdd);
}

BEGIN_MESSAGE_MAP(CDlgDataSearch, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_FIND, &CDlgDataSearch::OnBnClickedButtonFind)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_1, &CDlgDataSearch::OnNMDblclkListCsvFindFile)
ON_WM_SIZE()
ON_BN_CLICKED(IDC_RADIO_RAW, &CDlgDataSearch::OnBnClickedRadioRaw)
ON_BN_CLICKED(IDC_RADIO_LOT, &CDlgDataSearch::OnBnClickedRadioLot)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_CSV_INFO, &CDlgDataSearch::OnNMDblclkListCsvInfo)
ON_NOTIFY(NM_CLICK, IDC_LIST_CSV_INFO, &CDlgDataSearch::OnNMClickListCsvInfo)
ON_BN_CLICKED(IDC_BUTTON_INPUT, &CDlgDataSearch::OnBnClickedButtonInput)
ON_BN_CLICKED(IDC_BUTTON_OUTPUT, &CDlgDataSearch::OnBnClickedButtonOutput)
ON_NOTIFY(NM_CLICK, IDC_LIST_SELECT_INFO, &CDlgDataSearch::OnNMClickListSelectInfo)
ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CDlgDataSearch::OnBnClickedButtonExport)
ON_BN_CLICKED(IDC_BUTTON_RESET, &CDlgDataSearch::OnBnClickedButtonReset)
ON_BN_CLICKED(IDC_BUTTON_EXCELCLEAR, &CDlgDataSearch::OnBnClickedButtonExcelclear)
ON_BN_CLICKED(IDC_RADIO_NONE, &CDlgDataSearch::OnBnClickedRadioNone)
ON_BN_CLICKED(IDC_BUTTON_DEFECTCSVEXPORT, &CDlgDataSearch::OnBnClickedButtonDefectcsvexport)
END_MESSAGE_MAP()
// CDlgDataSearch 메시지 처리기


bool TimeCompare(DefectTimetable a, DefectTimetable b)
{
    return a.tm < b.tm;
}

bool CTimeCompare_2(CTime a, CTime b)
{
    return a < b;
}
BOOL CDlgDataSearch::OnInitDialog() 
{
    CDialogEx::OnInitDialog();
    IPVM::Rect rect(0, 0, 0, 0);
    m_pImageView[0] = new IPVM::ImageView(this->GetSafeHwnd(), rect, (long)IDC_STATIC_IMAGE_VIEW_BRIGHT);
    m_pImageView[1] = new IPVM::ImageView(this->GetSafeHwnd(), rect, (long)IDC_STATIC_IMAGE_VIEW_DARK);
    this->ShowWindow(SW_SHOW);
    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_vecTitle = {_T("STATUS"), _T("AVG_GV"), _T("MIN_GV"), _T("MAX_GV"), _T("BRIGHT_AVG"), _T("DARK_AVG"),
        _T("SIZE_X"), _T("SIZE_Y"), _T("AREA_Ratio"), _T("RATIO_YX"), _T("RATIO_XY"), _T("COMPACT"), _T("RATIO_WHITE"),
        _T("RATIO_DARK"), _T("EDGE_ENERGY"), _T("MASS_CENTER_GV"), _T("LotID"), _T("CellID"), _T("Time"),
        _T("DefectType"), _T("TopBack"), _T("Defect_X"), _T("Defect_Y"), _T("Judge"), _T("Bright_Path"), _T("Dark_Path")

    };
    m_vecDefectName = {_T("Line"), _T("Pinhole"), _T("Horizontal_Line"), _T("Scratch"), _T("Crack"),
        _T("Stain"), _T("Stain_Pinhole"), _T("Delamination_Crack"), _T("Dent"), _T("Black_Dot"), _T("Delamination_Pinhole"), _T("Banking"),
        _T("ETC_Defect"), _T("Edge_ChippingHole"), _T("Drag"), _T("Edge_Line"), _T("Edge_Defect"),
        _T("Island"), _T("Plain_Wrinkle")};


    DlgControlSetting();
    TotalControlResize();

    m_pDlgDefectRawSearch = new CDlgDefectRawSearch(this);
    m_pDlgDefectRawSearch->Create(IDD_DIALOG_SEARCH_DEFECTRAW);
    m_pDlgDefectRawSearch->ShowWindow(SW_SHOW);

    m_pDlgLotSearch = new CDlgLotSearch(this);
    m_pDlgLotSearch->Create(IDD_DIALOG_SEARCH_LOT);

    m_pDlgDefectTimeTable = new CDlgDefectTimeTable(this);
    m_pDlgDefectTimeTable->Create(IDD_DIALOG_DEFECTTIMETABLE);
    //m_pDlgDefectTimeTable->ShowWindow(SW_SHOW);
    m_ListFont.CreatePointFont(75, _T("HY 견고딕"));
    return TRUE;
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgDataSearch::DlgControlSetting()
{
    int controlIDs[] = {IDC_STATIC_STATUS, IDC_STATIC_AVG_GV, IDC_STATIC_MIN_GV, IDC_STATIC_MAX_GV,
        IDC_STATIC_BRIGHT_AVG, IDC_STATIC_DARK_GV, IDC_STATIC_SIZE_X, IDC_STATIC_SIZE_Y, IDC_STATIC_RATIO_AREA,
        IDC_STATIC_RATIO_YX, IDC_STATIC_RATIO_XY, IDC_STATIC_COMPACT, IDC_STATIC_RATIO_WHITE, IDC_STATIC_RATIO_DARK,
        IDC_STATIC_EDGE_ENERGY, IDC_STATIC_MASS_CENTER, IDC_STATIC_LOTID, IDC_STATIC_CELLID, IDC_STATIC_TIME,
        IDC_STATIC_DEFECT_TYPE, IDC_STATIC_TOP_BACK, IDC_STATIC_DEFECT_X, IDC_STATIC_DEFECT_Y, IDC_STATIC_JUDGE};


    m_listCtrlCsvList.InsertColumn(1, _T("Num"), NULL, 50, NULL);
    m_listCtrlCsvList.InsertColumn(2, _T("File Name"), NULL, 250, NULL);
    m_listCtrlCsvList.InsertColumn(3, _T("Total Num"), NULL, 150, NULL);
    long nList = 4;

#if INSPECTION_MACHINE_UC1
    for (long nDefect = (long)DefectType::White_Line; nDefect < (long)DefectType::Edge_Defect; nDefect++)
    {
        CString strDefecntName;
        strDefecntName = GetDefectName((DefectType)nDefect);
        strDefecntName = strDefecntName + _T("_Num");
        m_listCtrlCsvList.InsertColumn(nList, strDefecntName, NULL, 100, NULL);
        nList++;
    }
#else
    for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
    {
        CString strDefecntName;
        strDefecntName = GetDefectName((DefectType)nDefect);
        strDefecntName = strDefecntName + _T("_Num");
        m_listCtrlCsvList.InsertColumn(nList, strDefecntName, NULL, 100, NULL);
        nList++;
    }
#endif
    

    InitDefectNum();
    //m_listCtrlCsvList.InsertColumn(3, _T("Create Time"), NULL, 200, NULL);

    for (long n = 0; n < m_vecTitle.size(); n++)
    {
        if (n == 0)
        {
            m_listCtrlExcelSelect.InsertColumn(1, _T("Num"), NULL, 50, NULL);
        }
        m_listCtrlExcelSelect.InsertColumn(n + 1, m_vecTitle[n], NULL, 50, NULL);
        m_listCtrlCsvInfo.InsertColumn(n + 1, m_vecTitle[n], NULL, 50, NULL);
    }

    for (int i = 0; i < sizeof(controlIDs) / sizeof(controlIDs[0]); i++)
    {
        SetDlgItemTextW(controlIDs[i], m_vecTitle[i] + _T(" : "));
    }

    m_list_DefectTimeTable.InsertColumn(1, _T("Year"), NULL, 50, NULL);
    m_list_DefectTimeTable.InsertColumn(2, _T("Month"), NULL, 50, NULL);
    m_list_DefectTimeTable.InsertColumn(3, _T("Day"), NULL, 50, NULL);
    m_list_DefectTimeTable.InsertColumn(4, _T("TIME"), NULL, 50, NULL);

    long nNum = 5;

#if INSPECTION_MACHINE_UC1
    for (long nDefect = (long)DefectType::White_Line; nDefect < (long)DefectType::Edge_Defect; nDefect++)
    {
        //vecvecDefectInfo[nSize].push_back(m_vecDefectInfo[nDefect]);
        CString strDefecntName;
        DefectInfo Defect;
        strDefecntName = GetDefectName((DefectType)nDefect);
        m_list_DefectTimeTable.InsertColumn(nNum, strDefecntName, NULL, 50, NULL);
        nNum++;
    }
#else
    for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
    {
        //vecvecDefectInfo[nSize].push_back(m_vecDefectInfo[nDefect]);
        CString strDefecntName;
        DefectInfo Defect;
        strDefecntName = GetDefectName((DefectType)nDefect);
        m_list_DefectTimeTable.InsertColumn(nNum, strDefecntName, NULL, 50, NULL);
        nNum++;
    }
#endif
}

void CDlgDataSearch::OnBnClickedButtonFind()
{
    CString strTimeCheck;
    IPVM::TimeCheck tc;
    tc.Reset();

    m_nRadioID = GetCheckedRadioButton(IDC_RADIO_RAW, IDC_RADIO_NONE);
    VectorAndListClear();

    //SYSTEMTIME st;
    //m_dateStartMonth.GetTime(&st);
    switch (m_nRadioID)
    {
        case IDC_RADIO_RAW:
            RawSearchMode();
            break;
        case IDC_RADIO_LOT:
            LotSearchMode();
            break;
        case IDC_RADIO_NONE:
            break;
        default:
            break;
    }
    //float fElapsedTime = tc.Elapsed_ms();
    //tc.Reset();
    //strTimeCheck.Format(_T("%f"), fElapsedTime);
    //MessageBox(strTimeCheck + _T("(ms)"));
    

}

BOOL CDlgDataSearch::BetweenTimeRange(const CTime& fileTime, const CTime& startTime, const CTime& endTime)
{
    return (fileTime >= startTime && fileTime <= endTime);
}

void CDlgDataSearch::OnNMDblclkListCsvFindFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    //m_vecFilteredRows.clear();

    m_nSelect = pNMItemActivate->iItem;
    int nSelectRow = ListRowSelect(IDC_LIST_1, pNMItemActivate);
    m_listCtrlCsvList.SetItemState(nSelectRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    m_listCtrlCsvList.SetSelectionMark(nSelectRow);
    m_listCtrlCsvList.EnsureVisible(nSelectRow, FALSE);
    m_vecvecInfoList.clear();

    LoadCSVFile(m_vecCSVFilePath[nSelectRow]);

    *pResult = 0;
}

void CDlgDataSearch::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    ::GetWindowRect(GetSafeHwnd(), m_rtDlgSize);
    GetClientRect(m_rtDlgSize);
    TotalControlResize();
}

void CDlgDataSearch::TotalControlResize()
{
    ControlSizeChange(IDC_STATIC_DETAIL_DATASEARCH);
    ControlSizeChange(IDC_DATETIMEPICKER_START_DAY);
    ControlSizeChange(IDC_DATETIMEPICKER_START_TIME);
    ControlSizeChange(IDC_DATETIMEPICKER_END_DAY);
    ControlSizeChange(IDC_DATETIMEPICKER_END_TIME);
    ControlSizeChange(IDC_EDIT_SEARCH_BLANK);
    ControlSizeChange(IDC_BUTTON_FIND);
    ControlSizeChange(IDC_RADIO_RAW);
    ControlSizeChange(IDC_RADIO_LOT);
    ControlSizeChange(IDC_LIST_CSV_FIND_FILE);
    ControlSizeChange(IDC_LIST_CSV_INFO);
    ControlSizeChange(IDC_LIST_SELECT_INFO);
    ControlSizeChange(IDC_STATIC_IMAGE_VIEW_BRIGHT);
    ControlSizeChange(IDC_STATIC_IMAGE_VIEW_DARK); //
    ControlSizeChange(IDC_STATIC_DEFECT_INFO);
    ControlSizeChange(IDC_BUTTON_EXPORT);
    ControlSizeChange(IDC_BUTTON_INPUT);
    ControlSizeChange(IDC_BUTTON_OUTPUT);

    ControlSizeChange(IDC_STATIC_STATUS); // 1010
    ControlSizeChange(IDC_STATIC_AVG_GV); // 1011
    ControlSizeChange(IDC_STATIC_MIN_GV); // 1012
    ControlSizeChange(IDC_STATIC_MAX_GV); // 1013
    ControlSizeChange(IDC_STATIC_BRIGHT_AVG); // 1014

    ControlSizeChange(IDC_STATIC_DARK_GV); // 1016
    ControlSizeChange(IDC_STATIC_SIZE_X); // 1017
    ControlSizeChange(IDC_STATIC_SIZE_Y); // 1018
    ControlSizeChange(IDC_STATIC_RATIO_AREA); // 1019
    ControlSizeChange(IDC_STATIC_RATIO_YX); // 1020

    ControlSizeChange(IDC_STATIC_RATIO_XY);
    ControlSizeChange(IDC_STATIC_COMPACT);
    ControlSizeChange(IDC_STATIC_RATIO_WHITE);
    ControlSizeChange(IDC_STATIC_RATIO_DARK);
    ControlSizeChange(IDC_STATIC_EDGE_ENERGY);

    ControlSizeChange(IDC_STATIC_MASS_CENTER);
    ControlSizeChange(IDC_STATIC_LOTID);
    ControlSizeChange(IDC_STATIC_CELLID);
    ControlSizeChange(IDC_STATIC_TIME);
    ControlSizeChange(IDC_STATIC_DEFECT_TYPE);

    ControlSizeChange(IDC_STATIC_TOP_BACK);
    ControlSizeChange(IDC_STATIC_DEFECT_X);
    ControlSizeChange(IDC_STATIC_DEFECT_Y);
    ControlSizeChange(IDC_STATIC_JUDGE);
    ControlSizeChange(IDC_CHECK_CROP);
    ControlSizeChange(IDC_BUTTON_RESET);

    ControlSizeChange(IDC_BUTTON_EXCELCLEAR);
    ControlSizeChange(IDC_RADIO_NONE);

    ControlSizeChange(IDC_BUTTON_DEFECTCSVEXPORT);
    ControlSizeChange(IDC_LIST_DEFECTTIMETABLE);

    ControlSizeChange(IDC_CHECK_DEFECTPOSIMAGEADD);
}

void CDlgDataSearch::ControlSizeChange(int nID)
{
    CRect rectCtl;
    HWND hwnd_ID = ::GetDlgItem(this->m_hWnd, nID);
    ::GetClientRect(hwnd_ID, &rectCtl);
    long cx = m_rtDlgSize.Width() * 0.01;
    long cy = m_rtDlgSize.Height() * 0.01;

    CWnd* pCtl;
    pCtl = GetDlgItem(nID);




    if (nID == IDC_STATIC_IMAGE_VIEW_BRIGHT)
    {
        if (pCtl != nullptr)
        {
            pCtl->MoveWindow(cx * 62, cy * 43, cx * 15, cy * 20, TRUE);
            pCtl->ShowWindow(SW_SHOW);
            ::MoveWindow(m_pImageView[0]->GetSafeHwnd(), cx * 62, cy * 43, cx * 15, cy * 20, TRUE);
            m_pImageView[0]->ZoomImageFit();
        }
    }

    if (nID == IDC_STATIC_IMAGE_VIEW_DARK)
    {
        if (pCtl != nullptr)
        {
            pCtl->MoveWindow(cx * 62, cy * 66, cx * 15, cy * 20, TRUE);
            pCtl->ShowWindow(SW_SHOW);
            ::MoveWindow(m_pImageView[1]->GetSafeHwnd(), cx * 62, cy * 66, cx * 15, cy * 20, TRUE);
            m_pImageView[1]->ZoomImageFit();
        }
    }

    if (nID == IDC_STATIC_DETAIL_DATASEARCH)
    {
        ::MoveWindow(hwnd_ID, cx * 2, cy * 17, cx *40, cy * 20, TRUE);
    }

    if (nID == IDC_DATETIMEPICKER_START_DAY)
    {
        ::MoveWindow(hwnd_ID, cx * 3, cy * 2, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_DATETIMEPICKER_START_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 15, cy * 2, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_DATETIMEPICKER_END_DAY)
    {
        ::MoveWindow(hwnd_ID, cx * 3, cy * 7, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_DATETIMEPICKER_END_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 15, cy * 7, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_EDIT_SEARCH_BLANK)
    {
        ::MoveWindow(hwnd_ID, cx * 30, cy * 2, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_BUTTON_FIND)
    {
        ::MoveWindow(hwnd_ID, cx * 42, cy * 2, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_RADIO_RAW)
    {
        ::MoveWindow(hwnd_ID, cx * 3, cy * 12, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_RADIO_LOT)
    {
        ::MoveWindow(hwnd_ID, cx * 14, cy * 12, cx * 10, cy * 3, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID==IDC_RADIO_NONE)
    {
        ::MoveWindow(hwnd_ID, cx * 25, cy * 12, cx * 10, cy * 3, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_LIST_CSV_FIND_FILE)
    {
        ::MoveWindow(hwnd_ID, cx * 2, cy * 42, cx * 25, cy * 45, TRUE);
    }

    if (nID == IDC_LIST_CSV_INFO)
    {
        ::MoveWindow(hwnd_ID, cx * 30, cy * 42, cx * 30, cy * 45, TRUE);
    }

    if (nID == IDC_LIST_SELECT_INFO)
    {
        ::MoveWindow(hwnd_ID, cx * 2, cy * 90, cx * 99, cy * 11.5, TRUE);
    }

    if (nID == IDC_LIST_DEFECTTIMETABLE)
    {
        ::MoveWindow(hwnd_ID, cx * 78, cy * 39, cx * 24, cy * 46, TRUE);
        //::MoveWindow(hwnd_ID, cx * 77, cy * 66, cx * 99, cy * 20, TRUE);
        //::MoveWindow(hwnd_ID, cx * 57, cy * 40, cx * 10, cy * 3, TRUE);
    }
    if (nID == IDC_STATIC_DEFECT_INFO)
    {
        ::MoveWindow(hwnd_ID, cx * 42, cy * 12, cx * 45, cy * 26, TRUE); //50
    }

    if (nID == IDC_STATIC_STATUS) //
    {
        ::MoveWindow(hwnd_ID, cx * 44, cy * 14, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_AVG_GV)
    {
        ::MoveWindow(hwnd_ID, cx * 44, cy * 19, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_DEFECT_X)
    {
        ::MoveWindow(hwnd_ID, cx * 44, cy * 24, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_RATIO_WHITE)
    {
        ::MoveWindow(hwnd_ID, cx * 44, cy * 29, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 44, cy * 34, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_SIZE_X)
    {
        ::MoveWindow(hwnd_ID, cx * 54, cy * 14, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_MIN_GV)
    {
        ::MoveWindow(hwnd_ID, cx * 54, cy * 19, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_DEFECT_Y)
    {
        ::MoveWindow(hwnd_ID, cx * 54, cy * 24, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_RATIO_DARK)
    {
        ::MoveWindow(hwnd_ID, cx * 54, cy * 29, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_EDGE_ENERGY)
    {
        ::MoveWindow(hwnd_ID, cx * 54, cy * 34, cx * 10, cy * 3, TRUE);
    }
    if (nID == IDC_STATIC_SIZE_Y)
    {
        ::MoveWindow(hwnd_ID, cx * 64, cy * 14, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_MAX_GV)
    {
        ::MoveWindow(hwnd_ID, cx * 64, cy * 19, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_TOP_BACK)
    {
        ::MoveWindow(hwnd_ID, cx * 64, cy * 24, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_RATIO_XY)
    {
        ::MoveWindow(hwnd_ID, cx * 64, cy * 29, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_MASS_CENTER)
    {
        ::MoveWindow(hwnd_ID, cx * 64, cy * 34, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_LOTID)
    {
        ::MoveWindow(hwnd_ID, cx * 74, cy * 14, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_BRIGHT_AVG)
    {
        ::MoveWindow(hwnd_ID, cx * 74, cy * 19, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_COMPACT)
    {
        ::MoveWindow(hwnd_ID, cx * 74, cy * 24, cx * 10, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_RATIO_YX)
    {
        ::MoveWindow(hwnd_ID, cx * 74, cy * 29, cx * 8, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_JUDGE)
    {
        ::MoveWindow(hwnd_ID, cx * 74, cy * 34, cx * 10, cy * 3, TRUE);
    }
    //
    if (nID == IDC_STATIC_CELLID)
    {
        ::MoveWindow(hwnd_ID, cx * 83, cy * 14, cx * 5, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_DARK_GV)
    {
        ::MoveWindow(hwnd_ID, cx * 83, cy * 19, cx * 6, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_DEFECT_TYPE)
    {
        ::MoveWindow(hwnd_ID, cx * 83, cy * 24, cx * 8, cy * 3, TRUE);
    }

    if (nID == IDC_STATIC_RATIO_AREA)
    {
        ::MoveWindow(hwnd_ID, cx * 83, cy * 29, cx * 6, cy * 3, TRUE);
    }
    //
    if (nID == IDC_BUTTON_EXPORT)
    {
        ::MoveWindow(hwnd_ID, cx * 102, cy * 90, cx * 4, cy * 5, TRUE);
    }
    if (nID == IDC_BUTTON_EXCELCLEAR)
    {
        ::MoveWindow(hwnd_ID, cx * 102, cy * 96, cx * 4, cy * 5, TRUE);
    }
    if (nID == IDC_BUTTON_INPUT)
    {
        ::MoveWindow(hwnd_ID, cx * 50, cy * 87.5, cx * 5, cy * 2, TRUE);
    }

    if (nID == IDC_BUTTON_OUTPUT)
    {
        ::MoveWindow(hwnd_ID, cx * 56, cy * 87.5, cx * 5, cy * 2, TRUE);
    }

    if (nID == IDC_CHECK_CROP)
    {
        ::MoveWindow(hwnd_ID, cx * 100, cy * 87.5, cx * 7, cy * 2, TRUE);
    }
    if (nID == IDC_BUTTON_RESET)
    {
        ::MoveWindow(hwnd_ID, cx * 53, cy * 2, cx * 10, cy * 3, TRUE);
    }

    
    if (nID == IDC_BUTTON_DEFECTCSVEXPORT)
    {
        if (pCtl != nullptr)
        {
            ::MoveWindow(hwnd_ID, cx * 70, cy * 39, cx * 5, cy * 3, TRUE);
            pCtl->ShowWindow(SW_SHOW);
        }
    }

    if (nID == IDC_CHECK_DEFECTPOSIMAGEADD)
    {
        if (pCtl != nullptr)
        {
            ::MoveWindow(hwnd_ID, cx * 62, cy * 39, cx * 7, cy * 3, TRUE);
            pCtl->ShowWindow(SW_SHOW);
        }
    }
    
}

void CDlgDataSearch::OnBnClickedRadioRaw()
{
    m_pDlgLotSearch->ShowWindow(SW_HIDE);
    m_pDlgDefectRawSearch->ShowWindow(SW_SHOW);
    m_nRadioID = GetCheckedRadioButton(IDC_RADIO_RAW, IDC_RADIO_LOT);
    SetDlgItemText(IDC_EDIT_SEARCH_BLANK, _T(""));
}

void CDlgDataSearch::OnBnClickedRadioLot()
{
    m_pDlgDefectRawSearch->ShowWindow(SW_HIDE);
    m_pDlgLotSearch->ShowWindow(SW_SHOW);
    m_nRadioID = GetCheckedRadioButton(IDC_RADIO_RAW, IDC_RADIO_LOT);
}

void CDlgDataSearch::LoadCSVFile(CString strCSVPath)
{
    // info List 정보 뿌려주는 함수
    m_listCtrlCsvInfo.DeleteAllItems();
    long nRowIndex = 0;
    std::vector<CString> vecTimeTableCSVPath;
    std::vector<CString> vecTimeTableTIME;
    for (long nIndex = 0; nIndex<m_vecFilterRawInfo.size();nIndex++)
    {
        vecTimeTableCSVPath.push_back(m_vecFilterRawInfo[nIndex].strCSVPath);
        //vecTimeTableTIME.push_back(m_vecFilterRawInfo[nIndex].strTime);
        if (strCSVPath == m_vecFilterRawInfo[nIndex].strCSVPath)
        {
            long nRow = m_vecFilterRawInfo[nIndex].nCSVRow;

            m_vecvecRowColInfo = m_vecFilterRawInfo[nIndex].vecvecRowColInfo;
            m_vecvecInfoList.push_back(m_vecFilterRawInfo[nIndex].vecvecRowColInfo[nRow]);
            for (long nCol = 0; nCol < m_vecvecRowColInfo[nRow].size(); nCol++)
            {
                CString strData = m_vecvecRowColInfo[nRow][nCol];
                if (nCol == 0)
                {
                    m_listCtrlCsvInfo.InsertItem(nRowIndex, strData);
                }
                else
                {
                    m_listCtrlCsvInfo.SetItem(nRowIndex, nCol, LVIF_TEXT, strData, 0, 0, 0, NULL);
                }
                if (nCol == 18) // 시간
                {
                    vecTimeTableTIME.push_back(strData);
                }
            }
            nRowIndex++;
        }
    }

}

void CDlgDataSearch::GetImagePath(long nSel, CString& strBrightImagePath, CString& strDarkImagePath)
{
    if (nSel == -1)
    {
        return;
    }

    if (m_vecvecInfoList.size() > 0)
    {
        for (long nLoad = 0; nLoad < m_vecvecInfoList[nSel].size(); nLoad++)
        {
            CString strData = m_vecvecInfoList[nSel][nLoad];

            // 영상 경로 마지막에 jpg,bmp를 알려주니까 이걸로 확인 하자
            if (strData.Right(3) == _T("jpg") || strData.Right(3) == _T("bmp"))
            {
                // 시스템 경로를 갖고 오도록 수정 필요
                CString strSystemPath = _T("D:\\Load");
                strSystemPath = m_strSystemCSVPath;

                CString strCompare = strData.Mid(strData.ReverseFind('_') + 1, strData.GetLength());
                strCompare = strCompare.Mid(0, strCompare.ReverseFind('.'));
                // Bright 랑 Dark 를 구분해서 넣자
                if (strCompare == _T("Bright"))
                {
                    strBrightImagePath = strSystemPath + strData;
                }
                else if (strCompare == _T("Dark"))
                {
                    strDarkImagePath = strSystemPath + strData;

                }
            }
        }
    }
}

BOOL CDlgDataSearch::ImageLoad(CString strBrightImagePath, CString strDarkImagePath)
{
    CFileFind finder;
    BOOL bFindBright = finder.FindFile(strBrightImagePath);
    BOOL bFindDark = finder.FindFile(strDarkImagePath);
    BOOL bCheck;

    if (bFindBright == FALSE || bFindDark == FALSE)
    {
        IPVM::Image_8u_C1 imgNothing;

        m_pImageView[0]->SetImage(imgNothing);
        m_pImageView[1]->SetImage(imgNothing);

        AfxMessageBox(_T("Can't Find Image File!"));

        bCheck = FALSE;
    }

    else
    {
        IPVM::Image_8u_C1 BrightImage;
        IPVM::Image_8u_C1 DarkImage;

        BrightImage.LoadFrom(strBrightImagePath);
        m_pImageView[0]->SetImage(BrightImage);

        DarkImage.LoadFrom(strDarkImagePath);
        m_pImageView[1]->SetImage(DarkImage);

        bCheck = TRUE;
    }

    return bCheck;
}

void CDlgDataSearch::OnNMDblclkListCsvInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;
    long nRow = pNMItemActivate->iItem; // 전역변수로 할까말까 고민중
    int nCol = pNMItemActivate->iSubItem;

    int controlIDs[] = {IDC_STATIC_STATUS, IDC_STATIC_AVG_GV, IDC_STATIC_MIN_GV, IDC_STATIC_MAX_GV,
        IDC_STATIC_BRIGHT_AVG, IDC_STATIC_DARK_GV, IDC_STATIC_SIZE_X, IDC_STATIC_SIZE_Y, IDC_STATIC_RATIO_AREA,
        IDC_STATIC_RATIO_YX, IDC_STATIC_RATIO_XY, IDC_STATIC_COMPACT, IDC_STATIC_RATIO_WHITE, IDC_STATIC_RATIO_DARK,
        IDC_STATIC_EDGE_ENERGY, IDC_STATIC_MASS_CENTER, IDC_STATIC_LOTID, IDC_STATIC_CELLID, IDC_STATIC_TIME,
        IDC_STATIC_DEFECT_TYPE, IDC_STATIC_TOP_BACK, IDC_STATIC_DEFECT_X, IDC_STATIC_DEFECT_Y, IDC_STATIC_JUDGE};

    CString strBrightImagePath;
    CString strDarkImagePath;
    CString strTemp;
    long nType;
    CString strFindLotID;
    GetDlgItemText(IDC_EDIT_SEARCH_BLANK, strFindLotID);

    int nSelectRow = ListRowSelect(IDC_LIST_CSV_INFO,pNMItemActivate);
    m_listCtrlCsvInfo.SetItemState(nSelectRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    m_listCtrlCsvInfo.SetSelectionMark(nSelectRow);
    m_listCtrlCsvInfo.EnsureVisible(nSelectRow, FALSE);

    GetImagePath(nSelectRow, strBrightImagePath, strDarkImagePath);

    BOOL bImageCheck = ImageLoad(strBrightImagePath, strDarkImagePath);

    if (bImageCheck == FALSE)
    {
        for (int i = 0; i < sizeof(controlIDs) / sizeof(controlIDs[0]); i++)
        {
            // 정보 없으면 초기화
            SetDlgItemTextW(controlIDs[i], m_vecTitle[i] + _T(" : "));
        }
        return;
    }

    if (m_vecvecInfoList.size() > 0 && nSelectRow != -1)
    {
        for (long n = 0; n < m_vecTitle.size(); n++)
        {
            CString strInfo = m_vecvecInfoList[nSelectRow][n];
            CString strText = m_vecTitle[n] + _T(" : ") + _T("\n") + strInfo;

            if (!strText.Find(_T("EDGE_"), 0))
            {
                strText = m_vecTitle[n] + _T(" : ") + _T("\n") + strInfo;
                SetDlgItemTextW(controlIDs[n], strText);
            }

            else if (!strText.Find(_T("DefectType"), 0))
            {
                nType = _ttoi(strInfo);
                CString str = GetDefectName((DefectType)nType);
                strTemp = m_vecTitle[n] + _T(" : ") + _T("\n") + str + _T("(") + strInfo + _T(")");
                SetDlgItemTextW(controlIDs[n], strTemp);
            }

            else if (!strText.Find(_T("Judge"), 0))
            {
                if (strInfo == _T("2"))
                {
                    strTemp = m_vecTitle[n] + _T(" : ") + _T("\n") + _T("Reject") + _T("(") + strInfo + _T(")");
                }
                else
                {
                    strTemp = m_vecTitle[n] + _T(" : ") + _T("\n") + _T("Pass") + _T("(") + strInfo + _T(")");
                }
                SetDlgItemTextW(controlIDs[n], strTemp);
            }

            else
            {
                SetDlgItemTextW(controlIDs[n], strText);
            }
        }
    }

    for (int i = 0; i < sizeof(controlIDs) / sizeof(controlIDs[0]); i++)
    {
        GetDlgItem(controlIDs[i])->SetFont(&m_ListFont);
    }
    
}

BOOL CDlgDataSearch::StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue)
{
    long nDivideTotalCount = 0;
    CString strSub;
    while (FALSE != AfxExtractSubString(strSub, strFull, nDivideTotalCount++, chDivide))
    {
        vecstrValue.push_back(strSub);
    }
    return TRUE;
}

void CDlgDataSearch::OnNMClickListCsvInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    m_bInputCheck = TRUE;
    m_nSelectInfo = ListRowSelect(IDC_LIST_CSV_INFO, pNMItemActivate);
    m_listCtrlCsvInfo.SetItemState(m_nSelectInfo, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    m_listCtrlCsvInfo.SetSelectionMark(m_nSelectInfo);
    m_listCtrlCsvInfo.EnsureVisible(m_nSelectInfo, FALSE);
    *pResult = 0;
}

void CDlgDataSearch::OnBnClickedButtonInput()
{
    if (m_bInputCheck == FALSE)
    {
        return;
    }
    else
    {
        long nSel = m_nSelectInfo;
        CString strData, strText;
        int nRow = 0;
        std::vector<CString> vecTemp;
        strData.Format(_T("%d"), nRow);
        m_listCtrlExcelSelect.InsertItem(nSel + 1, strData);

        for (long n = 0; n < m_vecvecInfoList[0].size(); n++)
        {
            strText = m_vecvecInfoList[nSel][n]; 
            m_listCtrlExcelSelect.SetItem(nRow, n + 1, LVIF_TEXT, strText, 0, 0, 0, NULL);
            vecTemp.push_back(m_vecvecInfoList[nSel][n]); 
        }
        m_vecAllData.push_back(vecTemp);

        UpdateListCtrl();

        nRow++;
        m_bInputCheck = FALSE;
    }
}

void CDlgDataSearch::OnBnClickedButtonOutput()
{
    if (m_vecAllData.size() == 0 || m_nSelectInfo == -1)
    {
        return;
    }

    else
    {
        if (m_nExcelSelectInfo==1)
        {
            m_nExcelSelectInfo = 0;
        }
        m_vecAllData.erase(m_vecAllData.begin() + m_nExcelSelectInfo);
        UpdateListCtrl();
    }
}

void CDlgDataSearch::UpdateListCtrl()
{
    m_listCtrlExcelSelect.DeleteAllItems();
    CString strNum;
    for (long i = 0; i < m_vecAllData.size(); i++)
    {
        strNum.Format(_T("%d"), i);
        m_listCtrlExcelSelect.InsertItem(i, strNum);
        for (long j = 0; j < m_vecAllData[i].size(); j++)
        {
            m_listCtrlExcelSelect.SetItem(i, j + 1, LVIF_TEXT, m_vecAllData[i][j], 0, 0, 0, NULL);
        }
    }
}

void CDlgDataSearch::OnNMClickListSelectInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    //m_nExcelSelectInfo = pNMItemActivate->iItem;

    m_nExcelSelectInfo = ListRowSelect(IDC_LIST_SELECT_INFO, pNMItemActivate);
    m_listCtrlExcelSelect.SetItemState(m_nExcelSelectInfo, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    m_listCtrlExcelSelect.SetSelectionMark(m_nExcelSelectInfo);
    m_listCtrlExcelSelect.EnsureVisible(m_nExcelSelectInfo, FALSE);

    *pResult = 0;
}

void CDlgDataSearch::OnBnClickedButtonExport()
{
    ExcelRejectDataInput();
}

void CDlgDataSearch::ExcelRejectDataInput()
{
    vector<CString> vecData;

    long nImageSizeX = 100;
    long nImageSizeY = 100;
    m_ExcelSave.Create(CRect(0, 0, nImageSizeX, nImageSizeY), 1);

    CFileDialog SaveDlg(FALSE, _T("xlsx"), _T(""), OFN_HIDEREADONLY, _T("xlsx Files (*.xlsx)|*.xlsx||"));

    if (SaveDlg.DoModal() != IDOK)
    {
        return;
    }

    CString strPath, strDirecPath, strFileName, strFullPath, strName;
    strPath = SaveDlg.GetPathName();
    strDirecPath = strPath.Left(strPath.ReverseFind('\\') + 1);

    strName = SaveDlg.GetFileName();
    strFileName = SaveDlg.GetFileName();

    strFullPath = strDirecPath + strName;

    if (m_checkCropSave.GetCheck() == TRUE)
    {
        vecData.push_back(_T("Num"));
        vecData.push_back(_T("Status"));
        vecData.push_back(_T("Bright_Image"));
        vecData.push_back(_T("Dark_Image"));
        vecData.push_back(_T("AVG_GV"));
        vecData.push_back(_T("MIN_AVG"));
        vecData.push_back(_T("MAX_AVG"));
        vecData.push_back(_T("BRIGHT_AVG"));
        vecData.push_back(_T("DARK_AVG"));
        vecData.push_back(_T("SIZE_X"));
        vecData.push_back(_T("SIZE_Y"));
        vecData.push_back(_T("AREA_RATIO"));
        vecData.push_back(_T("RATIO_YX"));
        vecData.push_back(_T("RATIO_XY"));
        vecData.push_back(_T("COMPACT"));
        vecData.push_back(_T("RATIO_WHITE"));
        vecData.push_back(_T("RATIO_DARK"));
        vecData.push_back(_T("EDGE_ENERGY"));
        vecData.push_back(_T("MASS_CENTER_GV"));
        vecData.push_back(_T("LotID"));
        vecData.push_back(_T("CellID"));
        vecData.push_back(_T("Time"));
        vecData.push_back(_T("DefectType"));
        vecData.push_back(_T("TopBack"));
        vecData.push_back(_T("Defect_X"));
        vecData.push_back(_T("Defect_Y"));
        vecData.push_back(_T("Judge"));
        vecData.push_back(_T("BrightImagePath"));
        vecData.push_back(_T("DarkImagePath"));

        m_ExcelSave.RejectListName(vecData);
        m_nExcelRow = 0;
        //if (CreateDirectory(strDirecPath + strFolder, nullptr))
        {
            IPVM::Image_8u_C1 BrightImage, DarkImage;

           CString strSystemPath = m_strSystemCSVPath; //_T("D:\\Load");
            std::vector<CString> vecDefectName = {_T("Test")};

            CString strBrightPath, strDarkPath;
            for (long n = 0; n < m_vecAllData.size(); n++)
            {
                CString strImagePath
                    = m_vecAllData[n][16] + _T("_") + m_vecAllData[n][17] + _T("_") + m_vecAllData[n][18];
                CString strCount;
                strCount.Format(_T("%d"), n);

                strBrightPath = strSystemPath + m_vecAllData[n][24];
                BrightImage.LoadFrom(strBrightPath);
                IPVM::Rect OriginRect(BrightImage);
                //BrightImage.SaveJpeg(strDirecPath + strFolder + _T("\\") + strImagePath + strCount+ _T("_Bright.jpg"));

                strDarkPath = strSystemPath + m_vecAllData[n][25];
                DarkImage.LoadFrom(strDarkPath);
                //DarkImage.SaveJpeg(strDirecPath + strFolder + _T("\\") + strImagePath + strCount+ _T("_Dark.jpg"));

                m_ExcelSave.SetBrightImage(BrightImage);
                m_ExcelSave.SetDarkImage(DarkImage);
                m_nExcelRow += 1;

                vector<CString> vecResultData;
                CString strResult, ResultName;
                ResultName = _T("Reject");

                for (long nSub = 0; nSub < m_vecAllData[0].size() - 4; nSub++)
                {
                    strResult = m_vecAllData[n][nSub];
                    vecResultData.push_back(strResult);
                }

                m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkPath, strBrightPath, vecResultData,
                    ResultName, OriginRect, n);
            }
            m_ExcelSave.Savexlsfile(strFullPath, FALSE);
            return;
        }
    }
    else
    {
        vecData.push_back(_T("Num"));
        vecData.push_back(_T("Status"));
        vecData.push_back(_T("AVG_GV"));
        vecData.push_back(_T("MIN_AVG"));
        vecData.push_back(_T("MAX_AVG"));
        vecData.push_back(_T("BRIGHT_AVG"));
        vecData.push_back(_T("DARK_AVG"));
        vecData.push_back(_T("SIZE_X"));
        vecData.push_back(_T("SIZE_Y"));
        vecData.push_back(_T("AREA_RATIO"));
        vecData.push_back(_T("RATIO_YX"));
        vecData.push_back(_T("RATIO_XY"));
        vecData.push_back(_T("COMPACT"));
        vecData.push_back(_T("RATIO_WHITE"));
        vecData.push_back(_T("RATIO_DARK"));
        vecData.push_back(_T("EDGE_ENERGY"));
        vecData.push_back(_T("MASS_CENTER_GV"));
        vecData.push_back(_T("LotID"));
        vecData.push_back(_T("CellID"));
        vecData.push_back(_T("Time"));
        vecData.push_back(_T("DefectType"));
        vecData.push_back(_T("TopBack"));
        vecData.push_back(_T("Defect_X"));
        vecData.push_back(_T("Defect_Y"));
        vecData.push_back(_T("Judge"));
        vecData.push_back(_T("BrightImagePath"));
        vecData.push_back(_T("DarkImagePath"));

        m_ExcelSave.RejectListName(vecData);

        IPVM::Image_8u_C1 BrightImage, DarkImage;
        //CString strSystemPath = _T("D:\\Load");
        std::vector<CString> vecDefectName = {_T("Test")};
        m_nExcelRow = 0;

        CString strBrightPath, strDarkPath;
        for (long n = 0; n < m_vecAllData.size(); n++)
        {
            m_nExcelRow += 1;

            vector<CString> vecResultData;
            CString strResult, ResultName;
            ResultName = _T("Reject");
            for (long nSub = 0; nSub < m_vecAllData[0].size() - 4; nSub++)
            {
                strResult = m_vecAllData[n][nSub];
                vecResultData.push_back(strResult);
            }
            m_ExcelSave.AddTextDatas(m_nExcelRow, vecDefectName[0], vecResultData, ResultName, n);
        }
        m_ExcelSave.Savexlsfile(strPath, FALSE);
        return;
    }
}

void CDlgDataSearch::OnBnClickedButtonReset()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int controlIDs[] = {IDC_STATIC_STATUS, IDC_STATIC_AVG_GV, IDC_STATIC_MIN_GV, IDC_STATIC_MAX_GV,
        IDC_STATIC_BRIGHT_AVG, IDC_STATIC_DARK_GV, IDC_STATIC_SIZE_X, IDC_STATIC_SIZE_Y, IDC_STATIC_RATIO_AREA,
        IDC_STATIC_RATIO_YX, IDC_STATIC_RATIO_XY, IDC_STATIC_COMPACT, IDC_STATIC_RATIO_WHITE, IDC_STATIC_RATIO_DARK,
        IDC_STATIC_EDGE_ENERGY, IDC_STATIC_MASS_CENTER, IDC_STATIC_LOTID, IDC_STATIC_CELLID, IDC_STATIC_TIME,
        IDC_STATIC_DEFECT_TYPE, IDC_STATIC_TOP_BACK, IDC_STATIC_DEFECT_X, IDC_STATIC_DEFECT_Y, IDC_STATIC_JUDGE};

    for (int i = 0; i < sizeof(controlIDs) / sizeof(controlIDs[0]); i++)
    {
        SetDlgItemTextW(controlIDs[i], m_vecTitle[i] + _T(" : "));
        GetDlgItem(controlIDs[i])->SetFont(&m_ListFont);
    }

    m_listCtrlCsvList.DeleteAllItems();
    m_listCtrlCsvInfo.DeleteAllItems();
    m_nSelect = -1;
    m_nSelectInfo = -1;
    m_nExcelSelectInfo = -1;
    m_vecAllData.clear();
    m_vecCSVFilePath.clear();
    m_vecFilteredRows.clear();
    m_vecRawInfo.clear();
    m_vecLotInfo.clear();
    m_vecFilterRawInfo.clear();
    m_vecvecRowColInfo.clear();
    // Lot
    //if (m_nRadioID == IDC_RADIO_LOT)
    //{
    //    m_pDlgLotSearch->SetDlgItemText(IDC_EDIT_LOT_FINAL_METER, _T(""));
    //    m_pDlgLotSearch->SetDlgItemText(IDC_EDIT_TOTAL_DEFECT_NUM, _T(""));
    //    m_pDlgLotSearch->SetDlgItemText(IDC_EDIT_LAIN_NUM, _T(""));
    //    m_pDlgLotSearch->SetDlgItemText(IDC_COMBO_DEFECT_NAME, _T(""));
    //    m_pDlgLotSearch->SetDlgItemText(IDC_EDIT_DIMENSION_TYPE, _T(""));
    //    m_pDlgLotSearch->SetDlgItemText(IDC_EDIT_TOTAL_LAIN_NUM2, _T(""));
    //    m_pDlgLotSearch->SetDlgItemText(IDC_EDIT_TOTAL_PPK, _T(""));
    //    m_pDlgLotSearch->SetDlgItemText(IDC_EDIT_RECIPE, _T(""));
    //}

    // Raw
    //if (m_nRadioID == IDC_RADIO_RAW)
    {
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_PATTERN_NUM, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_COMBO_CAM_INDEX, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_COMBO_ELECTRODE_TYPE, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_COMBO_RAW_DEFECT_NAME, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_MODE, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_LAIN_NUM, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_RECIPE, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_METER, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_WIDTH_LENGTH, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_AREA, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_X_SIZE, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_Y_SIZE, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_DEFECT_X, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_DEFECT_Y, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_MAX_GV, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_MIN_GV, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_AVG_GV, _T(""));
        m_pDlgDefectRawSearch->SetDlgItemText(IDC_EDIT_RAW_DEFECT_AREA_GV, _T(""));
    }

    IPVM::Image_8u_C1 ClearImage;
    m_pImageView[0]->SetImage(ClearImage);
    m_pImageView[1]->SetImage(ClearImage);

    SetDlgItemText(IDC_EDIT_SEARCH_BLANK, _T(""));
}

void CDlgDataSearch::OnBnClickedButtonExcelclear()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_listCtrlExcelSelect.DeleteAllItems();
    m_vecAllData.clear();
    m_nExcelRow = 0;
}

long CDlgDataSearch::LotValueCheck(LotDefectInfo Lotinfo)
{
    long nCnt = 0;
    if (!Lotinfo.strLotID.IsEmpty())
    {
        nCnt++;
    }
    if (!Lotinfo.strLotStartTime.IsEmpty())
    {
        nCnt++;
    }
    if (!Lotinfo.strLotEndTime.IsEmpty())
    {
        nCnt++;
    }
    if (!Lotinfo.strRecipe.IsEmpty())
    {
        nCnt++;
    }
    if (!Lotinfo.strTotalMeter.IsEmpty())
    {
        nCnt++;
    }
    //if (!Lotinfo.strDefectName.IsEmpty())
    //{
    //    nCnt++;
    //}
    if (!Lotinfo.strTotalNum.IsEmpty())
    {
        nCnt++;
    }
    if (!Lotinfo.strLain.IsEmpty())
    {
        nCnt++;
    }
    if (!Lotinfo.strMachiceType.IsEmpty())
    {
        nCnt++;
    }

    return nCnt;
}

void CDlgDataSearch::LotTxtFileRead(CString strTxtFilePath, std::vector<CString>& vecstrValue)
{
    CString strKeyName = _T("Lot_Info");
    TCHAR szBuffer[256] = {
        NULL,
    };
    LotDefectInfo lotdefect;

    GetPrivateProfileString(strKeyName, _T("LOT_ID"), _T(""), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);
    GetPrivateProfileString(strKeyName, _T("Recipe_Name"), _T(""), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);
    GetPrivateProfileString(strKeyName, _T("LOT_StartTime"), _T(""), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);
    GetPrivateProfileString(strKeyName, _T("LOT_EndTime"), _T(""), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);
    GetPrivateProfileString(strKeyName, _T("Lot_TotalDistance"), _T(""), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);
    GetPrivateProfileString(strKeyName, _T("Machine_Type"), _T(""), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);
    GetPrivateProfileString(strKeyName, _T("TotalDefect"), _T(""), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);
    GetPrivateProfileString(strKeyName, _T("Lane_0"), _T("0"), szBuffer, 256, strTxtFilePath);
    vecstrValue.push_back(szBuffer);

    for (long n = 0; n < m_vecDefectName.size(); n++)
    {
        GetPrivateProfileString(strKeyName, m_vecDefectName[n], _T("0"), szBuffer, 256, strTxtFilePath);
        if (_tcscmp(szBuffer, _T("0")) != 0)
        {
            vecstrValue.push_back(szBuffer);
        }
    }
}

void CDlgDataSearch::LotSummaryInfo(CString strTxtFilePath, std::vector<LotDefectInfo>& vecstrValue)
{
    CString strKeyName = _T("Lot_Info");
    TCHAR szBuffer[256] = {
        NULL,
    };
    LotDefectInfo lotdefect;
    CellDefectInfo cellinfo;
    lotdefect.init();

    GetPrivateProfileString(strKeyName, _T("LOT_ID"), _T(""), szBuffer, 256, strTxtFilePath);
    lotdefect.strLotID = szBuffer;
    GetPrivateProfileString(strKeyName, _T("Recipe_Name"), _T(""), szBuffer, 256, strTxtFilePath);
    lotdefect.strRecipe = szBuffer;
    GetPrivateProfileString(strKeyName, _T("LOT_StartTime"), _T(""), szBuffer, 256, strTxtFilePath);
    lotdefect.strLotStartTime = szBuffer;
    GetPrivateProfileString(strKeyName, _T("LOT_EndTime"), _T(""), szBuffer, 256, strTxtFilePath);
    lotdefect.strLotEndTime = szBuffer;
    GetPrivateProfileString(strKeyName, _T("Lot_TotalDistance"), _T(""), szBuffer, 256, strTxtFilePath);
    lotdefect.strTotalMeter = szBuffer;
    GetPrivateProfileString(strKeyName, _T("Machine_Type"), _T(""), szBuffer, 256, strTxtFilePath);
    lotdefect.strMachiceType = szBuffer;
    GetPrivateProfileString(strKeyName, _T("TotalDefect"), _T(""), szBuffer, 256, strTxtFilePath);
    lotdefect.strTotalNum = szBuffer;
    GetPrivateProfileString(strKeyName, _T("Lane_0"), _T("0"), szBuffer, 256, strTxtFilePath);
    lotdefect.strLain = szBuffer;

    for (long n = 0; n < m_vecDefectName.size(); n++)
    {
        GetPrivateProfileString(strKeyName, m_vecDefectName[n], _T("0"), szBuffer, 256, strTxtFilePath);
        lotdefect.vecDefectData[n].strTotalNum = szBuffer;
    }

    CString strLotFolder;
    strLotFolder = strTxtFilePath.Left(strTxtFilePath.ReverseFind('\\') + 1);
    CFileFind finder;
    std::vector<CString> listPath;
    GatherRecursively(strLotFolder, listPath, _T(".csv"));
    for (long n = 0; n < listPath.size();n++)
    {
        lotdefect.vecIncludeCSVFilePath.push_back(listPath[n]);
        FILE* m_pFile;

        errno_t err = _tfopen_s(&m_pFile, listPath[n], _T("rt,ccs=UNICODE"));

        if (err != 0)
        {
            return;
        }

        CStdioFile m_File(m_pFile);
        CString str;

        std::vector<CString> vecstrDefectInfo;
        while (m_File.ReadString(str))
        {
            if (str != _T(""))
            {
                vecstrDefectInfo.push_back(str);
            }
        }
        m_File.Close();

        std::vector<std::vector<CString>> vecvecRowColInfo;
        vecvecRowColInfo.resize(vecstrDefectInfo.size());
        for (long nRow = 1; nRow < vecstrDefectInfo.size(); nRow++)
        {
            std::vector<CString> vecRowInfo;
            StringTokenDivideToCString(vecstrDefectInfo[nRow], ',', vecvecRowColInfo[nRow]);

            cellinfo.strCSVPath = listPath[n];
            cellinfo.nCSVRow = nRow;
            cellinfo.vecvecRowColInfo = vecvecRowColInfo;
            m_vecFilterRawInfo.push_back(cellinfo);
        }
    }
    vecstrValue.push_back(lotdefect);
}


long CDlgDataSearch::RawValueCheck(CellDefectInfo Rawinfo)
{
    long nCnt = 0;
    if (!Rawinfo.strLotID.IsEmpty())
    {
        nCnt++;
    }
    if (!Rawinfo.strLotStartTime.IsEmpty())
    {
        nCnt++;
    }
    if (!Rawinfo.strLotEndTime.IsEmpty())
    {
        nCnt++;
    }
    if (!Rawinfo.strRecipe.IsEmpty())
    {
        nCnt++;
    }
    if (Rawinfo.nMeter != 0)
    {
        nCnt++;
    }
    if (Rawinfo.nMachineType != 0)
    {
        nCnt++;
    }

    if (Rawinfo.nPatternNum != 0)
    {
        nCnt++;
    }
    if (Rawinfo.nLainNum != 0)
    {
        nCnt++;
    }

    if (Rawinfo.fAVG_GV != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fMIN_GV != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fMAX_GV != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fSize_X != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fSize_Y != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fArea_Ratio != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fDefectX != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fDefectY != 0)
    {
        nCnt++;
    }
    if (Rawinfo.fRatio_XY != 0)
    {
        nCnt++;
    }
    if (Rawinfo.strDefectName != _T(""))
    {
        nCnt++;
    }
    if (Rawinfo.strCamTopBack != _T(""))
    {
        nCnt++;
    }
    return nCnt;
}

void CDlgDataSearch::OverlapDelete(std::vector<CString>& vecTaget)
{
    std::sort(vecTaget.begin(), vecTaget.end());
    vecTaget.erase(std::unique(vecTaget.begin(), vecTaget.end()), vecTaget.end());
}

void CDlgDataSearch::OnBnClickedRadioNone()
{
    m_pDlgDefectRawSearch->ShowWindow(SW_HIDE);
    m_pDlgLotSearch->ShowWindow(SW_HIDE);
}

void CDlgDataSearch::LotSearchMode()
{
    LotDefectInfo lotdefect;
    lotdefect.init();
    GetDlgItemText(IDC_EDIT_SEARCH_BLANK, lotdefect.strLotID);
    //m_pDlgLotSearch->GetDlgItemText(IDC_DATETIMEPICKER_LOT_START_TIME, lotdefect.strLotStartTime);
    //m_pDlgLotSearch->GetDlgItemText(IDC_DATETIMEPICKER_LOT_END_TIME, lotdefect.strLotEndTime);
    m_pDlgLotSearch->GetDlgItemText(IDC_EDIT_LOT_FINAL_METER, lotdefect.strTotalMeter);
    m_pDlgLotSearch->GetDlgItemText(IDC_EDIT_TOTAL_DEFECT_NUM, lotdefect.strTotalNum);
    m_pDlgLotSearch->GetDlgItemText(IDC_EDIT_LAIN_NUM, lotdefect.strLain);
    m_pDlgLotSearch->GetDlgItemText(IDC_COMBO_DEFECT_NAME, lotdefect.strDefectName);
    m_pDlgLotSearch->GetDlgItemText(IDC_EDIT_DIMENSION_TYPE, lotdefect.strMachiceType);
    m_pDlgLotSearch->GetDlgItemText(IDC_EDIT_RECIPE, lotdefect.strRecipe);
    //lotdefect.nTotalNum = m_pDlgLotSearch->GetDlgItemInt(IDC_EDIT_TOTAL_LAIN_NUM2);

    m_nEmpty = LotValueCheck(lotdefect);
    LotDefectInfo lotinfo;
    std::vector<CString> listPath;

    GatherRecursively(m_strSystemCSVPath, listPath, _T(".txt"));

    for (long nIndex = 0; nIndex < listPath.size(); nIndex++)
    {
        std::vector<CString> vecstrLotInfo;
        LotSummaryInfo(listPath[nIndex], m_vecLotInfo);
    }

    for (long nIndex = 0; nIndex < m_vecLotInfo.size();nIndex++)
    {
        for (long nSub = 0; nSub < m_vecLotInfo[nIndex].vecIncludeCSVFilePath.size(); nSub++)
        {
            CString strFilePath = m_vecLotInfo[nIndex].vecIncludeCSVFilePath[nSub];
            m_vecCSVFilePath.push_back(strFilePath);
        }
    }

    for (long nFind = 0; nFind < m_vecCSVFilePath.size(); nFind++)
    {
        CString str;
        str.Format(_T("%d"), nFind + 1);
        m_listCtrlCsvList.InsertItem(nFind + 1, str);
        m_listCtrlCsvList.SetItemText(nFind, 1, m_vecCSVFilePath[nFind]);
    }

}

void CDlgDataSearch::RawSearchMode()
{
    m_list_DefectTimeTable.DeleteAllItems();
    CellDefectInfo rawdefect;
    GetDlgItemText(IDC_EDIT_SEARCH_BLANK, rawdefect.strLotID);
    //m_pDlgDefectRawSearch->GetDlgItemText(IDC_DATETIMEPICKER_RAW_START_TIME, rawdefect.strLotStartTime);
    //m_pDlgDefectRawSearch->GetDlgItemText(IDC_DATETIMEPICKER_RAW_END_TIME, rawdefect.strLotEndTime);

    //m_pDlgDefectRawSearch->GetDlgItemText(IDC_EDIT_RAW_RECIPE, rawdefect.strRecipe);
    m_pDlgDefectRawSearch->GetDlgItemText(IDC_COMBO_RAW_DEFECT_NAME, rawdefect.strDefectName);
    //m_pDlgDefectRawSearch->GetDlgItemText(IDC_COMBO_CAM_INDEX, rawdefect.strCamTopBack);

    CString strCheck;
    rawdefect.nMeter = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_METER);
    rawdefect.nMachineType = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_MODE);
    //rawdefect.nCamIndex = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_COMBO_CAM_INDEX);
    rawdefect.nPatternNum = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_PATTERN_NUM);

    rawdefect.nLainNum = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_LAIN_NUM);
    rawdefect.fAVG_GV = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_AVG_GV);
    GetDlgItemText(IDC_EDIT_RAW_MIN_GV, strCheck);
    if (strCheck == _T(""))
    {
        rawdefect.fMIN_GV = -1.f;
    }
    rawdefect.fMIN_GV = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_MIN_GV);
    rawdefect.fMAX_GV = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_MAX_GV);

    rawdefect.fSize_X = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_X_SIZE);
    rawdefect.fSize_Y = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_Y_SIZE);
    rawdefect.fArea_Ratio = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_AREA);
    //rawdefect.fRatio_YX = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_DEFECT_Y);
    rawdefect.fRatio_XY = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_WIDTH_LENGTH); // 가로대세로비
    rawdefect.fDefectX = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_DEFECT_X);
    rawdefect.fDefectY = m_pDlgDefectRawSearch->GetDlgItemInt(IDC_EDIT_RAW_DEFECT_Y);

    m_nRawEmpty = RawValueCheck(rawdefect);
    std::vector<long> vecIndex;

    // Raw조건 체크
    if (m_nRadioID == IDC_RADIO_RAW)
    {      
        std::vector<CString> listPath;

        CTime tmStartDay, tmEndDay, tmStartTime, tmEndTime;
        m_dateStartMonth.GetTime(tmStartDay);
        m_dateEndMonth.GetTime(tmEndDay);
        m_dateStartTime.GetTime(tmStartTime);
        m_dateEndTime.GetTime(tmEndTime);
        std::vector<CString> listPath_Temp;
        if ((tmStartDay == tmEndDay) && (tmStartTime == tmEndTime))
        {
            GatherRecursively(m_strSystemCSVPath, listPath, _T(".csv"));
        }
        else
        {
            SYSTEMTIME stStartDay, stEndDay, stStartTime, stEndTime;
            m_dateStartMonth.GetTime(&stStartDay);
            m_dateEndMonth.GetTime(&stEndDay);
            m_dateStartTime.GetTime(&stStartTime);
            m_dateEndTime.GetTime(&stEndTime);

            CTime tmStart(stStartDay.wYear, stStartDay.wMonth, stStartDay.wDay, stStartTime.wHour, stStartTime.wMinute,
                stStartTime.wSecond, 0);

            CTime tmEnd(stEndDay.wYear, stEndDay.wMonth, stEndDay.wDay, stEndTime.wHour, stEndTime.wMinute,
                stEndTime.wSecond, 0);

            if (tmStart >= tmEnd)
            {
                MessageBox(_T("끝난 날보다 시작날이 더 큽니다."), _T("재설정"), NULL);
                return;
            }
            GatherRecursively(m_strSystemCSVPath, listPath, _T(".csv"));

            for (long n = 0; n < listPath.size(); n++)
            {
                std::vector<CString> vectemp;
                StringTokenDivideToCString(listPath[n], '\\', vectemp);

                CString strYear = vectemp[2];
                CString strMonth = vectemp[3];
                CString strDay = vectemp[4];

                CTime tmCheck(_ttoi(strYear), _ttoi(strMonth), _ttoi(strDay), 0, 0, 0, 0);

                if ((tmCheck >= tmStart) && (tmCheck <= tmEnd))
                {
                    listPath_Temp.push_back(listPath[n]);
                }
            }
            listPath = listPath_Temp;
        }
        //24.10.07 JJS 추가
        m_vecvecDefectInfo.clear();
        m_vecvecDefectInfo.resize(listPath.size());
        //m_vecDefectInfo.clear();
        for (long nSize = 0; nSize < m_vecvecDefectInfo.size(); nSize++)
        {
#if INSPECTION_MACHINE_UC1
            for (long nDefect = (long)DefectType::White_Line; nDefect < (long)DefectType::Edge_Defect; nDefect++)
            {
                //vecvecDefectInfo[nSize].push_back(m_vecDefectInfo[nDefect]);
                CString strDefecntName;
                DefectInfo Defect;
                strDefecntName = GetDefectName((DefectType)nDefect);
                Defect.strDefecntName = strDefecntName;
                Defect.nDefectNum = 0;
                Defect.strCSVPath = _T("");
                m_vecvecDefectInfo[nSize].push_back(Defect);
            }
#else
            for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
            {
                //vecvecDefectInfo[nSize].push_back(m_vecDefectInfo[nDefect]);
                CString strDefecntName;
                DefectInfo Defect;
                strDefecntName = GetDefectName((DefectType)nDefect);
                Defect.strDefecntName = strDefecntName;
                Defect.nDefectNum = 0;
                Defect.strCSVPath = _T("");
                m_vecvecDefectInfo[nSize].push_back(Defect);
            }
#endif
            
            
        }

        for (long nIndex = 0; nIndex < listPath.size(); nIndex++)
        {
            FILE* m_pFile;

            errno_t err = _tfopen_s(&m_pFile, listPath[nIndex], _T("rt,ccs=UNICODE"));

            if (err != 0)
            {
                return;
            }

            CStdioFile m_File(m_pFile);
            CString str;

            std::vector<CString> vecstrDefectInfo;
            while (m_File.ReadString(str))
            {
                if (str != _T(""))
                {
                    vecstrDefectInfo.push_back(str);
                }
            }
            m_File.Close();

            std::vector<std::vector<CString>> vecvecRowColInfo;
            vecvecRowColInfo.resize(vecstrDefectInfo.size());
            for (long nRow = 0; nRow < vecstrDefectInfo.size(); nRow++)
            {
                std::vector<CString> vecRowInfo;
                StringTokenDivideToCString(vecstrDefectInfo[nRow], ',', vecvecRowColInfo[nRow]);
            }

            CellDefectInfo rawinfo;
            DefectFilterInfo DefectFilterInfo;
            long nSize = vecvecRowColInfo[0].size();
            for (long nRow = 1; nRow < vecvecRowColInfo.size(); nRow++)
            {
                long nCol = 0;
                long nNum = 1;
                rawinfo.nStatus = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fAVG_GV = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fMIN_GV = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fMAX_GV = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fBrightAVG = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fDarkAVG = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fSize_X = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fSize_Y = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fArea_Ratio = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fRatio_YX = _tstof(vecvecRowColInfo[nRow][nCol++]);

                rawinfo.fRatio_XY = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fCompact = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fRatio_White = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fRatio_Dark = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fEdge_Energy = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fMass_Center_GV = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.strLotID = vecvecRowColInfo[nRow][nCol++];
                rawinfo.strCellID = vecvecRowColInfo[nRow][nCol++];
                rawinfo.strTime = vecvecRowColInfo[nRow][nCol++];
                rawinfo.strDefectName = GetDefectName((DefectType)_ttoi(vecvecRowColInfo[nRow][nCol++]));
                //rawinfo.vecdefectinfo.push_back(defectinfo);
                DefectInfoSet(rawinfo.strDefectName, (DefectType)_ttoi(vecvecRowColInfo[nRow][nCol - 1]), nIndex);
                m_vecvecDefectInfo[nIndex][0].strCSVPath = listPath[nIndex];
                //m_vecvecDefectInfo[nIndex].
                rawinfo.strCamTopBack = (vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fDefectX = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.fDefectY = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.nJudge = _tstof(vecvecRowColInfo[nRow][nCol++]);
                rawinfo.strBrightImagePath = vecvecRowColInfo[nRow][nCol++];
                rawinfo.strDarkImagePath = vecvecRowColInfo[nRow][nCol++];
                
                rawinfo.strCSVPath = listPath[nIndex];
                rawinfo.nCSVRow = nRow;
                rawinfo.vecvecRowColInfo = vecvecRowColInfo;
                rawinfo.nTotalDefectCount = vecvecRowColInfo.size()-1;
                
                m_vecRawInfo.push_back(rawinfo);
                DefectFilterInfo.strCSVPath = listPath[nIndex];
                DefectFilterInfo.defectinfo = m_vecvecDefectInfo[nIndex];
                DefectFilterInfo.nTotalNum = vecvecRowColInfo.size() - 1;
                m_vecDefectFilterInfo.push_back(DefectFilterInfo);
                if (nCol == nSize)
                    break;

            }
        }

        
        for (long nIndex = 0; nIndex < m_vecRawInfo.size(); nIndex++)
        {
            long nTrueCnt = 0;
            if (rawdefect.fAVG_GV >= m_vecRawInfo[nIndex].fAVG_GV)
                nTrueCnt++;
            if (rawdefect.fMIN_GV > m_vecRawInfo[nIndex].fMIN_GV)
                nTrueCnt++;
            if (rawdefect.fMAX_GV >= m_vecRawInfo[nIndex].fMAX_GV)
                nTrueCnt++;
            if (rawdefect.fSize_X >= m_vecRawInfo[nIndex].fSize_X)
                nTrueCnt++;
            if (rawdefect.fSize_Y >= m_vecRawInfo[nIndex].fSize_Y)
                nTrueCnt++;
            if (rawdefect.fArea_Ratio >= m_vecRawInfo[nIndex].fArea_Ratio)
                nTrueCnt++;
            if (rawdefect.fRatio_XY >= m_vecRawInfo[nIndex].fRatio_XY)
                nTrueCnt++;
            if (rawdefect.fDefectX >= m_vecRawInfo[nIndex].fDefectX)
                nTrueCnt++;
            if (rawdefect.fDefectY >= m_vecRawInfo[nIndex].fDefectY)
                nTrueCnt++;
            if (rawdefect.strDefectName == m_vecRawInfo[nIndex].strDefectName)
                nTrueCnt++;
            if (rawdefect.strCamTopBack == m_vecRawInfo[nIndex].strCamTopBack)
                nTrueCnt++;

            if (m_nRawEmpty == 0)
            {
                //Find List에 정보 뿌려주기 위한 vector
                m_vecCSVFilePath.push_back(m_vecRawInfo[nIndex].strCSVPath);
                // 조건에 맞는 구조체만 담아주는 vector
                m_vecFilterRawInfo.push_back(m_vecRawInfo[nIndex]);
                m_vecDefectFilterInfo_.push_back(m_vecDefectFilterInfo[nIndex]);
                vecIndex.push_back(nIndex);
            }
            else
            {
                if (m_nRawEmpty == nTrueCnt)
                {
                    //Find List에 정보 뿌려주기 위한 vector
                    m_vecCSVFilePath.push_back(m_vecRawInfo[nIndex].strCSVPath);
                    // 조건에 맞는 구조체만 담아주는 vector
                    m_vecFilterRawInfo.push_back(m_vecRawInfo[nIndex]);
                    m_vecDefectFilterInfo_.push_back(m_vecDefectFilterInfo[nIndex]);
                    vecIndex.push_back(nIndex);
                }

            }

        }
        // 겹치는 벡터 삭제
        OverlapDelete(m_vecCSVFilePath);

        if (m_vecDefectFilterInfo_.size() >1)
        {
            for (long nCheck = 0; nCheck < m_vecDefectFilterInfo_.size() - 1; nCheck++)
            {
                if (m_vecDefectFilterInfo_[nCheck].strCSVPath == m_vecDefectFilterInfo_[nCheck + 1].strCSVPath)
                {
                    m_vecDefectFilterInfo_.erase(m_vecDefectFilterInfo_.begin() + nCheck);
                    nCheck--;
                }
            }
        }
 




        /*m_vecvecDefectInfo.resize(m_vecFilterRawInfo.size());
        for (long n = 0; n < m_vecFilterRawInfo.size(); n++)
        {
            DefectInfoSet(m_vecFilterRawInfo[n].strDefectName, n);
        }
        */

    }
    std::vector<CString> vecYear;
    std::vector<CString> vecMonth;
    std::vector<CString> vecDay;
    std::vector<CString> vecHour;
    std::vector<CString> vecMin;
    m_vecTimeTable.clear();


    for (long n = 0; n < m_vecFilterRawInfo.size(); n++)
    {
        DefectTimetable timetable;
        timetable.strDefectName = m_vecFilterRawInfo[n].strDefectName;
        timetable.DtTable.nDefectNum++;
        timetable.strYear = m_vecFilterRawInfo[n].strTime.Mid(0, 4);
        timetable.strMonth = m_vecFilterRawInfo[n].strTime.Mid(4, 2);
        timetable.strDay = m_vecFilterRawInfo[n].strTime.Mid(6, 2);
        timetable.strHour = m_vecFilterRawInfo[n].strTime.Mid(8, 2);
        timetable.strMin = m_vecFilterRawInfo[n].strTime.Mid(10, 2);
        timetable.strSecond = m_vecFilterRawInfo[n].strTime.Mid(12, 2);
        CTime tm(_ttoi(timetable.strYear), _ttoi(timetable.strMonth), _ttoi(timetable.strDay), _ttoi(timetable.strHour),
            _ttoi(timetable.strMin), _ttoi(timetable.strSecond), 0);
        timetable.tm = tm;
        m_vecTimeTable.push_back(timetable);
        //vec.push_back(m_vecFilterRawInfo[n].strTime);
    }
    sort(m_vecTimeTable.begin(), m_vecTimeTable.end(), TimeCompare);

    if (m_vecTimeTable.size() != 0)
    {
        long nCheck = 0;
        CTime timer;
        CTimeSpan tmCheck(0, 0, 30, 0);

        CTime StartTm = m_vecTimeTable[0].tm;
        CTime EndTm = m_vecTimeTable[m_vecTimeTable.size() - 1].tm;
        long nCrater = 0;
        std::vector<CTime> vectm;
        while (true)
        {
            vectm.push_back(StartTm);
            CString strTime;
            strTime.Format(_T("%04d"), StartTm.GetYear());
            m_list_DefectTimeTable.InsertItem(nCrater, strTime);
            strTime.Format(_T("%02d"), StartTm.GetMonth());
            m_list_DefectTimeTable.SetItemText(nCrater, 1, strTime);
            strTime.Format(_T("%02d"), StartTm.GetDay());
            m_list_DefectTimeTable.SetItemText(nCrater, 2, strTime);
            strTime.Format(_T("%02d:%02d:%02d"), StartTm.GetHour(), StartTm.GetMinute(), StartTm.GetSecond());
            m_list_DefectTimeTable.SetItemText(nCrater, 3, strTime);

            StartTm = StartTm + tmCheck;
            nCrater++;
            if (StartTm > EndTm)
            {
                break;
            }
        }
        long n = m_list_DefectTimeTable.GetItemCount();
        long nOK = 0;
        long nNo = 0;
        for (long nList = 1; nList < m_list_DefectTimeTable.GetItemCount(); nList++)
        {
            if (nList == m_list_DefectTimeTable.GetItemCount())
            {
                continue;
            }
            CString strYear = m_list_DefectTimeTable.GetItemText(nList, 0);
            CString strMonth = m_list_DefectTimeTable.GetItemText(nList, 1);
            CString strDay = m_list_DefectTimeTable.GetItemText(nList, 2);
            CString strTime = m_list_DefectTimeTable.GetItemText(nList, 3);

            for (long nTable = 0; nTable < m_vecTimeTable.size(); nTable++)
            {
                if ((vectm[nList - 1] <= m_vecTimeTable[nTable].tm) && (vectm[nList] >= m_vecTimeTable[nTable].tm))
                {
#if INSPECTION_MACHINE_UC1
                    for (long nDefect = (long)DefectType::White_Line; nDefect < (long)DefectType::Edge_Defect; nDefect++)
#else
                    for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
#endif
                    {
                        CString strDefecntName;
                        CString str;
                        strDefecntName = GetDefectName((DefectType)nDefect);
                        if (m_vecTimeTable[nTable].strDefectName == strDefecntName)
                        {
                            nOK++;
                            str.Format(_T("%d"), nOK);
                            m_list_DefectTimeTable.SetItemText(nList - 1, nDefect - 8, str);
                        }
                        else
                        {
                            str.Format(_T("%d"), nNo);
                            CString strCheck = m_list_DefectTimeTable.GetItemText(nList - 1, nDefect - 8);
                            if (strCheck == _T(""))
                            {
                                m_list_DefectTimeTable.SetItemText(nList - 1, nDefect - 8, str);
                            }
                        }
                    }
                }
                else
                {
#if INSPECTION_MACHINE_UC1
                    for (long nDefect = (long)DefectType::White_Line; nDefect < (long)DefectType::Edge_Defect;
                         nDefect++)
#else
                    for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
#endif
                    {
                        CString str;
                        str.Format(_T("%d"), nNo);
                        CString strCheck = m_list_DefectTimeTable.GetItemText(nList - 1, nDefect - 8);
                        if (strCheck == _T(""))
                        {
                            m_list_DefectTimeTable.SetItemText(nList - 1, nDefect - 8, str);
                        }
                    }
                }
            }
            nOK = 0;
        }
        //for (long nTable = 0; nTable < m_vecTimeTable.size(); nTable++)
        //{
        //    m_list_DefectTimeTable.InsertItem(nTable, m_vecTimeTable[nTable].strYear);
        //    m_list_DefectTimeTable.SetItemText(nTable, 1, m_vecTimeTable[nTable].strMonth);
        //    m_list_DefectTimeTable.SetItemText(nTable, 2, m_vecTimeTable[nTable].strDay);

        //    CString strTime;
        //    if (nTable == 0)
        //    {
        //        strTime.Format(_T("%s:%s:%s"), m_vecTimeTable[nTable].strHour, m_vecTimeTable[nTable].strMin,
        //            m_vecTimeTable[nTable].strSecond);
        //        m_list_DefectTimeTable.SetItemText(nTable, 3, strTime);

        //        for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
        //        {
        //            CString strDefecntName;
        //            strDefecntName = GetDefectName((DefectType)nDefect);
        //            if (m_vecTimeTable[nTable].strDefectName == strDefecntName)
        //            {
        //                m_list_DefectTimeTable.SetItemText(nTable, nDefect - 8, _T("1"));
        //            }
        //            else
        //            {
        //                m_list_DefectTimeTable.SetItemText(nTable, nDefect - 8, _T("0"));
        //            }
        //        }

        //    }
        //    else
        //    {
        //

        //        CTimeSpan tmSpan;
        //        if (timer == 0)
        //        {
        //            timer = m_vecTimeTable[nTable].tm + tmCheck;
        //        }
        //        else
        //        {
        //            timer = timer + tmCheck;
        //        }
        //
        //        strTime.Format(_T("%02d:%02d:%02d"), timer.GetHour(), timer.GetMinute(), timer.GetSecond());
        //        m_list_DefectTimeTable.SetItemText(nTable, 3, strTime);

        //    }

        //}
        //std::vector<long> vecDefectCnt;
        //vecDefectCnt.resize((long)DefectType::Plain_Wrinkle - (long) DefectType::Line);

        //long n = 0;
        //CTimeSpan Span(0, 0, 30, 0);
        //CTime tm = m_vecTimeTable[0].tm + Span;
        //for (long nTable = 0; nTable < m_vecTimeTable.size(); nTable++)
        //{
        //    CTimeSpan CheckSpan;
        //    CheckSpan = m_vecTimeTable[nTable].tm - tm;
        //    long nMin = CheckSpan.GetTotalMinutes();
        //    CHECK:
        //    if (nMin < 30)
        //    {
        //        for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
        //        {
        //            CString strDefecntName;
        //            strDefecntName = GetDefectName((DefectType)nDefect);
        //            if (m_vecTimeTable[nTable].strDefectName == strDefecntName)
        //            {
        //                vecDefectCnt[nDefect - 12]++;
        //            }
        //        }
        //    }
        //    else
        //    {
        //        RECHECK:
        //        tm = m_vecTimeTable[0].tm + Span;
        //        CheckSpan = m_vecTimeTable[nTable].tm - tm;
        //        long nMin = CheckSpan.GetTotalMinutes();
        //        if (nMin < 30)
        //        {
        //            goto CHECK;
        //        }
        //        else
        //        {
        //            goto RECHECK;
        //        }
        //
        //    }

        //}


    
    }
        for (long nFind = 0; nFind < m_vecCSVFilePath.size(); nFind++)
    {
        //m_listCtrlCsvList.InsertColumn(3, _T("Total Num"), NULL, 150, NULL);
        //long nList = 4;
        //for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
        //{
        //    CString strDefecntName;
        //    strDefecntName = GetDefectName((DefectType)nDefect);
        //    strDefecntName = strDefecntName + _T("_Num");
        //    m_listCtrlCsvList.InsertColumn(nList, strDefecntName, NULL, 100, NULL);
        //    nList++;
        //}
        CString str;
        str.Format(_T("%d"), nFind + 1);
        m_listCtrlCsvList.InsertItem(nFind + 1, str);
        m_listCtrlCsvList.SetItemText(nFind, 1, m_vecCSVFilePath[nFind]);


        //m_vecDefectFilterInfo_[nFind].defectinfo[0].nDefectNum;
        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].nTotalNum);
        m_listCtrlCsvList.SetItemText(nFind , 2, str); // Total

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[0].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 3, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[1].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 4, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[2].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 5, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[3].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 6, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[4].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 7, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[5].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 8, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[6].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 9, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[7].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 10, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[8].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 11, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[9].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 12, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[10].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 13, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[11].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 14, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[12].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 15, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[13].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 16, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[14].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 17, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[15].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 18, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[16].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 19, str);

        str.Format(_T("%d"), m_vecDefectFilterInfo_[nFind].defectinfo[17].nDefectNum);
        m_listCtrlCsvList.SetItemText(nFind, 20, str);

        //str.Format(_T("%d"), m_vecFilterRawInfo[nFind].nTotalDefectCount);
        //m_listCtrlCsvList.SetItemText(nFind, 2,_T("")); // Total

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][0].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 3, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][1].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 4, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][2].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 5, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][3].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 6, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][4].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 7, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][5].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 8, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][6].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 9, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][7].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 10, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][8].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 11, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][9].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 12, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][10].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 13, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][11].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 14, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][12].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 15, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][13].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 16, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][14].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 17, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][15].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 18, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][16].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 19, str);

        //str.Format(_T("%d"), m_vecvecDefectInfo[vecIndex[nFind]][17].nDefectNum);
        //m_listCtrlCsvList.SetItemText(nFind, 20, str);

    }

}

void CDlgDataSearch::GatherRecursively(LPCTSTR szDirectory, std::vector<CString>& listFiles, const CString& fileType)
{
    CFileFind finder;

    CString strFindFilter(szDirectory);
    strFindFilter += "\\*";

    BOOL bWorking = finder.FindFile(strFindFilter);

    while (bWorking)
    {
        bWorking = finder.FindNextFile();

        if (finder.IsDirectory())
        {
            if (finder.IsDots())
                continue;
            else
                GatherRecursively(finder.GetFilePath(), listFiles, fileType);
        }
        else
        {
            CString strPathName(finder.GetFilePath());
            strPathName.MakeLower();

            if (strPathName.Right(4) == fileType)
                listFiles.push_back(finder.GetFilePath());

            if (strPathName.Find(fileType) != -1&& strPathName.Right(4) != fileType) //.csv, .csv2, csv12
                listFiles.push_back(finder.GetFilePath());


        }
    }
    return;
}

int CDlgDataSearch::ListRowSelect(int nID, LPNMITEMACTIVATE pNMItemActivate)
{
    CPoint ptClick = pNMItemActivate->ptAction;
    CListCtrl* pListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(nID));
    LVHITTESTINFO listinfo;
    listinfo.pt = ptClick;
    
    return pListCtrl->SubItemHitTest(&listinfo); // 행
}

void CDlgDataSearch::VectorAndListClear()
{
    m_listCtrlCsvList.DeleteAllItems();
    m_listCtrlCsvInfo.DeleteAllItems();

    m_vecCSVFilePath.clear();
    m_vecFilteredRows.clear();
    m_vecRawInfo.clear();
    m_vecLotInfo.clear();
    m_vecFilterRawInfo.clear();
    m_vecvecRowColInfo.clear();
    m_vecDefectFilterInfo_.clear();
    m_vecDefectFilterInfo.clear();
    m_vecTimeTable.clear();
}

void CDlgDataSearch::InitDefectNum()
{
    m_vecDefectInfo.clear();
#if INSPECTION_MACHINE_UC1
    for (long nDefect = (long)DefectType::White_Line; nDefect < (long)DefectType::Edge_Defect; nDefect++)
#else
    for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
#endif
    {
        CString strDefecntName;
        DefectInfo Defect;
        strDefecntName = GetDefectName((DefectType)nDefect);
        Defect.strDefecntName = strDefecntName;
        Defect.nDefectNum = 0;

        m_vecDefectInfo.push_back(Defect);
    }
}

void CDlgDataSearch::DefectInfoSet(CString strDefectName, DefectType dt, long nIndex)
{
    for (long nDefect = 0; nDefect < m_vecvecDefectInfo[nIndex].size(); nDefect++)
    {
        if (m_vecvecDefectInfo[nIndex][nDefect].strDefecntName == GetDefectName((DefectType)dt))
        {
            m_vecvecDefectInfo[nIndex][nDefect].nDefectNum++;
        }

    }
}

void CDlgDataSearch::DefectInfoSet(CString strDefectName, long nIndex)
{
#if INSPECTION_MACHINE_UC1
    for (long nDefect = (long)DefectType::White_Line; nDefect < (long)DefectType::Edge_Defect; nDefect++)
#else
    for (long nDefect = (long)DefectType::Line; nDefect < (long)DefectType::Plain_Wrinkle; nDefect++)
#endif
    {
        if (strDefectName == GetDefectName((DefectType)nDefect))
        {
            m_vecvecDefectInfo[nIndex][nDefect].nDefectNum++;
        }
    }
}

void CDlgDataSearch::OnBnClickedButtonDefectcsvexport()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    vector<CString> vecData;

    long nImageSizeX = 100;
    long nImageSizeY = 100;
    m_ExcelSave.Create(CRect(0, 0, nImageSizeX, nImageSizeY), 1);

    CFileDialog SaveDlg(FALSE, _T("xlsx"), _T(""), OFN_HIDEREADONLY, _T("xlsx Files (*.xlsx)|*.xlsx||"));
    
    if (SaveDlg.DoModal() != IDOK)
    {
        return;
    }

    CString strPath, strDirecPath, strFileName, strFullPath, strName;
    strPath = SaveDlg.GetPathName();
    strDirecPath = strPath.Left(strPath.ReverseFind('\\') + 1);

    strName = SaveDlg.GetFileName();
    strFileName = SaveDlg.GetFileName();

    strFullPath = strDirecPath + strName;

    m_ExcelSave.RejectListName(vecData);
    m_nExcelRow = 0;
    if (m_chk_DefectPosAdd.GetCheck() == TRUE)
    {
        vecData.push_back(_T("Num"));
        vecData.push_back(_T("Bright_Image"));
        vecData.push_back(_T("Dark_Image"));
        vecData.push_back(_T(""));
        vecData.push_back(_T("Image"));
        vecData.push_back(_T(""));
        vecData.push_back(_T("SIZE_X"));
        vecData.push_back(_T("SIZE_Y"));
        vecData.push_back(_T("LotID"));
        vecData.push_back(_T("CellID"));
        vecData.push_back(_T("Defect_X"));
        vecData.push_back(_T("Defect_Y"));
        vecData.push_back(_T("BrightImagePath"));
        vecData.push_back(_T("DarkImagePath"));
    }
    else
    {
        vecData.push_back(_T("Num"));
        vecData.push_back(_T("Bright_Image"));
        vecData.push_back(_T("Dark_Image"));
        vecData.push_back(_T("SIZE_X"));
        vecData.push_back(_T("SIZE_Y"));
        vecData.push_back(_T("LotID"));
        vecData.push_back(_T("CellID"));
        vecData.push_back(_T("Defect_X"));
        vecData.push_back(_T("Defect_Y"));
        vecData.push_back(_T("BrightImagePath"));
        vecData.push_back(_T("DarkImagePath"));
    }
    //if (m_checkCropSave.GetCheck() == TRUE)
    {
        
        m_ExcelSave.RejectListName(vecData);
        m_nExcelRow = 0;
        //if (CreateDirectory(strDirecPath + strFolder, nullptr))
        {
            IPVM::Image_8u_C1 BrightImage, DarkImage;
            IPVM::Image_8u_C1 OriImage;

            CString strSystemPath = m_strSystemCSVPath; //_T("D:\\Load");
            std::vector<CString> vecDefectName = {_T("Test")};

            for (long n = 0; n < m_vecFilterRawInfo.size(); n++)
            {
                //CString strImagePath
                //    = m_vecAllData[n][16] + _T("_") + m_vecAllData[n][17] + _T("_") + m_vecAllData[n][18];
                CString strCount;
                strCount.Format(_T("%d"), n);

                
                strCount = m_strSystemCSVPath.Mid(0, m_strSystemCSVPath.ReverseFind('\\'));
                strCount = strCount + m_strSystemCSVPath.Mid(m_strSystemCSVPath.ReverseFind('\\') + 1, m_strSystemCSVPath.GetLength());


                
                CString strBrightPath = strCount + m_vecFilterRawInfo[n].strBrightImagePath;
                CString strDarkPath = strCount + m_vecFilterRawInfo[n].strDarkImagePath;
                
                BrightImage.LoadFrom(strBrightPath);
                DarkImage.LoadFrom(strDarkPath);

                TCHAR programpath[_MAX_PATH];
                GetCurrentDirectory(_MAX_PATH, programpath);
                CString ImageFolderName = _T("\\DataSearchImage.jpg");
                CString strImagePath = _T("D:\\UC1\\DataSearchImage.jpg"); //programpath + ImageFolderName;

                CString str = _T("D:\\UC1\\DataSearchImage.jpg");
                BOOL bRet;
                CFileFind find;
                bRet = find.FindFile(strImagePath);
                if (bRet == FALSE)
                {
                    DarkImage.SaveJpeg(strImagePath);
                }
                OriImage.LoadFrom(strImagePath);
                IPVM::Rect CropROIRect(BrightImage);
                IPVM::Rect OriginRect(OriImage);

                m_ExcelSave.SetBrightImage(BrightImage);
                m_ExcelSave.SetDarkImage(DarkImage);
                m_ExcelSave.SetBinImage(OriImage);


                m_nExcelRow += 1;

                vector<CString> vecResultData;
                CString strResult, ResultName;
                ResultName = _T("Reject");

                //for (long nSub = 0; nSub < m_vecAllData[0].size() - 4; nSub++)
                {
                  //  strResult = m_vecAllData[n][nSub];
                  strResult.Format(_T("%.3f"), m_vecFilterRawInfo[n].fSize_X);
                  vecResultData.push_back(strResult);
                  strResult.Format(_T("%.3f"), m_vecFilterRawInfo[n].fSize_Y);
                  vecResultData.push_back(strResult);

                  vecResultData.push_back(m_vecFilterRawInfo[n].strLotID);
                  vecResultData.push_back(m_vecFilterRawInfo[n].strCellID);
                  strResult.Format(_T("%.3f"), m_vecFilterRawInfo[n].fDefectX);
                  vecResultData.push_back(strResult);
                  strResult.Format(_T("%.3f"), m_vecFilterRawInfo[n].fDefectY);
                  vecResultData.push_back(strResult);

                  vecResultData.push_back(m_vecFilterRawInfo[n].strBrightImagePath);
                  vecResultData.push_back(m_vecFilterRawInfo[n].strDarkImagePath);
                }

                IPVM::Rect DefectROI(m_vecFilterRawInfo[n].fDefectX - 2, m_vecFilterRawInfo[n].fDefectY - 2,
                    m_vecFilterRawInfo[n].fDefectX + 2, m_vecFilterRawInfo[n].fDefectY + 2);
                
                if (m_chk_DefectPosAdd.GetCheck() == TRUE)
                {
                    m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkPath, strBrightPath,
                        strImagePath, DefectROI, vecResultData, CropROIRect, OriginRect, n, TRUE);
                }
                else
                {
                    m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkPath, strBrightPath, vecResultData, ResultName, OriginRect, n,TRUE);
                }

  

            }
            m_ExcelSave.Savexlsfile(strFullPath, FALSE);
            return;
        }
    }
    
}

BOOL CDlgDataSearch ::CreateFileTimeCheck(SYSTEMTIME StartDate, SYSTEMTIME EndDate, SYSTEMTIME StartTime, SYSTEMTIME EndTime, SYSTEMTIME CheckTime)
{
    if ((StartDate.wYear <= CheckTime.wYear) &&  (CheckTime.wYear <= EndDate.wYear))
    {
        if ((StartDate.wMonth <= CheckTime.wMonth) &&(CheckTime.wMonth<= EndDate.wMonth))
        {
            if ((StartDate.wDay <= CheckTime.wDay) && (CheckTime.wDay<= EndDate.wDay))
            {
                return TRUE;
                //if ((StartTime.wHour <= CheckTime.wHour) && ( CheckTime.wHour<= EndTime.wHour))
                //{
                //    if ((StartTime.wMinute <= CheckTime.wMinute) && ( CheckTime.wMinute<= EndTime.wMinute))
                //    {
                //        if (StartTime.wSecond <= CheckTime.wSecond <= EndTime.wSecond)
                //        {
                //            return TRUE;
                //        }
                //    }
                //}
            }
        }
    }

    return FALSE;
}