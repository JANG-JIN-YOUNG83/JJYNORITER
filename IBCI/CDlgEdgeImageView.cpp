// CDlgEdgeImageView.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CDlgEdgeImageView.h"
#include "afxdialogex.h"

#include "Widget/ImageView.h"
#include "Types/Image_8u_C1.h"
#include "Types/Image_8u_C4.h"
#include "Types/Point_32f_C2.h"
#include "Types/LineSeg_32f.h"
#include "Types/Rect.h"
#include "Algorithm/ImageProcessing.h"
#include "basedef.h"
#include "IBCIDlg.h"
// CDlgEdgeImageView 대화 상자

IMPLEMENT_DYNAMIC(CDlgEdgeImageView, CDialogEx)

CDlgEdgeImageView::CDlgEdgeImageView(long nRow, long nCol, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_EDGEIMAGE_VIEW, pParent)
    , m_DlgSize(0, 0, 0, 0)
    , m_nCount(0)
    , m_indexUpdate(0)
{
    m_parent = (CIBCIDlg*)pParent;
    SetIndex(nRow, nCol);
}
void CDlgEdgeImageView::SetIndex(long nRow, long nCol)
{
    m_nRow = nRow;
    m_nCol = nCol;

#ifdef _DEBUG
    if (m_nRow * m_nCol > 40)
    {
        AfxMessageBox(_T("장난하냐?"));
    }
#endif
    m_nVecSize = m_nRow * m_nCol; // 2행 4열
    m_pVecImageView.resize(m_nVecSize);

    m_indexUpdate = 0;
}
CDlgEdgeImageView::~CDlgEdgeImageView()
{
}

void CDlgEdgeImageView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEdgeImageView, CDialogEx)
END_MESSAGE_MAP()


// CDlgEdgeImageView 메시지 처리기
void CDlgEdgeImageView::SetDlg(CIBCIDlg* pParent)
{
    m_parent = (CIBCIDlg*)pParent;
}

BOOL CDlgEdgeImageView::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    for (long n = 0; n < m_nVecSize; n++)
    {
        IPVM::Rect rect1(0, 0, 0, 0);
        m_pVecImageView[n] = new IPVM::ImageView(this->GetSafeHwnd(), rect1, (long)IDC_STATIC_DEFECT_IMAGE1 + n);
    }

    GetClientRect(&m_DlgSize);
    long nNum = 0;
    for (long nCol = 0; nCol < m_nCol; nCol++)
    {
        for (long nRow = 0; nRow < m_nRow; nRow++)
        {
            ControlSizeChange(IDC_STATIC_EDGE_IMAGE1 + nNum, nNum, nRow, nCol);
            nNum++;
        }
    }
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgEdgeImageView::ControlSizeChange(int nID, int nNum, long nRow, long nCol)
{
    long nStaticSizeX = m_DlgSize.Width() / m_nRow - 6;
    long nStaticSizeY = m_DlgSize.Height() / m_nCol - 6;

    if (g_systemParameter.nLaneCount == 1)
    {
        nStaticSizeX = m_DlgSize.Width() / 4 - 6;
        nStaticSizeY = m_DlgSize.Height() / m_nCol - 6;
    }

    long nGapX = 6;
    long nGapY = 6;

    long nStartPointX = m_DlgSize.left + nGapX;
    long nStartPointY = m_DlgSize.top + nGapY;

    CWnd* pCtl;
    pCtl = GetDlgItem(nID);

    if (pCtl != NULL)
    {
        if (g_systemParameter.nLaneCount == 1 && nNum >= 4)
        {
            pCtl->MoveWindow(nStartPointX, nStartPointY, 0, 0);
            ::MoveWindow(m_pVecImageView[nNum]->GetSafeHwnd(), nStartPointX, nStartPointY, 0, 0, TRUE);
        }
        else
        {
            pCtl->MoveWindow(nStartPointX + nStaticSizeX * nRow + nGapX * nRow - 3,
                nStartPointY + nStaticSizeY * nCol + nGapY * nCol - 3, nStaticSizeX, nStaticSizeY);

            pCtl->ShowWindow(SW_SHOW);

            ::MoveWindow(m_pVecImageView[nNum]->GetSafeHwnd(), nStartPointX + nStaticSizeX * nRow + nGapX * nRow - 3,
                nStartPointY + nStaticSizeY * nCol + nGapY * nCol - 3, nStaticSizeX, nStaticSizeY, TRUE);
            m_pVecImageView[nNum]->ZoomImageFit();
        }
    }
}

void CDlgEdgeImageView::UpdateUI(std::vector<IPVM::Image_8u_C1> EdgeImages)
{
    for (long nEdge = 0; nEdge < EdgeImages.size(); nEdge++)
    {
        m_pVecImageView[nEdge]->ImageOverlayClear();
        m_pVecImageView[nEdge]->SetImage(EdgeImages[nEdge]);
        m_pVecImageView[nEdge]->ImageOverlayShow();
    }

}

void CDlgEdgeImageView::UpdateUI(std::vector<IPVM::Image_8u_C1> EdgeImages, std::vector<IPVM::LineSeg_32f> lineFinds, std::vector<CString> vecTexts)
{
    UpdateUI(EdgeImages);

    for (long nEdge = 0; nEdge < EdgeImages.size(); nEdge++)
    {
        //IPVM::Rect rt(EdgeImages[nEdge]);
        IPVM::LineSeg_32f lineFind(lineFinds[nEdge]);
        m_pVecImageView[nEdge]->ImageOverlayAdd(lineFind, RGB(255, 0, 0), 2);
    }

    //for (long cnt = 0; cnt < vecTexts.size(); cnt++)
    //{
    //    long nSize = 8;
    //    m_pVecImageView[m_indexUpdate]->ImageOverlayAdd(IPVM::Point_32f_C2(0, cnt * nSize), vecTexts[cnt], RGB(0, 255, 0), nSize);
    //    m_pVecImageView[m_indexUpdate + 1]->ImageOverlayAdd(IPVM::Point_32f_C2(0, cnt * nSize), vecTexts[cnt], RGB(0, 255, 0), nSize);
    //}

    for (long nEdge = 0; nEdge < EdgeImages.size(); nEdge++)
    {
        m_pVecImageView[nEdge]->ImageOverlayShow();
    }
    
    /*m_pVecImageView[m_indexUpdate]->ImageOverlayShow();
    m_pVecImageView[m_indexUpdate + 1]->ImageOverlayShow();*/

    //m_indexUpdate += 2;
    //if (m_indexUpdate >= m_nVecSize)
    //    m_indexUpdate = 0;
}

void CDlgEdgeImageView::ModifyLaneCount(long count)
{
    //if (count == 1)
    //{
    //    m_nCol = 1;
    //    m_nRow = 4;
    //        
    //}
    //else if (count == 2)
    //{
    //    m_nCol = 1;
    //    m_nRow = 6;
    //}

    long nNum = 0;
    for (long nCol = 0; nCol < m_nCol; nCol++)
    {
        for (long nRow = 0; nRow < m_nRow; nRow++)
        {
            ControlSizeChange(IDC_STATIC_EDGE_IMAGE1 + nNum, nNum, nRow, nCol);
            nNum++;
        }
    }
}