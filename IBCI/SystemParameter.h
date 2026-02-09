#pragma once
#include "Types.h"

enum class ImageSaveType : int
{
	Bmp,
	Jpg,
	Png,
};

enum class CIS_Site : int
{
	ESOC,
	UC1,
	//ESWA,
	//ESNB,
	//ESNA,
	//ESNJ,
	//ESMI,
};

enum class MachineType;
enum class ExecutorType;
class SystemParameter
{
public:
	SystemParameter();
	~SystemParameter();

	void Load();
	void Save();
	void SavePara(CString filePath);
	CString MakeBackupPath();


	//{{ 다음의 값들은 직접변수를 접근하지 않고 해당 함수를 사용하여 읽을 것
	//   IBNI_PCSetting의 세팅값에 따라 값을 다르게 사용하기도 한다. 시뮬레이션 모드 등을 위함이다.
	LPCTSTR	GetConnectIP() const;		// HOST 접속 IP
	bool	IsExistGrabHardware() const;	// Grab Hardware가 존재하는가
	long	GetCoreCount() const;
	long	GetImageSaveOption() const;
	bool GetImageSaveAll()const;
	//}}

	int nPC_ID;
	CString strCfPath;
	int nAnodeCathode;
	BOOL bSensorImageMirror;

	double m_dPixelToUmXUp;
	double m_dPixelToUmXDown;
	double m_dPixelToUmY;

	CString	m_currentModel;
	int nLanguage;
	MachineType machineType;
	ExecutorType executorType;

	int				m_imageQueueSize;
	ImageSaveType	m_imageSaveType;
    CString strPathImageSave;
    long m_nImageResize;

	CString szMachineNumber;
	long nCuttingMargin;
	float fCuttingPitch;
	CString szConnectIP;
	long nConnectPort;
	long nConnectPort2;
	BOOL bUseISI;
	BOOL bXaxisMirror;
	BOOL bYaxisMirror;
	BOOL bConnectionTapeInterlock;
	
	// 예진
	BOOL bUseServer;
	BOOL bServer;
	CString strHostIP;
	long nPort;
	long nClientPCID;
	long nLaneCount;
	BOOL bMSASave;
	BOOL bMeasureSave;
	BOOL bMismatchSave;

	BOOL bDirection;
	BOOL bDefectCsvSave;
    BOOL bNotPair;
    BOOL bOSDS;
    BOOL bRollerOS;
    BOOL bRollerTwoSide;

    CIS_Site nCustomerSite; // 0 : ESOC, 1 : ESWA  //20220425 [jjh] 2 : ESNB/ESNA -> 추가 2 : ESNB 3 : ESNA 4 : ESNJ 5 : ESMI 윤여선
    CString pathResultDataCsv;

	BOOL bDisplayShow;
	//모터 IO Retry 
	long m_nIORetry;
    long nWarningCnt;
    long nSpecCnt;
    long nEdgeViewImageType;
    BOOL bEdgeViewSampling;
    long nSamplingCnt;
    BOOL bBatchFolderTree;
	//RUN_MODE enRunMode;
	//BOOL bCisAutoReset;

	//int nCamSingleDual;
	//int nCameraGain[4];
	//int nLedBrightness[4];
	//BOOL bUseAreaCam;
	//long nCellIDBit;
	//BOOL bIsLaserNotching;
	//int nInsulateType;
	//long nEncoderAlarmTimer;
	//long nInterlockSleepTIme;
	//BOOL bInspectTilt;
	//BOOL bInspectMisalign;
	//BOOL bInspectPetVinyl;
	//BOOL bMismatchAbsolute;
	//BOOL bPitchUpDownUse;
	//BOOL bScrabInterlock;
	//int nConnectionTapeSize;
	//BOOL bTabCrossCheck;
	////BOOL bTabSensorExceptionHandling;
	//BOOL bTabSensorSameID_Modify;
	////BOOL bUseImageLineMissedAlarm;
	//BOOL bSaveLineBlankImage;
	//BOOL bUse128x128FocusCalculation;
	//BOOL bUseTabHeightTop;
	//long nTabShapeRectCount;
	//CString strLogPath;
	//BOOL bUseInspectionForceExit;
	//BOOL bUseBurrIntoTabShape;
	//int nGenerationVersion;
	//BOOL bUseNewSurfaceInspectLogic;
	////BOOL bSaveGrabBufferImage;
	//BOOL bSaveSplitterInfo;
	//BOOL bSplitterAlarmAll;

	//BOOL bMainSendBrightImage;
	//BOOL bOverlaySendDarkImage;
	//BOOL bTabShapeSendBrightImage;
	//
	///*매우 중요한 설비 파라미터임*/
	//bool bUseSensorVHomeInSystem;  //vhome offset을 레시피별로갈지, 시스템별로갈지위한 변수
	//long nSensorVHomeOffsetUp;
	//long nSensorVHomeOffsetDown;
	//float fTabSensorDelayOffset;

	//long nRightVhomeIsAboveTheLeft;
	//float fCameraTitleOffset_pixel;	//미스 얼라인 보는 레이저 전용

	//long m_nUpdateBoard;

	//BOOL m_bVhomeOffsetChecker;

	//BOOL	m_recipeAutoBackup;
	//BOOL	m_FriendAliveAlarm;

	//BOOL	m_simualation_CellDown;
	//BOOL	m_simualation_usingInterface;
	//CString	m_simualation_lastestImageFolder;
	//CString	m_Cuttingsimualation_lastestImageFolder_Up;
	//CString	m_Cuttingsimualation_lastestImageFolder_Down;
	//int		nSimulationSleepTime;

	//BOOL bEncoderAlram;
	//int nInspectionMode;		// 0 : Grab only
	//// 1 : Grab and cell cutting
	//// 2 : Grab and cell cutting and inspection

	//bool			m_appendRejectTagWhenSavingImage;

	//CString szResultAName;
	//CString szResultBName;
	//BOOL bResultImageSave;
	//BOOL bDebugImageSave;
	//BOOL bMarkingCsvSave;

	//BOOL bUseIO;

	//CString szHDDPathInfo1;
	//CString szHDDPathInfo2;

	//BOOL bMonitoring_Drive_1;
	//BOOL bMonitoring_Drive_2;
	//BOOL bMonitoring_Drive_3;
	//long nWarningUsageRate;
	//long nAlarmUsageRate;
	//long nPeriodSec;

	//long nUpInvalidAlarmCount;
	//long nDownInvalidAlarmCount;
	//long nUpBufferAlarmCount;
	//long nDownBufferAlarmCount;

	//long nMultiImageCheckPos;
	//long nImageCheckBrightness;
	//long nImageCheckDarkness;

	//long nUpImageLeftPos;
	//long nUpImageRightPos;
	//long nDownImageLeftPos;
	//long nDownImageRightPos;
	//bool m_dontSendImageToHost;

	//CString szFriendIP;
	//long nFriendPort;


	//long m_resetResultValueOnAbnormalReset;

	//bool bSendRollMap;
	//BOOL bRollMapCamera;
	//BOOL bRollMapChannel;
	//long nRotation;
	//long nRollMapImageOffsetX;
	//long nRollMapImageOffsetY;
	//long nRollMapImageQuality;

	//long nIQ_CAM_ANGLE_VALUE;	// System.ini에서 JSON IQ_CAM_ANGLE_VALUE 값 설정 가능

	//BOOL m_bUseCellIDFixer;		// 셀 아이디 보상 기능 옵션 처리
	//int	 m_nFixerCoverage;
		
protected:
	// System Dialog 설정을 위해서만 접근할 변수
	BOOL	m_isExistGrabHardware;
	int		m_imageSaveOption;
	long	m_coreCount;
	


	friend class CSystemDlg;
};

extern SystemParameter g_systemParameter;
