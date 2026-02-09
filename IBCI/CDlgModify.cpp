// CDlgModify.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CDlgModify.h"
#include "afxdialogex.h"


// CDlgModify 대화 상자

IMPLEMENT_DYNAMIC(CDlgModify, CDialogEx)

CDlgModify::CDlgModify(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MODIFY, pParent)
{

}

CDlgModify::~CDlgModify()
{
}

void CDlgModify::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgModify, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgModify::OnBnClickedButtonOk)
ON_BN_CLICKED(IDC_BUTTON_RETURN, &CDlgModify::OnBnClickedButtonReturn)
END_MESSAGE_MAP()


// CDlgModify 메시지 처리기

BOOL CDlgModify::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.

    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgModify::OnBnClickedButtonOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString szCurPassword, szNewPasswordConfirm;

    GetDlgItemText(IDC_EDIT_CURPASSWORD, szCurPassword);
    GetDlgItemText(IDC_EDIT_NEWPASSWORD, m_szNewPassword);
    GetDlgItemText(IDC_EDIT_NEWPASSWORDCONFIRM, szNewPasswordConfirm);

    if (szCurPassword != m_szCurPassword)
    {
        AfxMessageBox(_T("The current password is not correct"));
    }
    else if (m_szNewPassword != szNewPasswordConfirm)
    {
        AfxMessageBox(_T("New password and confirmation password are not same"));
    }
    else
    {
        OnOK();
    }
}

void CDlgModify::OnBnClickedButtonReturn()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    OnCancel();
}
