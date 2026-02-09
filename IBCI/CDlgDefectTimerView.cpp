// CDlgDefectTimerView.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CDlgDefectTimerView.h"
#include "afxdialogex.h"
#include "DlgDefectMap.h"
#include "Widget/ImageView.h"
#include "Types/Image_8u_C1.h"
#include "Types/Image_8u_C4.h"
#include "Types/Point_32f_C2.h"
#include "Types/LineSeg_32f.h"
#include "Types/Rect_32f.h"
#include "Types/Rect.h"
#include "Algorithm/ImageProcessing.h"
// CDlgDefectTimerView 대화 상자

IMPLEMENT_DYNAMIC(CDlgDefectTimerView, CDialogEx)

CDlgDefectTimerView::CDlgDefectTimerView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DEFECTTIMERVIEW, pParent)
{
    m_pParent = (CDlgDefectMap*)pParent;
}

CDlgDefectTimerView::~CDlgDefectTimerView()
{
}

void CDlgDefectTimerView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDefectTimerView, CDialogEx)
ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgDefectTimerView 메시지 처리기

BOOL CDlgDefectTimerView::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    
    CDlgDefectMap* pMainFrm = static_cast<CDlgDefectMap*>(::AfxGetMainWnd());
    CRect rect;
    //m_pParent->m_nDefectMapSizeX;
    //m_pParent->m_nDefectMapSizeY;
    pMainFrm->GetDlgItem(IDC_STATIC_DEFECT_2_TITLE)->GetWindowRect(rect);
    //pMainFrm->ScreenToClient(m_pParent->m_rtSize);
    ::MoveWindow(this->m_hWnd, m_pParent->m_rtSize.left, m_pParent->m_rtSize.top, m_pParent->m_rtSize.Width(),
        m_pParent->m_rtSize.Height(), TRUE);
    //::MoveWindow(this->m_hWnd, 0, 0, 633, 410, TRUE);
    //DlgControlSetting();
    

    //GetDlgItem(IDC_STATIC_TIMERVEIWIMAGE)->ShowWindow(SW_HIDE);
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgDefectTimerView::Init()
{
    CRect rt(0, 0, 0, 0);
    
    m_pImageView = new IPVM::ImageView(this->GetSafeHwnd(), rt, (long)IDC_STATIC_TIMERVEIWIMAGE);
    CALL_Function_CSC();

    IPVM::Image_8u_C1 Image;
    TCHAR programpath[_MAX_PATH];
    GetCurrentDirectory(_MAX_PATH, programpath);
    CString ImageFolderName = _T("D:\\UC1\\DataSearchImage.jpg");
    CString strImagePath = programpath + ImageFolderName;
    Image.LoadFrom(ImageFolderName);
    m_pImageView->SetImage(Image);
}

void CDlgDefectTimerView::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    GetClientRect(&m_DlgSize);
    CALL_Function_CSC();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgDefectTimerView::CALL_Function_CSC()
{
    // Static
    ControlSizeChange(IDC_STATIC_TIMERVEIWIMAGE);

}

void CDlgDefectTimerView::ControlSizeChange(int nID)
{
    CWnd* pCtl;

    long nStartPointX = m_DlgSize.Width() * 0.01;
    long nStartPointY = m_DlgSize.top;

    long nStaticSizeX = m_DlgSize.Width() * 0.49;
    long nTitleSizeX = nStaticSizeX;
    long nBtnSizeX = m_DlgSize.Width();

    long nStaticSizeY = m_DlgSize.Height() * 0.9;
    long nTitleSizeY = m_DlgSize.Height() * 0.05;
    long nBtnSizeY = m_DlgSize.Height() * 0.05;



    // Btn
    pCtl = GetDlgItem(nID);
    if (pCtl != NULL)
    {
        //pCtl->MoveWindow(nStartPointX, nStartPointY + nBtnSizeY, nStaticSizeX, nTitleSizeY);
        pCtl->MoveWindow(nStartPointX * 2 + 10 , nStartPointY + nBtnSizeY, nStaticSizeX*1.9, nStaticSizeY * 0.5);
        ::MoveWindow(m_pImageView->GetSafeHwnd(), nStartPointX * 2 + 10, nStartPointY + nBtnSizeY, nStaticSizeX * 1.9,
            nStaticSizeY * 0.5, TRUE);
    }
}

void CDlgDefectTimerView::SetShowWindow(bool bShow)
{
    ::EnableWindow(m_pImageView->GetSafeHwnd(), bShow);
}

void CDlgDefectTimerView::SetROI(std::vector<IPVM::Rect> vecROI)
{
 /*   IPVM::Image_8u_C1 Image;
    TCHAR programpath[_MAX_PATH];
    GetCurrentDirectory(_MAX_PATH, programpath);
    CString ImageFolderName = _T("D:\\UC1\\DataSearchImage.jpg");
    CString strImagePath = programpath + ImageFolderName;
    Image.LoadFrom(ImageFolderName);
    m_pImageView->SetImage(Image);*/
    if (m_pImageView)
    {
        for (long nROI = 0; nROI < vecROI.size(); nROI++)
        {
            //m_pImageView->ImageOverlayClear();
            //m_pImageView->SetImage(Image);
            m_pImageView->ImageOverlayShow();
            m_pImageView->ImageOverlayAdd(vecROI[nROI], RGB(255, 0, 0), 2);
            m_pImageView->ImageOverlayShow();
        }
    }

}

void CDlgDefectTimerView::ROIClear()
{
    if (m_pImageView)
    {
        m_pImageView->ImageOverlayClear();
        m_pImageView->ImageOverlayShow();
    }

    //m_pImageView->SetImage(Image);
}