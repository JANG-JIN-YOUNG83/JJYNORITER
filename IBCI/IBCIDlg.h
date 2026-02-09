
// IBCIDlg.h: 헤더 파일
//

#pragma once
#include "InspectionInputData.h"
#include "BatchInsPection.h"
#include "../Inspection/CISIDefectProperty.h"
#include "../SharedModule/VisionHostShare/SpecDataVision.h"

#include "SurfaceInspect.h"
#include "ThreadControl.h"
#include "../Inspection/InspectionBase.h"
#include "../Inspection/InspectionCell.h"
#include "../Inspection/InspectionRoot.h"
#include "CDlgLogDisplay.h"

#define UWM_UPDATE_UI (WM_USER + 1000)


class CCPUUsage;

class Database;
class CCustomStatic;
class CCustomButton;
class CDlgViewCellImage;
class CDlgVIewGrabImage;
class CSyncboard;
class CellImageDispatcher;
class CellImageProducer;
class CellImageMerger;
class ViewGrabImageUpdater;
class ViewCellImageUpdater;
class CDlgDefectCount;
class CDlgGraphWindow;
class CISComm;
class CISIViewerProperty;
class CSVSaveThread;
class CDefectCount;
class CDlgAnimationWindow;
class CDlgViewCameraSetting;
class CDlgRecipeManager;
class CDlgAccDefectMap;
class CDlgDefectImageView;
class CDlgNormalSpecView;
class CDlgSmallSpecView;
class CServerSock;
class CClientSock;
class CClientSock_S;
class CDlgCalculationResolution;
class InlineResultCoupler;
class InlineResultUiUpdater;
class InlineResultUiUpdater_View;
class InlineResultUiUpdater_Map;
class InlineResultUiUpdater_Count;
class CellImageSaver;
class CDlgSpecTable;
class CCalMap_Distortion;
class CDlgEdgeImageView;
class OptionCriteriaList;
class AlarmCommSocket;
class DefectCount;
class TcpResultSender;
class CDlgDefectMapUpdate;
class CDlgLogin;
class DlgHWControl;
class DlgAjinMot;

enum class ENUM_MODE_VIEW
{
	MAIN = 0,
	SURFACE,
	TEACHING
};

enum class ENUM_MODE_INSP
{
	START = 0,
	STOP,
	PAUSE
};

/// <summary>
///  서버꺼
/// </summary>
class CThreadSock : public CWinThread
{
public:
	DECLARE_DYNCREATE(CThreadSock)
	SOCKET m_hSocket;
	CClientSock_S* m_pClientSock_S;
	POSITION m_pos;
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};
enum class GrabError;

typedef struct S_PathCropImages 
{
    CString pathDirectBright;
    CString pathDirectDark;
    CString pathCropBright;
    CString pathCropDark;
    CString pathCropOversizeBright;
    CString pathCropOversizeDark;
} SPathCropImages;


// CIBCIDlg 대화 상자
class CIBCIDlg : public CDialogEx
{
// 생성입니다.
public:
	//클라이언트꺼
	CClientSock* m_pClientSock_C;
	void OnReceive();
	void OnSend2(CString strMsg_in);
	void OnSend();
	void OnClose_C();
	void OnConnect_C();
	CString	m_strSend_C;
	CString m_ipAddress_C;
	// 서버꺼
	void Send_S(CString strMsg);
	void OnReceive_S(CClientSock_S* pSock);
	void OnServerClose_S(CClientSock_S* pSock);
	BOOL m_bClientStatus ;
	
	void OnAccept_S();

	afx_msg void OnSend_S();
    void OnSend2_S(CString strMsg_in);
    std::deque<ResultMismatch> m_vecMismatchData;
    std::vector<Share::ResultVision> m_vecResultVisionData;
	CServerSock* m_pServerSock_S;
    CTypedPtrList<CObList, CClientSock_S*> m_clientSocks_S;
    void SaveCsvMeasureNotMsa(long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect,
        CString strLotID, long nCellID);
    // result
    void ProcCoupleResult(const ResultMismatch& result, Share::ResultVision resultdata);
    void ProcCoupleResult(const ResultMismatch& result);
    InlineResultCoupler* m_inlineResultCoupler;
    InlineResultUiUpdater* m_inlineResultUiUpdater;
    InlineResultUiUpdater_View* m_inlineResultUiUpdater_View;
    InlineResultUiUpdater_Map* m_inlineResultUiUpdater_Map;
    InlineResultUiUpdater_Count* m_inlineResultUiUpdater_Count;
    HANDLE m_Signal_Kill_UiUpdater;
    CCriticalSection m_csResult;
    ThreadControl m_threadControl_Result;
    void ThreadStart_Result();
    static UINT ThreadFunc_Result(LPVOID pParam);
    UINT ThreadFunc_Result();


	std::vector<CString> Msg_Pars(CString strMsg_in);
	std::vector<CString> m_vecstrPars;

	std::vector<float> m_vecfMismatchData;
	ResultSpec m_setSpec;
	bool m_bOpenJob;
	long m_nCropPixel_X;
	long m_nCropPixel_Y;
	afx_msg void OnBnClickedStatusVision();

	CellImageSaver* m_cellImageSaver;
	//CIBCIDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CIBCIDlg(IPVM::LoggerInterface& loggerSequence,
		IPVM::LoggerInterface& loggerSensorPosition,
		IPVM::LoggerInterface& loggerMarkPosition, 
		IPVM::LoggerInterface& loggerAlarm,
		IPVM::LoggerInterface& loggerGrab,
		IPVM::LoggerInterface& loggerIO,
		IPVM::LoggerInterface& loggerResult,
		IPVM::LoggerInterface& loggerDebug,
		IPVM::LoggerInterface& loggerTcp,
		IPVM::LoggerInterface& loggerTime,
		IPVM::LoggerInterface& loggerCPU,
		IPVM::LoggerInterface& loggerEswa,
		IPVM::LoggerInterface& loggerSimulation,
		IPVM::LoggerInterface& loggerImageSaveFailed,
		IPVM::LoggerInterface& loggerParamChanged,
		IPVM::LoggerInterface& loggerJudge,
		IPVM::LoggerInterface& loggerCellResult,
		IPVM::LoggerInterface& loggerFocus,
		CWnd* pParent = nullptr);	// 표준 생성자입니다.
	virtual ~CIBCIDlg();

	CSVSaveThread* m_csvSaveThread;
	CSVSaveThread* GetCSVSaveThread();

	std::vector<CCalMap_Distortion*> m_vecpDistortion;

	std::vector<CString> m_vecStrDefectType;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IBCI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	// Message Handler Override
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnSelChangeListThemes();
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	void SaveCellDesign(Database& dbJob);
	void LoadCellDesign(Database& dbJob);

public:
    // tcp
    afx_msg LRESULT OnConnectSocket(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnReceiveSocket(WPARAM wParam, LPARAM lParam);
    void ReceiveSocket_SpecCurrentData(const void* message, const long messageLength);
    void ReceiveSocket_JsonData();

    void GetJsonSystemData(Share::ParameterSystem& Params, SystemParameter& SysParam);
    void SendVisionParameters();
    void ReceiveSocket_VisionReset();
    void ReceiveSocket_RequestJSONData(const void* message, const long messageLength);
    void ReceiveSocket_RecipeChange(const void* message, const long messageLength);
    void ReceiveSocket_LotChange(const void* message, const long messageLength);
    void LotClear();
    BOOL m_bReceiveRequestData;
    AlarmCommSocket* m_pClientSocket;
    void SendSpec(int nVisionType); // 0 : 치수 검사, 1 : 표면 검사, 2: Mismach
    CCriticalSection m_csSpecData;
    void PushSpec_Measurement(DefectType defecttype, const double dCelldesignValue,
        Inspection::MeasureOffsets& measureOffset, Inspection::InspectionCriteriaList& criteriaList,
        OptionCriteriaList& optionCriteriaList);
    void PushSpec_Surface(DefectType defecttype, const double& dSizeX, const double& dSizeY, const bool& bAndor);
    Share::SpecDataVision m_specData;
    afx_msg void OnBnClickedStatusMaster();

	TcpResultSender* m_tcpResultSender; // Write Tcp Thread

	// Grab
    long m_nCellID_sync;
    long m_nCellID_calc;
    long m_nCellID_finalcalc;
	long m_nvirtualid;
	CISComm* m_cisComm;
    CellImageMerger* m_cellImageMerger;
    CellImageProducer* m_cellImageProducers;
	CellImageDispatcher* m_cellImageDispathcer;
	CCriticalSection m_csCellID;
	CSyncboard* m_pSyncIO;
	BOOL ReadSyncCellID();
	void OnInlineStart(BOOL bLive = FALSE);
	void OnInlineStop();
	static void CallbackOnFrameUpdated(void* callbackData, int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);
	static void CallbackOnFrameUpdated_Area(void* callbackData, int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);
	void CallbackOnFrameUpdated(int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);
	void CallbackOnFrameUpdated_Area(int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);
	static void CallbackOnCellImageProduced(void* callbackData, InspectionInputData& inputData);
    void CallbackOnCellImageProduced(InspectionInputData& inputData);

    // Dispatch
    CEvent m_eventCancelWaitingInspection;
	static void CallbackOnCellImageDispatched(void* callbackData, InspectionInputData& inputData);
    void CallbackOnCellImageDispatched(InspectionInputData& inputData);
    void CallbackOnCellImageDispatched_Dimension(InspectionInputData& inputData);
    //void CallbackOnCellImageDispatched_Surface(InspectionInputData& inputData);

	// Inspection
    std::vector<CInspectionCell> m_InspectionCellGroup;
    std::vector<ThreadControl> m_threadControl_InlineInspection;
    ThreadControl m_threadControl_InlineInspectionResultUpdate;
    BOOL m_bCheckInvalid;
    long m_nCheckPass;
	//Thead에서 사용되는 Buffer 만들어 놓자


	// 여기에 만들어야 하냐?
    void ThreadStart_InlineInspection();
    void ThreadStop_InlineInspeciton();
    static UINT ThreadFunc_InlineInspection(LPVOID pParam);
    UINT ThreadFunc_InlineInspection();

	static UINT ThreadFunc_InlineInspection_UC1(LPVOID pParam);
    UINT ThreadFunc_InlineInspection_UC1();

	static UINT ThreadFunc_InlineInspectionResultUpdate(LPVOID pParam);
    UINT ThreadFunc_InlineInspectionResultUpdate();

	// UI, CSV
    CString MakeCsvRowString(long index, CISIViewerProperty propertyView, CInspectionCell* unit);
    SPathCropImages SaveCropImages(CInspectionCell* unit, long index, IPVM::Image_8u_C1& OriginImage_Bright, IPVM::Image_8u_C1& OriginImage_Dark,
        IPVM::Image_8u_C1& CropImage_Bright, IPVM::Image_8u_C1& CropImage_Dark, CString strDirectPath);
    void UpdateUiData(vector<CISIViewerProperty> vecReceiveData);
    CCriticalSection m_csPropertyView;
    std::vector<CISIViewerProperty> m_vecReceiveData;
        CISIViewerProperty MakeISIViewerProperty(
        CInspectionCell* unit, long index, IPVM::Image_8u_C1& CropImage_Bright, IPVM::Image_8u_C1& CropImage_Dark, IPVM::Rect rtDraw, SPathCropImages pathsCrop);
    void SaveCsvMeasureMSA(CInspectionCell* unit);
    void SaveCsvMeasure(CInspectionCell* unit, long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect, CString pathFile);
    void SaveCsvMismatch(ResultMismatch resultMismatch, CString pathFile);
    void SaveCsvSurface(CInspectionCell* unit, CString strReceive, CString pathFile);
    void UpdateDefectView(CISIViewerProperty& propertyView);
    void UpdateDefectCount(CISIViewerProperty& propertyView);
    void UpdateDefectMap(CISIViewerProperty& propertyView);
    void UpdateDefectMap(CISIViewerProperty& propertyView, IPVM::Rect rect, IPVM::Image_8u_C1& imageBright);
    void UpdateAccDefectMap(IPVM::Rect rect);
    void UpdateEdgeView(CInspectionCellAlign* itemCellAlign); // 장지수 Edge View 추가


	ResultSpec LaneSpecData(std::vector<CIDISpecInfo> vecMeasureDefectUp, std::vector<ResultSpec> resvecLaneSpec, int nItemIdx);
    ResultMismatch MismathchData(ResultMismatch stResAllMismatch, std::vector<ResultLaneData> resvecLaneUp, std::deque<ResultMismatch> m_vecMismatchData,
        std::vector<ResultSpec> resvecLaneSpec, float fFullLength, long n, int check);
	ResultLaneData LaneUpData(std::vector<CIDISpecInfo> vecMeasureDefectUp, std::vector<ResultLaneData> resvecLaneUp, int nItemIdx);
    
	void ResultMismatchData(long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect, CString strLotID, long nCellID);
	void ResultMismatchData(long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect, CString strLotID, long nCellID, Share::ResultVision resultdata);
    void DisplayMismatchData(long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect, CString strLotID, long nCellID);
    ViewGrabImageUpdater* m_viewGrabImageUpdaters[2];
    ViewCellImageUpdater* m_viewCellImageUpdater;

	// 코터 추가, Merged
	static void CallbackOnFrameMerged(void* callbackData, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);
	void CallbackOnFrameMerged(UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);


	void CamsetApply();
    CEvent test;
    ResultMismatch m_stResAllMismatchData;
    BOOL HostSocketConnectCheck();

protected:	
	// Message Handler Docking Pane
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
	afx_msg void OnUpdateKeyIndicator(CCmdUI* pCmdUI);

	// Message Handler Custom Control
	afx_msg void OnBnClickedButtonTestInspection();
	afx_msg void OnBnClickedButtonInspStart();
	afx_msg void OnBnClickedButtonInspEnd();
	afx_msg void OnBnClickedButtonData();
	afx_msg void OnBnClickedButtonSystem();
	afx_msg void OnBnClickedButtonRecipe();
	afx_msg void OnBnClickedButtonSpec();
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonPgmClose();
	afx_msg void OnBnClickedButtonViewmodeMain();
	afx_msg void OnBnClickedButtonViewmodeSurface();
	afx_msg void OnBnClickedButtonViewmodeTeaching();
    afx_msg LRESULT OnUWMUpdateUI(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()

	// Variables
protected:
	// 
	BOOL m_bInitUI;
	CCustomStatic* m_pStaticLogo;
	CCustomStatic* m_pStaticMainTitle;
	CCustomStatic* m_pStaticCurTime;
	CCustomStatic* m_pStaticQueueStatus;
    CCustomStatic* m_pStaticMemoryStatus;
    long m_nCellId;

	CCustomStatic* m_pStaticNameMachine;
	CCustomStatic* m_pStaticNameVision;
	CCustomStatic* m_pStaticNameRecipe;
	CCustomStatic* m_pStaticNameModel;
    CCustomStatic* m_pStaticNamePosition;

	CCustomStatic* m_pStaticStatusCamera;
	CCustomStatic* m_pStaticStatusMaster;
	CCustomStatic* m_pStaticStatusFriendVision;
	CCustomStatic* m_pStaticStatusRes0;
	CCustomStatic* m_pStaticStatusRes1;

	CCustomStatic* m_pStaticStatusCaptionCamera;
	CCustomStatic* m_pStaticStatusCaptionMaster;
	CCustomStatic* m_pStaticStatusCaptionFriendVision;
	CCustomStatic* m_pStaticStatusCaptionRes0;
	CCustomStatic* m_pStaticStatusCaptionRes1;

	CCustomStatic* m_pStaticBtnInsp;
	
	
	CCustomButton* m_pButtonTestInsp;

	CCustomButton* m_pButtonInspStart;
	CCustomButton* m_pButtonInspEnd;
	CCustomButton* m_pButtonInspDlg;
	CCustomButton* m_pButtonData;
	CCustomButton* m_pButtonSystem;
	CCustomButton* m_pButtonRecipe;
	CCustomButton* m_pButtonSpec;
	CCustomButton* m_pButtonLogin;
	CCustomButton* m_pButtonPGMClose;
    CCustomButton* m_pButtomExportData;
    CCustomButton* m_pButtonHWControl;

	CCustomButton* m_pButtonViewmodeMain;
	CCustomButton* m_pButtonViewmodeSurface;
	CCustomButton* m_pButtonViewmodeTeaching;
	CCustomButton* m_pButtonBatchInsp;
	CCustomButton* m_pButtonCamSetting;

	CCustomButton* m_pButtonInitGraph;
	CCustomButton* m_pButtonCalResol;

	CCustomButton* m_pButtonReserve[2];
    CCustomButton* m_pButtonDataSearch;


	CCustomButton* m_pButtomCamera[4];
    long m_nCameraIndex;
    void OnButtonEvent();
 	void OnButtonResercveClick();
    void OnButtonEvent(UINT nIdex);
    void CellViewDisplay();
    void GrabViewDisplay();

	// General Variable
	CString m_strIniFileName;

	CString m_strInlinePath;

	// Pane Manager Variable
	CDialog m_wndPaneDlg[3];
	CEdit m_wndEdit[3];
	CFont m_fntEdit;

	CXTPDockingPane* m_pwndPaneMismatchAnimaiton;		//Mismatch Animation
	CXTPDockingPane* m_pwndPaneMismatchGraph;			//Mismatch Graph
	CXTPDockingPane* m_pwndPaneDefectMap;				//Defect Map
	CXTPDockingPane* m_pwndPaneSurfaceDefectSpec;		//Surface Defect Spec
	CXTPDockingPane* m_pwndPaneDefectCount;				//Defect Count
	CXTPDockingPane* m_pwndPaneDefectImage;				//Defect Image
	CXTPDockingPane* m_pwndPaneEdgeView;				//Edge View
	CXTPDockingPane* m_pwndPaneViewGrabImage;			//View Grab Image
	CXTPDockingPane* m_pwndPaneViewCellImage;			//View Cell Image

	// Control Variable
	//CStatic m_wndThemesLabel;
	CListBox m_wndThemesList;

	// Docking Pane Variable
	CXTPModuleHandle m_hModule2013;
	CXTPModuleHandle m_hModule2012;

	CXTPDockingPaneManager m_paneManager;

	CXTPStatusBar m_wndStatusBar;



	// View(Dlg) Variable
	CDlgViewCellImage* m_pViewCellImage;
	CDlgVIewGrabImage* m_pViewGrabImage;

	// General Variables
	ENUM_MODE_VIEW m_nModeView;



	// Functions
protected:
	void SetShowWindowMaximize();
	void UpdateControlLayer();
	CString GetFileVersion();
	void ConstructorCustomControl();
	void DestructorCustomControl();
	void InitializeCustomControl();
	
private:
		// logger
		IPVM::LoggerInterface& m_loggerSequence;
		IPVM::LoggerInterface& m_loggerSensorPosition;
		IPVM::LoggerInterface& m_loggerMarkPosition;
        IPVM::LoggerInterface& m_loggerAlarm;
		IPVM::LoggerInterface& m_loggerGrab;
		IPVM::LoggerInterface& m_loggerIO;
		IPVM::LoggerInterface& m_loggerResult;
		IPVM::LoggerInterface& m_loggerDebug;
		IPVM::LoggerInterface& m_loggerTcp;
		IPVM::LoggerInterface& m_loggerTime;
		IPVM::LoggerInterface& m_loggerCPU;
		IPVM::LoggerInterface& m_loggerEswa;
		IPVM::LoggerInterface& m_loggerSimulation;
		IPVM::LoggerInterface& m_loggerImageSaveFailed;
		IPVM::LoggerInterface& m_loggerParamChanged;
		IPVM::LoggerInterface& m_loggerJudge;
		IPVM::LoggerInterface& m_loggerCellResult;
		IPVM::LoggerInterface& m_loggerFocus;
public:
    afx_msg void OnBnClickedButtonHWControl();
	afx_msg void OnBnClickedButtonInspectionDlg();
	afx_msg void OnBnClickedButton4(); // m_dlgDefectmap 버튼
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	CDlgDefectMap* m_dlgDefectmap;
	CDlgDefectImageView* m_dlgDefectView;

	// 장지수 Edge Image View 추가
    CDlgEdgeImageView* m_dlgEdgeView;
	CDlgDefectCount* m_dlgDefectCount;
    CDlgAccDefectMap* m_dlgAcc;
    afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButtonBatch();
	CBatchInsPection* m_dlgBatch;
    void Inspection(CString strPath, CString strPath2, long nIndex, BOOL bIsUp = TRUE);


	CDlgGraphWindow* m_pDlgGraphWindow;
	afx_msg void OnBnClickedButtonFnGraph();
	void DisplayMismatchData();
	
	
	std::vector<CISIViewerProperty> m_vecISIAllDefect;
	long m_nDefectStartID;
	void InitializeResMismatchData(long nDataCnt, bool bInit = true);

	CDlgAnimationWindow* m_pDlgAnimationWindow;
	afx_msg void OnBnClickedButtonFnAnimation();
	afx_msg void OnBnClickedButtonNormalSpecView();
	afx_msg void OnBnClickedButtonSmallSpecView();

	CDlgNormalSpecView* m_dlgNormalSpecView;
	CDlgSmallSpecView* m_dlgSmallSpecView;
	
	CDlgViewCameraSetting* m_dlgCameraSetting;
	afx_msg void OnBnClickedButtonFnCameraSetting();

	CDlgRecipeManager* m_dlgRecipeManager;
	bool RecipeLoad(CString strFilePath = NULL);
	bool RecipeSave();

	CDlgSpecTable* m_dlgSpecTable;

	CXTPDockingPaneLayout* m_pPaneLayout;

	void cropImageSave(CString strPath, CString LotID, long cellID, IPVM::Image_8u_C1 cropImage_Bright, IPVM::Image_8u_C1 cropImage_Drak);
	std::vector<CISIViewerProperty> m_vecCsvData;   

	afx_msg void OnStnClickedStaticStartInsp();
	ENUM_MODE_INSP m_nModeInsp;
	afx_msg void OnBnClickedButtonInitGraph();
	afx_msg void OnBnClickedButtonCalcResolution();
    afx_msg void OnDestroy();
    afx_msg void OnNcDestroy();


	//장지수 Illum Control 관련 추가
    void Illum_On();
    void Illum_Off();

    void SetCameraVerticalLength();

	BOOL m_bLight;
	afx_msg void OnBnClickedButtonExportData();
    void ExportDefectCountData(std::vector<DefectCount> m_vecData);
    void ExportMismatchGraphData(long nCellIndex, std::vector<float> fDiffWidth);
    void ImportDefectCountData(CString strDefectName, long nTopNum, long nBottomNum, long nSumNum);
    void ImportMismatchGraphData(std::vector<long> nCellIndex, std::vector<std::vector<float>> vecvecDiffWidth);
    void ImportDefectMapData(long nTopBack,long nCellIndex, long X, long Y, long nPass);
    BOOL StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue);
    void ImportDefectCountFile();
    void ImportMismatchGraphFile();
    void ImportDefectMapFile();
    long m_nDefectMapCount;
	std::vector<long> m_vecnCellID;
    std::vector<long> m_vecCellIndex;
    std::vector<std::vector<float>> m_vecvecDiffWidth;
    void ExportDefectMapData(long nTopBack, long nCellIndex, DefectPointXY vecXY, long vecbPass, long nDefectCode,
        CString strBrightPath, CString strDarkPath);

	BOOL m_bInline;
    void ReDrawMismatchAnimation();
    BOOL m_bProgramOpened;
    afx_msg void OnBnClickedButtonDataSearch();
    BOOL m_bIBCIDlgOpened;
    int m_NetIndex;
    BOOL ALTIllumControllerInit();

	void UpdateDefectROIView(std::vector<IPVM::Rect> vecROI);
    void UdateDefectROIViewClear();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	long m_nSamplingCnt;
	// 20241112 ResultData Push InfoSet Function
    void SendHostResultDataInfo(Share::ResultVision &ResultData, CInspectionCell* itemCellAlign = nullptr);
	void SendHostResultDataInfo(Share::ResultVision& ResultData, std::vector<IPVM::Image_8u_C1>& vecBrightImg,std::vector<IPVM::Image_8u_C1>& vecDarkImg, CInspectionCell* itemCellAlign = nullptr);
    void SendHostCameraInfoData(Share::ResultVision &ResultData, CInspectionCell* itemCellAlign = nullptr);
    void SendHostDimensionData(Share::ResultVision &ResultData, CInspectionCell* itemCellAlign = nullptr);
    void SendHostMismatchData(Share::ResultVision &ResultData, CInspectionCell* itemCellAlign = nullptr);
    void SendHostDefectData(Share::ResultVision &ResultData, CInspectionCell* itemCellAlign = nullptr);
    void SendHostOriginalImageData(Share::ResultVision &ResultData, CInspectionCell* itemCellAlign = nullptr);

	void SendHostDefectData(Share::ResultVision& ResultData, std::vector<IPVM::Image_8u_C1>& vecBrightImg,std::vector<IPVM::Image_8u_C1>& vecDarkImg, CInspectionCell* itemCellAlign = nullptr);
	long m_nTestCellID;
    void SendHostData(Share::ResultVision ResultData);
    long m_nReceiveCellIndex;
	void GetDefectCropImage(IPVM::Image_8u_C1 brightImg, IPVM::Image_8u_C1 darkImg, std::vector<IPVM::Image_8u_C1> &vecBrightImg, std::vector<IPVM::Image_8u_C1> &vecDarkImg, CInspectionCell* itemCellAlign = nullptr);

	CDlgLogin* m_pDlgLogin;
	
	BOOL ContinueBrightOnOff(BOOL bOn, long nCh, long nValue);
    BOOL IllumConnect();
    SOCKET m_sockClient;
    DlgHWControl* m_dlgHWControl;
    DlgAjinMot* m_dlgAjinMot;
    CString m_strInspTime;
    CString m_strFolderTime;
    CDlgLogDisplay* m_pDlgLogDisplay;
    BOOL m_bBatchView;
};

