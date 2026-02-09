#include "stdafx.h"
#include "Inspection.h"
#include "DlgEdgeDetectPara.h"
#include "afxdialogex.h"
#include "Algorithm/basedef.h"
#include"../Module/CustomControl/PropertyGridHelper.h"
#include "../VisionModule/Language/PropertyGridHelperEx.h"

// DlgEdgeDetectPara 대화 상자

IMPLEMENT_DYNAMIC(DlgEdgeDetectPara, CDialogEx)

DlgEdgeDetectPara::DlgEdgeDetectPara(Base::ParaEdgeDetection& paraEdgeDetection, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_EDGE, pParent)
	, m_paraEdgeDetection(paraEdgeDetection)
{
}

DlgEdgeDetectPara::~DlgEdgeDetectPara()
{
}

void DlgEdgeDetectPara::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgEdgeDetectPara, CDialogEx)
END_MESSAGE_MAP()


BOOL DlgEdgeDetectPara::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CRect rtProperty;
	this->GetDlgItem(IDC_PROPERTY)->GetWindowRect(rtProperty);
	ScreenToClient(rtProperty);

	m_property.Create(rtProperty, this, WS_CHILD | WS_VISIBLE);
	m_property.SetViewDivider(0.6);
	m_property.HighlightChangedItems(TRUE);
	m_property.SetBorderStyle(XTPPropertyGridBorderStyle::xtpGridBorderFlat);
	m_property.SetShowInplaceButtonsAlways(TRUE);

	if (auto* category = m_property.AddCategory(_T("Para")))
	{
		//PropertyGridHelper::AddLink(category, _T("HalfFilterLength"), *m_paraEdgeDetection.m_HalfFilterLength);
		PropertyGridHelper::AddLink(category, _T("HalfFilterLength"), m_paraEdgeDetection.m_HalfFilterLength);
		PropertyGridHelper::AddLink(category, _T("GaussianDeviation"), _T("%.2f"), m_paraEdgeDetection.m_GaussianDeviation);
		PropertyGridHelper::AddLink_Bool(category, _T("FindFirstEdge"),  m_paraEdgeDetection.m_FindFirstEdge);
		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("DetectType"), m_paraEdgeDetection.m_DetectType))
		{
			item->GetConstraints()->AddConstraint(_T("Both"), (int)IPVM::EdgeType::Both);
			item->GetConstraints()->AddConstraint(_T("Rising"), (int)IPVM::EdgeType::Rising);
			item->GetConstraints()->AddConstraint(_T("Falling"), (int)IPVM::EdgeType::Falling);
		}
		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("SearchDirection"), m_paraEdgeDetection.m_SearchDirection))
		{
			item->GetConstraints()->AddConstraint(_T("ToLeft"), (int)IPVM::SearchDirection::ToLeft);
			item->GetConstraints()->AddConstraint(_T("ToRight"), (int)IPVM::SearchDirection::ToRight);
			item->GetConstraints()->AddConstraint(_T("ToUp"), (int)IPVM::SearchDirection::ToUp);
			item->GetConstraints()->AddConstraint(_T("ToDown"), (int)IPVM::SearchDirection::ToDown);
		}
		//PropertyGridHelper::AddLink(category, _T("EdgeThreshold"), _T("%.2f"), *m_paraEdgeDetection.m_EdgeThreshold);
		PropertyGridHelper::AddLink(category, _T("EdgeThreshold"), _T("%.2f"), m_paraEdgeDetection.m_EdgeThreshold);

		
		PropertyGridHelper::AddLink(category, _T("RansacThreshold"), _T("%.2f"), m_paraEdgeDetection.m_RansacThreshold);
		PropertyGridHelper::AddLink(category, _T("DetectCount"), m_paraEdgeDetection.m_DetectCount);

		if(auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Select ImageType"), m_paraEdgeDetection.m_ImageTypeDark))
        {
            item->GetConstraints()->AddConstraint(_T("Bright"), 0);
            item->GetConstraints()->AddConstraint(_T("Dark"), 1);
		}

		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Select Edge Detect Mode"), m_paraEdgeDetection.m_EdgeDetectMode))
        {
            item->GetConstraints()->AddConstraint(_T("Inner"), 0);
            item->GetConstraints()->AddConstraint(_T("Outer"), 1);
            item->GetConstraints()->AddConstraint(_T("Average"), 2);
        }
		category->Expand();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
