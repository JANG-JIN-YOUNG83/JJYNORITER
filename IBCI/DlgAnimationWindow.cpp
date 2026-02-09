// CDlgGraphWindow.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgAnimationWindow.h"
#include "afxdialogex.h"
//#include "GraphOption.h"

#include "IBCIDlg.h"
#include "AnimationCustomizeMismatch.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_BLUE					RGB(46, 117, 182)
#define COLOR_GRAY2					RGB(75, 75, 75)
#define COLOR_BLACK					RGB(0, 0, 0)

// CDlgAnimationWindow 대화 상자
IMPLEMENT_DYNAMIC(CDlgAnimationWindow, CDialogEx)

CDlgAnimationWindow::CDlgAnimationWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ANIMATION_WINDOW, pParent)
{
	m_pParent = (CIBCIDlg*)pParent;
	m_pDlgAnimation = nullptr;

	m_pDlgAnimation = new CDlgAnimation(this);
}

CDlgAnimationWindow::~CDlgAnimationWindow()
{
	delete m_pDlgAnimation;
	
	m_font.DeleteObject();
	m_font_2.DeleteObject();

	m_BrushBlock.DeleteObject();
	m_BrushBlue.DeleteObject();
}

void CDlgAnimationWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAnimationWindow, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgAnimationWindow 메시지 처리기
BOOL CDlgAnimationWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_BrushBlue.CreateSolidBrush(COLOR_GRAY2);
	m_BrushBlock.CreateSolidBrush(COLOR_BLACK);
	
	m_pDlgAnimation->Create(IDD_DIALOG_ANIMATION, this);

	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgAnimationWindow::PreTranslateMessage(MSG* pMsg)
{
	if (VK_RETURN || VK_ESCAPE == pMsg->wParam)
	{
		return FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgAnimationWindow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	GetClientRect(&m_rtDlgSize);
    m_pDlgAnimation->m_animationStyle->UpdateViewSize(this->GetDC(), m_rtDlgSize.Width(), m_rtDlgSize.Height());
    Invalidate(FALSE);
}
void CDlgAnimationWindow::SetAnimationParameter(ResultMismatch Mismatch, float fOffset, float fMin, float fMax,
    float fSpecNominal, float fSpecMin, float fSpecMax, float fAiSpecMin, float fAiSpecMax, float fNgSpecMin,
    float fNgSpecMax, int nLaneCount, bool bOSDS, bool bRollerOS)
{
	m_pDlgAnimation->SetAnimationParameter( Mismatch,  fOffset,  fMin,  fMax,  fSpecNominal,  fSpecMin,  fSpecMax,  fAiSpecMin ,  fAiSpecMax ,  fNgSpecMin ,  fNgSpecMax, nLaneCount, bOSDS, bRollerOS);
}
void CDlgAnimationWindow::ArrangeWindow_Graph(long nHorizentalCnt, long nVerticalCount)
{
	//정렬
	CRect rtROI = m_rtDlgSize;
	//GetWindowRect(&rtROI);
	//ScreenToClient(rtROI);

	long FrameTop = rtROI.top + 50;
	long GraphTop = rtROI.top + 100;
	long nWidthStep = rtROI.Width() / nHorizentalCnt;
	long nHeightStep = (rtROI.Height() - 50) / nVerticalCount;
	
	long nPos(0);

	//for (long y = 0; y < nVerticalCount; y++)
	//{
	//	for (long x = 0; x < nHorizentalCnt; x++)
	//	{
	//		if (m_pvecDlgGraphFrame.size() <= nPos)
	//			continue;

	//		//Set Position of Frame and Graph
	//		::MoveWindow(m_pvecDlgGraphFrame[nPos]->GetSafeHwnd(), 
	//			rtROI.left + (nWidthStep * x),
	//			FrameTop + (nHeightStep * y), 
	//			nWidthStep, 
	//			nHeightStep, 
	//			TRUE);


	//		::ShowWindow(m_pvecDlgGraphFrame[nPos]->GetSafeHwnd(), SW_SHOW);

	//		m_pvecDlgGraphFrame[nPos]->FontChange(BOX, (long)(rtROI.Width() * 0.10));
	//		m_pvecDlgGraphFrame[nPos]->FontChange(TEXT, (long)(rtROI.Width() * 0.10));
	//		m_pvecDlgGraphFrame[nPos]->FontChange(SPEC, (long)(rtROI.Width() * 0.10));
	//		nPos++;
	//	}
	//}
}

void CDlgAnimationWindow::DataReset()
{
	m_pDlgAnimation->DataSetZero();
}