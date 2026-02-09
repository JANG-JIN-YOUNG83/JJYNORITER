#pragma once
// DlgAjinDIO 대화 상자
enum machine
{
    Machine_Sitting_TOPLAMI,
    Machine_Sitting_TOPMISMATCH,
    Machine_Sitting_2,
    RollPress_Surface,
    RollPress_Pinhole,
};
enum BtnDIO
{
    DIO_POWERON,
    DIO_POWEROFF,
    DIOTOPCYLINDERUP,
    DIOTOPCYLINDERDOWN,
    DIOBACKCYLINDERUP,
    DIOBACKCYLINDERDOWN,
    DIOENCODERUP,
    DIOENCODERDOWN,
    DIOBLOWON,
    DIOBLOW_2ON,
    BtnDIO_COUNT,

};
class CCustomButton;
class CCustomStatic;

class DlgAjinDIO : public CDialogEx
    {
	DECLARE_DYNAMIC(DlgAjinDIO)

public:
	DlgAjinDIO(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~DlgAjinDIO();
    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DIO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void UIDisplay(long nMachine);
    void AlarmDisplay(CString strBtnIOName);
    void MachineBtnSetting(long nMachine);
    std::vector<long> vec_DIO;
    int m_nMachine;
    long m_nRetry;
    
   
    BOOL IORetry(long nAdress);
    afx_msg void RadioCtrl(UINT nRadioNo);
    afx_msg void OnBnClickedButtonPoweron();
    afx_msg void OnBnClickedButtonPoweroff();
    afx_msg void OnBnClickedButtonTopFwd();
    afx_msg void OnBnClickedButtonTopBwd();
    afx_msg void OnBnClickedButtonEncodeDown();
    afx_msg void OnBnClickedButtonEncodeUp();
    afx_msg void OnBnClickedButtonBackBwd();
    afx_msg void OnBnClickedButtonBackFwd();
    afx_msg void OnBnClickedButtonBlow1();
    afx_msg void OnBnClickedButtonBlow2();
    afx_msg void OnBnClickedButtonBlowOff();
    afx_msg void OnBnClickedButtonBlowOff2();
    void InPutIOCheck();

    void CustomControl();

    CCustomButton* m_pButtonPowerOn;
    CCustomButton* m_pButtonPowerOff;
    CCustomButton* m_pButtonTopFwd;
    CCustomButton* m_pButtonTopBwd;
    CCustomButton* m_pButtonBackFwd;
    CCustomButton* m_pButtonBackBwd;
    CCustomButton* m_pButtonEncodeUp;
    CCustomButton* m_pButtonEncodeDown;
    CCustomButton* m_pButtonBlow1;
    CCustomButton* m_pButtonBlow2;
    CCustomButton* m_pButtonBlowOff;
    CCustomButton* m_pButtonBlowOff2;
    CCustomStatic* m_pStaticIOAlarm;
 };
