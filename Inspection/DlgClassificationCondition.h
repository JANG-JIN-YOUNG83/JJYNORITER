#pragma once


// CDlgClassificationCondition 대화 상자
class CDlgClassificationST;
class CCustomButton;

class CDlgClassificationCondition : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgClassificationCondition)

public:
    CDlgClassificationCondition(CWnd* pParent, DefectType DefectName); // 표준 생성자입니다.
	virtual ~CDlgClassificationCondition();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CLASSIFICATION_CONDITION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    CDlgClassificationST* m_pParent;
    DefectType m_defectName;
    virtual BOOL OnInitDialog();
    CString m_strValue;
    std::vector<CString> m_vecStrValue;
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();
    void updateDefectValue();
    afx_msg void OnClose();
    afx_msg void OnDestroy();

    BOOL StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue);
    

    afx_msg void OnBnClickedBtnEdit1();

    CXTPPropertyGrid* m_inspectionItemGrid;
    void InitGrid();
    afx_msg LRESULT OnGridNotify(WPARAM wparam, LPARAM lparam);
    std::vector<CString> m_vecStrCondition;
    long m_nIndex;
    void OnBnClickedSetting(LPARAM lparam, const int& nID);
    CCustomButton* m_btnAddOrCondition;
    afx_msg void OnBnClickedBtnAddOrCondition();
    CCustomButton* m_btnDeleteOrCondition;
    afx_msg void OnBnClickedBtnDeleteOrCondition();
    void updateGrid();
    CCustomButton* m_btnApply;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CRect m_nDlgSize;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    CEdit m_editTotal;
};
