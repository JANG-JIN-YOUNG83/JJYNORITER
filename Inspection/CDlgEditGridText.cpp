// CDlgEditGridText.cpp: 구현 파일
//
#include "resource.h"
#include "stdafx.h"
#include "Inspection.h"
#include "CDlgEditGridText.h"
#include "afxdialogex.h"


// CDlgEditGridText 대화 상자

IMPLEMENT_DYNAMIC(CDlgEditGridText, CDialogEx)

CDlgEditGridText::CDlgEditGridText(CString strTitle, CString strValue, CPoint cPoint, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_EDIT, pParent)
{
	m_cWnd = pParent;
	m_strValue = strValue;
	m_strTitle = strTitle;
	m_cPoint = cPoint;
}

CDlgEditGridText::~CDlgEditGridText()
{
}

void CDlgEditGridText::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEditGridText, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgEditGridText::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgEditGridText 메시지 처리기


BOOL CDlgEditGridText::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWindowText(m_strTitle);
	
	CRect rtROI;
	GetWindowRect(rtROI);
	
	
	MoveWindow(m_cPoint.x + 10, m_cPoint.y + 80, rtROI.Width(), rtROI.Height());
	ShowWindow(SW_SHOW);
	
	{
		GetDlgItem(IDC_EDIT)->SetFocus();
		SetDlgItemText(IDC_EDIT, m_strValue);
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgEditGridText::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItemText(IDC_EDIT, m_strValue);
	CDialogEx::OnOK();
}
