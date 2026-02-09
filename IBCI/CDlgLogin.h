#pragma once
#include "resource.h"

// CDlgLogin 대화 상자

typedef struct _LOGINUSERINFO
{
    CString strUserID;
    CString strUserPW;
    long nUserMode;
} LOGINUSERINFO;
class CIBCIDlg;
class CDlgLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLogin();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	virtual BOOL OnInitDialog();

	CIBCIDlg* m_pParent;
    std::vector<LOGINUSERINFO> m_vecAccounts;
	void SaveUserInfo();
    BOOL LoadUserInfo();
    afx_msg void OnBnClickedButtonCreate();
    void CreateDefaultPath();
    afx_msg void OnBnClickedButtonLogin();
    long m_nUserMode;
    CListCtrl m_list_User;

    void InitListUserInfo();
    int ListRowSelect(int nID, LPNMITEMACTIVATE pNMItemActivate);
    afx_msg void OnBnClickedButtonDelete();
    afx_msg void OnHdnItemclickListLoginuser(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickListLoginuser(NMHDR* pNMHDR, LRESULT* pResult);

    long m_nUserInfoSelect;
    afx_msg void OnBnClickedButtonModify();
};

