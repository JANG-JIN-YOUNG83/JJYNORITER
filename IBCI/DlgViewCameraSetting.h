#pragma once

#include "../Module/CustomControl/CustomButton.h"
#include "../Module/CustomControl/CustomStatic.h"
#include "../SharedModule/VisionHostShare//LoginInfo.h"
// 장지수 Trigger Board 제어를 위한 cpp,header 파일 추가
#include "TriggerBaseParameter.h"
#include "TriggerRecipeParameter.h"
class CIBCIDlg;
class GrabExecutor;
class CellImageMerger;
class CellImageProducer;
class CellImageDispatcher;
class CCriticalSection;
class CSyncboard;
//rs232
class CMycomm;
//class GrabExecutor_Sapera;
    // CDlgViewCameraSetting 대화 상자
class CDlgViewCameraSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgViewCameraSetting)

public:
	CDlgViewCameraSetting(IPVM::LoggerInterface& loggerSequence, IPVM::LoggerInterface& loggerParamChanged, CIBCIDlg* pParent);   // standard constructor
	virtual ~CDlgViewCameraSetting();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_VIEW_CAMERA_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedRadioPort0();
	afx_msg void OnBnClickedRadioPort1();
	afx_msg void OnBnClickedButtonSettingApply();
	afx_msg LRESULT OnGridNotify(WPARAM wparam, LPARAM lparam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

protected:
	enum class Mode
	{
		Offline,
		Inline,
		Disable,
	};
	
	struct BestFocusInfo
	{
		float							m_pos;
		double							m_profileScore;
		std::vector<IPVM::Point_32f_C2>	m_profile;
	};

	Mode m_mode;
	IPVM::LoggerInterface& m_loggerSequence;
	IPVM::LoggerInterface& m_loggerParamChanged;
	CIBCIDlg* m_parent;
	CCriticalSection m_csImage;
	IPVM::Image_8u_C1 m_liveImage;
	
	BestFocusInfo m_bestFocus[2][2];	// [U/D][B/D]
	IPVM::Rect m_meanRoi;
	IPVM::Rect m_profileRoi;

	CString m_hexChannelValue[3];

public:
	IPVM::ImageView* m_liveView;
	IPVM::ProfileView* m_profileView[2];

	CCustomStatic m_staticTitle;
	CCustomButton m_buttonPort0;
	CCustomButton m_buttonPort1; 
	CCustomButton* m_pButtonOpenPort;
    CCustomButton* m_pButtonLiveStatus;
	CXTPPropertyGrid m_property;

	//CCustomButton m_buttonSettingApply;

    
	Share::enLoginMode m_loginMode;

public:
	void ResetBestFocusInformation(long portIndex);
	
	void MakeHexChannelValue();
	void UpdateControlLayer();
	void UpdateChannelValue();
	void UpdateHexChannelValue();

	static void CallbackOnLiveFrameUpdated(void* callbackData, int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount);
	void CallbackOnLiveFrameUpdated(int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount);

	void CISInitialize();
	void SetInlineEditMode();
	void SetOfflineEditMode();
	void Disable();

	void Show(bool visible);
	void UpdateUI_Parameter();

	LRESULT OnReceive(WPARAM length, LPARAM lpara);
    BOOL m_bRecive;
    BOOL m_brs232cState;
    //CButton m_buttonOpenPort;
    afx_msg void OnBnClickedButtonPortopen();

	CString m_strTBBaseParaPath;
    TriggerBaseParameter* m_pTBbaseParameter;
    TriggerRecipeParameter* m_triggerParameter;
    BOOL ReadData(long nAdd);
    BOOL WriteData(long nAdd, unsigned int nData);
    stMessageMap m_stMSGMAP;
    CEvent m_eventWriteData;
    BOOL OpenRS232CPort(CString strComPort, CString strBaudrate);
    CMycomm* m_rs232c;

    BOOL SendDataRS232C(LPCTSTR outbuf, int len);
    BYTE m_byRecValue[8]; //받는거...

    BOOL ReciveDataProcess(BYTE* byRecValue, int nLen);
    void SetReadData(BYTE by2, BYTE by3, int value);

    long m_nCommon_VirtualTrigger;
    BYTE m_bySend[8];
    BYTE m_byRecive[8];
    afx_msg void OnBnClickedButtonTriggerRead();
    afx_msg void OnBnClickedButtonTriggerWrite();
    void EEPROMWrite();


	HANDLE m_tRS232Handle;
	static UINT ThreadRS232Proc(LPVOID pParam);
    void ThreadRS232Data();

	CEvent m_eventTerminate;
    long m_nStep;

	//GrabExecutor_Sapera* m_pGrabSapera;
    void TriggerPeriodSet(long nValue);
    afx_msg void OnBnClickedButtonLiveOnOff();
    afx_msg void OnBnClickedButtonCamSetting();
    void OnChangeSetCameraPara(long nCamIndex, long nID);
    void OnChangeGain(long nCamIndex, long nID);
    void OnChangeExposureTime(long nCamIndex, long nRow, long nID);
    void OnChangeExposureTime(long nCamIndex, long nID);
	//Sensor Gain
	void OnChangeSensorGain(long nCamIndex, long nID);
    void OnChangeSensorGainValue(long nCamIndex, long nID);
    void OnChangeExposureSelector(long nCamIndex, long nID);
    void OnChangeRowGain(long nCamIndex, long nID);
    CString m_strExposure;
    CString m_strGain;
    void OnLiveCamStart();
    BOOL m_bLiveOnOff;

	int m_nValue;

	void ListCOMpPorts();
    std::vector<CString> m_vecPortName;

	void GetCOMPortNum();
    CString m_strCOMPort;
    CString m_strBaurdRate;

	CString m_strSesorGain;
    CString m_strSesorGainValue;

	CString m_strExposureSelector;
    CString m_strRowGain;

	void ResponseLeveling(long nCamIndex);

	BOOL m_bReset;
    void Reset(long nCamIndex);
};
