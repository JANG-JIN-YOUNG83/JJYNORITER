// CDlgLogin.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "IBCIDlg.h"
#include "CDlgLogin.h"
#include "afxdialogex.h"
#include "CDlgUserCreateAccount.h"
#include "CDlgModify.h"
#include "..\SharedModule\VisionHostShare\LoginInfo.h"
// CDlgLogin 대화 상자

IMPLEMENT_DYNAMIC(CDlgLogin, CDialogEx)

CDlgLogin::CDlgLogin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{
    m_pParent = (CIBCIDlg*)pParent;
}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LOGINUSER, m_list_User);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialogEx)

ON_BN_CLICKED(IDC_BUTTON_CREATE, &CDlgLogin::OnBnClickedButtonCreate)
ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CDlgLogin::OnBnClickedButtonLogin)
ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgLogin::OnBnClickedButtonDelete)
ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgLogin::OnHdnItemclickListLoginuser)
ON_NOTIFY(NM_CLICK, IDC_LIST_LOGINUSER, &CDlgLogin::OnNMClickListLoginuser)
ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CDlgLogin::OnBnClickedButtonModify)
END_MESSAGE_MAP()


// CDlgLogin 메시지 처리기



BOOL CDlgLogin::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    HFONT hNewFont;

    hNewFont = CreateFont(15, 0, 0, 0, 500, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, _T("휴먼둥근헤드라인"));
    m_list_User.SendMessage(WM_SETFONT, (WPARAM)hNewFont, (LPARAM)TRUE);

    //현재 User 정보 Load 후, List 에 뿌려주자눙
    LoadUserInfo();

    // 만약 유저가 아무것도 없다면 Intekplus 관리자 하나 넣기.
    if (m_vecAccounts.size() == 0)
    {
        LOGINUSERINFO admin_Account;
        admin_Account.strUserID = _T("Intekplus");
        admin_Account.strUserPW = _T("1");
        admin_Account.nUserMode = 0;

        m_vecAccounts.push_back(admin_Account);
    }

    // 관리자 Intekplus 초기화 예외처리
    BOOL bRet = FALSE;

    for (long nUser = 0; nUser < m_vecAccounts.size(); nUser++)
    {
        if (m_vecAccounts[nUser].strUserID == _T("Intekplus"))
            bRet = TRUE;
    }
    if (bRet == FALSE)
    {
        m_vecAccounts.clear();

        LOGINUSERINFO admin_Account;
        admin_Account.strUserID = _T("Intekplus");
        admin_Account.strUserPW = _T("1");
        admin_Account.nUserMode = 0;

        m_vecAccounts.push_back(admin_Account);
        LoadUserInfo();
    }

    InitListUserInfo();
    
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgLogin::OnBnClickedButtonCreate()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDlgUserCreateAccount CreateDlg;

    if(CreateDlg.DoModal() == IDOK)
    {
        LOGINUSERINFO LoginInfo;

        LoginInfo.strUserID = CreateDlg.GetID();
        LoginInfo.strUserPW = CreateDlg.GetPassword();
        LoginInfo.nUserMode = CreateDlg.GetMode();

        m_vecAccounts.push_back(LoginInfo);

        SaveUserInfo();

		//LOGINUSERINFO admin_Account;
  //      admin_Account.strUserID = _T("admin");
  //      admin_Account.strUserPW = _T("admin");
  //      admin_Account.nUserMode = 1.0;

       // m_vecAccounts.push_back(admin_Account);
        m_vecAccounts.clear();
        LoadUserInfo();


        InitListUserInfo();
    }
}

BOOL CDlgLogin::LoadUserInfo()
{
    CStdioFile file;
    BOOL res = file.Open(_T("C:\\Intekplus\\IBCI\\Accounts.ini"), CFile::modeRead);

    if (res)
    {
        LOGINUSERINFO admin_Account;
        CString strLine;
        int lineIndex = 1;
        while (file.ReadString(strLine))
        {
            int itemIndex = 0;
            CString strItem;

            while (AfxExtractSubString(strItem, strLine, itemIndex, '='))
            {
                if (itemIndex > 0)
                {
                    if (lineIndex % 3 == 1)
                    {
                        admin_Account.strUserID = strItem;
                    }
                    else if (lineIndex % 3 == 2)
                    {
                        admin_Account.strUserPW = strItem;
                    }
                    else if (lineIndex % 3 == 0)
                    {
                        admin_Account.nUserMode = _ttof(strItem);
                        m_vecAccounts.push_back(admin_Account);
                    }
                }
                itemIndex++;
            }

            lineIndex++;
        }

        file.Close();

        return res;
    }
    else
    {
        return res;
    }
}

void CDlgLogin::SaveUserInfo()
{
    CreateDefaultPath();

    CStdioFile file;
    if (file.Open(_T("C:\\Intekplus\\IBCI\\Accounts.ini"), CFile::modeCreate | CFile::modeWrite))
    {
        for (int i = 0; i < m_vecAccounts.size(); i++)
        {
            CString szIDLine, szPasswordLine, szModeLine;
            szIDLine.Format(_T("ID=%s\n"), (LPCTSTR)m_vecAccounts.at(i).strUserID);
            szPasswordLine.Format(_T("PASSWORD=%s\n"), (LPCTSTR)m_vecAccounts.at(i).strUserPW);
            szModeLine.Format(_T("MODE=%d\n"), m_vecAccounts.at(i).nUserMode);
            file.SeekToEnd();
            file.WriteString(szIDLine);
            file.WriteString(szPasswordLine);
            file.WriteString(szModeLine);
        }

        file.Close();
    }
}

void CDlgLogin::CreateDefaultPath()
{
    CFileFind find;
    if (!find.FindFile(_T("C:\\Intekplus")))
    {
        CreateDirectory(_T("C:\\Intekplus"), NULL);
        CreateDirectory(_T("C:\\Intekplus\\IBCI"), NULL);
    }
    else if (!find.FindFile(_T("C:\\Intekplus\\IBCI")))
    {
        CreateDirectory(_T("C:\\Intekplus\\IBCI"), NULL);
    }
}

void CDlgLogin::OnBnClickedButtonLogin()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString szID, szPassword;

    GetDlgItemText(IDC_EDIT_ID, szID);
    GetDlgItemText(IDC_EDIT_PASSWORD, szPassword);

    Share::LoginInfo Login;

    if (TRUE == m_pParent->HostSocketConnectCheck())
    {
        Login.SetID(szID);
        Login.SetPW(szPassword);
       // m_pParnet->m_bLoginSuccess = FALSE;
       // m_pParnet->m_bLoginFail = FALSE;
        //m_pParnet->SendLoginTry2Host(Login);
       // SetTimer(TIMER_LOGIN_RESULT, 1000, NULL);
    }
    else
    {
        long nMode = 0;

        if (((CButton*)GetDlgItem(IDC_RADIO_SELET_MODE3))->GetCheck())
        {
            Login.SetMode(Share::enLoginMode::Operator);
            nMode = 2;
        }
        else if (((CButton*)GetDlgItem(IDC_RADIO_SELET_MODE2))->GetCheck())
        {
            Login.SetMode(Share::enLoginMode::Engineer);
            nMode = 1;
        }
        else if (((CButton*)GetDlgItem(IDC_RADIO_SELET_MODE1))->GetCheck())
        {
            Login.SetMode(Share::enLoginMode::Administrator);
            nMode = 0;
        }

        int count = 0;

        while (m_vecAccounts.at(count).strUserID != szID)
        {
            if (m_vecAccounts.size() - 1 > count)
            {
                count++;
            }
            else
            {
                AfxMessageBox(_T("Don't Exist User ID"));
                count = -1;
                break;
            }
        }

        if (count > -1)
        {
            if (szPassword == m_vecAccounts.at(count).strUserPW)
            {
                if (m_vecAccounts.at(count).nUserMode == nMode)
                {
                    m_nUserMode = nMode;
                    Login.Login();
                    //m_pParent->m_stLoginInfo = Login;
                    //m_pParent->SetLogin();
                    OnOK();
                }
                else
                {
                    AfxMessageBox(_T("Not Correct Grade!"));
                }
            }
            else
            {
                AfxMessageBox(_T("Not Correct Password!"));
            }
        }
    }
}

void CDlgLogin::InitListUserInfo()
{
    m_list_User.DeleteAllItems();

    m_list_User.InsertColumn(0, _T("Num"), NULL, 50, NULL);
    m_list_User.InsertColumn(1, _T("User ID"), NULL, 250, NULL);

    if (m_vecAccounts.size() > 0)
    {
        for (int i = 0; i < m_vecAccounts.size(); i++)
        {
            CString str;
            str.Format(_T("%d"), i);
            m_list_User.InsertItem(i + 1, str);
            m_list_User.SetItemText(i, 1, m_vecAccounts[i].strUserID);
            //if (m_vecAccounts.at(i).nUserMode == 0)
        }
    }
}

void CDlgLogin::OnBnClickedButtonDelete()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    long nUserIndex = m_nUserInfoSelect;
    CString strID;
    if (nUserIndex > -1)
    {
        strID = m_list_User.GetItemText(nUserIndex, 1);

        if (m_vecAccounts.size() > 0)
        {
            for (int i = 0; i < m_vecAccounts.size(); i++)
            {
                if (m_vecAccounts.at(i).strUserID == strID)
                    m_vecAccounts.erase(m_vecAccounts.begin() + i);
            }

            SaveUserInfo();

            m_list_User.DeleteAllItems();
            m_vecAccounts.clear();

            LOGINUSERINFO admin_Account;
            admin_Account.strUserID = _T("Intekplus");
            admin_Account.strUserPW = _T("1");
            admin_Account.nUserMode = 0;

            m_vecAccounts.push_back(admin_Account);

            LoadUserInfo();
            InitListUserInfo();


            //ReadAccountsInfo();

            //((CButton*)(GetDlgItem(IDC_RADIO_USER)))->SetCheck(TRUE);

            //OnBnClickedRadioUser();
        }
    }
}

void CDlgLogin::OnHdnItemclickListLoginuser(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    //long m_nSelect = phdr->iItem;
    //int nSelectRow = ListRowSelect(IDC_LIST_LOGINUSER, phdr);

    *pResult = 0;
}

int CDlgLogin::ListRowSelect(int nID, LPNMITEMACTIVATE pNMItemActivate)
{
    CPoint ptClick = pNMItemActivate->ptAction;
    CListCtrl* pListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(nID));
    LVHITTESTINFO listinfo;
    listinfo.pt = ptClick;

    return pListCtrl->SubItemHitTest(&listinfo); // 행
}

void CDlgLogin::OnNMClickListLoginuser(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    long m_nSelect = pNMItemActivate->iItem;
    m_nUserInfoSelect = ListRowSelect(IDC_LIST_LOGINUSER, pNMItemActivate);

    CString strUserID;
    strUserID = m_list_User.GetItemText(m_nSelect, 1);

    SetDlgItemText(IDC_EDIT_ID, strUserID);
    *pResult = 0;
}

void CDlgLogin::OnBnClickedButtonModify()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int index = m_nUserInfoSelect;
    CString strID;
    if (index > -1)
    {
        strID = m_list_User.GetItemText(index, 1);

        CDlgModify dlg;
        //dlg.m_nLang = m_nLang;
        if (m_vecAccounts.size() > 1)
        {
            for (int i = 0; i < m_vecAccounts.size(); i++)
            {
                if (m_vecAccounts.at(i).strUserID == strID)
                    dlg.m_szCurPassword = m_vecAccounts.at(i).strUserPW;
            }
        }

        if (dlg.m_szCurPassword != _T(""))
        {
            if (dlg.DoModal() == IDOK)
            {
                for (int i = 0; i < m_vecAccounts.size(); i++)
                {
                    if (m_vecAccounts.at(i).strUserID == strID)
                        m_vecAccounts.at(i).strUserPW = dlg.GetNewPassword();
                }

                SaveUserInfo();

                m_list_User.DeleteAllItems();
                //m_list_User.clear();

                //LOGIN_INFO admin_Account;
                //admin_Account.m_szID = _T("admin");
                //admin_Account.m_szPassword = _T("admin");
                //admin_Account.m_fMode = 1.0;

                //m_vecAccounts.push_back(admin_Account);

                LoadUserInfo();

                //((CButton*)(GetDlgItem(IDC_RADIO_USER)))->SetCheck(TRUE);

                InitListUserInfo();
            } 
        }
    }
}
