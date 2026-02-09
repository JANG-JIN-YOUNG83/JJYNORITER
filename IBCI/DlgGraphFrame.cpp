// DlgGraphFrame.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgGraphFrame.h"
#include "afxdialogex.h"

#include "../Module/Utility/SystemFont.h"

#include <Gadget/TimeCheck.h>

#define WM_UPDATE_GRAPH_SETDATA (WM_USER + 106)


#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgGraphFrame 대화 상자

IMPLEMENT_DYNAMIC(CDlgGraphFrame, CDialogEx)

CDlgGraphFrame::CDlgGraphFrame(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GRAPH_FRAME, pParent)
	, m_pDlgGraph(NULL)
	, m_bOK(FALSE)
	, m_isOnlyMaxDiffValueShowed(false)
	, m_resultUpDateRequest(false)
{
	m_Brushblock = CreateSolidBrush(RGB(0, 0, 0));
	m_Brushblue1 = CreateSolidBrush(RGB(46, 117, 182));
}

CDlgGraphFrame::~CDlgGraphFrame()
{
	if (NULL != m_pDlgGraph)
		delete m_pDlgGraph;

	DeleteObject(m_Brushblock);
	DeleteObject(m_Brushblue1);

	m_font1.DeleteObject();
	m_font2.DeleteObject();
	m_font3.DeleteObject();
	m_font4.DeleteObject();
}

void CDlgGraphFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGraphFrame, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_MESSAGE(WM_UPDATE_GRAPH_SETDATA, &CDlgGraphFrame::OnUpdateGraphSetData)
END_MESSAGE_MAP()


// CDlgGraphFrame 메시지 처리기
BOOL CDlgGraphFrame::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pDlgGraph = new CDlgGraph(this);
	m_pDlgGraph->Create(CDlgGraph::IDD, this);

	//TEXT 글씨
	GetDlgItem(IDC_STATIC_MAX_2)->SetFont(&m_font1);
	GetDlgItem(IDC_STATIC_MAX_2)->SetWindowText(_T("Max"));
	GetDlgItem(IDC_STATIC_AVG_2)->SetFont(&m_font1);
	GetDlgItem(IDC_STATIC_AVG_2)->SetWindowText(_T("Average"));
	GetDlgItem(IDC_STATIC_MIN_2)->SetFont(&m_font1);
	GetDlgItem(IDC_STATIC_MIN_2)->SetWindowText(_T("Min"));
	GetDlgItem(IDC_STATIC_SPEC)->SetFont(&m_font1);
	GetDlgItem(IDC_STATIC_AIMAX_2)->SetWindowText(_T("AIMax"));
	GetDlgItem(IDC_STATIC_AIMIN_2)->SetWindowText(_T("AIMin"));

	//BOX 글씨
	GetDlgItem(IDC_STATIC_MAX)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_MAX)->SetWindowText(_T("0.00"));
	GetDlgItem(IDC_STATIC_AVG)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_AVG)->SetWindowText(_T("(Average)"));
	GetDlgItem(IDC_STATIC_MIN)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_MIN)->SetWindowText(_T("0.00"));
	GetDlgItem(IDC_STATIC_AIMAX)->SetWindowText(_T("0.00"));
	GetDlgItem(IDC_STATIC_AIMIN)->SetWindowText(_T("0.00"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HBRUSH CDlgGraphFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void CDlgGraphFrame::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//그래프 다이얼로그 생성
	CRect rtROI;
	GetClientRect(rtROI);
	
	
	::MoveWindow(GetDlgItem(IDC_STATIC_SPEC)->GetSafeHwnd(),	rtROI.Width() * 0.02,	rtROI.Height() * 0.92,	rtROI.Width() * 0.2, rtROI.Height() * 0.08, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_MAX_2)->GetSafeHwnd(),	rtROI.Width() * 0.22,	rtROI.Height() * 0.92,	rtROI.Width() * 0.15, rtROI.Height() * 0.08, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_MAX)->GetSafeHwnd(),		rtROI.Width() * 0.37,	rtROI.Height() * 0.92,	rtROI.Width() * 0.15, rtROI.Height() * 0.08, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_MIN_2)->GetSafeHwnd(),	rtROI.Width() * 0.52,	rtROI.Height() * 0.92,	rtROI.Width() * 0.15, rtROI.Height() * 0.08, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_MIN)->GetSafeHwnd(),		rtROI.Width() * 0.67,	rtROI.Height() * 0.92,	rtROI.Width() * 0.15, rtROI.Height() * 0.08, TRUE);
	


	//if (TRUE == g_systemParameter.systemOption.m_bShowSpecWindow)
	//{
	//	::MoveWindow(m_pDlgGraph->GetSafeHwnd(), 2, height * 0.01, rtROI.Width() * 0.99, rtROI.Height() - height * 0.15, TRUE);
	//}
	//else
	//{
	//	::MoveWindow(m_pDlgGraph->GetSafeHwnd(), 2, height * 0.01, rtROI.Width() * 0.99, rtROI.Height() - height * 0.03, TRUE);
	//}
	::MoveWindow(m_pDlgGraph->GetSafeHwnd(), rtROI.Width() * 0.01, rtROI.Width() * 0.01, rtROI.Width() * 0.99, rtROI.Height() * 0.9, TRUE);
}

BOOL CDlgGraphFrame::PreTranslateMessage(MSG* pMsg)
{
	if (VK_RETURN || VK_ESCAPE == pMsg->wParam)
	{
		return FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

//title,min,avg,max on/off
void CDlgGraphFrame::SetTextBox(BOOL Title, BOOL Min, BOOL Avg, BOOL max, BOOL spec)
{
	if (TRUE == Min)
	{
		this->GetDlgItem(IDC_STATIC_MIN_2)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_STATIC_MIN)->ShowWindow(TRUE);
	}
	else
	{
		this->GetDlgItem(IDC_STATIC_MIN_2)->ShowWindow(FALSE);
		this->GetDlgItem(IDC_STATIC_MIN)->ShowWindow(FALSE);
	}

	if (TRUE == Avg)
	{
		this->GetDlgItem(IDC_STATIC_AVG_2)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_STATIC_AVG)->ShowWindow(TRUE);
	}
	else
	{
		this->GetDlgItem(IDC_STATIC_AVG_2)->ShowWindow(FALSE);
		this->GetDlgItem(IDC_STATIC_AVG)->ShowWindow(FALSE);
	}

	if (TRUE == max)
	{
		this->GetDlgItem(IDC_STATIC_MAX_2)->ShowWindow(TRUE);
		this->GetDlgItem(IDC_STATIC_MAX)->ShowWindow(TRUE);
	}
	else
	{
		this->GetDlgItem(IDC_STATIC_MAX_2)->ShowWindow(FALSE);
		this->GetDlgItem(IDC_STATIC_MAX)->ShowWindow(FALSE);
	}

	if (TRUE == spec)
	{
		GetDlgItem(IDC_STATIC_SPEC)->ShowWindow(TRUE);

	}
	else
	{
		GetDlgItem(IDC_STATIC_SPEC)->ShowWindow(FALSE);
	}
}

void CDlgGraphFrame::SetPosition(int ID, long Left, long Top, long With, long Height)
{
	CWnd* pCtl;
	if (ID != NULL)
	{
		pCtl = this->GetDlgItem(ID);
		pCtl->MoveWindow(Left, Top, With, Height);
	}
	else
	{
		return;
	}
}

void CDlgGraphFrame::SetFont(int m_nID)
{
	if (m_nID == TEXT)
	{
		GetDlgItem(IDC_STATIC_SPEC)->SetFont(&m_font1);
		GetDlgItem(IDC_STATIC_MAX_2)->SetFont(&m_font1);
		GetDlgItem(IDC_STATIC_MIN_2)->SetFont(&m_font1);
		GetDlgItem(IDC_STATIC_AIMAX_2)->SetFont(&m_font1);
		GetDlgItem(IDC_STATIC_AIMIN_2)->SetFont(&m_font1);
	}
	else if (m_nID == BOX)
	{
		GetDlgItem(IDC_STATIC_MIN)->SetFont(&m_font2);
		GetDlgItem(IDC_STATIC_MAX)->SetFont(&m_font2);
		GetDlgItem(IDC_STATIC_AIMIN)->SetFont(&m_font2);
		GetDlgItem(IDC_STATIC_AIMAX)->SetFont(&m_font2);

	}
	else if (m_nID == SPEC)
	{
		GetDlgItem(IDC_STATIC_SPEC)->SetFont(&m_font4);
	}
}


void CDlgGraphFrame::FontChange(FontMode m_Mode, long fontSize)
{
	switch (m_Mode)
	{
	case BOX:
		Utility::SystemFont::CreateGUIFont(m_font2, fontSize);
		SetFont(BOX);
		break;

	case TEXT:
		Utility::SystemFont::CreateGUIFont(m_font1, fontSize);
		SetFont(TEXT);
		break;
	case SPEC:
		Utility::SystemFont::CreateGUIFont(m_font4, fontSize);
		SetFont(SPEC);
		break;
	}
}

void CDlgGraphFrame::Initialize(const std::vector<DefectType>& inspTypes, LPCTSTR key, LPCTSTR title, bool isOnlyMaxDiffValueShowed)
{
	m_graphKey = key;
	m_inspTypeToID.clear();
	m_isOnlyMaxDiffValueShowed = isOnlyMaxDiffValueShowed;

	if (inspTypes.size() == 0)
		return;

	for (long index = 0; index < (long)inspTypes.size(); index++)
	{
		auto& inspType = inspTypes[index];
		m_inspTypeToID[inspType] = index;
	}

	m_pDlgGraph->SetTitle(title);

	if (isOnlyMaxDiffValueShowed)
	{
		// 가장 큰놈만 보여주는 모드
		m_pDlgGraph->SetDataGroupCount(2);
	}
	else
	{
		m_pDlgGraph->SetDataGroupCount((long)inspTypes.size() * 2);
	}
}

void CDlgGraphFrame::SetLogMode(int Mode)
{
	m_pDlgGraph->SetLogMode(Mode);
}

void CDlgGraphFrame::ShowAIspec(BOOL bShow)
{
	CRect rtROI;
	GetClientRect(rtROI);
	ScreenToClient(rtROI);

	long height;
	if (rtROI.Height() > 250)
		height = 250;
	else if (rtROI.Height() < 200)
		height = 200;
	else
		height = rtROI.Height();

	if (!bShow) 
	{
		::MoveWindow(GetDlgItem(IDC_STATIC_MAX_2)->GetSafeHwnd(), rtROI.Width() * 0.2, rtROI.Height() - height * 0.12, rtROI.Width() * 0.13, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_MAX)->GetSafeHwnd(), rtROI.Width() * 0.35, rtROI.Height() - height * 0.12, rtROI.Width() * 0.19, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_MIN_2)->GetSafeHwnd(), rtROI.Width() * 0.6, rtROI.Height() - height * 0.12, rtROI.Width() * 0.13, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_MIN)->GetSafeHwnd(), rtROI.Width() * 0.75, rtROI.Height() - height * 0.12, rtROI.Width() * 0.19, height * 0.09, TRUE);
		GetDlgItem(IDC_STATIC_AIMIN_2)->ShowWindow(bShow);
		GetDlgItem(IDC_STATIC_AIMIN)->ShowWindow(bShow);
		GetDlgItem(IDC_STATIC_AIMAX_2)->ShowWindow(bShow);
		GetDlgItem(IDC_STATIC_AIMAX)->ShowWindow(bShow);
		Invalidate();
	}
	else
	{
		GetDlgItem(IDC_STATIC_AIMIN_2)->ShowWindow(bShow);
		GetDlgItem(IDC_STATIC_AIMIN)->ShowWindow(bShow);
		GetDlgItem(IDC_STATIC_AIMAX_2)->ShowWindow(bShow);
		GetDlgItem(IDC_STATIC_AIMAX)->ShowWindow(bShow);

		::MoveWindow(GetDlgItem(IDC_STATIC_MAX_2)->GetSafeHwnd(), rtROI.Width() * 0.18, rtROI.Height() - height * 0.12, rtROI.Width() * 0.08, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_MAX)->GetSafeHwnd(), rtROI.Width() * 0.26, rtROI.Height() - height * 0.12, rtROI.Width() * 0.105, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_MIN_2)->GetSafeHwnd(), rtROI.Width() * 0.365, rtROI.Height() - height * 0.12, rtROI.Width() * 0.08, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_MIN)->GetSafeHwnd(), rtROI.Width() * 0.445, rtROI.Height() - height * 0.12, rtROI.Width() * 0.105, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_AIMAX_2)->GetSafeHwnd(), rtROI.Width() * 0.55, rtROI.Height() - height * 0.12, rtROI.Width() * 0.1, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_AIMAX)->GetSafeHwnd(), rtROI.Width() * 0.65, rtROI.Height() - height * 0.12, rtROI.Width() * 0.105, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_AIMIN_2)->GetSafeHwnd(), rtROI.Width() * 0.755, rtROI.Height() - height * 0.12, rtROI.Width() * 0.1, height * 0.09, TRUE);
		::MoveWindow(GetDlgItem(IDC_STATIC_AIMIN)->GetSafeHwnd(), rtROI.Width() * 0.855, rtROI.Height() - height * 0.12, rtROI.Width() * 0.105, height * 0.09, TRUE);
	}
}

void CDlgGraphFrame::DataSetZero()
{
	m_pDlgGraph->DataSetZero();
}

void CDlgGraphFrame::SetSpec(float fOffset, float fSpecNominal, float fSapeMin, float fSpecMax, float fLCL, float fUCL, float fLSL, float fUSL, float fSpecMinAi, float fSpecMaxAi, float fSpecMinNg, float fSpecMaxNg)
{
	CString min, max;
	min.Format(_T("%.2f"), fSapeMin);
	max.Format(_T("%.2f"), fSpecMax);

	SetDlgItemText(IDC_STATIC_MIN, min);
	SetDlgItemText(IDC_STATIC_MAX, max);

	CString AImin, AImax;
	AImin.Format(_T("%.2f"), fSpecMinAi);
	AImax.Format(_T("%.2f"), fSpecMaxAi);

	SetDlgItemText(IDC_STATIC_AIMIN, AImin);
	SetDlgItemText(IDC_STATIC_AIMAX, AImax);

	//if (TRUE == g_systemParameter.systemOption.m_bShowSpecWindow)
	//{
	//	this->GetDlgItem(IDC_STATIC_MIN_2)->ShowWindow(SW_SHOW);
	//	this->GetDlgItem(IDC_STATIC_MIN)->ShowWindow(SW_SHOW);
	//	this->GetDlgItem(IDC_STATIC_AVG_2)->ShowWindow(SW_SHOW);
	//	this->GetDlgItem(IDC_STATIC_AVG)->ShowWindow(SW_SHOW);
	//	this->GetDlgItem(IDC_STATIC_MAX_2)->ShowWindow(SW_SHOW);
	//	this->GetDlgItem(IDC_STATIC_MAX)->ShowWindow(SW_SHOW);
	//	GetDlgItem(IDC_STATIC_SPEC)->ShowWindow(SW_SHOW);


	//	if ((NODATA > fSpecMinAi) && (NODATA > fSpecMaxAi))
	//		ShowAIspec(FALSE);
	//	else
	//		ShowAIspec(TRUE);
	//}
	//else
	//{
	//	this->GetDlgItem(IDC_STATIC_MIN_2)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_STATIC_MIN)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_STATIC_AVG_2)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_STATIC_AVG)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_STATIC_MAX_2)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_STATIC_MAX)->ShowWindow(SW_HIDE);
	//	GetDlgItem(IDC_STATIC_SPEC)->ShowWindow(SW_HIDE);
	//}

	this->GetDlgItem(IDC_STATIC_MIN_2)->ShowWindow(SW_SHOW);
	this->GetDlgItem(IDC_STATIC_MIN)->ShowWindow(SW_SHOW);
	this->GetDlgItem(IDC_STATIC_AVG_2)->ShowWindow(SW_SHOW);
	this->GetDlgItem(IDC_STATIC_AVG)->ShowWindow(SW_SHOW);
	this->GetDlgItem(IDC_STATIC_MAX_2)->ShowWindow(SW_SHOW);
	this->GetDlgItem(IDC_STATIC_MAX)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_SPEC)->ShowWindow(SW_SHOW);


	if ((NODATA > fSpecMinAi) && (NODATA > fSpecMaxAi))
		ShowAIspec(FALSE);
	else
		ShowAIspec(TRUE);

	float fmin = fSapeMin - abs(fSapeMin - fSpecMax) * 0.1;
	float fmax = fSpecMax + abs(fSapeMin - fSpecMax) * 0.1;


	m_pDlgGraph->SetGraphParameter(fOffset, fmin, fmax, fSpecNominal, fSapeMin, fSpecMax, fLCL, fUCL, fLSL, fUSL, fSpecMinAi, fSpecMaxAi, fSpecMinNg, fSpecMaxNg);
}

BOOL CDlgGraphFrame::SetData(BOOL bEven, DefectType inspType, int index, float value, BOOL bInvalidData, CString strJudge)
{
	m_bEven = bEven;
	m_inspType = inspType;
	m_index = index;
	m_value = value;
	m_bInvalidData = bInvalidData;
	m_strJudge = strJudge;

	if (!m_resultUpDateRequest)
	{
		m_resultUpDateRequest = true;


		//PostMessage(WM_UPDATE_GRAPH_SETDATA);
        SendMessage(WM_UPDATE_GRAPH_SETDATA);
	}

	return TRUE;
}

BOOL CDlgGraphFrame::SetData2(BOOL bEven, DefectType inspType, int index, float value, BOOL bInvalidData, CString strJudge)
{
	if (NULL == m_pDlgGraph)
		return FALSE;

	//if (!g_systemParameter.systemOption.m_graphDisplayByFeeder)
	//{
	//	bEven = FALSE;
	//}

	m_bOK = (_T("OK") == strJudge) ? TRUE : FALSE;

	BOOL bRet = FALSE;
	auto id = 0;

	if (m_isOnlyMaxDiffValueShowed)
	{
		bRet = m_pDlgGraph->SetAccMaxDiffData((bEven ? 1 : 0), index, value, m_bOK, bInvalidData);
	}
	else
	{
		auto itID = m_inspTypeToID.find(inspType);
		if (itID != m_inspTypeToID.end())
		{
			bRet = m_pDlgGraph->SetData(itID->second * 2 + (bEven ? 1 : 0), index, value, m_bOK, bInvalidData);
		}
	}
	return bRet;
}

const std::map<DefectType, long>& CDlgGraphFrame::GetInspTypes() const
{
	return m_inspTypeToID;
}

LPCTSTR CDlgGraphFrame::GetGraphKey() const
{
	return m_graphKey;
}

bool CDlgGraphFrame::CheckInspNameValidate(DefectType inspType)
{
	return m_inspTypeToID.find(inspType) != m_inspTypeToID.end();
}

LRESULT CDlgGraphFrame::OnUpdateGraphSetData(WPARAM, LPARAM)
{
	m_resultUpDateRequest = false;
    SetData2(m_bEven, m_inspType, m_index, m_value, m_bInvalidData, m_strJudge);

	return 0;
}