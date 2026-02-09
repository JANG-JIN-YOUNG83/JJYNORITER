#pragma once
#include "../ThreadControl.h"
#include "../../CompanySharedLibrary/Persistence/Database.h"
#include "DlgAjinMot.h"
// AjinMot 대화 상자
class CCustomStatic;
class DlgAjinDIO;
enum ENUM_MOTOR_SPEED
{
    SPEED_LOW,
    SPEED_MIDDLE,
    SPEED_HIGH,
    SPEED_END
};
enum ENUM_MOTOR_TYPE
{
    MOTORTYPE_SERVO,
    MOTORTYPE_STEP,
    MOTORTYPE_END
};

enum ENUM_DRIVE_TYPE
{
    DRIVETYPE_REALPOS,
    DRIVETYPE_TWOPOS,
    DRIVETYPE_END
};
class DlgAjinMot : public CDialogEx
{
    DECLARE_DYNAMIC(DlgAjinMot)

public:
    DlgAjinMot(CWnd* pParent = nullptr); // 표준 생성자입니다.
    virtual ~DlgAjinMot();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MOTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    DlgAjinDIO* m_AjinDio;
    afx_msg void OnBnClickedButtonMove();
    afx_msg void OnBnClickedButtonhome();
    afx_msg void OnBnClickedButtonHomestop();

    CEdit m_edit_TargetPos;
    CEdit m_edit_Command;
    CEdit m_edit_Actual;
    CEdit m_edit_MovePulse;
    CEdit m_edit_Vel;
    CEdit m_edit_Acc;
    CEdit m_edit_Dec;
    long m_combo_SpeedLevel;

    // 모터 관련 Code 추가
    //
    double m_dMoveplus;

    double m_dSpeedMM[2][3]; //속도
    double m_dAccelMM[2][3]; //가감속
    double m_dDecelMM[2][3]; //가감속

    int m_nAxis;
    int m_nMotionID;
    CString m_strAxisDescription;
    BOOL m_bUseMode;
    int m_nMotorType; //MOTORTYPE_SERVO, MOTORTYPE_STEP=1 //롤프레스 모터 타입 서보모터
    int m_nDriveType; //DriveType_RealPos, DriveType_TwoPos=1
    int m_nSpeedIndex;
    double m_dInspectionPos[2];
    CString m_strMotorParam;
    char* m_Para;
    ///
    BOOL AjinLibraryInitialize();
    BOOL AjinLibraryClose();
    void MoveHoming(int nAxisNo);
    DWORD MoveHomeComplete(int nAxisNo);
    void MoveInspetion(int nAxisNo, double dPosMM);
    BOOL PosMoveComplete(int nAxisNo);
    BOOL LoadParameter(int nAxisNo);
    BOOL SaveParameter(int nAxisNo);
    BOOL ServoOnOff(int nAxisNo, BOOL bOn);
    void SetDefault(int nAxisNo);
    //void SetDefaultAJin(int nAxisNo);
    static UINT Thread8KLineScanMotorinit(LPVOID pParam);
    UINT Thread8KLineScanMotor();
    afx_msg void ChangeEditMovepulse();
    ThreadControl m_threadControl_Motion;
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnBnClickedCamRadio(UINT id);
    double GetActualPosition(void);
    double GetCommandPosition(void);
    void MotorStatusDisplay();
    afx_msg void ChangeDeceleration();
    afx_msg void ChangeAcceleration();
    afx_msg void ChangeVelocity();
    CButton m_radio_axis;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButtonJogM();
    afx_msg void OnBnClickedButtonJogP();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    int m_nSelectedID;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
    void SaveInspectionPos(Database& dbJob);
    void LoadInspectionPos(Database& dbJob);
    void SetDefaultAJin(int nAxis);
    BOOL GetPluseLimitStatus(int nAxis);
    BOOL GetMinusLimitStatus(int nAxis);
    BOOL GetAlarmStatus(int nAxis);
    BOOL GetHomeStatus(int nAxis);
    DWORD GetStatusMechanical(int nAxis);

    CCustomStatic* m_static_LimitP1;
    CCustomStatic* m_static_Home1;
    CCustomStatic* m_static_LimitM1;
    CCustomStatic* m_static_LimitP2;
    CCustomStatic* m_static_Home2;
    CCustomStatic* m_static_LimitM2;
    CCustomStatic* m_static_Alarm;
    afx_msg void OnBnClickedButtonInspMove();
    afx_msg void OnBnClickedButtonInspStop();
    afx_msg void OnStnClickedStaticAlarm();
};
