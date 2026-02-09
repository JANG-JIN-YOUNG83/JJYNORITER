#pragma once
#include "InspectionCriteriaList.h"

class CGridCtrl;

// DlgCriteriaList dialog

class DlgCriteriaList : public CDialogEx
{
	DECLARE_DYNAMIC(DlgCriteriaList)

public:
	DlgCriteriaList(const Inspection::InspectionCriteriaList& info, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgCriteriaList();

	Inspection::InspectionCriteriaList m_info;

// Dialog Data
	enum { IDD = IDD_DIALOG_CRITERIA_LIST };

protected:
	CGridCtrl* m_gridCtrl;

	Inspection::InspectionCriteria* GetCriteria(long index);
	LPCTSTR GetCriteriaName(long index);
	long CalcCriteriaCount();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
