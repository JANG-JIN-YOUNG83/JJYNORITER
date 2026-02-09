// CDlgDefectRawSearch.cpp: 구현 파일
//

#include "pch.h"
#include "DataSearch.h"
#include "CDlgDefectRawSearch.h"
#include "afxdialogex.h"
#include "DlgDataSearch.h"

// CDlgDefectRawSearch 대화 상자

IMPLEMENT_DYNAMIC(CDlgDefectRawSearch, CDialogEx)

CDlgDefectRawSearch::CDlgDefectRawSearch(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SEARCH_DEFECTRAW, pParent)
{

}

CDlgDefectRawSearch::~CDlgDefectRawSearch()
{
}

void CDlgDefectRawSearch::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ELECTRODE_TYPE, m_comboElectrodeType);
    DDX_Control(pDX, IDC_COMBO_CAM_INDEX, m_comboCamIndex);
    DDX_Control(pDX, IDC_COMBO_RAW_DEFECT_NAME, m_comboDefectName);
}


BEGIN_MESSAGE_MAP(CDlgDefectRawSearch, CDialogEx)
ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgDefectRawSearch 메시지 처리기

BOOL CDlgDefectRawSearch::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    CDlgDataSearch* pMainFrm = static_cast<CDlgDataSearch*>(::AfxGetMainWnd());
    CRect rect;

    pMainFrm->GetDlgItem(IDC_STATIC_DETAIL_DATASEARCH)->GetWindowRect(rect);
    pMainFrm->ScreenToClient(rect);
    ::MoveWindow(this->m_hWnd, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
    DlgControlSetting();
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgDefectRawSearch::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    GetClientRect(m_rtDlgSize);
    CALL_Function_CSC();
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgDefectRawSearch::CALL_Function_CSC()
{
    ControlSizeChange(IDC_STATIC_LOT_START_TIME);
    ControlSizeChange(IDC_STATIC_LOT_END_TIME);
    ControlSizeChange(IDC_STATIC_ELECTRODE_TYPE);
    ControlSizeChange(IDC_STATIC_MODE);
    ControlSizeChange(IDC_STATIC_CAM_INDEX);
    ControlSizeChange(IDC_STATIC_PATTERN_NUMBER);
    ControlSizeChange(IDC_STATIC_LAIN_NUMBER);
    ControlSizeChange(IDC_STATIC_DEFECT_NAME2);

    ControlSizeChange(IDC_DATETIMEPICKER_RAW_START_TIME);
    ControlSizeChange(IDC_DATETIMEPICKER_RAW_END_TIME);
    ControlSizeChange(IDC_COMBO_ELECTRODE_TYPE);
    ControlSizeChange(IDC_EDIT_MODE);
    ControlSizeChange(IDC_COMBO_CAM_INDEX);
    ControlSizeChange(IDC_EDIT_PATTERN_NUM);
    ControlSizeChange(IDC_EDIT_RAW_LAIN_NUM);
    ControlSizeChange(IDC_COMBO_RAW_DEFECT_NAME);

    int controlIDs[] = {IDC_STATIC_RAW_RECIPE, IDC_STATIC_RAW_METER, IDC_STATIC_RAW_WIDTH_LENGTH, IDC_STATIC_RAW_AREA,
        IDC_STATIC_RAW_X_SIZE, IDC_STATIC_RAW_Y_SIZE, IDC_STATIC_RAW_DEFECT_X, IDC_STATIC_RAW_DEFECT_Y,
        IDC_STATIC_RAW_MAX_GV, IDC_STATIC_RAW_MIN_GV, IDC_STATIC_RAW_AVG_GV, IDC_STATIC_RAW_DEFECT_AREA_GV,
        IDC_EDIT_RAW_RECIPE, IDC_EDIT_RAW_METER, IDC_EDIT_RAW_WIDTH_LENGTH, IDC_EDIT_RAW_AREA, IDC_EDIT_RAW_X_SIZE,
        IDC_EDIT_RAW_Y_SIZE, IDC_EDIT_RAW_DEFECT_X, IDC_EDIT_RAW_DEFECT_Y, IDC_EDIT_RAW_MAX_GV, IDC_EDIT_RAW_MIN_GV,
        IDC_EDIT_RAW_AVG_GV, IDC_EDIT_RAW_DEFECT_AREA_GV};

    for (long n = 0; n < sizeof(controlIDs)/sizeof(controlIDs[0]);n++)
    {
        ControlSizeChange(controlIDs[n]);
    }

}

void CDlgDefectRawSearch::ControlSizeChange(int nID)
{
    CRect rectCtl;
    HWND hwnd_ID = ::GetDlgItem(this->m_hWnd, nID);

    ::GetClientRect(hwnd_ID, &rectCtl);

    long cx = m_rtDlgSize.Width() * 0.01;
    long cy = m_rtDlgSize.Height() * 0.01;
    
    //First Row
    if (nID == IDC_STATIC_LOT_START_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 5, cx * 17, cy * 10, TRUE); 
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_DATETIMEPICKER_RAW_START_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 18, cy * 5, cx * 19, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);

    }

    if (nID == IDC_STATIC_LOT_END_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 39, cy * 5, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);

    }

    if (nID == IDC_DATETIMEPICKER_RAW_END_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 55, cy * 5, cx * 19, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);

    }

    if (nID == IDC_STATIC_PATTERN_NUMBER)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 5, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_EDIT_PATTERN_NUM)
    {
        ::MoveWindow(hwnd_ID, cx * 93, cy * 5, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    //Second Row
    if (nID == IDC_STATIC_CAM_INDEX)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 23, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_COMBO_CAM_INDEX)
    {
        ::MoveWindow(hwnd_ID, cx * 18, cy * 23, cx * 19, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_STATIC_ELECTRODE_TYPE)
    {
        ::MoveWindow(hwnd_ID, cx * 39, cy * 23, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_COMBO_ELECTRODE_TYPE)
    {
        ::MoveWindow(hwnd_ID, cx * 55, cy * 23, cx * 19, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_STATIC_DEFECT_NAME2)
    {
        //::MoveWindow(hwnd_ID, cx * 76, cy * 23, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 1, cy * 5, cx * 17, cy * 10, TRUE); 
    }

    if (nID == IDC_COMBO_RAW_DEFECT_NAME)
    {
        //::MoveWindow(hwnd_ID, cx * 93, cy * 23, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 17, cy * 5, cx * 15, cy * 10, TRUE); 
    }


    //Third Row
    if (nID == IDC_STATIC_MODE)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 41, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_EDIT_MODE)
    {
        ::MoveWindow(hwnd_ID, cx * 18, cy * 41, cx * 19, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_STATIC_LAIN_NUMBER)
    {
        ::MoveWindow(hwnd_ID, cx * 39, cy * 41, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    if (nID == IDC_EDIT_RAW_LAIN_NUM)
    {
        ::MoveWindow(hwnd_ID, cx * 55, cy * 41, cx * 19, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    // fourth Row
    if (nID == IDC_STATIC_RAW_RECIPE)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 59, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_STATIC_RAW_METER)
    {
        ::MoveWindow(hwnd_ID, cx * 26, cy * 59, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_STATIC_RAW_WIDTH_LENGTH)
    {
        //::MoveWindow(hwnd_ID, cx * 51, cy * 59, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 1, cy * 41, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_STATIC_RAW_AREA)
    {
        //::MoveWindow(hwnd_ID, cx * 82, cy * 59, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 35, cy * 41, cx * 13, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_EDIT_RAW_RECIPE)
    {
        ::MoveWindow(hwnd_ID, cx * 10, cy * 59, cx * 13, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_EDIT_RAW_METER)
    {
        ::MoveWindow(hwnd_ID, cx * 34, cy * 59, cx * 13, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_EDIT_RAW_WIDTH_LENGTH)
    {
        //::MoveWindow(hwnd_ID, cx * 67, cy * 59, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 17, cy * 41, cx * 13, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_EDIT_RAW_AREA)
    {
        //::MoveWindow(hwnd_ID, cx * 89, cy * 59, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 45, cy * 41, cx * 13, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }

    // fifth Row
    if (nID == IDC_STATIC_RAW_X_SIZE)
    {
        //::MoveWindow(hwnd_ID, cx * 1, cy * 77, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 1, cy * 23, cx * 17, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_RAW_Y_SIZE)
    {
        //::MoveWindow(hwnd_ID, cx * 26, cy * 77, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 35, cy * 23, cx * 17, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_RAW_DEFECT_X)
    {
        //::MoveWindow(hwnd_ID, cx * 51, cy * 77, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 61, cy * 23, cx * 17, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_RAW_DEFECT_Y)
    {
        //::MoveWindow(hwnd_ID, cx * 80, cy * 77, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 86, cy * 23, cx * 17, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_RAW_X_SIZE)
    {
        //::MoveWindow(hwnd_ID, cx * 10, cy * 77, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 17, cy * 23, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_RAW_Y_SIZE)
    {
        ::MoveWindow(hwnd_ID, cx * 45, cy * 23, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_RAW_DEFECT_X)
    {
        //::MoveWindow(hwnd_ID, cx * 63, cy * 77, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 71, cy * 23, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_RAW_DEFECT_Y)
    {
        //::MoveWindow(hwnd_ID, cx * 92, cy * 77, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 96, cy * 23, cx * 13, cy * 10, TRUE);
    }

    // sixth Row
    if (nID == IDC_STATIC_RAW_MAX_GV)
    {
        //::MoveWindow(hwnd_ID, cx * 1, cy * 95, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 86, cy * 5, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_RAW_MIN_GV)
    {
        //::MoveWindow(hwnd_ID, cx * 26, cy * 95, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 61, cy * 5, cx * 13, cy * 10, TRUE);
        
    }
    if (nID == IDC_STATIC_RAW_AVG_GV)
    {
        //::MoveWindow(hwnd_ID, cx * 51, cy * 95, cx * 17, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 35, cy * 5, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_RAW_DEFECT_AREA_GV)
    {
        ::MoveWindow(hwnd_ID, cx * 80, cy * 95, cx * 17, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
    if (nID == IDC_EDIT_RAW_MAX_GV)
    {
        //::MoveWindow(hwnd_ID, cx * 10, cy * 95, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 96, cy * 5, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_RAW_MIN_GV)
    {
        //::MoveWindow(hwnd_ID, cx * 34, cy * 95, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 71, cy * 5, cx * 13, cy * 10, TRUE);

    }
    if (nID == IDC_EDIT_RAW_AVG_GV)
    {
        //::MoveWindow(hwnd_ID, cx * 63, cy * 95, cx * 13, cy * 10, TRUE);
        ::MoveWindow(hwnd_ID, cx * 45, cy * 5, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_RAW_DEFECT_AREA_GV)
    {
        ::MoveWindow(hwnd_ID, cx * 97, cy * 95, cx * 13, cy * 10, TRUE);
        ::ShowWindow(hwnd_ID, SW_HIDE);
    }
}

void CDlgDefectRawSearch::DlgControlSetting()
{
    std::vector<CString> vecStrName = {
        _T("Line"),
        _T("Pinhole"),
        _T("Horizontal_Line"),
        _T("Scratch"),
        _T("Crack"), 
        _T("Stain"),
        _T("Stain_Pinhole"),
        _T("Delamination_Crack"),
        _T("Dent"),
        _T("Black_Dot"),
        _T("Delamination_Pinhole"), 
        _T("Banking"),
        _T("ETC_Defect"),
        _T("Edge_ChippingHole"),
        _T("Drag"),
        _T("Edge_Line"),
        _T("Edge_Defect"),
        _T("Island"),
        _T("Plain_Wrinkle")
    };
    for (long i = 0; i < vecStrName.size(); i++)
    {
        m_comboDefectName.InsertString(i, vecStrName[i]);
    }

    m_comboElectrodeType.InsertString(0, _T("양극"));
    m_comboElectrodeType.InsertString(1, _T("음극"));
    m_comboCamIndex.InsertString(0, _T("0")); // BACK
    m_comboCamIndex.InsertString(1, _T("1")); // TOP
}