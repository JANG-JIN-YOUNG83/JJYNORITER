// CDlgUserCreateAccount.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CDlgUserCreateAccount.h"
#include "afxdialogex.h"


// CDlgUserCreateAccount 대화 상자

IMPLEMENT_DYNAMIC(CDlgUserCreateAccount, CDialogEx)

CDlgUserCreateAccount::CDlgUserCreateAccount(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CREATEACCOUNT, pParent)
{
    m_nMode = -1;
}

CDlgUserCreateAccount::~CDlgUserCreateAccount()
{
}

void CDlgUserCreateAccount::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgUserCreateAccount, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgUserCreateAccount::OnBnClickedButtonOk)
ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_CREATEMASTER, IDC_RADIO_CREATEOPERATOR, &CDlgUserCreateAccount::OnBnCheckRadio)
END_MESSAGE_MAP()


// CDlgUserCreateAccount 메시지 처리기



void CDlgUserCreateAccount::OnBnClickedButtonOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString szPasswordRe;

	GetDlgItemText(IDC_EDIT_CREATEUSERID, m_szID);
    GetDlgItemText(IDC_EDIT_CREATEUSERPASSWORD, m_szPassword);
    GetDlgItemText(IDC_EDIT_CREATEPASSWORDCONFIRM, szPasswordRe);

    if (m_szID == _T("") || m_szPassword == _T("") || szPasswordRe == _T(""))
    {
        AfxMessageBox(_T("Check ID Or P/W or Confirm"));
        return;
    }
    if (m_nMode == -1)
    {
        AfxMessageBox(_T("Check User Mode Select!"));
        return;
    }

    OnOK();
    //if (m_szPassword != szPasswordRe)
    //{
    //    AfxMessageBox(_T("비밀번호가 일치하지 않습니다."));
    //    return;
    //}

}

afx_msg void CDlgUserCreateAccount::OnBnCheckRadio(UINT id)
{
    switch (id)
    {
        case IDC_RADIO_CREATEMASTER:
            m_nMode = 0;
            // do something...
            break;
        case IDC_RADIO_CREATEENGINEER:
            m_nMode = 1;
            // do something...
            break;
        case IDC_RADIO_CREATEOPERATOR:
            m_nMode = 2;
            // do something...
            break;
        default:
            m_nMode = -1;
            break;
    }
}