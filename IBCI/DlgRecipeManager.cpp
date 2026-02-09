// DlgRecipeManager.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgRecipeManager.h"
#include "afxdialogex.h"
#include "IBCIDlg.h"


// CDlgRecipeManager 대화 상자

IMPLEMENT_DYNAMIC(CDlgRecipeManager, CDialogEx)

CDlgRecipeManager::CDlgRecipeManager(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RECIPE_MANAGER, pParent)
{
	m_pParent = (CIBCIDlg*)pParent;
}

CDlgRecipeManager::~CDlgRecipeManager()
{
}

void CDlgRecipeManager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRecipeManager, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_RECIPE_LOAD, &CDlgRecipeManager::OnBnClickedButtonRecipeLoad)
	ON_BN_CLICKED(IDC_BUTTON_RECIPE_SAVE, &CDlgRecipeManager::OnBnClickedButtonRecipeSave)
END_MESSAGE_MAP()


// CDlgRecipeManager 메시지 처리기


void CDlgRecipeManager::OnBnClickedButtonRecipeLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strRes;
	if (m_pParent->RecipeLoad())
	{
		strRes.Format(_T("Recipe Load Success"));
	}
	else
	{
		strRes.Format(_T("Recipe Load Failed"));
	}

	AfxMessageBox(strRes);
	this->ShowWindow(SW_HIDE);
}


void CDlgRecipeManager::OnBnClickedButtonRecipeSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strRes;
	if (m_pParent->RecipeSave())
	{
		strRes.Format(_T("Recipe Save Success"));
	}
	else
	{
		strRes.Format(_T("Recipe Save Failed"));
	}

	AfxMessageBox(strRes);
	this->ShowWindow(SW_HIDE);
}


