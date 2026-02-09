#pragma once
#include "basedef.h"
#include "../Module/CustomControl/gridctrl.h"
#include "../Inspection/CISISpecInfo.h"
#include "../Inspection/CISISpecTable.h"

// CDlgNormalSpecView 대화 상자

class CDlgNormalSpecView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNormalSpecView)

public:
	CDlgNormalSpecView(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgNormalSpecView();

	//GRID 관련한 내용
	CGridCtrl m_ctrGrid;
	std::vector<CString> vecTitle;
	std::vector<int> m_vecnSizeCol;
	int m_nSizeRow;
	long m_nRow;
	long m_nCol;
	//std::vector<std::pair<DefectType, CISISpecInfo*>> vecItem;
    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>*>> vecItem_SpecView;

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SPEC_VIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CRect m_dlgSize;

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void NextOrAnd(int nRow, int nCol);
	void NextCompareOperator(int nRow, int nCol);
	void EditGridText(int nRaw, int nCol);
	void ShowTable();
    void ModifySpecTable();
	
	template <typename T>
	CString Numeral2String(CString type, T value)
	{
		CString strValue;
		strValue.Format(type, value);
		return strValue;
	}
	CRect m_rtGrid;
	CPoint GetCellPoint(int nRow, int nCol);

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
