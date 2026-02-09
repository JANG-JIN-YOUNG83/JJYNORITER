// CDlgEditClassificationCondition.cpp: 구현 파일
//
#include "resource.h"
#include "stdafx.h"
#include "Inspection.h"
#include "DlgEditClassification.h"
#include "afxdialogex.h"
#include "DlgClassificationCondition.h"
#include "../Module/Utility/SystemFont.h"
#include "../Module/CustomControl/CustomButton.h"

class CDlgClassificationCondition;

// CDlgEditClassificationCondition 대화 상자

IMPLEMENT_DYNAMIC(CDlgEditClassification, CDialogEx)

CDlgEditClassification::CDlgEditClassification(CString strValue, CDlgClassificationCondition* pParent) 
	: CDialogEx(IDD_DIALOG_EDIT_CONDITION, pParent)
    , m_nListCount(0)
    , m_nSelectIndex(-1)
{
    m_strTotalCondition = strValue;
    m_parent = pParent;
    m_btnAddAndCondition = new CCustomButton;
    m_btnDeleteCondition = new CCustomButton;
    m_btnApply = new CCustomButton;
    m_btnTransCondition = new CCustomButton;
}

CDlgEditClassification::~CDlgEditClassification()
{

    delete m_btnAddAndCondition;
    delete m_btnDeleteCondition;
    delete m_btnApply;
    delete m_btnTransCondition;
}

void CDlgEditClassification::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SET_DEFECTTYPE, m_comboSetDefectType);
    DDX_Control(pDX, IDC_COMBO_SET_COMPARE, m_comboSetCompare);
    DDX_Control(pDX, IDC_EDIT_SET_VALUE, m_editSetValue);
    DDX_Control(pDX, IDC_EDIT_AND_CONDITION, m_editCondition);
    DDX_Control(pDX, IDC_BTN_ADD_AND_CONDTION, *m_btnAddAndCondition);
    DDX_Control(pDX, IDC_LIST_TOTAL_CONDITION, m_listCtrl);
    DDX_Control(pDX, IDC_BTN_DELETE_CONDITION, *m_btnDeleteCondition);
    DDX_Control(pDX, IDC_BTN_APPLY, *m_btnApply);
    DDX_Control(pDX, IDC_BTN_TRANS_CONDITION, *m_btnTransCondition);
}


BEGIN_MESSAGE_MAP(CDlgEditClassification, CDialogEx)
ON_WM_SIZE()
ON_BN_CLICKED(IDC_BTN_ADD_AND_CONDTION, &CDlgEditClassification::OnBnClickedBtnAddAndCondtion)
ON_CBN_SELCHANGE(IDC_COMBO_SET_DEFECTTYPE, &CDlgEditClassification::OnCbnSelchangeComboSetDefecttype)
ON_CBN_SELCHANGE(IDC_COMBO_SET_COMPARE, &CDlgEditClassification::OnCbnSelchangeComboSetCompare)
ON_EN_CHANGE(IDC_EDIT_SET_VALUE, &CDlgEditClassification::OnEnChangeEditSetValue)
ON_BN_CLICKED(IDC_BTN_DELETE_CONDITION, &CDlgEditClassification::OnBnClickedBtnDeleteCondition)
ON_NOTIFY(NM_CLICK, IDC_LIST_TOTAL_CONDITION, &CDlgEditClassification::OnNMClickListTotalCondition)
ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgEditClassification::OnBnClickedBtnApply)
ON_BN_CLICKED(IDC_BTN_TRANS_CONDITION, &CDlgEditClassification::OnBnClickedBtnTransCondition)
ON_WM_LBUTTONDOWN()
ON_EN_KILLFOCUS(IDC_EDIT_SET_VALUE, &CDlgEditClassification::OnEnKillfocusEditSetValue)
END_MESSAGE_MAP()


// CDlgEditClassificationCondition 메시지 처리기

BOOL CDlgEditClassification::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.

    m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);

    CString strIndex;
    strIndex.Format(_T("%d"),m_parent->m_nIndex);
    SetWindowText(_T("Setting"));

    CRect rtDlg;
    GetClientRect(rtDlg);

    CRect rtTotalList(rtDlg.left + 10, 20, rtDlg.right - 10, rtDlg.Height() * 0.6);
    m_listCtrl.MoveWindow(rtTotalList);

    CRect btnAdd(rtDlg.CenterPoint().x - 150, rtDlg.bottom - 60, rtDlg.CenterPoint().x, rtDlg.bottom - 10);
    m_btnAddAndCondition->MoveWindow(btnAdd);

    m_btnAddAndCondition->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 4, 5);
    //m_btnAddAndCondition->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);

    //CRect btnTrans(btnAdd.right + 10, btnAdd.top, btnAdd.right + 160, btnAdd.bottom);
    //m_btnTransCondition->MoveWindow(btnTrans);

    m_btnTransCondition->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 4, 5);
    //m_btnTransCondition->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);

    CRect btnDelete(btnAdd.right + 10, btnAdd.top, btnAdd.right + 70, btnAdd.bottom);
    m_btnDeleteCondition->MoveWindow(btnDelete);

    m_btnDeleteCondition->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 4, 5);
    //m_btnDeleteCondition->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);

    CRect btnApply(btnDelete.right + 10, btnDelete.top, btnDelete.right + 70, btnDelete.bottom);
    m_btnApply->MoveWindow(btnApply);

    m_btnApply->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 4, 5);
    //m_btnApply->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);

    CRect rtCondtion(rtTotalList.left, rtTotalList.bottom + 10, rtTotalList.left + rtTotalList.Width() * 0.35 - 10, rtTotalList.bottom + 30);
    m_editCondition.MoveWindow(rtCondtion);

    CRect rtDefectType(rtCondtion.right + 10, rtCondtion.top, rtCondtion.right + rtTotalList.Width() * 0.3 - 10, btnAdd.top);
    m_comboSetDefectType.MoveWindow(rtDefectType);

    CRect rtCompare(rtDefectType.right + 10, rtDefectType.top, rtDefectType.right + rtTotalList.Width() * 0.2 - 10, btnAdd.top);
    m_comboSetCompare.MoveWindow(rtCompare);

    CRect rtValue(rtCompare.right + 10, rtCompare.top, rtCompare.right + rtTotalList.Width() * 0.2 - 10, rtCompare.top + 20);
    m_editSetValue.MoveWindow(rtValue);

    CFont fontMain, font;

    CFont listFont;
    listFont.CreatePointFont(100, L"HY 견고딕");

    CFont btnFont;
    btnFont.CreatePointFont(60, L"휴먼둥근헤드라인");

    m_comboSetCompare.SetFont(&listFont);
    m_comboSetCompare.SetItemHeight(0, 20);

    m_comboSetDefectType.SetFont(&listFont);
    m_comboSetDefectType.SetItemHeight(0, 20);
    
    m_editCondition.SetFont(&listFont);
    m_editSetValue.SetFont(&listFont);
   
    
    Utility::SystemFont::CreateGUIFont(fontMain, 50);	   
    Utility::SystemFont::CreateGUIFont(font, 80);	   
    GetDlgItem(IDC_LIST_TOTAL_CONDITION)->SetFont(&listFont);
    GetDlgItem(IDC_EDIT_AND_CONDITION)->SetFont(&font);
    /*GetDlgItem(IDC_EDIT_TOTAL_AND_CONDITION)->SetFont(&listFont);
    GetDlgItem(IDC_COMBO_SET_DEFECTTYPE)->SetFont(&listFont);
    GetDlgItem(IDC_COMBO_SET_COMPARE)->SetFont(&listFont);*/
        
    // 항목 셋팅
    //m_comboSetDefectType.InsertString(0, _T("SIZE_X_mm"));
    //m_comboSetDefectType.InsertString(1, _T("SIZE_Y_mm"));
    m_comboSetDefectType.InsertString(0, _T("Area"));
    m_comboSetDefectType.InsertString(1, _T("RATIO_Area"));
    m_comboSetDefectType.InsertString(2, _T("RATIO_XY"));
    m_comboSetDefectType.InsertString(3, _T("RATIO_YX"));
    m_comboSetDefectType.InsertString(4, _T("Width"));
    m_comboSetDefectType.InsertString(5, _T("Length"));
    m_comboSetDefectType.InsertString(6, _T("RATIO_WL"));
    m_comboSetDefectType.InsertString(7, _T("RATIO_LW"));
    m_comboSetDefectType.InsertString(8, _T("MIN_GV"));
    m_comboSetDefectType.InsertString(9, _T("AVG_GV"));
    m_comboSetDefectType.InsertString(10, _T("MAX_GV"));
    m_comboSetDefectType.InsertString(11, _T("BRIGHT_AVG_GV"));
    m_comboSetDefectType.InsertString(12, _T("DARK_AVG_GV"));
    m_comboSetDefectType.InsertString(13, _T("MASS_CENTER_GV"));
    m_comboSetDefectType.InsertString(14, _T("EDGE_ENERGY"));
    m_comboSetDefectType.InsertString(15, _T("EDGE_ENERGY_BLACK"));
    m_comboSetDefectType.InsertString(16, _T("EDGE_ENERGY_WHITE"));
    m_comboSetDefectType.InsertString(17, _T("SECTION_THRESHOLD"));
    m_comboSetDefectType.InsertString(18, _T("HOR"));
    m_comboSetDefectType.InsertString(19, _T("VER"));
    m_comboSetDefectType.InsertString(20, _T("RATIO_HV"));
    m_comboSetDefectType.InsertString(21, _T("RATIO_VH"));
    m_comboSetDefectType.InsertString(22, _T("RATIO_WHITE"));
    m_comboSetDefectType.InsertString(23, _T("RATIO_DARK"));
    m_comboSetDefectType.InsertString(24, _T("250>CNT"));
    m_comboSetDefectType.InsertString(25, _T("DISTANCE_TO_EDGE"));
    //m_comboSetDefectType.InsertString(26, _T("DISTANCE_TO_EDGE_2"));
    m_comboSetDefectType.InsertString(26, _T("IS_NEAR_EDGE"));
    m_comboSetDefectType.InsertString(27, _T("EDGE_ENERGY_SHARPNESS"));
    m_comboSetDefectType.InsertString(28, _T("EDGE_ENERGY_SHARPNESS_WHITE"));
    m_comboSetDefectType.InsertString(29, _T("EDGE_ENERGY_SHARPNESS_BLACK"));
    //m_comboSetDefectType.InsertString(28, _T("ORAND"));
    //m_comboSetDefectType.InsertString(2, _T("Inspection_Area"));
    //m_comboSetDefectType.InsertString(29, _T("Inspection_ImageType"));

    m_listCtrl.InsertColumn(0, _T("Index"), LVCFMT_LEFT, rtTotalList.Width() *0.1);
    m_listCtrl.InsertColumn(1, _T("조건"), LVCFMT_LEFT, rtTotalList.Width() * 0.9);

    CString szSpec;
    m_comboSetDefectType.SetCurSel(0);
    GetDlgItemText(IDC_COMBO_SET_DEFECTTYPE, szSpec);

    IninCombo();
    CString szCmp;
    m_comboSetCompare.SetCurSel(0);
    GetDlgItemText(IDC_COMBO_SET_COMPARE, szCmp);

    CString szValue;
    SetDlgItemText(IDC_EDIT_SET_VALUE, _T(""));
    
    StringTokenDivideToCString(m_strTotalCondition, '&', m_vecStrDivCondition);

    if (m_vecStrDivCondition.size() == 1)
    {
        if (m_vecStrDivCondition[0] == _T(""))
        {
            m_vecStrDivCondition.pop_back();
            GetDlgItem(IDC_BTN_DELETE_CONDITION)->EnableWindow(FALSE);
            GetDlgItem(IDC_BTN_APPLY)->EnableWindow(FALSE);
            return TRUE;
        }
    }
    if (m_vecStrDivCondition.size() < 1)
    {
        m_vecStrDivCondition.push_back(m_strTotalCondition);
    }

    for (int n = 0; n < m_vecStrDivCondition.size(); n++)
    {
        if (m_vecStrDivCondition[n] == _T(""))
        {
            m_vecStrDivCondition.erase(m_vecStrDivCondition.begin() + n);
            continue;
        }
        m_vecStrDivCondition[n].Trim();
        CString strNum;
        strNum.Format(_T("%d"), n);
        m_listCtrl.InsertItem(n, strNum);
        m_listCtrl.SetItem(n, 1, LVIF_TEXT, m_vecStrDivCondition[n], 0, 0, 0, NULL);
    }

    m_nListCount = m_vecStrDivCondition.size();
    
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgEditClassification::StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue)
{
    long nDivideTotalCount = 0;
    CString strSub;
    while (FALSE != AfxExtractSubString(strSub, strFull, nDivideTotalCount++, chDivide))
    {
        vecstrValue.push_back(strSub);
    }
    return TRUE;
}

void CDlgEditClassification::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    GetClientRect(m_nDlgSize);

}

void CDlgEditClassification::OnBnClickedBtnAddAndCondtion()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다. 
    
    // 추가
    CString strGetEdit;
    GetDlgItemText(IDC_EDIT_AND_CONDITION, strGetEdit);

    if (strGetEdit.GetLength() > 0)
    {
        if (m_strDefectType == _T("Inspection_Area") || m_strDefectType == _T("Inspection_ImageType") || m_strDefectType == _T("ORAND"))
        {
            if (m_strDefectType.GetLength() > 0 && m_strCompare.GetLength() > 0)
            {
                m_strAddCondition = m_strDefectType + m_strCompare;
                m_vecStrDivCondition.push_back(m_strAddCondition);

                CString strListCnt;
                strListCnt.Format(_T("%d"), m_nListCount);
                m_listCtrl.InsertItem(m_nListCount, strListCnt);
                m_listCtrl.SetItem(m_nListCount, 1, LVIF_TEXT, m_strAddCondition, 0, 0, 0, NULL);
            }
        }
        else
        {
            if (m_strDefectType.GetLength() > 0 && m_strCompare.GetLength() > 0 && m_strSpecValue.GetLength())
            {
                m_strAddCondition = m_strDefectType + m_strCompare + m_strSpecValue;
                m_vecStrDivCondition.push_back(m_strAddCondition);

                CString strListCnt;
                strListCnt.Format(_T("%d"), m_nListCount);
                m_listCtrl.InsertItem(m_nListCount, strListCnt);
                m_listCtrl.SetItem(m_nListCount, 1, LVIF_TEXT, m_strAddCondition, 0, 0, 0, NULL);
            }
        }

        GetDlgItem(IDC_BTN_DELETE_CONDITION)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_APPLY)->EnableWindow(TRUE);
    }

    m_nListCount = m_vecStrDivCondition.size();
    SetDlgItemText(IDC_EDIT_AND_CONDITION, _T(""));
    m_nSelectIndex = -1;

    /*CString strListCnt;
    strListCnt.Format(_T("%d"), m_nListCount);
    m_listCtrl.InsertItem(m_nListCount, strListCnt);
    m_listCtrl.SetItem(m_nListCount, 1, LVIF_TEXT, m_strAddCondition, 0, 0, 0, NULL);
    m_nListCount += 1;

    SetDlgItemText(IDC_EDIT_AND_CONDITION, _T(""));

   */

    
}




void CDlgEditClassification::OnCbnSelchangeComboSetDefecttype()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int index = m_comboSetDefectType.GetCurSel();
    m_comboSetDefectType.GetLBText(index, m_strDefectType);
    IninCombo();
    SetTotalCondtion();
    
    // 함수 발생 --> IDC_EDIT_AND_CONDITION 여기에 값을 넣어줄꺼야;;;;

}

void CDlgEditClassification::OnCbnSelchangeComboSetCompare()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int index = m_comboSetCompare.GetCurSel();
    int TypeIndex = m_comboSetDefectType.GetCurSel();

    CString strComp;
    m_comboSetCompare.GetLBText(index, strComp);

    CString strType;
    m_comboSetDefectType.GetLBText(TypeIndex, strType);

    if (strType == _T("Inspection_Area") || strType == _T("Inspection_ImageType") || strType == _T("ORAND"))
    {
        m_strCompare = _T(" : ") + strComp;
    }
    else
    {
        m_strCompare = strComp;
    }
    

    SetTotalCondtion();
}

void CDlgEditClassification::OnEnChangeEditSetValue()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CDialogEx::OnInitDialog() 함수를 재지정
    //하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
    // 이 알림 메시지를 보내지 않습니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetDlgItemText(IDC_EDIT_SET_VALUE, m_strSpecValue);
    //SetTotalCondtion();
}

void CDlgEditClassification::SetTotalCondtion()
{
    m_strAddCondition = _T("");
    m_strAddCondition = m_strDefectType + m_strCompare + m_strSpecValue;
    SetDlgItemText(IDC_EDIT_AND_CONDITION, m_strAddCondition);

    if (m_nSelectIndex >= 0)
    {
        m_listCtrl.SetItemState(m_nSelectIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
        m_listCtrl.EnsureVisible(m_nSelectIndex, false);
        m_listCtrl.SetFocus();
    }
}

void CDlgEditClassification::IninCombo()
{
    if (m_strDefectType == _T("Inspection_Area"))
    {
        m_comboSetCompare.ResetContent();
        m_comboSetCompare.InsertString(0, _T("None"));
        m_comboSetCompare.InsertString(1, _T("Burr"));
        m_comboSetCompare.InsertString(2, _T("Scrab"));
        m_comboSetCompare.InsertString(3, _T("Tab"));
        m_comboSetCompare.InsertString(4, _T("Insulation"));
        m_comboSetCompare.InsertString(5, _T("Electrode Edge"));
        m_comboSetCompare.InsertString(6, _T("Electrode"));

        SetDlgItemText(IDC_EDIT_SET_VALUE, _T(""));
        m_strSpecValue = _T("");
        m_editSetValue.SetReadOnly();
    }

    else if (m_strDefectType == _T("Inspection_ImageType"))
    {
        m_comboSetCompare.ResetContent();
        m_comboSetCompare.InsertString(0, _T("Bright"));
        m_comboSetCompare.InsertString(1, _T("Dark"));
        
        SetDlgItemText(IDC_EDIT_SET_VALUE, _T(""));
        m_strSpecValue = _T("");
        m_editSetValue.SetReadOnly();
    }
    //else if (m_strDefectType == _T("ORAND"))
    //{
    //    m_comboSetCompare.ResetContent();
    //    m_comboSetCompare.InsertString(0, _T("OR"));
    //    m_comboSetCompare.InsertString(1, _T("AND"));
    //    
    //    SetDlgItemText(IDC_EDIT_SET_VALUE, _T(""));
    //    m_strSpecValue = _T("");
    //    m_editSetValue.SetReadOnly();
    //}
    else
    {
        // 조건 셋팅
        m_comboSetCompare.ResetContent();
        m_comboSetCompare.InsertString(0,_T(" (작)> "));
        m_comboSetCompare.InsertString(1,_T(" (크)< "));
        //m_comboSetCompare.InsertString(2,_T(" = "));

        m_editSetValue.SetReadOnly(0);
    }
}

void CDlgEditClassification::OnBnClickedBtnDeleteCondition()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_nSelectIndex < 0)
        return;

    m_listCtrl.DeleteItem(m_nSelectIndex);
    m_vecStrDivCondition.erase(m_vecStrDivCondition.begin() + m_nSelectIndex);
    m_nListCount = m_vecStrDivCondition.size();

    m_nSelectIndex = -1;

    initList();
}

void CDlgEditClassification::OnNMClickListTotalCondition(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;
    NM_LISTVIEW* pNMView = (NM_LISTVIEW*)pNMHDR;
    m_nSelectIndex = pNMView->iItem;

    m_listCtrl.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    m_listCtrl.SetItemState(m_nSelectIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
    //m_listCtrl.EnsureVisible(m_nSelectIndex, false);
    //m_listCtrl.SetFocus();

    if (m_nSelectIndex >= 0)
    {
        CString strListItem = m_listCtrl.GetItemText(m_nSelectIndex, 1);

        CurSelectComboSet(m_nSelectIndex);
        SetDlgItemText(IDC_EDIT_AND_CONDITION, strListItem);
    }
}

void CDlgEditClassification::CurSelectComboSet(long nCursel)
{
    CString strListItem = m_listCtrl.GetItemText(nCursel, 1);

    CString strDefectCondition = strListItem.Mid(0, strListItem.Find(' '));
    AfxExtractSubString(strDefectCondition, strListItem, 0, ' '); 
    CString strDefectCompare = strListItem.Mid(strDefectCondition.GetLength()+1, strListItem.Find(')'));
    AfxExtractSubString(strDefectCompare, strListItem, 1, ' '); 
    CString strDefectValue = strListItem.Mid(strListItem.ReverseFind(' '), strListItem.GetLength());
    AfxExtractSubString(strDefectValue, strListItem, 2, ' '); 


    CString strTemp;
    strTemp.Format(_T(" %s "), strDefectCompare);

    for (long Idx = 0; Idx < m_comboSetDefectType.GetCount(); Idx++)
    {
        CString strCompare;
        m_comboSetDefectType.GetLBText(Idx, strCompare);
        if (strDefectCondition == strCompare)
        {
            m_comboSetDefectType.SetCurSel(Idx);
            m_comboSetDefectType.GetLBText(Idx, m_strDefectType);
        }
    }

    for (long Idx = 0; Idx < m_comboSetCompare.GetCount(); Idx++)
    {
        CString strCompare;
        m_comboSetCompare.GetLBText(Idx, strCompare);
        if (strTemp == strCompare)
        {
            m_comboSetCompare.SetCurSel(Idx);
            m_comboSetCompare.GetLBText(Idx, m_strCompare);
        }
    }

    SetDlgItemText(IDC_EDIT_SET_VALUE, strDefectValue);
    m_strSpecValue = strDefectValue;
}

void CDlgEditClassification::OnBnClickedBtnApply()
    {
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_strSetTotal = _T("");
    for (int n = 0; n < m_listCtrl.GetItemCount(); n++)
    {
        if (n == m_listCtrl.GetItemCount() - 1)
        {
            CString strListItem = m_listCtrl.GetItemText(n, 1);

            m_strSetTotal += strListItem;
        }
        else
        {
            CString strListItem = m_listCtrl.GetItemText(n, 1);

            m_strSetTotal += strListItem + _T(" & ");
        }
        
    }

    //CString str = CommonDefectConditionSet();

    m_parent->m_vecStrCondition[m_parent->m_nIndex-1] = m_strSetTotal;
    m_parent->updateGrid();  
    CDialogEx::OnOK();
}

CString CDlgEditClassification::CommonDefectConditionSet()
{
    CString str;
    std::vector<CString> vecTemp;
    StringTokenDivideToCString(m_strSetTotal,'&',vecTemp);

    for (long nVal = 0; nVal < vecTemp.size(); nVal++)
    {
        for (long nSub = 0; nSub < vecTemp.size(); nSub++)
        {
            if (vecTemp[nVal] == vecTemp[nSub])
            {
                continue;
            }
            CString strOri = vecTemp[nVal].Mid(0,vecTemp[nVal].ReverseFind('('));
            CString strCompare = vecTemp[nSub].Mid(0,vecTemp[nSub].ReverseFind('('));

            if (strOri == strCompare)
            {
                CString strCompareOperator_Ori = vecTemp[nVal].Mid(vecTemp[nVal].ReverseFind('(') , vecTemp[nVal].ReverseFind(' '));
                CString strCompareOperator_Sub = vecTemp[nSub].Mid(vecTemp[nSub].ReverseFind('(') , vecTemp[nSub].ReverseFind(' '));

                CString strValue_Ori = vecTemp[nVal].Mid(vecTemp[nVal].ReverseFind(' ') + 1, vecTemp[nVal].GetLength());
                CString strValue_Sub = vecTemp[nSub].Mid(vecTemp[nSub].ReverseFind(' ') + 1, vecTemp[nSub].GetLength());

                if (strCompareOperator_Ori == strCompareOperator_Sub)
                {

                }
            }
        }
        //vecTemp[n];
    }

    return str;
}

void CDlgEditClassification::OnBnClickedBtnTransCondition()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_nSelectIndex >= 0)
    {
        m_listCtrl.SetSelectionMark(m_nSelectIndex);
        m_strAddCondition = m_strDefectType + m_strCompare + m_strSpecValue;
        m_listCtrl.SetItem(m_nSelectIndex, 1, LVIF_TEXT, m_strAddCondition, 0, 0, 0, NULL);
    }

}

void CDlgEditClassification::initList()
{
    m_listCtrl.DeleteAllItems();
    
    for (int n = 0; n < m_vecStrDivCondition.size(); n++)
    {

        CString strIndex;
        strIndex.Format(_T("%d"), n);
        m_listCtrl.InsertItem(n, strIndex);
        m_listCtrl.SetItem(n, 1, LVIF_TEXT, m_vecStrDivCondition[n], 0, 0, 0, NULL);
    }
    m_nListCount = m_vecStrDivCondition.size();
}

void CDlgEditClassification::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


    m_listCtrl.SetItemState(m_nSelectIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
    m_listCtrl.SetFocus();
    CDialogEx::OnLButtonDown(nFlags, point);



}

BOOL CDlgEditClassification::PreTranslateMessage(MSG* pMsg)
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

void CDlgEditClassification::OnEnKillfocusEditSetValue()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    if (m_strSpecValue.GetLength() > 0)
        SetTotalCondtion();
}
