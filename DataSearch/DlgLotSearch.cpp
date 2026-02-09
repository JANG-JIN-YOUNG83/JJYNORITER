// DlgLotSearch.cpp: 구현 파일
//

#include "pch.h"
#include "DataSearch.h"
#include "DlgLotSearch.h"
#include "afxdialogex.h"
#include "DlgDataSearch.h"



// CDlgLotSearch 대화 상자

IMPLEMENT_DYNAMIC(CDlgLotSearch, CDialogEx)

CDlgLotSearch::CDlgLotSearch(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SEARCH_LOT, pParent)
{

}

CDlgLotSearch::~CDlgLotSearch()
{
}

void CDlgLotSearch::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_DEFECT_NAME, m_comboLotDefectName);
}


BEGIN_MESSAGE_MAP(CDlgLotSearch, CDialogEx)
ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgLotSearch 메시지 처리기

BOOL CDlgLotSearch::OnInitDialog()
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

void CDlgLotSearch::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    GetClientRect(m_rtDlgSize);
    CALL_Function_CSC();
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgLotSearch::CALL_Function_CSC()
{
    ControlSizeChange(IDC_STATIC_LOT_START_TIME);  
    ControlSizeChange(IDC_DATETIMEPICKER_LOT_START_TIME);
    ControlSizeChange(IDC_STATIC_LOT_END_TIME2);
    ControlSizeChange(IDC_DATETIMEPICKER_LOT_END_TIME);
    ControlSizeChange(IDC_STATIC_LOT_FINAL_METER);
    ControlSizeChange(IDC_EDIT_LOT_FINAL_METER);

    ControlSizeChange(IDC_STATIC_TOTAL_DEFECT);
    ControlSizeChange(IDC_EDIT_TOTAL_DEFECT_NUM);
    ControlSizeChange(IDC_STATIC_LAIN_NUMBER);
    ControlSizeChange(IDC_EDIT_LAIN_NUM);
    ControlSizeChange(IDC_STATIC_LOT_DEFECT_NAME);
    ControlSizeChange(IDC_COMBO_DEFECT_NAME);

    ControlSizeChange(IDC_STATIC_DIMENSION_TYPE);
    ControlSizeChange(IDC_STATIC_LAIN_NUMBER2);
    ControlSizeChange(IDC_STATIC_LOT_TOTAL_PPK);
    ControlSizeChange(IDC_STATIC_RECIPE);
    ControlSizeChange(IDC_EDIT_DIMENSION_TYPE);
    ControlSizeChange(IDC_EDIT_TOTAL_LAIN_NUM2);
    ControlSizeChange(IDC_EDIT_TOTAL_PPK);
    ControlSizeChange(IDC_EDIT_RECIPE);
}

void CDlgLotSearch::ControlSizeChange(int nID)
{
    CRect rectCtl;
    HWND hwnd_ID = ::GetDlgItem(this->m_hWnd, nID);

    ::GetClientRect(hwnd_ID, &rectCtl);

    long cx = m_rtDlgSize.Width() * 0.01;
    long cy = m_rtDlgSize.Height() * 0.01;

    if (nID == IDC_STATIC_LOT_START_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 5, cx * 17, cy * 10, TRUE); //+20
    }

    if (nID == IDC_DATETIMEPICKER_LOT_START_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 18, cy * 5, cx * 19, cy * 10, TRUE);
    }

    if (nID == IDC_STATIC_LOT_END_TIME2)
    {
        ::MoveWindow(hwnd_ID, cx * 39, cy * 5, cx * 16, cy * 10, TRUE);
    }

    if (nID == IDC_DATETIMEPICKER_LOT_END_TIME)
    {
        ::MoveWindow(hwnd_ID, cx * 55, cy * 5, cx * 19, cy * 10, TRUE);
    }

    if (nID == IDC_STATIC_LOT_FINAL_METER)
    {
        ::MoveWindow(hwnd_ID, cx * 76, cy * 5, cx * 17, cy * 10, TRUE);
    }

    if (nID == IDC_EDIT_LOT_FINAL_METER)
    {
        ::MoveWindow(hwnd_ID, cx * 93, cy * 5, cx * 17, cy * 10, TRUE);
    }

    // second Row
    if (nID == IDC_STATIC_TOTAL_DEFECT)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 23, cx * 19, cy * 10, TRUE);
    }

    if (nID == IDC_EDIT_TOTAL_DEFECT_NUM)
    {
        ::MoveWindow(hwnd_ID, cx * 21, cy * 23, cx * 17, cy * 10, TRUE);
    }

    if (nID == IDC_STATIC_LAIN_NUMBER)
    {
        ::MoveWindow(hwnd_ID, cx * 42, cy * 23, cx * 17, cy * 10, TRUE);
    }

    if (nID == IDC_EDIT_LAIN_NUM)
    {
        ::MoveWindow(hwnd_ID, cx * 55, cy * 23, cx * 19, cy * 10, TRUE);
    }

    if (nID == IDC_STATIC_LOT_DEFECT_NAME)
    {
        ::MoveWindow(hwnd_ID, cx * 76, cy * 23, cx * 17, cy * 10, TRUE);
    }

    if (nID == IDC_COMBO_DEFECT_NAME)
    {
        ::MoveWindow(hwnd_ID, cx * 93, cy * 23, cx * 17, cy * 10, TRUE);
    }


    // third Row
    if (nID == IDC_STATIC_DIMENSION_TYPE)
    {
        ::MoveWindow(hwnd_ID, cx * 1, cy * 41, cx * 17, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_LAIN_NUMBER2)
    {
        ::MoveWindow(hwnd_ID, cx * 32, cy * 41, cx * 17, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_LOT_TOTAL_PPK)
    {
        ::MoveWindow(hwnd_ID, cx * 58, cy * 41, cx * 17, cy * 10, TRUE);
    }
    if (nID == IDC_STATIC_RECIPE)
    {
        ::MoveWindow(hwnd_ID, cx * 85, cy * 41, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_DIMENSION_TYPE)
    {
        ::MoveWindow(hwnd_ID, cx * 18, cy * 41, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_TOTAL_LAIN_NUM2)
    {
        ::MoveWindow(hwnd_ID, cx * 43, cy * 41, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_TOTAL_PPK)
    {
        ::MoveWindow(hwnd_ID, cx * 69, cy * 41, cx * 13, cy * 10, TRUE);
    }
    if (nID == IDC_EDIT_RECIPE)
    {
        ::MoveWindow(hwnd_ID, cx * 93, cy * 41, cx * 13, cy * 10, TRUE);
    }

    
}

void CDlgLotSearch::DlgControlSetting()
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
        m_comboLotDefectName.InsertString(i, vecStrName[i]);
    }
}


