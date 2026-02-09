#pragma once
#include "basedef.h"
#include "CDlgClassificationST.h"
#include "CISICellAllDefect.h"


// CDlgInspectionArea 대화 상자

class CDlgInspectionArea : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInspectionArea)

public:
	CDlgInspectionArea(CWnd* pParent, CISICellAllDefect* pDefectInfo, long blobConditionSize, std::vector<long> vecAreaSet);   // 표준 생성자입니다.
	virtual ~CDlgInspectionArea();
	CDlgClassificationST* m_pDlgClassifcationST;
	CISICellAllDefect& m_DefectInfo;

	

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INSPECTION_AREA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	long m_ncolSize;
	long m_nrowSize;
	std::vector<long> m_vecInspAreaSet;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	CListCtrl m_ctrList_area;

	int getInspectionArea(long nAreaIndex)
	{
		//int numInspectionArea;// m_DefectInfo.m_vecCandidate[nAreaIndex].nInspectionArea;
		return m_vecInspAreaSet[nAreaIndex];
	}

	void setInspectionArea(long nCol, long nRow)
	{
		//DefectInfo.m_vecCandidate[nCol].nInspectionArea = nRow;
        if (nCol > -1)
			m_vecInspAreaSet[nCol]= nRow;
	}

	int checkRowItem(int nCol, int nRow)
	{
		for (int row = 0; row < nRow; row++)
		{
			CString getStr = m_ctrList_area.GetItemText(row, nCol);
			if (getStr == _T("True"))
			{
				return row;
			}
		}
		return 0;
	}

	afx_msg void OnNMClickListInspectionArea(NMHDR* pNMHDR, LRESULT* pResult);
};
