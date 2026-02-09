// DlgMergeOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Inspection.h"
#include "DlgMergeOptions.h"
#include "afxdialogex.h"
#include "InspMergeOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DlgMergeOptions dialog

IMPLEMENT_DYNAMIC(DlgMergeOptions, CDialogEx)

DlgMergeOptions::DlgMergeOptions(const Inspection::InspMergeOptions& option, CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgMergeOptions::IDD, pParent)
	, m_length((long)option.m_mergeLength.m_type)
	, m_topTilt((long)option.m_mergeTopTilt.m_type)
	, m_bottomTilt((long)option.m_mergeBottomTilt.m_type)
	, m_misalign((long)option.m_mergeMisalign.m_type)
{

}

DlgMergeOptions::~DlgMergeOptions()
{
}

void DlgMergeOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_LENGTH, m_length);
	DDX_CBIndex(pDX, IDC_COMBO_TOP_TILT, m_topTilt);
	DDX_CBIndex(pDX, IDC_COMBO_BOTTOM_TILT, m_bottomTilt);
	DDX_CBIndex(pDX, IDC_COMBO_MISALIGN, m_misalign);
}


BEGIN_MESSAGE_MAP(DlgMergeOptions, CDialogEx)
END_MESSAGE_MAP()


// DlgMergeOptions message handlers
