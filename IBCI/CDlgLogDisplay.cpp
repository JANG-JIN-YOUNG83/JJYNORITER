// CDlgLogDisplay.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CDlgLogDisplay.h"
#include "afxdialogex.h"


// CDlgLogDisplay 대화 상자

IMPLEMENT_DYNAMIC(CDlgLogDisplay, CDialogEx)

CDlgLogDisplay::CDlgLogDisplay(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_LOGDISPLAY, pParent)
{

}

CDlgLogDisplay::~CDlgLogDisplay()
{
}

void CDlgLogDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLogDisplay, CDialogEx)
ON_NOTIFY(TCN_SELCHANGE, 1234, &CDlgLogDisplay::OnSelchangeTab1)
ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgLogDisplay 메시지 처리기

BOOL CDlgLogDisplay::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_tab1 = new CTabCtrlFixable();
    m_tab1->Create(WS_CHILD | WS_VISIBLE | TCS_TABS | TCS_SINGLELINE, CRect(0, 0, 500, 20), this, 1234);
    CFont font1;
    font1.CreatePointFont(95, _T("굴림"));
    m_tab1->SetFont(&font1, true);

    m_tab1->InsertItem(0, _T("Edge View"));
    m_tab1->InsertItem(1, _T("Sequence"));
    m_tab1->InsertItem(2, _T("Sensor Position"));
    m_tab1->InsertItem(3, _T("Mark Position"));
    m_tab1->InsertItem(4, _T("Alarm"));
    m_tab1->InsertItem(5, _T("Grab"));
    m_tab1->InsertItem(6, _T("IO"));
    m_tab1->InsertItem(7, _T("Result"));
    m_tab1->InsertItem(8, _T("Debug"));
    m_tab1->InsertItem(9, _T("Tcp"));
    m_tab1->InsertItem(10, _T("Time"));
    m_tab1->InsertItem(11, _T("CPU"));
    m_tab1->InsertItem(12, _T("Eswa"));
    m_tab1->InsertItem(13, _T("Simul"));
    m_tab1->InsertItem(14, _T("ImageSaveFailed"));
    m_tab1->InsertItem(15, _T("Param Changed"));
    m_tab1->InsertItem(16, _T("Judge"));
    m_tab1->InsertItem(17, _T("CellResult"));
    m_tab1->InsertItem(18, _T("Focus"));
    m_tab1->ShowWindow(SW_SHOW);

    m_edit1 = new CEdit;
    m_edit1->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit2 = new CEdit;
    m_edit2->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit3 = new CEdit;
    m_edit3->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit4 = new CEdit;
    m_edit4->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit5 = new CEdit;
    m_edit5->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit6 = new CEdit;
    m_edit6->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit7 = new CEdit;
    m_edit7->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit8 = new CEdit;
    m_edit8->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit9 = new CEdit;
    m_edit9->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit10 = new CEdit;
    m_edit10->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit11 = new CEdit;
    m_edit11->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit12 = new CEdit;
    m_edit12->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit13 = new CEdit;
    m_edit13->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit14 = new CEdit;
    m_edit14->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit15 = new CEdit;
    m_edit15->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit16 = new CEdit;
    m_edit16->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit17 = new CEdit;
    m_edit17->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    m_edit18 = new CEdit;
    m_edit18->Create(WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,
        CRect(0, 20, 630, 300), this, IDD_DLG_LOGDISPLAY);
    return TRUE;
}

void CDlgLogDisplay::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    if (isEdgeViewExist == true)
    {
        m_pDlgEdgeView->MoveWindow(0, 20, cx, cx - 20);
        m_edit1->MoveWindow(0, 20, cx, cy - 20);
        m_edit2->MoveWindow(0, 20, cx, cy - 20);
        m_edit3->MoveWindow(0, 20, cx, cy - 20);
        m_edit4->MoveWindow(0, 20, cx, cy - 20);
        m_edit5->MoveWindow(0, 20, cx, cy - 20);
        m_edit6->MoveWindow(0, 20, cx, cy - 20);
        m_edit7->MoveWindow(0, 20, cx, cy - 20);
        m_edit8->MoveWindow(0, 20, cx, cy - 20);
        m_edit9->MoveWindow(0, 20, cx, cy - 20);
        m_edit10->MoveWindow(0, 20, cx, cy - 20);
        m_edit11->MoveWindow(0, 20, cx, cy - 20);
        m_edit12->MoveWindow(0, 20, cx, cy - 20);
        m_edit13->MoveWindow(0, 20, cx, cy - 20);
        m_edit14->MoveWindow(0, 20, cx, cy - 20);
        m_edit15->MoveWindow(0, 20, cx, cy - 20);
        m_edit16->MoveWindow(0, 20, cx, cy - 20);
        m_edit17->MoveWindow(0, 20, cx, cy - 20);
        m_edit18->MoveWindow(0, 20, cx, cy - 20);
    }
}


void CDlgLogDisplay::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (pNMHDR->idFrom == 1234)
    {
        int sel = m_tab1->GetCurSel();
        switch (sel)
        {
            case 0:
                m_pDlgEdgeView->ShowWindow(SW_SHOW);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 1:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_SHOW);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 2:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_SHOW);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 3:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_SHOW);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 4:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_SHOW);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 5:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_SHOW);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 6:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_SHOW);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 7:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_SHOW);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 8:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_SHOW);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 9:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_SHOW);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 10:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_SHOW);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 11:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_SHOW);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 12:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_SHOW);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 13:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_SHOW);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 14:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_SHOW);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 15:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_SHOW);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 16:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_SHOW);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 17:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_SHOW);
                m_edit18->ShowWindow(SW_HIDE);
                break;
            case 18:
                m_pDlgEdgeView->ShowWindow(SW_HIDE);
                m_edit1->ShowWindow(SW_HIDE);
                m_edit2->ShowWindow(SW_HIDE);
                m_edit3->ShowWindow(SW_HIDE);
                m_edit4->ShowWindow(SW_HIDE);
                m_edit5->ShowWindow(SW_HIDE);
                m_edit6->ShowWindow(SW_HIDE);
                m_edit7->ShowWindow(SW_HIDE);
                m_edit8->ShowWindow(SW_HIDE);
                m_edit9->ShowWindow(SW_HIDE);
                m_edit10->ShowWindow(SW_HIDE);
                m_edit11->ShowWindow(SW_HIDE);
                m_edit12->ShowWindow(SW_HIDE);
                m_edit13->ShowWindow(SW_HIDE);
                m_edit14->ShowWindow(SW_HIDE);
                m_edit15->ShowWindow(SW_HIDE);
                m_edit16->ShowWindow(SW_HIDE);
                m_edit17->ShowWindow(SW_HIDE);
                m_edit18->ShowWindow(SW_SHOW);
                break;
            default:
                break;
        }
    }
    *pResult = 0;
}

void CDlgLogDisplay::GetEdgeView(CDlgEdgeImageView* _pDlgEdgeView)
{
    isEdgeViewExist = true;
    m_pDlgEdgeView = _pDlgEdgeView;
    m_pDlgEdgeView->SetParent(this);
    m_pDlgEdgeView->ShowWindow(SW_SHOW);
}