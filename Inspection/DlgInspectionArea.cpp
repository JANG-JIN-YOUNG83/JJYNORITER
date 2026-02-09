// DlgInspectionArea.cpp: 구현 파일
//

#include "resource.h"
#include "stdafx.h"
#include "Inspection.h"
#include "DlgInspectionArea.h"
#include "afxdialogex.h"

// CDlgInspectionArea 대화 상자
IMPLEMENT_DYNAMIC(CDlgInspectionArea, CDialogEx)

CDlgInspectionArea::CDlgInspectionArea(CWnd* pParent, CISICellAllDefect* pDefectInfo, long blobConditionSize, std::vector<long> vecAreaSet)
	: CDialogEx(IDD_DIALOG_INSPECTION_AREA, pParent)
	, m_DefectInfo(*pDefectInfo)
	, m_ncolSize(blobConditionSize)
	, m_nrowSize(14)
{

	m_vecInspAreaSet = vecAreaSet;
	/*m_vecInspAreaSet.clear();
	m_vecInspAreaSet.resize(blobConditionSize,0);*/
	if (m_vecInspAreaSet.size() < blobConditionSize)
	{
		m_vecInspAreaSet.resize(blobConditionSize);
	}
}

CDlgInspectionArea::~CDlgInspectionArea()
{
}

void CDlgInspectionArea::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INSPECTION_AREA, m_ctrList_area);
}


BEGIN_MESSAGE_MAP(CDlgInspectionArea, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_INSPECTION_AREA, &CDlgInspectionArea::OnNMClickListInspectionArea)
END_MESSAGE_MAP()


// CDlgInspectionArea 메시지 처리기


BOOL CDlgInspectionArea::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strColumn;
	CString strItem;
	int nWidth = 70;

	m_ctrList_area.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	
	m_ctrList_area.InsertColumn(0, _T(" "), LVCFMT_LEFT, nWidth * 2);

	// Inspection Area 정보는 CISIDefectProperty 클래스
    for (int row = 0; row < (enInspectionArea)IA_END; row++) // 원래는 row도 nInspectionArea의 갯수를 가져와야함
	{
        if (row == (enInspectionArea)IA_TAB_END)
            continue;
		strItem = ConvertInspectionArea().GetString((enInspectionArea)row);
		m_ctrList_area.InsertItem(row, strItem);
	}

	// m_para.m_vecBlobParaCondition.size(); 로 사이즈 가져온 것 - 열 개수
	for (int col = 1; col < m_ncolSize + 1; col++)
	{
		strColumn.Format(_T("%d"), col - 1);
		m_ctrList_area.InsertColumn(col, strColumn, LVCFMT_LEFT, nWidth);
	}

	for (int col = 0; col < m_ncolSize ; col++)
	{
		for (int row = 0; row < m_nrowSize; row++)
		{
			if (m_vecInspAreaSet[col] == row)
			{
				m_ctrList_area.SetItemText(row, col+1, _T("True"));
			}
			/*int numInspectionArea = m_DefectInfo.m_vecCandidate[col - 1].nInspectionArea;
			if (row == numInspectionArea)
			{
				m_ctrList_area.SetItemText(numInspectionArea, col, _T("True"));
			}*/
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CDlgInspectionArea::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);


	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDlgInspectionArea::OnNMClickListInspectionArea(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(true);
    if (pNMItemActivate->iSubItem > 0 && pNMItemActivate->iItem > 1)
	{
		int checkRow = checkRowItem(pNMItemActivate->iSubItem, m_nrowSize);
		
		if (pNMItemActivate->iItem != checkRow)
		{
			m_ctrList_area.SetItemText(checkRow, pNMItemActivate->iSubItem, _T(""));
			m_ctrList_area.SetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, _T("True"));
			setInspectionArea(pNMItemActivate->iSubItem - 1, pNMItemActivate->iItem);
		}

	}
	UpdateData(false);
	*pResult = 0;
}
