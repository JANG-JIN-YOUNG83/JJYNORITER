#pragma once


// CDlgModify 대화 상자

class CDlgModify : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgModify)

public:
	CDlgModify(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgModify();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MODIFY };
#endif

 private:
    CString m_szNewPassword;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

	CString m_szCurPassword;
	CString GetNewPassword(){ return m_szNewPassword; }
	CStatic m_staticCurPW;
	CStatic m_staticNewPW;
	CStatic m_staticConfirm;
	CButton m_btnModifyOK;
	CButton m_btnModifyCancel;
    afx_msg void OnBnClickedButtonOk();
    afx_msg void OnBnClickedButtonReturn();
};
