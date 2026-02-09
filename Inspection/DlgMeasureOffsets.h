#pragma once
#include "MeasureOffsets.h"

// DlgMeasureOffsets dialog

class DlgMeasureOffsets : public CDialogEx
{
	DECLARE_DYNAMIC(DlgMeasureOffsets)

public:
	DlgMeasureOffsets(const Inspection::MeasureOffsets& measureOffset, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgMeasureOffsets();

	Inspection::MeasureOffsets m_measureOffset;

// Dialog Data
	enum { IDD = IDD_DIALOG_MEASURE_OFFSETS };

protected:
	CXTPPropertyGrid m_property;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
