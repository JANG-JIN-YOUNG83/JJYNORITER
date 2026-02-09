// DlgClassificationCondition.cpp: 구현 파일
//

#include "resource.h"
#include "stdafx.h"
#include "Inspection.h"
#include "DlgClassificationCondition.h"
#include "afxdialogex.h"
#include "CDlgClassificationST.h"
#include "../Module/CustomControl/XTPPropertyGridItemButton.h"
#include "../Module/CustomControl/PropertyGridHelper.h"
#include "../VisionModule/Language/LanguageText.h"
#include "../VisionModule/Language/PropertyGridHelperEx.h"

#include <PropertyGrid/XTPPropertyGridDefines.h>
#include "DlgEditClassification.h"
#include "../Module/Utility/SystemFont.h"
#include "../Module/CustomControl/CustomButton.h"

// CDlgClassificationCondition 대화 상자

IMPLEMENT_DYNAMIC(CDlgClassificationCondition, CDialogEx)

CDlgClassificationCondition::CDlgClassificationCondition(CWnd* pParent, DefectType DefectName)
	: CDialogEx(IDD_DIALOG_CLASSIFICATION_CONDITION, pParent)
    , m_inspectionItemGrid(new CXTPPropertyGrid)
{
    m_pParent = (CDlgClassificationST*)pParent;
    m_defectName = DefectName;
    m_btnAddOrCondition = new CCustomButton;
    m_btnDeleteOrCondition = new CCustomButton;
    m_btnApply = new CCustomButton;

}

CDlgClassificationCondition::~CDlgClassificationCondition()
{
    delete m_inspectionItemGrid;
    delete m_btnAddOrCondition;
    delete m_btnDeleteOrCondition;
    delete m_btnApply;
}

void CDlgClassificationCondition::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_ADD_OR_CONDITION, *m_btnAddOrCondition);
    DDX_Control(pDX, IDC_BTN_DELETE_OR_CONDITION, *m_btnDeleteOrCondition);
    DDX_Control(pDX, IDOK, *m_btnApply);
    DDX_Control(pDX, IDC_EDIT_TOTAL_CONDITION, m_editTotal);
}


BEGIN_MESSAGE_MAP(CDlgClassificationCondition, CDialogEx)
ON_WM_CLOSE()
ON_BN_CLICKED(IDCANCEL, &CDlgClassificationCondition::OnBnClickedCancel)
ON_BN_CLICKED(IDOK, &CDlgClassificationCondition::OnBnClickedOk)
ON_WM_CLOSE()
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BTN_EDIT_1, &CDlgClassificationCondition::OnBnClickedBtnEdit1)
ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
ON_BN_CLICKED(IDC_BTN_ADD_OR_CONDITION, &CDlgClassificationCondition::OnBnClickedBtnAddOrCondition)
ON_BN_CLICKED(IDC_BTN_DELETE_OR_CONDITION, &CDlgClassificationCondition::OnBnClickedBtnDeleteOrCondition)
ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgClassificationCondition 메시지 처리기

BOOL CDlgClassificationCondition::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    
    SetWindowText(GetDefectName(m_defectName));
    updateDefectValue();

    CFont listFont;
    listFont.CreatePointFont(100, L"HY 견고딕");
    
    CRect rtDlg;
    GetClientRect(rtDlg);

    CRect rtTotal(10, 10, rtDlg.Width() - 10, 10 + rtDlg.Height() * 0.2);
    m_editTotal.MoveWindow(rtTotal);
    m_editTotal.SetFont(&listFont);

    CRect rtGrid(10, rtTotal.bottom + 10, rtDlg.Width() - 10, rtTotal.bottom + 10 + rtDlg.Height() * 0.6);

    CRect btnAdd(rtDlg.CenterPoint().x - 220, rtDlg.bottom - 45, rtDlg.CenterPoint().x - 50, rtDlg.bottom - 5);
    m_btnAddOrCondition->MoveWindow(btnAdd);
    m_btnAddOrCondition->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 4, 5);

    CRect btnDelete(btnAdd.right + 10, btnAdd.top, btnAdd.right + 90, btnAdd.bottom);
    m_btnDeleteOrCondition->MoveWindow(btnDelete);
    m_btnDeleteOrCondition->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 4, 5);

    CRect btnApply(btnDelete.right + 10, btnDelete.top, btnDelete.right + 90, btnDelete.bottom);
    m_btnApply->MoveWindow(btnApply);
    m_btnApply->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 4, 5);

    /*CFont btnFont;
    btnFont.CreatePointFont(50, L"HY 견고딕");

    m_btnAddOrCondition.SetFont(&btnFont);
    m_btnDeleteOrCondition.SetFont(&btnFont);
    m_btnApply.SetFont(&btnFont);*/

    

    

    m_inspectionItemGrid->Create(rtGrid, this, 0);
    m_inspectionItemGrid->SetViewDivider(0.2);
    m_inspectionItemGrid->HighlightChangedItems(TRUE);
    m_inspectionItemGrid->SetBorderStyle(XTPPropertyGridBorderStyle::xtpGridBorderFlat);
    m_inspectionItemGrid->SetShowInplaceButtonsAlways(TRUE);
    m_inspectionItemGrid->SetFont(&listFont);
    


    InitGrid();
    /*GetDlgItem(IDC_EDIT)->SetFocus();
    SetDlgItemText(IDC_EDIT, m_strValue);*/

    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgClassificationCondition::OnBnClickedCancel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDialogEx::OnCancel();
}

void CDlgClassificationCondition::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    //OnDestroy();
    //m_bTest = TRUE;
    auto* item = (CXTPPropertyGridItems*)m_inspectionItemGrid->GetCategories();

    long nitem = m_inspectionItemGrid->GetCount();
    std::vector<CString> vecORConditions;
    for (long nIndex = 1; nIndex < nitem; nIndex++)
    {
        auto* item = (CXTPPropertyGridItem*)m_inspectionItemGrid->GetItem(nIndex);
        CString strValue = item->GetValue();

        vecORConditions.push_back(strValue);
    }

    m_pParent->SpecTableTrans(vecORConditions, m_defectName);
    CDialogEx::OnOK();
    
    //m_pParent->DeleteDlg();
}

void CDlgClassificationCondition::updateDefectValue()
{
    CString strVal;
    CISISpecInfo* spec = m_pParent->m_ISI_Spec_table.GetSpecInfo(m_defectName);
    //std::vector<CISISpecInfo>* vecSpec = m_pParent->m_ISI_Spec_table.GetSpecInfo_Multi(m_defectName);

    //long n = vecSpec->size();
    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_pParent->m_ISI_Spec_table.m_mapSpec_multi.begin(), m_pParent->m_ISI_Spec_table.m_mapSpec_multi.end());
    //
    ////std::sort(vecSpec.begin(), vecSpec.end(), compare_multi);

    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>>::iterator vecitrStartVec, vecitrEndVec;
    vecitrStartVec = vecSpec.begin();
    vecitrEndVec = vecSpec.end();

    m_vecStrCondition.clear();


    while (vecitrStartVec != vecitrEndVec)
    {
        if (vecitrStartVec->first == m_defectName)
        {
            
            for (long n = 0; n < vecitrStartVec->second.size(); n++)
            {
                long index = (long)vecitrStartVec->first;

                CString strValue;


                if (vecitrStartVec->second[n].AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("Area %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].AREA_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].AREA.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].AREA_RATIO_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_Area %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].AREA_RATIO_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].AREA_Ratio.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_XY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_XY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_XY_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_XY.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_YX_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_YX %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_YX_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_YX.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].WIDTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("Width %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].WIDTH_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].WIDTH_mm.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].LENGTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("Length %s %.2f &"), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].LENGTH_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].LENGTH_mm.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_WL_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_WL %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_WL_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_WL.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_LW_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_LW %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_LW_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_LW.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].MIN_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("MIN_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].MIN_GV_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].MIN_GV.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].AVG_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].AVG_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].AVG_GV.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].MAX_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("MAX_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].MAX_GV_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].MAX_GV.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("BRIGHT_AVG_GV %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].BRIGHT_AVG_GV_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].BRIGHT_AVG_GV.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("DARK_AVG_GV %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].DARK_AVG_GV_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].DARK_AVG_GV.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("MASS_CENTER_GV %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].MASS_CENTER_GV_GV.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY_BLACK %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_BLACK.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY_WHITE %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_WHITE.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].SECTION_THRESHOLD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("SECTION_THRESHOLD %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].SECTION_THRESHOLD_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].SECTION_THRESHOLD.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].HOR_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("HOR %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].HOR_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].HOR.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].VER_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("VER %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].VER_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].VER.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_HV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_HV %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_HV_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_HV.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_VH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_VH %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_VH_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_VH.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_WHITE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_WHITE %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_WHITE_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_WHITE.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_DARK_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_DARK %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_DARK_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].RATIO_DARK.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("250>CNT %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].OVER_200_GV_PIXEL_COUNT.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("DISTANCE_TO_EDGE %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].DISTANCE_TO_EDGE_mm.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].DIS_TO_EDGE_2_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("DISTANCE_TO_EDGE_2 %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].DIS_TO_EDGE_2_mm_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].DISTANCE_TO_EDGE_2_mm.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].IS_NEAR_EDGE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("IS_NEAR_EDGE %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].IS_NEAR_EDGE_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].IS_NEAR_EDGE.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY_SHARPNESS %s %.2f & "),
                        enumtoString().CompareOperator2String(
                            (enCompareOperator)vecitrStartVec->second[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS.GetValue());
                    strValue += strVal;
                }
                if (vecitrStartVec->second[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("WHITE_EDGE_ENERGY_SHARPNESS %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
                    strValue += strVal;
                }
                if (vecitrStartVec->second[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("BLACK_EDGE_ENERGY_SHARPNESS %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
                    strValue += strVal;
                }
                // _2 변수 값 추가
                if (vecitrStartVec->second[n].AREA_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("Area %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].AREA_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].AREA_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].AREA_RATIO_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_Area %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].AREA_RATIO_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].AREA_Ratio_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_XY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_XY %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_XY_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_XY_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_YX_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_YX %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_YX_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_YX_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].WIDTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("Width %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].WIDTH_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].WIDTH_mm_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].LENGTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("Length %s %.2f &"), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].LENGTH_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].LENGTH_mm_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_WL_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_WL %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_WL_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_WL_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_LW_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_LW %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_LW_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_LW_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].MIN_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("MIN_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].MIN_GV_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].MIN_GV_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].AVG_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].AVG_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].AVG_GV_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].MAX_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("MAX_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].MAX_GV_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].MAX_GV_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].BRIGHT_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("BRIGHT_AVG_GV %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].BRIGHT_AVG_GV_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].BRIGHT_AVG_GV_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].DARK_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("DARK_AVG_GV %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].DARK_AVG_GV_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].DARK_AVG_GV_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("MASS_CENTER_GV %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].MASS_CENTER_GV_GV_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY_BLACK %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_BLACK_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY_WHITE %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_WHITE_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].SECTION_THRESHOLD_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("SECTION_THRESHOLD %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].SECTION_THRESHOLD_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].SECTION_THRESHOLD_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].HOR_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("HOR %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].HOR_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].HOR_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].VER_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("VER %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].VER_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].VER_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_HV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_HV %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_HV_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_HV_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_VH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_VH %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_VH_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_VH_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_WHITE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_WHITE %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_WHITE_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_WHITE_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].RATIO_DARK_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("RATIO_DARK %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].RATIO_DARK_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].RATIO_DARK_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("250>CNT %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].OVER_200_GV_PIXEL_COUNT_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("DISTANCE_TO_EDGE %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].DISTANCE_TO_EDGE_mm_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("DISTANCE_TO_EDGE_2 %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].DISTANCE_TO_EDGE_2_mm_2.GetValue());
                    strValue += strVal;
                }

                if (vecitrStartVec->second[n].IS_NEAR_EDGE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("IS_NEAR_EDGE %s %d & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].IS_NEAR_EDGE_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].IS_NEAR_EDGE_2.GetValue());
                    strValue += strVal;
                }
                if (vecitrStartVec->second[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("EDGE_ENERGY_WHITE %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_2.GetValue());
                    strValue += strVal;
                }
                if (vecitrStartVec->second[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("WHITE_EDGE_ENERGY_SHARPNESS %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_WHITE_2.GetValue());
                    strValue += strVal;
                }
                if (vecitrStartVec->second[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
                {
                    strVal.Format(_T("BLACK_EDGE_ENERGY_SHARPNESS %s %.2f & "),
                        enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                        vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_BLACK_2.GetValue());
                    strValue += strVal;
                }
                strValue = strValue.Mid(0, strValue.GetLength() - 3);
                m_vecStrValue.push_back(strValue);
            }

        }
        vecitrStartVec++;
    }

}

void CDlgClassificationCondition::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    CDialogEx::OnClose();
}

void CDlgClassificationCondition::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL CDlgClassificationCondition::StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue)
{
    long nDivideTotalCount = 0;
    CString strSub;
    while (FALSE != AfxExtractSubString(strSub, strFull, nDivideTotalCount++, chDivide))
    {
        vecstrValue.push_back(strSub);
    }
    return TRUE;
}




void CDlgClassificationCondition::OnBnClickedBtnEdit1()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    // Dialog 생성 되면서 수정이 되게끔...
   

    auto* item = (CXTPPropertyGridItem*)m_inspectionItemGrid->GetItem(m_nIndex);


    CDlgEditClassification* dlg = new CDlgEditClassification(item->GetValue(), this);
    dlg->Create(IDD_DIALOG_EDIT_CONDITION, this);
    dlg->CenterWindow(CWnd::GetDesktopWindow());
    dlg->ShowWindow(SW_SHOW);	

    

    //CDlgEditClassification dlg;
    //if (dlg->DoModal() == IDOK)
    //{
    //
    //}


}

void CDlgClassificationCondition::OnBnClickedSetting(LPARAM lparam, const int& nID)
{
   /* if (((CXTPPropertyGridItem*)lparam)->GetIndex() <= 0)
        return;

    CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
    if (pItem == nullptr)
        return;
    
    CString strCaption = pItem->GetCaption();
    long nSettingIndex = _ttoi(strCaption.Right(3));

    m_nIndex = nSettingIndex;*/

}

void CDlgClassificationCondition::InitGrid()
{
    m_inspectionItemGrid->ResetContent();

    if (m_strValue.Right(1) == _T(","))
    {
        m_strValue.Delete(m_strValue.GetLength() - 1, 1);
    }
    //m_vecStrCondition.push_back(m_strValue);

    m_vecStrCondition = m_vecStrValue;



    /*StringTokenDivideToCString(m_strValue, ',', m_vecStrCondition);

    for (int n = 0; n < m_vecStrCondition.size(); n++)
    {
        if (m_vecStrCondition[n] == _T(""))
        {
            m_vecStrCondition.erase(m_vecStrCondition.begin() + n);
            continue;
        }
        SetDlgItemText(IDC_EDIT_CONDITION_1 + n, m_vecStrCondition[n]);
    }
    */
    updateGrid();
}

LRESULT CDlgClassificationCondition::OnGridNotify(WPARAM wparam, LPARAM lparam)
{
    if (wparam == XTP_PGN_INPLACEBUTTONDOWN)
    {
        auto index = ((CXTPPropertyGridInplaceButton*)lparam)->GetID();

        auto* pItem = ((CXTPPropertyGridInplaceButton*)lparam)->GetItem();
        long nCurIndex = pItem->GetIndex();
       // long nCnt = ((CXTPPropertyGridItem*)lparam)->GetDefaultValue();
        m_nIndex = nCurIndex;
        switch (index)
        {
            case IDC_BTN_EDIT_1:
                OnBnClickedBtnEdit1();
                break;
        }
    }
    if (wparam == XTP_PGN_SELECTION_CHANGED)
    {
        auto index = ((CXTPPropertyGridItem*)lparam)->GetID();

        auto pItem = ((CXTPPropertyGridItem*)lparam)->GetIndex();
        m_nIndex = pItem;
    }
    return 0;

}

void CDlgClassificationCondition::OnBnClickedBtnAddOrCondition()
{   
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_vecStrCondition.push_back(_T(""));

    //GetDlgItem(IDC_BTN_DELETE_OR_CONDITION)->EnableWindow(FALSE);
    //GetDlgItem(IDOK)->EnableWindow(FALSE);
    updateGrid();
}

void CDlgClassificationCondition::OnBnClickedBtnDeleteOrCondition()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_nIndex > 0)
    {
        m_vecStrCondition.erase(m_vecStrCondition.begin() + m_nIndex - 1);
        updateGrid();
    }
}


void CDlgClassificationCondition::updateGrid()
{
    m_inspectionItemGrid->ResetContent();
    CString strTotal;

    if (auto* category = m_inspectionItemGrid->AddCategory(_T("Current Classification")))
    {
        CString str;
        str.Format(_T(""));

        for (long nCondition = 0; nCondition < m_vecStrCondition.size(); nCondition++)
        {
            str.Format(_T("조건_%d"), nCondition);
            auto* item1 = PropertyGridHelper::AddLink(category, str, m_vecStrCondition[nCondition]);

            item1->SetDescription(m_vecStrCondition[nCondition]);
            item1->SetTooltip(str);

            auto pButton = item1->GetInplaceButtons()->AddButton(new CXTPPropertyGridInplaceButton(IDC_BTN_EDIT_1));
            pButton->SetCaption(_T("Setting"));
            
            if (m_vecStrCondition[nCondition].GetLength() > 0)
            {
                strTotal += _T("(") + m_vecStrCondition[nCondition] + _T(")");
                if (nCondition + 1 < m_vecStrCondition.size())
                    strTotal += _T("|");
            }
        }
        category->Expand();
    }
    m_inspectionItemGrid->RedrawControl();
    
    
    SetDlgItemText(IDC_EDIT_TOTAL_CONDITION, strTotal);
}

void CDlgClassificationCondition::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    GetClientRect(m_nDlgSize);
}

BOOL CDlgClassificationCondition::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN) // ENTER키 눌릴 시
            return TRUE;
        else if (pMsg->wParam == VK_ESCAPE) // ESC키 눌릴 시
            return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

