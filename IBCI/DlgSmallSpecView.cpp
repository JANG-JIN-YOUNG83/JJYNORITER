// CDlgSmallSpecView.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgSmallSpecView.h"
#include "afxdialogex.h"
#include "../Inspection/Accessor.h"


#define	MESSAGE_GRID	(WM_USER + 400)

// CDlgSmallSpecView 대화 상자

IMPLEMENT_DYNAMIC(CDlgSmallSpecView, CDialogEx)

CDlgSmallSpecView::CDlgSmallSpecView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SMALL_SPEC_VIEW, pParent)
{

}

CDlgSmallSpecView::~CDlgSmallSpecView()
{
}

void CDlgSmallSpecView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSmallSpecView, CDialogEx)
END_MESSAGE_MAP()


// CDlgSmallSpecView 메시지 처리기
BOOL CDlgSmallSpecView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	GetClientRect(&m_dlgSize);
	CRect rtGrid(0, 0, m_dlgSize.right, m_dlgSize.bottom);
	m_ctrGrid.Create(rtGrid, this, MESSAGE_GRID);

	// 테이블 
	auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
	//isi_spec.m_mapSpec;

	vecItem.clear();

	long nRawNum = 0;
	for (int i = (long)DefectType::SmallDefect_CraterLevel4; i < (long)DefectType::ISI_END; i++)
	{
		CISISpecInfo* spec = isi_spec.GetSpecInfo((DefectType)i);
		if (nullptr != spec)
		{
			vecItem.push_back(std::make_pair((DefectType)i, spec));

		}
		nRawNum = (long)vecItem.size();
	}

	long nColNum = 9; // 예진 -- 컬럼 넣을 때 수정

	m_ctrGrid.SetColumnCount(nColNum);
	m_ctrGrid.SetRowCount(nRawNum + 1);
	m_ctrGrid.SetEditable(FALSE);
	m_ctrGrid.SetSingleColSelection(FALSE);
	m_ctrGrid.SetSingleRowSelection(FALSE);
	m_ctrGrid.SetFixedColumnSelection(FALSE);
	m_ctrGrid.SetFixedRowSelection(TRUE);
	m_ctrGrid.SetColumnResize(TRUE);
	m_ctrGrid.SetRowResize(TRUE);
	m_ctrGrid.SetFixedRowCount();
	m_ctrGrid.SetFixedColumnCount();

	std::vector<CString> vecTitle;
	/*0*/vecTitle.push_back(_T("         ITEM         "));
	/*1*/vecTitle.push_back(_T("SIZE X(mm)"));
	/*2*/vecTitle.push_back(_T(">,<   "));
	/*3*/vecTitle.push_back(_T("SIZE Y(mm)"));
	/*4*/vecTitle.push_back(_T(">,<   "));
	/*5*/vecTitle.push_back(_T("OR/AND"));
	/*6*/vecTitle.push_back(_T("COUNT"));
	/*7*/vecTitle.push_back(_T("Label Attach"));
	/*8*/vecTitle.push_back(_T("Continuous"));

	m_ctrGrid.SetItemText(0, 0, _T("         ITEM         "));

	for (int col = 0; col < nColNum; col++)
	{
		if (vecTitle.size() > col)
		{
			m_ctrGrid.SetItemText(0, col, vecTitle[col]);
		}
	}
	m_ctrGrid.AutoSizeColumns();



	for (int row = 0; row < nRawNum; row++)
	{
		if (vecItem.size() > row)
		{
			m_ctrGrid.SetItemText(row + 1, 0, GetDefectName(vecItem[row].first));
		}
	}

	m_nSizeRow = m_ctrGrid.GetRowHeight(0);


	for (int row = 0; row < nRawNum; row++)
	{
		for (int col = 0; col < nColNum; col++)
		{
			CString str;
			if (col == 1) // SIZE_X
			{
				str.Format(_T("%.1f"), vecItem[row].second->SIZE_X_mm.GetValue());
				m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 2) // SIZE_X CO
			{
				str.Format(_T("%d"), vecItem[row].second->SIZEX_COMPARE_OPERATOR.GetValue());
				if (str == _T("1"))
					str = _T("(크)<");
				else
					str = _T("(작)>");
				m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 3) // SIZE_Y
			{
				str.Format(_T("%.1f"), vecItem[row].second->SIZE_Y_mm.GetValue());
				m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 4) // SIZE_Y CO
			{
				str.Format(_T("%d"), vecItem[row].second->SIZEY_COMPARE_OPERATOR.GetValue());
				if (str == _T("1"))
					str = _T("(크)<");
				else
					str = _T("(작)>");
				m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 5) // OR/AND
			{
				str.Format(_T("%d"), vecItem[row].second->ORAND.GetValue());
				if (str == _T("1"))
					str = _T("AND");
				else
					str = _T("OR");
				m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 6)
			{
				str.Format(_T("%.d"), vecItem[row].second->COUNT.GetValue());
				m_ctrGrid.SetItemText(row + 1, col, str);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}