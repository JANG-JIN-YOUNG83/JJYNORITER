// DlgHWControl.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgHWControl.h"
#include "Ajin/DlgAjinMot.h"
#include "afxdialogex.h"
#include "../Inspection/Accessor.h"
#include "SystemParameter.h"
#include "../Grab/GrabExecutor.h"
#include "../Inspection/InspectionRoot.h"
#include "Path.h"

// DlgHWControl 대화 상자

IMPLEMENT_DYNAMIC(DlgHWControl, CDialogEx)

DlgHWControl::DlgHWControl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HWCONTROL, pParent)
{

}

DlgHWControl::~DlgHWControl()
{
}

void DlgHWControl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB_HW_CONTROL, m_Tab_HWControl);
}


BEGIN_MESSAGE_MAP(DlgHWControl, CDialogEx)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_HW_CONTROL, &DlgHWControl::OnSelchangeTabHwControl)
ON_WM_CLOSE()
END_MESSAGE_MAP()


// DlgHWControl 메시지 처리기

BOOL DlgHWControl::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    CRect rect;
    m_Tab_HWControl.GetWindowRect(rect);
    if (g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
    {
        m_Tab_HWControl.InsertItem(0, _T("Motor Move"));
        m_Tab_HWControl.InsertItem(1, _T("Motor I/O"));
    
        dlgAjinMot = new DlgAjinMot(this);
        dlgAjinMot->Create(IDD_DIALOG_MOTION, &m_Tab_HWControl);
        dlgAjinMot->MoveWindow(5, 20, rect.Width(), rect.Height());
        dlgAjinMot->ShowWindow(SW_SHOW);

        dlgAjinDIO = new DlgAjinDIO(this);
        dlgAjinDIO->Create(IDD_DIALOG_DIO, &m_Tab_HWControl);
        dlgAjinDIO->MoveWindow(5, 20, rect.Width(), rect.Height());
        dlgAjinDIO->ShowWindow(SW_HIDE);

    }
    else
    {
        m_Tab_HWControl.InsertItem(0, _T("Motor I/O"));
        dlgAjinDIO = new DlgAjinDIO;
        dlgAjinDIO->Create(IDD_DIALOG_DIO, &m_Tab_HWControl);
        dlgAjinDIO->MoveWindow(5, 20, rect.Width(), rect.Height());
        dlgAjinDIO->ShowWindow(SW_SHOW);

    }

    Database dbJob;
    CString strPath;
    strPath.Format(_T("%s\\%s.job"), Path::GetRecipeFolder(), (LPCTSTR)g_systemParameter.m_currentModel);
    dbJob.Load(strPath);
    //RecipeLoad(strPath);
    //LinkDataBase(false, dbJob);
    dlgAjinMot->LoadInspectionPos(dbJob);
    m_Tab_HWControl.SetCurSel(0);

   
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgHWControl::OnSelchangeTabHwControl(NMHDR* pNMHDR, LRESULT* pResult)
{
    long nSelect;
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (IDC_TAB_HW_CONTROL == pNMHDR->idFrom)
    {
        nSelect = m_Tab_HWControl.GetCurSel();
    }
    switch (nSelect)
    {
        case 0:
            if (g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
            {
                dlgAjinMot->ShowWindow(SW_SHOW);
                dlgAjinDIO->ShowWindow(SW_HIDE);
            }
            else
            {
                dlgAjinDIO->ShowWindow(SW_SHOW);
            }
            break;
        case 1:
            if (g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
            {
                dlgAjinMot->ShowWindow(SW_HIDE);
                dlgAjinDIO->ShowWindow(SW_SHOW);
            }
            break;

        default:
            break;
    }
    *pResult = 0;
}



void DlgHWControl::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (m_Tab_HWControl.GetCurSel() == 0 &&  g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
    {
        switch (MessageBox(_T("Insp Move Pos Save?"), _T("Message"), MB_YESNOCANCEL))
        {
            case IDCANCEL:
                return;
            case IDNO:
                CDialogEx::OnClose();
                return;
        }

        dlgAjinMot->SaveParameter(dlgAjinMot->m_nMotionID);
        CString strjobValue;
        // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
        dlgAjinMot->m_edit_Actual.GetWindowText(strjobValue);
        dlgAjinMot->m_dInspectionPos[dlgAjinMot->m_nMotionID] = wcstod(strjobValue, NULL);
        Database dbJob;
        CString strPath;
        strPath.Format(_T("%s\\%s.job"), Path::GetRecipeFolder(), (LPCTSTR)g_systemParameter.m_currentModel);
        dbJob.Load(strPath);
        dlgAjinMot->SaveInspectionPos(dbJob);
        dbJob.Save(strPath);
        //RecipeLoad(strPath);
        //LinkDataBase(true, dbJob);
       // dlgAjinMot->SaveInspectionPos(dbJob);
    }

    CDialogEx::OnClose();
}
