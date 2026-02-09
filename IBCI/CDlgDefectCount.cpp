// CDlgDefectCount.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CDlgDefectCount.h"
#include "afxdialogex.h"
#include "../Module/CustomControl/CustomStatic.h"
#include "IBCIDlg.h"

// CDlgDefectCount 대화 상자

IMPLEMENT_DYNAMIC(CDlgDefectCount, CDialogEx)

CDlgDefectCount::CDlgDefectCount(long nRow, long nCol, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DEFECTCOUNT, pParent)
{
	m_nRow = nRow;
	m_nCol = nCol;
	m_bInitCheck = FALSE; // onSize()체크
	m_vecCustomStatic.resize(m_nRow * m_nCol * 4);

	for (long nX = 0; nX < m_nRow; nX++)
	{
		for (long nY = 0; nY < m_nCol; nY++)
		{
			m_vecCustomStatic[(nX * 4) + nY * m_nRow * 4] = new CCustomStatic;
			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4] = new CCustomStatic;
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4] = new CCustomStatic;
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4] = new CCustomStatic;
		}
	}
}

CDlgDefectCount::~CDlgDefectCount()
{
	for (long nX = 0; nX < m_nRow * 4; nX++)
	{
		for (long nY = 0; nY < m_nCol; nY++)
		{
			delete m_vecCustomStatic[nX + nY * m_nRow * 4];// = new CCustomStatic;

		}

	}
}

void CDlgDefectCount::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (long nX = 0; nX < m_nRow; nX++)
	{
		for (long nY = 0; nY < m_nCol; nY++)
		{

			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1 + nX * 4 + nY * m_nRow * 4, *m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]);
			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1 + nX * 4 + 1 + nY * m_nRow * 4, *m_vecCustomStatic[nX * 4 + 1 + nY * m_nRow * 4]);
			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1 + nX * 4 + 2 + nY * m_nRow * 4, *m_vecCustomStatic[nX * 4 + 2 + nY * m_nRow * 4]);
			DDX_Control(pDX, IDC_STATIC_DEFECTCOUNT1 + nX * 4 + 3 + nY * m_nRow * 4, *m_vecCustomStatic[nX * 4 + 3 + nY * m_nRow * 4]);

		}

	}

}


BEGIN_MESSAGE_MAP(CDlgDefectCount, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgDefectCount 메시지 처리기
void CDlgDefectCount::SetDlg(CIBCIDlg* pParent)
{
	m_parent = pParent;
}

BOOL CDlgDefectCount::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	GetClientRect(&m_DlgSize);


	for (long nY = 0; nY < m_nRow; nY++)
	{
		for (long nX = 0; nX < m_nCol; nX++)
		{
			//m_vecCustomStatic[nX * nY] = new CCustomStatic;



			/*long cx = 100;
			long cy = 50;
			long cx2 = 40;
			long xterm = cx + cx2 * 3;*/

			long cx = m_DlgSize.Width() / (m_nCol * 4);
			long cx2 = m_DlgSize.Width() / (m_nCol * 4);
			long cy = m_DlgSize.Height() / m_nRow;
			long xterm = cx * 4;

			CString strTemp0;
			strTemp0.Format(_T("temp"));
			m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
			m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
			m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
			m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->SetWindowText(strTemp0);

			CString strTemp1;
			strTemp1.Format(_T("1"));
			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->SetWindowText(strTemp1);

			strTemp1.Format(_T("2"));
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->SetWindowText(strTemp1);

			strTemp1.Format(_T("3"));
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->ModifyStyle(0x0FL, SS_OWNERDRAW);
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->SetWindowText(strTemp1);

			m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm, cy * nY, cx - 1, cy - 1);
			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + cx, cy * nY, cx2 - 1, cy - 1);
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + cx + cx2, cy * nY, cx2 - 1, cy - 1);
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + cx + cx2 + cx2, cy * nY, cx2 - 1, cy - 1);


			m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->ShowWindow(TRUE);
			m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->ShowWindow(TRUE);
			m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->ShowWindow(TRUE);
			m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->ShowWindow(TRUE);

			//m_vecCustomStatic[nX * nY]->MoveWindow(nOffsetStartX + (int)(cx * 0.2 + 2), nOffsetStartY + logoSizeY - titltSizeY, (int)(cx * 0.1), logoSizeY);
		}

	}
	m_bInitCheck = TRUE;
	UpdateData();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgDefectCount::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	GetClientRect(&m_DlgSize);

	long staticX = m_DlgSize.Width() / (m_nCol * 4); // 각 객체
	long cx2 = m_DlgSize.Width() / (m_nCol * 4);	 
	long staticY = m_DlgSize.Height() / m_nRow;		 // 행 높이
	long xterm = staticX * 4;						 // 4개 공간

	if (m_bInitCheck)
	{
		for (long nY = 0; nY < m_nRow; nY++)
		{
			for (long nX = 0; nX < m_nCol; nX++)   
			{
				m_vecCustomStatic[nX * 4 + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm, staticY * nY, staticX - 1, staticY - 1);
				m_vecCustomStatic[(nX * 4 + 1) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + staticX, staticY * nY, cx2 - 1, staticY - 1);
				m_vecCustomStatic[(nX * 4 + 2) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + staticX + cx2, staticY * nY, cx2 - 1, staticY - 1);
				m_vecCustomStatic[(nX * 4 + 3) + nY * m_nRow * 4]->MoveWindow(0 + nX * xterm + staticX + cx2 + cx2, staticY * nY, cx2 - 1, staticY - 1);

			}
		}
	}
}
