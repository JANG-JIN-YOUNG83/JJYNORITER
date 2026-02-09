// CDlgVIewGrabImage.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgVIewGrabImage.h"
#include "afxdialogex.h"

#include <Widget/ImageView.h>

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgVIewGrabImage 대화 상자

IMPLEMENT_DYNAMIC(CDlgVIewGrabImage, CDialogEx)

CDlgVIewGrabImage::CDlgVIewGrabImage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIEW_GRAB_IMAGE, pParent)
    , m_cameraIndex(4)
{
	::memset(m_imageViews, 0, sizeof(m_imageViews));
}

CDlgVIewGrabImage::~CDlgVIewGrabImage()
{
    for (int cameraIndex = 0; cameraIndex < m_cameraIndex; cameraIndex++)
	{
		delete m_imageViews[cameraIndex];
	}
}

void CDlgVIewGrabImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgVIewGrabImage, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgVIewGrabImage 메시지 처리기
BOOL CDlgVIewGrabImage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	LPCTSTR captions[6] =
	{
		_T("Grab #1"),
		_T("#1"),
		_T("Grab #2"),
		_T("#2"),
		//_T("CAM #5"),
		//_T("CAM #6"),
		_T("Gradient Image"),
		_T("ABS Image")
	};

	for (int cameraIndex = 0; cameraIndex < m_cameraIndex; cameraIndex++)
	{
		m_imageViews[cameraIndex] = new IPVM::ImageView(GetSafeHwnd(), IPVM::Rect(0, 0, 0, 0));

		m_imageViews[cameraIndex]->WindowOverlayAdd(IPVM::Point_32s_C2(3, 0), captions[cameraIndex], RGB(0, 255, 0), 18);
		m_imageViews[cameraIndex]->WindowOverlayShow();
	}

	CRect rtClient;




	GetClientRect(rtClient);

	Resize(rtClient.Width(), rtClient.Height());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgVIewGrabImage::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_imageViews[0]->GetSafeHwnd())
	{
		Resize(cx, cy);
	}
}

void CDlgVIewGrabImage::Resize(int cx, int cy)
{
	//불필요할 수 있으나 CDlgViewCellImage 와 동일한 위치에 있도록 하기 위해 수정
	CRect rtCombo;
	rtCombo.left = 0;
	rtCombo.top = 0;
	rtCombo.right = (long)cx * 0.1;
	rtCombo.bottom = (long)cy * 0.1;
	
	int nGap = 5;

	int nViewCx		= cx - 2 * nGap;
    int nViewCy     = cy - rtCombo.Height() - 2 * nGap;
	int nViewWidth	= nViewCx / 2;
    int nViewHeight = nViewCy / 2;

    ::MoveWindow(m_imageViews[0]->GetSafeHwnd(), 0, rtCombo.bottom, nViewWidth, nViewCy, TRUE);
  // ::MoveWindow(m_imageViews[1]->GetSafeHwnd(), 0, rtCombo.bottom + nViewHeight + nGap, nViewWidth, nViewHeight, TRUE);
    ::MoveWindow(m_imageViews[2]->GetSafeHwnd(), nViewWidth + nGap, rtCombo.bottom, nViewWidth, nViewCy, TRUE);
 //  ::MoveWindow(m_imageViews[3]->GetSafeHwnd(), nViewWidth + nGap, rtCombo.bottom + nViewHeight + nGap, nViewWidth,nViewHeight, TRUE);

	//::MoveWindow(m_imageViews[0]->GetSafeHwnd(), 0, 0, nViewWidth, nViewHeight, TRUE);
	//::MoveWindow(m_imageViews[1]->GetSafeHwnd(), 0, nViewHeight + nGap, nViewWidth, nViewHeight, TRUE);

	//::MoveWindow(m_imageViews[2]->GetSafeHwnd(), nViewWidth + nGap, 0                   , nViewWidth, nViewHeight, TRUE);
	//::MoveWindow(m_imageViews[3]->GetSafeHwnd(), nViewWidth + nGap, nViewHeight + nGap  , nViewWidth, nViewHeight, TRUE);

	//::MoveWindow(m_imageViews[4]->GetSafeHwnd(), 0, (nViewHeight + nGap) * 2, cx - nGap, nViewHeight, TRUE);
	//::MoveWindow(m_imageViews[5]->GetSafeHwnd(), 0, (nViewHeight + nGap) * 3, cx - nGap, nViewHeight, TRUE);

	//::MoveWindow(m_imageViews[4]->GetSafeHwnd(), 0, 0, nViewWidth, cy + nGap, TRUE);
 //   ::MoveWindow(m_imageViews[5]->GetSafeHwnd(), nViewWidth + nGap, 0, nViewWidth, cy + nGap, TRUE);
}
BOOL CDlgVIewGrabImage::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN)
            return TRUE;
        else if (pMsg->wParam == VK_ESCAPE)
            return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}
