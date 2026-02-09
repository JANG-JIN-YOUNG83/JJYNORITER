#pragma once


// CDlgRecipeManager 대화 상자
class CIBCIDlg;

class CDlgRecipeManager : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRecipeManager)

public:
	CDlgRecipeManager(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgRecipeManager();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RECIPE_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CIBCIDlg* m_pParent;
	afx_msg void OnBnClickedButtonRecipeLoad();
	afx_msg void OnBnClickedButtonRecipeSave();
};
