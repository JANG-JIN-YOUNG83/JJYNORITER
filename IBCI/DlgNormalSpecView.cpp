// DlgNormalSpecView.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgNormalSpecView.h"
#include "afxdialogex.h"
#include "../Inspection/Accessor.h"
#include "DlgEditGrid.h"


#define	MESSAGE_GRID	(WM_USER + 400)
// CDlgSpecView 대화 상자

IMPLEMENT_DYNAMIC(CDlgNormalSpecView, CDialogEx)

CDlgNormalSpecView::CDlgNormalSpecView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_NORMAL_SPEC_VIEW, pParent)
	, m_nRow(0)
	, m_nCol(0)
	, m_nSizeRow(0)
{

}

CDlgNormalSpecView::~CDlgNormalSpecView()
{
}

void CDlgNormalSpecView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNormalSpecView, CDialogEx)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CDlgSpecView 메시지 처리기

bool compare(std::pair<DefectType, std::vector<CISISpecInfo>*>& a, std::pair<DefectType, std::vector<CISISpecInfo>*>& b)
{
    if (a.second->size() == 0 || b.second->size() == 0)
    {
        return true;
    }

    if (a.second->at(0).PRIORITY < b.second->at(0).PRIORITY)
    {
        return true;
    }
    else
        return false;
}

BOOL CDlgNormalSpecView::OnInitDialog()
{
 	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.


	GetClientRect(&m_dlgSize);
	CRect rtGrid(m_dlgSize.left, m_dlgSize.top, m_dlgSize.right, m_dlgSize.bottom);
	m_rtGrid = rtGrid;
	m_ctrGrid.Create(rtGrid, this, MESSAGE_GRID);
	// CDlgClassificationST - m_ctrGrid 참고하기

	long nRawNum = 0;

	// 테이블 
	auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
	//isi_spec.m_mapSpec;

	vecItem_SpecView.clear();

    for (int n = 0; n < (long)DefectType::ISI_END; n++)
    {
        std::vector<CISISpecInfo>* spec = isi_spec.GetSpecInfo_Multi((DefectType)n);
        if (nullptr != spec)
        {
            vecItem_SpecView.push_back(std::make_pair((DefectType)n, spec));
            if (n > (long)DefectType::START_Coater_Surface && n < (long)DefectType::END_Coater_Surface)
            {
                nRawNum++;
			}
        }
    }
	
	sort(vecItem_SpecView.begin(), vecItem_SpecView.end(), compare);

	long nColNum = 8; // 예진 -- 컬럼 넣을 때 수정

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
	m_nCol = nColNum;
	m_nRow = nRawNum;

	std::vector<CString> vecTitle;
	/*0*/vecTitle.push_back(_T("         ITEM         "));
	/*1*/vecTitle.push_back(_T("SIZE X(mm)"));
	/*2*/vecTitle.push_back(_T(">,<   "));
	/*3*/vecTitle.push_back(_T("SIZE Y(mm)"));
	/*4*/vecTitle.push_back(_T(">,<   "));
	/*5*/vecTitle.push_back(_T("OR/AND"));
	/*6*/vecTitle.push_back(_T("Label Attach"));
	/*7*/vecTitle.push_back(_T("Continuous"));

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
        if (vecItem_SpecView.size() > row)
		{
            m_ctrGrid.SetItemText(row + 1, 0, GetDefectName(vecItem_SpecView[row].first));
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
                str.Format(_T("%.2f"), vecItem_SpecView[row].second->at(0).SIZE_X_mm.GetValue());
                m_ctrGrid.SetItemText(row + 1, col, str);
				//str.Format(_T("%.1f"), vecItem[row].second->SIZE_X_mm.GetValue());
				//m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 2) // SIZE_X CO
			{
                str.Format(_T("%s"), enumtoString().CompareOperator2String((enCompareOperator)vecItem_SpecView[row].second->at(0).SIZEX_COMPARE_OPERATOR.GetValue()));
                m_ctrGrid.SetItemText(row + 1, col, str);

				/*str.Format(_T("%d"), vecItem[row].second->SIZEX_COMPARE_OPERATOR.GetValue());
				if (str == _T("1"))
					str = _T("(크)<");
				else if (str == _T("2"))
					str = _T("(작)>");
				else
					str = _T("");
				m_ctrGrid.SetItemText(row + 1, col, str);*/
			}
			else if (col == 3) // SIZE_Y
			{
                str.Format(_T("%.2f"), vecItem_SpecView[row].second->at(0).SIZE_Y_mm.GetValue());
                m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 4) // SIZE_Y CO
			{
				str.Format(_T("%s"), enumtoString().CompareOperator2String((enCompareOperator)vecItem_SpecView[row].second->at(0).SIZEY_COMPARE_OPERATOR.GetValue()));
                m_ctrGrid.SetItemText(row + 1, col, str);
			}
			else if (col == 5) // OR/AND
			{
                str.Format(_T("%s"), enumtoString().OrAnd2String(vecItem_SpecView[row].second->at(0).ORAND.GetValue()));
                m_ctrGrid.SetItemText(row + 1, col, str);
                //str.Format(_T("%d"), vecItem_SpecView[row].second->at(0).ORAND.GetValue());
				/*str.Format(_T("%d"), vecItem[row].second->ORAND.GetValue());
				if (str == _T("1"))
					str = _T("AND");
				else
					str = _T("OR");*/
				
			}
		}
	}

	m_nRow = nRawNum;
    m_nCol = nColNum;
    

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgNormalSpecView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (MESSAGE_GRID == wParam)
	{
		//grid event
		NM_GRIDVIEW* nmgv = (NM_GRIDVIEW*)lParam;

		int nCol = nmgv->iColumn;
		int nRaw = nmgv->iRow;

		nRaw = nRaw - 1;

		switch (nmgv->hdr.code)
		{
		case GVN_SELCHANGING:
			break;

		case GVN_SELCHANGED:
			break;

		case NM_DBLCLK:
		{
            ModifySpecTable();
			NextOrAnd(nRaw, nCol);
			NextCompareOperator(nRaw, nCol);
			EditGridText(nRaw, nCol);
			m_ctrGrid.Invalidate();
			ShowTable();
		}
		break;

		case NM_CLICK:
			/*TogeterValue(nRaw, nCol);*/
			m_ctrGrid.Invalidate();
			break;

		case NM_RCLICK:
			break;

		case GVN_BEGINLABELEDIT:
			break;

		case GVN_ENDLABELEDIT:
			break;

		default:
			break;
		}
	}
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}

void CDlgNormalSpecView::NextOrAnd(int nRow, int nCol)
{
	if (nCol == 5)
	{
		if (vecItem_SpecView.size() > nRow)
		{
            bool bOrAnd = vecItem_SpecView[nRow].second->at(0).ORAND.GetValue();
            vecItem_SpecView[nRow].second->at(0).ORAND = !bOrAnd;
		}
	}
}

void CDlgNormalSpecView::NextCompareOperator(int nRow, int nCol)
{
	BOOL bUse = false;
    if (vecItem_SpecView.size() <= nRow)
	{
		ASSERT(1);
		return;
	}
	Base::ParaLong* pValue = nullptr;

	if (nCol == 2)
	{
        pValue = &vecItem_SpecView[nRow].second->at(0).SIZEX_COMPARE_OPERATOR;
		bUse = true;
	}
	else if (nCol == 4)
	{
        pValue = &vecItem_SpecView[nRow].second->at(0).SIZEY_COMPARE_OPERATOR;
		bUse = true;
	}

	if (nullptr != pValue && true == (bool)bUse)
	{
		*pValue = (pValue->GetValue() + 1) % enCompareOperator::CO_END;
	}

}

void CDlgNormalSpecView::EditGridText(int nRow, int nCol)
{
    if (vecItem_SpecView.size() <= nRow)
	{
		ASSERT(1);
		return;
	}
	BOOL bUse = false;
	Base::ParaFloat* pValue = nullptr;

	if (nCol == 1)
	{
        pValue = &vecItem_SpecView[nRow].second->at(0).SIZE_X_mm;
		bUse = true;
	}
	else if (nCol == 3)
	{
        pValue = &vecItem_SpecView[nRow].second->at(0).SIZE_Y_mm;
		bUse = true;
	}

	if (nullptr != pValue && true == (bool)bUse)
	{
        CDlgEditGrid dlg(GetDefectName(vecItem_SpecView[nRow].first), Numeral2String(_T("%.2f"), pValue->GetValue()),
            GetCellPoint(nRow, nCol), this);
		if (dlg.DoModal() == IDOK)
		{
			*pValue = (float)_wtoi(dlg.m_strValue);
			auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
            isi_spec.SetValueTogether(vecItem_SpecView[nRow].first, (enColTitle)nCol, (float)_wtoi(dlg.m_strValue));
		}
	}
}

CPoint CDlgNormalSpecView::GetCellPoint(int nRow, int nCol)
{
	int nScrollPosX = m_ctrGrid.GetScrollPos(0);

	CRect roi;

	long nWidth = 0;
	for (int c = 0; c < nCol; c++)
	{
		m_ctrGrid.GetCellRect(nRow, c, LPRECT(roi));

		nWidth += roi.Width();
	}

	long nHeight = 0;
	for (int h = 0; h < nRow; h++)
	{
		m_ctrGrid.GetCellRect(h, 0, LPRECT(roi));
		nHeight += roi.Height();
	}

	CRect gridrect;
	m_ctrGrid.GetWindowRect(gridrect);

	CPoint cp(nWidth - nScrollPosX, nHeight + m_rtGrid.top);
	cp.x += gridrect.left;
	return cp;
}

void CDlgNormalSpecView::ShowTable()
{
	for (int row = 0; row < m_nRow; row++)
    {
        if (vecItem_SpecView.size() > row)
        {
            m_ctrGrid.SetItemText(row + 1, 0, GetDefectName(vecItem_SpecView[row].first));
        }
    }

    for (int row = 0; row < m_nRow; row++)
    {
        for (int col = 0; col < m_nCol; col++)
        {
            CString str;
            if (col == 1) // SIZE_X
            {
                str.Format(_T("%.2f"), vecItem_SpecView[row].second->at(0).SIZE_X_mm.GetValue());
                m_ctrGrid.SetItemText(row + 1, col, str);
                //str.Format(_T("%.1f"), vecItem[row].second->SIZE_X_mm.GetValue());
                //m_ctrGrid.SetItemText(row + 1, col, str);
            }
            else if (col == 2) // SIZE_X CO
            {
                str.Format(_T("%s"),
                    enumtoString().CompareOperator2String(
                        (enCompareOperator)vecItem_SpecView[row].second->at(0).SIZEX_COMPARE_OPERATOR.GetValue()));
                m_ctrGrid.SetItemText(row + 1, col, str);

                /*str.Format(_T("%d"), vecItem[row].second->SIZEX_COMPARE_OPERATOR.GetValue());
				if (str == _T("1"))
					str = _T("(크)<");
				else if (str == _T("2"))
					str = _T("(작)>");
				else
					str = _T("");
				m_ctrGrid.SetItemText(row + 1, col, str);*/
            }
            else if (col == 3) // SIZE_Y
            {
                str.Format(_T("%.2f"), vecItem_SpecView[row].second->at(0).SIZE_Y_mm.GetValue());
                m_ctrGrid.SetItemText(row + 1, col, str);
            }
            else if (col == 4) // SIZE_Y CO
            {
                str.Format(_T("%s"),
                    enumtoString().CompareOperator2String(
                        (enCompareOperator)vecItem_SpecView[row].second->at(0).SIZEY_COMPARE_OPERATOR.GetValue()));
                m_ctrGrid.SetItemText(row + 1, col, str);
            }
            else if (col == 5) // OR/AND
            {
                str.Format(_T("%s"), enumtoString().OrAnd2String(vecItem_SpecView[row].second->at(0).ORAND.GetValue()));
                m_ctrGrid.SetItemText(row + 1, col, str);
                //str.Format(_T("%d"), vecItem_SpecView[row].second->at(0).ORAND.GetValue());
                /*str.Format(_T("%d"), vecItem[row].second->ORAND.GetValue());
				if (str == _T("1"))
					str = _T("AND");
				else
					str = _T("OR");*/
            }
        }
    }

	//for (int row = 0; row < m_nRow; row++)
	//{
	//	for (int col = 0; col < m_nCol; col++)
	//	{
	//		CString str;
	//		if (col == 1) // SIZE_X
	//		{
	//			str.Format(_T("%.1f"), vecItem[row].second->SIZE_X_mm.GetValue());
	//			m_ctrGrid.SetItemText(row + 1, col, str);
	//		}
	//		else if (col == 2) // SIZE_X CO
	//		{
	//			str.Format(_T("%d"), vecItem[row].second->SIZEX_COMPARE_OPERATOR.GetValue());
	//			if (str == _T("1"))
	//				str = _T("(크)<");
	//			else if (str == _T("2"))
	//				str = _T("(작)>");
	//			else
	//				str = _T("");
	//			m_ctrGrid.SetItemText(row + 1, col, str);
	//		}
	//		else if (col == 3) // SIZE_Y
	//		{
	//			str.Format(_T("%.1f"), vecItem[row].second->SIZE_Y_mm.GetValue());
	//			m_ctrGrid.SetItemText(row + 1, col, str);
	//		}
	//		else if (col == 4) // SIZE_Y CO
	//		{
	//			str.Format(_T("%d"), vecItem[row].second->SIZEY_COMPARE_OPERATOR.GetValue());
	//			if (str == _T("1"))
	//				str = _T("(크)<");
	//			else if (str == _T("2"))
	//				str = _T("(작)>");
	//			else
	//				str = _T("");
	//			m_ctrGrid.SetItemText(row + 1, col, str);
	//		}
	//		else if (col == 5) // OR/AND
	//		{
	//			str.Format(_T("%d"), vecItem[row].second->ORAND.GetValue());
	//			if (str == _T("1"))
	//				str = _T("AND");
	//			else
	//				str = _T("OR");
	//			m_ctrGrid.SetItemText(row + 1, col, str);
	//		}
	//	}
	//}
}

void CDlgNormalSpecView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	if (nState == WA_CLICKACTIVE)
		ShowTable();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDlgNormalSpecView::ModifySpecTable()
{
    auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;

    vecItem_SpecView.clear();

    for (int n = 0; n < (long)DefectType::ISI_END; n++)
    {
        std::vector<CISISpecInfo>* spec = isi_spec.GetSpecInfo_Multi((DefectType)n);
        if (nullptr != spec)
        {
            vecItem_SpecView.push_back(std::make_pair((DefectType)n, spec));
        }
    }
    sort(vecItem_SpecView.begin(), vecItem_SpecView.end(), compare);

	ShowTable();
}