// DlgDefectImageView.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CDlgDefectImageView.h"
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

// DlgDefectImageView 대화 상자

IMPLEMENT_DYNAMIC(DlgDefectImageView, CDialogEx)

DlgDefectImageView::DlgDefectImageView( CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DEFECTIMAGE_VIEW, pParent)
	, m_DlgSize(0, 0, 0, 0)
	, m_nCount(0)
{
	m_parent = (CIBCIDlg*)pParent;
	SetIndex(6, 3);

}
void DlgDefectImageView::SetIndex(long nRow , long nCol )
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
}
DlgDefectImageView::~DlgDefectImageView()
{
	for (int i = 0; i < m_nVecSize; i++)
		delete m_pVecImageView[i];
}

void DlgDefectImageView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(DlgDefectImageView, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// DlgDefectImageView 메시지 처리기

void DlgDefectImageView::SetDlg(CIBCIDlg* pParent)
{
	m_parent = (CIBCIDlg*)pParent;
}

BOOL DlgDefectImageView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//::GetClientRect(this,&m_DlgSize);
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
			ControlSizeChange(IDC_STATIC_DEFECT_IMAGE1 + nNum, nNum, nRow, nCol);
			nNum++;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void DlgDefectImageView::UpdateUI(int nNumber, IPVM::Image_8u_C1* brightImage, IPVM::Image_8u_C1* darkImage)
{

	m_pVecImageView[nNumber]->ImageOverlayClear();
	m_pVecImageView[nNumber]->SetImage(*brightImage);
	m_pVecImageView[nNumber]->ImageOverlayShow();
		
	m_pVecImageView[nNumber+1]->ImageOverlayClear();
	m_pVecImageView[nNumber+1]->SetImage(*darkImage);
	m_pVecImageView[nNumber+1]->ImageOverlayShow();
	

}



void DlgDefectImageView::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.



	GetClientRect(&m_DlgSize);
	long nNum = 0;
	for (long nCol = 0; nCol < m_nCol; nCol++)
	{
		for (long nRow = 0; nRow < m_nRow; nRow++)
		{
			ControlSizeChange(IDC_STATIC_DEFECT_IMAGE1 + nNum, nNum, nRow, nCol);
			nNum++;
		}
	}

	
}

void DlgDefectImageView::SetPoint(long nType, DefectPointRect XY, CString str, int nNum)
{
	m_pVecImageView[nNum]->ImageOverlayAdd(IPVM::Point_32f_C2(XY.nX, XY.nY), XY.Color, 5.0);			// Point
	m_pVecImageView[nNum]->ImageOverlayShow();

}

void DlgDefectImageView::ControlSizeChange(int nID, int nNum, long nRow, long nCol)
{
	long	nStaticSizeX = m_DlgSize.Width() / m_nRow -6;
	long	nStaticSizeY = m_DlgSize.Height() / m_nCol-6;

	long	nGapX = 6;
	long	nGapY = 6;

	long	nStartPointX = m_DlgSize.left + nGapX;
	long	nStartPointY = m_DlgSize.top  + nGapY;

	CWnd* pCtl;
	pCtl = GetDlgItem(nID);

	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX + nStaticSizeX * nRow + nGapX * nRow-3
			, nStartPointY + nStaticSizeY * nCol + nGapY * nCol-3,
			nStaticSizeX,
			nStaticSizeY);
		
		pCtl->ShowWindow(SW_SHOW);

			::MoveWindow(m_pVecImageView[nNum]->GetSafeHwnd(),
				nStartPointX + nStaticSizeX * nRow + nGapX * nRow-3
				, nStartPointY + nStaticSizeY * nCol + nGapY * nCol-3,
				nStaticSizeX,
				nStaticSizeY,TRUE);
			m_pVecImageView[nNum]->ZoomImageFit();

	}
}
