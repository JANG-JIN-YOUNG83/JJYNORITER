// DlgGraph.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgAnimation.h"
#include "afxdialogex.h"
#include "DlgAnimationWindow.h"

//
#include "AnimationCustomizeMismatch.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgAnimation 대화 상자

IMPLEMENT_DYNAMIC(CDlgAnimation, CDialogEx)

CDlgAnimation::CDlgAnimation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ANIMATION, pParent)
	//, m_dataXaxisCount(g_systemParameter.systemOption.m_graphMaxPointNum)
	, m_dataXaxisCount(50)
	, m_dataGroupCount(2)
{
	m_pParent = (CDlgAnimationWindow*)pParent;
	m_nLoginMode = -1;
	//m_graphStyle = std::make_shared<GraphCustomizeDefault>();
	m_animationStyle = std::make_shared<AnimationCustomizeMismatch>();

	// Machine Site별 Graph 모드 변경
	//switch (HostBase::Site::Get())
	//{
	//case HostBase::enumSite::LGESMI:	m_graphStyle = std::make_shared<GraphCustomizeLGESMI>();	break;
	//case HostBase::enumSite::LGESWA:	m_graphStyle = std::make_shared<GraphCustomizeLGESWA>();	break;
	//case HostBase::enumSite::LGESNA:	m_graphStyle = std::make_shared<GraphCustomizeLGESNA>();	break;
	//case HostBase::enumSite::LGESNB:	m_graphStyle = std::make_shared<GraphCustomizeLGESNB>();	break;
	//default:
	//	if (g_systemParameter.systemOption.nFriend == enFriend::FRIEND_SIEMENS)
	//		m_graphStyle = std::make_shared<GraphCustomizeLGESWA>();
	//	else
	//		m_graphStyle = std::make_shared<GraphCustomizeDefault>();
	//	break;
	//}
}

CDlgAnimation::~CDlgAnimation()
{
	End();
}

void CDlgAnimation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAnimation, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgAnimation 메시지 처리기
BOOL CDlgAnimation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	Ready();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgAnimation::PreTranslateMessage(MSG* pMsg)
{
	if (VK_RETURN || VK_ESCAPE == pMsg->wParam)
	{
		return FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CDlgAnimation::OnEraseBkgnd(CDC* pDC)
{
	return false;

	return CDialogEx::OnEraseBkgnd(pDC);
}


void CDlgAnimation::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	auto* oldBrush = m_MemDC.GetCurrentBrush();
	auto* oldFont = m_MemDC.SelectObject(GetFont());
	auto* oldPen = m_MemDC.GetCurrentPen();

	CRect rt;
	//GetClientRect(rt);
	//m_pParent->GetClientRect(rt);
	rt = m_pParent->m_rtDlgSize;
	long cx = rt.Width();
	long cy = rt.Height();
	::MoveWindow(this->m_hWnd, rt.left, rt.top, rt.Width(), rt.Height(), true);

	AnimationData data;
	if (true)
	{
		CSingleLock lock(&m_csData, TRUE);
		data = m_data;
	}

	//m_graphStyle->Draw(&m_MemDC, data, m_nLoginMode == Share::enLoginMode::Administrator);
	m_animationStyle->Draw(&m_MemDC, data, 0);

	m_MemDC.SelectObject(oldBrush);
	m_MemDC.SelectObject(oldFont);
	m_MemDC.SelectObject(oldPen);

	dc.BitBlt(0, 0, cx, cy, &m_MemDC, 0, 0, SRCCOPY);
}


void CDlgAnimation::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	Ready();
	Invalidate(FALSE);
}

void CDlgAnimation::SetAnimationParameter(ResultMismatch Mismatch, float fOffset, float fMin, float fMax,
    float fSpecNominal, float fSpecMin, float fSpecMax, float fAiSpecMin, float fAiSpecMax, float fNgSpecMin,
    float fNgSpecMax, int nLaneCount, bool bOSDS, bool bRollerOS)
{
	CSingleLock lock(&m_csData, TRUE);
	m_data.m_Mismatch = Mismatch;
	m_data.m_fYaxisMin = fMin;
	m_data.m_fYaxisMax = fMax;

	m_data.m_fSpecNominal = fSpecNominal;
	m_data.m_fSpecMin = fSpecMin;
	m_data.m_fSpecMax = fSpecMax;

	m_data.m_fSpecAiMin = fAiSpecMin;
	m_data.m_fSpecAiMax = fAiSpecMax;

	m_data.m_fSpecNgMin = fNgSpecMin;
	m_data.m_fSpecNgMax = fNgSpecMax;

	m_data.m_fOffset = fOffset;
	m_data.m_nLaneCount = nLaneCount;

	m_data.m_bOSDS = bOSDS;
    m_data.m_bRollerOS = bRollerOS;

	Invalidate(FALSE);
}

void CDlgAnimation::DataSetZero()
{
	CSingleLock lock(&m_csData, TRUE);

	//m_dataXaxisCount = g_systemParameter.systemOption.m_graphMaxPointNum;
	m_dataXaxisCount = 50;
	m_data.Init(m_dataXaxisCount, m_dataGroupCount);
	m_data.m_Mismatch.m_vecfDiffWidth.resize(4,0);
	m_data.m_Mismatch.m_vecResUpLane.resize(4);
	m_data.m_Mismatch.m_vecResDownLane.resize(4);
	m_data.m_Mismatch.m_vecResSpec.resize(4);
	m_animationStyle->DataSetZero();

	if (GetSafeHwnd()) Invalidate(FALSE);
}

BOOL CDlgAnimation::SetAccMaxDiffData(long id, int index, float value, BOOL judge, BOOL bInvalidData)
{
	CSingleLock lock(&m_csData, TRUE);

	if (!m_data.SetAccMaxDiffData(id, index, value, judge))
	{
		return FALSE;
	}

	if (TRUE == bInvalidData)
	{
		Invalidate();
	}

	return TRUE;
}

BOOL CDlgAnimation::SetData(long id, int index, float value, BOOL judge, BOOL bInvalidData)
{
	CSingleLock lock(&m_csData, TRUE);

	if (!m_data.SetData(id, index, value, judge))
	{
		return FALSE;
	}

	if (TRUE == bInvalidData)
	{
		Invalidate();
	}

	return TRUE;
}

void CDlgAnimation::SetTitle(LPCTSTR title)
{
	CSingleLock lock(&m_csData, TRUE);

	m_data.m_title = title;
}

void CDlgAnimation::SetLogMode(int mode)
{
	m_nLoginMode = mode;
}

void CDlgAnimation::Ready()
{
	if (m_MemDC.m_hDC)
	{
		End();
	}

	auto* pDC = GetDC();

	CRect rtClient, rtParent;
	GetClientRect(rtClient);
	//m_pParent->GetClientRect(rtClient);
	
	BOOL bResult = m_MemDC.CreateCompatibleDC(pDC);
	if (TRUE == bResult)
	{
		m_MemBitmap.CreateCompatibleBitmap(pDC, rtClient.Width(), rtClient.Height());
		m_pOldMemBitmap = m_MemDC.SelectObject(&m_MemBitmap);
	}
	else
	{
		DWORD error = GetLastError();
		TCHAR* message = nullptr;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			nullptr,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(TCHAR*)&message,
			0,
			nullptr);
		AfxMessageBox(_T("GDI A leak error has occurred. 4359834732"));
		AfxMessageBox(message);
	}

	m_animationStyle->UpdateViewSize(pDC, rtClient.Width(), rtClient.Height());

	ReleaseDC(pDC);
}

void CDlgAnimation::End()
{
	m_MemDC.SelectObject(m_pOldMemBitmap);
	m_MemDC.DeleteDC();
	m_MemBitmap.DeleteObject();

	m_pOldMemBitmap = NULL;
}

void CDlgAnimation::SetDataGroupCount(long groupCount)
{
	CSingleLock lock(&m_csData, TRUE);

	m_dataGroupCount = min(ANIMATION_DATA_MAX_NUM, groupCount);
	DataSetZero();

	Invalidate(FALSE);
}

void CDlgAnimation::SetGraphStyle(std::shared_ptr<AnimationCustomizeBase> animationStyle)
{
	m_animationStyle = animationStyle;
}

std::shared_ptr<AnimationCustomizeBase> CDlgAnimation::GetAnimationStyle()
{
	return m_animationStyle;
}