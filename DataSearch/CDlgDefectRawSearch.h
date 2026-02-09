#pragma once
#include "afxdialogex.h"
class CDlgDataSearch;

// CDlgDefectRawSearch 대화 상자

class CDlgDefectRawSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectRawSearch)

public:
	CDlgDefectRawSearch(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDefectRawSearch();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SEARCH_DEFECTRAW };
#endif
    CDlgDataSearch* m_pParent;
    CRect m_rtDlgSize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void ControlSizeChange(int nID);
    void CALL_Function_CSC();
    void DlgControlSetting();
    CComboBox m_comboElectrodeType;
    CComboBox m_comboCamIndex;
    CComboBox m_comboDefectName;
};
