#pragma once
#include "../Module/CustomControl/gridctrl.h"
#include "CISICellAllDefect.h"

// CDlgDefectGrid 대화 상자

class CDlgClassificationST;
class CDlgDefectGrid : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectGrid)

public:
	CDlgDefectGrid(CWnd* pParent, CISICellAllDefect* pDefectInfo);   // 표준 생성자입니다.
	virtual ~CDlgDefectGrid();

	long m_nSelectDefectIndex;
	CGridCtrl m_ctrDefectGrid;
	

	CISICellAllDefect &m_DefectInfo;

	void MoveWindows();
	
	void DisplayDefectData(GV_DISPINFO* pDisplay, long nrow, long ncor);
	BOOL CallBackDefectGrid(GV_DISPINFO* pDisplay);
	void initDefectGrid();
	void SelectDefectIndex(long nIndex);

	void ReFresh();

	std::vector<CString> vecDefectTitle;
	CDlgClassificationST *m_pDlgClassifcationST;
	
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DEFECT_GRID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
