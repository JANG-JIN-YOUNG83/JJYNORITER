// DlgCriteriaList.cpp : implementation file
//

#include "stdafx.h"
#include "Inspection.h"
#include "DlgCriteriaList.h"
#include "afxdialogex.h"

#include "../Module/CustomControl/GridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDC_GRID_INFO						1001

// DlgCriteriaList dialog

IMPLEMENT_DYNAMIC(DlgCriteriaList, CDialogEx)

DlgCriteriaList::DlgCriteriaList(const Inspection::InspectionCriteriaList& info, CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgCriteriaList::IDD, pParent)
	, m_gridCtrl(new CGridCtrl)
	, m_info(info)
{
}

DlgCriteriaList::~DlgCriteriaList()
{
	delete m_gridCtrl;
}

void DlgCriteriaList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgCriteriaList, CDialogEx)
END_MESSAGE_MAP()


// DlgCriteriaList message handlers


BOOL DlgCriteriaList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect gridRect;
	GetDlgItem(IDC_GRID)->GetWindowRect(gridRect);
	ScreenToClient(gridRect);

	m_gridCtrl->Create(gridRect, this, IDC_GRID_INFO);
	m_gridCtrl->EnableDragAndDrop(FALSE);

	long criteriaCount = CalcCriteriaCount();

	m_gridCtrl->SetColumnCount(4);
	m_gridCtrl->SetRowCount(criteriaCount + 1);
	m_gridCtrl->SetFixedRowCount(1);

	m_gridCtrl->SetItemText(0, 0, _T("Name"));			m_gridCtrl->SetColumnWidth(0, 200);
	m_gridCtrl->SetItemText(0, 1, _T("Standard(mm)"));	m_gridCtrl->SetColumnWidth(1, 100);
	m_gridCtrl->SetItemText(0, 2, _T("Min(mm)"));		m_gridCtrl->SetColumnWidth(2, 100);
	m_gridCtrl->SetItemText(0, 3, _T("Max(mm)"));		m_gridCtrl->SetColumnWidth(3, 100);
	
	for (long index = 0; index < criteriaCount; index++)
	{
		auto* criteria = GetCriteria(index);
		m_gridCtrl->SetItemText(index + 1, 0, GetCriteriaName(index));
		m_gridCtrl->SetItemState(index + 1, 0, GVIS_READONLY);

		if (criteria->m_useStandardValue)
		{
			CString standard;
			standard.Format(_T("%.5f"), criteria->m_standardValue_mm);
			m_gridCtrl->SetItemText(index + 1, 1, standard);
		}
		else
		{
			m_gridCtrl->SetItemBkColour(index + 1, 1, RGB(128, 128, 128));
			m_gridCtrl->SetItemState(index + 1, 1, GVIS_READONLY);
		}

		CString valueMin;
		CString valueMax;
		valueMin.Format(_T("%.5f"), criteria->m_specMin_mm);
		valueMax.Format(_T("%.5f"), criteria->m_specMax_mm);

		m_gridCtrl->SetItemText(index + 1, 2, valueMin);
		m_gridCtrl->SetItemText(index + 1, 3, valueMax);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

Inspection::InspectionCriteria* DlgCriteriaList::GetCriteria(long index)
{
	switch (index)
	{
	case  0:	return &m_info.m_tabPitch;
	case  1:	return &m_info.m_tabWidth;
	case  2:	return &m_info.m_tabHeight;
	case  3:	return &m_info.m_topTabHeight2;
	case  4:	return &m_info.m_bottomTabHeight2;
	case  5:	return &m_info.m_tabSide;
	case  6:	return &m_info.m_mismatch;
	case  7:	return &m_info.m_cellLength;
	case  8:	return &m_info.m_cellTopLength2;
	case  9:	return &m_info.m_cellBottomLength2;
	case  10:	return &m_info.m_shoulderLineTop;
	case  11:	return &m_info.m_shoulderLineBot;
	case  12:	return &m_info.m_vhomeHeight;
	case  13:	return &m_info.m_vhomeTiltTop;
	case 14:	return &m_info.m_vhomeTiltBot;
	case 15:	return &m_info.m_misalign;
	case 16:	return &m_info.m_frontCuttingHeightTop;
	case 17:	return &m_info.m_frontCuttingHeightBot;
	case 18:	return &m_info.m_backCuttingHeightTop;
	case 19:	return &m_info.m_backCuttingHeightBot;
	case 20:	return &m_info.m_frontOverlay;
	case 21:	return &m_info.m_backOverlay;
	case 22:	return &m_info.m_frontInsulation;
	case 23:	return &m_info.m_backInsulation;
	case 24:	return &m_info.m_frontBotOverlay;
	case 25:	return &m_info.m_backBotOverlay;
	}

	return nullptr;
}

LPCTSTR DlgCriteriaList::GetCriteriaName(long index)
{
	switch (index)
	{
	case  0:	return _T("Tab Pitch");
	case  1:	return _T("Tab Width");
	case  2:	return _T("Tab Height");
	case  3:	return _T("Top Tab Height 2");
	case  4:	return _T("Bottom Tab Height 2");
	case  5:	return _T("Tab Side");
	case  6:	return _T("Mismatch");
	case  7:	return _T("Cell Length");
	case  8:	return _T("Cell Top Length 2");
	case  9:	return _T("Cell Bottom Length 2");
	case  10:	return _T("Shoulder Line Top");
	case  11:	return _T("Shoulder Line Bot");
	case  12:	return _T("V Home Height");
	case  13:	return _T("V Home Tilt Top");
	case 14:	return _T("V Home Tilt Bot");
	case 15:	return _T("Misalign");
	case 16:	return _T("Front Cutting Height Top");
	case 17:	return _T("Front Cutting Height Bot");
	case 18:	return _T("Back Cutting Height Top");
	case 19:	return _T("Back Cutting Height Bot");
	case 20:	return _T("Front Overlay");
	case 21:	return _T("Back Overlay");
	case 22:	return _T("Front Insulation");
	case 23:	return _T("Back Insulation");
	case 24:	return _T("Front Overlay Bot");
	case 25:	return _T("Back Overlay Bot");
	}

	return _T("");
}

long DlgCriteriaList::CalcCriteriaCount()
{
	long count = 0;
	while (GetCriteria(count) != nullptr)
	{
		count++;
	}

	return count;
}

void DlgCriteriaList::OnOK()
{
	long criteriaCount = CalcCriteriaCount();

	for (long index = 0; index < criteriaCount; index++)
	{
		auto* criteria = GetCriteria(index);

		if (criteria->m_useStandardValue)
		{
			criteria->m_standardValue_mm = _ttof(m_gridCtrl->GetItemText(index + 1, 1));
		}

		criteria->m_specMin_mm = _ttof(m_gridCtrl->GetItemText(index + 1, 2));
		criteria->m_specMax_mm = _ttof(m_gridCtrl->GetItemText(index + 1, 3));
	}

	CDialogEx::OnOK();
}
