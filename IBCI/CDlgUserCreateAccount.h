#pragma once


// CDlgUserCreateAccount 대화 상자

class CDlgUserCreateAccount : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgUserCreateAccount)

public:
	CDlgUserCreateAccount(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgUserCreateAccount();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CREATEACCOUNT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
    CString m_szID;
    CString m_szPassword;
    long m_nMode;

public:

	CString GetID(){ return m_szID; }
	CString GetPassword(){ return m_szPassword; }
	long GetMode(){ return m_nMode; }
    afx_msg void OnBnClickedButtonOk();
    afx_msg void OnBnCheckRadio(UINT id);

	
};
