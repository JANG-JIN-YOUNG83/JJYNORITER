// DlgAjinDIO.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "IBCIDlg.h"
#include "DlgAjinDIO.h"
#include "afxdialogex.h"
#include "AJin/AXD.h"
#include "AJin/AXL.h"
#include "../Module/CustomControl/CustomButton.h"
#include "../Module/CustomControl/CustomStatic.h"
#include "../SyncPci/Syncboard.h"
#include "../Grab/CISComm.h"
#include "CellImageMerger.h"

// DlgAjinDIO 대화 상자

IMPLEMENT_DYNAMIC(DlgAjinDIO, CDialogEx)

DlgAjinDIO::DlgAjinDIO(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DIO, pParent)
{
    m_nMachine = 0;
    vec_DIO.resize(BtnDIO_COUNT);
    for (long nIndex = 0; nIndex < vec_DIO.size(); nIndex++)
    {
        vec_DIO[nIndex] = 0;
    }
    //DWORD dWord = AxlOpenNoReset(7);

    CustomControl();
}

DlgAjinDIO::~DlgAjinDIO()
{
    delete m_pButtonPowerOn;
    delete m_pButtonPowerOff;
    delete m_pButtonTopFwd;
    delete m_pButtonTopBwd;
    delete m_pButtonBackFwd;
    delete m_pButtonBackBwd;
    delete m_pButtonEncodeUp;
    delete m_pButtonEncodeDown;
    delete m_pButtonBlow1;
    delete m_pButtonBlow2;
    delete m_pButtonBlowOff;
    delete m_pButtonBlowOff2;
    delete m_pStaticIOAlarm;
}

void DlgAjinDIO::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_BUTTON_POWERON, *m_pButtonPowerOn);
    DDX_Control(pDX, IDC_BUTTON_POWEROFF, *m_pButtonPowerOff);
    DDX_Control(pDX, IDC_BUTTON_TOP_FWD, *m_pButtonTopFwd);
    DDX_Control(pDX, IDC_BUTTON_TOP_BWD, *m_pButtonTopBwd);
    DDX_Control(pDX, IDC_BUTTON_BACK_FWD, *m_pButtonBackFwd);
    DDX_Control(pDX, IDC_BUTTON_BACK_BWD, *m_pButtonBackBwd);
    DDX_Control(pDX, IDC_BUTTON_ENCODE_UP, *m_pButtonEncodeUp);
    DDX_Control(pDX, IDC_BUTTON_ENCODE_DOWN, *m_pButtonEncodeDown);
    DDX_Control(pDX, IDC_BUTTON_BLOW_1, *m_pButtonBlow1);
    DDX_Control(pDX, IDC_BUTTON_BLOW_2, *m_pButtonBlow2);
    DDX_Control(pDX, IDC_BUTTON_BLOW_OFF, *m_pButtonBlowOff);
    DDX_Control(pDX, IDC_BUTTON_BLOW_OFF2, *m_pButtonBlowOff2);
    DDX_Control(pDX, IDC_STATIC_IO_ALARM, *m_pStaticIOAlarm);
    CDialogEx::DoDataExchange(pDX);
   
}


BEGIN_MESSAGE_MAP(DlgAjinDIO, CDialogEx)
ON_WM_TIMER()
ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_Sitting1, IDC_RADIO_RollPress2, &DlgAjinDIO::RadioCtrl)
ON_BN_CLICKED(IDC_BUTTON_POWERON, &DlgAjinDIO::OnBnClickedButtonPoweron)
ON_BN_CLICKED(IDC_BUTTON_POWEROFF, &DlgAjinDIO::OnBnClickedButtonPoweroff)
ON_BN_CLICKED(IDC_BUTTON_TOP_FWD, &DlgAjinDIO::OnBnClickedButtonTopFwd)
ON_BN_CLICKED(IDC_BUTTON_TOP_BWD, &DlgAjinDIO::OnBnClickedButtonTopBwd)
ON_BN_CLICKED(IDC_BUTTON_ENCODE_DOWN, &DlgAjinDIO::OnBnClickedButtonEncodeDown)
ON_BN_CLICKED(IDC_BUTTON_ENCODE_UP, &DlgAjinDIO::OnBnClickedButtonEncodeUp)
ON_BN_CLICKED(IDC_BUTTON_BACK_BWD, &DlgAjinDIO::OnBnClickedButtonBackBwd)
ON_BN_CLICKED(IDC_BUTTON_BACK_FWD, &DlgAjinDIO::OnBnClickedButtonBackFwd)
ON_BN_CLICKED(IDC_BUTTON_BLOW_1, &DlgAjinDIO::OnBnClickedButtonBlow1)
ON_BN_CLICKED(IDC_BUTTON_BLOW_2, &DlgAjinDIO::OnBnClickedButtonBlow2)
ON_BN_CLICKED(IDC_BUTTON_BLOW_OFF, &DlgAjinDIO::OnBnClickedButtonBlowOff)
ON_BN_CLICKED(IDC_BUTTON_BLOW_OFF2, &DlgAjinDIO::OnBnClickedButtonBlowOff2)
END_MESSAGE_MAP()


// DlgAjinDIO 메시지 처리기

BOOL DlgAjinDIO::OnInitDialog()
{
    CDialogEx::OnInitDialog();
 
       
    m_nRetry = 1;
    m_nRetry = g_systemParameter.m_nIORetry;
    SetTimer(100, 100, NULL);
    //InPutIOCheck();
    
    // TODO:  여기에 추가 초기화 작업을 추가합니다.

    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void DlgAjinDIO::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == 100)
    {
        KillTimer(100);
        InPutIOCheck();
        SetTimer(100, 100, NULL);
    }
    
    CDialogEx::OnTimer(nIDEvent);
}

void DlgAjinDIO::RadioCtrl(UINT nRadioNo)
{
    switch (nRadioNo)
    {
        case IDC_RADIO_Sitting1:   
            m_nMachine = machine::Machine_Sitting_TOPLAMI;
            break;
        case IDC_RADIO_Sitting_mismatch:
            m_nMachine = machine::Machine_Sitting_TOPMISMATCH;
            break;
        case IDC_RADIO_Sitting2:
            m_nMachine = machine::Machine_Sitting_2;
            break;
        case IDC_RADIO_RollPress1:
            m_nMachine = machine::RollPress_Surface;
            break;
        case IDC_RADIO_RollPress2:
            m_nMachine = machine::RollPress_Pinhole;
            break;
        default:
            break;
    }

    UIDisplay(m_nMachine);
    MachineBtnSetting(m_nMachine);
}

void DlgAjinDIO::OnBnClickedButtonPoweron()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwPowerOn;
    AxdiReadInportBit(0, vec_DIO[DIO_POWEROFF], &dwPowerOn);
    if (dwPowerOn == 1)
    {
        AxdoWriteOutport(vec_DIO[DIO_POWEROFF], 0);
    }
    AxdoWriteOutport(vec_DIO[DIO_POWERON], 1);

    AxdiReadInport(vec_DIO[DIO_POWERON], &dwPowerOn);
    if (m_nRetry > 1 && dwPowerOn == 0)
    {
        long nAdress = vec_DIO[DIO_POWERON];
        bRet = IORetry(nAdress);
        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_POWER_ON"));
        }
    }
}

void DlgAjinDIO::OnBnClickedButtonPoweroff()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwPowerOff;
    AxdiReadInportBit(0, vec_DIO[DIO_POWERON], &dwPowerOff);
    if (dwPowerOff == 1)
    {
        AxdoWriteOutport(vec_DIO[DIO_POWERON], 0);
    }
    AxdoWriteOutport(vec_DIO[DIO_POWEROFF], 1);

    AxdiReadInportBit(0,vec_DIO[DIO_POWEROFF], &dwPowerOff);
    if (m_nRetry > 1 && dwPowerOff == 0)
    {
        bRet = IORetry(vec_DIO[DIO_POWEROFF]);
        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_POWER_OFF"));
        }
    }
}
void DlgAjinDIO::OnBnClickedButtonTopFwd()
{
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwTopFwd;
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다
    AxdiReadInportBit(0, vec_DIO[DIOTOPCYLINDERDOWN], &dwTopFwd);
    if (dwTopFwd == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOTOPCYLINDERDOWN], 0);
    }
    AxdoWriteOutport(vec_DIO[DIOTOPCYLINDERUP], 1);

    AxdiReadInport(vec_DIO[DIOTOPCYLINDERUP], &dwTopFwd);
    if (m_nRetry > 1 && dwTopFwd == 0)
    {
        bRet = IORetry(vec_DIO[DIOTOPCYLINDERUP]);

        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_TOP_CYLINDER_UP"));
        }
        // 조명끄기
    }
    if (bRet == 1)
    {
        CIBCIDlg* dlg = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
        dlg->Illum_Off();
        //BOOL bIllum = FALSE;
        //m_pSyncIO->SetSignal_HandlerOutput(SyncboardOutput::Illumination, bIllum);
        //m_pSyncIO->LiveStartSyncboard();
    }
}
void DlgAjinDIO::OnBnClickedButtonTopBwd()
{
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwTopBwd;
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다
    AxdiReadInportBit(0, vec_DIO[DIOTOPCYLINDERUP], &dwTopBwd);
    if (dwTopBwd == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOTOPCYLINDERUP], 0);
    }

    AxdoWriteOutport(vec_DIO[DIOTOPCYLINDERDOWN], 1);

    AxdiReadInport(vec_DIO[DIOTOPCYLINDERDOWN], &dwTopBwd);
    if (m_nRetry > 1 && dwTopBwd == 0)
    {
        bRet = IORetry(vec_DIO[DIOTOPCYLINDERDOWN]);
        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_TOP_CYLINDER_DOWN"));
        }
        // 조명끄기
    }
    if (dwTopBwd == 1 && bRet == 1)
    {
        CIBCIDlg* dlg = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
        dlg->Illum_On();
    }
}

void DlgAjinDIO::OnBnClickedButtonEncodeDown()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwEncordDown = 0;
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다
    AxdiReadInportBit(0, vec_DIO[DIOENCODERUP], &dwEncordDown);
    if (dwEncordDown == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOENCODERUP], 0);
    }

    AxdoWriteOutport(vec_DIO[DIOENCODERDOWN], 1);

    AxdiReadInport(vec_DIO[DIOENCODERDOWN], &dwEncordDown);
    if (m_nRetry > 1 && dwEncordDown == 0)
    {
        bRet = IORetry(vec_DIO[DIOENCODERDOWN]);
        // 조명끄기
        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_ENCODER_DOWN"));
        }
    }
}

void DlgAjinDIO::OnBnClickedButtonEncodeUp()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwEncordUp;
    AxdiReadInportBit(0, vec_DIO[DIOENCODERDOWN], &dwEncordUp);
    if (dwEncordUp == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOENCODERDOWN], 0);
    }

    AxdoWriteOutport(vec_DIO[DIOENCODERUP], 1);
    
    AxdiReadInportBit(0,vec_DIO[DIOENCODERUP], &dwEncordUp);
    if (m_nRetry > 1 && dwEncordUp == 0)
    {
        bRet = IORetry(vec_DIO[DIOENCODERUP]);

        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_ENCODER_UP"));
        }
    }

        //AxdoWriteOutport(vec_DIO[DIOENCODERUP], 1);
}

void DlgAjinDIO::OnBnClickedButtonBackBwd()
{
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwBackBwd;
    AxdiReadInportBit(0, vec_DIO[DIOBACKCYLINDERUP], &dwBackBwd);
    if (dwBackBwd == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOBACKCYLINDERUP], 0);
    }

    AxdoWriteOutport(vec_DIO[DIOBACKCYLINDERDOWN], 1);

    AxdiReadInport(vec_DIO[DIOBACKCYLINDERDOWN], &dwBackBwd);
   
    if (m_nRetry > 1 && dwBackBwd == 0)
    {
        bRet = IORetry(vec_DIO[DIOBACKCYLINDERDOWN]);

        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_BACK_CYLINDER_DOWN"));
        }

    }

    if (bRet == 1)
    {
        //m_Parents->Illum_Off();
        CString strSend;
        strSend.Format(_T("IllumOff"));
        //m_Parents->Illum_On();
        if (g_systemParameter.bServer == 1) // Server // Top이 서버
        {
            CIBCIDlg* dlg = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
            dlg->OnSend2_S(strSend);
        }
    }
    
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void DlgAjinDIO::OnBnClickedButtonBackFwd()
{
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwBacFwd = 0;
    AxdiReadInportBit(0, vec_DIO[DIOBACKCYLINDERDOWN], &dwBacFwd);
    if (dwBacFwd == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOBACKCYLINDERDOWN], 0);
    }

    AxdoWriteOutport(vec_DIO[DIOBACKCYLINDERUP], 1);

    AxdiReadInport(vec_DIO[DIOBACKCYLINDERUP], &dwBacFwd);
    if (m_nRetry > 1 && dwBacFwd == 0)
    {
        bRet = IORetry(vec_DIO[DIOBACKCYLINDERUP]);
        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_BACK_CYLINDER_UP"));
        }
    }
    if (bRet == 1)
    {
        CString strSend;
        strSend.Format(_T("IllumOn"));
        //m_Parents->Illum_On();
        if (g_systemParameter.bServer == 1) // Server // Top이 서버 
        {
            CIBCIDlg* dlg = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
            dlg->OnSend2_S(strSend);
        }
    }
    
}

void DlgAjinDIO::OnBnClickedButtonBlow1()
{
    BOOL bRet = TRUE;
    AlarmDisplay(_T(""));
    DWORD dwBlowOn;
    AxdiReadInportBit(0, vec_DIO[DIOBLOWON], &dwBlowOn);
    if (dwBlowOn == 0)
    {
        AxdoWriteOutport(vec_DIO[DIOBLOWON], 1);
    }

    AxdiReadInport(vec_DIO[DIOBLOWON], &dwBlowOn);
    if (m_nRetry > 1 && dwBlowOn == 0)
    {
        bRet = IORetry(vec_DIO[DIOBLOWON]);
        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_BLOW_ON"));
        }
    }
}

void DlgAjinDIO::OnBnClickedButtonBlow2()
{
    BOOL bRet = TRUE;
    DWORD dwBlowOn2;
    AxdiReadInportBit(0, vec_DIO[DIOBLOW_2ON], &dwBlowOn2);
    if (dwBlowOn2 == 0)
    {
        AxdoWriteOutport(vec_DIO[DIOBLOW_2ON], 1);
    }

    AxdiReadInport(vec_DIO[DIOBLOW_2ON], &dwBlowOn2);

    if (m_nRetry > 1 && dwBlowOn2 == 0)
    {
        bRet = IORetry(vec_DIO[DIOBLOW_2ON]);
        if (bRet == FALSE)
        {
            AlarmDisplay(_T("IO_BLOW2_ON"));
        }
    }
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
   // AxdoWriteOutport(vec_DIO[DIOBLOW_2ON], 1);
}
void DlgAjinDIO::OnBnClickedButtonBlowOff()
{
    DWORD dwBlowOff;
    AxdiReadInportBit(0, vec_DIO[DIOBLOWON], &dwBlowOff);
    if (dwBlowOff == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOBLOWON], 0);
    } 

    AxdiReadInport(vec_DIO[DIOBLOWON], &dwBlowOff);
    //if (m_nRetry > 1 && dwBlowOff == 1)
    //{
    //    bRet = IORetry(vec_DIO[DIOBLOWON]);
    //}
}

void DlgAjinDIO::OnBnClickedButtonBlowOff2()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    DWORD dwBlowOff2;
    AxdiReadInportBit(0, vec_DIO[DIOBLOW_2ON], &dwBlowOff2);
    if (dwBlowOff2 == 1)
    {
        AxdoWriteOutport(vec_DIO[DIOBLOW_2ON], 0);
    }

    AxdiReadInport(vec_DIO[DIOBLOW_2ON], &dwBlowOff2);
}

void DlgAjinDIO::UIDisplay(long nMachine)
{
    if (nMachine == machine::Machine_Sitting_TOPLAMI)
    {
        GetDlgItem(IDC_BUTTON_POWERON)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_POWEROFF)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BACK_FWD)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BACK_BWD)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_TOP_FWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_TOP_BWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_UP)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_DOWN)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_1)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_2)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF2)->EnableWindow(FALSE);
    }
    else if (nMachine == machine::Machine_Sitting_TOPMISMATCH)
    {
        GetDlgItem(IDC_BUTTON_POWERON)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_POWEROFF)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BACK_FWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BACK_BWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_TOP_FWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_TOP_BWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_UP)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_DOWN)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_1)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_2)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF2)->EnableWindow(FALSE);
    }
    else if (nMachine == machine::Machine_Sitting_2)
    {
        GetDlgItem(IDC_BUTTON_POWERON)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_POWEROFF)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BACK_FWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BACK_BWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_TOP_FWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_TOP_BWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_UP)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_DOWN)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_1)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_2)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF2)->EnableWindow(FALSE);
    }
    else if (nMachine == machine::RollPress_Surface)
    {
        GetDlgItem(IDC_BUTTON_POWERON)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_POWEROFF)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BACK_FWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BACK_BWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_TOP_FWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_TOP_BWD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_UP)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ENCODE_DOWN)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_1)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_2)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF2)->EnableWindow(FALSE);
    }
    else if (nMachine == machine::RollPress_Pinhole)
    {
        GetDlgItem(IDC_BUTTON_POWERON)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_POWEROFF)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BACK_FWD)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BACK_BWD)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_TOP_FWD)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_TOP_BWD)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_ENCODE_UP)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_ENCODE_DOWN)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BLOW_1)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_2)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BLOW_OFF2)->EnableWindow(TRUE);
    }

}
void DlgAjinDIO::MachineBtnSetting(long nMachine)
{
    if (nMachine == machine::Machine_Sitting_TOPLAMI)
    {
        vec_DIO[DIOTOPCYLINDERDOWN] = 8;
        vec_DIO[DIOTOPCYLINDERUP] = 9;
        vec_DIO[DIOENCODERUP] = 10;
        vec_DIO[DIOENCODERDOWN] = 11;
    }
    else if (nMachine == machine::Machine_Sitting_TOPMISMATCH)
    {
        vec_DIO[DIOTOPCYLINDERDOWN] = 10;
        vec_DIO[DIOTOPCYLINDERUP] = 11;
        vec_DIO[DIOBACKCYLINDERUP] = 12;
        vec_DIO[DIOBACKCYLINDERDOWN] = 13;
        vec_DIO[DIOENCODERUP] = 14;
        vec_DIO[DIOENCODERDOWN] = 15;
    }
    else if (nMachine == machine::Machine_Sitting_2)
    {
        vec_DIO[DIOBLOWON] = 9;
        vec_DIO[DIOTOPCYLINDERDOWN] = 10;
        vec_DIO[DIOTOPCYLINDERUP] = 11;
        vec_DIO[DIOBACKCYLINDERUP] = 12;
        vec_DIO[DIOBACKCYLINDERDOWN] = 13;
        vec_DIO[DIOENCODERUP] = 14;
        vec_DIO[DIOENCODERDOWN] = 15;
        
    }
    else if (nMachine == machine::RollPress_Surface)
    {
        vec_DIO[DIOTOPCYLINDERDOWN] = 10;
        vec_DIO[DIOTOPCYLINDERUP] = 11;
        vec_DIO[DIOBACKCYLINDERUP] = 12;
        vec_DIO[DIOBACKCYLINDERDOWN] = 13;
        vec_DIO[DIOENCODERUP] = 14;
        vec_DIO[DIOENCODERDOWN] = 15;
    }
    else if (nMachine == machine::RollPress_Pinhole)
    {
        vec_DIO[DIO_POWERON] = 5;
        vec_DIO[DIO_POWEROFF] = 6;
        vec_DIO[DIOBLOWON] = 14;
        vec_DIO[DIOBLOW_2ON] = 15;
    }
}

void DlgAjinDIO::InPutIOCheck()
{
  
    std::vector<DWORD> dwCheck;
    DWORD dw; 
   // DIO_POWERON, DIO_POWEROFF, DIOTOPCYLINDERUP, DIOTOPCYLINDERDOWN, DIOBACKCYLINDERUP, DIOBACKCYLINDERDOWN,
     //   DIOENCODERUP, DIOENCODERDOWN, DIOBLOWON, DIOBLOW_2ON 
     dwCheck.resize(vec_DIO.size());
    for (long nIndex = 0; nIndex < dwCheck.size(); nIndex ++)
     {
        dwCheck[nIndex] = 0;
     }

     AxdiReadInportBit(0,vec_DIO[DIO_POWERON], &dwCheck[DIO_POWERON]);
     AxdiReadInportBit(0,vec_DIO[DIO_POWEROFF], &dwCheck[DIO_POWEROFF]);
     AxdiReadInportBit(0,vec_DIO[DIOTOPCYLINDERUP], &dwCheck[DIOTOPCYLINDERUP]);
     AxdiReadInportBit(0,vec_DIO[DIOTOPCYLINDERDOWN], &dwCheck[DIOTOPCYLINDERDOWN]);
     AxdiReadInportBit(0,vec_DIO[DIOBACKCYLINDERUP], &dwCheck[DIOBACKCYLINDERUP]);
     AxdiReadInportBit(0,vec_DIO[DIOBACKCYLINDERDOWN], &dwCheck[DIOBACKCYLINDERDOWN]); 
     AxdiReadInportBit(0,vec_DIO[DIOENCODERUP], &dwCheck[DIOENCODERUP]); 
     AxdiReadInportBit(0,vec_DIO[DIOENCODERDOWN], &dwCheck[DIOENCODERDOWN]); 
     AxdiReadInportBit(0,vec_DIO[DIOBLOWON], &dwCheck[DIOBLOWON]);
     AxdiReadInportBit(0,vec_DIO[DIOBLOW_2ON], &dwCheck[DIOBLOW_2ON]); 

    for (long nIndex = 0; nIndex < dwCheck.size(); nIndex++)
    {
        if (dwCheck[nIndex] == High)
        {
            if (nIndex == DIO_POWERON)
            {
                m_pButtonPowerOn->SetBackGradation(RGB(49, 191, 252), RGB(179, 202, 254), 1);
                m_pButtonPowerOn->SetDraw3D(TRUE);
            }
            else if (nIndex == DIO_POWEROFF)
            {
                m_pButtonPowerOff->SetBackColor(RGB(255, 0, 0));
            }
            else if (nIndex == DIOTOPCYLINDERUP)
            {
                m_pButtonTopFwd->SetBackColor(RGB(255, 0, 0));
            }
            else if (nIndex == DIOTOPCYLINDERDOWN)
            {
                m_pButtonTopBwd->SetBackColor(RGB(255, 0, 0));
            }
            else if (nIndex == DIOBACKCYLINDERUP)
            {
                m_pButtonBackFwd->SetBackColor(RGB(255, 0, 0));
            }
            else if (nIndex == DIOBACKCYLINDERDOWN)
            {
                m_pButtonBackBwd->SetBackColor(RGB(255, 0, 0));
            }
            else if (nIndex == DIOENCODERUP)
            {
                m_pButtonEncodeUp->SetBackColor(RGB(255, 0, 0));
            }
            else if (nIndex == DIOENCODERDOWN)
            {
                m_pButtonEncodeDown->SetBackColor(RGB(255, 0, 0));
            }
            else if (nIndex == DIOBLOWON)
            {
                m_pButtonBlow1->SetBackColor(RGB(255, 0, 0));
                m_pButtonBlowOff->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOBLOW_2ON)
            {
                m_pButtonBlow2->SetBackColor(RGB(255, 0, 0));
                m_pButtonBlowOff2->SetBackColor(RGB(255, 255, 255));
            }
        }

        if (dwCheck[nIndex] == Low)
        {
            if (nIndex == DIO_POWERON)
            {
                m_pButtonPowerOn->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIO_POWEROFF)
            {
                m_pButtonPowerOff->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOTOPCYLINDERUP)
            {
                m_pButtonTopFwd->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOTOPCYLINDERDOWN)
            {
                m_pButtonTopBwd->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOBACKCYLINDERUP)
            {
                m_pButtonBackFwd->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOBACKCYLINDERDOWN)
            {
                m_pButtonBackBwd->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOENCODERUP)
            {
                m_pButtonEncodeUp->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOENCODERDOWN)
            {
                m_pButtonEncodeDown->SetBackColor(RGB(255, 255, 255));
            }
            else if (nIndex == DIOBLOWON)
            {
                m_pButtonBlow1->SetBackColor(RGB(255, 255, 255));
                m_pButtonBlowOff->SetBackColor(RGB(255, 0, 0));

            }
            else if (nIndex == DIOBLOW_2ON)
            {
                m_pButtonBlow2->SetBackColor(RGB(255, 255, 255));
                m_pButtonBlowOff2->SetBackColor(RGB(255, 0, 0));
            }
        }
    }
    Invalidate(FALSE);
  //  Update;
}


void DlgAjinDIO::CustomControl()
{
    m_pButtonPowerOn        = new CCustomButton;
    m_pButtonPowerOff       = new CCustomButton;
    m_pButtonTopFwd         = new CCustomButton;
    m_pButtonTopBwd         = new CCustomButton;
    m_pButtonBackFwd        = new CCustomButton;
    m_pButtonBackBwd        = new CCustomButton;
    m_pButtonEncodeUp       = new CCustomButton;
    m_pButtonEncodeDown     = new CCustomButton;
    m_pButtonBlow1          = new CCustomButton;
    m_pButtonBlow2          = new CCustomButton;
    m_pButtonBlowOff        = new CCustomButton;
    m_pButtonBlowOff2       = new CCustomButton;
    m_pStaticIOAlarm        = new CCustomStatic;
}

 BOOL DlgAjinDIO::IORetry(long nAdress)
{
     DWORD dwInputVelue, dwOutVelue;
     for (long nRetry = 0; nRetry < m_nRetry; nRetry++)
     {
        if (AxdiReadInportBit(0, nAdress, &dwInputVelue) == 0 && AxdoReadOutportBit(0, nAdress, &dwOutVelue) == 1)
        {
            AxdoWriteOutport(nAdress, 0);
        }
        if (AxdoReadOutportBit(0, nAdress, &dwOutVelue) == 0)
        {
            AxdoWriteOutport(nAdress, 1);
        } 
     }
    if (AxdiReadInportBit(0, nAdress, &dwInputVelue) == 0)
     {
        return FALSE;
    }
     else
    {
         AxdoWriteOutport(nAdress, 0);
        return TRUE;
    }
    //
 }
void DlgAjinDIO::AlarmDisplay(CString strBtnIOName)
 {
     CString strAlarm;
     if (strBtnIOName == _T(""))
     {
         strAlarm.Format(_T(""));
     }
     else
     {
         strAlarm.Format(_T("%s IO Error"), strBtnIOName);
     }
     m_pStaticIOAlarm->SetWindowTextW(strAlarm);
 }
 