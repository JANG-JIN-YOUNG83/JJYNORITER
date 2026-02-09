#pragma once


// CDlgEditClassification 대화 상자
class CDlgClassificationCondition;
class CCustomButton;

class CDlgEditClassification : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditClassification)

public:
    CDlgEditClassification(CString strValue, CDlgClassificationCondition* pParent); // 표준 생성자입니다.
	virtual ~CDlgEditClassification();
    CDlgClassificationCondition* m_parent;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDIT_CONDITION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedBtnAddAndCondtion();
    CComboBox m_comboSetDefectType;
    CComboBox m_comboSetCompare;
    CEdit m_editSetValue;
    CEdit m_editCondition;  
    CCustomButton* m_btnAddAndCondition;

    CString m_strTotalCondition;
    CString m_strAddCondition;
    CString m_strDefectType;
    CString m_strCompare;
    CString m_strSpecValue;
    
    afx_msg void OnCbnSelchangeComboSetDefecttype();
    afx_msg void OnCbnSelchangeComboSetCompare();
    afx_msg void OnEnChangeEditSetValue();

    void SetTotalCondtion();
    void IninCombo();
    CListCtrl m_listCtrl;
    long m_nListCount;
    CCustomButton* m_btnDeleteCondition;
    afx_msg void OnBnClickedBtnDeleteCondition();
    afx_msg void OnNMClickListTotalCondition(NMHDR* pNMHDR, LRESULT* pResult);
    long m_nSelectIndex;
    CCustomButton* m_btnApply;
    afx_msg void OnBnClickedBtnApply();
    afx_msg void OnBnClickedBtnTransCondition();
    CCustomButton* m_btnTransCondition;
    void initList();
    BOOL StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue);
    CString m_strSetTotal;
    std::vector<CString> m_vecStrDivCondition;
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    CRect m_nDlgSize;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnEnKillfocusEditSetValue();
    void CurSelectComboSet(long nCursel);

    CString CommonDefectConditionSet();
};
