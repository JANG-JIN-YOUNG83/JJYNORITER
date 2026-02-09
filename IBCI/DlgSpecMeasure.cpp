// DlgSpecMeasure.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgSpecMeasure.h"
#include "afxdialogex.h"
#include "../Module/CustomControl/XTPPropertyGridItemButton.h"
#include "../Module/CustomControl/PropertyGridHelper.h"
#include "../VisionModule/Language/LanguageText.h"
#include "../VisionModule/Language/PropertyGridHelperEx.h"
#include "../Inspection/InspectionMeasure.h"
#include "../Inspection/DlgInspectionMain.h"
#include "../Inspection/InspectionRoot.h"
#include "../Inspection/Accessor.h"


// CDlgSpecMeasure 대화 상자

IMPLEMENT_DYNAMIC(CDlgSpecMeasure, CDialogEx)

CDlgSpecMeasure::CDlgSpecMeasure(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SPEC_MEASURE, pParent)
    , m_inspectionItemGrid(new CXTPPropertyGrid)
{

}

CDlgSpecMeasure::~CDlgSpecMeasure()
{
    delete m_inspectionItemGrid;
}

void CDlgSpecMeasure::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSpecMeasure, CDialogEx)
ON_BN_CLICKED(IDOK, &CDlgSpecMeasure::OnBnClickedOk)
ON_BN_CLICKED(IDCANCEL, &CDlgSpecMeasure::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgSpecMeasure 메시지 처리기

void CDlgSpecMeasure::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDialogEx::OnOK();
}

void CDlgSpecMeasure::OnBnClickedCancel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDialogEx::OnCancel();
}

BOOL CDlgSpecMeasure::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    CRect rtDlg;
    GetClientRect(rtDlg);

    CRect rtGrid(rtDlg.left + 10, rtDlg.top + 10 , rtDlg.right - 10, rtDlg.bottom - 35);

    m_inspectionItemGrid->Create(rtGrid, this, 0);
    m_inspectionItemGrid->SetViewDivider(0.5);
    m_inspectionItemGrid->HighlightChangedItems(TRUE);
    m_inspectionItemGrid->SetBorderStyle(XTPPropertyGridBorderStyle::xtpGridBorderFlat);
    m_inspectionItemGrid->SetShowInplaceButtonsAlways(TRUE);
    //m_inspectionItemGrid->SetFont(&listFont);

    updateGrid();
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgSpecMeasure::updateGrid()
{
    m_inspectionItemGrid->ResetContent();   

    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionMeasure* CellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);
    

    if (auto* category = m_inspectionItemGrid->AddCategory(_T("General")))
    {
        if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, _T("Use Inspection"), CellMeasure->m_para.m_bSaveMeasureResult))
        {
            item->SetID(IDC_PARA_SAVE_MEASURE_RESULT);
            if (Share::enLoginMode::CrewLeader >= m_loginMode)
                item->SetReadOnly();
        }

        if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, _T("Select ImageType (Bright/Dark)"), CellMeasure->m_para.m_nImageDark))
        {
            item->GetConstraints()->AddConstraint(_T("Bright Image"), 0);
            item->GetConstraints()->AddConstraint(_T("Dark Image"), 1);

            item->SetID(IDC_PARA_MEASURE_SELECT_IMAGE_TYPE);
        }
        
        if (auto* item = PropertyGridHelper::AddLink(category, _T("Inspection Range(1 Cycle) [mm]"), _T("%.2f"), CellMeasure->m_para.m_fCellLengthCalcPosition))
        {
            item->SetID(IDC_PARA_CELL_LENGTH_TILT_CALC_POSITION);
            if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("Measure Item Count"), CellMeasure->m_para.m_nCountMeasureItem))
        {
            item->SetID(IDC_PARA_MEASURE_ITEM_COUNT);
            item->SetDescription(_T("Measure Item Count"));
        }

        if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, _T("Use MSA"), CellMeasure->m_para.m_bUseMSA))
        {
        }
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("MSA Line To Point Distance"), CellMeasure->m_para.m_nMSADistance))
        {
            item->GetConstraints()->AddConstraint(_T("None"), 0);
            item->GetConstraints()->AddConstraint(_T("Start"), 1);
            item->GetConstraints()->AddConstraint(_T("End"), 2);

            if (CellMeasure->m_para.m_bUseMSA == 0)
                item->SetReadOnly();
        }

        category->Expand();
    }
    
    CInspectionCellAlign* cellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
    long nCountLine = cellAlign->m_para.m_nLineCountMeasure.GetValue();

    CString strMeasureOrder;
    strMeasureOrder = _T("Measure Order");

    CString strType, strStart, strEnd;
    CString strSpecStandard, strSpecMin, strSpecMax, strSpecMin_C, strSpecMax_C;
    for (long measure = 0; measure < COUNT_CELL_MEASURE; measure++)
    {
        CString strMeasureItemCategory;
        strMeasureItemCategory.Format(_T("Measure Item %d"), measure + 1);
        if (auto* category = m_inspectionItemGrid->AddCategory(strMeasureItemCategory))
        {
            strType.Format(_T("%d Type"), measure + 1);
            strStart.Format(_T("%d Start Boundary"), measure + 1);
            strEnd.Format(_T("%d End Boundary"), measure + 1);

            strSpecStandard.Format(_T("%d Spec Standard"), measure + 1);
            strSpecMax.Format(_T("%d USL"), measure + 1);
            strSpecMin.Format(_T("%d LSL"), measure + 1);

            strSpecMax_C.Format(_T("%d UCL"), measure + 1);
            strSpecMin_C.Format(_T("%d LCL"), measure + 1);

            if (auto* item = PropertyGridHelper::AddLink_Enum(category, strType, CellMeasure->m_para.m_vecMeasureType[measure]))
            {
                for (long measure2 = (long)DefectType::START_Coater_Dimension + 1; measure2 < (long)DefectType::End_Coater_Dimension; measure2++)
                {
                    item->GetConstraints()->AddConstraint(GetDefectName((DefectType)measure2), measure2 - (long)DefectType::START_Coater_Dimension - 1);
                }
                item->SetID(IDC_PARA_MEASURE_ITEM_TYPE + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink_Enum(category, strStart, CellMeasure->m_para.m_vecMeasureStart[measure]))
            {
                long constraint = 0;
                for (constraint = 0; constraint < nCountLine; constraint++)
                {
                    CString strRoi;
                    strRoi.Format(_T("%d"), constraint + 1);
                    item->GetConstraints()->AddConstraint(strRoi, constraint);
                }
                item->GetConstraints()->AddConstraint(_T("none"), constraint++);
                item->GetConstraints()->AddConstraint(_T("Half Point"), constraint++);

                item->SetID(IDC_PARA_MEASURE_ITEM_START + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink_Enum(category, strEnd, CellMeasure->m_para.m_vecMeasureEnd[measure]))
            {
                long constraint = 0;
                for (constraint = 0; constraint < nCountLine; constraint++)
                {
                    CString strRoi;
                    strRoi.Format(_T("%d"), constraint + 1);
                    item->GetConstraints()->AddConstraint(strRoi, constraint);
                }
                item->GetConstraints()->AddConstraint(_T("none"), constraint++);
                item->GetConstraints()->AddConstraint(_T("Half Point"), constraint++);

                item->SetID(IDC_PARA_MEASURE_ITEM_END + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }

            if (auto* item = PropertyGridHelper::AddLink(category, strSpecStandard, _T("%.1f"), CellMeasure->m_para.m_vecMeasureStandard[measure]))
            {
                item->SetID(IDC_PARA_MEASURE_ITEM_STANDARD + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }
            
            if (auto* item = PropertyGridHelper::AddLink(category, strSpecMax_C, _T("%.1f"), CellMeasure->m_para.m_vecMeasureOffsetUCL[measure]))
            {
                item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETUCL + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink(category, strSpecMin_C, _T("%.1f"), CellMeasure->m_para.m_vecMeasureOffsetLCL[measure]))
            {
                item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETLCL + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink(category, strSpecMax, _T("%.1f"), CellMeasure->m_para.m_vecMeasureOffsetUSL[measure]))
            {
                item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETUSL + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }
            if (auto* item = PropertyGridHelper::AddLink(category, strSpecMin, _T("%.1f"), CellMeasure->m_para.m_vecMeasureOffsetLSL[measure]))
            {
                item->SetID(IDC_PARA_MEASURE_ITEM_OFFSETLSL + measure);
                item->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            }

            category->SetHidden(!(CellMeasure->m_para.m_nCountMeasureItem > measure));
            category->Expand();
        }
    }
}
