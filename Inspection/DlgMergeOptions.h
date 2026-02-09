#pragma once
#include "basedef.h"

// DlgMergeOptions dialog

class DlgMergeOptions : public CDialogEx
{
	DECLARE_DYNAMIC(DlgMergeOptions)

public:
	DlgMergeOptions(const Inspection::InspMergeOptions& option, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgMergeOptions();

// Dialog Data
	enum { IDD = IDD_DIALOG_MERGEOPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_length;
	int m_topTilt;
	int m_bottomTilt;
	int m_misalign;
};
