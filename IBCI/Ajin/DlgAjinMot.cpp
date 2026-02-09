// AjinMot.cpp: 구현 파일
//
#include "pch.h"
#include "../IBCI.h"
#include "DlgAjinMot.h"
#include "afxdialogex.h"
#include "../../CompanySharedLibrary/Persistence/IniHelper.h"
#include "AXL.h"
#include "AXM.h"
#include "AXHS.h"
#include "../Path.h"
#include "../../CompanySharedLibrary/Persistence/Database.h"
#include "../../Inspection/Accessor.h"
#include "../../Module/CustomControl/CustomStatic.h"
#include "../DlgAjinDIO.h"
#include "../SystemParameter.h"
#pragma comment(lib, "AXL.lib")

// AjinMot 대화 상자

IMPLEMENT_DYNAMIC(DlgAjinMot, CDialogEx)

DlgAjinMot::DlgAjinMot(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MOTION, pParent)
{
    m_static_LimitP1 = new CCustomStatic;
    m_static_LimitP2 = new CCustomStatic;
    m_static_LimitM1 = new CCustomStatic;
    m_static_LimitM2 = new CCustomStatic;
    m_static_Home1   = new CCustomStatic;
    m_static_Home2   = new CCustomStatic;
    m_static_Alarm   = new CCustomStatic;
}

DlgAjinMot::~DlgAjinMot()
{
    delete m_static_LimitP1;
    delete m_static_LimitP2;
    delete m_static_LimitM1;
    delete m_static_LimitM2;
    delete m_static_Home1  ;
    delete m_static_Home2  ;
    delete m_static_Alarm  ;
}

void DlgAjinMot::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_Command, m_edit_Command);
    DDX_Control(pDX, IDC_EDIT_ACTUAL, m_edit_Actual);
    //DDX_Control(pDX, IDC_COMBO_SPEED, m_combo_SpeedLevel);
    DDX_Control(pDX, IDC_EDIT_PULSE, m_edit_MovePulse);
    DDX_Control(pDX, IDC_EDIT_VELOCITY, m_edit_Vel);
    DDX_Control(pDX, IDC_EDIT_ACC, m_edit_Acc);
    DDX_Control(pDX, IDC_EDIT_DEC, m_edit_Dec);
    DDX_Control(pDX, IDC_EDIT_TARGET, m_edit_TargetPos);
    DDX_Control(pDX, IDC_STATIC_LIMIT_M_1, *m_static_LimitM1);
    DDX_Control(pDX, IDC_STATIC_LIMIT_M_2, *m_static_LimitM2);
    DDX_Control(pDX, IDC_STATIC_LIMIT_P_1, *m_static_LimitP1);
    DDX_Control(pDX, IDC_STATIC_LIMIT_P_2, *m_static_LimitP2);
    DDX_Control(pDX, IDC_STATIC_HOME, *m_static_Home1);
    DDX_Control(pDX, IDC_STATIC_HOME2, *m_static_Home2);
    DDX_Control(pDX, IDC_STATIC_ALARM, *m_static_Alarm);

}


BEGIN_MESSAGE_MAP(DlgAjinMot, CDialogEx)
ON_BN_CLICKED(IDC_BUTTON_MOVE, &DlgAjinMot::OnBnClickedButtonMove)
ON_BN_CLICKED(IDC_BUTTONHOME, &DlgAjinMot::OnBnClickedButtonhome)
ON_BN_CLICKED(IDC_BUTTON_HOMESTOP, &DlgAjinMot::OnBnClickedButtonHomestop)
//ON_EN_CHANGE(IDC_EDIT_PULSE, &DlgAjinMot::OnEnChangeEditMovepulse)
ON_BN_CLICKED(IDC_BUTTON_STOP, &DlgAjinMot::OnBnClickedButtonStop)
ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_CAM1, IDC_RADIO_CAM2, &DlgAjinMot::OnBnClickedCamRadio)

ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON_JOG_M, &DlgAjinMot::OnBnClickedButtonJogM)
ON_BN_CLICKED(IDC_BUTTON_JOG_P, &DlgAjinMot::OnBnClickedButtonJogP)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_CLOSE()
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_BUTTON_INSP_MOVE, &DlgAjinMot::OnBnClickedButtonInspMove)
ON_BN_CLICKED(IDC_BUTTON_INSP_STOP, &DlgAjinMot::OnBnClickedButtonInspStop)
ON_STN_CLICKED(IDC_STATIC_ALARM, &DlgAjinMot::OnStnClickedStaticAlarm)
END_MESSAGE_MAP()


// AjinMot 메시지 처리기

void DlgAjinMot::OnBnClickedButtonMove()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (!IsWindowVisible())
        return;

    ChangeEditMovepulse();
 
    CString strMove_TargetPos;
    m_edit_TargetPos.GetWindowText(strMove_TargetPos);
    double dTargetPos = ::_tstof(strMove_TargetPos);
    //m_nSpeedIndex = m_combo_SpeedLevel.GetCurSel();
    m_nAxis = m_radio_axis.GetCheck();

    SaveParameter(m_nAxis);
    MoveInspetion(m_nAxis, dTargetPos);
}

void DlgAjinMot::OnBnClickedButtonhome()
{
  
        SetDefault(m_nMotionID);
        LoadParameter(m_nMotionID);
        ServoOnOff(m_nMotionID, TRUE);
        MoveHoming(m_nMotionID);
        if (MoveHomeComplete(m_nMotionID) == HOME_SUCCESS)
        {
           // MoveInspetion(nIndex,0.0f);
           // if (PosMoveComplete(nIndex) == 0)
           // {
           //     // 끝
           //     ::SetEvent(m_threadControl_Motion.m_Signal_Kill);
           // }
        }
        else
        {
            //Error
        }
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void DlgAjinMot::OnBnClickedButtonHomestop()
{
    AxmMoveEStop(m_nMotionID);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
// JOB 오픈시
//BOOL DlgAjinMot::AjinLibraryInitialize()
//{
//    //DWORD dWord  = AxlOpenNoReset(7);
//    //if (AxlOpenNoReset(7) != AXT_RT_SUCCESS)
//    {
//        return FALSE;
//    }
//
//    return TRUE;
//}

BOOL DlgAjinMot::AjinLibraryClose()
{
    return AxlClose();
}

void DlgAjinMot::MoveInspetion(int nAxisNo, double dPosMM)
{
    AxmMoveStartPos(nAxisNo, dPosMM, m_dSpeedMM[nAxisNo][m_nSpeedIndex], m_dAccelMM[nAxisNo][m_nSpeedIndex], m_dDecelMM[nAxisNo][m_nSpeedIndex]);
}
BOOL DlgAjinMot::PosMoveComplete(int nAxisNo)
{
    DWORD Status;
    BOOL bRun = TRUE;
    while (bRun)
    {
        AxmStatusReadInMotion(0, &Status);
        switch (Status)
        {
            case 0:
                bRun = FALSE;
                break;
            case 1:
                Sleep(100);
                break;
        }
    }
    return Status;
}

void DlgAjinMot::MoveHoming(int nAxisNo)
{
    //축 원점 검색 결과를 사용자 BREAK로 설정
    //원점 검색이 완료 되고 나면 결과가 AXT_RT_SUCCESS 로 변환
    AxmHomeSetResult(nAxisNo, HOME_ERR_USER_BREAK);
    //(-) 방향으로 원점 검색, Home Sensor 이용, Home Sensor Type A 접점, Z상 검출 사용 안 함,
    //  Offset 이동 안 함으로 설정.
    long lHmDir = 1;
    DWORD uHmsig = 4;
    DWORD uZphas = 0;
    double dHClrTim = 2000.0, dHOffset = 0.0;
    AxmHomeSetMethod(0, lHmDir, uHmsig, uZphas, dHClrTim, dHOffset);
    AxmHomeSetVel(0, 5, 5, 5, 1, 20, 20);

    AxmHomeSetStart(nAxisNo);
}
DWORD DlgAjinMot::MoveHomeComplete(int nAxisNo)
{
    BOOL bRun = TRUE;
    DWORD uHomeResult;
    while (bRun)
    {
        //원점 검색 함수를 이용하여 성공적으로 원점 검색이 수행되고 나면 검색 결과를 출력한다.
        AxmHomeGetResult(0, &uHomeResult);
        switch (uHomeResult)
        {
            case HOME_ERR_UNKNOWN:
                bRun = FALSE;
                break;
            case HOME_ERR_GNT_RANGE:
                bRun = FALSE;
                break;
            case HOME_ERR_USER_BREAK:
                bRun = FALSE;
                break;
            case HOME_ERR_VELOCITY:
                bRun = FALSE;
                break;
            case HOME_ERR_AMP_FAULT:
                bRun = FALSE;
                break;
            case HOME_ERR_NEG_LIMIT:
                bRun = FALSE;
                break;
            case HOME_ERR_POS_LIMIT:
                bRun = FALSE;
                break;
            case HOME_SEARCHING:
                Sleep(100);
                break;
            case HOME_SUCCESS:
                bRun = FALSE;
                break;
        }
    }

    long lHmDir;
    DWORD dwHmsig, dwZphas;
    double dHClrTim, dHOffset;
    AxmHomeGetMethod(nAxisNo, &lHmDir, &dwHmsig, &dwZphas, &dHClrTim, &dHOffset);

    return uHomeResult;
}
// Teaching 창 띄우면 
BOOL DlgAjinMot::LoadParameter(int nAxisNo)
{
    CString strMotorParam = m_strMotorParam;
    BOOL bRet = TRUE;
    CString strSection, strKey;
    strSection.Format(_T("Axis_%d"), nAxisNo);
    DWORD dwMethod = 0, dwLevel = 0;
    int lMovePulse = 0;
    double dMinSpeed = 0.0f, dMaxSpeed = 0.0f;
    m_nAxis = nAxisNo;

    
    m_strAxisDescription = IniHelper::LoadSTRING(strMotorParam, strSection, _T("Description"), _T("AXIS_INSPECTOR_X1"));

    m_nMotorType
        = IniHelper::LoadINT(strMotorParam, strSection, _T("MotorType"), 0); //MOTORTYPE_SERVO, MOTORTYPE_STEP=1
    m_nDriveType
        = IniHelper::LoadINT(strMotorParam, strSection, _T("DriveType"), 0); //DriveType_RealPos, DriveType_TwoPos=1

    for (int nIndex = 0; nIndex < ENUM_MOTOR_SPEED::SPEED_END; nIndex++)
    {
        // 0 : Low, 1 : Middle, 2 : HIGH
        //롤프레스 현재 Low 만 사용
        // 근데 구별해서 저장하고 구별해서 로드 해줘야하는데
        CString strVelTemp, strDecTemp,strAccTemp;
        strVelTemp.Format(_T("MoveSpeed[%d]"), nIndex);
        strDecTemp.Format(_T("MoveDecel[%d]"), nIndex);
        strAccTemp.Format(_T("MoveAccel[%d]"), nIndex);
        m_dSpeedMM[nAxisNo][nIndex] = IniHelper::LoadINT(strMotorParam, strSection, strVelTemp, 10); //초기값
        m_dAccelMM[nAxisNo][nIndex] = IniHelper::LoadINT(strMotorParam, strSection, strAccTemp, 10);
        m_dDecelMM[nAxisNo][nIndex] = IniHelper::LoadINT(strMotorParam, strSection, strDecTemp, 10);
    }

  
    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_NEG_SIGNAL"), dwLevel);
    AxmSignalSetLimit(nAxisNo, NULL, NULL, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_POS_SIGNAL"), dwLevel);
    AxmSignalSetLimit(nAxisNo, NULL, dwLevel, NULL);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_HOME_SIGNAL"), dwLevel);
    AxmHomeSetSignalLevel(nAxisNo, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_ALARM_SIGNAL"), dwLevel);
    AxmSignalSetServoAlarm(nAxisNo, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_INPOS_SIGNAL"), dwLevel);
    AxmSignalSetInpos(nAxisNo, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_ZPHAS_SIGNAL"), dwLevel);
    AxmSignalSetZphaseLevel(nAxisNo, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_EMG_SIGNAL"), dwLevel);
    AxmSignalSetStop(nAxisNo, NULL, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_RESETALARM_SIGNAL"), dwLevel);
    AxmSignalSetServoAlarmResetLevel(nAxisNo, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("LEVEL_SERVOON_SIGNAL"), dwLevel);
    AxmSignalSetServoOnLevel(nAxisNo, dwLevel);

    IniHelper::LoadINT(strMotorParam, strSection, _T("MOVE_PULSE"), lMovePulse);
    AxmMotSetMoveUnitPerPulse(nAxisNo, NULL, lMovePulse);

    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("MIN_VELOCITY"), dMinSpeed);
    AxmMotSetMinVel(nAxisNo, dMinSpeed);

    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("MAX_VELOCITY"), dMaxSpeed);
    AxmMotSetMaxVel(nAxisNo, dMaxSpeed);

    IniHelper::LoadINT(strMotorParam, strSection, _T("PULSE_OUT_METHOD"), dwMethod);
    AxmMotSetPulseOutMethod(nAxisNo, dwMethod);

    IniHelper::LoadINT(strMotorParam, strSection, _T("ENC_INPUT_METHOD"), dwMethod);
    AxmMotSetEncInputMethod(nAxisNo, dwMethod);

    // 기본적으로 사용 안하는 것으로 한다.
    m_bUseMode = IniHelper::LoadINT(strMotorParam, strSection, _T("UseMode"), 1); //NotUse, Use=1

    // SetSpeedPercent(Ini.ReadFloat(strSection, _T("SpeedPercent"), GetSpeedPercent()));
    //   SetTimeoutPercent(Ini.ReadFloat(strSection, _T("TimeoutPercent"), GetTimeoutPercent()));

    long lHmDir;
    DWORD dwHmsig, dwZphas;
    double dHClrTim, dHOffset;
    AxmHomeGetMethod(nAxisNo, &lHmDir, &dwHmsig, &dwZphas, &dHClrTim, &dHOffset);
    IniHelper::LoadINT(strMotorParam, strSection, _T("HOME_SEARCH_DIR"), lHmDir);
    IniHelper::LoadINT(strMotorParam, strSection, _T("HOME_SEARCH_SENSOR"), dwHmsig);
    IniHelper::LoadINT(strMotorParam, strSection, _T("HOME_SEARCH_ZPHAS"), dwZphas);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_CLRTIME"), dHClrTim);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_OFFSET"), dHOffset);
    AxmHomeSetMethod(nAxisNo, lHmDir, dwHmsig, dwZphas, dHClrTim, dHOffset);

    double dVelFirst, dVelSecond, dVelThird, dVelLast, dAccFirst, dAccSecond;
    AxmHomeGetVel(nAxisNo, &dVelFirst, &dVelSecond, &dVelThird, &dVelLast, &dAccFirst, &dAccSecond);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_FIRST"), dVelFirst);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_SECOND"), dVelSecond);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_THIRD"), dVelThird);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_LAST"), dVelLast);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_ACC_FIRST"), dAccFirst);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_ACC_SECOND"), dAccSecond);

    AxmHomeSetVel(nAxisNo, dVelFirst, dVelSecond, dVelThird, dVelLast, dAccFirst, dAccSecond);

    DWORD dwUse, dwStopMode, dwSelection;
    double dPositivePos, dNegativePos;
    AxmSignalGetSoftLimit(nAxisNo, &dwUse, &dwStopMode, &dwSelection, &dPositivePos, &dNegativePos);
    IniHelper::LoadINT(strMotorParam, strSection, _T("SOFT_LIMIT_USE"), dwUse);
    IniHelper::LoadINT(strMotorParam, strSection, _T("SOFT_LIMIT_STOPMODE"), dwStopMode);
    IniHelper::LoadINT(strMotorParam, strSection, _T("SOFT_LIMIT_SELECTION"), dwSelection);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("SOFT_LIMIT_POS"), dPositivePos);
    IniHelper::LoadDOUBLE(strMotorParam, strSection, _T("SOFT_LIMIT_NEG"), dNegativePos);
    AxmSignalSetSoftLimit(nAxisNo, dwUse, dwStopMode, dwSelection, dPositivePos, dNegativePos);

    DWORD dwProfileMode;
    AxmMotGetProfileMode(nAxisNo, &dwProfileMode);
    AxmMotSetProfileMode(nAxisNo, dwProfileMode);

    return bRet;
}
BOOL DlgAjinMot::SaveParameter(int nAxisNo)
{
    CString strMotorParam = m_strMotorParam;
    int nTemp;
    double dTemp;
    CString strTemp;
    BOOL bRet = TRUE;
    CString strSection, strKey;
    DWORD dwLevel, dwMethod, dwProfileMode;
    long lMovePulse;
    double dMinSpeed;
    double dMaxSpeed;
    ChangeAcceleration();
    ChangeDeceleration();
    ChangeVelocity();

    strSection.Format(_T("Axis_%d"), nAxisNo);
    m_nAxis = nAxisNo;
    if (nAxisNo == 0)
    {
        m_strAxisDescription = _T("AXIS_INSPECTOR_X1");
    }
    else if (nAxisNo == 1)
    {
        m_strAxisDescription = _T("AXIS_INSPECTOR_X2");
    }

    IniHelper::SaveSTRING(strMotorParam, strSection, _T("Description"), m_strAxisDescription);
    IniHelper::SaveINT(strMotorParam, strSection, _T("MotorType"), m_nMotorType); //MOTORTYPE_SERVO, MOTORTYPE_STEP=1
    IniHelper::SaveINT(
        strMotorParam, strSection, _T("DriveType"), m_nDriveType); //DriveType_RealPos, DriveType_TwoPos=1
    IniHelper::SaveINT(strMotorParam, strSection, _T("UseMode"), 1); //NotUse, Use=1

    for (int nIndex = 0; nIndex < ENUM_MOTOR_SPEED::SPEED_END; nIndex++)
    {
        // 0 : Low, 1 : Middle, 2 : HIGH
        //롤프레스 현재 Low 만 사용
        // 근데 구별해서 저장하고 구별해서 로드 해줘야하는데
        strKey.Format(_T("[%d]"), nIndex);
        IniHelper::SaveINT(strMotorParam, strSection, _T("MoveSpeed") + strKey, m_dSpeedMM[nAxisNo][nIndex]); //초기값
        IniHelper::SaveINT(strMotorParam, strSection, _T("MoveAccel") + strKey, m_dAccelMM[nAxisNo][nIndex]);
        IniHelper::SaveINT(strMotorParam, strSection, _T("MoveDecel") + strKey, m_dDecelMM[nAxisNo][nIndex]);
    }

    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("SpeedPercent"), 1);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("TimeoutPercent"), 1);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("SoftPosErrorLimit"), 100);

    AxmSignalGetLimit(nAxisNo, NULL, NULL, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_NEG_SIGNAL"), dwLevel);

    AxmSignalGetLimit(nAxisNo, NULL, &dwLevel, NULL);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_POS_SIGNAL"), dwLevel);

    AxmHomeGetSignalLevel(nAxisNo, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_HOME_SIGNAL"), dwLevel);

    AxmSignalGetServoAlarm(nAxisNo, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_ALARM_SIGNAL"), dwLevel);

    AxmSignalGetInpos(nAxisNo, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_INPOS_SIGNAL"), dwLevel);

    AxmSignalGetZphaseLevel(nAxisNo, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_ZPHAS_SIGNAL"), dwLevel);

    AxmSignalGetStop(nAxisNo, NULL, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_EMG_SIGNAL"), dwLevel);

    AxmSignalGetServoAlarmResetLevel(nAxisNo, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_RESETALARM_SIGNAL"), dwLevel);

    AxmSignalGetServoOnLevel(nAxisNo, &dwLevel);
    IniHelper::SaveINT(strMotorParam, strSection, _T("LEVEL_SERVOON_SIGNAL"), dwLevel);

    AxmMotGetMoveUnitPerPulse(nAxisNo, NULL, &lMovePulse);
    IniHelper::SaveINT(strMotorParam, strSection, _T("MOVE_PULSE"), lMovePulse);

    AxmMotGetMinVel(nAxisNo, &dMinSpeed);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("MIN_VELOCITY"), dMinSpeed);

    AxmMotGetMaxVel(nAxisNo, &dMaxSpeed);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("MAX_VELOCITY"), dMaxSpeed);

    AxmMotGetPulseOutMethod(nAxisNo, &dwMethod);
    IniHelper::SaveINT(strMotorParam, strSection, _T("PULSE_OUT_METHOD"), dwMethod);

    AxmMotGetEncInputMethod(nAxisNo, &dwMethod);
    IniHelper::SaveINT(strMotorParam, strSection, _T("ENC_INPUT_METHOD"), dwMethod);

    long lHmDir;
    DWORD dwHmsig, dwZphas;
    double dHClrTim, dHOffset;
    AxmHomeGetMethod(nAxisNo, &lHmDir, &dwHmsig, &dwZphas, &dHClrTim, &dHOffset);
    IniHelper::SaveINT(strMotorParam, strSection, _T("HOME_SEARCH_DIR"), lHmDir);
    IniHelper::SaveINT(strMotorParam, strSection, _T("HOME_SEARCH_SENSOR"), dwHmsig);
    IniHelper::SaveINT(strMotorParam, strSection, _T("HOME_SEARCH_ZPHAS"), dwZphas);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_CLRTIME"), dHClrTim);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_OFFSET"), dHOffset);

    double dVelFirst, dVelSecond, dVelThird, dVelLast, dAccFirst, dAccSecond;
    AxmHomeGetVel(nAxisNo, &dVelFirst, &dVelSecond, &dVelThird, &dVelLast, &dAccFirst, &dAccSecond);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_FIRST"), dVelFirst);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_SECOND"), dVelSecond);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_THIRD"), dVelThird);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_VEL_LAST"), dVelLast);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_ACC_FIRST"), dAccFirst);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("HOME_SEARCH_ACC_SECOND"), dAccSecond);

    DWORD dwUse, dwStopMode, dwSelection;
    double dPositivePos, dNegativePos;
    AxmSignalGetSoftLimit(nAxisNo, &dwUse, &dwStopMode, &dwSelection, &dPositivePos, &dNegativePos);
    IniHelper::SaveINT(strMotorParam, strSection, _T("SOFT_LIMIT_USE"), dwUse);
    IniHelper::SaveINT(strMotorParam, strSection, _T("SOFT_LIMIT_STOPMODE"), dwStopMode);
    IniHelper::SaveINT(strMotorParam, strSection, _T("SOFT_LIMIT_SELECTION"), dwSelection);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("SOFT_LIMIT_POS"), dPositivePos);
    IniHelper::SaveDOUBLE(strMotorParam, strSection, _T("SOFT_LIMIT_NEG"), dNegativePos);

    AxmMotGetProfileMode(nAxisNo, &dwProfileMode);
    IniHelper::SaveINT(strMotorParam, strSection, _T("MOTION_PROFILE_MODE"), dwProfileMode);


    return bRet;
}
BOOL DlgAjinMot::ServoOnOff(int nAxisNo, BOOL bOn)
{
    DWORD dwENABLE;
    if (bOn == TRUE) // Servo On
    {
        AxmSignalIsServoOn(nAxisNo, &dwENABLE);
        if (!dwENABLE)
        {
            AxmMoveEStop(nAxisNo);
            /* if (!bInitHoming)
            {
                AbortHome();
            }
            AbortRepeat();
            MatchPosition();*/

            AxmSignalServoOn(nAxisNo, TRUE);
        }
    }
    else // Servo Off
    {
        AxmSignalServoOn(nAxisNo, FALSE);

        /*if (!bInitHoming)
        {
            AbortHome();
        }
        AbortRepeat();*/
    }
    return TRUE;
}
void DlgAjinMot::SetDefault(int nAxisNo)
{
    CString strAxisName;
    strAxisName.Format(_T("Axis %d"), nAxisNo);
    if (nAxisNo == 0)
    {
        m_strAxisDescription = _T("AXIS_INSPECTOR_X1");
    }
    else if (nAxisNo == 1)
    {
        m_strAxisDescription = _T("AXIS_INSPECTOR_X2");
    }

    m_nMotorType = MOTORTYPE_SERVO;
    m_nDriveType = DRIVETYPE_REALPOS;
    m_bUseMode = TRUE;

    for (int nIndex = 0; nIndex < SPEED_END; nIndex++)
    {
        m_dSpeedMM[nAxisNo][nIndex] = (nIndex + 1) * 10;
        m_dAccelMM[nAxisNo][nIndex] = (nIndex + 1) * 10;
        m_dDecelMM[nAxisNo][nIndex] = (nIndex + 1) * 10;
    }
    m_nSpeedIndex = 0;
}

void DlgAjinMot::SetDefaultAJin(int nAxis) //보드에 직접 입력한 파라메타 초기화 용도.
{
    AxmMotSetPulseOutMethod(nAxis, OneHighLowHigh);
    AxmMotSetEncInputMethod(nAxis,ObverseSqr4Mode);
    AxmMotSetMoveUnitPerPulse(nAxis, 1, 1000);
    AxmMotSetMaxVel(nAxis, 70000);
    AxmMotSetMinVel(nAxis,0.01);
    AxmMotSetAbsRelMode(nAxis,POS_ABS_MODE);
    AxmMotSetProfileMode(nAxis,ASYM_TRAPEZOIDE_MODE);
    AxmHomeSetMethod(nAxis,0, 1, 0, 1000, 0.0);
    AxmHomeSetVel(nAxis,100, 50, 20, 10, 50, 1000);
    AxmSignalSetServoOnLevel(nAxis, High);
    DWORD dwLevel;
    AxmSignalGetLimit(nAxis, NULL, NULL, &dwLevel);
    AxmSignalSetLimit(nAxis, EMERGENCY_STOP, High, dwLevel);
    Sleep(100);
    AxmSignalGetLimit(nAxis, NULL, &dwLevel, NULL);
    AxmSignalSetLimit(nAxis, EMERGENCY_STOP, dwLevel, High);
    AxmSignalSetSoftLimit(nAxis, _AXT_USE::ENABLE, EMERGENCY_STOP, ACTUAL, 10, 0);
    AxmSignalSetInpos(nAxis,USED);
    AxmSignalSetServoAlarmResetLevel(nAxis,High);
    AxmSignalSetServoAlarm(nAxis,High);
}
UINT DlgAjinMot::Thread8KLineScanMotorinit(LPVOID pParam)
{
    DlgAjinMot* pDlg = (DlgAjinMot*)pParam;
    return pDlg->Thread8KLineScanMotor();
}
UINT DlgAjinMot::Thread8KLineScanMotor()
{
    CString strMotorParamPath;
    double dInspPos = 0.0f;
    ::SetEvent(m_threadControl_Motion.m_Signal_Ready);
    HANDLE pHandles[] = {m_threadControl_Motion.m_Signal_Kill, m_threadControl_Motion.m_Signal_Start};
    const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

    while (1)
    {
        DWORD dwRet = WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);
        if ((WAIT_OBJECT_0 + 1) == dwRet)
        {
            for (int nIndex = 0; nIndex < 2; nIndex++)
            {
                SetDefault(nIndex);
                LoadParameter(nIndex);
                ServoOnOff(nIndex, TRUE);
                MoveHoming(nIndex);
                if (MoveHomeComplete(nIndex) == HOME_SUCCESS)
                {
                    MoveInspetion(nIndex, 0/*m_dInspectionPos[nIndex]*/);
                    if (PosMoveComplete(nIndex) == 0)
                    {
                       
                    }
                }
            }
        }
        ::SetEvent(m_threadControl_Motion.m_Signal_Kill);
    }
}

void DlgAjinMot::ChangeEditMovepulse()
{
    if (!IsWindowVisible())
        return;

    CString str_MovePulse;
    m_edit_MovePulse.GetWindowText(str_MovePulse);
    long nPulse;
    nPulse = _wtol(str_MovePulse);
    AxmMotSetMoveUnitPerPulse(m_nMotionID, 1, nPulse);
}

void DlgAjinMot::OnBnClickedButtonStop()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    AxmMoveEStop(m_nMotionID);
}

afx_msg void DlgAjinMot::OnBnClickedCamRadio(UINT id)
{
    switch (id)
    {
        case IDC_RADIO_CAM1:
            m_nMotionID = 0;
            MotorStatusDisplay();
            // do something...
            break;
        case IDC_RADIO_CAM2:
            m_nMotionID = 1;
            MotorStatusDisplay();
            // do something...
            break;
        default:
            break;
    }
}

double DlgAjinMot::GetActualPosition(void)
{
    double dPosition;
    AxmStatusGetActPos(m_nMotionID, &dPosition);
    return dPosition;
}

double DlgAjinMot::GetCommandPosition(void)
{
    double dPosition;
    AxmStatusGetCmdPos(m_nMotionID, &dPosition);
    return dPosition;
}
void DlgAjinMot::MotorStatusDisplay()
{
    CString CommandPos, ActualPos, MovePulse;
    CString Vel, Acc, Dec;
    CommandPos.Format(_T("%lf"), GetCommandPosition());
    ActualPos.Format(_T("%lf"), GetActualPosition());

    m_edit_Command.SetWindowText(CommandPos);
    m_edit_Actual.SetWindowText(ActualPos);

    long nPulse;
    AxmMotGetMoveUnitPerPulse(m_nMotionID, NULL, &nPulse);
    MovePulse.Format(_T("%d"), nPulse);
    m_edit_MovePulse.SetWindowText(MovePulse);

    Dec.Format(_T("%lf"), m_dDecelMM[m_nMotionID][m_nSpeedIndex]);
    Acc.Format(_T("%lf"), m_dAccelMM[m_nMotionID][m_nSpeedIndex]);
    Vel.Format(_T("%lf"), m_dSpeedMM[m_nMotionID][m_nSpeedIndex]);
    m_edit_Dec.SetWindowText(Dec);
    m_edit_Acc.SetWindowText(Acc);
    m_edit_Vel.SetWindowText(Vel);
}
void DlgAjinMot::ChangeDeceleration()
{
    CString strMove_Deceleration;
    m_edit_Dec.GetWindowText(strMove_Deceleration);
    //m_nSpeedIndex = m_combo_SpeedLevel.GetCurSel();
    m_dDecelMM[m_nMotionID][0] = _wtof(strMove_Deceleration);
}

void DlgAjinMot::ChangeAcceleration()
{ 
    CString strMove_Acceleration;
    m_edit_Acc.GetWindowText(strMove_Acceleration);
    //m_nSpeedIndex = m_combo_SpeedLevel.GetCurSel();
    m_dAccelMM[m_nMotionID][0] = _wtof(strMove_Acceleration);
}

void DlgAjinMot::ChangeVelocity()
{
    CString strMove_Velocity;
    m_edit_Vel.GetWindowText(strMove_Velocity);
    //m_nSpeedIndex = m_combo_SpeedLevel.GetCurSel();
    m_dSpeedMM[m_nMotionID][0] = _wtof(strMove_Velocity);
}
DWORD DlgAjinMot::GetStatusMechanical(int nAxis)
{
    // 축의센서 감지 상태를 읽어온다.
    DWORD dwStatus;
    AxmStatusReadMechanical(nAxis, &dwStatus);
    return dwStatus;
}

BOOL DlgAjinMot::GetPluseLimitStatus(int nAxis)
{
    BOOL bState = (GetStatusMechanical(nAxis) & QIMECHANICAL_PELM_LEVEL) != 0 ? TRUE : FALSE;

    return bState;  
}
BOOL DlgAjinMot::GetMinusLimitStatus(int nAxis)
{
    BOOL bState = (GetStatusMechanical(nAxis) & QIMECHANICAL_NELM_LEVEL) != 0 ? TRUE : FALSE;

    return bState;
}

BOOL DlgAjinMot::GetAlarmStatus(int nAxis)
{
    BOOL bState = (GetStatusMechanical(nAxis) & QIMECHANICAL_ALARM_LEVEL) != 0 ? TRUE : FALSE;

    return bState;
}

BOOL DlgAjinMot::GetHomeStatus(int nAxis)
{
    DWORD dwStatus;
    AxmHomeReadSignal(nAxis, &dwStatus);

    return ACTIVE == dwStatus;
}


void DlgAjinMot::OnTimer(UINT_PTR nIDEvent)
    {
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    MotorStatusDisplay();

    if (nIDEvent == 30)
    {
        KillTimer(30);
        if (GetPluseLimitStatus(m_nMotionID) == TRUE)
        {
            if (m_nMotionID == 0)
            {
                m_static_LimitP1->SetBackColor(RGB(255, 0, 0));
            }
            else
            {
                m_static_LimitP2->SetBackColor(RGB(255, 0, 0)); 
            }
        }

        else if (GetPluseLimitStatus(m_nMotionID) == FALSE)
        {
            if (m_nMotionID == 0)
            {
                m_static_LimitP1->SetBackColor(RGB(255, 255, 255));
            }
            else
            {
                m_static_LimitP2->SetBackColor(RGB(255, 255, 255));
            }
        }

        if (GetMinusLimitStatus(m_nMotionID) == TRUE)
        {
            if (m_nMotionID == 0)
            {
                m_static_LimitM1->SetBackColor(RGB(255, 0, 0));
            }
            else
            {
                m_static_LimitM2->SetBackColor(RGB(255, 0, 0));
            }
            Sleep(10000);
        }

        
        else if (GetMinusLimitStatus(m_nMotionID) == FALSE)
        {
           if (m_nMotionID == 0)
           {
                m_static_LimitM1->SetBackColor(RGB(255, 255, 255));
           }
           else
           {
                m_static_LimitM2->SetBackColor(RGB(255, 255, 255));
           }
        }

        if (GetHomeStatus(m_nMotionID) == TRUE)
        {
            if (m_nMotionID == 0)
            {
                m_static_Home1->SetBackColor(RGB(255, 0, 0));
            }
            else
            {
                m_static_Home2->SetBackColor(RGB(255, 0, 0));
            }
        
        }
        else if (GetHomeStatus(m_nMotionID) == FALSE)
        {
           if (m_nMotionID == 0)
           {
                m_static_Home1->SetBackColor(RGB(255, 255, 255));
           }
           else
           {
                m_static_Home2->SetBackColor(RGB(255, 255, 255));
               
           }
        }
        SetTimer(30, 150, NULL);
    }

    if (nIDEvent == 50)
    {
        KillTimer(50);
        
        if (GetAlarmStatus(m_nMotionID) == TRUE)
        {
            char* chAlarm = "";
            DWORD dwAlarm = AxmStatusRequestServoAlarm(m_nMotionID);
            AxmStatusGetServoAlarmString(m_nMotionID, dwAlarm, 1024, chAlarm);
            Sleep(100);
            CString strAlarm;
            strAlarm.Format(_T("%s"), (LPCTSTR)chAlarm);
            m_static_Alarm->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_static_Alarm->SetBackColor(RGB(255, 0, 0));
            m_static_Alarm->SetFont(_T("휴먼둥근헤드라인"), RGB(255, 255, 255), 12, 1, 5);
            m_static_Alarm->SetWindowText(strAlarm);
        }

        if (GetAlarmStatus(m_nMotionID) == FALSE)
        {
            m_static_Alarm->ModifyStyle(0x0FL, SS_OWNERDRAW);
            m_static_Alarm->SetBackColor(RGB(255, 255, 255));
            m_static_Alarm->SetFont(_T("휴먼둥근헤드라인"), RGB(255, 255, 255), 12, 1, 5);
            m_static_Alarm->SetWindowText(_T(""));
        }

        SetTimer(50, 100, NULL);
    }
    CDialogEx::OnTimer(nIDEvent);
}

void DlgAjinMot::OnBnClickedButtonJogM()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    double dPosition, dPosMM;
    AxmStatusGetActPos(m_nMotionID, &dPosition);
    dPosMM = dPosition - 1;
    AxmMovePos(m_nMotionID, dPosMM, m_dSpeedMM[m_nMotionID][m_nSpeedIndex], m_dAccelMM[m_nMotionID][m_nSpeedIndex],
        m_dDecelMM[m_nMotionID][m_nSpeedIndex]);
}

void DlgAjinMot::OnBnClickedButtonJogP()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    double dPosition, dPosMM;
    AxmStatusGetActPos(m_nMotionID, &dPosition);
    dPosMM = dPosition + 1;
    AxmMovePos(m_nMotionID, dPosMM, m_dSpeedMM[m_nMotionID][m_nSpeedIndex], m_dAccelMM[m_nMotionID][m_nSpeedIndex],
        m_dDecelMM[m_nMotionID][m_nSpeedIndex]);
}

void DlgAjinMot::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    m_nSelectedID = 0;

    CWnd* pWnd = this->WindowFromPoint(point);
    if (pWnd != NULL)
    {
        m_nSelectedID = pWnd->GetDlgCtrlID();
    }

    double dYInterockDistance(0.0);
    double dYTargetPos(0.0);

    CString strErrorMessage;

    switch (m_nSelectedID)
    {
        case IDC_BUTTON_JOG_M:
            OnBnClickedButtonJogM();
            break;
        case IDC_BUTTON_JOG_P:
            OnBnClickedButtonJogP();
            break;
        default:
            break;
    }

    CDialogEx::OnLButtonDown(nFlags, point);
}

BOOL DlgAjinMot::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    switch (pMsg->message)
    {
        case WM_KEYDOWN:
            switch (pMsg->wParam)
            {
                case VK_ESCAPE:
                case VK_RETURN:
                    return TRUE;
            }
            break;

        case WM_LBUTTONDOWN:
            OnLButtonDown(TRUE, pMsg->pt);
            break;

        case WM_LBUTTONUP:
            OnLButtonUp(FALSE, pMsg->pt);
            break;
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

void DlgAjinMot::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    switch (m_nSelectedID)
    {
        case IDC_BUTTON_JOG_M:
        case IDC_BUTTON_JOG_P:
            OnBnClickedButtonStop();
            break;

        default:
            break;
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}

BOOL DlgAjinMot::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    CButton* pCheck;
    pCheck = (CButton*)GetDlgItem(IDC_RADIO_CAM1);
    pCheck->SetCheck(TRUE);
    BOOL bRet = FALSE;

    m_threadControl_Motion.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_Motion.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_Motion.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

    //Database dbJob;
    //CString strPath;
    //strPath.Format(_T("%s\\%s.job"), Path::GetRecipeFolder(), (LPCTSTR)g_systemParameter.m_currentModel);
    //dbJob.Load(strPath);
    ////RecipeLoad(strPath);
    ////LinkDataBase(false, dbJob);
    //LoadInspectionPos(dbJob);
    

    m_edit_TargetPos.SetWindowText(_T("0"));
    m_nMotionID = 0;
    m_nAxis = 0;
    m_nSpeedIndex = 0;
    m_dInspectionPos[0] = 0;
    m_dInspectionPos[1] = 0;
    m_strMotorParam.Format(_T("%s\\Motor.ini"), Path::GetRootFolder());
   // AxmMotLoadParaAll();
   // IO 에서 하도록 수정 
  //  bRet = AjinLibraryInitialize();

    for (int nindex = 0; nindex < 2; nindex++)
    {
        SetDefault(nindex);
        SetDefaultAJin(nindex);
        LoadParameter(nindex);
    }
    
    MotorStatusDisplay();
    SetTimer(30, 1000, NULL);
    SetTimer(50, 100, NULL);
    //if( bRet == TRUE)
    {
        AfxBeginThread(Thread8KLineScanMotorinit, LPVOID(this));
        ::SetEvent(m_threadControl_Motion.m_Signal_Start);
        ::ResetEvent(m_threadControl_Motion.m_Signal_Kill);
    }
   
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgAjinMot::OnClose()
{
    switch (MessageBox(_T("Insp Pos Save?"), _T("Message"), MB_YESNOCANCEL))
    {
        case IDCANCEL:
            return;
        case IDNO:
            CDialog::OnClose();
            return;
    }

    SaveParameter(m_nMotionID);
    CString strjobValue;
    m_edit_Actual.GetWindowText(strjobValue);
    m_dInspectionPos[m_nMotionID] = wcstod(strjobValue, NULL);

    CDialogEx::OnClose();
}
void DlgAjinMot::SaveInspectionPos(Database& dbJob)
{
    auto& db_pos = dbJob[_T("Inspection Pos")];
    m_dInspectionPos[0] = 80;
    m_dInspectionPos[1] = 90;
    db_pos[_T("1")].Link(TRUE, m_dInspectionPos[0]);
    db_pos[_T("2")].Link(TRUE, m_dInspectionPos[1]);
}
void DlgAjinMot::LoadInspectionPos(Database& dbJob)
{
    auto& db_pos = dbJob[_T("Inspection Pos")];
    db_pos[_T("1")].Link(FALSE, m_dInspectionPos[0]);
        //m_dInspectionPos[0] = 0;
    db_pos[_T("2")].Link(FALSE, m_dInspectionPos[1]);
        //m_dInspectionPos[1] = 0;
}

void DlgAjinMot::OnBnClickedButtonInspMove()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (!IsWindowVisible())
        return;

    ChangeEditMovepulse();

    if (m_dInspectionPos[m_nMotionID] != 0)
    {
        MoveInspetion(m_nMotionID, m_dInspectionPos[m_nMotionID]);
    }
    

}

void DlgAjinMot::OnBnClickedButtonInspStop()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    AxmMoveEStop(m_nMotionID);
}

void DlgAjinMot::OnStnClickedStaticAlarm()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (GetAlarmStatus(m_nMotionID) == TRUE)
    {
        AxmSignalServoAlarmReset(m_nMotionID, TRUE);
        AxmSignalServoAlarmReset(m_nMotionID, FALSE);
    }
}