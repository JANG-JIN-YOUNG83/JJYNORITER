#pragma once
#include "basedef.h"
#include "../Module/CustomControl/gridctrl.h"
#include "../Inspection/CISISpecInfo.h"
#include "../Inspection/CISISpecTable.h"


// CDlgSmallSpecView 대화 상자

class CDlgSmallSpecView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSmallSpecView)

public:
	CDlgSmallSpecView(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSmallSpecView();
	//GRID 관련한 내용
	CGridCtrl m_ctrGrid;
	std::vector<CString> vecTitle;
	int m_nSizeRow;
	std::vector<std::pair<DefectType, CISISpecInfo*>> vecItem;


	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SMALL_SPEC_VIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CRect m_dlgSize;
};
