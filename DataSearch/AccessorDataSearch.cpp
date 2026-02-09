#include "pch.h"
#include "AccessorDataSearch.h"

#include "DlgDataSearch.h"
#include "resource.h"

AccessorDataSearch* AccessorDataSearch::m_instance = nullptr;

bool AccessorDataSearch::ShowDlg()
{
    // 이 코드가 없으면 Dialog 가 보이지 않음
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CDlgDataSearch* pDlg;
    pDlg = new CDlgDataSearch();
    pDlg->Create(IDD_DIALOG_DATA_SEARCH);
    pDlg->m_strSystemCSVPath = m_strFilePath;
    pDlg->ShowWindow(SW_SHOW);

    return true;
}

void AccessorDataSearch::GetSystemFilePath(CString strPath)
{
    m_strFilePath = strPath;
}

void AccessorDataSearch::GetSystemTopBack(CString strPath)
{
    m_strTopBack = strPath;
}