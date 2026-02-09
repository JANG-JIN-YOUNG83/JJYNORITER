#pragma once
#include "DlgAjinDIO.h"
#include "Ajin/DlgAjinMot.h"
#include "IBCIDlg.h"
    // DlgHWControl 대화 상자

class DlgHWControl : public CDialogEx
{
	DECLARE_DYNAMIC(DlgHWControl)

public:
	DlgHWControl(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~DlgHWControl();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HWCONTROL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    CTabCtrl m_Tab_HWControl;
    afx_msg void OnSelchangeTabHwControl(NMHDR* pNMHDR, LRESULT* pResult);
    CIBCIDlg* dlgIBCI;
    DlgAjinMot* dlgAjinMot;
    DlgAjinDIO* dlgAjinDIO;
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
};
