#pragma once
#include "CDlgEdgeImageView.h"
#include "CTabCtrlFixable.h"

// CDlgLogDisplay 대화 상자

class CDlgLogDisplay : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogDisplay)

public:
	CDlgLogDisplay(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLogDisplay();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LOGDISPLAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    CTabCtrlFixable* m_tab1;
    CDlgEdgeImageView* m_pDlgEdgeView;
    CEdit** m_edits[18];
    CEdit* m_edit1;
    CEdit* m_edit2;
    CEdit* m_edit3;
    CEdit* m_edit4;
    CEdit* m_edit5;
    CEdit* m_edit6;
    CEdit* m_edit7;
    CEdit* m_edit8;
    CEdit* m_edit9;
    CEdit* m_edit10;
    CEdit* m_edit11;
    CEdit* m_edit12;
    CEdit* m_edit13;
    CEdit* m_edit14;
    CEdit* m_edit15;
    CEdit* m_edit16;
    CEdit* m_edit17;
    CEdit* m_edit18;
    BOOL isEdgeViewExist = false;

    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void GetEdgeView(CDlgEdgeImageView* _pDlgEdgeView);
};
