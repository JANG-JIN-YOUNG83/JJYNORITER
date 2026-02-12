
// IBCIDlg.cpp: 구현 파일
//
// 1. 미리 컴파일된 헤더
#include "framework.h"
#include "pch.h"

// 2. 주 헤더
#include "IBCIDlg.h"
#include "IBCI.h"
#include "IBCIDlg_InnerDefine.h"
#include "afxdialogex.h"

// 3. 보조 헤더
#include "../DataSearch/AccessorDataSearch.h"
#include "../Inspection/Accessor.h"
#include "../Inspection/InspectionSetupImage.h"

#include "Algorithm/ImageProcessing.h"
#include "DlgAccDefectMap.h"
#include "DlgAnimationWindow.h"
#include "DlgCalculationResolution.h"
#include "DlgDefectCount.h"
#include "DlgDefectImageView.h"
#include "DlgDefectMap.h"
#include "DlgGraphWindow.h"
#include "DlgNormalSpecView.h"
#include "DlgRecipeManager.h"
#include "DlgSmallSpecView.h"
#include "DlgVIewGrabImage.h"
#include "DlgViewCameraSetting.h"
#include "DlgViewCellImage.h"
#include "SystemDlg.h"
#include "ViewGrabImageUpdater.h"
#include "ViewCellImageUpdater.h"
#include "InlineResultCoupler.h"
#include "InlineResultUiUpdater.h"
#include "DlgSpecTable.h"
#include "CDlgEdgeImageView.h"
#include "../Grab/CISComm.h"
#include "../Grab/CisSetting.h"
#include "../Grab/GrabExecutor.h"
#include "../Grab/GrabExecutorContainer.h"
#include "../Module/Utility/Logger.h"
#include "../SyncPci/Syncboard.h"
#include "CellImageDispatcher.h"
#include "CellImageMerger.h"
#include "CellImageProducer.h"
#include "DlgSpecMeasure.h"
#include "AlarmCommSocket.h"
#include "TcpResultSender.h"
#include "CDlgLogin.h"
#include "CISIViewerProperty.h"

#include "Utility.h"
#include "Gadget/Miscellaneous.h"

#include "CSVReader.h"
#include "CSVSaveThread.h"

#include "ClientSock.h"
#include "ClientSock_S.h"
#include "ServerSock.h"

#include "CalMap_Distortion_Impl.h"
#include "CalMap_Distortion.h"
#include "pdh.h"

#include "..\SharedModule\CommonShare\ArchiveAllType.h"
//#include "Path.h"
#include "../Module/CustomControl/CustomButton.h"
#include "../Module/CustomControl/CustomStatic.h"

#include <algorithm>
#include <cmath>
#include <crtdbg.h>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <shellapi.h>
#include <string> //MFC
#include <vector>
#include <windows.h>
#include "CellImageSaver.h"
#include "../CompanySharedLibrary/CrashHandler/CrashHandler.h"
#include "ALT_Controller/ALT_LSTPE.h"
#include "DlgHWControl.h"
#include "Ajin/DlgAjinMot.h"
#include "Types/Image_8u_C1.h"
#include "Types/Image_16u_C1.h"
#include "Types/Image_32f_C1.h"
#include "Types/BlobInfo.h"


#pragma comment(lib, "version.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "ALT_LSTPE.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CThreadSock, CWinThread)

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

#define CELLRESIZEDIVID  32

#define MISMATCH_HORIZONTAL_SIZE 14288
#define PINHOLE_HORIZONTAL_SIZE 8192

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_ABOUTBOX
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다.

    // 구현입니다.

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg()
    : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static HANDLE self;
static int numProcessors;
double getCurrentValueTotal()
{
    PDH_FMT_COUNTERVALUE counterVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    return counterVal.doubleValue;
}

void init()
{
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;
    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    self = GetCurrentProcess();
    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
    //PdhOpenQuery(NULL, NULL, &cpuQuery);
    //PdhAddCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    //PdhCollectQueryData(cpuQuery);
  /*  PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);*/
}

double getCurrentValue()
{
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (double)((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent /= numProcessors;
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    return percent * 100;
  //  PDH_FMT_COUNTERVALUE counterVal;
  //
  //  PdhCollectQueryData(cpuQuery);
  //  PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
  //  return counterVal.doubleValue;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//{{AFX_MSG_MAP(CDialogPanesDlg)
END_MESSAGE_MAP()

// CIBCIDlg 대화 상자

CIBCIDlg::CIBCIDlg(IPVM::LoggerInterface& loggerSequence, IPVM::LoggerInterface& loggerSensorPosition,
    IPVM::LoggerInterface& loggerMarkPosition, IPVM::LoggerInterface& loggerAlarm, IPVM::LoggerInterface& loggerGrab,
    IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, IPVM::LoggerInterface& loggerDebug,
    IPVM::LoggerInterface& loggerTcp, IPVM::LoggerInterface& loggerTime, IPVM::LoggerInterface& loggerCPU,
    IPVM::LoggerInterface& loggerEswa, IPVM::LoggerInterface& loggerSimulation,
    IPVM::LoggerInterface& loggerImageSaveFailed, IPVM::LoggerInterface& loggerParamChanged,
    IPVM::LoggerInterface& loggerJudge, IPVM::LoggerInterface& loggerCellResult, IPVM::LoggerInterface& loggerFocus,
    CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_IBCI_DIALOG, pParent)
    , m_loggerSequence(loggerSequence)
    , m_loggerSensorPosition(loggerSensorPosition)
    , m_loggerMarkPosition(loggerMarkPosition)
    , m_loggerAlarm(loggerAlarm)
    , m_loggerGrab(loggerGrab)
    , m_loggerIO(loggerIO)
    , m_loggerResult(loggerResult)
    , m_loggerDebug(loggerDebug)
    , m_loggerTcp(loggerTcp)
    , m_loggerTime(loggerTime)
    , m_loggerCPU(loggerCPU)
    , m_loggerEswa(loggerEswa)
    , m_loggerSimulation(loggerSimulation)
    , m_loggerImageSaveFailed(loggerImageSaveFailed)
    , m_loggerParamChanged(loggerParamChanged)
    , m_loggerJudge(loggerJudge)
    , m_loggerCellResult(loggerCellResult)
    , m_loggerFocus(loggerFocus)
    , m_bInitUI(FALSE)
    , m_nModeView(ENUM_MODE_VIEW::MAIN)
    , m_cisComm(nullptr)
    , m_nvirtualid(0)
    , m_csvSaveThread(nullptr)
    , m_bOpenJob(false)
    , m_nModeInsp(ENUM_MODE_INSP::STOP)
    , m_eventCancelWaitingInspection(FALSE, TRUE)
    , m_inlineResultCoupler(nullptr)
    , m_inlineResultUiUpdater(nullptr)
    , m_inlineResultUiUpdater_View(nullptr)
    , m_inlineResultUiUpdater_Map(nullptr)
    , m_inlineResultUiUpdater_Count(nullptr)
    , m_nCellId(-1)
    , m_bLight(TRUE)
    , m_bInline(FALSE)
    , m_nSamplingCnt(0)
    , m_bReceiveRequestData(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bClientStatus = FALSE;
    m_bBatchView = FALSE;
    m_Signal_Kill_UiUpdater = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    // get path of executable
    TCHAR szBuff[_MAX_PATH];
    VERIFY(::GetModuleFileName(AfxGetInstanceHandle(), szBuff, _MAX_PATH));

    CString strAppPath(szBuff);

    LPTSTR lpszExt = _tcsrchr(szBuff, '.');
    lstrcpy(lpszExt, _T(".xml"));

    m_strIniFileName = szBuff;

    int nIndex = strAppPath.ReverseFind(_T('\\'));
    if (nIndex > 0)
    {
        strAppPath = strAppPath.Left(nIndex);
    }
    else
    {
        strAppPath.Empty();
    }

    CString strStylesPath = strAppPath + _T("\\Styles\\");
    m_hModule2013.LoadLibrary(strStylesPath + _T("Office2013.dll"));
    m_hModule2012.LoadLibrary(strStylesPath + _T("VisualStudio2012.dll"));

    // Initialize UI Control
    ConstructorCustomControl();

    // Initialize Path
    Path::SetImageFolder(m_strInlinePath = _T("D:"));
    if (g_systemParameter.machineType == MachineType::UC1_mismatch)
    {
        Path::SetImageFolder(m_strInlinePath = _T("Z:"));
    }
    g_systemParameter.Load();

    // Initialize Dialog
    m_pViewCellImage = NULL;
    m_pViewGrabImage = NULL;
    m_dlgDefectmap = nullptr;
    m_dlgAcc = nullptr;
    m_dlgDefectView = nullptr;
    m_dlgDefectCount = nullptr;
    m_pDlgGraphWindow = nullptr;
    m_pDlgAnimationWindow = nullptr;
    m_dlgNormalSpecView = nullptr;
    m_dlgSmallSpecView = nullptr;
    m_dlgCameraSetting = nullptr;
    m_dlgRecipeManager = nullptr;
    m_dlgEdgeView = nullptr;
    m_pPaneLayout = nullptr;
    m_vecpDistortion.resize(0);
    m_bCheckInvalid = FALSE;
}

void CIBCIDlg::OnDestroy()
{
    CDialogEx::OnDestroy();
    long n = 0;

    KillTimer(TIMER_ID_CLOCK);
    KillTimer(TIMER_ID_UPDATE_GRAB_QUEUE_STATUS);

    
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CIBCIDlg::OnNcDestroy()
{
    CDialogEx::OnNcDestroy();
    long n = 0;
   
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
CIBCIDlg::~CIBCIDlg()
{
    ::SetEvent(m_Signal_Kill_UiUpdater);

    //ThreadStop_InlineInspeciton();

    if (m_vecpDistortion.size() > 0)
    {
        for (auto nidx = 0; nidx < m_vecpDistortion.size(); nidx++)
        {
            if (m_vecpDistortion[nidx] != nullptr)
                delete m_vecpDistortion[nidx];
        }
    }
    

    DestructorCustomControl();

    if (m_dlgEdgeView)
        delete m_dlgEdgeView;
    if (m_dlgDefectmap)
        delete m_dlgDefectmap;
    if (m_dlgDefectView)
        delete m_dlgDefectView;
    if (m_dlgAcc)
        delete m_dlgAcc;
    if (m_dlgDefectCount)
        delete m_dlgDefectCount;
    if (m_pDlgGraphWindow)
        delete m_pDlgGraphWindow;
    if (m_dlgBatch)
        delete m_dlgBatch;
    if (m_pDlgAnimationWindow)
        delete m_pDlgAnimationWindow;
    if (m_dlgNormalSpecView)
        delete m_dlgNormalSpecView;
    if (m_dlgSmallSpecView)
        delete m_dlgSmallSpecView;
    if (m_dlgCameraSetting)
        delete m_dlgCameraSetting;
    if (m_dlgRecipeManager)
        delete m_dlgRecipeManager;
    if (m_pViewCellImage)
        delete m_pViewCellImage;
    if (m_pDlgLogin)
        delete m_pDlgLogin;
    

    for (int cameraIndex = 0; cameraIndex < 2; cameraIndex++)
    {
        delete m_viewGrabImageUpdaters[cameraIndex];
    }
    delete m_viewCellImageUpdater;

    if (m_pViewGrabImage)
        delete m_pViewGrabImage;

    delete m_csvSaveThread;
    
    delete m_cisComm;
    delete m_pSyncIO;

    if (m_cellImageMerger)
        delete m_cellImageMerger;
    delete m_cellImageProducers;
    delete m_cellImageDispathcer;
    if (m_pPaneLayout)
        delete m_pPaneLayout;

    delete m_inlineResultCoupler;
    delete m_inlineResultUiUpdater;
    delete m_inlineResultUiUpdater_View;
    delete m_inlineResultUiUpdater_Map;
    delete m_inlineResultUiUpdater_Count;

    delete m_cellImageSaver;
 

    
    if (g_systemParameter.bUseServer)
    {
        if (g_systemParameter.bServer == 0)
        {
            //클라이언트라면
            delete m_pClientSock_C;
        }
        else if (g_systemParameter.bServer == 1)
        {
            // 서버라면
            delete m_pServerSock_S;
            POSITION pos = m_clientSocks_S.GetHeadPosition();
            while (pos != NULL)
            {
                CClientSock_S* p = m_clientSocks_S.GetNext(pos);
                if (p != NULL && p->m_hSocket != 0)
                {
                    p->m_pThread->PostThreadMessageW(WM_QUIT, 0, 0);
                    Sleep(300);
                }
            }
        }
    }
    m_eventCancelWaitingInspection.SetEvent();
    m_paneManager.CloseAll();
    if (g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
    {
        ALT_LSTPE_Close(m_NetIndex);
    }
    if (m_Signal_Kill_UiUpdater != NULL)
    {
        ::CloseHandle(m_Signal_Kill_UiUpdater);
        m_Signal_Kill_UiUpdater = NULL;
    }
}

void CIBCIDlg::DoDataExchange(CDataExchange* pDX)
{
    //DDX_Control(pDX, IDC_TXT_THEMES, m_wndThemesLabel);
    DDX_Control(pDX, IDC_LIST_THEMES, m_wndThemesList);
    DDX_Control(pDX, IDC_STATIC_LOGO, *m_pStaticLogo);
    DDX_Control(pDX, IDC_STATIC_MAIN_TITLE, *m_pStaticMainTitle);
    DDX_Control(pDX, IDC_STATIC_CURRUNT_TIME, *m_pStaticCurTime);
    DDX_Control(pDX, IDC_STATIC_QUEUE_STATUS, *m_pStaticQueueStatus);
    DDX_Control(pDX, IDC_STATIC_NAME_MACHINE, *m_pStaticNameMachine);
    DDX_Control(pDX, IDC_STATIC_NAME_VISION, *m_pStaticNameVision);
    DDX_Control(pDX, IDC_STATIC_NAME_RECIPE, *m_pStaticNameRecipe);
    DDX_Control(pDX, IDC_STATIC_NAME_MODEL, *m_pStaticNameModel);
    DDX_Control(pDX, IDC_STATIC_STATUS_CAMERA, *m_pStaticStatusCamera);
    DDX_Control(pDX, IDC_STATIC_STATUS_MASTER, *m_pStaticStatusMaster);
    DDX_Control(pDX, IDC_STATIC_STATUS_VISION, *m_pStaticStatusFriendVision);
    DDX_Control(pDX, IDC_STATIC_STATUS_RES0, *m_pStaticStatusRes0);
    DDX_Control(pDX, IDC_STATIC_STATUS_RES1, *m_pStaticStatusRes1);
    DDX_Control(pDX, IDC_STATIC_STATUS_CAPTION_CAMERA, *m_pStaticStatusCaptionCamera);
    DDX_Control(pDX, IDC_STATIC_STATUS_CAPTION_MASTER, *m_pStaticStatusCaptionMaster);
    DDX_Control(pDX, IDC_STATIC_STATUS_CAPTION_VISION, *m_pStaticStatusCaptionFriendVision);
    DDX_Control(pDX, IDC_STATIC_STATUS_CAPTION_RES0, *m_pStaticStatusCaptionRes0);
    DDX_Control(pDX, IDC_STATIC_STATUS_CAPTION_RES1, *m_pStaticStatusCaptionRes1);
    DDX_Control(pDX, IDC_STATIC_START_INSP, *m_pStaticBtnInsp);
    DDX_Control(pDX, IDC_BUTTON_TEST_INSP, *m_pButtonTestInsp);
    DDX_Control(pDX, IDC_BUTTON_INSP_START, *m_pButtonInspStart);
    DDX_Control(pDX, IDC_BUTTON_INSP_END, *m_pButtonInspEnd);
    DDX_Control(pDX, IDC_BUTTON_INSP_DLG, *m_pButtonInspDlg);
    DDX_Control(pDX, IDC_BUTTON_BATCH, *m_pButtonBatchInsp);
    DDX_Control(pDX, IDC_BUTTON_DATA, *m_pButtonData);
    DDX_Control(pDX, IDC_BUTTON_SYSTEM, *m_pButtonSystem);
    DDX_Control(pDX, IDC_BUTTON_RECIPE, *m_pButtonRecipe);
    DDX_Control(pDX, IDC_BUTTON_FN_CAMERA_SETTING, *m_pButtonCamSetting);
    DDX_Control(pDX, IDC_BUTTON_SPEC, *m_pButtonSpec);
    DDX_Control(pDX, IDC_BUTTON_LOGIN, *m_pButtonLogin);
    DDX_Control(pDX, IDC_BUTTON_CLOSE_PGM, *m_pButtonPGMClose);
    DDX_Control(pDX, IDC_BUTTON_VIEWMODE_MAIN, *m_pButtonViewmodeMain);
    DDX_Control(pDX, IDC_BUTTON_VIEWMODE_SURFACE, *m_pButtonViewmodeSurface);
    DDX_Control(pDX, IDC_BUTTON_VIEWMODE_TEACHING, *m_pButtonViewmodeTeaching);
    DDX_Control(pDX, IDC_BUTTON_INIT_GRAPH, *m_pButtonInitGraph);
    DDX_Control(pDX, IDC_BUTTON_CALC_RESOLUTION, *m_pButtonCalResol);
    DDX_Control(pDX, IDC_BUTTON_EXPORT_DATA, *m_pButtomExportData);
    DDX_Control(pDX, IDC_BUTTON_DATA_SEARCH, *m_pButtonDataSearch);
    DDX_Control(pDX, IDC_STATIC_MEMORYSTATUS, *m_pStaticMemoryStatus);
    DDX_Control(pDX, IDC_STATIC_MACHINE_POSITION, *m_pStaticNamePosition);

    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIBCIDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_SIZE()
ON_WM_CLOSE()
ON_WM_TIMER()
ON_LBN_SELCHANGE(IDC_LIST_THEMES, &CIBCIDlg::OnSelChangeListThemes)
ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, &CIBCIDlg::OnDockingPaneNotify)
ON_MESSAGE(WM_KICKIDLE, &CIBCIDlg::OnKickIdle)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_CAPS, &CIBCIDlg::OnUpdateKeyIndicator)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_NUM, &CIBCIDlg::OnUpdateKeyIndicator)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, &CIBCIDlg::OnUpdateKeyIndicator)
ON_BN_CLICKED(IDC_BUTTON_TEST_INSP, &CIBCIDlg::OnBnClickedButtonTestInspection)
ON_BN_CLICKED(IDC_BUTTON_INSP_START, &CIBCIDlg::OnBnClickedButtonInspStart)
ON_BN_CLICKED(IDC_BUTTON_INSP_END, &CIBCIDlg::OnBnClickedButtonInspEnd)
ON_BN_CLICKED(IDC_BUTTON_DATA, &CIBCIDlg::OnBnClickedButtonData)
ON_BN_CLICKED(IDC_BUTTON_SYSTEM, &CIBCIDlg::OnBnClickedButtonSystem)
ON_BN_CLICKED(IDC_BUTTON_RECIPE, &CIBCIDlg::OnBnClickedButtonRecipe)
ON_BN_CLICKED(IDC_BUTTON_SPEC, &CIBCIDlg::OnBnClickedButtonSpec)
ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CIBCIDlg::OnBnClickedButtonLogin)
ON_BN_CLICKED(IDC_BUTTON_CLOSE_PGM, &CIBCIDlg::OnBnClickedButtonPgmClose)
ON_BN_CLICKED(IDC_BUTTON_VIEWMODE_MAIN, &CIBCIDlg::OnBnClickedButtonViewmodeMain)
ON_BN_CLICKED(IDC_BUTTON_VIEWMODE_SURFACE, &CIBCIDlg::OnBnClickedButtonViewmodeSurface)
ON_BN_CLICKED(IDC_BUTTON_VIEWMODE_TEACHING, &CIBCIDlg::OnBnClickedButtonViewmodeTeaching)
ON_BN_CLICKED(IDC_BUTTON_INSP_DLG, &CIBCIDlg::OnBnClickedButtonInspectionDlg)
ON_BN_CLICKED(IDC_BUTTON_HW_CONTROL, &CIBCIDlg::OnBnClickedButtonHWControl)
ON_BN_CLICKED(IDC_BUTTON4, &CIBCIDlg::OnBnClickedButton4)
ON_BN_CLICKED(IDC_BUTTON5, &CIBCIDlg::OnBnClickedButton5)
ON_BN_CLICKED(IDC_BUTTON6, &CIBCIDlg::OnBnClickedButton6)
ON_BN_CLICKED(IDC_BUTTON7, &CIBCIDlg::OnBnClickedButton7)
ON_BN_CLICKED(IDC_BUTTON_BATCH, &CIBCIDlg::OnBnClickedButtonBatch)
ON_BN_CLICKED(IDC_BUTTON_FN_GRAPH, &CIBCIDlg::OnBnClickedButtonFnGraph)
ON_BN_CLICKED(IDC_BUTTON_FN_ANIMATION, &CIBCIDlg::OnBnClickedButtonFnAnimation)
ON_BN_CLICKED(IDC_BUTTON_NORMAL_SPEC_VIEW, &CIBCIDlg::OnBnClickedButtonNormalSpecView)
ON_BN_CLICKED(IDC_BUTTON_SMALL_SPEC_VIEW, &CIBCIDlg::OnBnClickedButtonSmallSpecView)
ON_BN_CLICKED(IDC_BUTTON_FN_CAMERA_SETTING, &CIBCIDlg::OnBnClickedButtonFnCameraSetting)
ON_BN_CLICKED(IDC_STATIC_STATUS_VISION, &CIBCIDlg::OnBnClickedStatusVision)
ON_BN_CLICKED(IDC_STATIC_STATUS_MASTER, &CIBCIDlg::OnBnClickedStatusMaster)

ON_STN_CLICKED(IDC_STATIC_START_INSP, &CIBCIDlg::OnStnClickedStaticStartInsp)
ON_BN_CLICKED(IDC_BUTTON_INIT_GRAPH, &CIBCIDlg::OnBnClickedButtonInitGraph)
ON_BN_CLICKED(IDC_BUTTON_CALC_RESOLUTION, &CIBCIDlg::OnBnClickedButtonCalcResolution)
ON_WM_DESTROY()
ON_WM_NCDESTROY()
ON_MESSAGE(UWM_UPDATE_UI, &CIBCIDlg::OnUWMUpdateUI)
ON_BN_CLICKED(IDC_BUTTON_RESERVE1, &CIBCIDlg::OnButtonResercveClick)
ON_COMMAND_RANGE(IDC_BUTTON_CAM1_B, IDC_BUTTON_CAM2_D, &CIBCIDlg::OnButtonEvent)
ON_MESSAGE(WM_CONNECT_SOCKET, &CIBCIDlg::OnConnectSocket)
ON_MESSAGE(WM_RECEIVE_SOCKET, &CIBCIDlg::OnReceiveSocket)
ON_BN_CLICKED(IDC_BUTTON_EXPORT_DATA, &CIBCIDlg::OnBnClickedButtonExportData)
ON_BN_CLICKED(IDC_BUTTON_DATA_SEARCH, &CIBCIDlg::OnBnClickedButtonDataSearch)
END_MESSAGE_MAP()

// CIBCIDlg 메시지 처리기

static UINT indicators[] = {
    ID_SEPARATOR, // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

struct DOCKINGPANETHEME
{
    XTPDockingPanePaintTheme eTheme;
    LPCTSTR lpszTheme;
};

static DOCKINGPANETHEME paneThemes[] = {
    {xtpPaneThemeDefault, _T("Default Theme")},
    {xtpPaneThemeGrippered, _T("Grippered")},
    {xtpPaneThemeWinNative, _T("Windows Native")},
    {xtpPaneThemeWinExplorer, _T("Windows Explorer")},
    {xtpPaneThemeVisualStudio6, _T("Visual Studio 6")},
    {xtpPaneThemeVisualStudio2003, _T("Visual Studio 2003")},
    {xtpPaneThemeVisualStudio2005, _T("Visual Studio 2005")},
    {xtpPaneThemeVisualStudio2005Beta1, _T("Visual Studio 2005 Beta 1")},
    {xtpPaneThemeVisualStudio2005Beta2, _T("Visual Studio 2005 Beta 2")},
    {xtpPaneThemeVisualStudio2008, _T("Visual Studio 2008")},
    {xtpPaneThemeVisualStudio2010, _T("Visual Studio 2010")},
    {xtpPaneThemeVisualStudio2012, _T("Visual Studio 2012")},
    {xtpPaneThemeOffice2002Visio, _T("Office 2002 Visio")},
    {xtpPaneThemeOffice2003Visio, _T("Office 2003 Visio")},
    {xtpPaneThemeOffice2007Visio, _T("Office 2007 Visio")},
    {xtpPaneThemeOffice2003, _T("Office 2003")},
    {xtpPaneThemeOffice2003Outlook, _T("Office 2003 Outlook")},
    {xtpPaneThemeResource, _T("Office 2007")},
    {xtpPaneThemeOffice2007Outlook, _T("Office 2007 Outlook")},
    {xtpPaneThemeOffice2007Word, _T("Office 2007 Word")},
    {xtpPaneThemeOffice2013, _T("Office 2013")},
};

void CIBCIDlg::OnButtonResercveClick()
{
    // send spec
    if (GetAsyncKeyState(VK_SHIFT) < 0)
        SendSpec(0);
    else if (GetAsyncKeyState(VK_CONTROL) < 0)
        SendSpec(2);
    else if (GetAsyncKeyState(VK_MENU) < 0)
        SendSpec(1);
    else
        ReceiveSocket_JsonData();
}

// 카메라 별 Customizing
void CIBCIDlg::OnButtonEvent(UINT index)
{
    switch (index)
    {
        case IDC_BUTTON_CAM1_B:
            if (m_pButtomCamera[0]->m_nFlags == 0)
            {
                m_pButtomCamera[0]->m_nFlags = 1;
                m_pButtomCamera[0]->SetBackGradation(RGB(125, 125, 125), RGB(200, 223, 21), 1);
            }
            else if (m_pButtomCamera[0]->m_nFlags == 1)
            {
                m_pButtomCamera[0]->m_nFlags = 0;
                m_pButtomCamera[0]->SetBackGradation(RGB(200, 200, 200), RGB(230, 230, 230), 1);
            }
            break;
        case IDC_BUTTON_CAM1_D:
            if (m_pButtomCamera[1]->m_nFlags == 0)
            {
                m_pButtomCamera[1]->m_nFlags = 1;
                m_pButtomCamera[1]->SetBackGradation(RGB(125, 125, 125), RGB(200, 223, 21), 1);
            }
            else if (m_pButtomCamera[1]->m_nFlags == 1)
            {
                m_pButtomCamera[1]->m_nFlags = 0;
                m_pButtomCamera[1]->SetBackGradation(RGB(200, 200, 200), RGB(230, 230, 230), 1);
            }
            break;
        case IDC_BUTTON_CAM2_B:
            if (m_pButtomCamera[2]->m_nFlags == 0)
            {
                m_pButtomCamera[2]->m_nFlags = 1;
                m_pButtomCamera[2]->SetBackGradation(RGB(125, 125, 125), RGB(200, 223, 21), 1);
            }
            else if (m_pButtomCamera[2]->m_nFlags == 1)
            {
                m_pButtomCamera[2]->m_nFlags = 0;
                m_pButtomCamera[2]->SetBackGradation(RGB(200, 200, 200), RGB(230, 230, 230), 1);
            }
            break;
        case IDC_BUTTON_CAM2_D:
            if (m_pButtomCamera[3]->m_nFlags == 0)
            {
                m_pButtomCamera[3]->m_nFlags = 1;
                m_pButtomCamera[3]->SetBackGradation(RGB(125, 125, 125), RGB(200, 223, 21), 1);
            }
            else if (m_pButtomCamera[3]->m_nFlags == 1)
            {
                m_pButtomCamera[3]->m_nFlags = 0;
                m_pButtomCamera[3]->SetBackGradation(RGB(200, 200, 200), RGB(230, 230, 230), 1);
            }
            break;
    }

    CellViewDisplay();
    GrabViewDisplay();
}

void CIBCIDlg::CellViewDisplay()
{
    if (m_nModeView != ENUM_MODE_VIEW::SURFACE)
    {
        return;
    }
    CRect dlgRect;
    this->GetClientRect(&dlgRect);

    CRect childRect;
    m_pViewCellImage->GetWindowRect(&childRect);
    this->ScreenToClient(&childRect);

    int nViewCx = childRect.Width() - 2 * 5;
    int nViewCy = childRect.Height() - 25 - 2 * 5;
    int nViewWidth = childRect.Width();
    int nViewHeight = nViewCy + 5;

    if (m_pButtomCamera[0]->m_nFlags == 1)
    {
        ::MoveWindow(m_pViewCellImage->m_imageViews[0]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_pViewCellImage->m_imageViews[1]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);

       /* OPENFILENAME ofn;
        memset(&ofn, 0, sizeof(ofn));
        std::vector<TCHAR> pathName(32768, L'\0');
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = m_hWnd;
        ofn.lpstrFilter
            = _T("All Supported Image Files (*.bmp;*.jpg;*.jpeg;*.png)\0*.bmp;*.jpg;*.jpeg;*.png\0BMP Image Files ")
              _T("(*.bmp)\0*.bmp\0PNG Image Files (*.png)\0*.png\0JPEG Image Files (*.jpg;*.jpeg)\0*.jpg;*.jpeg\0\0");
        ofn.lpstrFile = &pathName[0];
        ofn.nMaxFile = 32768;
        ofn.lpstrDefExt = _T("*.bmp");
        ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        if (::GetOpenFileName(&ofn) == FALSE)
        {
            return ;
        }
         long horSize = 0;
        long verSize = 0;
         long bpp = 0;
        IPVM::Image::InfoFrom(ofn.lpstrFile, horSize, verSize, bpp);
        IPVM::Image_8u_C1 tempImage;
        tempImage.LoadFrom(ofn.lpstrFile);
                
        m_pViewCellImage->m_imageViews[0]->SetImage(tempImage); */
     
        if (m_nCameraIndex == 4)
        {
            ::MoveWindow(m_pViewCellImage->m_imageViews[2]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[3]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        }
    }
    else if (m_pButtomCamera[1]->m_nFlags == 1)
    {
        ::MoveWindow(m_pViewCellImage->m_imageViews[1]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_pViewCellImage->m_imageViews[0]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        if (m_nCameraIndex == 4)
        {
            ::MoveWindow(m_pViewCellImage->m_imageViews[2]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[3]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        }
    }
    if (m_nCameraIndex == 4)
    {
        if (m_pButtomCamera[2]->m_nFlags == 1)
        {
            ::MoveWindow(m_pViewCellImage->m_imageViews[2]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[0]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[1]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[3]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        }
        else if (m_pButtomCamera[3]->m_nFlags == 1)
        {
            ::MoveWindow(m_pViewCellImage->m_imageViews[3]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[0]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[1]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
            ::MoveWindow(m_pViewCellImage->m_imageViews[2]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        }
    }

    if (m_nCameraIndex == 2 && m_pButtomCamera[0]->m_nFlags == 0 && m_pButtomCamera[1]->m_nFlags == 0)
    {
        ::MoveWindow(m_pViewCellImage->m_imageViews[0]->GetSafeHwnd(), 0, 25, nViewWidth / 2 - 5, nViewHeight, TRUE);
        ::MoveWindow(
            m_pViewCellImage->m_imageViews[1]->GetSafeHwnd(), nViewWidth / 2 + 5, 25, nViewWidth, nViewHeight, TRUE);
    }
    else if (m_nCameraIndex == 4 && m_pButtomCamera[0]->m_nFlags == 0 && m_pButtomCamera[1]->m_nFlags == 0
        && m_pButtomCamera[2]->m_nFlags == 0 && m_pButtomCamera[3]->m_nFlags == 0)
    {
        ::MoveWindow(
            m_pViewCellImage->m_imageViews[0]->GetSafeHwnd(), 0, 25, nViewWidth / 2 - 5, nViewHeight / 2 - 5, TRUE);
        ::MoveWindow(m_pViewCellImage->m_imageViews[1]->GetSafeHwnd(), nViewWidth / 2 + 5, 25, nViewWidth,
            nViewHeight / 2 - 5, TRUE);
        ::MoveWindow(m_pViewCellImage->m_imageViews[2]->GetSafeHwnd(), 0, nViewHeight / 2 + 25, nViewWidth / 2 - 5,
            nViewHeight - 5, TRUE);
        ::MoveWindow(m_pViewCellImage->m_imageViews[3]->GetSafeHwnd(), nViewWidth / 2 + 5, nViewHeight / 2 + 25,
            nViewWidth, nViewHeight - 5, TRUE);
    }

    m_pViewCellImage->m_imageViews[0]->ZoomImageFit();
    m_pViewCellImage->m_imageViews[1]->ZoomImageFit();
    m_pViewCellImage->m_imageViews[2]->ZoomImageFit();
    m_pViewCellImage->m_imageViews[3]->ZoomImageFit();
}
void CIBCIDlg::GrabViewDisplay()
{
    if (m_nModeView != ENUM_MODE_VIEW::TEACHING)
    {
        return;
    }
    CRect dlgRect;
    this->GetClientRect(&dlgRect);

    CRect childRect;
    m_pViewGrabImage->GetWindowRect(&childRect);
    this->ScreenToClient(&childRect);

    int nViewCx = childRect.Width() - 2 * 5;
    int nViewCy = childRect.Height() - 25 - 2 * 5;
    int nViewWidth = childRect.Width();
    int nViewHeight = nViewCy + 5;

    if (m_pButtomCamera[0]->m_nFlags == 1)
    {
        ::MoveWindow(m_pViewGrabImage->m_imageViews[0]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[1]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[2]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[3]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
    }
    else if (m_pButtomCamera[1]->m_nFlags == 1)
    {
        ::MoveWindow(m_pViewGrabImage->m_imageViews[1]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[0]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[2]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[3]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
    }
    if (m_pButtomCamera[2]->m_nFlags == 1)
    {
        ::MoveWindow(m_pViewGrabImage->m_imageViews[2]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[0]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[1]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[3]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
    }
    else if (m_pButtomCamera[3]->m_nFlags == 1)
    {
        ::MoveWindow(m_pViewGrabImage->m_imageViews[3]->GetSafeHwnd(), 0, 25, nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[0]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[1]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[2]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
    }

    //(엣지뷰?)

    if (m_pButtomCamera[0]->m_nFlags == 0 && m_pButtomCamera[1]->m_nFlags == 0 && m_pButtomCamera[2]->m_nFlags == 0
        && m_pButtomCamera[3]->m_nFlags == 0)
    {
        ::MoveWindow(
            m_pViewGrabImage->m_imageViews[0]->GetSafeHwnd(), 0, 25, nViewWidth / 2 - 5, nViewHeight / 2 - 5, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[1]->GetSafeHwnd(), nViewWidth / 2 + 5, 25, nViewWidth,
            nViewHeight / 2 - 5, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[2]->GetSafeHwnd(), 0, nViewHeight / 2 + 25, nViewWidth / 2 - 5,
            nViewHeight - 5, TRUE);
        ::MoveWindow(m_pViewGrabImage->m_imageViews[3]->GetSafeHwnd(), nViewWidth / 2 + 5, nViewHeight / 2 + 25,
            nViewWidth, nViewHeight - 5, TRUE);
    }

    m_pViewCellImage->m_imageViews[0]->ZoomImageFit();
    m_pViewCellImage->m_imageViews[1]->ZoomImageFit();
    m_pViewCellImage->m_imageViews[2]->ZoomImageFit();
    m_pViewCellImage->m_imageViews[3]->ZoomImageFit();
}

void CIBCIDlg::PushSpec_Measurement(DefectType defecttype, double dCelldesignValue, Inspection::MeasureOffsets& measureOffset, Inspection::InspectionCriteriaList& criteriaList,
    OptionCriteriaList& optionCriteriaList)
{
    Share::SpecData data;

    data.m_nType = 0;
    data.m_inspType = defecttype;
    data.m_dCam_Unit = 0;
    data.m_dOffset = measureOffset.GetValue_mm(defecttype);
    data.m_dStandard_Value = dCelldesignValue;
    criteriaList.Find(defecttype)->ToHost(data);
    optionCriteriaList.Find(defecttype)->ToHost(data);

    m_specData.m_vecSpecDataList.push_back(data);
}

void CIBCIDlg::PushSpec_Surface(DefectType defecttype, const double& dSizeX, const double& dSizeY, const bool& bAndor)
{
    for (int i = 0; i < m_specData.m_vecSpecDataList.size(); i++)
    {
        if (m_specData.m_vecSpecDataList[i].m_inspType == defecttype)
        {
            //중복 리턴
            return;
        }
    }

    Share::SpecData data;

    data.m_nType = 1;
    data.m_inspType = defecttype;
    data.m_dCam_Unit = 0;
    data.m_dOffset = -9999;
    data.m_dStandard_Value = -9999;
    data.m_dMinusTolerance = -9999;
    data.m_dPlusTolerance = -9999;
    data.m_dAiMinusTolerance = -9999;
    data.m_dAiPlusTolerance = -9999;
    //data.m_dMarkingMinusTolerance = -9999;
    //data.m_dMarkingPlusTolerance = -9999;

    data.m_dDefectSizeX = dSizeX;
    data.m_dDefectSizeY = dSizeY;
    //data.m_bAndOr = bAndor;

    m_specData.m_vecSpecDataList.push_back(data);
}

afx_msg LRESULT CIBCIDlg::OnConnectSocket(WPARAM wParam, LPARAM lParam)
{
    const auto evtInfo = IPVM::MessageSocket::ParseEvt(wParam, lParam);

    if (evtInfo.m_connected)
    {
        m_pClientSocket->SendVisionInformation(GetFileVersion());
        //m_pClientSocket->SendVisionInformation();

        // 처음 접속했을때 현재 Vision Status 상태를 Host에게 전송해 주고 시작한다
        m_pClientSocket->SendVisionStatus(
            Share::VisionStatusType::VisionRunEnable, m_pSyncIO->GetOutputBit(SyncboardOutput::VisionRunEnable));

        m_pClientSocket->SendCrashDump(Path::GetCrashDumpFolder());
        m_pClientSocket->SendVisionRecipeAll(Path::GetRecipeFolder());
        m_pClientSocket->SendHostLoginAccess();
        if (TRUE == m_pClientSocket->IsConnected())
        {
            m_pStaticStatusMaster->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_ON2);
        }
        m_pStaticStatusMaster->Invalidate();
        //SetHost2Connect(TRUE);
    }
    else if (evtInfo.m_disconnected)
    {
        if (FALSE == m_pClientSocket->IsConnected())
        {
            m_pStaticStatusMaster->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
        }
        m_pStaticStatusMaster->Invalidate();
        //SetHost2Connect(TRUE);
    }
    return 0;
}

void CIBCIDlg::SendSpec(int nVisionType)
{
    CSingleLock lock(&m_csSpecData,
        TRUE); // m_specData를 쓸 때는 Lock을 걸고 쓰자. 검사 쓰레드에서 사용중인데 도중에 바뀌면 안되기 때문이다

    auto backupSpecData = m_specData; // Spec 변경전 기존 Spec 데이터를 백업해 둔다

    m_specData.init();

    m_specData.bIsValidData = TRUE;
    m_specData.m_strModel_ID = g_systemParameter.m_currentModel;

    auto& measureOffsets = Inspection::Accessor::Get().m_measureOffsets;
    auto& criteriaList = Inspection::Accessor::Get().m_criteriaList;
    auto& optionCriteriaList = Inspection::Accessor::Get().m_optionCriteriaList;

    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);
    CInspectionCellAlign* align = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
    CInspectionMeasure* measure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);

    auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;

    switch (nVisionType)
    {
        case 0:
        {
            m_specData.m_strVisionType = _T("Dimension");
            Share::SpecData data;
            data.m_nType = 0;

            for (long meas = 0; meas < measure->m_para.m_nCountMeasureItem; meas++)
            {
                DefectType defectType = (DefectType)(measure->m_para.m_vecMeasureType[meas]
                    + ((long)DefectType::START_Coater_Dimension + 1));
                //CIDISpecInfo idiSpecInfo(defectType, measure->m_para.m_vecMeasureType[meas]);

                data.m_inspType = defectType;

                // type이 0일때 보는 항목
                data.m_dStandard_Value = measure->m_para.m_vecMeasureStandard[meas]; // 기준값
                data.m_dMinusTolerance = measure->m_para.m_vecMeasureOffsetLSL[meas]; // 하한치
                data.m_dPlusTolerance = measure->m_para.m_vecMeasureOffsetUSL[meas]; // 상한치
                data.m_dCam_Unit = 0; // Cam unit
                data.m_dOffset = 0; // Offset 설정
                data.m_dAiMinusTolerance = measure->m_para.m_vecMeasureOffsetLCL[meas]; // 경고 하한치
                data.m_dAiPlusTolerance = measure->m_para.m_vecMeasureOffsetUCL[meas]; // 경고 상한치

                int m_nDefectSection = 0;

                data._EnableInspection = TRUE; //검사 여부
                data._bEssential_Inspection_Item = TRUE; //필수 검사항목

                m_specData.m_vecSpecDataList.push_back(data);
            }
            break;
        }
        case 1:
        {
            m_specData.m_strVisionType = _T("Surface");
            Share::SpecData data;
            data.m_nType = 1;

            data.m_dCam_Unit = 0;
#if INSPECTION_MACHINE_UC1
    data.m_inspType = DefectType::White_Line;

    CISISpecInfo* pSpec = isi_spec.GetSpecInfo(DefectType::White_Line);
#else
    data.m_inspType = DefectType::Line;

    CISISpecInfo* pSpec = isi_spec.GetSpecInfo(DefectType::Line);
#endif // INSPECTION_MACHINE_UC1

            
            data.m_dDefectSizeX = pSpec->SIZE_X_mm;
            data.m_dDefectSizeY = pSpec->SIZE_Y_mm;
            data.m_nCompareOperatorX = 1; // m_dDefectSizeX에 대한 부등호(0:Not Use, 1:크, 2:작)
            data.m_nCompareOperatorY = 1; // m_dDefectSizeY에 대한 부등호(0:Not Use, 1:크, 2:작)
            data.m_nOrAnd = 1; // 0:OR, 1:AND
            data.m_nLabelAttach = 0; // 미정
            data.m_nContinuous = 0; // 미정

            m_specData.m_vecSpecDataList.push_back(data);
            break;
        }
        case 2:
        {
            m_specData.m_strVisionType = _T("Mismatch");
            Share::SpecData data;
            data.m_nType = 2;

            ////Mismatch
            //data.m_inspType = DefectType::Mismatch;
            //data.m_dCam_Unit = 0;
            //data.m_dOffset = 0;
            //criteriaList.m_mismatch.ToHost(data);
            //optionCriteriaList.Find(DefectType::Mismatch)->ToHost(data);
            //
            //m_specData.m_vecSpecDataList.push_back(data);

            for (long meas = 0; meas < 2; meas++)
            {
                DefectType defectType = (DefectType)(measure->m_para.m_vecMeasureType[meas]
                    + ((long)DefectType::Start_Dimension_Mismatch + 1));
                //CIDISpecInfo idiSpecInfo(defectType, measure->m_para.m_vecMeasureType[meas]);

                data.m_inspType = defectType;

                // type이 0일때 보는 항목
                data.m_dStandard_Value = 500; // 기준값
                data.m_dMinusTolerance = -10; // 하한치
                data.m_dPlusTolerance = 10; // 상한치
                data.m_dCam_Unit = 0; // Cam unit
                data.m_dOffset = 0; // Offset 설정
                data.m_dAiMinusTolerance = -5; // 경고 하한치
                data.m_dAiPlusTolerance = 5; // 경고 상한치

                int m_nDefectSection = 0;

                data._EnableInspection = TRUE; //검사 여부
                data._bEssential_Inspection_Item = TRUE; //필수 검사항목

                m_specData.m_vecSpecDataList.push_back(data);
            }
            break;
        }
        default:;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    // 원래 Spec 내용중 Host에서 전달해 줬는데 Vision에서 저장하지 않는 정보는 저장해 두었다가 m_specData를 만든후 다시 복원해 준다
    // 이를 하지 않으면 Send Spec후 Spec Change 사이의 공백시간에 검사결과가 날라오면 잘못된 specData정보로 검사할 수 있다
    //-----------------------------------------------------------------------------------------------------------------------

    for (const auto& oldSpec : backupSpecData.m_vecSpecDataList)
    {
        if (auto* inspSpec = m_specData.FindSpec(oldSpec.m_inspType))
        {
            inspSpec->m_nDefectSection = oldSpec.m_nDefectSection;
            //inspSpec->_Rank = oldSpec._Rank;
            //inspSpec->_NGMarkingCount = oldSpec._NGMarkingCount;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------

    m_pClientSocket->SendSpecCurrentData(m_specData);
}

BOOL CIBCIDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

    // IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
    //  프레임워크가 이 작업을 자동으로 수행합니다.
    SetIcon(m_hIcon, TRUE); // 큰 아이콘을 설정합니다.
    SetIcon(m_hIcon, FALSE); // 작은 아이콘을 설정합니다.

    // TODO: 여기에 추가 초기화 작업을 추가합니다.

     m_nCameraIndex = g_systemParameter.machineType == MachineType::ES_rollpress_pinhole ? 4 : 2;
    ::CreateDirectory(_T("C:\\Intekplus"), NULL);
    ::CreateDirectory(_T("C:\\Intekplus\\iBCI"), NULL);
    ::CreateDirectory(_T("C:\\Intekplus\\iBCI\\CrashDump"), NULL);
    CrashHandler::Register(false, L"C:\\Intekplus\\iBCI\\CrashDump\\", L"iBCI_CrashDump");

    SetShowWindowMaximize();

    IPVM::MessageSocket::INIT_INFO initInfo = {0, 0, GetSafeHwnd(), WM_CONNECT_SOCKET, WM_RECEIVE_SOCKET};
    m_pClientSocket = new AlarmCommSocket(m_loggerTcp, m_loggerAlarm, initInfo);
    m_pClientSocket->SetHwnd(this->GetSafeHwnd());

    m_tcpResultSender = new TcpResultSender(*m_pClientSocket, m_loggerTcp);

    m_cisComm = new CISComm(m_loggerSequence, m_loggerGrab, g_systemParameter.machineType, 1,
        g_systemParameter.strCfPath, g_systemParameter.executorType, 0);
    //ThreadStart_Result();

    // sync
    bool bEnableSync = false;
    if (m_cisComm->GetExecutorType() == ExecutorType::iGrab || m_cisComm->GetExecutorType() == ExecutorType::Sapera
        || m_cisComm->GetExecutorType() == ExecutorType::Insnex)
        bEnableSync = true;
    std::deque<long> m_dqTirgger_Cellid;
    std::deque<long> m_dqBuffer_Cellid;
    std::deque<long> m_dqGroup_Cellid;
    m_pSyncIO = new CSyncboard(m_loggerIO, GetSafeHwnd(), bEnableSync,
        0, //g_systemParameter.nGenerationVersion,
        m_dqTirgger_Cellid, //m_pDlgSyncgrabWindow->m_dqTirgger_Cellid,
        m_dqBuffer_Cellid, //m_pDlgSyncgrabWindow->m_dqBuffer_Cellid,
        m_dqGroup_Cellid //m_pDlgSyncgrabWindow->m_dqOutput_Cellid
    );

	// merger hor, ver;
    if (m_cisComm->GetCameraCount() > 0)
    {
        switch (m_cisComm->GetExecutorType())
        {
            case ExecutorType::Euresys:
            {
                //m_cellImageMerger = new CellImageMerger(m_loggerTime, m_loggerSensorPosition, m_loggerSequence);
                const long nCamCnt = m_cisComm->GetCameraCount();
                GrabExecutor* grabExecutor[4] = {
                    nullptr,
                };
                m_vecpDistortion.resize(nCamCnt);
                CString strFilePath = _T("");

                for (long nindex = 0; nindex < nCamCnt; nindex++)
                {
                    //CdistortionAPI();
                    //CCalMap_Distortion* distortionTemp;
                    grabExecutor[nindex] = m_cisComm->GetGrabExcutor(nindex);
                    m_vecpDistortion[nindex] = new CCalMap_Distortion;
                    strFilePath.Format(_T("C:\\Intekplus\\IBCI\\distortion%d.tmp"), nindex);
                    m_vecpDistortion[nindex]->Load(strFilePath);
                }

                m_cellImageMerger = new CellImageMerger(
                    grabExecutor, nCamCnt, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, m_loggerGrab);
                m_cellImageMerger->SetHorizontalSize(m_cisComm->GetGrabExcutor(0)->GetHorizontalSize()
                    * m_cisComm->GetCameraCount()); // todo:2024.03.29 merge option
                m_cellImageMerger->SetVerticalSize(
                    m_cisComm->GetGrabExcutor(0)->GetVerticalSize()); ////todo:2024.03.29 merge option
                m_cellImageMerger->InitializeMergedBuffer();
                m_cellImageMerger->RegisterCallbackOnFrameMerged(CallbackOnFrameMerged, this);
                m_cellImageMerger->InitializeMergedBufferReady(m_cisComm->GetCameraCount());
                break;
            }
            case ExecutorType::Sapera:
            {
                GrabExecutor* grabExecutor[2] = {m_cisComm->GetGrabExcutor(0), m_cisComm->GetGrabExcutor(1)};
                m_cellImageMerger = new CellImageMerger(
                    grabExecutor, 2, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, m_loggerGrab);
                m_cisComm->GetGrabExcutor(0)->SetSensorImageMirror(g_systemParameter.bSensorImageMirror);
                m_cisComm->GetGrabExcutor(1)->SetSensorImageMirror(g_systemParameter.bSensorImageMirror);

                m_cellImageMerger->SetHorizontalSize(
                    m_cisComm->GetGrabExcutor(0)->GetHorizontalSize() * 2); // todo:2024.03.29 merge option
                m_cellImageMerger->SetVerticalSize(
                    m_cisComm->GetGrabExcutor(0)->GetVerticalSize() * 2); ////todo:2024.03.29 merge option
                m_cellImageMerger->InitializeMergedBuffer();
                m_cellImageMerger->RegisterCallbackOnFrameMerged(CallbackOnFrameMerged, this);
                m_cellImageMerger->InitializeMergedBufferReady(m_cisComm->GetCameraCount());
                break;
            }
            case ExecutorType::iGrab:
            {
                GrabExecutor* grabExecutor[3]
                    = {m_cisComm->GetGrabExcutor(0), m_cisComm->GetGrabExcutor(1), m_cisComm->GetGrabExcutor(2)};
                // merger hor, ver;
                m_cellImageMerger = new CellImageMerger(
                    grabExecutor, 3, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, m_loggerGrab);
                m_cellImageMerger->SetHorizontalSize(m_cisComm->GetGrabExcutor(0)->GetHorizontalSize() * 2 + 6912);
                m_cellImageMerger->SetVerticalSize(m_cisComm->GetGrabExcutor(0)->GetVerticalSize());
                m_cellImageMerger->InitializeMergedBuffer();
                m_cellImageMerger->RegisterCallbackOnFrameMerged(CallbackOnFrameMerged, this);
                m_cellImageMerger->InitializeMergedBufferReady(m_cisComm->GetCameraCount());
            }
            case ExecutorType::Insnex:
            {
                GrabExecutor* grabExecutor[1] = {m_cisComm->GetGrabExcutor(0)};
                m_cellImageMerger = new CellImageMerger(
                    grabExecutor, 1, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, m_loggerGrab);
                m_cisComm->GetGrabExcutor(0)->SetSensorImageMirror(g_systemParameter.bSensorImageMirror);
                //카메라가 하나라서 일단 하나만 살림
                //m_cisComm->GetGrabExcutor(1)->SetSensorImageMirror(g_systemParameter.bSensorImageMirror);
                 
                int sizeX = m_cisComm->GetGrabExcutor(0)->GetHorizontalSize(); 
                int sizeY = m_cisComm->GetGrabExcutor(0)->GetVerticalSize(); 
               
                m_cellImageMerger->SetHorizontalSize(sizeX ); // todo:2024.03.29 merge option
                m_cellImageMerger->SetVerticalSize(sizeY); ////todo:2024.03.29 merge option
                m_cellImageMerger->InitializeMergedBuffer();
                m_cellImageMerger->RegisterCallbackOnFrameMerged(CallbackOnFrameMerged, this);
                m_cellImageMerger->InitializeMergedBufferReady(m_cisComm->GetCameraCount());
                break;
            }
            default:;
        }
    }

    // merger hor size todo:2024.06.11 hor size option
    if (!m_cellImageMerger)
    {
        if (m_cisComm->GetExecutorType() == ExecutorType::Sapera)
        {
            GrabExecutor* grabExecutor;
            m_cellImageMerger = new CellImageMerger(
                &grabExecutor, 0, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, m_loggerGrab);
            //m_cellImageMerger->SetHorizontalSize(14288 * 2);

            if (MachineType::ES_rollpress_pinhole == g_systemParameter.machineType)
            {
                m_cellImageMerger->SetHorizontalSize(PINHOLE_HORIZONTAL_SIZE * 2);
            }
            else
            {
                m_cellImageMerger->SetHorizontalSize(MISMATCH_HORIZONTAL_SIZE * 2);
            }
        }
        if (m_cisComm->GetExecutorType() == ExecutorType::iGrab)
        {
            GrabExecutor* grabExecutor;
            m_cellImageMerger = new CellImageMerger(
                &grabExecutor, 0, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, m_loggerGrab);
            m_cellImageMerger->SetHorizontalSize(7344 * 4 + 6912);
        }
        if (m_cisComm->GetExecutorType() == ExecutorType::Insnex)
        {
            GrabExecutor* grabExecutor;
            m_cellImageMerger = new CellImageMerger(
                &grabExecutor, 0, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, m_loggerGrab);
            //m_cellImageMerger->SetHorizontalSize(14288 * 2);

            if (MachineType::ES_rollpress_pinhole == g_systemParameter.machineType)
            {
                m_cellImageMerger->SetHorizontalSize(PINHOLE_HORIZONTAL_SIZE * 2);
            }
            else
            {
                m_cellImageMerger->SetHorizontalSize(MISMATCH_HORIZONTAL_SIZE * 2);
            }
        }
    }

    //m_pSyncIO->OpenPort(); // 관리자 권한 모드로 실행 필요

    m_cellImageProducers = new CellImageProducer(*m_cellImageMerger, m_loggerTime, m_loggerSensorPosition, m_loggerSequence, true);
    m_cellImageProducers->RegisterCallbackOnCellImageProduced(CallbackOnCellImageProduced, this);
    if (m_cisComm->GetExecutorType() == ExecutorType::Euresys)
    {
        const long nCamCnt = m_cisComm->GetCameraCount();
        for (int cameraIndex = 0; cameraIndex < nCamCnt; cameraIndex++)
        {
            auto* grabExcutor = m_cisComm->GetGrabExcutor(cameraIndex);
            grabExcutor->RegisterCallbackOnFrameUpdated(GrabMode::AREA, CallbackOnFrameUpdated_Area, this);
            grabExcutor->RegisterCallbackOnFrameUpdated(GrabMode::AREA, CallbackOnFrameUpdated_Area, this);
        }
    }
    else
    {
    	if (m_cisComm->GetExecutorType() == ExecutorType::iGrab)
    	{
    		for (int cameraIndex = 0; cameraIndex < 3; cameraIndex++)
	        {
	            auto* grabExcutor = m_cisComm->GetGrabExcutor(cameraIndex);
	            grabExcutor->RegisterCallbackOnFrameUpdated(GrabMode::Inline, CallbackOnFrameUpdated, this);
	        }
    	}
    	else
    	{
	        for (int cameraIndex = 0; cameraIndex < 2; cameraIndex++)
	        {
	            auto* grabExcutor = m_cisComm->GetGrabExcutor(cameraIndex);
	            grabExcutor->RegisterCallbackOnFrameUpdated(GrabMode::Inline, CallbackOnFrameUpdated, this);
	        }
	    }
    }
    

    m_cellImageDispathcer = new CellImageDispatcher();
    m_cellImageDispathcer->RegisterCallbackOnCellImageDispatched(CallbackOnCellImageDispatched, this);
    m_cellImageDispathcer->Start();

    	//{{ inspection thread initialize
    m_threadControl_InlineInspection.resize(g_systemParameter.GetCoreCount() + 1);
    m_InspectionCellGroup.reserve(g_systemParameter.GetCoreCount() + 1);

    for (long idx = 0; idx < g_systemParameter.GetCoreCount() + 1; idx++)
    {
        m_InspectionCellGroup.emplace_back(m_loggerSequence, m_loggerResult);
    }
    //}} inspection thread initialize
    
    ///////////////////////////////////////////////////////
    //    Docking Pane Style define
    //////////////////////////////////////////////////////
    if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1; // fail to create
    }
    m_wndThemesList.ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

    for (int i = 0; i < _countof(paneThemes); i++)
    {
        int nItem = m_wndThemesList.AddString(paneThemes[i].lpszTheme);
        m_wndThemesList.SetItemDataPtr(nItem, &paneThemes[i]);
    }
    m_wndThemesList.ShowWindow(SW_HIDE);

    VERIFY(m_paneManager.InstallDockingPanes(this));
    m_paneManager.SetTheme(xtpPaneThemeNativeWindows10);
    m_paneManager.UseSplitterTracker(TRUE);
    m_paneManager.SetShowContentsWhileDragging(TRUE);
    m_paneManager.SetAlphaDockingContext(TRUE);
    m_paneManager.CreateLayout();

    // Create docking panes.
    m_pwndPaneMismatchAnimaiton
        = m_paneManager.CreatePane(IDR_PANE1, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockBottom); // Mismatch Animation
    m_pwndPaneDefectMap 
        = m_paneManager.CreatePane(IDR_PANE2, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockRight, m_pwndPaneMismatchAnimaiton); // Defect Map
    m_pwndPaneMismatchGraph
        = m_paneManager.CreatePane(IDR_PANE3, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockBottom); // Mismatch Graph
    m_pwndPaneSurfaceDefectSpec 
        = m_paneManager.CreatePane(IDR_PANE4, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockRight, m_pwndPaneMismatchGraph); // Surface Defect Spec
    m_pwndPaneDefectCount
        = m_paneManager.CreatePane(IDR_PANE5, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockBottom); // Defect Count
    m_pwndPaneDefectImage 
        = m_paneManager.CreatePane(IDR_PANE6, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockRight, m_pwndPaneDefectCount); // Defect Image
    m_pwndPaneEdgeView 
        = m_paneManager.CreatePane(IDR_PANE7, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockRight, m_pwndPaneDefectImage); // Edge View
    m_pwndPaneViewGrabImage
        = m_paneManager.CreatePane(IDR_PANE8, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockRight, m_pwndPaneEdgeView);
    m_pwndPaneViewCellImage
        = m_paneManager.CreatePane(IDR_PANE9, XTP_DPI(CRect(0, 0, 400, 165)), xtpPaneDockRight, m_pwndPaneEdgeView);

    //
    //LOGFONT lfIcon;
    //VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0));
    //m_fntEdit.CreateFontIndirect(&lfIcon);

    m_pPaneLayout = new CXTPDockingPaneLayout(&m_paneManager);
    if (m_pPaneLayout->LoadFromFile(_T("C:\\IBCI_DockingPaneLayout.txt"), _T("MainViewLayout")))
    {
        m_pPaneLayout->Save(_T("MainViewLayout"));
    }

    if (m_pPaneLayout->LoadFromFile(_T("C:\\IBCI_DockingPaneLayout.txt"), _T("SurfaceViewLayout")))
    {
        m_pPaneLayout->Save(_T("SurfaceViewLayout"));
    }

    if (m_pPaneLayout->LoadFromFile(_T("C:\\IBCI_DockingPaneLayout.txt"), _T("TeachingViewLayout")))
    {
        m_pPaneLayout->Save(_T("TeachingViewLayout"));
    }

    m_pPaneLayout->Load(_T("MainViewLayout"));
    m_paneManager.SetLayout(m_pPaneLayout);

    ///////////////////////////////////////////////////////
    //    Static Text Style define
    //////////////////////////////////////////////////////
    InitializeCustomControl();
    m_bInitUI = TRUE;

    ///////////////////////////////////////////////////////
    //    UI Object 생성
    //////////////////////////////////////////////////////
    m_pDlgAnimationWindow = new CDlgAnimationWindow(this);
    m_pDlgAnimationWindow->Create(IDD_DIALOG_ANIMATION_WINDOW, this);
    m_pDlgAnimationWindow->DataReset();

    m_dlgDefectmap = new CDlgDefectMap(this);
    m_dlgDefectmap->Create(IDD_DIALOG_DEFECT_MAP, this);

    m_pDlgGraphWindow = new CDlgGraphWindow(this);
    m_pDlgGraphWindow->Create(IDD_DIALOG_GRAPH_WINDOW, this);
    {
        BOOL bShow = TRUE;
        std::vector<DefectType> vecDisplayGraph;
        int nCount = 4;
        long type = 0;
        //nCount = g_systemParameter.inspItemInfo.GetGraphDisplayCount(vecDisplayGraph);
        //m_pDlgGraphWindow->SetGraphFrame(vecDisplayGraph, bShow);
        if (m_pDlgGraphWindow->m_pvecDlgGraphFrame.size() == 0)
        {
            nCount = 4;
        }

        for (int nIdx = 0; nIdx < nCount; nIdx++)
        {
            type = (long)DefectType::Start_Dimension_Mismatch + 1 + nIdx;
            vecDisplayGraph.push_back((DefectType)type);
        }

        m_pDlgGraphWindow->SetGraphFrame(vecDisplayGraph, bShow);
        for (long n = 0; n < nCount; n++)
        {
            m_pDlgGraphWindow->GetGraphFrame(n)->SetSpec(10.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
        }
        /*m_pDlgGraphWindow->GetGraphFrame(0)->SetSpec(10.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
		m_pDlgGraphWindow->GetGraphFrame(1)->SetSpec(20.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
		m_pDlgGraphWindow->GetGraphFrame(2)->SetSpec(30.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
		m_pDlgGraphWindow->GetGraphFrame(3)->SetSpec(40.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);*/
    }

    m_dlgNormalSpecView = new CDlgNormalSpecView;
    m_dlgNormalSpecView->Create(IDD_DIALOG_NORMAL_SPEC_VIEW, this);
	
#if INSPECTION_MACHINE_UC1
    m_dlgDefectView = new CDlgDefectImageView(6, 3);
#else
    m_dlgDefectView = new CDlgDefectImageView(4, 2);
#endif // INSPECTION_MACHINE_UC1

    m_dlgDefectView->Create(IDD_DLG_DEFECTIMAGE_VIEW, this);
    m_dlgDefectView->SetDlg(this);
    {
        m_vecISIAllDefect.clear(); // MAX = 40, 6*3 = 18
        // 예진 이미지 뿌린 개수에 맞게 개수 조절
        m_nDefectStartID = 0;
    }
    /*if (m_cisComm->GetExecutorType() == ExecutorType::iGrab)
	{
		m_dlgEdgeView = new CDlgEdgeImageView(6, 1);
	}
	else
	{
		m_dlgEdgeView = new CDlgEdgeImageView(4, 1);
	}*/
    m_dlgEdgeView = new CDlgEdgeImageView(6, 1);
    m_dlgEdgeView->Create(IDD_DLG_EDGEIMAGE_VIEW, this);
    m_dlgEdgeView->SetDlg(this);
    m_dlgEdgeView->ModifyLaneCount(g_systemParameter.nLaneCount);

    m_pDlgLogDisplay = new CDlgLogDisplay;
    m_pDlgLogDisplay->Create(IDD_DLG_LOGDISPLAY);
    m_pDlgLogDisplay->GetEdgeView(m_dlgEdgeView);
    Utility::BaseLogger& logger1 = (Utility::BaseLogger&)m_loggerSequence;
    Utility::BaseLogger& logger2 = (Utility::BaseLogger&)m_loggerSensorPosition;
    Utility::BaseLogger& logger3 = (Utility::BaseLogger&)m_loggerMarkPosition;
    Utility::BaseLogger& logger4 = (Utility::BaseLogger&)m_loggerAlarm;
    Utility::BaseLogger& logger5 = (Utility::BaseLogger&)m_loggerGrab;
    Utility::BaseLogger& logger6 = (Utility::BaseLogger&)m_loggerIO;
    Utility::BaseLogger& logger7 = (Utility::BaseLogger&)m_loggerResult;
    Utility::BaseLogger& logger8 = (Utility::BaseLogger&)m_loggerDebug;
    Utility::BaseLogger& logger9 = (Utility::BaseLogger&)m_loggerTcp;
    Utility::BaseLogger& logger10 = (Utility::BaseLogger&)m_loggerTime;
    Utility::BaseLogger& logger11 = (Utility::BaseLogger&)m_loggerCPU;
    Utility::BaseLogger& logger12 = (Utility::BaseLogger&)m_loggerEswa;
    Utility::BaseLogger& logger13 = (Utility::BaseLogger&)m_loggerSimulation;
    Utility::BaseLogger& logger14 = (Utility::BaseLogger&)m_loggerImageSaveFailed;
    Utility::BaseLogger& logger15 = (Utility::BaseLogger&)m_loggerParamChanged;
    Utility::BaseLogger& logger16 = (Utility::BaseLogger&)m_loggerJudge;
    Utility::BaseLogger& logger17 = (Utility::BaseLogger&)m_loggerCellResult;
    Utility::BaseLogger& logger18 = (Utility::BaseLogger&)m_loggerFocus;

    logger1.RegistToDisplay(m_pDlgLogDisplay->m_edit1);
    logger2.RegistToDisplay(m_pDlgLogDisplay->m_edit2);
    logger3.RegistToDisplay(m_pDlgLogDisplay->m_edit3);
    logger4.RegistToDisplay(m_pDlgLogDisplay->m_edit4);
    logger5.RegistToDisplay(m_pDlgLogDisplay->m_edit5);
    logger6.RegistToDisplay(m_pDlgLogDisplay->m_edit6);
    logger7.RegistToDisplay(m_pDlgLogDisplay->m_edit7);
    logger8.RegistToDisplay(m_pDlgLogDisplay->m_edit8);
    logger9.RegistToDisplay(m_pDlgLogDisplay->m_edit9);
    logger10.RegistToDisplay(m_pDlgLogDisplay->m_edit10);
    logger11.RegistToDisplay(m_pDlgLogDisplay->m_edit11);
    logger12.RegistToDisplay(m_pDlgLogDisplay->m_edit12);
    logger13.RegistToDisplay(m_pDlgLogDisplay->m_edit13);
    logger14.RegistToDisplay(m_pDlgLogDisplay->m_edit14);
    logger15.RegistToDisplay(m_pDlgLogDisplay->m_edit15);
    logger16.RegistToDisplay(m_pDlgLogDisplay->m_edit16);
    logger17.RegistToDisplay(m_pDlgLogDisplay->m_edit17);
    logger18.RegistToDisplay(m_pDlgLogDisplay->m_edit18);

    m_dlgHWControl = new DlgHWControl(this);
    m_dlgHWControl->Create(IDD_DIALOG_HWCONTROL, this);

 

    //최근 Recipe Open
    RecipeLoad(g_systemParameter.m_currentModel);

    m_dlgNormalSpecView->ModifySpecTable();
    //
    
    std::vector<CString> vecStrDefectType;
    std::vector<CString> vecStrDefectTypeMismatch;

    auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;

    for (long n = (long)DefectType::START_Coater_Surface + 1; n < (long)DefectType::END_Coater_Surface; n++)
    {
        CISISpecInfo* info = isi_spec.GetSpecInfo((DefectType)n);

        if (info->ENABLE.GetValue() == true)
        {
            vecStrDefectType.push_back(GetDefectName((DefectType)n));
            
        }
    }

#if INSPECTION_MACHINE_UC1

#else
    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);
    CInspectionMeasure* measure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);

    for (long meas = 0; meas < measure->m_para.m_nCountMeasureItem; meas++)
    {
        DefectType defectType
            = (DefectType)(measure->m_para.m_vecMeasureType[meas] + ((long)DefectType::START_Coater_Dimension + 1));
        if (defectType == DefectType::Width_NonCoating_1 || defectType == DefectType::Width_NonCoating_2
            || defectType == DefectType::Width_Coating_1 || defectType == DefectType::Width_Coating_2)
        {
            vecStrDefectTypeMismatch.push_back(GetDefectName(defectType));
        }
    }
#endif

    int nVecSize = vecStrDefectType.size();
    int nRow = 0;
    int nCol = 3;
    if(nVecSize % 3 != 0)
    {
        nRow = (nVecSize / nCol) + 1;
    }
    else
    {
        nRow = nVecSize / nCol;
    }

    m_dlgDefectCount = new CDlgDefectCount(nCol, nRow);
    m_dlgDefectCount->Create(IDD_DLG_DEFECTCOUNT, this);
   

    for (int n = 0; n < vecStrDefectType.size(); n++)
    {
        DefectCount cnt(vecStrDefectType[n], 0, 0);
        m_dlgDefectCount->m_vecData.push_back(cnt);
    }
    m_dlgDefectCount->DefectDraw();

#if INSPECTION_MACHINE_UC1

#else
    for (long n = 0; n < vecStrDefectTypeMismatch.size(); n++)
    {
        DefectCount cnt(vecStrDefectTypeMismatch[n], 0.f, 0.f, 0.f, true);
        m_dlgDefectCount->m_vecMismatchData.push_back(cnt);
    }
    m_dlgDefectCount->DefectMismatchDraw();
#endif
    ///////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////
    m_pViewCellImage = new CDlgViewCellImage(g_systemParameter.machineType);
    m_pViewCellImage->Create(CDlgViewCellImage::IDD, this);
    m_pViewCellImage->SetDlg(this);

    m_viewCellImageUpdater = new ViewCellImageUpdater(g_systemParameter.machineType, *m_pViewCellImage->m_imageViews[0],
        *m_pViewCellImage->m_imageViews[1], *m_pViewCellImage->m_imageViews[2], *m_pViewCellImage->m_imageViews[3]);

    m_pViewGrabImage = new CDlgVIewGrabImage();
    m_pViewGrabImage->Create(CDlgVIewGrabImage::IDD, this);
    //for (int cameraIndex = 0; cameraIndex < 2; cameraIndex++)
    //{
    m_viewGrabImageUpdaters[0]
        = new ViewGrabImageUpdater(*m_pViewGrabImage->m_imageViews[0], *m_pViewGrabImage->m_imageViews[1], 0);
    m_viewGrabImageUpdaters[1]
        = new ViewGrabImageUpdater(*m_pViewGrabImage->m_imageViews[2], *m_pViewGrabImage->m_imageViews[3], 1);
    //}

    
    //{{ Thread Start
    ThreadStart_InlineInspection();
    m_inlineResultCoupler = new InlineResultCoupler(m_loggerIO, m_loggerResult, *m_pSyncIO, this);
    m_inlineResultUiUpdater = new InlineResultUiUpdater(
        m_loggerIO, m_loggerResult, this, CTime::GetCurrentTime(), 500, g_systemParameter.pathResultDataCsv);
    m_inlineResultUiUpdater_View = new InlineResultUiUpdater_View(m_loggerIO, m_loggerResult, this);
    m_inlineResultUiUpdater_Map = new InlineResultUiUpdater_Map(m_loggerIO, m_loggerResult, this);
    m_inlineResultUiUpdater_Count = new InlineResultUiUpdater_Count(m_loggerIO, m_loggerResult, this);
    //}} Thread Start

    ///////////////////////////////////////////////////////
    //    Timer Start
    //////////////////////////////////////////////////////
    SetTimer(TIMER_ID_CLOCK, 1000, NULL);
    SetTimer(TIMER_ID_UPDATE_GRAB_QUEUE_STATUS, 250, nullptr);
    SetTimer(TIMER_ID_CAMERA_CONNECT_CHECK, 1000, NULL);
    //if (g_systemParameter.machineType != MachineType::UC1_mismatch)
    {
        SetTimer(TIMER_ID_UPDATE_CPUMEMORY_STATUS, 1000, NULL);
    }
    
    //m_vecISIAllDefect;
    // 예진 이미지 뿌린 개수에 맞게 개수 조절
    m_nDefectStartID = 0;
    m_pClientSock_C == nullptr;
    if (g_systemParameter.bUseServer)
    {
        if (!AfxSocketInit())
        {
            AfxMessageBox(_T("AfxSocketInit() FAILED. InitInstance"));
        }

        if (g_systemParameter.bServer == 0)
        {
            // 클라이언트라면 oninit

            m_pClientSock_C = new CClientSock();
            m_pClientSock_C->Create();
            m_pClientSock_C->Connect(g_systemParameter.strHostIP, g_systemParameter.nPort);

        }
        else if (g_systemParameter.bServer == 1)
        {
            // 서버라면 oninit
            m_pServerSock_S = new CServerSock();
            m_pServerSock_S->Create(g_systemParameter.nPort, SOCK_STREAM, FD_ACCEPT);
            m_pServerSock_S->Listen();
        }
    }

    m_inlineResultCoupler->ClearLot();

    m_nCellID_sync=0;
    m_nCellID_calc = 0;
    m_nCellID_finalcalc = 0;

  /*  m_cellImageSaver = new CellImageSaver(m_strEquipmentID, m_strLotID, m_loggerImageSaveFailed,
    g_systemParameter.nGenerationVersion, g_systemParameter.nCISInspType);*/
    m_cellImageSaver = new CellImageSaver(_T("test"), _T("test"), m_loggerImageSaveFailed,
        1, 1);


    //ReadData(_TB03_VirtualTrigger);
    //m_stMSGMAP.pop(*m_bySend);

    //SendDataRS232C((LPCTSTR)m_bySend, 8);

    //ReciveDataProcess(m_byRecive, 8);

    //ReadData(_TB15_Line_TriggerRatio);
    
    //UINT TriggerPeriod = m_triggerParameter->m_nArrData[_TB03_VirtualTrigger];
    //UINT V1LTRG = m_triggerParameter->m_nArrData[_TB15_Line_TriggerRatio];
    
    //AfxMessageBox(_T("TriggerPeriod  = %d,V1LTRG = %d  "), TriggerPeriod, V1LTRG);

    //AccessorDataSearch::Get().ShowDlg();
    if (g_systemParameter.bDisplayShow == TRUE)
    {
        m_bProgramOpened = TRUE;

        ImportDefectCountFile();
        ImportMismatchGraphFile();
    }
    
    m_bIBCIDlgOpened = TRUE;
    //ImportDefectMapFile();

    //CPU 사용량 확인을 위한 메모리데이터 초기화 진행
    init();
    if (g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
    {
        ALTIllumControllerInit();
    }
    m_nTestCellID = 0;
    return TRUE; // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 클라이언트에서 받는 애
void CIBCIDlg::OnReceive()
{
    wchar_t temp[MAX_PATH];
    m_pClientSock_C->Receive(temp, sizeof(temp));
    wchar_t* receive;
    receive = temp;
    int len;
    CString str;
    CString str2;
    BSTR bufTemp;
    BYTE stx = 0x05;
    BYTE etx = 0x0a;

    /*char buf[200] = {};
	m_pClientSock->Receive(buf,200);
	len = MultiByteToWideChar(CP_ACP, 0, buf, strlen(buf), NULL, NULL);
	bufTemp = SysAllocStringLen(NULL, len);
	MultiByteToWideChar(CP_ACP, 0, buf, strlen(buf), bufTemp, len);
	str.Format(_T("%s"), bufTemp);*/

    str.Format(receive);

    if (str.Left(10) == _T("MISSMATCH,"))
    {
        TRACE(str + _T ("\n"));
        str = str.Right(str.GetLength() - 10);
        ResultMismatch receiveResMismatch;

        long nCnt = str.Find(_T(","), 0);
        CString strLotID = str.Left(nCnt);

        receiveResMismatch.m_strLotID = strLotID;

        long nCnt2 = str.Find(_T(","), nCnt + 1);
        CString strCellIndex = str.Mid(nCnt + 1, nCnt2 - nCnt);

        int nCellIndex = _ttoi(strCellIndex);
        receiveResMismatch.m_nCellIndex = nCellIndex;

        long nCnt3 = str.Find(_T(","), nCnt2 + 1);
        CString strDirection = str.Mid(nCnt2 + 1, nCnt3 - nCnt2);

        int nDirection = _ttoi(strDirection);

        long nCnt4 = str.Find(_T(","), nCnt3 + 1);
        CString strVecCnt = str.Mid(nCnt3 + 1, nCnt4 - nCnt3);
        int nVecCnt = _ttoi(strVecCnt);

        long nTemp = nCnt4;
        long nTemp2;
        std::vector<float> vecfData;
        std::vector<float> vecfData2;

        if (nDirection == 0) // top
        {
            receiveResMismatch.m_vecResUpLane.resize(nVecCnt);
        }
        else
        {
            receiveResMismatch.m_vecResDownLane.resize(nVecCnt);
        }
        for (int n = 0; n < nVecCnt; n++)
        {
            nTemp2 = str.Find(_T(","), nTemp + 1);
            CString strVecCnt = str.Mid(nTemp + 1, nTemp2 - nTemp);
            vecfData.push_back(_ttof(strVecCnt));
            nTemp = nTemp2;

            if (nDirection == 0) // top
            {
                receiveResMismatch.m_vecResUpLane[n].fLaneWidth = vecfData[n];
            }
            else if (nDirection == 1) // back
            {
                receiveResMismatch.m_vecResDownLane[n].fLaneWidth = vecfData[n];
            }
        }
        nTemp2 = str.Find(_T(","), nTemp + 1);
        CString strFullLength = str.Mid(nTemp + 1, nTemp2 - nTemp);
        float fFull = _ttof(strFullLength);
        if (nDirection == 0)
        {
            receiveResMismatch.m_fTopFullLength = fFull;
        }
        else if (nDirection == 1)
        {
            receiveResMismatch.m_fBackFullLength = fFull;
        }

        m_inlineResultCoupler->Push2(receiveResMismatch);
       
        m_loggerResult.Log(0, str);
    }

    if (str.Left(12) == _T("InlineStart"))
    {
        OnInlineStart();
    }
    else if (str.Left(12) == _T("StartCellID,"))
    {
        TRACE(str + _T("\n"));
        str = str.Right(str.GetLength() - 12);
        int nCellIndex = _ttoi(str);

        long sensorID = m_pSyncIO->GetCell_ID();
        if (nCellIndex<sensorID)
        {
            m_nCellID_finalcalc = sensorID - 63;
        }
    }
    if (str.Left(7) == _T("IllumOn"))
    {
        Illum_On();
    }
    if (str.Left(8) == _T("IllumOff"))
    {
        m_cisComm->GrabExcutorContainerStop();
        m_cellImageMerger->Stop();
        Illum_Off();
    }

    /*if (str.Left(22) == _T("OTM_SEND_AFMOTOR_READY"))
	{
		CStringA strRerutn;
		strRerutn = _T("MTO_RECV_AFMOTOR_READY") + str.Right(str.GetLength() - 22);
		OnSend2((CString)strRerutn);
		Sleep(20);
	}
	else if (str.Left(25) == _T("OTM_SEND_AFMOTOR_INITDONE"))
	{
		CStringA strRerutn;
		strRerutn = _T("MTO_RECV_AFMOTOR_INITDONE") + str.Right(str.GetLength() - 25);
		OnSend2((CString)strRerutn);
		Sleep(20);
	}
	else if (str.Left(17) == _T("OTM_SEND_GRAB_END"))
	{
		CStringA strRerutn;
		strRerutn = _T("MTO_RECV_GRAB_END#");
		OnSend2((CString)strRerutn);
	}
	else if (str.Left(15) == _T("OTM_SEND_RESULT"))
	{
		CStringA strRerutn;
		strRerutn = _T("MTO_RECV_RESULT") + str.Right(str.GetLength() - 15);
		OnSend2((CString)strRerutn);
	}*/

    //m_listReceive.InsertString(0, (CString)str);
}

void CIBCIDlg::OnClose_C()
{
    m_pStaticStatusFriendVision->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    //UpdateData(TRUE);
    m_pStaticStatusFriendVision->SetWindowText(_T(""));
    m_pClientSock_C == nullptr;
}

void CIBCIDlg::OnConnect_C()
{
    m_pStaticStatusFriendVision->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_ON2);
    //UpdateData(TRUE);
    m_pStaticStatusFriendVision->SetWindowText(_T(""));

    /*CString strSend;

    strSend.Format(_T("Connect"));
    OnSend2(strSend);*/
}
void CIBCIDlg::OnSend()
{
    // TODO: Add your control notification handler code here

    /*UpdateData(TRUE);
	CStringA strTemp = m_strSend;
	char buf[200]={0,};

	memcpy(buf,strTemp.GetBuffer(),strTemp.GetLength());

	m_pClientSock->Send(buf, strlen(buf)+1);*/

    CString strTemp2;
    UpdateData(TRUE);
    CStringA strTemp = (CStringA)m_strSend_C;
    if (m_strSend_C == _T(""))
    {
        strTemp = strTemp2;
    }
    else
        strTemp = m_strSend_C;

    if (m_pClientSock_C)
    {
        BYTE stx = 0x05;
        BYTE etx = 0x0a;
        CString str = _T("");
        CString strLen;
        strLen.Format(_T("%04d"), strTemp.GetLength());
        //str.AppendChar(stx); // 0x05
        //str.AppendFormat(_T("0000")); // 예비
        str.AppendFormat((CString)strTemp); // 메세지길이 + 메세지
        //str.AppendChar(etx); // 0x0a
        m_pClientSock_C->Send((LPCTSTR)str, sizeof(TCHAR) * (str.GetLength() + 1));
    }
}

// 클라이언트가 보내는 애
void CIBCIDlg::OnSend2(CString strMsg_in)
{
    // TODO: Add your control notification handler code here

    CStringA strTemp = (CStringA)strMsg_in;
    BYTE stx = 0x05;
    BYTE etx = 0x0a;
    CString str = _T("");
    CString strLen;
    strLen.Format(_T("%04d"), strTemp.GetLength());
    //str.AppendChar(stx); // 0x05
    //str.AppendFormat(_T("0000")); // 예비
    str.AppendFormat((CString)strTemp); // 메세지길이 + 메세지
    //str.AppendChar(etx); // 0x0a

    //if (g_systemParameter.machineType != MachineType::UC1_mismatch)
    {
        m_pClientSock_C->Send((LPCTSTR)str, sizeof(TCHAR) * (str.GetLength() + 1));
    }
}

void CIBCIDlg::OnAccept_S()
{
    CClientSock p;
    m_pServerSock_S->Accept(p);

    CThreadSock* pThread
        = (CThreadSock*)::AfxBeginThread(RUNTIME_CLASS(CThreadSock), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    pThread->m_hSocket = p.Detach();
    //pThread->m_pClientSock = new CClientSock;
    //pThread->m_pClientSock = &p;
    pThread->ResumeThread();
    //m_clientSocks.AddTail(p);
    //p->Send("연결되었습니다.", 16);
    //m_clientSocks.AddTail(p);
    //::AfxMessageBox();
    m_bClientStatus = TRUE;

    //m_pStaticStatusFriendVision->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    m_pStaticStatusFriendVision->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_ON2);
    //UpdateData(TRUE);
    m_pStaticStatusFriendVision->SetWindowText(_T(""));
}

void CIBCIDlg::OnServerClose_S(CClientSock_S* pSock)
{
    long nError = pSock->GetLastError();
    CString str;
    str.Format(_T("%d"), nError);
    m_bClientStatus = FALSE;
    //m_pStaticStatusFriendVision->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    m_pStaticStatusFriendVision->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    m_pStaticStatusFriendVision->SetWindowText(_T(""));
    //UpdateData(TRUE);
}

void CIBCIDlg::ProcCoupleResult(const ResultMismatch& result, Share::ResultVision resultdata)
{
    {
        CSingleLock lock(&m_csResult, TRUE);
        m_loggerResult.Log(0, _T("PropCoupleResult [%d] "), result.m_nCellIndex);

        m_vecMismatchData.push_back(result);
        CString topBack;

        //topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
        //if (topBack != _T("BACK"))
        {
            m_vecResultVisionData.push_back(resultdata);

        }
    }

    ::WaitForSingleObject(m_threadControl_Result.m_Signal_Ready, INFINITE);
    ::SetEvent(m_threadControl_Result.m_Signal_Start);
}

void CIBCIDlg::ProcCoupleResult(const ResultMismatch& result)
{
    {
        CSingleLock lock(&m_csResult, TRUE);
        m_loggerResult.Log(0, _T("PropCoupleResult [%d] "), result.m_nCellIndex);

        m_vecMismatchData.push_back(result);
    }

    ::WaitForSingleObject(m_threadControl_Result.m_Signal_Ready, INFINITE);
    ::SetEvent(m_threadControl_Result.m_Signal_Start);
}


void CIBCIDlg::OnReceive_S(CClientSock_S* pSock)
{
    int len;
    BSTR bufTemp;
    CString str;
    CString str2;
    BYTE stx = 0x05;
    BYTE etx = 0x0a;
    CString strlength;

    //230517 - 조경훈 추가
    //멀티바이트로 날라오면, 변경해줘야 할듯

    {
        wchar_t temp[4096] = {
            0,
        };
        long ret = pSock->Receive(temp, 4096);
        if (ret == -1)
        {
            long nError = pSock->GetLastError();
            CString str;
            str.Format(_T("%d"), nError);
            //AfxMessageBox(str);
        }
        wchar_t* receive;
        receive = temp;
        str.Format(receive);
    }
    str2 = str;
    if (str2.Left(1) == stx)
    {
        str2.Remove(stx);
        strlength = str.Mid(5, 4);
        long nlength = _ttoi(strlength);
        str2 = str.Mid(9, nlength);
        //230920 - 조경훈, 한글포함 시 널문자 제거
        if (str2.Right(1) == _T("\n"))
        {
            str2 = str2.Mid(0, str2.GetLength() - 1);
        }
    }

    if (str.Left(7) == _T("DEFECT,"))
    {
        /*CStringA strRerutn;
		strRerutn = _T("MTO_RECV_AFMOTOR_READY") + str.Right(str.GetLength() - 22);
		OnSend2((CString)strRerutn);
		Sleep(20);*/
        CString strTemp = str.Right(str.GetLength() - 7);
        long nCnt = strTemp.Find(_T(","), 0);

        CString strStatus = strTemp.Left(nCnt);
        //long nStatus = _ttoi(strStatus);

        long nCnt2 = strTemp.Find(_T(","), nCnt + 1);
        CString strAVG_GV = strTemp.Mid(nCnt + 1, nCnt2 - nCnt);
        long nAVG_GV = _ttoi(strAVG_GV);

        long nCnt3 = strTemp.Find(_T(","), nCnt2 + 1);
        CString strMIN_GV = strTemp.Mid(nCnt2 + 1, nCnt3 - nCnt2);
        long nMIN_GV = _ttoi(strMIN_GV);

        long nCnt4 = strTemp.Find(_T(","), nCnt3 + 1);
        CString strMAX_GV = strTemp.Mid(nCnt3 + 1, nCnt4 - nCnt3);
        long nMAX_GV = _ttoi(strMAX_GV);

        long nCnt5 = strTemp.Find(_T(","), nCnt4 + 1);
        CString strBRIGHT_AVG_GV = strTemp.Mid(nCnt4 + 1, nCnt5 - nCnt4);
        long nBRIGHT_AVG_GV = _ttoi(strBRIGHT_AVG_GV);

        long nCnt6 = strTemp.Find(_T(","), nCnt5 + 1);
        CString strDARK_AVG_GV = strTemp.Mid(nCnt5 + 1, nCnt6 - nCnt5);
        long nDARK_AVG_GV = _ttoi(strDARK_AVG_GV);

        long nCnt7 = strTemp.Find(_T(","), nCnt6 + 1);
        CString strSIZE_X_mm = strTemp.Mid(nCnt6 + 1, nCnt7 - nCnt6);
        float fSIZE_X_mm = _ttof(strSIZE_X_mm);

        long nCnt8 = strTemp.Find(_T(","), nCnt7 + 1);
        CString strSIZE_Y_mm = strTemp.Mid(nCnt7 + 1, nCnt8 - nCnt7);
        float fSIZE_Y_mm = _ttof(strSIZE_Y_mm);

        long nCnt9 = strTemp.Find(_T(","), nCnt8 + 1);
        CString strAREA_Ratio = strTemp.Mid(nCnt8 + 1, nCnt9 - nCnt8);
        float fAREA_Ratio = _ttof(strAREA_Ratio);

        long nCnt10 = strTemp.Find(_T(","), nCnt9 + 1);
        CString strRATIO_YX = strTemp.Mid(nCnt9 + 1, nCnt10 - nCnt9);
        float fRATIO_YX = _ttof(strRATIO_YX);

        long nCnt11 = strTemp.Find(_T(","), nCnt10 + 1);
        CString strRATIO_XY = strTemp.Mid(nCnt10 + 1, nCnt11 - nCnt10);
        float fRATIO_XY = _ttof(strRATIO_XY);

        long nCnt12 = strTemp.Find(_T(","), nCnt11 + 1);
        CString strCOMPACT = strTemp.Mid(nCnt11 + 1, nCnt12 - nCnt11);
        float fCOMPACT = _ttof(strCOMPACT);

        long nCnt13 = strTemp.Find(_T(","), nCnt12 + 1);
        CString strRATIO_WHITE = strTemp.Mid(nCnt12 + 1, nCnt13 - nCnt12);
        float fRATIO_WHITE = _ttof(strRATIO_WHITE);

        long nCnt14 = strTemp.Find(_T(","), nCnt13 + 1);
        CString strRATIO_DARK = strTemp.Mid(nCnt13 + 1, nCnt14 - nCnt13);
        float fRATIO_DARK = _ttof(strRATIO_DARK);

        long nCnt15 = strTemp.Find(_T(","), nCnt14 + 1);
        CString strEDGE_ENERGY = strTemp.Mid(nCnt14 + 1, nCnt15 - nCnt14);
        float fEDGE_ENERGY = _ttof(strEDGE_ENERGY);

        long nCnt16 = strTemp.Find(_T(","), nCnt15 + 1);
        CString strMASS_CENTER_GV_GV = strTemp.Mid(nCnt15 + 1, nCnt16 - nCnt15);
        float fMASS_CENTER_GV_GV = _ttof(strMASS_CENTER_GV_GV);

        long nCnt17 = strTemp.Find(_T(","), nCnt16 + 1);
        CString strLotID = strTemp.Mid(nCnt16 + 1, nCnt17 - nCnt16 - 1);

        long nCnt18 = strTemp.Find(_T(","), nCnt17 + 1);
        CString strCellID = strTemp.Mid(nCnt17 + 1, nCnt18 - nCnt17);
        long nCellID = _ttof(strCellID);

        long nCnt19 = strTemp.Find(_T(","), nCnt18 + 1);
        CString strTime = strTemp.Mid(nCnt18 + 1, nCnt19 - nCnt18 - 1);

        long nCnt20 = strTemp.Find(_T(","), nCnt19 + 1);
        CString strDefectType = strTemp.Mid(nCnt19 + 1, nCnt20 - nCnt19 - 1);
        long defectType = _ttof(strDefectType);

        long nCnt21 = strTemp.Find(_T(","), nCnt20 + 1);
        CString strTopBack = strTemp.Mid(nCnt20 + 1, nCnt21 - nCnt20 - 1);
        long nTopBack = _ttof(strTopBack);

        long nCnt22 = strTemp.Find(_T(","), nCnt21 + 1);
        CString strDefectX = strTemp.Mid(nCnt21 + 1, nCnt22 - nCnt21 - 1);
        long nDefectX = _ttof(strDefectX);

        long nCnt23 = strTemp.Find(_T(","), nCnt22 + 1);
        CString strDefectY = strTemp.Mid(nCnt22 + 1, nCnt23 - nCnt22 - 1);
        long nDefectY = _ttof(strDefectY);

        CISIViewerProperty temp;

        temp.nAVG_GV = nAVG_GV;
        temp.nMIN_GV = nMIN_GV;
        temp.nMAX_GV = nMAX_GV;
        temp.nBRIGHT_AVG_GV = nBRIGHT_AVG_GV;
        temp.nDARK_AVG_GV = nDARK_AVG_GV;
        temp.fSIZE_X_mm = fSIZE_X_mm;
        temp.fSIZE_Y_mm = fSIZE_Y_mm;
        temp.fAREA_Ratio = fAREA_Ratio;
        temp.fRATIO_YX = fRATIO_YX;
        temp.fRATIO_XY = fRATIO_XY;
        temp.fCOMPACT = fCOMPACT;
        temp.fRATIO_WHITE = fRATIO_WHITE;
        temp.fRATIO_DARK = fRATIO_DARK;
        temp.fEDGE_ENERGY = fEDGE_ENERGY;
        temp.fMASS_CENTER_GV_GV = fMASS_CENTER_GV_GV;
        temp.strLotID = strLotID;
        temp.nCellIndex = nCellID;
        temp.strTime = strTime;
        temp.m_nCandidateDefectCode = defectType;
        temp.nUp = nTopBack;
        temp.nDefectX = nDefectX;
        temp.nDefectY = nDefectY;

        CSingleLock lock(&m_csPropertyView, TRUE);
        m_vecReceiveData.push_back(temp);
        lock.Unlock();
        m_inlineResultUiUpdater->Trigger();
    }
    else if (str.Left(12) == _T("StartCellID,"))
    {
        TRACE(str + _T ("\n"));
        str = str.Right(str.GetLength() - 12);
        int nCellIndex = _ttoi(str);

        long sensorID = m_pSyncIO->GetCell_ID();
        if (nCellIndex < sensorID)
        {
            m_nCellID_finalcalc = sensorID - 63;
            m_nReceiveCellIndex = nCellIndex;
        }


    }
    else if (str.Left(10) == _T("MISSMATCH,"))
    {
        TRACE(str + _T ("\n"));
        /*CStringA strRerutn;
		strRerutn = _T("MTO_RECV_AFMOTOR_READY") + str.Right(str.GetLength() - 22);
		OnSend2((CString)strRerutn);
		Sleep(20);*/
        str = str.Right(str.GetLength() - 10);
        ResultMismatch receiveResMismatch;

        long nCnt = str.Find(_T(","), 0);
        CString strLotID = str.Left(nCnt);

        receiveResMismatch.m_strLotID = strLotID;

        long nCnt2 = str.Find(_T(","), nCnt + 1);
        CString strCellIndex = str.Mid(nCnt + 1, nCnt2 - nCnt);

        int nCellIndex = _ttoi(strCellIndex);
        receiveResMismatch.m_nCellIndex = nCellIndex;

        long nCnt3 = str.Find(_T(","), nCnt2 + 1);
        CString strDirection = str.Mid(nCnt2 + 1, nCnt3 - nCnt2);

        int nDirection = _ttoi(strDirection);

        long nCnt4 = str.Find(_T(","), nCnt3 + 1);
        CString strVecCnt = str.Mid(nCnt3 + 1, nCnt4 - nCnt3);
        int nVecCnt = _ttoi(strVecCnt);

        long nTemp = nCnt4;
        long nTemp2;
        std::vector<float> vecfData;
        std::vector<float> vecfData2;

        if (nDirection == 0) // top
        {
            receiveResMismatch.m_vecResUpLane.resize(nVecCnt);
        }
        else
        {
            receiveResMismatch.m_vecResDownLane.resize(nVecCnt);
        }
        for (int n = 0; n < nVecCnt; n++)
        {
            /*float fMeasureStandard = 0.f;

			float fMeasureOffsetUCL = 0.f;
			float fMeasureOffsetLCL = 0.f;

			float fMeasureOffsetUSL = 0.f;
			float fMeasureOffsetLSL = 0.f;*/
            nTemp2 = str.Find(_T(","), nTemp + 1);
            CString strVecCnt = str.Mid(nTemp + 1, nTemp2 - nTemp);
            vecfData.push_back(_ttof(strVecCnt));
            nTemp = nTemp2;

            if (nDirection == 0) // top
            {
                receiveResMismatch.m_vecResUpLane[n].fLaneWidth = vecfData[n];
            }
            else if (nDirection == 1) // back
            {
                receiveResMismatch.m_vecResDownLane[n].fLaneWidth = vecfData[n];
            }
        }
        nTemp2 = str.Find(_T(","), nTemp + 1);
        CString strFullLength = str.Mid(nTemp + 1, nTemp2 - nTemp);
        float fFull = _ttof(strFullLength);
        if (nDirection == 0)
        {
            receiveResMismatch.m_fTopFullLength = fFull;
        }
        else if (nDirection == 1)
        {
            receiveResMismatch.m_fBackFullLength = fFull;
        }

        m_inlineResultCoupler->Push2(receiveResMismatch);
        m_loggerResult.Log(0, str);
    }
    else if (str.Left(12) == _T("InlineStart"))
    {
        OnInlineStart();
    }
    else if (str.Left(11) == _T("InlineStop"))
    {
        OnInlineStop();
    }
    else if (str.Left(12) == _T("StartCellID,"))
    {
        str = str.Right(str.GetLength() - 12);
        m_nReceiveCellIndex = _ttoi(str);
    }
    if (str.Left(7) == _T("Connect"))
    {
        str = str.Right(str.GetLength() - 7);
        GetDlgItem(IDC_BUTTON_INSP_START)->EnableWindow(FALSE);
    }
    /*CString strSend;
	CString strData;
	strSend = stResAllMismatch.m_strLotID + _T(",");

	strData.Format(_T("%d,"), stResAllMismatch.m_nCellIndex);
	strSend += strData;
	strData.Format(_T("%d,"), g_systemParameter.bDirection);
	strSend += strData;
	strData.Format(_T("%d,"), resvecLaneUp.size());
	strSend += strData;


	for (int i = 0; i < resvecLaneUp.size(); i++)
	{
		strData.Format(_T("%f,"), resvecLaneUp[i].fLaneWidth);
		strSend += strData;
	}*/

    /*ResultMismatch receiveResMismatch;


	long nCnt = str.Find(_T(","), 0);
	CString strLotID = str.Left(nCnt);

	receiveResMismatch.m_strLotID = strLotID;

	long nCnt2 = str.Find(_T(","), nCnt+1);
	CString strCellIndex = str.Mid(nCnt+1, nCnt2 - nCnt);

	int nCellIndex = _ttoi(strCellIndex);
	receiveResMismatch.m_nCellIndex = nCellIndex;

	long nCnt3 = str.Find(_T(","), nCnt2+1);
	CString strDirection = str.Mid(nCnt2+1, nCnt3 - nCnt2);

	int nDirection = _ttoi(strDirection);


	long nCnt4 = str.Find(_T(","), nCnt3+1);
	CString strVecCnt = str.Mid(nCnt3+1, nCnt4 - nCnt3);
	int nVecCnt = _ttoi(strVecCnt);

	long nTemp = nCnt4;
	long nTemp2;
	std::vector<float> vecfData;*/
}

void CIBCIDlg::OnSend_S()
{
    CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    long nSizeDivideVertical = (long)unitUp->m_ISICellAllDefect.m_vvMeasureDefect.size();
    auto& vecMeasureDefectUp = unitUp->m_ISICellAllDefect.m_vvMeasureDefect;
}

void CIBCIDlg::OnSend2_S(CString strMsg_in)
{
    // TODO: Add your control notification handler code here
    CStringA strTemp = (CStringA)strMsg_in;

    BYTE stx = 0x05;
    BYTE etx = 0x0a;
    CString str = _T("");
    CString strLen;
    strLen.Format(_T("%04d"), strTemp.GetLength());
    //str.AppendChar(stx); // 0x05
    //str.AppendFormat(_T("0000")); // 예비
    str.AppendFormat((CString)strTemp); // 메세지길이 + 메세지
    //str.AppendChar(etx); // 0x0a
    Send_S(str);
}

void CIBCIDlg::Send_S(CString strMsg)
{
    POSITION pos = m_clientSocks_S.GetHeadPosition();
    while (pos != NULL)
    {
        CClientSock_S* p = m_clientSocks_S.GetNext(pos);
        p->Send((LPCTSTR)strMsg, sizeof(TCHAR) * (strMsg.GetLength() + 1));
    }
}

BOOL CThreadSock::InitInstance()
{
    CIBCIDlg* pParent = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
    m_pClientSock_S = new CClientSock_S;
    m_pClientSock_S->Attach(m_hSocket);
    m_pClientSock_S->m_pThread = this;
    pParent->m_clientSocks_S.AddTail(m_pClientSock_S);
    m_pos = pParent->m_clientSocks_S.GetTailPosition();
    return TRUE;
}

int CThreadSock::ExitInstance()
{
    // TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
    CIBCIDlg* pParent = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
    if (pParent != NULL)
        pParent->m_clientSocks_S.RemoveAt(m_pos);
    if (m_pClientSock_S != nullptr)
        delete m_pClientSock_S;
    m_pClientSock_S = NULL;
    return CWinThread::ExitInstance();
}

std::vector<CString> CIBCIDlg::Msg_Pars(CString strMsg_in)
{
    std::vector<CString> vecstrPars;
    vecstrPars.resize(0);
    CString strCheckLength = _T(""), strTest = _T(""), strlength = _T("");
    long nCheckBefore = 0, nCheckAfter = 0, nCntSTX = 0, nPos = 0;
    strCheckLength = strMsg_in;
    nCheckBefore = strCheckLength.GetLength();
    nPos = strCheckLength.Find(',', 1);
    if (nPos <= -1)
        return std::vector<CString>();
    CString strTemp = strCheckLength.Mid(nPos + 1, strCheckLength.GetLength());
    //nCheckAfter = str2.GetLength();
    strCheckLength = strTemp;
    while (1)
    {
        nCheckBefore = strCheckLength.GetLength();
        if (strCheckLength.Find(',') <= -1)
        {
            if (nCheckBefore > 0)
            {
                vecstrPars.emplace_back(strCheckLength.Mid(0, nCheckBefore - 1));
            }
            break;
        }
        long nlength = strCheckLength.Find(',');
        vecstrPars.emplace_back(strCheckLength.Mid(0, nlength));
        strCheckLength.Delete(0, strCheckLength.Find(',', 1) + 1);
        nCheckAfter = vecstrPars.back().GetLength();
    }

    return vecstrPars;
}

void CIBCIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CIBCIDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 아이콘을 그립니다.
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();

        //if (m_bProgramOpened == TRUE)
        {
            /*ImportDefectCountFile();
            ImportMismatchGraphFile();
            ImportDefectMapFile();*/
            //OnBnClickedButtonExportData();
        //    m_bProgramOpened = FALSE;
        }
        //ImportDefectMapFile();
    }
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CIBCIDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CIBCIDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // Separate Area
    if (m_bInitUI)
    {
        CRect rcClient(0, 0, cx, cy);
        RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, 0, 0, &rcClient);
        RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &rcClient, &rcClient);

        UpdateControlLayer();

        //if (::IsWindow(m_wndThemesLabel.m_hWnd))
        //{
        //	CRect rc;
        //	m_wndThemesLabel.GetWindowRect(&rc);
        //	ScreenToClient(&rc);

        //	int x = rcClient.left;
        //	int y = rcClient.top + XTP_DPI_Y(5);
        //	int cx = rc.Width();
        //	int cy = rc.Height();

        //	rc.SetRect(CPoint(x, y), CPoint(x + cx, y + cy));
        //	m_wndThemesLabel.MoveWindow(rc);
        //	m_wndThemesLabel.Invalidate(FALSE);
        //	rcClient.top += rc.bottom + XTP_DPI_Y(5);
        //}

        //if (::IsWindow(m_wndThemesList.m_hWnd))
        //{
        //	m_wndThemesList.MoveWindow(rcClient);
        //}
    }
}

static int nInit = 0;
LRESULT CIBCIDlg::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
    //if (wParam == XTP_DPN_ACTION) // Pane Action Disable
    //{
    //    XTP_DOCKINGPANE_ACTION* pAction = (XTP_DOCKINGPANE_ACTION*)lParam;
    //    pAction->bCancel = TRUE;
    //    return TRUE;
    //}

    if (wParam == XTP_DPN_SHOWWINDOW)
    {
        // get a pointer to the docking pane being shown.
        CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
        if (!pPane->IsValid())
        {
            int nIndex = -1;

            switch (pPane->GetID())
            {
                case IDR_PANE1:
                    nIndex = 0;
                    pPane->Attach(m_pDlgAnimationWindow);
                    break;
                case IDR_PANE2:
                    nIndex = 1;
                    pPane->Attach(m_dlgDefectmap);
                    break;
                case IDR_PANE3:
                    nIndex = 2;
                    pPane->Attach(m_pDlgGraphWindow);
                    break;
                case IDR_PANE4:
                    nIndex = 3;
                    pPane->Attach(m_dlgNormalSpecView);
                    break;
                case IDR_PANE5:
                    nIndex = 4;
                    pPane->Attach(m_dlgDefectCount);
                    break;
                case IDR_PANE6:
                    nIndex = 5;
                    pPane->Attach(m_dlgDefectView);
                    break;
                case IDR_PANE7:
                    nIndex = 6;
                    //pPane->Attach(m_dlgEdgeView);
                    pPane->Attach(m_pDlgLogDisplay);
                    break;
                case IDR_PANE8:
                    nIndex = 7;
                    pPane->Attach(m_pViewGrabImage);
                    break;
                case IDR_PANE9:
                    nIndex = 8;
                    pPane->Attach(m_pViewCellImage);
                    break;
            }

            //pPane->Attach(&m_wndPaneDlg[nIndex]);

            //int nIndex = pPane->GetID() - IDR_PANE1;
            //ASSERT(nIndex >= 0 && nIndex < sizeof(m_wndEdit) / sizeof(m_wndEdit[0]));
            //CEdit& wndEdit = m_wndEdit[nIndex];
            //
            //// create and attach the edit control for this pane.
            //if (!::IsWindow(wndEdit.m_hWnd))
            //{
            //	// Edit Control Attach
            //	//if (!wndEdit.CreateEx(WS_EX_STATICEDGE, _T("EDIT"), _T(""),
            //	//	WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
            //	//	XTP_DPI(CRect(0, 0, 200, 120)), this, pPane->GetID()))
            //	//{
            //	//	TRACE0("Error creating pane edit control.\n");
            //	//}
            //	//wndEdit.SetFont(&m_fntEdit);
            //}
            ////pPane->Attach(&wndEdit);
        }

        return TRUE; // handled
    }

    return FALSE;
}

LRESULT CIBCIDlg::OnKickIdle(WPARAM, LPARAM)
{
    if (m_paneManager.GetSafeHwnd())
        m_paneManager.UpdatePanes();

    if (m_wndStatusBar.GetSafeHwnd())
        m_wndStatusBar.SendMessage(WM_IDLEUPDATECMDUI, TRUE);

    return 0;
}

void CIBCIDlg::OnClose()
{
    //CDialog::OnClose();
    ::SetEvent(m_Signal_Kill_UiUpdater);
    ThreadStop_InlineInspeciton();
    ::SetEvent(m_threadControl_Result.m_Signal_Kill);

    // 메시지 펌핑 - 워커 스레드가 SendMessage 중일 때 교착상태 방지
    // SetEvent 후 워커가 kill 체크하기 전에 이미 SendMessage에 진입한 경우,
    // 메인 스레드가 해당 메시지를 처리해줘야 워커가 반환할 수 있음
    MSG msg;
    DWORD dwStart = ::GetTickCount();
    while (::GetTickCount() - dwStart < 500)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            ::Sleep(10);
        }
    }

    // InlineResultUiUpdater 스레드 종료 대기 후 삭제 (워커가 이미 종료된 상태)
    if (m_inlineResultUiUpdater)
    {
        delete m_inlineResultUiUpdater;
        m_inlineResultUiUpdater = nullptr;
    }
    if (m_inlineResultUiUpdater_View)
    {
        delete m_inlineResultUiUpdater_View;
        m_inlineResultUiUpdater_View = nullptr;
    }
    if (m_inlineResultUiUpdater_Map)
    {
        delete m_inlineResultUiUpdater_Map;
        m_inlineResultUiUpdater_Map = nullptr;
    }
    if (m_inlineResultUiUpdater_Count)
    {
        delete m_inlineResultUiUpdater_Count;
        m_inlineResultUiUpdater_Count = nullptr;
    }

    OnCancel();
}



void CIBCIDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    switch (nIDEvent)
    {
        case TIMER_ID_CLOCK:
            if (true)
            {
                CTime time = CTime::GetCurrentTime();
                //	if (g_systemParameter.nLanguage == 1)
                {
                    setlocale(LC_ALL, "korean");
                }
                //else if (g_systemParameter.nLanguage == 3)
                //{
                //	setlocale(LC_ALL, "chinese");
                //}
                //else
                //{
                //	setlocale(LC_ALL, "english");
                //}

                CString szTime = time.Format("%Y-%m-%d %a %H:%M:%S");
                m_pStaticCurTime->SetWindowText(szTime);
            }
            break;
        case TIMER_ID_UPDATE_GRAB_QUEUE_STATUS:
        {
            CString grabQueueState(_T("GRAB QUEUE STATUS  "));

		    for (long cameraIndex = 0; cameraIndex < m_cisComm->GetCameraCount(); cameraIndex++)
            {
                const auto* executor = m_cisComm->GetGrabExcutor(cameraIndex);

                if (executor == nullptr)
                    break;

                int64_t headFrameIndex = 0;
                int64_t tailFrameIndex = 0;

                executor->GetUpdatedFrameIndices(headFrameIndex, tailFrameIndex);

                const auto circularBufferCount = executor->GetCircularBufferCount();

                CString str;
                str.Format(_T(":: %s [ %lld / %d ]  "), cameraIndex == 0 ? _T("cam1") : _T("cam2"),
                    tailFrameIndex - headFrameIndex, circularBufferCount);

                grabQueueState.AppendFormat(_T("%s"), str);
            }
            int64_t headFrameIndex = 0;
            int64_t tailFrameIndex = 0;
            if (m_cellImageMerger)
            {
                m_cellImageMerger->GetUpdatedFrameIndices(headFrameIndex, tailFrameIndex);
                grabQueueState.AppendFormat(_T(":: MERGER [ %lld / %d ]"), tailFrameIndex - headFrameIndex, m_cellImageMerger->GetCircularBufferCount());
            }
            grabQueueState.AppendFormat(_T(":: CellId [ %d ]"), m_nCellId);
            m_pStaticQueueStatus->SetWindowText(grabQueueState);
        }
        break;
        case TIMER_ID_UPDATE_CPUMEMORY_STATUS:
        {

            CString strDrive;
            strDrive.Format(_T("C:"));
            ULARGE_INTEGER _64FreeBytesToCaller, _64TotalBytes, _64FreeBytes;
            GetDiskFreeSpaceEx(strDrive, &_64FreeBytesToCaller, &_64TotalBytes, &_64FreeBytes);
            double fTempC = (double)_64FreeBytes.QuadPart / (double)_64TotalBytes.QuadPart;
            fTempC = 100 - (fTempC * 100);
            strDrive.Format(_T("D:"));

            if (g_systemParameter.strPathImageSave == _T(""))
            {
                strDrive.Format(_T("D:"));
            }
            else
            {
                CString strTemp;
                strTemp = g_systemParameter.strPathImageSave.Mid(0, 2);
                strDrive.Format(strTemp);
            }

            GetDiskFreeSpaceEx(strDrive, &_64FreeBytesToCaller, &_64TotalBytes, &_64FreeBytes);

            double dUsageHDD = (double)_64TotalBytes.QuadPart - (double)_64FreeBytes.QuadPart;
            
            double fTempD = dUsageHDD / (double)_64TotalBytes.QuadPart * 100;
            //fTempD = 100 - (fTempD );
            //float fCPU = (float)getCurrentValueTotal();
            float fCPU = (float)getCurrentValue();

            MEMORYSTATUS memoryStatus;
            GlobalMemoryStatus(&memoryStatus);

            CString memoryInfo;
            double fMemory = (double)memoryStatus.dwAvailPhys / (double)memoryStatus.dwTotalPhys;
            fMemory = 100 - (fMemory * 100);

            double dTempereature = 0.0f;
            CString strCurState;
            if(m_cisComm->GetCameraCount() > 0)
            {
                dTempereature = m_cisComm->GetGrabExcutor(0)->GetFPGATemperature();
  
                strCurState.Format(_T("Usage:: CPU : %.1f      Memory : %.1f      %sDrive : %.1f     FPGA:: ")_T("Temperature : %.1f"),
                    fCPU, fMemory, strDrive, fTempD, dTempereature);
                //strCurState.Format(_T("Current CPU : %.1f      Memory : %.1f      Driver Usage Space : %.1f     FPGA Temperature : %.1f"),fCPU, fMemory, fTempD, dTempereature);

                if (g_systemParameter.machineType == MachineType::UC1_mismatch)
                {
                    strCurState.Format(_T("Current CPU : %.1f      Memory : %.1f      Driver Usage Space : %.1f     "),
                        fCPU, fMemory, fTempD);
                }
            }
            else
            {
                strCurState.Format(_T("Current CPU : %.1f      Memory : %.1f      %sDrive : %.1f     FPGA Temperature: NONE"), 
                    fCPU, fMemory, strDrive, fTempD);
            }
            //double dTempereature = m_cisComm->GetGrabExcutor(0)->GetFPGATemperature();
            m_pStaticMemoryStatus->SetWindowText(strCurState);

        }
        case TIMER_ID_UPDATE_RESULT_UI:
        {
            //double dTempereature = m_cisComm->GetGrabExcutor(0)->GetFPGATemperature();
            //m_cisComm->GetFPGATemperature();
        }
        case TIMER_ID_CAMERA_CONNECT_CHECK:
        {
            if (m_cisComm->GetExecutorType()==ExecutorType::Sapera && m_cisComm->GetCameraCount() > 0)
            {
                if (m_cisComm->GetGrabExcutor(0)->GetCameraConnectCheck())
                {
                    m_pStaticStatusCamera->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_ON2);
                }
                else
                {
                    m_pStaticStatusCamera->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
                }
                m_pStaticStatusCamera->Invalidate(FALSE);
            }
        }
        break;
        
        default:
            break;
    }
}

void CIBCIDlg::OnUpdateKeyIndicator(CCmdUI* pCmdUI)
{
    UINT nVK;
    UINT flag = 0x0001;

    switch (pCmdUI->m_nID)
    {
        case ID_INDICATOR_CAPS:
            nVK = VK_CAPITAL;
            break;

        case ID_INDICATOR_NUM:
            nVK = VK_NUMLOCK;
            break;

        case ID_INDICATOR_SCRL:
            nVK = VK_SCROLL;
            break;

        default:
            TRACE1("Warning: OnUpdateKeyIndicator - unknown indicator 0x%04X.\n", pCmdUI->m_nID);
            pCmdUI->ContinueRouting();
            return; // not for us
    }

    pCmdUI->Enable(::GetKeyState(nVK) & flag);
    // enable static text based on toggled key state
    ASSERT(pCmdUI->m_bEnableChanged);
}

void CIBCIDlg::OnSelChangeListThemes()
{
    int nItem = m_wndThemesList.GetCurSel();
    if (nItem != LB_ERR)
    {
        DOCKINGPANETHEME& paneTheme = *(DOCKINGPANETHEME*)m_wndThemesList.GetItemDataPtr(nItem);

        switch (paneTheme.eTheme)
        {
            case xtpPaneThemeVisualStudio2012:
                XTPResourceImages()->SetHandle(m_hModule2012, _T("VisualStudio2012Dark.INI"));
                break;
            case xtpPaneThemeOffice2013:
                XTPResourceImages()->SetHandle(m_hModule2013, _T("OFFICE2013WHITE.INI"));
                break;
            default:
                XTPResourceImages()->SetHandle(AfxGetResourceHandle());
                break;
        }

        m_paneManager.SetTheme(paneTheme.eTheme);
    }
}

void CIBCIDlg::Inspection(CString strPath, CString strPath2, long nIndex, BOOL bIsUp)
{    
    BOOL isUp = bIsUp;
    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);

    IPVM::Image_8u_C1 imageFull;
    //imageFull.LoadFrom(strPath);

    IPVM::Image_8u_C1 imageBright, imageDark;
   // strPath.Replace(_T("_DARK_"), _T("_BRIGHT_"));
    imageBright.LoadFrom(strPath);
    //strPath.Replace(_T("_BRIGHT_"), _T("_DARK_"));
    imageDark.LoadFrom(strPath2);
    m_pViewCellImage->m_imageViews[0]->SetImage(imageBright);
    m_pViewCellImage->m_imageViews[1]->SetImage(imageDark);
    //if (g_systemParameter.machineType == MachineType::ES_mismatch)
    //{
    //    // MSA 데이터 추출 시 사용
    //    UtilityImage::CreateBrightDarkImage_MSA(imageFull, imageBright, imageDark, false);
    //}
    //else
    //{
    //    UtilityImage::CreateBrightDarkImage(imageFull, imageBright, imageDark, false);
    //}
    //UtilityImage::CreateBrightDarkImage(imageFull, imageBright, imageDark, false);
    unit->m_cellID = nIndex;
    unit->m_ISICellAllDefect.m_nCellindex = nIndex;
    unit->m_ISICellAllDefect.strLotID = _T("test1234");

    //unit->m_inspectionImageFull = imageFull;
    unit->SetInspectionSplitImage(imageBright, imageDark);

    unit->m_bInlineTest = false;
    unit->DoInspection();

    // SurfaceInspection 인 경우
    if (m_viewCellImageUpdater->m_bSurfaceInspCheck == TRUE && m_pViewCellImage->m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::DO_NOT_UPDATE
        || m_bInline == FALSE)
    {
        m_pViewCellImage->m_InspectionCell = unit;
    }

    // batch의 경우
    if (m_viewCellImageUpdater->m_bSurfaceInspCheck == FALSE && m_pViewCellImage->IsWindowVisible())
    {
        m_pViewCellImage->m_InspectionCell = unit;  
        m_viewCellImageUpdater->SetInspectionCell(unit);
        m_viewCellImageUpdater->SetGrayMode(m_pViewCellImage->m_nModeGray);
        m_viewCellImageUpdater->SetInspMode(0, m_bInline, m_bBatchView);
        m_viewCellImageUpdater->Push(imageBright, imageDark, unit->m_cellID, unit->m_ISICellAllDefect);
    }   

    /*if (m_viewCellImageUpdater->m_bSurfaceInspCheck == FALSE && m_pViewCellImage->IsWindowVisible()
        && m_pViewCellImage->m_nModeUpdate != ENUM_VIEW_MODE_CELL_IMAGE::DO_NOT_UPDATE)
    {
    }*/

    if (unit->m_resultValue == II_RESULT_VALUE::INVALID && g_systemParameter.machineType == MachineType::UC1_mismatch)
    {
        long n = 0;
        return;
    }
    //===========================================================================================
    // [ Surface Defect Result Display ]
    //
    BOOL SaveNG=false;
    long nSizeDivideVertical = (long)unit->m_ISICellAllDefect.m_vvMeasureDefect.size();
    auto& vecMeasureDefect = unit->m_ISICellAllDefect.m_vvMeasureDefect;

    CString strDirectPath, strRows;
    CString strCsvFolderPath, strCsvNoCellIDPath;
    SYSTEMTIME stCreateTime;
    if (!g_systemParameter.pathResultDataCsv.IsEmpty())
    {
        //CString lotId = _T("LOT");
        //if (!unit->m_ISICellAllDefect.strLotID.IsEmpty())
        //    lotId.Format(_T("%s"), unit->m_ISICellAllDefect.strLotID);
        //CString topBack;
        //topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
        //strDirectPath.Format(
        //    _T("%s\\%s\\%s\\%010d"), g_systemParameter.pathResultDataCsv, topBack, lotId, unit->m_cellID);
        CString lotId = _T("LOT");
        if (!unit->m_ISICellAllDefect.strLotID.IsEmpty())
            lotId.Format(_T("%s"), unit->m_ISICellAllDefect.strLotID);
        CString topBack;
        //topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
        topBack = !bIsUp ? _T("TOP") : _T("BACK");

        FILETIME timeCreate;
        FILETIME timeAccess;
        FILETIME timeWrite;

        HANDLE hFile = NULL;
        hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        BOOL bRet = GetFileTime(hFile, &timeCreate, &timeAccess, &timeWrite);


        bRet = FileTimeToSystemTime(&timeCreate, &stCreateTime);


        //CString strYear;
        //CString strMonth;
        //CString strDay;
        //std::vector<CString> vecFilePath;
        //StringTokenDivideToCString(strPath, '\\', vecFilePath);

        //strYear = vecFilePath[vecFilePath.size() -4];
        //strMonth = vecFilePath[vecFilePath.size() - 3];
        //strDay = vecFilePath[vecFilePath.size() - 2];

        CString strCellID;
        CString strSurfaceFolder = _T("Surface_Inspection");
        strCellID.Format(_T("%04d%02d%02d%02d%02d%02d"), stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay);
        if (m_pViewCellImage->m_bSurfaceInsp) 
        {
            if (g_systemParameter.bBatchFolderTree) // Current
            {
                GetLocalTime(&stCreateTime);
                strDirectPath.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\%s\\%s\\%09d"), g_systemParameter.pathResultDataCsv,
                    strSurfaceFolder, stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, topBack, lotId,
                    unit->m_cellID);
            }
            else if (!g_systemParameter.bBatchFolderTree) // ImageTime
            {
                strDirectPath.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\%s\\%s\\%09d"), g_systemParameter.pathResultDataCsv,
                    strSurfaceFolder, stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, topBack, lotId,
                    unit->m_cellID);
            }
        }
        else
        {
            if (g_systemParameter.bBatchFolderTree) // Current
            {
                GetLocalTime(&stCreateTime);
                strDirectPath.Format(_T("%s\\%04d\\%02d\\%02d\\%s\\%s\\%09d"), g_systemParameter.pathResultDataCsv,
                    stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, topBack, lotId, unit->m_cellID);
            }
            else if (!g_systemParameter.bBatchFolderTree) // ImageTime
            {
                strDirectPath.Format(_T("%s\\%04d\\%02d\\%02d\\%s\\%s\\%09d"), g_systemParameter.pathResultDataCsv,
                    stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, topBack, lotId, unit->m_cellID);
            }
        }

        // result/2024_01/10/Top/Data/Measure.csv
        //CString strPathTemp;
        /*strDirectPath.Format(_T("%s\\%04d_%02d\\%02d\\%s\\Data\\%09d"), g_systemParameter.pathResultDataCsv,
            stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, topBack, unit->m_cellID);*/

        strCsvFolderPath.Format(_T("%s\\%04d_%02d\\%02d\\%02d\\%s\\Data\\%09d"), g_systemParameter.pathResultDataCsv,
            stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, stCreateTime.wHour, topBack, unit->m_cellID);

        m_strFolderTime.Format(_T("%04d_%02d\\%02d\\%02d"), stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay,
            stCreateTime.wHour);

        strCsvNoCellIDPath.Format(_T("%s\\%04d_%02d\\%02d\\%02d\\%s\\Data"), g_systemParameter.pathResultDataCsv,
            stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, stCreateTime.wHour, topBack);

        IPVM::CreateDirectories(strCsvNoCellIDPath);
        //strDirectPath.Format(_T("%s\\%s\\%s\\%s\\%s\\%s\\%010d"), g_systemParameter.pathResultDataCsv, strYear,
        //    strMonth, strDay, topBack,
        //    lotId, unit->m_cellID);
    }
    else
    {
        FILETIME timeCreate;
        FILETIME timeAccess;
        FILETIME timeWrite;

        HANDLE hFile = NULL;
        hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        BOOL bRet = GetFileTime(hFile, &timeCreate, &timeAccess, &timeWrite);

        bRet = FileTimeToSystemTime(&timeCreate, &stCreateTime);

        CString lotId = _T("LOT");
        if (!unit->m_ISICellAllDefect.strLotID.IsEmpty())
            lotId.Format(_T("%s"), unit->m_ISICellAllDefect.strLotID);

        CString topBack;
        topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
        strDirectPath.Format(_T("D:\\RESULT\\%s\\%s\\%010d"), topBack, lotId, unit->m_cellID);

        strCsvFolderPath.Format(_T("D:\\result\\%04d_%02d\\%02d\\%02d\\%s\\Data\\%09d"), stCreateTime.wYear,
            stCreateTime.wMonth, stCreateTime.wDay, stCreateTime.wHour, topBack, unit->m_cellID);

        strCsvNoCellIDPath.Format(_T("D:\\result\\%04d_%02d\\%02d\\%02d\\%s\\Data"), stCreateTime.wYear,
            stCreateTime.wMonth, stCreateTime.wDay, stCreateTime.wHour, topBack);

        IPVM::CreateDirectories(strCsvNoCellIDPath);
    }
    BOOL bResult[100] = {FALSE,};
    std::vector<IPVM::Rect> vecDefectROI;
    std::vector<IPVM::Image_8u_C1> vecCropImg_Bright;
    std::vector<IPVM::Image_8u_C1> vecCropImg_Dark;
    for (long n = 0; n < unit->m_ISICellAllDefect.m_vecCandidate.size(); n++)
    {

        IPVM::Rect rtDraw = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel;

        IPVM::Rect rect = rtDraw;
        
        int OriginRect_Width = rect.Width();
        int OriginRect_Height = rect.Height();
        int Offset_X = 0;
        int Offset_Y = 0;
        if (100 > max(rect.Width(), rect.Height()))
        {
            Offset_X = 50;
            Offset_Y = 50;
        }
        else if (rect.Width() > rect.Height())
        {
            int Diff = rect.Width() - rect.Height();
            int Diff_half = Diff / 2;
            Offset_Y = Diff_half;
        }
        else
        {
            int Diff = rect.Height() - rect.Width();
            int Diff_half = Diff / 2;
            Offset_X = Diff_half;
        }
        rect.InflateRect(Offset_X, Offset_Y, Offset_X, Offset_Y);
        rect &= IPVM::Rect(imageBright);
        vecDefectROI.push_back(rect);
        int offset_left = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_left - rect.m_left;
        int offset_top = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_top - rect.m_top;
        int offset_right = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_right - rect.m_right;
        int offset_bottom = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_bottom - rect.m_bottom;

        IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
        IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());

        IPVM::Rect OriginRect(CropImage_Bright);
        OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);

        IPVM::ImageProcessing::Copy(imageBright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);
        IPVM::ImageProcessing::Copy(imageDark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);

        if ((II_RESULT_VALUE)unit->m_ISICellAllDefect.m_vecISIResultCode[n] == II_RESULT_VALUE::PASS)
        {
            continue;
        }
        SaveNG = true;
        IPVM::CreateDirectories(strDirectPath);
        CString strCreateTime;
        CString strInspTime;
        if (g_systemParameter.bBatchFolderTree) // Current
        {
            GetLocalTime(&stCreateTime);
            strInspTime.Format(_T("%d%d%d%d"), stCreateTime.wHour, stCreateTime.wMinute, stCreateTime.wSecond,stCreateTime.wMilliseconds);
            strCreateTime.Format(_T("%04d%02d%02d%s"), stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, strInspTime);
        }
        if (!g_systemParameter.bBatchFolderTree) // Defalut
        {
            strInspTime = strPath.Mid(strPath.ReverseFind('\\'));
            strInspTime = strInspTime.Mid(9, 9);
            strCreateTime.Format(_T("%04d%02d%02d%s"), stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, strInspTime);
        }
        m_strInspTime = strCreateTime;
        SPathCropImages sPathCropImages = SaveCropImages(unit, n, imageBright, imageDark, CropImage_Bright, CropImage_Dark, strDirectPath);
        CISIViewerProperty propertyView = MakeISIViewerProperty(unit, n, CropImage_Bright, CropImage_Dark, rtDraw, sPathCropImages);   
        propertyView.strTime = strCreateTime; // 해당 이미지의 BrightPath 시간
        //strCreateTime.Format(_T("%04d%02d%02d%09d"), stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, unit->m_cellID);

        strRows.AppendFormat(_T("%s\n"), MakeCsvRowString(n, propertyView, unit));
        bResult[unit->m_ISICellAllDefect.m_vecISIDefectType[n]] = TRUE;

        // 호스트에게 보내 줄 Crop Image 를 여기에서 넣어준다.
        IPVM::Image_8u_C1 ImgCrop_bright(128, 128);
        IPVM::ImageProcessing::Resize_Linear(CropImage_Bright, ImgCrop_bright);

        IPVM::Image_8u_C1 ImgCrop_dark(128, 128);
        IPVM::ImageProcessing::Resize_Linear(CropImage_Dark, ImgCrop_dark);

        vecCropImg_Bright.push_back(ImgCrop_bright);
        vecCropImg_Dark.push_back(ImgCrop_dark);
        
    }
    CString strResult = _T("");
   
    
    //if (SaveNG  && g_systemParameter.GetImageSaveOption() != 2
    //    && g_systemParameter.GetImageSaveOption() != 1)
    //{
    //    // m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageFull, 0, inspectionCell.m_cellID);

    //    m_cellImageSaver->PushCell(unit->m_inspectionImageBright, 0, unit->m_cellID);
    //    m_cellImageSaver->PushCell(unit->m_inspectionImageDark, 1, unit->m_cellID);
    //}
    //if (g_systemParameter.GetImageSaveOption() == 2 || g_systemParameter.GetImageSaveOption() == 1)
    //{
    //    //m_cellImageSaver->PushCell(inspectionCell.m_inspectionImageBright, 0, inspectionCell.m_cellID);

    //    if (SaveNG )
    //    {
    //        m_cellImageSaver->PushCell(unit->m_inspectionImageFull, 0, unit->m_cellID,true);
    //        m_cellImageSaver->PushCell(unit->m_inspectionImageBright, 0, unit->m_cellID);
    //        m_cellImageSaver->PushCell(unit->m_inspectionImageDark, 1, unit->m_cellID);
    //    }

    //    long nTempResize = g_systemParameter.m_nImageResize;
    //    unit->m_inspectionImageBrightResize.Free();
    //    unit->m_inspectionImageDarkResize.Free();
    //    unit->m_inspectionImageBrightResize.Create(unit->m_inspectionImageBright.GetSizeX() / nTempResize,
    //        unit->m_inspectionImageBright.GetSizeY() / nTempResize);
    //    unit->m_inspectionImageDarkResize.Create(unit->m_inspectionImageDark.GetSizeX() / nTempResize, unit->m_inspectionImageDark.GetSizeY() / nTempResize);

    //    IPVM::ImageProcessing::Resize_Linear(unit->m_inspectionImageBright, unit->m_inspectionImageBrightResize);
    //    IPVM::ImageProcessing::Resize_Linear(unit->m_inspectionImageDark, unit->m_inspectionImageDarkResize);
    //    m_cellImageSaver->PushCell(unit->m_inspectionImageBrightResize, 0, unit->m_cellID);
    //    m_cellImageSaver->PushCell(unit->m_inspectionImageDarkResize, 1, unit->m_cellID);
    //}

    for (long n=0; n<100; n++)
    {
        if(bResult[n]==TRUE)
        {
            strResult += _T("_");
            strResult += GetDefectName((DefectType)n);
        }
    }
    if (!strRows.IsEmpty() && !strDirectPath.IsEmpty())
    {
        CString strCsvFile;
        if (m_pViewCellImage->m_bSurfaceInsp)
        {
            strCsvFile.Format(_T("%s.csv"), strDirectPath);
        }
        else
        {
            strCsvFile.Format(_T("%s.csv12"), strDirectPath);
        }
        SaveCsvSurface(unit, strRows, strCsvFile);
    }
    //UpdateUi();

    auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);

    std::vector<IPVM::Image_8u_C1> vecEdgeImage;
    std::vector<IPVM::Rect_32f> vecEdgeROI;
    std::vector<IPVM::LineSeg_32f> vecLineFind;
    //align->image
    for (long nEdge = 0; nEdge < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size(); nEdge++)
    {
        IPVM::Rect rtResult;
        rtResult = itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge];

        IPVM::Rect rtCropROI;
        rtCropROI.m_left = 0;
        rtCropROI.m_top = 0;
        rtCropROI.m_bottom = rtResult.Height();
        rtCropROI.m_right = rtResult.Width();

        IPVM::Image_8u_C1 CropImage;
        CropImage.Create(rtResult.Width(), rtResult.Height()); // 좀 더 크게 보여 주기 위해 어떻게 진행이 되어야 할까? off set 값을 어떻게 계산하여 넣어줄까?

        IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageBright, rtResult, rtCropROI, CropImage);
        vecEdgeImage.push_back(CropImage);

        vecEdgeROI.push_back(itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge]);
        IPVM::LineSeg_32f lineFind;
        lineFind.m_sx = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].TopLeft().m_x - itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge].m_left;
        lineFind.m_sy = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].TopLeft().m_y;
        lineFind.m_ex = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].BottomLeft().m_x - itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge].m_left;
        lineFind.m_ey = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].BottomLeft().m_y;
        vecLineFind.push_back(lineFind);
        //align->m_paraEdgeDetectMeasure.m_vecrtDetect
    }

    if (m_dlgEdgeView)
    {
        if (g_systemParameter.bEdgeViewSampling)
            m_nSamplingCnt++;
        UpdateEdgeView((CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign));
        
    }


    //===========================================================================================
    // [ Mismatch Result Display ]
    //DisplayMismatchData();
    Share::ResultVision ResultData;
    //SendHostResultDataInfo(ResultData, unit);
    SendHostResultDataInfo(ResultData, vecCropImg_Bright, vecCropImg_Dark, unit);
    ResultMismatchData(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), unit->m_cellID, ResultData);
    //DisplayMismatchData(nSizeDivideVertical, vecMeasureDefect, _T("test1234"), unit->m_cellID);
    //m_dlgDefectmap->m_pDlgDefectTimerView->SetROI(vecDefectROI);
    if (m_dlgDefectmap->m_pDlgDefectTimerView)
    {
        UpdateDefectROIView(vecDefectROI);
    }
    if (g_systemParameter.bMSASave)
    {
        SaveCsvMeasureMSA(unit);
    }
    if (g_systemParameter.bMeasureSave)
    {   
        CString strCsvFile;
        //if (g_systemParameter.bDirection == 0)
        //{
        //    strCsvFile.Format(_T("%s_measure_top.csv"), strDirectPath);
        //}
        //else
        //{
        //    strCsvFile.Format(_T("%s_measure_back.csv"), strDirectPath);
        //}
        strCsvFile.Format(_T("%s_measure.csv"), strCsvFolderPath);
        SaveCsvMeasure(unit, nSizeDivideVertical, vecMeasureDefect, strCsvFile);
    }
    return;
    //	Redraw();
}

CString CIBCIDlg::MakeCsvRowString(long index, CISIViewerProperty propertyView, CInspectionCell* unit)
{
    CString strReceive;
    strReceive.AppendFormat(_T("%d,"), index);
    strReceive.AppendFormat(_T("%d,"), propertyView.nAVG_GV);
    strReceive.AppendFormat(_T("%d,"), propertyView.nMIN_GV);
    strReceive.AppendFormat(_T("%d,"), propertyView.nMAX_GV);
    strReceive.AppendFormat(_T("%d,"), propertyView.nBRIGHT_AVG_GV);
    strReceive.AppendFormat(_T("%d,"), propertyView.nDARK_AVG_GV);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fSIZE_X_mm);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fSIZE_Y_mm);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fAREA_Ratio);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fRATIO_YX);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fRATIO_XY);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fCOMPACT);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fRATIO_WHITE);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fRATIO_DARK);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fEDGE_ENERGY);
    strReceive.AppendFormat(_T("%.3f,"), propertyView.fMASS_CENTER_GV_GV);
    strReceive.AppendFormat(_T("%s,"), propertyView.strLotID);
    strReceive.AppendFormat(_T("%d,"), unit->m_cellID);
    strReceive.AppendFormat(_T("%s_,"), propertyView.strTime);
    strReceive.AppendFormat(_T("%d,"), propertyView.m_nCandidateDefectCode);
    strReceive.AppendFormat(_T("%d,"), propertyView.nUp);
    strReceive.AppendFormat(_T("%d,"), propertyView.nDefectX);
    strReceive.AppendFormat(_T("%d,"), propertyView.nDefectY);
    strReceive.AppendFormat(_T("%d,"), propertyView.m_nJudge);
    strReceive.AppendFormat(_T("%s,"), propertyView.pathDirectBright);
    strReceive.AppendFormat(_T("%s,"), propertyView.pathDirectDark);
    strReceive.AppendFormat(_T("%s,"), propertyView.pathCropBright);
    strReceive.AppendFormat(_T("%s,"), propertyView.pathCropDark);
    strReceive.AppendFormat(_T("%s,"), propertyView.pathCropOversizeBright);
    strReceive.AppendFormat(_T("%s,"), propertyView.pathCropOversizeDark);

    return strReceive;
}

SPathCropImages CIBCIDlg::SaveCropImages(CInspectionCell* unit, long index, IPVM::Image_8u_C1& OriginImage_Bright, IPVM::Image_8u_C1& OriginImage_Dark,
    IPVM::Image_8u_C1& CropImage_Bright, IPVM::Image_8u_C1& CropImage_Dark, CString strDirectPath)
{
    int nCropLeft = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_x - (m_nCropPixel_X / 2);
    int nCropTop = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_y - (m_nCropPixel_Y / 2);
    int nCropRight = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_x + (m_nCropPixel_X / 2);
    int nCropBottom = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_y + (m_nCropPixel_Y / 2);

    IPVM::Rect CropRect_orig(nCropLeft, nCropTop, nCropRight, nCropBottom);

    int nCropLeft_Offset = 0;
    int nCropTop_Offset = 0;
    int nCropRight_Offset = 0;
    int nCropBottom_Offset = 0;

    if (nCropLeft < 0)
    {
        nCropLeft_Offset = -nCropLeft;
        nCropLeft = 0;
    }

    if (nCropTop < 0)
    {
        nCropTop_Offset = -nCropTop;
        nCropTop = 0;
    }

    if (nCropRight > IPVM::Rect(OriginImage_Bright).Width())
    {
        nCropRight_Offset = nCropRight - IPVM::Rect(OriginImage_Bright).m_right;
        nCropRight = IPVM::Rect(OriginImage_Bright).m_right;
    }

    if (nCropBottom > IPVM::Rect(OriginImage_Bright).Height())
    {
        nCropBottom_Offset = nCropBottom - IPVM::Rect(OriginImage_Bright).m_bottom;
        nCropBottom = IPVM::Rect(OriginImage_Bright).m_bottom;
    }

    IPVM::Rect CropRect(nCropLeft, nCropTop, nCropRight, nCropBottom);

    IPVM::Image_8u_C1 ImageCrop_bright(CropRect_orig.Width(), CropRect_orig.Height());
    ImageCrop_bright.FillZero();
    IPVM::Rect CropRect_over_Orig_Diff(IPVM::Rect(ImageCrop_bright).m_left + nCropLeft_Offset, IPVM::Rect(ImageCrop_bright).m_top + nCropTop_Offset,
        IPVM::Rect(ImageCrop_bright).m_right - nCropRight_Offset, IPVM::Rect(ImageCrop_bright).m_bottom - nCropBottom_Offset);
    IPVM::ImageProcessing::Copy(OriginImage_Bright, CropRect, CropRect_over_Orig_Diff, ImageCrop_bright);

    IPVM::Image_8u_C1 ImageCrop_dark(CropRect_orig.Width(), CropRect_orig.Height());
    ImageCrop_dark.FillZero();
    IPVM::ImageProcessing::Copy(OriginImage_Dark, CropRect, CropRect_over_Orig_Diff, ImageCrop_dark);

    IPVM::Image_8u_C1 ImgCrop_bright(128, 128);
    IPVM::ImageProcessing::Resize_Linear(ImageCrop_bright, ImgCrop_bright);

    IPVM::Image_8u_C1 ImgCrop_dark(128, 128);
    IPVM::ImageProcessing::Resize_Linear(ImageCrop_dark, ImgCrop_dark);

    IPVM::Image_8u_C1 ImgCrop_over_bright(128, 128);
    IPVM::Image_8u_C1 ImgCrop_over_dark(128, 128);

    bool bCropOversize = false;
    if (unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.Width() > m_nCropPixel_X)
    {
        int nRtWidth = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.Width();
        int nRtHeight = m_nCropPixel_Y * nRtWidth / m_nCropPixel_X;

        int nCropLeft_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_x - (nRtWidth / 2);
        int nCropTop_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_y - (nRtHeight / 2);
        int nCropRight_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_x + (nRtWidth / 2);
        int nCropBottom_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_y + (nRtHeight / 2);

        IPVM::Rect CropRect_over_Orig(nCropLeft_over, nCropTop_over, nCropRight_over, nCropBottom_over);
        int nCropLeft_over_Offset = 0;
        int nCropTop_over_Offset = 0;
        int nCropRight_over_Offset = 0;
        int nCropBottom_over_Offset = 0;

        if (nCropLeft_over < 0)
        {
            nCropLeft_over_Offset = -nCropLeft_over;
            nCropLeft_over = 0;
        }

        if (nCropTop_over < 0)
        {
            nCropTop_over_Offset = -nCropTop_over;
            nCropTop_over = 0;
        }

        if (nCropRight_over > nRtWidth)
        {
            nCropRight_over_Offset = nCropRight_over - nRtWidth;
            nCropRight_over = nRtWidth;
        }

        if (nCropBottom_over > nRtHeight)
        {
            nCropBottom_over_Offset = nCropBottom_over - nRtHeight;
            nCropBottom_over = nRtHeight;
        }

        IPVM::Rect CropRect_over(nCropLeft_over, nCropTop_over, nCropRight_over, nCropBottom_over);
        IPVM::Image_8u_C1 Image_over_bright(CropRect_over_Orig.Width(), CropRect_over_Orig.Height());
        Image_over_bright.FillZero();
        IPVM::Rect CropRect_over_Orig_Diff(IPVM::Rect(Image_over_bright).m_left + nCropLeft_over_Offset, IPVM::Rect(Image_over_bright).m_top + nCropTop_over_Offset,
            IPVM::Rect(Image_over_bright).m_right - nCropRight_over_Offset, IPVM::Rect(Image_over_bright).m_bottom - nCropBottom_over_Offset);
        IPVM::ImageProcessing::Copy(OriginImage_Bright, CropRect_over, CropRect_over_Orig_Diff, Image_over_bright);

        IPVM::Image_8u_C1 Image_over_dark(CropRect_over_Orig.Width(), CropRect_over_Orig.Height());
        Image_over_dark.FillZero();
        IPVM::ImageProcessing::Copy(OriginImage_Dark, CropRect_over, CropRect_over_Orig_Diff, Image_over_dark);

        /*IPVM::ImageProcessing::Resize_Linear(Image_over_bright, ImgCrop_over_bright);
        IPVM::ImageProcessing::Resize_Linear(Image_over_dark, ImgCrop_over_dark);*/

        bCropOversize = true;
    }

    if (unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.Height() > m_nCropPixel_Y)
    {
        int nRtHeight = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.Height();
        int nRtWidth = m_nCropPixel_X * nRtHeight / m_nCropPixel_Y;

        int nCropLeft_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_x - (nRtWidth / 2);
        int nCropTop_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_y - (nRtHeight / 2);
        int nCropRight_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_x + (nRtWidth / 2);
        int nCropBottom_over = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel.CenterPoint().m_y + (nRtHeight / 2);

        IPVM::Rect CropRect_over_Orig(nCropLeft_over, nCropTop_over, nCropRight_over, nCropBottom_over);
        int nCropLeft_over_Offset = 0;
        int nCropTop_over_Offset = 0;
        int nCropRight_over_Offset = 0;
        int nCropBottom_over_Offset = 0;

        if (nCropLeft_over < 0)
        {
            nCropLeft_over_Offset = -nCropLeft_over;
            nCropLeft_over = 0;
        }

        if (nCropTop_over < 0)
        {
            nCropTop_over_Offset = -nCropTop_over;
            nCropTop_over = 0;
        }

        if (nCropRight_over > IPVM::Rect(OriginImage_Bright).Width())
        {
            nCropRight_over_Offset = nCropRight_over - IPVM::Rect(OriginImage_Bright).Width();
            nCropRight_over = IPVM::Rect(OriginImage_Bright).Width();
        }

        if (nCropBottom_over > IPVM::Rect(OriginImage_Bright).Height())
        {
            nCropBottom_over_Offset = nCropBottom_over - IPVM::Rect(OriginImage_Bright).Height();
            nCropBottom_over = IPVM::Rect(OriginImage_Bright).Height();
        }

        IPVM::Rect CropRect_over(nCropLeft_over, nCropTop_over, nCropRight_over, nCropBottom_over);
        IPVM::Image_8u_C1 Image_over_bright(CropRect_over_Orig.Width(), CropRect_over_Orig.Height());
        Image_over_bright.FillZero();
        IPVM::Rect CropRect_over_Orig_Diff(IPVM::Rect(Image_over_bright).m_left + nCropLeft_over_Offset, IPVM::Rect(Image_over_bright).m_top + nCropTop_over_Offset,
            IPVM::Rect(Image_over_bright).m_right - nCropRight_over_Offset, IPVM::Rect(Image_over_bright).m_bottom - nCropBottom_over_Offset);
        IPVM::ImageProcessing::Copy(OriginImage_Bright, CropRect_over, CropRect_over_Orig_Diff, Image_over_bright);

        IPVM::Image_8u_C1 Image_over_dark(CropRect_over_Orig.Width(), CropRect_over_Orig.Height());
        Image_over_dark.FillZero();
        IPVM::ImageProcessing::Copy(OriginImage_Dark, CropRect_over, CropRect_over_Orig_Diff, Image_over_dark);

        /*IPVM::ImageProcessing::Resize_Linear(Image_over_bright, ImgCrop_over_bright);
        IPVM::ImageProcessing::Resize_Linear(Image_over_dark, ImgCrop_over_dark);*/

        bCropOversize = true;
    }

    // save Crop Image
    SPathCropImages sPaths;

    CString pathIndex;
    CString strName;
    strName = GetDefectName(DefectType(unit->m_ISICellAllDefect.m_vecISIDefectType[index]));
    pathIndex.Format(_T("%s\\%s_%04d_%s"), strDirectPath, m_strInspTime, index, strName);
    //if (g_systemParameter.bCropImageNameType)
    //{
    //    CString strName;
    //    strName = GetDefectName(DefectType(unit->m_ISICellAllDefect.m_vecISIDefectType[index]));
    //    pathIndex.Format(_T("%s\\%s_%04d_%s"), strDirectPath, m_strInspTime, index, strName);
    //}
    //else
    //{
    //    pathIndex.Format(_T("%s\\%04d"), strDirectPath, index);
    //}

    CString ext = _T(".jpg");
    if (g_systemParameter.m_imageSaveType == ImageSaveType::Jpg)
        ext.Format(_T(".jpg"));
    else if (g_systemParameter.m_imageSaveType == ImageSaveType::Bmp)
        ext.Format(_T(".bmp"));

    sPaths.pathDirectBright.Format(_T("%s%s"), pathIndex, _T("_Bright.jpg"));
    sPaths.pathDirectDark.Format(_T("%s%s"), pathIndex, _T("_Dark.jpg"));
    sPaths.pathCropBright.Format(_T("%s_Bright_Crop%s"), pathIndex, ext);
    sPaths.pathCropDark.Format(_T("%s_Dark_Crop%s"), pathIndex, ext);
    sPaths.pathCropOversizeBright.Format(_T("%s_Bright_A%s"), pathIndex, ext);
    sPaths.pathCropOversizeDark.Format(_T("%s_Dark_A%s"), pathIndex, ext);

    if (!strDirectPath.IsEmpty())
    {
        ImgCrop_bright.SaveJpeg(sPaths.pathDirectBright);
        ImgCrop_dark.SaveJpeg(sPaths.pathDirectDark);
        /*CropImage_Bright.SaveJpeg(sPaths.pathDirectBright);
        CropImage_Dark.SaveJpeg(sPaths.pathDirectDark);

        ImgCrop_bright.SaveJpeg(sPaths.pathCropBright);
        ImgCrop_dark.SaveJpeg(sPaths.pathCropDark);

        if (bCropOversize)
        {
            ImgCrop_over_bright.SaveJpeg(sPaths.pathCropOversizeBright);
            ImgCrop_over_dark.SaveJpeg(sPaths.pathCropOversizeDark);
        }*/
    }

    sPaths.pathDirectBright.Replace(g_systemParameter.pathResultDataCsv, _T(""));
    sPaths.pathDirectDark.Replace(g_systemParameter.pathResultDataCsv, _T(""));
    sPaths.pathCropBright.Replace(g_systemParameter.pathResultDataCsv, _T(""));
    sPaths.pathCropDark.Replace(g_systemParameter.pathResultDataCsv, _T(""));
    sPaths.pathCropOversizeBright.Replace(g_systemParameter.pathResultDataCsv, _T(""));
    sPaths.pathCropOversizeDark.Replace(g_systemParameter.pathResultDataCsv, _T(""));
    return sPaths;
}

void CIBCIDlg::UpdateUiData(vector<CISIViewerProperty> vecReceiveData)
{
    // copy해서 각 thread에 뿌리기
    // SetEvent(m_threadControl_InlineInspectionResultUpdate.m_Signal_Start);
    {
        CSingleLock lock(&m_inlineResultUiUpdater_View->m_csUiUpdater_View, TRUE);
        m_inlineResultUiUpdater_View->m_vecReceiveData_View.insert(m_inlineResultUiUpdater_View->m_vecReceiveData_View.end(), vecReceiveData.begin(), vecReceiveData.end());
    }
    for (int nIdx = 0; nIdx < vecReceiveData.size(); nIdx++)
    {
        vecReceiveData[nIdx].brightImage.Free();
        vecReceiveData[nIdx].darkImage.Free();
    }
    {
        CSingleLock lock(&m_inlineResultUiUpdater_Map->m_csUiUpdater_Map, TRUE);
        m_inlineResultUiUpdater_Map->m_vecReceiveData_Map.insert(m_inlineResultUiUpdater_Map->m_vecReceiveData_Map.end(), vecReceiveData.begin(), vecReceiveData.end());
    }
    {
        CSingleLock lock(&m_inlineResultUiUpdater_Count->m_csUiUpdater_Count, TRUE);
        m_inlineResultUiUpdater_Count->m_vecReceiveData_Count.insert(m_inlineResultUiUpdater_Count->m_vecReceiveData_Count.end(), vecReceiveData.begin(), vecReceiveData.end());
    }

    m_inlineResultUiUpdater_View->Trigger();
    m_inlineResultUiUpdater_Map->Trigger();
    m_inlineResultUiUpdater_Count->Trigger();
    //std::vector<CISIViewerProperty> vecReceiveData = vecReceiveData;
}

void CIBCIDlg::UpdateDefectView(CISIViewerProperty& propertyView)
{
    vector<CString> vecInfos;
    CString info;
    info.Format(_T("PC ID : %d"), g_systemParameter.nPC_ID);
    vecInfos.push_back(info);
    info.Format(_T("LOT ID : %s"), propertyView.strLotID);
    vecInfos.push_back(info);
    info.Format(_T("EQUIP : %s"), g_systemParameter.szMachineNumber);
    vecInfos.push_back(info);
    info.Format(_T("TIME : %s"), propertyView.strTime);
    vecInfos.push_back(info);
    info.Format(_T("DEFECT : %s"), GetDefectName((DefectType)propertyView.m_nCandidateDefectCode));
    vecInfos.push_back(info); // DETECT NAME
    m_dlgDefectView->UpdateUI(&propertyView.brightImage, &propertyView.darkImage, vecInfos);
    //m_dlgDefectView->UpdateUI(&propertyView.brightImage, &propertyView.darkImage);

    m_dlgDefectView->m_indexUpdate += 2;
    if (m_dlgDefectView->m_indexUpdate >= m_dlgDefectView->m_nVecSize)
        m_dlgDefectView->m_indexUpdate = 0;
}

void CIBCIDlg::UpdateEdgeView(CInspectionCellAlign* itemCellAlign)
{
    std::vector<IPVM::Image_8u_C1> vecEdgeImage;
    std::vector<IPVM::Rect_32f> vecEdgeROI;
    std::vector<IPVM::LineSeg_32f> vecLineFind;
    // off
    if (!g_systemParameter.bEdgeViewSampling)
    {
        itemCellAlign->m_para.m_nSelectImageType;
        for (long nEdge = 0; nEdge < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size(); nEdge++)
        {
            IPVM::Rect rtResult;
            rtResult = itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge];

            IPVM::Rect rtCropROI;
            rtCropROI.m_left = 0;
            rtCropROI.m_top = 0;
            rtCropROI.m_bottom = rtResult.Height();
            rtCropROI.m_right = rtResult.Width();

            IPVM::Image_8u_C1 CropImage;
            CropImage.Create(rtResult.Width(),rtResult.Height()); // 좀 더 크게 보여 주기 위해 어떻게 진행이 되어야 할까? off set 값을 어떻게 계산하여 넣어줄까?

            if (g_systemParameter.nEdgeViewImageType == 0) //Default
            {
                long nType = itemCellAlign->m_para.m_nSelectImageType.GetValue();            
                if (nType == 0)
                {
                    IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageBright, rtResult, rtCropROI, CropImage);
                }
                if (nType == 1)
                {
                    IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageDark, rtResult, rtCropROI, CropImage);
                }
            }

            else if (g_systemParameter.nEdgeViewImageType == 1) // Bright
            {
                IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageBright, rtResult, rtCropROI, CropImage);
            }

            else if (g_systemParameter.nEdgeViewImageType == 2) // Dark
            {
                IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageDark, rtResult, rtCropROI, CropImage);
            }
            vecEdgeImage.push_back(CropImage);

            vecEdgeROI.push_back(itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge]);
            IPVM::LineSeg_32f lineFind;
            lineFind.m_sx = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].TopLeft().m_x
                - itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge].m_left;
            lineFind.m_sy = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].TopLeft().m_y;
            lineFind.m_ex = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].BottomLeft().m_x
                - itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge].m_left;
            lineFind.m_ey = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].BottomLeft().m_y;
            vecLineFind.push_back(lineFind);
            //align->m_paraEdgeDetectMeasure.m_vecrtDetect
        }
        vector<CString> vecInfos;
        m_dlgEdgeView->UpdateUI(vecEdgeImage, vecLineFind, vecInfos);
    }
    //on
    else if (g_systemParameter.bEdgeViewSampling&&m_nSamplingCnt==g_systemParameter.nSamplingCnt)
    {
        for (long nEdge = 0; nEdge < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size(); nEdge++)
        {
            IPVM::Rect rtResult;
            rtResult = itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge];

            IPVM::Rect rtCropROI;
            rtCropROI.m_left = 0;
            rtCropROI.m_top = 0;
            rtCropROI.m_bottom = rtResult.Height();
            rtCropROI.m_right = rtResult.Width();

            IPVM::Image_8u_C1 CropImage;
            CropImage.Create(rtResult.Width(),rtResult.Height()); // 좀 더 크게 보여 주기 위해 어떻게 진행이 되어야 할까? off set 값을 어떻게 계산하여 넣어줄까?

            if (g_systemParameter.nEdgeViewImageType == 0) //Default
            {
                long nType = itemCellAlign->m_para.m_nSelectImageType.GetValue();
                if (nType == 0)
                {
                    IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageBright, rtResult, rtCropROI, CropImage);
                }
                if (nType == 1)
                {
                    IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageDark, rtResult, rtCropROI, CropImage);
                }
            }

            else if (g_systemParameter.nEdgeViewImageType == 1) // Bright
            {
                IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageBright, rtResult, rtCropROI, CropImage);
            }

            else if (g_systemParameter.nEdgeViewImageType == 2) // Dark
            {
                IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageDark, rtResult, rtCropROI, CropImage);
            }
            vecEdgeImage.push_back(CropImage);

            vecEdgeROI.push_back(itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge]);
            IPVM::LineSeg_32f lineFind;
            lineFind.m_sx = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].TopLeft().m_x
                - itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge].m_left;
            lineFind.m_sy = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].TopLeft().m_y;
            lineFind.m_ex = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].BottomLeft().m_x
                - itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge].m_left;
            lineFind.m_ey = itemCellAlign->m_unionResult.m_rectAreaMeasure[nEdge].BottomLeft().m_y;
            vecLineFind.push_back(lineFind);
            //align->m_paraEdgeDetectMeasure.m_vecrtDetect
        }
        vector<CString> vecInfos;
        m_nSamplingCnt = 0;
        m_dlgEdgeView->UpdateUI(vecEdgeImage, vecLineFind, vecInfos);
    }
}

void CIBCIDlg::UpdateDefectROIView(std::vector<IPVM::Rect> vecROI)
{
    if (m_dlgDefectmap->m_pDlgDefectTimerView != nullptr)
    {
        m_dlgDefectmap->m_pDlgDefectTimerView->SetROI(vecROI);
    }
    
}

CISIViewerProperty CIBCIDlg::MakeISIViewerProperty(CInspectionCell* unit, long index, IPVM::Image_8u_C1& CropImage_Bright, IPVM::Image_8u_C1& CropImage_Dark, IPVM::Rect rtDraw, SPathCropImages pathsCrop)
{
    // temp
    CISIViewerProperty propertyView;
    propertyView.strLotID = unit->m_ISICellAllDefect.strLotID;
    propertyView.nCellIndex = unit->m_cellID;
    propertyView.nInspectImage = unit->m_ISICellAllDefect.m_vecCandidate[index].nInspectImage; // 밝은 or 어두운 이미지
    propertyView.nDefectBrightness = unit->m_ISICellAllDefect.m_vecCandidate[index].nDefectBrightness; // white or dark 불량
    propertyView.nInspectionArea = unit->m_ISICellAllDefect.m_vecCandidate[index].nInspectionArea; // 검사 영역
    propertyView.nCategory = unit->m_ISICellAllDefect.m_vecCandidate[index].nCategory; //
    propertyView.nBLOB_THRESHOLD = unit->m_ISICellAllDefect.m_vecCandidate[index].BLOB_THRESHOLD; //128전처리 이미지 위/아래 마진으로 찾은 후보들
    propertyView.nSECTION_THRESHOLD = unit->m_ISICellAllDefect.m_vecCandidate[index].SECTION_THRESHOLD; //화이트성 불량 : 최대값 - 평균값, 블랙성 불량은 : 평균값 - 최솟값
    propertyView.nAVG_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].AVG_GV;
    propertyView.nMIN_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].MIN_GV;
    propertyView.nMAX_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].MAX_GV;
    propertyView.nBRIGHT_AVG_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].BRIGHT_AVG_GV;
    propertyView.nDARK_AVG_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].DARK_AVG_GV;
    propertyView.fSIZE_X_mm = unit->m_ISICellAllDefect.m_vecCandidate[index].SIZE_X_mm;
    propertyView.fSIZE_Y_mm = unit->m_ISICellAllDefect.m_vecCandidate[index].SIZE_Y_mm;
    propertyView.fAREA_Ratio = unit->m_ISICellAllDefect.m_vecCandidate[index].AREA_Ratio;
    propertyView.fWIDTH_mm = unit->m_ISICellAllDefect.m_vecCandidate[index].WIDTH_mm;
    propertyView.fLENGTH_mm = unit->m_ISICellAllDefect.m_vecCandidate[index].LENGTH_mm;
    propertyView.fRATIO_WL = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_WL;
    propertyView.fRATIO_LW = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_LW;
    propertyView.fRATIO_YX = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_YX;
    propertyView.fRATIO_XY = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_XY;
    propertyView.fANGLE = unit->m_ISICellAllDefect.m_vecCandidate[index].ANGLE;
    propertyView.fSTD = unit->m_ISICellAllDefect.m_vecCandidate[index].STD;
    propertyView.fCOMPACT = unit->m_ISICellAllDefect.m_vecCandidate[index].COMPACT;
    propertyView.fAREA = unit->m_ISICellAllDefect.m_vecCandidate[index].AREA;
    propertyView.fRATIO_WHITE = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_WHITE;
    propertyView.fRATIO_DARK = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_DARK;
    propertyView.fEDGE_ENERGY = unit->m_ISICellAllDefect.m_vecCandidate[index].EDGE_ENERGY;
    //propertyView.fRATIO_AREA = unit->m_ISICellAllDefect.m_vecCandidate[index].AREA;
    propertyView.fMASS_CENTER_GV_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].MASS_CENTER_GV_GV;
    propertyView.fHOR = unit->m_ISICellAllDefect.m_vecCandidate[index].HOR;
    propertyView.fVER = unit->m_ISICellAllDefect.m_vecCandidate[index].VER;
    propertyView.fRATIO_HV = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_HV;
    propertyView.fRATIO_VH = unit->m_ISICellAllDefect.m_vecCandidate[index].RATIO_VH;

    propertyView.rtRect_pixel = unit->m_ISICellAllDefect.m_vecCandidate[index].rtRect_pixel; //불량 위치
    propertyView.srcImage = unit->m_ISICellAllDefect.m_vecCandidate[index].srcImage;
    propertyView.brightImage = CropImage_Bright;
    propertyView.darkImage = CropImage_Dark;
    propertyView.fEDGE_ENERGY_SHARPNESS = unit->m_ISICellAllDefect.m_vecCandidate[index].EDGE_ENERGY_SHARPNESS;
    propertyView.fEDGE_ENERGY_SHARPNESS_WHITE = unit->m_ISICellAllDefect.m_vecCandidate[index].EDGE_ENERGY_SHARPNESS_WHITE;
    propertyView.fEDGE_ENERGY_SHARPNESS_BLACK = unit->m_ISICellAllDefect.m_vecCandidate[index].EDGE_ENERGY_SHARPNESS_BLACK;

    //propertyView.MEDIAN_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].MEDIAN_GV;
    //propertyView.STDEV_GV = unit->m_ISICellAllDefect.m_vecCandidate[index].STDEV_GV;

    propertyView.m_nCandidateDefectCode = unit->m_ISICellAllDefect.m_vecISIDefectType[index];
    propertyView.m_nJudge = unit->m_ISICellAllDefect.m_vecISIResultCode[index];
    if ((II_RESULT_VALUE)unit->m_resultValue != II_RESULT_VALUE::INVALID)
    {
        propertyView.m_nJudge = unit->m_resultValue;
    }
    propertyView.strTime = unit->m_ISICellAllDefect.m_strTime;
    propertyView.nUp = !g_systemParameter.bDirection;
    propertyView.nDefectX = rtDraw.CenterPoint().m_x;
    propertyView.nDefectY = rtDraw.CenterPoint().m_y;

    propertyView.pathDirectBright = pathsCrop.pathDirectBright;
    propertyView.pathDirectDark = pathsCrop.pathDirectDark;
    propertyView.pathCropBright = pathsCrop.pathCropBright;
    propertyView.pathCropDark = pathsCrop.pathCropDark;
    propertyView.pathCropOversizeBright = pathsCrop.pathCropOversizeBright;
    propertyView.pathCropOversizeDark = pathsCrop.pathCropOversizeDark;
    // 판정이 2 이면 증가
    return propertyView;
}

void CIBCIDlg::SaveCsvSurface(CInspectionCell* unit, CString strReceive, CString pathFile)
{
    // CSV 저장 관련 테스트 코드 추가
    // Title 추가

    // 이미지 크롭한거 저장
    // 아래 이미지 저장주소 적어야 함
    /*CString LotID(_T(""));
				LotID.Format(_T("%d"), 1234);
				temp.strLotID = LotID;*/

    //cropImageSave(strPath, temp.strLotID, unit->m_cellID, CropImage_Bright, CropImage_Dark);
    CString lotId = unit->m_ISICellAllDefect.strLotID;
    long nCellId = unit->m_cellID;
    //
    //if (lotId == _T(""))
    //{
    //    lotId = _T("0000");
    //}
    //
    //CString strDirectPath;
    //strDirectPath.Format(_T("d:\\%s\\%010d"), lotId, nCellId);
    //IPVM::CreateDirectories(strDirectPath);
    //
    ////CString pathIndex;
    ////pathIndex.Format(_T("%s\\%04d"), strDirectPath, index);
    //
    //CString strDefectCSVPath;
    //CString strCellID;
    //strCellID.Format(_T("%d"), unit->m_cellID);
    //strDefectCSVPath = strDirectPath + _T("\\") + lotId + _T("_") + strCellID + _T(".csv");

    CString strDefectTitle = _T("STATUS,"
                                "nAVG_GV,"
                                "nMIN_GV,"
                                "nMAX_GV,"
                                "nBRIGHT_AVG_GV,"
                                "nDARK_AVG_GV,"
                                "fSIZE_X_mm,"
                                "fSIZE_Y_mm,"
                                "fAREA_Ratio,"
                                "fRATIO_YX,"
                                "fRATIO_XY,"
                                "fCOMPACT,"
                                "fRATIO_WHITE,"
                                "fRATIO_DARK,"
                                "fEDGE_ENERGY,"
                                "fMASS_CENTER_GV_GV,"
                                "LotID,"
                                "CellID,"
                                "Time,"
                                "DefectType,"
                                "nTopBack,"
                                "nDefectX,"
                                "nDefectY,"
                                "nJudge,"
                                "pathDirectBright,"
                                "pathDirectDark,"
                                "pathCropBright,"
                                "pathCropDark,"
                                "pathCropOversizeBright,"
                                "pathCropOversizeDark,"
    );

    GetCSVSaveThread()->Push(pathFile, strDefectTitle, strReceive, true);
}


bool compare(DefectCount a, DefectCount b)
{
    return a.m_nSumNum > b.m_nSumNum;
}

void CIBCIDlg::UpdateDefectCount(CISIViewerProperty& propertyView)
{
    IPVM::TimeCheck tm;

    BOOL bFirst = TRUE;
    for (long nId = 0; nId < m_dlgDefectCount->m_vecData.size(); nId++)
    {
        if (m_dlgDefectCount->m_vecData[nId].m_strName == GetDefectName((DefectType)propertyView.m_nCandidateDefectCode))
        {
            bFirst = FALSE;
            if (propertyView.nUp == 1)
            {
                m_dlgDefectCount->m_vecData[nId].m_nTopNum++;
                m_dlgDefectCount->m_vecData[nId].m_nSumNum++;

                /*CString strTemp = m_dlgDefectCount->m_vecData[0].m_strName;
                long nTopTemp = m_dlgDefectCount->m_vecData[0].m_nTopNum;
                long nSumTemp = m_dlgDefectCount->m_vecData[0].m_nSumNum;

                m_dlgDefectCount->m_vecData[0].m_strName = m_dlgDefectCount->m_vecData[nId].m_strName;
                m_dlgDefectCount->m_vecData[0].m_nTopNum = m_dlgDefectCount->m_vecData[nId].m_nTopNum;
                m_dlgDefectCount->m_vecData[0].m_nSumNum = m_dlgDefectCount->m_vecData[nId].m_nSumNum;

                m_dlgDefectCount->m_vecData[nId].m_strName = strTemp;
                m_dlgDefectCount->m_vecData[nId].m_nTopNum = nTopTemp;
                m_dlgDefectCount->m_vecData[nId].m_nSumNum = nSumTemp;*/

                std::sort(m_dlgDefectCount->m_vecData.begin(), m_dlgDefectCount->m_vecData.end(), compare);
            }
            else
            {
                m_dlgDefectCount->m_vecData[nId].m_nBottomNum++;
                m_dlgDefectCount->m_vecData[nId].m_nSumNum++;

                /*CString strTemp = m_dlgDefectCount->m_vecData[0].m_strName;
                long nTopTemp = m_dlgDefectCount->m_vecData[0].m_nBottomNum;
                long nSumTemp = m_dlgDefectCount->m_vecData[0].m_nSumNum;

                m_dlgDefectCount->m_vecData[0].m_strName = m_dlgDefectCount->m_vecData[nId].m_strName;
                m_dlgDefectCount->m_vecData[0].m_nBottomNum = m_dlgDefectCount->m_vecData[nId].m_nBottomNum;
                m_dlgDefectCount->m_vecData[0].m_nSumNum = m_dlgDefectCount->m_vecData[nId].m_nSumNum;

                m_dlgDefectCount->m_vecData[nId].m_strName = strTemp;
                m_dlgDefectCount->m_vecData[nId].m_nBottomNum = nTopTemp;
                m_dlgDefectCount->m_vecData[nId].m_nSumNum = nSumTemp;*/

                std::sort(m_dlgDefectCount->m_vecData.begin(), m_dlgDefectCount->m_vecData.end(), compare);
            }
        }
    }
    /*if (bFirst)
    {
        CString strDefectName = GetDefectName((DefectType)propertyView.m_nCandidateDefectCode);
        if (propertyView.nUp == 1)
        {
            DefectCount data(strDefectName, 1, 0);
            m_dlgDefectCount->m_vecData.push_back(data);
        }
        else
        {
            DefectCount data(strDefectName, 0, 1);
            m_dlgDefectCount->m_vecData.push_back(data);
        }
    }*/

    m_dlgDefectCount->DefectDraw();
    ExportDefectCountData(m_dlgDefectCount->m_vecData);

}

void CIBCIDlg::UpdateAccDefectMap(IPVM::Rect rtDraw)
{
    DefectPointRect temp;
    temp.rt = rtDraw;
    temp.nX = rtDraw.CenterPoint().m_x;
    temp.nY = rtDraw.CenterPoint().m_y;
    temp.Color = RGB(255, 0, 0);
    m_dlgAcc->SetPoint(0, temp);
}

void CIBCIDlg::UpdateDefectMap(CISIViewerProperty& propertyView)
{
    DefectPointXY ptDefect;
    ptDefect.nX = propertyView.nDefectX;
    ptDefect.nY = propertyView.nDefectY;
    ExportDefectMapData(propertyView.nUp, propertyView.nCellIndex, ptDefect, propertyView.m_nJudge,
        propertyView.m_nCandidateDefectCode, propertyView.pathDirectBright, propertyView.pathDirectDark); //[sm]
    bool bPass = propertyView.m_nJudge == 0;
    if (bPass)
    {
        ptDefect.Color = RGB(0, 255, 0);
        m_dlgDefectmap->SetPoint(0, ptDefect, propertyView.nCellIndex, m_cellImageMerger->GetHorizontalSize());
        ///m_dlgDefectmap->m_vecISIAllDefectTotal.push_back(propertyView);
    }
    else
    {
        ptDefect.Color = RGB(255, 0, 0);
        //long nHorSize = m_cellImageMerger->GetHorizontalSize();
        long nHorSize = 0;
        if (m_cellImageMerger != nullptr)
        {
            nHorSize = m_cellImageMerger->GetHorizontalSize();
        }
        else
        {
            CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);
            auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
            nHorSize = itemCellAlign->m_inspectionImageBright.GetSizeX();
        }
        if (nHorSize == 0)
            //nHorSize = 14288 * 2;
            nHorSize = m_cisComm->GetGrabExcutor(0)->GetHorizontalSize() * 2;
        if (propertyView.nCellIndex >= 25000)
        {
            m_dlgDefectmap->DataCountSetZero();

            CString filePath;
            //filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\DefectMapData.csv");
            filePath.Format(_T("%s\\DefectMapData.csv"), Path::GetBackupUIFolder());
            CFileFind find;
            if (find.FindFile(filePath))
            {
                DeleteFile(filePath);
            }
        }
        m_dlgDefectmap->SetPoint(propertyView.nUp, ptDefect, propertyView.nCellIndex, nHorSize);
        //m_dlgDefectmap->m_vecISIAllDefectTotal.push_back(propertyView);
    }
}

void CIBCIDlg::UpdateDefectMap(CISIViewerProperty& propertyView, IPVM::Rect rtDraw, IPVM::Image_8u_C1& imageBright)
{
    DefectPointXY ptDefect;
    ptDefect.nX = rtDraw.CenterPoint().m_x;
    ptDefect.nY = rtDraw.CenterPoint().m_y;
    bool bPass = propertyView.m_nJudge == 0;
    if (bPass)
    {
        ptDefect.Color = RGB(0, 255, 0);
        m_dlgDefectmap->SetPoint(0, ptDefect, propertyView.nCellIndex, imageBright.GetSizeX());
        //m_dlgDefectmap->m_vecISIAllDefectTotal.push_back(propertyView);
    }
    else
    {
        ptDefect.Color = RGB(255, 0, 0);
        m_dlgDefectmap->SetPoint(propertyView.nUp, ptDefect, propertyView.nCellIndex, imageBright.GetSizeX());
        //m_dlgDefectmap->m_vecISIAllDefectTotal.push_back(propertyView);
    }
}

void CIBCIDlg::SaveCsvMismatch(ResultMismatch resultMismatch, CString pathFile)
{
    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionMeasure* CellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);

    CString strTitle;
    CString strText;
    CString strResult;

    strTitle = _T("Num,");
    strText = _T(",");

    auto& resMismatch = resultMismatch;

    for (long n = 0; n < resMismatch.m_vecResUpLane.size(); n++)
    {
        CString strTemp, strData;
        if (resMismatch.m_vecResUpLane.size() == 3)
        {
#if INSPECTION_MACHINE_UC1
            if (n == 0)
            {
                strTemp = GetDefectName(DefectType::Width_LeftFoil);
            }
            else if (n == 1)
            {
                strTemp = GetDefectName(DefectType::Width_LeftCoating);
            }
            else if (n == 2)
            {
                strTemp = GetDefectName(DefectType::Width_RightFoil);
            }
#else
            if (n == 0)
            {
                strTemp = GetDefectName(DefectType::Width_NonCoating_1);
            }
            else if (n == 1)
            {
                strTemp = GetDefectName(DefectType::Width_Coating_1);
            }
            else if (n == 2)
            {
                strTemp = GetDefectName(DefectType::Width_NonCoating_2);
            }
#endif // INSPECTION_MACHINE_UC1
            
            strTemp += _T(",");
            strTitle += strTemp;

            strData.Format(_T("%.2f,"), resMismatch.m_vecfDiffWidth[n]);
            strText += strData;
        }
        else if (resMismatch.m_vecResUpLane.size() == 4)
        {
#if INSPECTION_MACHINE_UC1
            if (n == 0)
            {
                strTemp = GetDefectName(DefectType::Width_LeftFoil);
            }
            else if (n == 1)
            {
                strTemp = GetDefectName(DefectType::Width_LeftCoating);
            }
            else if (n == 2)
            {
                strTemp = GetDefectName(DefectType::Width_RightCoating);
            }
            else if (n == 3)
            {
                strTemp = GetDefectName(DefectType::Width_RightFoil);
            }
#else
            if (n == 0)
            {
                strTemp = GetDefectName(DefectType::Width_NonCoating_1);
            }
            else if (n == 1)
            {
                strTemp = GetDefectName(DefectType::Width_Coating_1);
            }
            else if (n == 2)
            {
                strTemp = GetDefectName(DefectType::Width_Coating_2);
            }
            else if (n == 3)
            {
                strTemp = GetDefectName(DefectType::Width_NonCoating_2);
            }
#endif // INSPECTION_MACHINE_UC1

            
            strTemp += _T(",");
            strTitle += strTemp;

            strData.Format(_T("%.2f,"), resMismatch.m_vecfDiffWidth[n]);
            strText += strData;
        }
    }
    GetCSVSaveThread()->Push(pathFile, strTitle, strText, true);
}

void CIBCIDlg::SaveCsvMeasure(CInspectionCell* unit, long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect,CString pathFile)
{
    CInspectionMeasure* CellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);
    //long nSizeMeasureItem = CellMeasure->m_para.m_nCountMeasureItem;

    CString strTitle;
    CString strText;
    CString strResult;

    strTitle = _T("Num,");

    for (long nVer = 0; nVer < nSizeDivideVertical; nVer++)
    {
        auto& vecMeasureDefect = m_vvMeasureDefect[nVer];
        long nSizeMeasureItem = (long)vecMeasureDefect.size();
        strText.Format(_T("%d,"), nVer);

        for (long nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            CString strTemp, strData;

            strTemp.Format(_T("%s,"), GetDefectName(vecMeasureDefect[nItemIdx].m_defectType));
            strTitle += strTemp;

            strData.Format(_T("%.2f,"), vecMeasureDefect[nItemIdx].LENGTH.GetValue());
            strText += strData;
        }
        GetCSVSaveThread()->Push(pathFile, strTitle, strText, true);
    }

    



    /*for (int nVerticalIdx = 0; nVerticalIdx < nSizeDivideVertical; nVerticalIdx++)
    {
        auto& vecMeasureDefect = m_vvMeasureDefect[nVerticalIdx];

        long nSizeMeasureItem = (long)vecMeasureDefect.size();
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftFoil)
            {
                fLeftFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftCoating)
            {
                fLeftCoating = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightCoating)
            {
                fRightCoating = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightFoil)
            {
                fRightFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_MiddleFoil)
            {
                fMiddleFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
    }*/

    /*strResult.Format(_T("CellID = %d,LF = %.4f,LC = %.4f,MF = %.4f,RC = %.4f,RF = %.4f,FL = %.4f"), nCellID, fLeftFoil,
        fLeftCoating, fMiddleFoil, fRightCoating, fRightFoil, fFullLength);
    m_loggerCellResult.Log(0, strResult);*/

}

void CIBCIDlg::SaveCsvMeasureMSA(CInspectionCell* unit)
{
    SYSTEMTIME tmModify;
    ::GetLocalTime(&tmModify);
    CString strTime;
    strTime.Format(_T("%04d%02d%02d"), tmModify.wYear, tmModify.wMonth, tmModify.wDay);

    CString strDirectPath;
    strDirectPath.Format(_T("d:\\MSA_Save\\%s\\%s\\%d"), strTime, unit->m_ISICellAllDefect.strLotID, unit->m_cellID);
    IPVM::CreateDirectories(strDirectPath);

    CString strMsaTitle;
    CString strMsaText;
    CString strMsaResult;
    CString strCellID;
    strCellID.Format(_T("_%d"), unit->m_cellID);

    CString strMsaPath = strDirectPath + _T("\\MSA_Data_") + strCellID + (".csv");

    CInspectionMeasure* CellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);

    strMsaTitle += _T("Num,");
    for (int i = 0; i < CellMeasure->m_para.m_nCountMeasureItem; i++)
    {
        strMsaTitle += CellMeasure->m_vecMSAType[i] + _T(",");
    }
    for (long nCnt = 0; nCnt < CellMeasure->m_para.m_fCellLengthCalcPosition; nCnt++)
    {
        strMsaResult = _T("");
        strMsaText.Format(_T("%d,"), nCnt);
        strMsaResult += strMsaText;
        for (long n = 0; n < CellMeasure->m_para.m_nCountMeasureItem; n++)
        {
            strMsaText.Format(_T("%f,"), CellMeasure->m_vecMSALength[nCnt][n]);
            strMsaResult += strMsaText;
        }
        /*strMsaText.Format(_T("%f,"), CellMeasure->m_vecMSALength[nCnt][0]);
            strMsaResult += strMsaText;
            strMsaText.Format(_T("%f,"), CellMeasure->m_vecMSALength[nCnt][1]);
            strMsaResult += strMsaText;
            strMsaText.Format(_T("%f,"), CellMeasure->m_vecMSALength[nCnt][2]);
            strMsaResult += strMsaText;
            strMsaText.Format(_T("%f,"), CellMeasure->m_vecMSALength[nCnt][3]);
            strMsaResult += strMsaText;*/
        GetCSVSaveThread()->Push(strMsaPath, strMsaTitle, strMsaResult, true);
    }
}


void CIBCIDlg::OnBnClickedButtonTestInspection()
{
    BOOL bShift = (::GetKeyState(VK_SHIFT) < 0);
    if (bShift)
    {
        //csv open 기능 테스트 코드 추가...
        CString filePath(_T("d:\\testsave.csv"));
        CSVReader csv;
        auto textlines = csv.LoadFileTextLines(filePath);
        if (textlines == nullptr)
            return;

        csv.Open(*textlines);

        /*strTitle += _T("STATUS,");
		strTitle += _T("fSIZE_X_mm,");
		strTitle += _T("fSIZE_Y_mm,");
		strTitle += _T("fAREA_Ratio,");
		strTitle += _T("rtRect_pixel.m_left,");
		strTitle += _T("rtRect_pixel.m_top,");
		strTitle += _T("rtRect_pixel.m_right,");
		strTitle += _T("rtRect_pixel.m_bottom,");


		long index_satus = csv.FindColumnIndex(_T("STATUS"));
		CString STATUS = csv.GetItemText(itemIndex, index_satus);
	*/
        CISIViewerProperty csvTemp;
        for (long itemIndex = 0; itemIndex < csv.GetItemCount(); itemIndex++)
        {
            CString data0(_T(""));
            CString data1(_T(""));
            CString data2(_T(""));
            CString data3(_T(""));
            CString data4(_T(""));
            CString data5(_T(""));
            CString data6(_T(""));
            CString data7(_T(""));
            CString data8(_T(""));
            CString data9(_T(""));
            CString data10(_T(""));
            CString data11(_T(""));
            CString data12(_T(""));
            CString data13(_T(""));
            CString data14(_T(""));
            CString data15(_T(""));
            CString data16(_T(""));
            CString data17(_T(""));
            CString data18(_T(""));
            CString data19(_T(""));
            CString data20(_T(""));
            CString data21(_T(""));
            CString data22(_T(""));
            CString data23(_T(""));
            CString data24(_T(""));
            CString data25(_T(""));
            CString data26(_T(""));
            CString data27(_T(""));
            CString data28(_T(""));
            CString data29(_T(""));
            CString data30(_T(""));
            CString data31(_T(""));
            CString data32(_T(""));
            CString data33(_T(""));
            CString data34(_T(""));
            CString data35(_T(""));
            CString data36(_T(""));
            CString data37(_T(""));
            CString data38(_T(""));
            CString data39(_T(""));
            CString data40(_T(""));
            CString data41(_T(""));
            CString data42(_T(""));
            CString data43(_T(""));
            CString data44(_T(""));

            data0 = csv.GetItemText(itemIndex, 0);
            data1 = csv.GetItemText(itemIndex, 1);
            data2 = csv.GetItemText(itemIndex, 2);
            data3 = csv.GetItemText(itemIndex, 3);
            data4 = csv.GetItemText(itemIndex, 4);

            data5 = csv.GetItemText(itemIndex, 5);
            data6 = csv.GetItemText(itemIndex, 6);
            data7 = csv.GetItemText(itemIndex, 7);
            data8 = csv.GetItemText(itemIndex, 8);
            data9 = csv.GetItemText(itemIndex, 9);

            data10 = csv.GetItemText(itemIndex, 10);
            data11 = csv.GetItemText(itemIndex, 11);
            data12 = csv.GetItemText(itemIndex, 12);
            data13 = csv.GetItemText(itemIndex, 13);
            data14 = csv.GetItemText(itemIndex, 14);

            data15 = csv.GetItemText(itemIndex, 15);
            data16 = csv.GetItemText(itemIndex, 16);
            data17 = csv.GetItemText(itemIndex, 17);
            data18 = csv.GetItemText(itemIndex, 18);
            data19 = csv.GetItemText(itemIndex, 19);

            data20 = csv.GetItemText(itemIndex, 20);
            data21 = csv.GetItemText(itemIndex, 21);
            data22 = csv.GetItemText(itemIndex, 22);
            data23 = csv.GetItemText(itemIndex, 23);
            data24 = csv.GetItemText(itemIndex, 24);

            data25 = csv.GetItemText(itemIndex, 25);
            data26 = csv.GetItemText(itemIndex, 26);
            data27 = csv.GetItemText(itemIndex, 27);
            data28 = csv.GetItemText(itemIndex, 28);
            data29 = csv.GetItemText(itemIndex, 29);

            data30 = csv.GetItemText(itemIndex, 30);
            data31 = csv.GetItemText(itemIndex, 31);
            data32 = csv.GetItemText(itemIndex, 32);
            data33 = csv.GetItemText(itemIndex, 33);
            data34 = csv.GetItemText(itemIndex, 34);

            data35 = csv.GetItemText(itemIndex, 35);
            data36 = csv.GetItemText(itemIndex, 36);
            data37 = csv.GetItemText(itemIndex, 37);
            data38 = csv.GetItemText(itemIndex, 38);
            data39 = csv.GetItemText(itemIndex, 39);

            data40 = csv.GetItemText(itemIndex, 40);
            data41 = csv.GetItemText(itemIndex, 41);
            data42 = csv.GetItemText(itemIndex, 42);
            data43 = csv.GetItemText(itemIndex, 43);
            data44 = csv.GetItemText(itemIndex, 44);

            csvTemp.nInspectImage = _ttoi(data1);
            csvTemp.nDefectBrightness = _ttoi(data2);
            csvTemp.nInspectionArea = _ttoi(data3);
            csvTemp.nCategory = _ttoi(data4);
            csvTemp.nBLOB_THRESHOLD = _ttoi(data5);

            csvTemp.nSECTION_THRESHOLD = _ttoi(data6);
            csvTemp.nAVG_GV = _ttoi(data7);
            csvTemp.nMIN_GV = _ttoi(data8);
            csvTemp.nMAX_GV = _ttoi(data9);
            csvTemp.nBRIGHT_AVG_GV = _ttoi(data10);

            csvTemp.nDARK_AVG_GV = _ttoi(data11);
            csvTemp.fSIZE_X_mm = _ttof(data12);
            csvTemp.fSIZE_Y_mm = _ttof(data13);
            csvTemp.fAREA_Ratio = _ttof(data14);
            csvTemp.fWIDTH_mm = _ttof(data15);

            csvTemp.fLENGTH_mm = _ttof(data16);
            csvTemp.fRATIO_WL = _ttof(data17);
            csvTemp.fRATIO_LW = _ttof(data18);
            csvTemp.fRATIO_YX = _ttof(data19);
            csvTemp.fRATIO_XY = _ttof(data20);

            csvTemp.fANGLE = _ttof(data21);
            csvTemp.fSTD = _ttof(data22);
            csvTemp.fCOMPACT = _ttof(data23);
            csvTemp.fAREA = _ttof(data24);
            csvTemp.fRATIO_WHITE = _ttof(data25);

            csvTemp.fRATIO_DARK = _ttof(data26);
            csvTemp.fEDGE_ENERGY = _ttof(data27);
            csvTemp.fAREA_Ratio = _ttof(data28);
            csvTemp.fMASS_CENTER_GV_GV = _ttof(data29);
            csvTemp.fHOR = _ttof(data30);

            csvTemp.fVER = _ttof(data31);
            csvTemp.fRATIO_HV = _ttof(data32);
            csvTemp.fRATIO_VH = _ttof(data33);
            csvTemp.m_nCandidateDefectCode = _ttoi(data34);
            csvTemp.m_nJudge = _ttoi(data35);

            csvTemp.brightImage.LoadFrom(data36);
            csvTemp.darkImage.LoadFrom(data37);
            csvTemp.rtRect_pixel.m_left = _ttoi(data38);
            csvTemp.rtRect_pixel.m_top = _ttoi(data39);
            csvTemp.rtRect_pixel.m_right = _ttoi(data40);

            csvTemp.rtRect_pixel.m_bottom = _ttoi(data41);
            csvTemp.strLotID = data42;
            csvTemp.nCellIndex = _ttoi(data43);
            csvTemp.strTime = data44;

            m_vecCsvData.push_back(csvTemp);
        }
        return;
    }

    const TCHAR szFilters[] = _T("Image files (*.bmp, *.png, *.jpg) | *.bmp;*.png;*.jpg|");

    CFileDialog dlg(TRUE, _T(""), _T(""),
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);

    CString strFileList;
    const int nMaxFiles = 400;
    const int nBuffSize = (nMaxFiles * (MAX_PATH + 1)) + 1;
    dlg.GetOFN().lpstrFile = strFileList.GetBuffer(nBuffSize);
    dlg.GetOFN().nMaxFile = nBuffSize;

    if (dlg.DoModal() != IDOK)
        return;

    POSITION pos;
    pos = dlg.GetStartPosition();
    CString strFullFilename;

    std::vector<CString> multiInlinePath;

    while (pos)
    {
        strFullFilename = dlg.GetNextPathName(pos);
        multiInlinePath.push_back(strFullFilename);
    }

    int nImageWidth = 0, nImageHeight = 0;

    if (multiInlinePath.size() == 1)
    {
        //Inspection(multiInlinePath[0], 1000, true);
        //BOOL isUp = true;
        //CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell(isUp);

        //IPVM::Image_8u_C1 imageFull;
        //imageFull.LoadFrom(multiInlinePath[0]);

        //IPVM::Image_8u_C1 imageBright, imageDark;
        //UtilityImage::CreateBrightDarkImage(imageFull, imageBright, imageDark, false);
        //unit->m_cellID = 1;
        //unit->m_inspectionImageFull = imageFull;
        //unit->SetInspectionSplitImage(imageBright, imageDark);
        //
        //
        //
        ///*if (unit->m_isUp)
        //{
        //	unit->SetPixelToUmX(g_systemParameter.m_dPixelToUmXUp);
        //}
        //else
        //{
        //	unit->SetPixelToUmX(g_systemParameter.m_dPixelToUmXDown);
        //}*/
        ///*unit->m_nAnodeCathode = g_systemParameter.nAnodeCathode;
        //unit->m_nInsulateType = g_systemParameter.nInsulateType;*/

        ////IPVM::TimeCheck tc;
        ////unit->SetPixelToUmY(g_systemParameter.m_dPixelToUmY * 2);
        //CInspectionCell* unitCopy = Inspection::Accessor::GetRoot().GetInspectionCell(isUp);
        //unit->CopyDataBase(unitCopy);
        //unit->m_bInlineTest = false;
        //unit->DoInspection();
        //
        ////===========================================================================================
        //// [ Surface Result Display ]
        ////
        //long nSize = unit->m_ISICellAllDefect.m_vecCandidate.size();
        ////unit->m_ISICellAllDefect.SetResultData();
        //std::vector<IPVM::Rect> vecrtDraw;
        //for (long n = 0; n < nSize; n++)
        //{
        //	IPVM::Rect rtDraw = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel;

        //	IPVM::Rect rect = rtDraw;

        //	int OriginRect_Width = rect.Width();
        //	int OriginRect_Height = rect.Height();
        //	int Offset_X = 0;
        //	int Offset_Y = 0;
        //	if (100 > max(rect.Width(), rect.Height()))
        //	{
        //		Offset_X = 50;
        //		Offset_Y = 50;
        //	}
        //	else if (rect.Width() > rect.Height())
        //	{
        //		int Diff = rect.Width() - rect.Height();
        //		int Diff_half = Diff / 2;
        //		Offset_Y = Diff_half;
        //	}
        //	else
        //	{
        //		int Diff = rect.Height() - rect.Width();
        //		int Diff_half = Diff / 2;
        //		Offset_X = Diff_half;
        //	}
        //	rect.InflateRect(Offset_X, Offset_Y, Offset_X, Offset_Y);
        //	rect &= IPVM::Rect(imageBright);

        //	int offset_left = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_left - rect.m_left;
        //	int offset_top = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_top - rect.m_top;
        //	int offset_right = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_right - rect.m_right;
        //	int offset_bottom = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_bottom - rect.m_bottom;
        //
        //	IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
        //	IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());
        //
        //	IPVM::Rect OriginRect(CropImage_Bright);
        //	OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);

        //	IPVM::Image_8u_C1* OriginImage_Bright;
        //	OriginImage_Bright = &imageBright;
        //	IPVM::ImageProcessing::Copy(*OriginImage_Bright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);

        //	IPVM::Image_8u_C1* OriginImage_Dark;
        //	OriginImage_Dark = &imageDark;
        //	IPVM::ImageProcessing::Copy(*OriginImage_Dark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);

        //	vecrtDraw.push_back(rtDraw);
        //	if (m_dlgDefectView)
        //	{
        //		CISIViewerProperty temp;
        //		temp.nInspectImage = unit->m_ISICellAllDefect.m_vecCandidate[n].nInspectImage;				// 밝은 or 어두운 이미지
        //		temp.nDefectBrightness = unit->m_ISICellAllDefect.m_vecCandidate[n].nDefectBrightness;			// white or dark 불량
        //		temp.nInspectionArea = unit->m_ISICellAllDefect.m_vecCandidate[n].nInspectionArea;				// 검사 영역
        //		temp.nCategory = unit->m_ISICellAllDefect.m_vecCandidate[n].nCategory;					//
        //		temp.BLOB_THRESHOLD = unit->m_ISICellAllDefect.m_vecCandidate[n].BLOB_THRESHOLD;				//128전처리 이미지 위/아래 마진으로 찾은 후보들
        //		temp.SECTION_THRESHOLD = unit->m_ISICellAllDefect.m_vecCandidate[n].SECTION_THRESHOLD;			//화이트성 불량 : 최대값 - 평균값, 블랙성 불량은 : 평균값 - 최솟값
        //		temp.AVG_GV = unit->m_ISICellAllDefect.m_vecCandidate[n].AVG_GV;
        //		temp.MIN_GV = unit->m_ISICellAllDefect.m_vecCandidate[n].MIN_GV;
        //		temp.MAX_GV = unit->m_ISICellAllDefect.m_vecCandidate[n].MAX_GV;
        //		temp.BRIGHT_AVG_GV = unit->m_ISICellAllDefect.m_vecCandidate[n].BRIGHT_AVG_GV;
        //		temp.DARK_AVG_GV = unit->m_ISICellAllDefect.m_vecCandidate[n].DARK_AVG_GV;
        //		temp.SIZE_X_mm = unit->m_ISICellAllDefect.m_vecCandidate[n].SIZE_X_mm;
        //		temp.SIZE_Y_mm = unit->m_ISICellAllDefect.m_vecCandidate[n].SIZE_Y_mm;
        //		temp.AREA_Ratio = unit->m_ISICellAllDefect.m_vecCandidate[n].AREA_Ratio;
        //		temp.WIDTH_mm = unit->m_ISICellAllDefect.m_vecCandidate[n].WIDTH_mm;
        //		temp.LENGTH_mm = unit->m_ISICellAllDefect.m_vecCandidate[n].LENGTH_mm;
        //		temp.RATIO_WL = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_WL;
        //		temp.RATIO_LW = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_LW;
        //		temp.RATIO_YX = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_YX;
        //		temp.RATIO_XY = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_XY;
        //		temp.ANGLE = unit->m_ISICellAllDefect.m_vecCandidate[n].ANGLE;
        //		temp.STD = unit->m_ISICellAllDefect.m_vecCandidate[n].STD;
        //		temp.COMPACT = unit->m_ISICellAllDefect.m_vecCandidate[n].COMPACT;
        //		temp.AREA = unit->m_ISICellAllDefect.m_vecCandidate[n].AREA;
        //		temp.RATIO_WHITE = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_WHITE;
        //		temp.RATIO_DARK = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_DARK;
        //		temp.EDGE_ENERGY = unit->m_ISICellAllDefect.m_vecCandidate[n].EDGE_ENERGY;
        //		temp.RATIO_AREA = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_AREA;
        //		temp.MASS_CENTER_GV_GV = unit->m_ISICellAllDefect.m_vecCandidate[n].MASS_CENTER_GV_GV;
        //		temp.HOR = unit->m_ISICellAllDefect.m_vecCandidate[n].HOR;
        //		temp.VER = unit->m_ISICellAllDefect.m_vecCandidate[n].VER;
        //		temp.RATIO_HV = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_HV;
        //		temp.RATIO_VH = unit->m_ISICellAllDefect.m_vecCandidate[n].RATIO_VH;

        //		temp.rtRect_pixel = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel;				//불량 위치
        //		temp.srcImage = unit->m_ISICellAllDefect.m_vecCandidate[n].srcImage;
        //		temp.brightImage = CropImage_Bright;
        //		temp.darkImage = CropImage_Dark;

        //		temp.m_nCandidateDefectCode = unit->m_ISICellAllDefect.m_vecISIDefectType[n];
        //		temp.m_nJudge = unit->m_ISICellAllDefect.m_vecISIResultCode[n];

        //		m_vecISIAllDefect.push_back(temp);
        //		if (m_vecISIAllDefect.size() > 9) // 생성할때 개수 나누기 2
        //		{
        //			m_vecISIAllDefect.erase(m_vecISIAllDefect.begin());
        //		}

        //	}

        //	if (m_dlgDefectmap)
        //	{
        //		DefectPointXY temp;
        //		temp.nX = rtDraw.CenterPoint().m_x;
        //		temp.nY = rtDraw.CenterPoint().m_y;
        //		temp.Color = RGB(255, 0, 0);
        //		m_dlgDefectmap->SetPoint(0, temp, unit->m_cellID, imageBright.GetSizeX());
        //	}
        //
        //	if (m_dlgAcc)
        //	{
        //		DefectPointRect temp;
        //		temp.rt = rtDraw;
        //		temp.nX = rtDraw.CenterPoint().m_x;
        //		temp.nY = rtDraw.CenterPoint().m_y;
        //		temp.Color = RGB(255, 0, 0);
        //		m_dlgAcc->SetPoint(0, temp);
        //	}
        //}
        //for (long n = 0; n < m_vecISIAllDefect.size(); n++)
        //{
        //	m_dlgDefectView->UpdateUI(n * 2, &m_vecISIAllDefect[n].brightImage, &m_vecISIAllDefect[n].darkImage);
        //}
        ////long n = 0;
        ////===========================================================================================
        //// [ Mismatch Result Display ]
        //DisplayMismatchData();
    }
    //Inspection::Accessor::GetRoot().GetInspectionCell
}

void CIBCIDlg::OnInlineStart(BOOL bLive)
{
    UdateDefectROIViewClear();
    //m_dlgDefectmap->m_pDlgDefectTimerView->ROIClear();
    //m_pViewCellImage->GetDlgItem(IDC_COMBO_MODE_INSPECTION)->EnableWindow(FALSE);
    //m_pViewCellImage->GetDlgItem(IDC_BUTTON_SURFACE_INSPECTION)->EnableWindow(FALSE);
    //m_pViewCellImage->GetDlgItem(IDC_BUTTON_SURFACE_IMAGE_LOAD)->EnableWindow(FALSE);

    //m_pSyncIO->TurnOffLight();
    //m_bLight = TRUE;
    //if (m_bLight==TRUE)
    //{
        Illum_On();
    //}   

    m_cellImageMerger->Start();

    if (m_cisComm->GetExecutorType() == ExecutorType::Euresys)
	{
		m_cisComm->GrabExcutorContainerStart(GrabMode::AREA);
	}
	else
	{
		m_cisComm->GrabExcutorContainerStart(GrabMode::Inline);
	}

    if (bLive)
    {
        m_pSyncIO->LiveStartSyncboard();
        m_pViewCellImage->EnableWindow(FALSE);
    }

    else
    {
        m_pSyncIO->StartSyncboard();
        m_bInline = TRUE;
        if (m_pViewCellImage->m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_ALWAYS
            || m_pViewCellImage->m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_NG_ONLY)
        {
            m_pViewCellImage->m_pButtonInlineInspMode->SetBackGradation(RGB(237, 26, 4), RGB(255, 163, 73), 1);
            m_pViewCellImage->EnableWindow(FALSE);
        }
        else
        {
            m_pViewCellImage->m_pButtonInlineInspMode->SetBackGradation(RGB(26, 237, 26), RGB(163, 255, 73), 1);
        }
        m_pViewCellImage->m_pButtonInlineInspMode->Invalidate(false);

    }
    

    m_cellImageProducers->ClearLot();
   // m_inlineResultCoupler->ClearLot();

    m_pStaticBtnInsp->LoadImageFromResource(_T("JPG"), IDR_JPG_BTN_START);
    //m_pStaticBtnInsp->LoadImageFromResource(_T("JPG"), IDR_JPG_BTN_STOP);
    m_pStaticBtnInsp->Invalidate(false);
    if (g_systemParameter.bDirection == 1)
    {
       long sensorID = m_pSyncIO->GetCell_ID();

       if (sensorID < 0)
       {
           sensorID = 0;
       }

       CString strSend;

       strSend.Format(_T("StartCellID,%d"), sensorID);
       
     
       if (g_systemParameter.bServer == 0) // Client 일 때,
       {
           OnSend2(strSend);

           if (::GetKeyState(VK_SHIFT) < 0)
           {
               Sleep(10);
               strSend.Format(_T("InlineStart"));
               OnSend2(strSend);
           }
       }
  
    }
   
}

void CIBCIDlg::OnInlineStop()
{
    m_cisComm->GrabExcutorContainerStop();
    m_cellImageMerger->Stop();
    m_inlineResultCoupler->ClearLot();
    m_cellImageProducers->ClearLot();
    m_cellImageDispathcer->ClearLot();
    m_cellImageSaver->ClearCellResultQueue();
    {
        CSingleLock lock(&m_csPropertyView, TRUE);
        m_vecReceiveData.clear();
    }
    {
        CSingleLock lock(&m_inlineResultUiUpdater_View->m_csUiUpdater_View, TRUE);
        m_inlineResultUiUpdater_View->m_vecReceiveData_View.clear();
    }
    {
        CSingleLock lock(&m_inlineResultUiUpdater_Map->m_csUiUpdater_Map, TRUE);
        m_inlineResultUiUpdater_Map->m_vecReceiveData_Map.clear();
    }
    {
        CSingleLock lock(&m_inlineResultUiUpdater_Count->m_csUiUpdater_Count, TRUE);
        m_inlineResultUiUpdater_Count->m_vecReceiveData_Count.clear();
    }
    //m_pSyncIO->TurnOffLight();
    m_bInline = FALSE;
    m_viewCellImageUpdater->SetInspMode(0, m_bInline, m_bBatchView);
    m_pViewCellImage->EnableWindow(TRUE);
    m_pViewCellImage->m_pButtonInlineInspMode->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), 1);
    m_pViewCellImage->m_pButtonInlineInspMode->Invalidate(false);
    //if (m_bLight == FALSE)
    //{
        Illum_Off();
    //}   

    m_pStaticBtnInsp->LoadImageFromResource(_T("JPG"), IDR_JPG_BTN_STOP);
    m_pStaticBtnInsp->Invalidate(false);
    m_nTestCellID = 0;
    if (g_systemParameter.bServer == 0) // Client 일 때,
    {
        if (::GetKeyState(VK_SHIFT) < 0)
        {
            Sleep(10);
            CString strSend;
            strSend.Format(_T("InlineStop"));
            OnSend2(strSend);
        }
    }
}

void CIBCIDlg::CamsetApply()
{
    //m_bIsCommanderHost = false;
    //m_inlineSync->CameraInitialize(true, false);
}

void CIBCIDlg::OnBnClickedButtonInspStart()
{
    OnInlineStart();
    m_nCellID_sync = 0;
    m_nCellID_calc = 0;
    m_nCellID_finalcalc = 0;
}

void CIBCIDlg::OnBnClickedButtonInspEnd()
{
    OnInlineStop();
}
void CIBCIDlg::OnBnClickedButtonData()
{
    ::AfxMessageBox(_T("구현 중"));
}
void CIBCIDlg::OnBnClickedButtonSystem()
{
    CSystemDlg dlg(g_systemParameter, this);
    if (dlg.DoModal() != IDOK)
        return;
    swap(g_systemParameter, dlg.m_sysParam);
    g_systemParameter.Save();

    ReDrawMismatchAnimation();
    m_dlgEdgeView->ModifyLaneCount(g_systemParameter.nLaneCount);
}
void CIBCIDlg::OnBnClickedButtonRecipe()
{
    if (m_dlgRecipeManager == nullptr)
    {
        m_dlgRecipeManager = new CDlgRecipeManager(this);
        m_dlgRecipeManager->Create(IDD_DIALOG_RECIPE_MANAGER, this);
        m_dlgRecipeManager->CenterWindow(CWnd::GetDesktopWindow());
    }
    m_dlgRecipeManager->ShowWindow(SW_SHOW);
}
void CIBCIDlg::OnBnClickedButtonSpec()
{
    //::AfxMessageBox(_T("구현 중"));    
    CDlgSpecMeasure dlg;
    if (dlg.DoModal() == IDOK)
    {
        
    }
}
void CIBCIDlg::OnBnClickedButtonLogin()
{
    //::AfxMessageBox(_T("구현 중"));
    if (m_pDlgLogin != nullptr)
    {
        AfxMessageBox(_T("Log-In Screen Is On!"));
        return;
    }
    m_pDlgLogin = new CDlgLogin(this);
    m_pDlgLogin->Create(IDD_DIALOG_LOGIN, this);
    m_pDlgLogin->ShowWindow(TRUE);


}
void CIBCIDlg::OnBnClickedButtonPgmClose()
{
    ThreadStop_InlineInspeciton();
    OnCancel();
}

void CIBCIDlg::OnBnClickedButtonViewmodeMain()
{
    m_nModeView = ENUM_MODE_VIEW::MAIN;
    /*m_pButtonViewmodeMain->SetBackGradation(RGB(110, 277, 247), RGB(128, 245, 255), RGB(146, 255, 255), 1);
	m_pButtonViewmodeSurface->SetBackGradation(RGB(224, 255, 139), RGB(242, 255, 157), RGB(255, 255, 175), 1);
	m_pButtonViewmodeTeaching->SetBackGradation(RGB(224, 255, 139), RGB(242, 255, 157), RGB(255, 255, 175), 1);*/

    m_pButtonViewmodeMain->SetBackGradation(RGB(49, 191, 252), RGB(179, 202, 254), 1);
    m_pButtonViewmodeSurface->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonViewmodeTeaching->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);

    m_pButtonViewmodeTeaching->SetFocus();
    m_pButtonViewmodeSurface->SetFocus();
    m_pButtonViewmodeMain->SetFocus();
    UpdateControlLayer();

    m_pPaneLayout->Load(_T("MainViewLayout"));
    m_paneManager.CloseAll();
    m_paneManager.SetLayout(m_pPaneLayout);
}

void CIBCIDlg::OnBnClickedButtonViewmodeSurface()
{
    m_nModeView = ENUM_MODE_VIEW::SURFACE;
    /*m_pButtonViewmodeMain->SetBackGradation(RGB(224, 255, 139), RGB(242, 255, 157), RGB(255, 255, 175), 1);
	m_pButtonViewmodeSurface->SetBackGradation(RGB(110, 277, 247), RGB(128, 245, 255), RGB(146, 255, 255), 1);
	m_pButtonViewmodeTeaching->SetBackGradation(RGB(224, 255, 139), RGB(242, 255, 157), RGB(255, 255, 175), 1);*/
    m_pButtonViewmodeMain->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonViewmodeSurface->SetBackGradation(RGB(49, 191, 252), RGB(179, 202, 254), 1);
    m_pButtonViewmodeTeaching->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonViewmodeMain->SetFocus();
    m_pButtonViewmodeTeaching->SetFocus();
    m_pButtonViewmodeSurface->SetFocus();
    UpdateControlLayer();

    m_pPaneLayout->Load(_T("SurfaceViewLayout"));
    m_paneManager.CloseAll();
    m_paneManager.SetLayout(m_pPaneLayout);
}

void CIBCIDlg::OnBnClickedButtonViewmodeTeaching()
{
    m_nModeView = ENUM_MODE_VIEW::TEACHING;
    /*m_pButtonViewmodeMain->SetBackGradation(RGB(224, 255, 139), RGB(242, 255, 157), RGB(255, 255, 175), 1);
	m_pButtonViewmodeSurface->SetBackGradation(RGB(224, 255, 139), RGB(242, 255, 157), RGB(255, 255, 175), 1);
	m_pButtonViewmodeTeaching->SetBackGradation(RGB(110, 277, 247), RGB(128, 245, 255), RGB(146, 255, 255), 1);*/
    m_pButtonViewmodeMain->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonViewmodeSurface->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonViewmodeTeaching->SetBackGradation(RGB(49, 191, 252), RGB(179, 202, 254), 1);
    m_pButtonViewmodeMain->SetFocus();
    m_pButtonViewmodeSurface->SetFocus();
    m_pButtonViewmodeTeaching->SetFocus();
    UpdateControlLayer();

    m_pPaneLayout->Load(_T("TeachingViewLayout"));
    m_paneManager.CloseAll();
    m_paneManager.SetLayout(m_pPaneLayout);
}

LRESULT CIBCIDlg::OnUWMUpdateUI(WPARAM wParam, LPARAM lParam)
{
    CISIViewerProperty* pPropertyView = (CISIViewerProperty*)wParam;
    
    UpdateDefectView(*pPropertyView);
    
    return 0;
}

void CIBCIDlg::SetShowWindowMaximize()
{
    long nStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
    nStyle &= ~WS_CAPTION;
    nStyle &= ~WS_SYSMENU;

    ::SetWindowLong(m_hWnd, GWL_STYLE, nStyle);
    int nScreenX = GetSystemMetrics(SM_CXSCREEN);
    int nScreenY = GetSystemMetrics(SM_CYSCREEN);

    SetWindowPos(NULL, -4, -4, nScreenX + 8, nScreenY + 4, SWP_NOZORDER);
}

CString CIBCIDlg::GetFileVersion()
{
    CString strVersion = _T("");
    HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hRsrc != NULL)
    {
        HGLOBAL hGlobalMemory = LoadResource(NULL, hRsrc);
        if (hGlobalMemory != NULL)
        {
            CString rVersion;
            LPVOID pVersionResouece = LockResource(hGlobalMemory);
            LPVOID pVersion = NULL;
            DWORD uLength, langD;
            BOOL retVal;
            retVal = VerQueryValue(
                pVersionResouece, _T("\\VarFileInfo\\Translation"), (LPVOID*)&pVersion, (UINT*)&uLength);
            if (retVal && uLength == 4)
            {
                memcpy(&langD, pVersion, 4);
                rVersion.Format(_T("\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion"), (langD & 0xff00) >> 8,
                    langD & 0xff, (langD & 0xff000000) >> 24, (langD & 0xff0000) >> 16);
            }
            else
            {
                rVersion.Format(_T("\\StringFileInfo\\%04X04B0\\FileVersion"), GetUserDefaultLangID());
            }

            if (VerQueryValue(pVersionResouece, rVersion.GetBuffer(0), (LPVOID*)&pVersion, (UINT*)&uLength) != 0)
            {
                //int cnt, len;
                //TCHAR ch;

                strVersion.Format(_T("%s"), pVersion);

                //	/* 버젼포맷 변경 : 1, 0, 1, 0 -> 1.010 */
                //	/*len = ver.GetLength();
                //	for (cnt = 0; cnt<len; cnt++)
                //	{
                //		ch = ver.GetAt(cnt);
                //		if (ch == ' ') continue;
                //		if (ch >= '0' && ch <= '9') {
                //			strVersion += ch;
                //			if (cnt == 0) strVersion += _T(".");
                //		}
                //	}*/
            }
        }
        FreeResource(hGlobalMemory);
    }
    return strVersion;
}

void CIBCIDlg::ConstructorCustomControl()
{
    m_pStaticLogo = new CCustomStatic;
    m_pStaticMainTitle = new CCustomStatic;
    m_pStaticCurTime = new CCustomStatic;
    m_pStaticQueueStatus = new CCustomStatic;

    m_pStaticNameMachine = new CCustomStatic;
    m_pStaticNameVision = new CCustomStatic;
    m_pStaticNameRecipe = new CCustomStatic;
    m_pStaticNameModel = new CCustomStatic;
    m_pStaticNamePosition = new CCustomStatic;

    m_pStaticStatusCamera = new CCustomStatic;
    m_pStaticStatusMaster = new CCustomStatic;
    m_pStaticStatusFriendVision = new CCustomStatic;
    m_pStaticStatusRes0 = new CCustomStatic;
    m_pStaticStatusRes1 = new CCustomStatic;

    m_pStaticBtnInsp = new CCustomStatic;

    m_pStaticStatusCaptionCamera = new CCustomStatic;
    m_pStaticStatusCaptionMaster = new CCustomStatic;
    m_pStaticStatusCaptionFriendVision = new CCustomStatic;
    m_pStaticStatusCaptionRes0 = new CCustomStatic;
    m_pStaticStatusCaptionRes1 = new CCustomStatic;

    m_pStaticMemoryStatus = new CCustomStatic;

    m_pButtonTestInsp = new CCustomButton;

    m_pButtonInspStart = new CCustomButton;
    m_pButtonInspEnd = new CCustomButton;
    m_pButtonInspDlg = new CCustomButton;
    m_pButtonBatchInsp = new CCustomButton;
    m_pButtonData = new CCustomButton;
    m_pButtonSystem = new CCustomButton;
    m_pButtonRecipe = new CCustomButton;
    m_pButtonCamSetting = new CCustomButton;
    m_pButtonSpec = new CCustomButton;
    m_pButtonLogin = new CCustomButton;
    m_pButtonPGMClose = new CCustomButton;
    m_pButtomExportData = new CCustomButton;

    m_pButtonViewmodeMain = new CCustomButton;
    m_pButtonViewmodeSurface = new CCustomButton;
    m_pButtonViewmodeTeaching = new CCustomButton;

    m_pButtonInitGraph = new CCustomButton;
    m_pButtonCalResol = new CCustomButton;
    m_pButtonDataSearch = new CCustomButton;

    m_pButtonHWControl = new CCustomButton;
    for (long cnt = 0; cnt < 3; cnt++)
    {
        m_pButtonReserve[cnt]  = new CCustomButton;
    }

    for (long cnt = 0; cnt < 4; cnt++)
    {
        m_pButtomCamera[cnt] = new CCustomButton;
    }
}

void CIBCIDlg::DestructorCustomControl()
{
    delete m_pStaticLogo;
    delete m_pStaticMainTitle;
    delete m_pStaticCurTime;
    delete m_pStaticQueueStatus;

    delete m_pStaticNameMachine;
    delete m_pStaticNameVision;
    delete m_pStaticNameRecipe;
    delete m_pStaticNameModel;
    delete m_pStaticNamePosition;

    delete m_pStaticStatusCamera;
    delete m_pStaticStatusMaster;
    delete m_pStaticStatusFriendVision;
    delete m_pStaticStatusRes0;
    delete m_pStaticStatusRes1;

    delete m_pStaticMemoryStatus;

    delete m_pStaticBtnInsp;

    delete m_pStaticStatusCaptionCamera;
    delete m_pStaticStatusCaptionMaster;
    delete m_pStaticStatusCaptionFriendVision;
    delete m_pStaticStatusCaptionRes0;
    delete m_pStaticStatusCaptionRes1;

    delete m_pButtonTestInsp;

    delete m_pButtonInspStart;
    delete m_pButtonInspEnd;
    delete m_pButtonInspDlg;
    delete m_pButtonBatchInsp;
    delete m_pButtonData;
    delete m_pButtonSystem;
    delete m_pButtonRecipe;
    delete m_pButtonCamSetting;
    delete m_pButtonSpec;
    delete m_pButtonLogin;
    delete m_pButtonPGMClose;
    delete m_pButtomExportData;

    delete m_pButtonViewmodeMain;
    delete m_pButtonViewmodeSurface;
    delete m_pButtonViewmodeTeaching;



    delete m_pButtonInitGraph;
    delete m_pButtonCalResol;
    delete m_pButtonDataSearch;
    delete m_pButtonHWControl;

    for (long cnt = 0; cnt < 2; cnt++)
    {
        delete m_pButtonReserve[cnt];
    }

    for (long cnt = 0; cnt < 4; cnt++)
    {
        delete m_pButtomCamera[cnt];
    }
}

void CIBCIDlg::InitializeCustomControl()
{
    m_pStaticLogo->SetWindowText(_T(""));
    m_pStaticLogo->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    if (g_systemParameter.machineType == MachineType::UC1_mismatch)
    {
        m_pStaticLogo->LoadImageFromResource(_T("JPG"), IDR_JPG_LOGO);
    }
    else
    {
        m_pStaticLogo->LoadImageFromResource(_T("JPG"), IDR_JPG_LOGO);
    }
    //m_pStaticLogo->Invalidate();

    CString strMainTitle;
    strMainTitle.Format(_T("S/W Version : %s"), (LPCTSTR)GetFileVersion());
    m_pStaticMainTitle->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticMainTitle->SetBackGradation(RGB(157, 195, 230), RGB(96, 158, 214), RGB(42, 105, 162), 1);
    m_pStaticMainTitle->SetFont(_T("휴먼둥근헤드라인"), RGB(255, 255, 255), 12, 1, 5);
    m_pStaticMainTitle->SetWindowText(strMainTitle);

    CString strCurruntTime;
    strCurruntTime.Format(_T("Currunt Time"));
    m_pStaticCurTime->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticCurTime->SetBackGradation(RGB(157, 195, 230), RGB(96, 158, 214), RGB(42, 105, 162), 1);
    m_pStaticCurTime->SetFont(_T("휴먼둥근헤드라인"), RGB(255, 255, 255), 12, 1, 5);
    m_pStaticCurTime->SetWindowText(strCurruntTime);

    CString strQueueStatus;
    strCurruntTime.Format(_T("Currunt Time"));
    m_pStaticQueueStatus->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticQueueStatus->SetBackGradation(RGB(157, 195, 230), RGB(96, 158, 214), RGB(42, 105, 162), 1);
    m_pStaticQueueStatus->SetFont(_T("휴먼둥근헤드라인"), RGB(255, 255, 255), 12, 1, 5);
    m_pStaticQueueStatus->SetWindowText(strCurruntTime);

    m_pStaticStatusCamera->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    m_pStaticStatusCamera->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    m_pStaticStatusCamera->SetWindowText(_T(""));
    CString strCaption;
    strCaption.Format(_T("[Camera]"));
    m_pStaticStatusCaptionCamera->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticStatusCaptionCamera->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 10, 1, 5);
    m_pStaticStatusCaptionCamera->SetWindowText(strCaption);

    m_pStaticStatusMaster->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    m_pStaticStatusMaster->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    m_pStaticStatusMaster->SetWindowText(_T(""));
    strCaption.Format(_T("[Master]"));
    m_pStaticStatusCaptionMaster->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticStatusCaptionMaster->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 10, 1, 5);
    m_pStaticStatusCaptionMaster->SetWindowText(strCaption);

    m_pStaticStatusFriendVision->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    m_pStaticStatusFriendVision->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    m_pStaticStatusFriendVision->SetWindowText(_T(""));
    strCaption.Format(_T("[Vision]"));
    m_pStaticStatusCaptionFriendVision->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticStatusCaptionFriendVision->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 10, 1, 5);
    m_pStaticStatusCaptionFriendVision->SetWindowText(strCaption);

    m_pStaticStatusRes0->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    m_pStaticStatusRes0->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    m_pStaticStatusRes0->SetWindowText(_T(""));
    strCaption.Format(_T("[Res #0]"));
    m_pStaticStatusCaptionRes0->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticStatusCaptionRes0->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 10, 1, 5);
    m_pStaticStatusCaptionRes0->SetWindowText(strCaption);

    m_pStaticStatusRes1->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    m_pStaticStatusRes1->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    m_pStaticStatusRes1->SetWindowText(_T(""));
    strCaption.Format(_T("[Res #1]"));
    m_pStaticStatusCaptionRes1->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticStatusCaptionRes1->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 10, 1, 5);
    m_pStaticStatusCaptionRes1->SetWindowText(strCaption);

    m_pStaticBtnInsp->ModifyStyle(0x0FL, SS_OWNERDRAW | SS_NOTIFY);
    //m_pStaticBtnInsp->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pStaticBtnInsp->SetWindowText(_T(""));
    m_pStaticBtnInsp->LoadImageFromResource(_T("JPG"), IDR_JPG_BTN_STOP);

    CString strNameMachine;
    strNameMachine.Format(_T("Machine #51"));
    m_pStaticNameMachine->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticNameMachine->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
    m_pStaticNameMachine->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
    m_pStaticNameMachine->SetWindowText(g_systemParameter.szMachineNumber);

    CString strNameVision;
    strNameVision.Format(_T("MISMATCH VISION"));
    m_pStaticNameVision->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticNameVision->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
    m_pStaticNameVision->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);

    switch (g_systemParameter.machineType)
    {
        case MachineType::ES_mismatch:
        case MachineType::UC1_mismatch:
            strNameVision = _T("MISMATCH");
            break;
        case MachineType::ES_notching_dual:
        case MachineType::ES_notching_single:
            strNameVision = _T("NOTCHING");
            break;
        case MachineType::ES_rollpress_pinhole:
            strNameVision = _T("PINHOLE");
            break;
        case MachineType::ES_rollpress_surface:
            strNameVision = _T("SURFACE");
            break;
    }

    m_pStaticNameVision->SetWindowText(strNameVision);

    CString strNameRecipe;
    strNameRecipe.Format(_T("15A"));
    m_pStaticNameRecipe->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticNameRecipe->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
    m_pStaticNameRecipe->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
    m_pStaticNameRecipe->SetWindowText(strNameRecipe);

    CString strNameModel;
    strNameModel.Format(_T("Anode 15A"));
    m_pStaticNameModel->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticNameModel->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
    m_pStaticNameModel->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
    m_pStaticNameModel->SetWindowText(strNameModel);

    CString strNamePosition;
    strNamePosition.Format(_T("%s"), (g_systemParameter.bDirection == 0 ? _T("TOP") : _T("BACK")));
    m_pStaticNamePosition->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticNamePosition->SetBackGradation(RGB(210, 210, 210), RGB(180, 180, 180), RGB(150, 150, 150), 1);
    m_pStaticNamePosition->SetFont(_T("휴먼둥근헤드라인"), RGB(0, 0, 0), 15, 1, 5);
    m_pStaticNamePosition->SetWindowText(strNamePosition);

    m_pButtonTestInsp->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonTestInsp->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonTestInsp->SetDraw3D(TRUE);

    m_pButtonInitGraph->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonInitGraph->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonInitGraph->SetDraw3D(TRUE);

    m_pButtonCalResol->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonCalResol->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonCalResol->SetDraw3D(TRUE);

    m_pButtomExportData->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtomExportData->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtomExportData->SetDraw3D(TRUE);

    
    //2024/09/24
    CString strStatus;
    strStatus.Format(_T("CPU, Memory Status"));
    m_pStaticMemoryStatus->ModifyStyle(0x0FL, SS_OWNERDRAW);
    m_pStaticMemoryStatus->SetBackGradation(RGB(157, 195, 230), RGB(96, 158, 214), RGB(42, 105, 162), 1);
    m_pStaticMemoryStatus->SetFont(_T("HY 견고딕"), RGB(255, 255, 255), 15, 1, 5);
    m_pStaticMemoryStatus->SetWindowText(strStatus);


    for (long cnt = 0; cnt < 2; cnt++)
    {
        if (cnt == 1)
        {
            CString caption;
            caption.Format(_T("HW Control"));
            m_pButtonHWControl->Create(
                caption, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON, CRect(10, 60, 100, 100), this, IDC_BUTTON_HW_CONTROL);
            m_pButtonHWControl->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
            m_pButtonHWControl->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
            m_pButtonHWControl->SetDraw3D(TRUE);
        }
        else
        {
            CString caption;
            caption.Format(_T("RESERVE%d"), cnt + 1);
            m_pButtonReserve[cnt]->Create(caption, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                CRect(10, 10 + (50 * (cnt + 1)), 100, 50 + (50 * (cnt + 1))), this, IDC_BUTTON_RESERVE1 + cnt);
            m_pButtonReserve[cnt]->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
            m_pButtonReserve[cnt]->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
            m_pButtonReserve[cnt]->SetDraw3D(TRUE);
        }
        
    }




    m_pButtonInspStart->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonInspStart->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonInspStart->SetDraw3D(TRUE);

    m_pButtonInspEnd->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonInspEnd->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonInspEnd->SetDraw3D(TRUE);

    m_pButtonInspDlg->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonInspDlg->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonInspDlg->SetDraw3D(TRUE);

    m_pButtonBatchInsp->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonBatchInsp->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonBatchInsp->SetDraw3D(TRUE);

    m_pButtonData->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonData->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonData->SetDraw3D(TRUE);

    m_pButtonSystem->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonSystem->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonSystem->SetDraw3D(TRUE);

    m_pButtonRecipe->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonRecipe->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonRecipe->SetDraw3D(TRUE);

    m_pButtonCamSetting->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonCamSetting->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonCamSetting->SetDraw3D(TRUE);

    m_pButtonSpec->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonSpec->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonSpec->SetDraw3D(TRUE);

    m_pButtonLogin->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonLogin->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonLogin->SetDraw3D(TRUE);

    //m_pButtonPGMClose->LoadImageFromResource(_T("JPG"), IDR_JPG_CLOSE);
    m_pButtonPGMClose->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonPGMClose->SetBackGradation(RGB(237, 26, 4), RGB(255, 163, 73), 1);
    m_pButtonPGMClose->SetDraw3D(TRUE);

    m_pButtonViewmodeMain->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 10, 6, 5);
    m_pButtonViewmodeMain->SetBackGradation(RGB(49, 191, 252), RGB(179, 202, 254), 1);
    m_pButtonViewmodeMain->SetDraw3D(TRUE);

    m_pButtonViewmodeSurface->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 10, 6, 5);
    m_pButtonViewmodeSurface->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonViewmodeSurface->SetDraw3D(TRUE);

    m_pButtonViewmodeTeaching->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 10, 6, 5);
    //m_pButtonViewmodeTeaching->SetBackGradation(RGB(224, 255, 139), RGB(242, 255, 157), RGB(255, 255, 175), 1);
    m_pButtonViewmodeTeaching->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonViewmodeTeaching->SetDraw3D(TRUE);

    for (long cnt = 0; cnt < 4; cnt++)
    {
        CString caption;
        if (cnt % 2 == 0)
        {
            caption.Format(_T("Camera%d_B"), (cnt / 2) + 1);
        }
        if (cnt % 2 == 1)
        {
            caption.Format(_T("Camera%d_D"), (cnt / 2) + 1);
        }

        m_pButtomCamera[cnt]->Create(
            caption, BS_OWNERDRAW | WS_VISIBLE | BS_CHECKBOX, CRect(10, 10, 10, 10), this, IDC_BUTTON_CAM1_B + cnt);
        m_pButtomCamera[cnt]->SetBackGradation(RGB(200, 200, 200), RGB(230, 230, 230), 1);
        m_pButtomCamera[cnt]->SetDraw3D(TRUE);
        m_pButtomCamera[cnt]->SetWindowText(caption);
        m_pButtomCamera[cnt]->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 12, 6, 5);
        m_pButtomCamera[cnt]->m_nFlags = 0;
    }


    m_pButtonDataSearch->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 6, 5);
    m_pButtonDataSearch->SetBackGradation(RGB(254, 247, 161), RGB(254, 223, 21), 1);
    m_pButtonDataSearch->SetDraw3D(TRUE);

    if(m_cisComm->GetExecutorType() == ExecutorType::SharedMem)
    {
        m_pButtonInspStart->EnableWindow(FALSE);
        m_pButtonInspEnd->EnableWindow(FALSE);
        m_pButtonCamSetting->EnableWindow(TRUE);
    }
}

void CIBCIDlg::UpdateControlLayer()
{
    CRect clientRegion;
    GetClientRect(clientRegion);

    long cx = clientRegion.Width();
    long cy = clientRegion.Height();

    long titleSizeY = 35;
    long logoSizeX = (long)(cx * 0.2);
    long logoSizeY = titleSizeY * 2;

    long nOffsetStartX = 5;
    long nOffsetStartY = 0;
    long nOffsetGapX = 0;
    long nOffsetGapY = 3;

    // First Line
    m_pStaticLogo->MoveWindow(nOffsetStartX + 0, nOffsetStartY + 0, logoSizeX, logoSizeY);

    // Second Line
    m_pStaticMainTitle->MoveWindow(nOffsetStartX + 0, nOffsetStartY + logoSizeY, (int)(cx * 0.1), titleSizeY);
    m_pStaticCurTime->MoveWindow(
        nOffsetStartX + (int)(cx * 0.1 + 1), nOffsetStartY + logoSizeY, (int)(cx * 0.1), titleSizeY);
    m_pStaticNameMachine->MoveWindow(
        nOffsetStartX + (int)(cx * 0.2 + 2), nOffsetStartY + titleSizeY, (int)(cx * 0.15 + 1), titleSizeY);
    m_pStaticNameVision->MoveWindow(
        nOffsetStartX + (int)(cx * 0.2 + 3) + (int)(cx * 0.15 + 1), nOffsetStartY + titleSizeY, (int)(cx * 0.15 + 1), titleSizeY);
    m_pStaticNamePosition->MoveWindow(
        nOffsetStartX + (int)(cx * 0.2 + 4) + (int)((cx * 0.15 + 1) * 2), nOffsetStartY + titleSizeY, (int)(cx * 0.1 + 1), titleSizeY);
    m_pStaticNameRecipe->MoveWindow(
        nOffsetStartX + (int)(cx * 0.2 + 2), nOffsetStartY + (2 * titleSizeY), (int)(cx * 0.2 + 2), titleSizeY);
    m_pStaticNameModel->MoveWindow(
        nOffsetStartX + (int)(cx * 0.2 + 2) + (int)(cx * 0.2 + 3), nOffsetStartY + (2 * titleSizeY), (int)(cx * 0.2 + 2), titleSizeY);
    m_pStaticQueueStatus->MoveWindow(
        nOffsetStartX + (int)(cx * 0.2 + 2), nOffsetStartY + logoSizeY + titleSizeY, (int)(cx * 0.4 + 5), titleSizeY);

    //2024.09.24
    m_pStaticMemoryStatus->MoveWindow(nOffsetStartX + (int)(cx * 0.2 + 2),
        nOffsetStartY + logoSizeY + titleSizeY + titleSizeY, (int)(cx * 0.4 + 5), titleSizeY);

    m_pStaticBtnInsp->MoveWindow(
        nOffsetStartX + (int)(cx * 0.65), nOffsetStartY + logoSizeY - titleSizeY, (int)(cx * 0.04), (int)(cx * 0.04));

    m_pButtonTestInsp->MoveWindow(
        nOffsetStartX + (int)(cx * 0.70), nOffsetStartY + logoSizeY + titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonInitGraph->MoveWindow(
        nOffsetStartX + (int)(cx * 0.75), nOffsetStartY + logoSizeY + titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonCalResol->MoveWindow(
        nOffsetStartX + (int)(cx * 0.80), nOffsetStartY + logoSizeY + titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonDataSearch->MoveWindow(
        nOffsetStartX + (int)(cx * 0.85), nOffsetStartY + logoSizeY + titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtomExportData->MoveWindow(
        nOffsetStartX + (int)(cx * 0.70), nOffsetStartY + logoSizeY + (2 * titleSizeY), (int)(cx * 0.05), titleSizeY);

    for (long cnt = 0; cnt < 2; cnt++)
    {
        float pos_x;
        if (cnt == 1)
        {
            pos_x = 0.90 + (0 * 0.05);
            m_pButtonHWControl->MoveWindow(nOffsetStartX + (int)(cx * pos_x), nOffsetStartY + logoSizeY + titleSizeY,
                (int)(cx * 0.05), titleSizeY);
        }
        else
        {
            pos_x = 0.90 + (1 * 0.05);
            m_pButtonReserve[cnt]->MoveWindow(nOffsetStartX + (int)(cx * pos_x), nOffsetStartY + logoSizeY + titleSizeY,
                (int)(cx * 0.05), titleSizeY);
        }
        
    }
   
    m_pButtonInspStart->MoveWindow(
        nOffsetStartX + (int)(cx * 0.70), nOffsetStartY + logoSizeY - titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonInspEnd->MoveWindow(
        nOffsetStartX + (int)(cx * 0.70), nOffsetStartY + logoSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonInspDlg->MoveWindow(
        nOffsetStartX + (int)(cx * 0.75), nOffsetStartY + logoSizeY - titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonBatchInsp->MoveWindow(
        nOffsetStartX + (int)(cx * 0.75), nOffsetStartY + logoSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonData->MoveWindow(
        nOffsetStartX + (int)(cx * 0.80), nOffsetStartY + logoSizeY - titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonSystem->MoveWindow(
        nOffsetStartX + (int)(cx * 0.80), nOffsetStartY + logoSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonRecipe->MoveWindow(
        nOffsetStartX + (int)(cx * 0.85), nOffsetStartY + logoSizeY - titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonCamSetting->MoveWindow(
        nOffsetStartX + (int)(cx * 0.85), nOffsetStartY + logoSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonSpec->MoveWindow(
        nOffsetStartX + (int)(cx * 0.90), nOffsetStartY + logoSizeY - titleSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonLogin->MoveWindow(
        nOffsetStartX + (int)(cx * 0.90), nOffsetStartY + logoSizeY, (int)(cx * 0.05), titleSizeY);
    m_pButtonPGMClose->MoveWindow(
        nOffsetStartX + (int)(cx * 0.95), nOffsetStartY + logoSizeY - titleSizeY, (int)(cx * 0.05), logoSizeY);

    // Third Line
    // m_pStaticStatusCamera->MoveWindow(nOffsetStartX + 0, nOffsetStartY + logoSizeY + titltSizeY + nOffsetGapY, (int)(cx * 0.026), titltSizeY);
    m_pStaticStatusCamera->MoveWindow(
        nOffsetStartX + 1, nOffsetStartY + logoSizeY + titleSizeY + nOffsetGapY, (int)(cx * 0.02), titleSizeY);
    m_pStaticStatusMaster->MoveWindow(nOffsetStartX + (int)(cx * 0.03 + 2),
        nOffsetStartY + logoSizeY + titleSizeY + nOffsetGapY, (int)(cx * 0.02), titleSizeY);
    m_pStaticStatusFriendVision->MoveWindow(nOffsetStartX + (int)(cx * 0.06 + 3),
        nOffsetStartY + logoSizeY + titleSizeY + nOffsetGapY, (int)(cx * 0.02), titleSizeY);
    m_pStaticStatusRes0->MoveWindow(nOffsetStartX + (int)(cx * 0.09 + 4),
        nOffsetStartY + logoSizeY + titleSizeY + nOffsetGapY, (int)(cx * 0.02), titleSizeY);
    m_pStaticStatusRes1->MoveWindow(nOffsetStartX + (int)(cx * 0.12 + 5),
        nOffsetStartY + logoSizeY + titleSizeY + nOffsetGapY, (int)(cx * 0.02), titleSizeY);

    // Fourth Line
    m_pStaticStatusCaptionCamera->MoveWindow(
        nOffsetStartX + 0, nOffsetStartY + logoSizeY + titleSizeY * 2 + nOffsetGapY, (int)(cx * 0.03), titleSizeY);
    m_pStaticStatusCaptionMaster->MoveWindow(nOffsetStartX + (int)(cx * 0.03),
        nOffsetStartY + logoSizeY + titleSizeY * 2 + nOffsetGapY, (int)(cx * 0.03), titleSizeY);
    m_pStaticStatusCaptionFriendVision->MoveWindow(nOffsetStartX + (int)(cx * 0.06),
        nOffsetStartY + logoSizeY + titleSizeY * 2 + nOffsetGapY, (int)(cx * 0.03), titleSizeY);
    m_pStaticStatusCaptionRes0->MoveWindow(nOffsetStartX + (int)(cx * 0.09),
        nOffsetStartY + logoSizeY + titleSizeY * 2 + nOffsetGapY, (int)(cx * 0.03), titleSizeY);
    m_pStaticStatusCaptionRes1->MoveWindow(nOffsetStartX + (int)(cx * 0.12),
        nOffsetStartY + logoSizeY + titleSizeY * 2 + nOffsetGapY, (int)(cx * 0.03), titleSizeY);

    // Fifth Line
    m_pButtonViewmodeMain->MoveWindow(
        nOffsetStartX + 0, nOffsetStartY + logoSizeY + titleSizeY * 3 + nOffsetGapY, (int)(cx * 0.05), titleSizeY);
    m_pButtonViewmodeSurface->MoveWindow(nOffsetStartX + (int)(cx * 0.05),
        nOffsetStartY + logoSizeY + titleSizeY * 3 + nOffsetGapY, (int)(cx * 0.05), titleSizeY);
    m_pButtonViewmodeTeaching->MoveWindow(nOffsetStartX + (int)(cx * 0.10),
        nOffsetStartY + logoSizeY + titleSizeY * 3 + nOffsetGapY, (int)(cx * 0.05), titleSizeY);

    for (long cnt = 0; cnt < 4; cnt++)
    {
        m_pButtomCamera[cnt]->MoveWindow(nOffsetStartX + cx * 0.16 + (int)(cx * 0.05 * cnt),
            nOffsetStartY + logoSizeY + titleSizeY * 3 + nOffsetGapY, (int)(cx * 0.05), titleSizeY);
    }

      if (m_nModeView == ENUM_MODE_VIEW::MAIN)
    {
        GetDlgItem(IDC_BUTTON_CAM1_B)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_CAM1_D)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_CAM2_B)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_CAM2_D)->ShowWindow(SW_HIDE);
    }
    else if (m_nModeView == ENUM_MODE_VIEW::SURFACE)
    {
        GetDlgItem(IDC_BUTTON_CAM1_B)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_CAM1_D)->ShowWindow(SW_SHOW);

        if (g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
        {
            GetDlgItem(IDC_BUTTON_CAM2_B)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_BUTTON_CAM2_D)->ShowWindow(SW_SHOW);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_CAM2_B)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_BUTTON_CAM2_D)->ShowWindow(SW_HIDE);
        }
    }
    else
    {
        GetDlgItem(IDC_BUTTON_CAM1_B)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_CAM1_D)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_CAM2_B)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUTTON_CAM2_D)->ShowWindow(SW_SHOW);
    }
}

void CIBCIDlg::OnCancel()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다..

    //CString strExit;
    //if (g_systemParameter.nLanguage == 1)
    //{
    //	strExit = _T("종료하시겠습니까?");
    //}
    //else if (g_systemParameter.nLanguage == 3)
    //{
    //	strExit = _T("确定要结束吗？");
    //}
    //else
    //{
    //	strExit = _T("Are you sure want to quit?");
    //}
    CString strExit = _T("Are you sure want to quit?");
    if (MessageBox(strExit, _T("IBCI"), MB_YESNO | MB_TOPMOST) == IDYES)
    {
        //m_pClientSocket->SendVisionNormalTurnOff((long)TCPMSG_V2I::VISION_NORMAL_TURN_OFF);
        //if (Share::GenerationVersion::SplitterRemoved == g_systemParameter.nGenerationVersion)
        //	m_pClientSocket_Surface->SendVisionNormalTurnOff((long)TCPMSG_V2I::VISION_NORMAL_TURN_OFF);
        CDialogEx::OnCancel();
    }
}

bool CIBCIDlg::RecipeLoad(CString strPath)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    //BOOL CMainDlg::LoadRecipe(CString strPath)
    CString strModelName;
    
    if (strPath.GetLength() <= 0)
    {
        const TCHAR szFilters[] = _T("Job Files (*.job)|*.job||");

        CFileDialog dlg(TRUE, _T("Job"), _T("*.job"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);
        dlg.m_ofn.lpstrInitialDir = Inspection::Accessor::Get().m_strPrevJobFolder;
        if (dlg.DoModal() == IDOK)
        {
            strPath = dlg.GetPathName();
            strModelName = dlg.GetFileName();
            strModelName = strModelName.Left(strModelName.GetLength() - 4);
            g_systemParameter.m_currentModel = strModelName;
            Inspection::Accessor::Get().m_strPrevJobFolder = dlg.GetPathName().Left(dlg.GetPathName().ReverseFind('\\') + 1);
        }
    }
    else
    {
        strPath.Format(_T("%s\\%s.job"), Path::GetRecipeFolder(), (LPCTSTR)g_systemParameter.m_currentModel);
    }

    /*if (strPath.GetLength() > 0)
			filePath.Format(_T("%s\\%s.job"), strPath, (LPCTSTR)g_systemParameter.m_currentModel);
		else
			filePath.Format(_T("%s\\%s.job"), Path::GetRecipeFolder(), (LPCTSTR)g_systemParameter.m_currentModel);*/

    // Job File을 Open한다.
    CFileFind find;
    if (!find.FindFile(strPath))
    {
        AfxMessageBox(_T("Can't find recipe!"));
        //g_systemParameter.m_currentModel = _T("");

        return false; // FALSE;
    }
    
    Database dbJob;
    //Base::ParaCollector_UpdateOldPara(true);
    if (!dbJob.Load(strPath))
    {
        //g_systemParameter.m_currentModel = _T("");
        //Base::ParaCollector_UpdateOldPara(false);

        return false; // FALSE;
    }

    Inspection::Accessor::GetRoot().LinkDataBase(false, dbJob);
    //m_paraSurface->LinkDataBase(false, dbJob[_T("Surface")]);
    //m_paraSurface->CopyTo(*m_surfaceInspect);

    //m_cisComm->LinkDataBase(false, dbJob);
    //
    LoadCellDesign(dbJob);

    auto& inspectionRoot = Inspection::Accessor::GetRoot();

    //inspectionRoot.SetVersion(g_systemParameter.nGenerationVersion, (int)g_systemParameter.nCustomerSite, g_systemParameter.nCISInspType);
    /*inspectionRoot.SetPixelToUmX(48, 48);
		inspectionRoot.SetPixelToUmY(100);
		inspectionRoot.SetCellCuttingMargin(0);*/
    CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    //unitUp->SetPixelToUmX(28.05);
    //unitUp->SetPixelToUmX(28.0071); // 0503 - MSA Top
    //unitUp->SetPixelToUmX(28.005); // 0503 - MSA Back
    //unitUp->SetPixelToUmY(28.5);

    if (g_systemParameter.bDirection == 0)
        unitUp->SetPixelToUmX(g_systemParameter.m_dPixelToUmXUp);
    else if (g_systemParameter.bDirection == 1)
        unitUp->SetPixelToUmX(g_systemParameter.m_dPixelToUmXDown);

    unitUp->SetPixelToUmY(g_systemParameter.m_dPixelToUmY);

    m_nCropPixel_X = 15000 / unitUp->m_pixelToUmX;
    m_nCropPixel_Y = 15000 / unitUp->m_pixelToUmY;
    
    CInspectionCellAlign* upCellAlign = (CInspectionCellAlign*)unitUp->GetProcessingItem(ProcItemCategory::cellAlign);

    CInspectionMeasure* CellMeasure = (CInspectionMeasure*)unitUp->GetProcessingItem(ProcItemCategory::measure);
    ResultMismatch resMismatch;
    ResultSpec resSpec;
    ResultLaneData resLane;

    resLane.fLaneWidth = 0.f;
    resLane.ptXLaneLeft = 0.f;
    resLane.ptXLaneRight = 0.f;

    for (long meas = 0; meas < CellMeasure->m_para.m_nCountMeasureItem; meas++)
    {
        CString text;
        LanguageDef langDef;
        float usl, lsl, ucl, lcl, standard;
        float fDefault = 0.f;
        switch (CellMeasure->m_para.m_vecMeasureType[meas])
        {
            case 1:
                //text.Format(_T("[mm] %d B Left Coating"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_B_LeftCoating, length, text);
                standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];

                m_pDlgGraphWindow->GetGraphFrame(1)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, usl, lsl);

                resSpec.fMeasureStandard = standard;
                resSpec.fMeasureOffsetUSL = usl;
                resSpec.fMeasureOffsetLSL = lsl;
                resSpec.fMeasureOffsetUCL = ucl;
                resSpec.fMeasureOffsetLCL = lcl;

                resMismatch.m_vecResSpec.push_back(resSpec);
                resMismatch.m_vecfDiffWidth.push_back(fDefault);
                resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                resMismatch.m_vecResUpLane.push_back(resLane);
                resMismatch.m_vecResDownLane.push_back(resLane);
                break;

            case 2:
                //text.Format(_T("[mm] %d C Right Coating"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_C_RightCoating, length, text);
                if (g_systemParameter.nLaneCount == 2)
                {
                    standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                    usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                    lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                    ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                    lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                    m_pDlgGraphWindow->GetGraphFrame(2)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, usl, lsl);

                    resSpec.fMeasureStandard = standard;
                    resSpec.fMeasureOffsetUSL = usl;
                    resSpec.fMeasureOffsetLSL = lsl;
                    resSpec.fMeasureOffsetUCL = ucl;
                    resSpec.fMeasureOffsetLCL = lcl;

                    resMismatch.m_vecResSpec.push_back(resSpec);
                    resMismatch.m_vecfDiffWidth.push_back(fDefault);
                    resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                    resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                    resMismatch.m_vecResUpLane.push_back(resLane);
                    resMismatch.m_vecResDownLane.push_back(resLane);
                }
                break;
            case 3:
                //text.Format(_T("[mm] %d D Left Foil"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_D_LeftFoil, length, text);
                standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                m_pDlgGraphWindow->GetGraphFrame(0)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, usl, lsl);

                resSpec.fMeasureStandard = standard;
                resSpec.fMeasureOffsetUSL = usl;
                resSpec.fMeasureOffsetLSL = lsl;
                resSpec.fMeasureOffsetUCL = ucl;
                resSpec.fMeasureOffsetLCL = lcl;

                resMismatch.m_vecResSpec.push_back(resSpec);
                resMismatch.m_vecfDiffWidth.push_back(fDefault);
                resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                resMismatch.m_vecResUpLane.push_back(resLane);
                resMismatch.m_vecResDownLane.push_back(resLane);
                break;
            case 7:
                //text.Format(_T("[mm] %d F RightFoil"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_F_RightFoil, length, text);
                standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];

                if (g_systemParameter.nLaneCount == 2)
                {
                    m_pDlgGraphWindow->GetGraphFrame(3)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, usl, lsl);
                }
                else if (g_systemParameter.nLaneCount == 1)
                {
                    m_pDlgGraphWindow->GetGraphFrame(2)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, usl, lsl);
                }

                resSpec.fMeasureStandard = standard;
                resSpec.fMeasureOffsetUSL = usl;
                resSpec.fMeasureOffsetLSL = lsl;
                resSpec.fMeasureOffsetUCL = ucl;
                resSpec.fMeasureOffsetLCL = lcl;

                resMismatch.m_vecResSpec.push_back(resSpec);
                resMismatch.m_vecfDiffWidth.push_back(fDefault);
                resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                resMismatch.m_vecResUpLane.push_back(resLane);
                resMismatch.m_vecResDownLane.push_back(resLane);
                break;
            default:
                break;
        }
    }

    //if (resMismatch.m_vecResSpec.size() < 4 && g_systemParameter.nLaneCount == 2)
        //return 0;
    //if (resMismatch.m_vecResSpec.size() < 3 && g_systemParameter.nLaneCount == 1)
        //return 0;
    if ((resMismatch.m_vecResSpec.size() >= 3 && g_systemParameter.nLaneCount == 1)
         || (resMismatch.m_vecResSpec.size() >= 4 && g_systemParameter.nLaneCount == 2))
        m_pDlgAnimationWindow->SetAnimationParameter(
            resMismatch, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, g_systemParameter.nLaneCount, g_systemParameter.bOSDS, g_systemParameter.bRollerOS);

    //CELL_DESIGN m_cellDesignParam;
    //	Inspection::Accessor::GetRoot().SetCellDesign(m_cellDesignParam);
    //Base::ParaCollector_UpdateOldPara(false);

    //m_viewCameraSettingDlg->UpdateUI_Parameter();

    CString strText;
    //strText.Format(_T("%s Recipe Open"), (LPCTSTR)g_systemParameter.m_currentModel);
    //m_loggerSequence.Log(0, strText);

    //CopyToISI(true, false);
    m_bOpenJob = true;

    //Open Recipe 표시
    m_pStaticNameRecipe->SetWindowTextW((LPCTSTR)g_systemParameter.m_currentModel);
    m_pStaticNameModel->SetWindowTextW((LPCTSTR)g_systemParameter.m_currentModel);

    return true;
}

void CIBCIDlg::ReDrawMismatchAnimation()
{
    CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionMeasure* CellMeasure = (CInspectionMeasure*)unitUp->GetProcessingItem(ProcItemCategory::measure);
    ResultMismatch resMismatch;
    ResultSpec resSpec;
    ResultLaneData resLane;

    resLane.fLaneWidth = 0.f;
    resLane.ptXLaneLeft = 0.f;
    resLane.ptXLaneRight = 0.f;

    for (long meas = 0; meas < CellMeasure->m_para.m_nCountMeasureItem; meas++)
    {
        CString text;
        LanguageDef langDef;
        float usl, lsl, ucl, lcl, standard;
        float fDefault = 0.f;
        switch (CellMeasure->m_para.m_vecMeasureType[meas])
        {
            case 1:
                //text.Format(_T("[mm] %d B Left Coating"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_B_LeftCoating, length, text);
                standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];

                //m_pDlgGraphWindow->GetGraphFrame(1)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, -9954, -9954);

                resSpec.fMeasureStandard = standard;
                resSpec.fMeasureOffsetUSL = usl;
                resSpec.fMeasureOffsetLSL = lsl;
                resSpec.fMeasureOffsetUCL = ucl;
                resSpec.fMeasureOffsetLCL = lcl;

                resMismatch.m_vecResSpec.push_back(resSpec);
                resMismatch.m_vecfDiffWidth.push_back(fDefault);
                resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                resMismatch.m_vecResUpLane.push_back(resLane);
                resMismatch.m_vecResDownLane.push_back(resLane);
                break;

            case 2:
                //text.Format(_T("[mm] %d C Right Coating"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_C_RightCoating, length, text);
                if (g_systemParameter.nLaneCount == 2)
                {
                    standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                    usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                    lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                    ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                    lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                    //m_pDlgGraphWindow->GetGraphFrame(2)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, -9954, -9954);

                    resSpec.fMeasureStandard = standard;
                    resSpec.fMeasureOffsetUSL = usl;
                    resSpec.fMeasureOffsetLSL = lsl;
                    resSpec.fMeasureOffsetUCL = ucl;
                    resSpec.fMeasureOffsetLCL = lcl;

                    resMismatch.m_vecResSpec.push_back(resSpec);
                    resMismatch.m_vecfDiffWidth.push_back(fDefault);
                    resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                    resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                    resMismatch.m_vecResUpLane.push_back(resLane);
                    resMismatch.m_vecResDownLane.push_back(resLane);
                }
                break;
            case 3:
                //text.Format(_T("[mm] %d D Left Foil"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_D_LeftFoil, length, text);
                standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];
                //m_pDlgGraphWindow->GetGraphFrame(0)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, -9954, -9954);

                resSpec.fMeasureStandard = standard;
                resSpec.fMeasureOffsetUSL = usl;
                resSpec.fMeasureOffsetLSL = lsl;
                resSpec.fMeasureOffsetUCL = ucl;
                resSpec.fMeasureOffsetLCL = lcl;

                resMismatch.m_vecResSpec.push_back(resSpec);
                resMismatch.m_vecfDiffWidth.push_back(fDefault);
                resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                resMismatch.m_vecResUpLane.push_back(resLane);
                resMismatch.m_vecResDownLane.push_back(resLane);
                break;
            case 7:
                //text.Format(_T("[mm] %d F RightFoil"), ver);
                //debugInfo->AddText(LanguageDef::Debug_Coater_F_RightFoil, length, text);
                standard = CellMeasure->m_para.m_vecMeasureStandard[meas];

                usl = CellMeasure->m_para.m_vecMeasureOffsetUSL[meas];
                lsl = CellMeasure->m_para.m_vecMeasureOffsetLSL[meas];

                ucl = CellMeasure->m_para.m_vecMeasureOffsetUCL[meas];
                lcl = CellMeasure->m_para.m_vecMeasureOffsetLCL[meas];

                /*if (g_systemParameter.nLaneCount == 2)
                {
                    m_pDlgGraphWindow->GetGraphFrame(3)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, -9954, -9954);
                }
                else if (g_systemParameter.nLaneCount == 1)
                {
                    m_pDlgGraphWindow->GetGraphFrame(2)->SetSpec(10.f, 0.f, usl, lsl, lcl, ucl, -9954, -9954);
                }*/

                resSpec.fMeasureStandard = standard;
                resSpec.fMeasureOffsetUSL = usl;
                resSpec.fMeasureOffsetLSL = lsl;
                resSpec.fMeasureOffsetUCL = ucl;
                resSpec.fMeasureOffsetLCL = lcl;

                resMismatch.m_vecResSpec.push_back(resSpec);
                resMismatch.m_vecfDiffWidth.push_back(fDefault);
                resMismatch.m_vecPtXDiffLeft.push_back(fDefault);
                resMismatch.m_vecPtXDiffRight.push_back(fDefault);
                resMismatch.m_vecResUpLane.push_back(resLane);
                resMismatch.m_vecResDownLane.push_back(resLane);
                break;
            default:
                break;
        }
    }

    //if (resMismatch.m_vecResSpec.size() < 4 && g_systemParameter.nLaneCount == 2)
    //    //return ;
    //if (resMismatch.m_vecResSpec.size() < 3 && g_systemParameter.nLaneCount == 1)
        //return ;
    if ((resMismatch.m_vecResSpec.size() >= 3 && g_systemParameter.nLaneCount == 1)
        || (resMismatch.m_vecResSpec.size() >= 4 && g_systemParameter.nLaneCount == 2))
        m_pDlgAnimationWindow->SetAnimationParameter(resMismatch, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            g_systemParameter.nLaneCount, g_systemParameter.bOSDS, g_systemParameter.bRollerOS);
}

bool CIBCIDlg::RecipeSave()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString strPath;
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    //BOOL CMainDlg::LoadRecipe(CString strPath)
    const TCHAR szFilters[] = _T("Job Files (*.job)|*.job||");

    CFileDialog dlg(TRUE, _T("Job"), _T("*.job"),
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);

    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();
        /*if (strPath.GetLength() > 0)
			filePath.Format(_T("%s\\%s.job"), strPath, (LPCTSTR)g_systemParameter.m_currentModel);
		else
			filePath.Format(_T("%s\\%s.job"), Path::GetRecipeFolder(), (LPCTSTR)g_systemParameter.m_currentModel);*/

        // Job File을 Open한다.
        CFileFind find;
        if (!find.FindFile(filePath))
        {
            AfxMessageBox(_T("Can't find recipe!"));
            //g_systemParameter.m_currentModel = _T("");
            return false; // FALSE;
        }

        Database dbJob;
        //Base::ParaCollector_UpdateOldPara(true);
        Inspection::Accessor::GetRoot().LinkDataBase(true, dbJob);
        //m_paraSurface->LinkDataBase(false, dbJob[_T("Surface")]);
        //m_paraSurface->CopyTo(*m_surfaceInspect);

        if (!dbJob.Save(filePath))
        {
            //g_systemParameter.m_currentModel = _T("");
            //Base::ParaCollector_UpdateOldPara(false);

            return false; // FALSE;
        }

        //m_cisComm->LinkDataBase(false, dbJob);
        //
        SaveCellDesign(dbJob);
        m_dlgAjinMot->SaveInspectionPos(dbJob);
        return true;
    }

    return false;
}

void CIBCIDlg::SaveCellDesign(Database& dbJob)
{
    auto& db_pos = dbJob[_T("Design Spec")];

    CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionCellAlign* upCellAlign = (CInspectionCellAlign*)unitUp->GetProcessingItem(ProcItemCategory::cellAlign);
    db_pos[_T("Cell Height")].Link(TRUE, upCellAlign->m_cellDesign.fCellHeight);
    db_pos[_T("Cell Height 2")].Link(TRUE, upCellAlign->m_cellDesign.fCellHeight2);
    db_pos[_T("Cell Width")].Link(TRUE, upCellAlign->m_cellDesign.fCellWidth);
    db_pos[_T("Tab Height")].Link(TRUE, upCellAlign->m_cellDesign.fTabHeight);
    db_pos[_T("Tab Height 2")].Link(TRUE, upCellAlign->m_cellDesign.fTabHeight2);
    db_pos[_T("Tab Width")].Link(TRUE, upCellAlign->m_cellDesign.fTabWidth);
    db_pos[_T("Tab Offset")].Link(TRUE, upCellAlign->m_cellDesign.fTabOffset);
    db_pos[_T("V Home Height")].Link(TRUE, upCellAlign->m_cellDesign.fVhomeHeight);
    db_pos[_T("V Home Size")].Link(TRUE, upCellAlign->m_cellDesign.fVHomeSize);
    db_pos[_T("Tab Sensor Delay Offset")].Link(TRUE, upCellAlign->m_cellDesign.fTabSensorDelayOffset);
    db_pos[_T("Mold Constant")].Link(TRUE, upCellAlign->m_cellDesign.fMoldConstant);
    db_pos[_T("V home OffSet UP")].Link(TRUE, upCellAlign->m_cellDesign.nVhomeOffsetUP);
    db_pos[_T("V home OffSet DOWN")].Link(TRUE, upCellAlign->m_cellDesign.nVhomeOffsetDOWN);
}
void CIBCIDlg::LoadCellDesign(Database& dbJob)
{
    auto& db_pos = dbJob[_T("Design Spec")];
    CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    CInspectionCellAlign* upCellAlign = (CInspectionCellAlign*)unitUp->GetProcessingItem(ProcItemCategory::cellAlign);
    if (true != db_pos[_T("Cell Height")].Link(FALSE, upCellAlign->m_cellDesign.fCellHeight))
        upCellAlign->m_cellDesign.fCellHeight = 0;
    if (true != db_pos[_T("Cell Height 2")].Link(FALSE, upCellAlign->m_cellDesign.fCellHeight2))
        upCellAlign->m_cellDesign.fCellHeight2 = 0;
    if (true != db_pos[_T("Cell Width")].Link(FALSE, upCellAlign->m_cellDesign.fCellWidth))
        upCellAlign->m_cellDesign.fCellWidth = 120;
    if (true != db_pos[_T("Tab Height")].Link(FALSE, upCellAlign->m_cellDesign.fTabHeight))
        upCellAlign->m_cellDesign.fTabHeight = 0;
    if (true != db_pos[_T("Tab Height 2")].Link(FALSE, upCellAlign->m_cellDesign.fTabHeight2))
        upCellAlign->m_cellDesign.fTabHeight2 = 0;
    if (true != db_pos[_T("Tab Width")].Link(FALSE, upCellAlign->m_cellDesign.fTabWidth))
        upCellAlign->m_cellDesign.fTabWidth = 0;
    if (true != db_pos[_T("Tab Offset")].Link(FALSE, upCellAlign->m_cellDesign.fTabOffset))
        upCellAlign->m_cellDesign.fTabOffset = 0;
    if (true != db_pos[_T("V Home Size")].Link(FALSE, upCellAlign->m_cellDesign.fVHomeSize))
        upCellAlign->m_cellDesign.fVHomeSize = 0;
    if (true != db_pos[_T("V Home Height")].Link(FALSE, upCellAlign->m_cellDesign.fVhomeHeight))
        upCellAlign->m_cellDesign.fVhomeHeight = 0;
    if (true != db_pos[_T("Tab Sensor Delay Offset")].Link(FALSE, upCellAlign->m_cellDesign.fTabSensorDelayOffset))
        upCellAlign->m_cellDesign.fTabSensorDelayOffset = 0;
    if (true != db_pos[_T("Mold Constant")].Link(FALSE, upCellAlign->m_cellDesign.fMoldConstant))
        upCellAlign->m_cellDesign.fMoldConstant
            = (upCellAlign->m_cellDesign.fCellWidth - upCellAlign->m_cellDesign.fTabWidth);
    if (true != db_pos[_T("V home OffSet UP")].Link(FALSE, upCellAlign->m_cellDesign.nVhomeOffsetUP))
        upCellAlign->m_cellDesign.nVhomeOffsetUP = 0;
    if (true != db_pos[_T("V home OffSet DOWN")].Link(FALSE, upCellAlign->m_cellDesign.nVhomeOffsetDOWN))
        upCellAlign->m_cellDesign.nVhomeOffsetDOWN = 0;
}

void CIBCIDlg::OnBnClickedButtonInspectionDlg()
{
    {
        CString strResult;
        CString strPath;

        long len1 = strPath.GetLength();
        if (GetModuleFileName(nullptr, strPath.GetBuffer(_MAX_PATH + 1), MAX_PATH) != FALSE)
        {
            long len2 = strPath.GetLength();
            strPath.ReleaseBuffer();
            long len3 = strPath.GetLength();

            strResult = strPath.Left(strPath.ReverseFind('\\') + 1);
        }
        else
        {
        }
        WCHAR* abc = strPath.GetBuffer(0);
        abc = _T("abc");
        long len4 = strPath.GetLength();

        strPath = _T("111");
        long x = 10;
    }

    //m_loggerGrab.Log(0, _T("1234"));

    int nLanguage = 1;
    std::map<long, std::vector<CString>> m_mapExplainParamDimension;

    Inspection::SetupImage setupImage;
    long nType = (long)g_systemParameter.machineType;
    Inspection::Accessor::GetRoot().ShowDlg(nLanguage, m_mapExplainParamDimension, setupImage, nType);

    return;

    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDialogEx::OnOK();
}

void CIBCIDlg::OnBnClickedButton4()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgDefectmap == nullptr)
    {
        m_dlgDefectmap = new CDlgDefectMap;
        m_dlgDefectmap->Create(IDD_DIALOG_DEFECT_MAP, this);
    }
    m_dlgDefectmap->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButton5()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgAcc == nullptr)
    {
        m_dlgAcc = new CDlgAccDefectMap;
        m_dlgAcc->Create(IDD_DIALOG_ACC_DEFECTMAP, this);
        m_dlgAcc->CenterWindow(CWnd::GetDesktopWindow());
    }
    m_dlgAcc->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButton6()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgDefectView == nullptr)
    {
        m_dlgDefectView = new CDlgDefectImageView(4, 2);
        m_dlgDefectView->Create(IDD_DLG_DEFECTIMAGE_VIEW, this);

        m_dlgDefectView->CenterWindow(CWnd::GetDesktopWindow());
        m_dlgDefectView->SetDlg(this);

        m_vecISIAllDefect.clear(); // MAX = 40, 6*3 = 18
        // 예진 이미지 뿌린 개수에 맞게 개수 조절
        m_nDefectStartID = 0;
    }
    m_dlgDefectView->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButton7()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgDefectCount == nullptr)
    {
        m_dlgDefectCount = new CDlgDefectCount(3, 3);
        m_dlgDefectCount->Create(IDD_DLG_DEFECTCOUNT, this);
        m_dlgDefectCount->CenterWindow(CWnd::GetDesktopWindow());
    }
    m_dlgDefectCount->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButtonBatch()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgBatch == nullptr)
    {
        m_dlgBatch = new CBatchInsPection;
        m_dlgBatch->Create(IDD_DLG_BATCH_INSPECTION, this);
        m_dlgBatch->CenterWindow(CWnd::GetDesktopWindow());
        m_dlgBatch->SetDlg(this);
    }
    m_bBatchView = TRUE;
    m_dlgBatch->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButtonFnGraph()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_pDlgGraphWindow == nullptr)
    {
        m_pDlgGraphWindow = new CDlgGraphWindow(this);
        m_pDlgGraphWindow->Create(IDD_DIALOG_GRAPH_WINDOW, this);
        m_pDlgGraphWindow->CenterWindow(CWnd::GetDesktopWindow());

        BOOL bShow = TRUE;
        std::vector<DefectType> vecDisplayGraph;
        int nCount = 4;
        long type = 0;
        //nCount = g_systemParameter.inspItemInfo.GetGraphDisplayCount(vecDisplayGraph);
        //m_pDlgGraphWindow->SetGraphFrame(vecDisplayGraph, bShow);
        if (m_pDlgGraphWindow->m_pvecDlgGraphFrame.size() == 0)
        {
            nCount = 4;
        }

        for (int nIdx = 0; nIdx < nCount; nIdx++)
        {
            type = (long)DefectType::Start_Dimension_Mismatch + 1 + nIdx;
            vecDisplayGraph.push_back((DefectType)type);
        }

        m_pDlgGraphWindow->SetGraphFrame(vecDisplayGraph, bShow);

        m_pDlgGraphWindow->GetGraphFrame(0)->SetSpec(10.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
        m_pDlgGraphWindow->GetGraphFrame(1)->SetSpec(20.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
        m_pDlgGraphWindow->GetGraphFrame(2)->SetSpec(30.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
        m_pDlgGraphWindow->GetGraphFrame(3)->SetSpec(40.f, 0.f, -50.f, 50.f, -9954, -9954, -9954, -9954);
    }
    m_pDlgGraphWindow->ShowWindow(SW_SHOW);
}

void CIBCIDlg::InitializeResMismatchData(long nDataCnt, bool bInit)
{
    /*if (bInit)
	{
		m_stResAllMismatch.m_vecResUpLane.clear();
		m_stResAllMismatch.m_vecResDownLane.clear();
		m_stResAllMismatch.m_vecPtXDiffLeft.clear();
		m_stResAllMismatch.m_vecPtXDiffRight.clear();
		m_stResAllMismatch.m_vecfDiffWidth.clear();

		m_stResAllMismatch.m_nType = 0;
		m_stResAllMismatch.m_nCellIndex = 0;
		m_stResAllMismatch.m_strLotID = _T("");
	}
	else
	{
		for (int nIdx = 0; nIdx < nDataCnt; nIdx++)
		{
			m_stResAllMismatch.m_vecResUpLane.resize(nDataCnt);
			m_stResAllMismatch.m_vecResDownLane.resize(nDataCnt);
			m_stResAllMismatch.m_vecPtXDiffLeft.resize(nDataCnt);
			m_stResAllMismatch.m_vecPtXDiffRight.resize(nDataCnt);
			m_stResAllMismatch.m_vecfDiffWidth.resize(nDataCnt);
		}
	}*/
}

void CIBCIDlg::DisplayMismatchData()
{
    CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);

    CInspectionCell* unitDown = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    //===

    long nSizeDivideVertical = (long)unitUp->m_ISICellAllDefect.m_vvMeasureDefect.size();
    //InitializeResMismatchData(nSizeDivideVertical, true);
    //InitializeResMismatchData(nSizeDivideVertical);
    m_inlineResultCoupler->ClearLot();
    ResultMismatch stResAllMismatch; //Missmatch 변수 통합 Data로 빼야하나?
    std::vector<ResultLaneData> resvecLaneUp;
    std::vector<ResultSpec> resvecLaneSpec;
    float fFullLength = 0.f;

    for (int nVerticalIdx = 0; nVerticalIdx < nSizeDivideVertical; nVerticalIdx++)
    {
        auto& vecMeasureDefectUp = unitUp->m_ISICellAllDefect.m_vvMeasureDefect[nVerticalIdx];
        auto& vecMeasureDefectDown = unitDown->m_ISICellAllDefect.m_vvMeasureDefect[nVerticalIdx];

        long nSizeMeasureItem = (long)vecMeasureDefectUp.size();
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            ResultLaneData resLaneUp;
            ResultSpec resSpec;

            /*if (m_bOpenJob)
			{
				m_setSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

				m_setSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
				m_setSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

				m_setSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
				m_setSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

				m_bOpenJob = false;
			}*/
#if INSPECTION_MACHINE_UC1
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_LeftFoil)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;
                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }
#else
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_NonCoating_1)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;
                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }
#endif // INSPECTION_MACHINE_UC1

            
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            ResultLaneData resLaneUp;
            ResultSpec resSpec;
#if INSPECTION_MACHINE_UC1
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_LeftCoating)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;

                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }

#else
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_Coating_1)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;

                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }
#endif // INSPECTION_MACHINE_UC1

            
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            ResultLaneData resLaneUp;
            ResultSpec resSpec;

#if INSPECTION_MACHINE_UC1
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_RightCoating)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;
                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }


#else
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_Coating_2)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;
                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }
#endif // INSPECTION_MACHINE_UC1

            
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            ResultLaneData resLaneUp;
            ResultSpec resSpec;

#if INSPECTION_MACHINE_UC1
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_RightFoil)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;
                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }
#else
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_NonCoating_2)
            {
                resLaneUp.fLaneWidth = (float)vecMeasureDefectUp[nItemIdx].LENGTH;
                resvecLaneUp.push_back(resLaneUp);

                resSpec.fMeasureStandard = vecMeasureDefectUp[nItemIdx].m_fMeasureStandard;

                resSpec.fMeasureOffsetUCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUCL;
                resSpec.fMeasureOffsetLCL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLCL;

                resSpec.fMeasureOffsetUSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetUSL;
                resSpec.fMeasureOffsetLSL = vecMeasureDefectUp[nItemIdx].m_fMeasureOffsetLSL;

                resvecLaneSpec.push_back(resSpec);
            }
#endif // INSPECTION_MACHINE_UC1

                    
        }

        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefectUp[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefectUp[nItemIdx].LENGTH;
            }
        }
    }
    ResultMismatch stRes;
    stRes.m_strLotID = _T("test1234");
    stRes.m_vecResUpLane = resvecLaneUp;
    stRes.m_vecResDownLane = resvecLaneUp;
    stRes.m_vecResSpec = resvecLaneSpec;
    stRes.m_fTopFullLength = fFullLength;
    stRes.m_fBackFullLength = fFullLength;
    stRes.m_nCellIndex = unitUp->m_cellID;
    m_inlineResultCoupler->Push1(stRes);

    if (g_systemParameter.bUseServer == 1)
    {
        ResultMismatch stResAllMismatchView;
        CString strSend;
        stResAllMismatch.m_strLotID = _T("test1234");
        strSend.Format(_T("MISSMATCH,%s,"), stResAllMismatch.m_strLotID);
        strSend.AppendFormat(_T("%d,"), unitUp->m_cellID);
        strSend.AppendFormat(_T("%d,"), g_systemParameter.bDirection);
        strSend.AppendFormat(_T("%d,"), resvecLaneUp.size());

        for (int i = 0; i < resvecLaneUp.size(); i++)
            strSend.AppendFormat(_T("%f,"), resvecLaneUp[i].fLaneWidth);

        strSend.AppendFormat(_T("%f,"), fFullLength);

        if (g_systemParameter.bServer == 0) //client
        {
            OnSend2(strSend);
            m_loggerResult.Log(0, strSend);
        }
        else if (g_systemParameter.bServer == 1) //server
        {
            OnSend2_S(strSend);

            m_loggerResult.Log(0, strSend);
        }
    }

    Sleep(1); // Debug Point
}

CSVSaveThread* CIBCIDlg::GetCSVSaveThread()
{
    if (m_csvSaveThread == nullptr)
    {
        m_csvSaveThread = new CSVSaveThread;
    }

    return m_csvSaveThread;
}

void CIBCIDlg::OnBnClickedButtonFnAnimation()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_pDlgAnimationWindow == nullptr)
    {
        m_pDlgAnimationWindow = new CDlgAnimationWindow();
        m_pDlgAnimationWindow->Create(IDD_DIALOG_ANIMATION_WINDOW, this);
        m_pDlgAnimationWindow->CenterWindow(CWnd::GetDesktopWindow());
    }
    m_pDlgAnimationWindow->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButtonNormalSpecView()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgNormalSpecView == nullptr)
    {
        m_dlgNormalSpecView = new CDlgNormalSpecView;
        m_dlgNormalSpecView->Create(IDD_DIALOG_NORMAL_SPEC_VIEW, this);
        m_dlgNormalSpecView->CenterWindow(CWnd::GetDesktopWindow());
    }
    m_dlgNormalSpecView->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButtonSmallSpecView()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgSmallSpecView == nullptr)
    {
        m_dlgSmallSpecView = new CDlgSmallSpecView;
        m_dlgSmallSpecView->Create(IDD_DIALOG_SMALL_SPEC_VIEW, this);
        m_dlgSmallSpecView->CenterWindow(CWnd::GetDesktopWindow());
    }
    m_dlgSmallSpecView->ShowWindow(SW_SHOW);
}

void CIBCIDlg::OnBnClickedButtonFnCameraSetting()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgCameraSetting == nullptr)
    {
        m_dlgCameraSetting = new CDlgViewCameraSetting(m_loggerDebug, m_loggerDebug, this);
        m_dlgCameraSetting->Create(IDD_DIALOG_VIEW_CAMERA_SETTING, this);
        m_dlgCameraSetting->CenterWindow(CWnd::GetDesktopWindow());
        m_dlgCameraSetting->SetOfflineEditMode();
        m_dlgCameraSetting->Show(true);
    }
    m_dlgCameraSetting->ShowWindow(SW_SHOW);
}

void CIBCIDlg::cropImageSave(
    CString strPath, CString LotID, long cellID, IPVM::Image_8u_C1 cropImage_Bright, IPVM::Image_8u_C1 cropImage_Drak)
{
    CString strFileName(_T(""));
    CString strID(_T(""));

    strID.Format(_T("%d"), LotID);
    strFileName += strID;
    strID.Format(_T("%d"), cellID);
    strFileName += strID;

    CTime time = CTime::GetCurrentTime();
    CString strDirectTime = time.Format(_T("%Y%m%d%H%M%S"));
    CString strDirectPath;
    strDirectPath.Format(_T("d:\\%s"), strDirectTime);

    if (GetFileAttributes((LPCTSTR)strDirectPath) == INVALID_FILE_ATTRIBUTES)
    {
        CreateDirectory(strDirectPath, nullptr);
    }

    CString imagePath;
    imagePath.Format(_T("%s\\%s.jpg"), strDirectPath, strFileName);

    if (!imagePath.IsEmpty())
    {
        cropImage_Bright.SaveJpeg(imagePath);
        cropImage_Drak.SaveJpeg(imagePath);
    }
}

void CIBCIDlg::OnBnClickedStatusMaster()
{
    if (m_pClientSocket->IsConnected())
    {
        m_pStaticStatusMaster->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
        AlarmCommSocket* pClient = new AlarmCommSocket(m_loggerTcp, m_loggerAlarm, m_pClientSocket->m_initInfo);
        pClient->SetHwnd(this->GetSafeHwnd());
        swap(pClient, m_pClientSocket);
        delete pClient;

        return;
    }
    if (m_pClientSocket->Connect(g_systemParameter.GetConnectIP(), g_systemParameter.nConnectPort, 1, true))
    {
        m_pStaticStatusMaster->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_ON2);
    }
    else
    {
        m_pStaticStatusMaster->LoadImageFromResource(_T("JPG"), IDR_JPG_STATUS_OFF2);
    }
}

void CIBCIDlg::OnBnClickedStatusVision()
{
    if (g_systemParameter.bServer == 0)
    {
        if (g_systemParameter.bServer == 0)
        {
            if (m_pClientSock_C)
                delete m_pClientSock_C;
            // 클라이언트라면 oninit
            m_pClientSock_C = new CClientSock();
            m_pClientSock_C->Create();
            m_pClientSock_C->Connect(g_systemParameter.strHostIP, g_systemParameter.nPort);
        }
    }


}

void CIBCIDlg::OnStnClickedStaticStartInsp()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
   
}

void CIBCIDlg::OnBnClickedButtonInitGraph()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int nGraphCnt;
    if (g_systemParameter.nLaneCount == 1)
        nGraphCnt = 3;
    else if (g_systemParameter.nLaneCount == 2)
        nGraphCnt = 4;

    for (int n = 0; n < nGraphCnt; n++)
    {
        m_pDlgGraphWindow->GetGraphFrame(n)->DataSetZero();
    }
    m_dlgDefectmap->DataCountSetZero();
    m_dlgDefectCount->SetZero();
    m_nDefectMapCount = 0;
    CString filePath;
    CFileFind find;
    //filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\DefectCountData.csv");
    filePath.Format(_T("%s\\DefectCountData.csv"), Path::GetBackupUIFolder());
    if (find.FindFile(filePath))
        DeleteFile(filePath);
    //filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\MismatchGraphData.csv");
    filePath.Format(_T("%s\\MismatchGraphData.csv"), Path::GetBackupUIFolder());
    if (find.FindFile(filePath))
        DeleteFile(filePath);
    //filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\DefectMapData.csv");
    filePath.Format(_T("%s\\DefectMapData_Top.csv"), Path::GetBackupUIFolder());
    if (find.FindFile(filePath))
        DeleteFile(filePath);
    filePath.Format(_T("%s\\DefectMapData_Back.csv"), Path::GetBackupUIFolder());
    if (find.FindFile(filePath))
        DeleteFile(filePath);
    CInspectionCell* unitUp = Inspection::Accessor::GetRoot().GetInspectionCell(true);
    long nSizeDivideVertical = (long)unitUp->m_ISICellAllDefect.m_vvMeasureDefect.size();
    //m_vecMismatchData2
}

void CIBCIDlg::OnBnClickedButtonCalcResolution()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDlgCalculationResolution dlg;
    if (dlg.DoModal() == IDOK)
    {
    }
}

void CIBCIDlg::Illum_On()
{
    if(g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
    {
        for (int nCh = 0; nCh <4; nCh++)
        {
            ContinueBrightOnOff(TRUE, nCh, 100);
        }
    }
    else
    {
        BOOL bIllum = m_pSyncIO->GetOutputBit(SyncboardOutput::Illumination);
        //m_bLight = bIllum; // 0
        //bIllum = !bIllum;
        bIllum = TRUE;
        m_pSyncIO->SetSignal_HandlerOutput(SyncboardOutput::Illumination, bIllum);
    }
}

void CIBCIDlg::Illum_Off()
{
    if (g_systemParameter.machineType == MachineType::ES_rollpress_pinhole)
    {
        for (int nCh = 0; nCh < 4; nCh++)
        {
            ContinueBrightOnOff(FALSE, nCh, 100);
        }
    }
    else
    {
        BOOL bIllum = m_pSyncIO->GetOutputBit(SyncboardOutput::Illumination);
        //m_bLight = bIllum; // 1
        //bIllum = !bIllum;
        bIllum = FALSE;
        m_pSyncIO->SetSignal_HandlerOutput(SyncboardOutput::Illumination, bIllum);
    }
   

}

BOOL CIBCIDlg::ContinueBrightOnOff(BOOL bOn, long nCh, long nValue)
{
    char chTxMsg[7];

    chTxMsg[0] = 0x4C; //header1
    chTxMsg[1] = 0x12; //command

    chTxMsg[2] = (char)nCh; //channel
    //chTxMsg[2] = 0x01; //channel

    chTxMsg[3] = (char)nValue; //Bright Value
    //chTxMsg[3] = 0x50; //Bright Value

    long nCheckSum = chTxMsg[1] ^ chTxMsg[2] ^ chTxMsg[3];
    chTxMsg[4] = (BYTE)nCheckSum;

    chTxMsg[5] = 0x0D;
    chTxMsg[6] = 0x0A;

    int nSendLen = send(m_sockClient, chTxMsg, sizeof(chTxMsg), 0);

    CString strSendMessage;
    strSendMessage.Format(_T("%02X%02X%02X%02X%02X%02X%02X"), chTxMsg[0], chTxMsg[1], chTxMsg[2], chTxMsg[3],
        chTxMsg[4], chTxMsg[5], chTxMsg[6]);

    CString strRecvMessage;

    if (nSendLen > 0)
    {
        char chRxMsg[8];
        //char chcpyRxMsg[100];

        memset(chRxMsg, 0, sizeof(chRxMsg));
        //int nReceiveLen =
        int nRecvLen = recv(m_sockClient, chRxMsg, sizeof(chRxMsg), 0);

        if (nRecvLen > 0)
        {
            strRecvMessage.Format(_T("%02X%02X%02X%02X%02X%02X%02X%02X"), chRxMsg[0], chRxMsg[1], chRxMsg[2],
                chRxMsg[3], chRxMsg[4], chRxMsg[5], chRxMsg[6], chRxMsg[7]);
        }
    }

    return TRUE;
}

BOOL CIBCIDlg::IllumConnect()
{
    const char* strIP = "192.168.10.10";
    long nPort = 1000;

    CString strLog;

    WSADATA wsaData;

    struct sockaddr_in addrSockSvr;

    char chIP[256];
    USHORT nPortNumber = (USHORT)nPort;

    if (INVALID_SOCKET == WSAStartup(MAKEWORD(2, 0), &wsaData))
    {
        return FALSE;
    }

    m_sockClient = socket(AF_INET, SOCK_STREAM, 0);

    addrSockSvr.sin_family = AF_INET;
    addrSockSvr.sin_port = htons(nPortNumber);
    //addrSockSvr.sin_addr.S_un.S_addr = 
    inet_pton(AF_INET, strIP, &(addrSockSvr.sin_addr));

    int a = connect(m_sockClient, (struct sockaddr*)&addrSockSvr, sizeof(addrSockSvr));
    if (SOCKET_ERROR == a)
    {
        return FALSE;
    }

    return TRUE;
}

void CIBCIDlg::SetCameraVerticalLength()
{
    // 라인스캔 카메라가 아니라면 리턴
    if (m_cisComm->GetExecutorType() != ExecutorType::Sapera)
    {
        return;
    }
    long nCamLine = 1234;
    m_cisComm->GetGrabExcutor(0)->SetVerticalLength(nCamLine);

    m_dlgCameraSetting->TriggerPeriodSet(nCamLine);
}

BOOL CIBCIDlg::StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue)
{
    long nDivideTotalCount = 0;
    CString strSub;
    while (FALSE != AfxExtractSubString(strSub, strFull, nDivideTotalCount++, chDivide))
    {
        vecstrValue.push_back(strSub);
    }
    return TRUE;
}

void CIBCIDlg::OnBnClickedButtonExportData()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ImportDefectCountFile();
    ImportMismatchGraphFile();
    ImportDefectMapFile();
}
void CIBCIDlg::ImportDefectCountFile()
{
    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeRead | CFile::typeText;

    //CString filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\DefectCountData.csv");
    CString filePath;
    filePath.Format(_T("%s\\DefectCountData.csv"), Path::GetBackupUIFolder());
    if (!file.Open(filePath, nCheck, &ex))
    {
        return;
    }

    setlocale(LC_ALL, "korean");

    std::vector<CString> vecStrSpec;
    CString line;

    while (file.ReadString(line))
    {
        vecStrSpec.push_back(line);
    }

    for (long n = 0; n < vecStrSpec.size(); n++)
    {
        std::vector<CString> vecDiv;
        StringTokenDivideToCString(vecStrSpec[n], ',', vecDiv);
        ImportDefectCountData(vecDiv[0], _ttoi(vecDiv[1]), _ttoi(vecDiv[2]), _ttoi(vecDiv[3]));
    }
    file.Close();
    m_dlgDefectCount->DefectDraw();
}

void CIBCIDlg::ImportMismatchGraphFile()
{
    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeRead | CFile::typeText;

    //CString filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\MismatchGraphData.csv");
    CString filePath;
    filePath.Format(_T("%s\\MismatchGraphData.csv"), Path::GetBackupUIFolder());

    if (!file.Open(filePath, nCheck, &ex))
    {
        return;
    }

    setlocale(LC_ALL, "korean");

    std::vector<CString> vecStrSpec;
    CString line;
    while (file.ReadString(line))
    {
        vecStrSpec.push_back(line);
    }
    
    std::vector<std::vector<float>> vecvecfWidth;
    std::vector<long> vecnName;


    vecvecfWidth.resize(vecStrSpec.size());
    for (long n = 0; n < vecStrSpec.size(); n++)
    {
        std::vector<CString> vecDiv;
        StringTokenDivideToCString(vecStrSpec[n], ',', vecDiv);
        vecnName.push_back(_ttoi(vecDiv[0]));
        
        for (long nSize = 0; nSize < vecDiv.size(); nSize++)
        {
            if (nSize != 0)
                vecvecfWidth[n].push_back(_ttof(vecDiv[nSize]));
        }
    }
    m_vecCellIndex = vecnName;
    m_vecvecDiffWidth = vecvecfWidth;
    ImportMismatchGraphData(vecnName, vecvecfWidth);
    file.Close();
}

void CIBCIDlg::ImportDefectMapFile()
{

    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeRead | CFile::typeText;

    //CString filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\DefectMapData.csv");
    CString filePath;
    filePath.Format(_T("%s\\DefectMapData.csv"), Path::GetBackupUIFolder());

    long nUp = !g_systemParameter.bDirection;
    if (!file.Open(filePath, nCheck, &ex))
    {

        return;
    }



    setlocale(LC_ALL, "korean");

    std::vector<CString> vecStrSpec;
    CString line;

    while (file.ReadString(line))
    {
        vecStrSpec.push_back(line);
        /*std::vector<CString> vecDiv;
        StringTokenDivideToCString(line, ',', vecDiv);
        ImportDefectMapData(nUp, n, _ttoi(vecDiv[0]), _ttoi(vecDiv[1]), _ttoi(vecDiv[2]));
        n++;*/
    }

    /*if (m_pDlgDefectMapMessage == nullptr)
    {
        m_pDlgDefectMapMessage = new CDlgDefectMapMessage;
        m_pDlgDefectMapMessage->Create(IDD_DLG_DEFECTMAP_MESSAGE, this);
    }
    m_pDlgDefectMapMessage->ShowWindow(SW_SHOW);*/
    file.Close();

    for (long n = 0; n < vecStrSpec.size(); n++)
    {
        std::vector<CString> vecDiv;
        StringTokenDivideToCString(vecStrSpec[n], ',', vecDiv);
        ImportDefectMapData(nUp, _ttoi(vecDiv[0]), _ttoi(vecDiv[1]), _ttoi(vecDiv[2]), _ttoi(vecDiv[3]));
    }   
}

void CIBCIDlg::ExportDefectCountData(std::vector<DefectCount> m_vecData)
{
    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeWrite;
    CString folderPath;
    //folderPath = _T("C:\\Intekplus\\IBCI\\backup_UI");
    folderPath = Path::GetBackupUIFolder();
    if (!find.FindFile(folderPath))
    {
        CreateDirectory(folderPath, NULL);
    }
    CString filePath;
    filePath = folderPath + _T("\\DefectCountData.csv");
    
    if (FALSE == find.FindFile(filePath))
        nCheck |= CFile::modeCreate;
    else
    {
        if (DeleteFile(filePath))
            nCheck |= CFile::modeCreate;
        else
            return ;
    }
    if (!file.Open(filePath, nCheck, &ex))
    {
        ex.ReportError();
        return ;
    }

    setlocale(LC_ALL, "korean");
    file.SeekToEnd();

    for (long n = 0; n < m_vecData.size(); n++)
    {
        file.WriteString(m_vecData[n].m_strName);
        CString str;
        str.Format(_T(",%d"), m_vecData[n].m_nTopNum);
        file.WriteString(str);
        str.Format(_T(",%d"), m_vecData[n].m_nBottomNum);
        file.WriteString(str);
        str.Format(_T(",%d"), m_vecData[n].m_nSumNum);
        file.WriteString(str);
        file.WriteString(_T("\n"));
    }
    file.Close();
}

void CIBCIDlg::ExportMismatchGraphData(long nCellIndex, std::vector<float> fDiffWidth)
{
    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeWrite;
    CString folderPath;
    //folderPath = _T("C:\\Intekplus\\IBCI\\backup_UI");
    folderPath = Path::GetBackupUIFolder();
    if (!find.FindFile(folderPath))
    {
        CreateDirectory(folderPath, NULL);
    }
    CString filePath;
    filePath = folderPath + _T("\\MismatchGraphData.csv");
    if (FALSE == find.FindFile(filePath))
        nCheck |= CFile::modeCreate;
    else
    {
        if (nCellIndex == 0)
        {
            if (DeleteFile(filePath))
                nCheck |= CFile::modeCreate;
        }
    }

    if (!file.Open(filePath, nCheck, &ex))
    {
        ex.ReportError();
        return;
    }

    setlocale(LC_ALL, "korean");
    file.SeekToEnd();
    CString str;

    str.Format(_T("%d"), nCellIndex);
    file.WriteString(str);
    
    for (long n = 0; n < fDiffWidth.size(); n++)
    {
        str.Format(_T(",%f"), fDiffWidth[n]);
        file.WriteString(str);
        
    }
    file.WriteString(_T("\n"));
    file.Close();

}

void CIBCIDlg::ExportDefectMapData(long nTopBack, long nCellIndex, DefectPointXY vecXY, long vecbPass, long nDefectCode,
    CString strBrightPath, CString strDarkPath)
{
    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeWrite;

    CString folderPath;
    folderPath = Path::GetBackupUIFolder();
    if (!find.FindFile(folderPath))
    {
        CreateDirectory(folderPath, NULL);
    }
    CString filePathTop, filePathBack;
    filePathTop = folderPath + _T("\\DefectMapData_Top.csv");
    filePathBack = folderPath + _T("\\DefectMapData_Back.csv");

    CString strTop, strBack;
    // 0 =BACK, 1 = TOP
    if (nTopBack == 1)
    {
        if (FALSE == find.FindFile(filePathTop))
            nCheck |= CFile::modeCreate;
        else
        {
            //if (m_nDefectMapCount == 0)
            //{
            //    if (DeleteFile(filePathTop))
            //        nCheck |= CFile::modeCreate;
            //}
        }
        if (!file.Open(filePathTop, nCheck, &ex))
        {
            ex.ReportError();
            return;
        }

        setlocale(LC_ALL, "korean");
        file.SeekToEnd();

        strTop.AppendFormat(_T("%d,"), nCellIndex);
        strTop.AppendFormat(_T("%d,"), vecXY.nX);
        strTop.AppendFormat(_T("%d,"), vecXY.nY);
        strTop.AppendFormat(_T("%d,"), vecbPass);
        strTop.AppendFormat(_T("%d,"), nDefectCode);
        strTop += strBrightPath + _T(",");
        strTop += strDarkPath + _T(",");
        file.WriteString(strTop + _T("\n"));
        file.Close();
    }

    if (nTopBack == 0)
    {
        if (FALSE == find.FindFile(filePathBack))
            nCheck |= CFile::modeCreate;
        else
        {
            //if (m_nDefectMapCount == 0)
            //{
            //    if (DeleteFile(filePathBack))
            //        nCheck |= CFile::modeCreate;
            //}
        }
        if (!file.Open(filePathBack, nCheck, &ex))
        {
            ex.ReportError();
            return;
        }

        setlocale(LC_ALL, "korean");
        file.SeekToEnd();

        strBack.AppendFormat(_T("%d,"), nCellIndex);
        strBack.AppendFormat(_T("%d,"), vecXY.nX);
        strBack.AppendFormat(_T("%d,"), vecXY.nY);
        strBack.AppendFormat(_T("%d,"), vecbPass);
        strBack.AppendFormat(_T("%d,"), nDefectCode);
        strBack += strBrightPath + _T(",");
        strBack += strDarkPath + _T(",");
        file.WriteString(strBack + _T("\n"));
        file.Close();
    }
    m_nDefectMapCount++;

    //CStdioFile file;
    //CFileFind find;
    //CFileException ex;
    //UINT nCheck = CFile::modeWrite;
    //CString folderPath;
    ////folderPath = _T("C:\\Intekplus\\IBCI\\backup_UI");
    //folderPath = Path::GetBackupUIFolder();
    //if (!find.FindFile(folderPath))
    //{
    //    CreateDirectory(folderPath, NULL);
    //}
    //CString filePath;
    //filePath = folderPath + _T("\\DefectMapData.csv");
    //if (FALSE == find.FindFile(filePath))
    //    nCheck |= CFile::modeCreate;
    //else
    //{
    //    if (m_nDefectMapCount == 0)
    //    {
    //        if (DeleteFile(filePath))
    //            nCheck |= CFile::modeCreate;
    //    }
    //}
    //if (!file.Open(filePath, nCheck, &ex))
    //{
    //    ex.ReportError();
    //    return;
    //}

    //setlocale(LC_ALL, "korean");
    //file.SeekToEnd();

    //CString str;
    //str.Format(_T("%d,"), nCellIndex);
    //file.WriteString(str);
    //str.Format(_T("%d,"), vecXY.nX);
    //file.WriteString(str);
    //str.Format(_T("%d,"), vecXY.nY);
    //file.WriteString(str);
    //str.Format(_T("%d"), vecbPass);
    //file.WriteString(str);
    //file.WriteString(_T("\n"));
    //file.Close();
    //m_nDefectMapCount++;
}

void CIBCIDlg::ImportDefectCountData(CString strDefectName, long nTopNum, long nBottomNum, long nSumNum)
{
    for (long nId = 0; nId < m_dlgDefectCount->m_vecData.size(); nId++)
    {
        if (m_dlgDefectCount->m_vecData[nId].m_strName == strDefectName)
        {
            m_dlgDefectCount->m_vecData[nId].m_strName = strDefectName;
            m_dlgDefectCount->m_vecData[nId].m_nTopNum = nTopNum;
            m_dlgDefectCount->m_vecData[nId].m_nBottomNum = nBottomNum;
            m_dlgDefectCount->m_vecData[nId].m_nSumNum = nSumNum;

            std::sort(m_dlgDefectCount->m_vecData.begin(), m_dlgDefectCount->m_vecData.end(), compare);
        }
    }
    //m_dlgDefectCount->DefectDraw();
}

void CIBCIDlg::ImportMismatchGraphData(std::vector<long> vecnCellIndex, std::vector<std::vector<float>> vecvecDiffWidth)
{
    for (long n = 0; n < vecnCellIndex.size(); n++)
    {
        long nCellIndex = vecnCellIndex[n];
        long nVecSize = vecvecDiffWidth[n].size();
        if (nVecSize == 2)
        {
            m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(
                TRUE, DefectType::Dimension_Mismatch1, nCellIndex, vecvecDiffWidth[n][0], true, _T("OK"));

            m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(
                TRUE, DefectType::Dimension_Mismatch2, nCellIndex, vecvecDiffWidth[n][1], true, _T("OK"));
        }
        if (nVecSize == 3)
        {
            m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(
                TRUE, DefectType::Dimension_Mismatch1, nCellIndex, vecvecDiffWidth[n][0], true, _T("OK"));

            m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(
                TRUE, DefectType::Dimension_Mismatch2, nCellIndex, vecvecDiffWidth[n][1], true, _T("OK"));
            m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(
                TRUE, DefectType::Dimension_Mismatch3, nCellIndex, vecvecDiffWidth[n][2], true, _T("OK"));
        }
        if (nVecSize == 4)
        {
            m_pDlgGraphWindow->m_pvecDlgGraphFrame[0]->SetData(
                TRUE, DefectType::Dimension_Mismatch1, nCellIndex, vecvecDiffWidth[n][0], true, _T("OK"));



            m_pDlgGraphWindow->m_pvecDlgGraphFrame[1]->SetData(
                TRUE, DefectType::Dimension_Mismatch2, nCellIndex, vecvecDiffWidth[n][1], true, _T("OK"));
            m_pDlgGraphWindow->m_pvecDlgGraphFrame[2]->SetData(
                TRUE, DefectType::Dimension_Mismatch3, nCellIndex, vecvecDiffWidth[n][2], true, _T("OK"));
            m_pDlgGraphWindow->m_pvecDlgGraphFrame[3]->SetData(
                TRUE, DefectType::Dimension_Mismatch4, nCellIndex, vecvecDiffWidth[n][3], true, _T("OK"));
        }
    }
}

void CIBCIDlg::ImportDefectMapData(long nTopBack, long nCellIndex, long X, long Y, long nPass)
{
    DefectPointXY ptDefect;
    ptDefect.nX = X;
    ptDefect.nY = Y;
    if (nPass == 0)
    {
        ptDefect.Color = RGB(0, 255, 0);
        m_dlgDefectmap->SetPoint(nTopBack, ptDefect, nCellIndex, m_cellImageMerger->GetHorizontalSize());
        //m_dlgDefectmap->m_vecISIAllDefectTotal.push_back(propertyView);
    }
    else
    {
        ptDefect.Color = RGB(255, 0, 0);
        long nHorSize = 0;
        if (m_cellImageMerger != nullptr)
        {
            nHorSize = m_cellImageMerger->GetHorizontalSize();
        }
        else
        {
            CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);
            auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
            nHorSize = itemCellAlign->m_inspectionImageBright.GetSizeX();
        }
        if (nHorSize == 0)
        {
            //nHorSize = 14288 * 2;
            nHorSize = m_cisComm->GetGrabExcutor(0)->GetHorizontalSize() * 2;
        }
        m_dlgDefectmap->SetPoint(nTopBack, ptDefect, nCellIndex, nHorSize);
        //m_dlgDefectmap->m_vecISIAllDefectTotal.push_back(propertyView);
    }
}

void CIBCIDlg::OnBnClickedButtonDataSearch()
{
    AccessorDataSearch::Get().GetSystemFilePath(g_systemParameter.pathResultDataCsv);
    AccessorDataSearch::Get().ShowDlg();
}

BOOL CIBCIDlg::ALTIllumControllerInit()
{
    CString strLog; 
    m_NetIndex = ALT_LSTPE_Create();

    //랜을 사용하도록 정보 설정
    if (!ALT_LSTPE_Lan_Start(m_NetIndex, TRUE, "192.168.10.10", 1000, MAX_CHANNEL_4))
    {
        strLog.Format(_T("[Line ILLUM] ALT_LSPE_Lan_Start 1 Error! : ALT-LSTPE"));
        return FALSE;
    }
    //설정값을 통해 접속
    if (!ALT_LSTPE_IsTcpConnect(m_NetIndex))
    {
        strLog.Format(_T("[Line ILLUM] ALT_LSTPE_IsTcpConnect Error! : ALT-LSTPE"));
        return FALSE;
    }
    //채널의Led의 Strobe On Time 설정
    //unsStrobValue = us 의 온타임값
    if (!ALT_LSTPE_StrobeValueSend(m_NetIndex, 0, 0, 4000))
    {
        strLog.Format(_T("[Line ILLUM] 1st ALT_LSTPE_AllStrobeValueSend Error! : ALT-LSTPE"));
        //m_pMainView->AddLog(strLog);
        return FALSE;
    }

}
void CIBCIDlg::SaveCsvMeasureNotMsa(
    long nSizeDivideVertical, std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect, CString strLotID, long nCellID)
{
    SYSTEMTIME tmModify;
    ::GetLocalTime(&tmModify);
    CString strTime;
    strTime.Format(_T("%04d%02d%02d"), tmModify.wYear, tmModify.wMonth, tmModify.wDay);

    CString strDirectPath;
    strDirectPath.Format(_T("d:\\Measure_Save\\%s\\%d"), strTime, nCellID);
    IPVM::CreateDirectories(strDirectPath);

    tmModify.wHour;

    CString strMsaTitle;
    CString strMsaText;
    CString strMsaResult;
    CString strCellID;
    strCellID.Format(_T("_%d"), nCellID);

    // CString strResult = strDirectPath + _T("\\Measure_Data") + strCellID + (".csv");
    float fLeftFoil = 0.;
    float fRightFoil = 0.;
    float fMiddleFoil = 0.;
    float fLeftCoating = 0.;
    float fRightCoating = 0.;
    float fFullLength = 0.;
    CString strResult;

    for (int nVerticalIdx = 0; nVerticalIdx < nSizeDivideVertical; nVerticalIdx++)
    {
        auto& vecMeasureDefect = m_vvMeasureDefect[nVerticalIdx];

        long nSizeMeasureItem = (long)vecMeasureDefect.size();
#if INSPECTION_MACHINE_UC1
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftFoil)
            {
                fLeftFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_LeftCoating)
            {
                fLeftCoating = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightCoating)
            {
                fRightCoating = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_RightFoil)
            {
                fRightFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_MiddleFoil)
            {
                fMiddleFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#else
        for (int nItemIdx = 0; nItemIdx < nSizeMeasureItem; nItemIdx++)
        {
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_1)
            {
                fLeftFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_1)
            {
                fLeftCoating = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_Coating_2)
            {
                fRightCoating = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_NonCoating_2)
            {
                fRightFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_MiddleFoil)
            {
                fMiddleFoil = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }

            if (vecMeasureDefect[nItemIdx].m_defectType == DefectType::Width_FullLength)
            {
                fFullLength = (float)vecMeasureDefect[nItemIdx].LENGTH;
            }
        }
#endif // INSPECTION_MACHINE_UC1
        
    }

    strResult.Format(_T("CellID = %d,LF = %.4f,LC = %.4f,MF = %.4f,RC = %.4f,RF = %.4f,FL = %.4f"), nCellID, fLeftFoil,
        fLeftCoating, fMiddleFoil, fRightCoating, fRightFoil, fFullLength);
    m_loggerCellResult.Log(0, strResult);
    // GetCSVSaveThread()->Push(strResult, strMsaTitle, strMsaResult, true);
}

void CIBCIDlg::UdateDefectROIViewClear()
{
    if (m_dlgDefectmap->m_pDlgDefectTimerView != nullptr)
    {
        m_dlgDefectmap->m_pDlgDefectTimerView->ROIClear();
    }
    
}

void CIBCIDlg::OnBnClickedButtonHWControl()
{
    DlgHWControl dlg;
    dlg.DoModal();
}

BOOL CIBCIDlg::PreTranslateMessage(MSG* pMsg)
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

void CIBCIDlg::SendHostResultDataInfo(Share::ResultVision& ResultData, CInspectionCell* itemCellAlign)
{
    Share::ResultVision ResultInfo = ResultData;
    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
    m_loggerTcp.Log(0, _T("ResultVision Make Start"));
    //ResultVision Information Setting
    //auto* InsepctionInputData = (InspectionInputData*)itemCellAlign;

    CInspectionCell* unit = itemCellAlign;

    ResultInfo.m_nCellindex = unit->m_cellID;
    ResultInfo.m_nTotalLength = unit->m_imageSizeY * g_systemParameter.m_dPixelToUmY * 1000;
    ResultInfo.m_systmVision_Input = unit->m_inputTime;
    ResultInfo.m_strRecipe_ID = g_systemParameter.m_currentModel;
    ResultInfo.m_dInspection_Processing_Time
        = unit->m_grabTime_ms + unit->m_ISICellAllDefect.m_dInspTotalTime_ms; //? 어떻게 가져오지?

    if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
    {
        ResultInfo.m_strCell_Final_Judge = _T("NG");
    }
    else if (unit->m_resultValue == II_RESULT_VALUE::PASS)
    {
        ResultInfo.m_strCell_Final_Judge = _T("OK");
    }
    else if (unit->m_resultValue == II_RESULT_VALUE::INVALID)
    {
        ResultInfo.m_strCell_Final_Judge = _T("INVALID");
    }
    ResultInfo.m_nTotal_Dimension_NG_Count = 0; // 이거 무엇인지 모름
    ResultInfo.m_nTotal_Missmatch_NG_Count = 0;
    ResultInfo.m_nTotal_Surface_NG_Count = unit->m_ISICellAllDefect.m_vecCandidate.size();
    ResultInfo.m_nTotal_Insulation_NG_Count = 0;
    ResultInfo.m_nLane_Count = g_systemParameter.nLaneCount;

    m_loggerTcp.Log(0, _T("ResultVision Surfave Name Input"));

    // 정태양 책임 요청 사항 현재 검사 이미지의 불량 항목의 이름만 들어가야함.
    // 아직 명확히 LG 와 협의되지 않는 부분이라 일단 주석 처리로 진행
    //    for (long nDefect = 0; nDefect < unit->m_ISICellAllDefect.m_vecCandidate.size(); nDefect++)
    //    {
    //        ResultInfo.m_vecSurface_Reason_All.push_back((DefectType)unit->m_ISICellAllDefect.m_vecISIDefectType[nDefect]);
    //    }
    //    auto* itemCellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);
    //    for (long nMeasure = 0; nMeasure < itemCellMeasure->m_para.m_nCountMeasureItem; nMeasure++)
    //    {
    //#if INSPECTION_MACHINE_UC1
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftFoil)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightFoil)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftCoating)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightCoating)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //#else
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_1)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
    //        {
    //           ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_2)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_1)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_2)
    //        {
    //           ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //#endif // INSPECTION_MACHINE_UC1
    //    }
    //.. 표면 불량 이름 넣기
    for (int nSurface = (int)DefectType::START_Coater_Surface + 1; nSurface < (int)DefectType::END_Coater_Surface;
         nSurface++)
    {
        //std::vector<CISISpecInfo>* spec = unit->m_isiSpecTable->GetSpecInfo_Multi(nSurface);

        CString str = GetDefectName((DefectType)nSurface);
        ResultInfo.m_vecSurface_Reason_All.push_back((DefectType)nSurface);
    }
    m_loggerTcp.Log(0, _T("ResultVision Dimesnsion Name Input"));

    //.. 치수 불량 이름 넣기
    for (int nDimension = (int)DefectType::START_Coater_Dimension + 1;
         nDimension < (int)DefectType::End_Coater_Dimension; nDimension++)
    {
        CString str = GetDefectName((DefectType)nDimension);
        ResultInfo.m_vecDimension_Reason_All.push_back((DefectType)nDimension);
    }

    // -- (1) CameraInfo
    SendHostCameraInfoData(ResultInfo, unit);

    // -- (2) Dimension
    SendHostDimensionData(ResultInfo, unit);

    // -- (3) Mismatch
    SendHostMismatchData(ResultInfo, unit);

    // -- (4) Defect
    SendHostDefectData(ResultInfo, unit);

    // -- (5) Original Image
    SendHostOriginalImageData(ResultInfo, unit);

    m_loggerTcp.Log(0, _T("ResultVision Make End"));
    ResultInfo.m_bNGTag = FALSE;

    long n = sizeof(ResultInfo);
    ResultData = ResultInfo;

    //m_tcpResultSender->Push(ResultInfo);

    //if (topBack == _T("BACK"))
    //{
    //    SendHostData(ResultInfo);
    //}
}

void CIBCIDlg::SendHostResultDataInfo(Share::ResultVision& ResultData, std::vector<IPVM::Image_8u_C1>& vecBrightImg,
    std::vector<IPVM::Image_8u_C1>& vecDarkImg, CInspectionCell* itemCellAlign)
{
    Share::ResultVision ResultInfo = ResultData;
    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
    m_loggerTcp.Log(0, _T("ResultVision Make Start"));
    //ResultVision Information Setting
    //auto* InsepctionInputData = (InspectionInputData*)itemCellAlign;

    CInspectionCell* unit = itemCellAlign;

    ResultInfo.m_nCellindex = unit->m_cellID;
    ResultInfo.m_nTotalLength = unit->m_imageSizeY * g_systemParameter.m_dPixelToUmY * 1000;
    ResultInfo.m_systmVision_Input = unit->m_inputTime;
    ResultInfo.m_strRecipe_ID = g_systemParameter.m_currentModel;
    ResultInfo.m_dInspection_Processing_Time
        = unit->m_grabTime_ms + unit->m_ISICellAllDefect.m_dInspTotalTime_ms; //? 어떻게 가져오지?

    if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
    {
        ResultInfo.m_strCell_Final_Judge = _T("NG");
    }
    else if (unit->m_resultValue == II_RESULT_VALUE::PASS)
    {
        ResultInfo.m_strCell_Final_Judge = _T("OK");
    }
    else if (unit->m_resultValue == II_RESULT_VALUE::INVALID)
    {
        ResultInfo.m_strCell_Final_Judge = _T("INVALID");
    }
    ResultInfo.m_nTotal_Dimension_NG_Count = 0; // 이거 무엇인지 모름
    ResultInfo.m_nTotal_Missmatch_NG_Count = 0;
    ResultInfo.m_nTotal_Surface_NG_Count = unit->m_ISICellAllDefect.m_vecCandidate.size();
    ResultInfo.m_nTotal_Insulation_NG_Count = 0;
    ResultInfo.m_nLane_Count = g_systemParameter.nLaneCount;

    m_loggerTcp.Log(0, _T("ResultVision Surfave Name Input"));

    // 정태양 책임 요청 사항 현재 검사 이미지의 불량 항목의 이름만 들어가야함.
    // 아직 명확히 LG 와 협의되지 않는 부분이라 일단 주석 처리로 진행
    //    for (long nDefect = 0; nDefect < unit->m_ISICellAllDefect.m_vecCandidate.size(); nDefect++)
    //    {
    //        ResultInfo.m_vecSurface_Reason_All.push_back((DefectType)unit->m_ISICellAllDefect.m_vecISIDefectType[nDefect]);
    //    }
    //    auto* itemCellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);
    //    for (long nMeasure = 0; nMeasure < itemCellMeasure->m_para.m_nCountMeasureItem; nMeasure++)
    //    {
    //#if INSPECTION_MACHINE_UC1
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftFoil)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightFoil)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftCoating)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightCoating)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //#else
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_1)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
    //        {
    //           ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_2)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_1)
    //        {
    //            ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //        if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_2)
    //        {
    //           ResultInfo.m_vecSurface_Reason_All.push_back(unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType);
    //        }
    //#endif // INSPECTION_MACHINE_UC1
    //    }
    //.. 표면 불량 이름 넣기
    for (int nSurface = (int)DefectType::START_Coater_Surface + 1; nSurface < (int)DefectType::END_Coater_Surface;
         nSurface++)
    {
        //std::vector<CISISpecInfo>* spec = unit->m_isiSpecTable->GetSpecInfo_Multi(nSurface);

        CString str = GetDefectName((DefectType)nSurface);
        ResultInfo.m_vecSurface_Reason_All.push_back((DefectType)nSurface);
    }
    m_loggerTcp.Log(0, _T("ResultVision Dimesnsion Name Input"));

    //.. 치수 불량 이름 넣기
    for (int nDimension = (int)DefectType::START_Coater_Dimension + 1;
         nDimension < (int)DefectType::End_Coater_Dimension; nDimension++)
    {
        CString str = GetDefectName((DefectType)nDimension);
        ResultInfo.m_vecDimension_Reason_All.push_back((DefectType)nDimension);
    }

    // -- (1) CameraInfo
    SendHostCameraInfoData(ResultInfo, unit);

    // -- (2) Dimension
    SendHostDimensionData(ResultInfo, unit);

    // -- (3) Mismatch
    SendHostMismatchData(ResultInfo, unit);

    // -- (4) Defect
    SendHostDefectData(ResultInfo, vecBrightImg, vecDarkImg, unit);

    // -- (5) Original Image
    SendHostOriginalImageData(ResultInfo, unit);

    m_loggerTcp.Log(0, _T("ResultVision Make End"));
    ResultInfo.m_bNGTag = FALSE;

    long n = sizeof(ResultInfo);
    ResultData = ResultInfo;

    //m_tcpResultSender->Push(ResultInfo);

    //if (topBack == _T("BACK"))
    //{
    //    SendHostData(ResultInfo);
    //}
}


// 기존코드
//void CIBCIDlg::SendHostResultDataInfo(Share::ResultVision& ResultData, CInspectionCell* itemCellAlign)
//{
//    Share::ResultVision ResultInfo = ResultData;
//    CString topBack;
//    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
//    m_loggerTcp.Log(0, _T("ResultVision Make Start"));
//    //ResultVision Information Setting
//    //auto* InsepctionInputData = (InspectionInputData*)itemCellAlign;
//
//    CInspectionCell* unit = itemCellAlign;
//
//    ResultInfo.m_nCellindex = unit->m_cellID;
//    ResultInfo.m_nTotalLength = unit->m_imageSizeY * g_systemParameter.m_dPixelToUmY * 1000;
//    ResultInfo.m_systmVision_Input = unit->m_inputTime;
//    ResultInfo.m_strRecipe_ID = g_systemParameter.m_currentModel;
//    ResultInfo.m_dInspection_Processing_Time
//        = unit->m_grabTime_ms + unit->m_inspectionStartTime_ms; //? 어떻게 가져오지?
//
//    if (unit->m_resultValue == II_RESULT_VALUE::REJECT)
//    {
//        ResultInfo.m_strCell_Final_Judge = _T("NG");
//    }
//    else if (unit->m_resultValue == II_RESULT_VALUE::PASS)
//    {
//        ResultInfo.m_strCell_Final_Judge = _T("OK");
//    }
//    else if (unit->m_resultValue == II_RESULT_VALUE::INVALID)
//    {
//        ResultInfo.m_strCell_Final_Judge = _T("INVALID");
//    }
//    ResultInfo.m_nTotal_Dimension_NG_Count = 0; // 이거 무엇인지 모름
//    ResultInfo.m_nTotal_Missmatch_NG_Count = 0;
//    ResultInfo.m_nTotal_Surface_NG_Count = unit->m_ISICellAllDefect.m_vecCandidate.size();
//    ResultInfo.m_nTotal_Insulation_NG_Count = 0;
//    ResultInfo.m_nLane_Count = g_systemParameter.nLaneCount;
//
//    m_loggerTcp.Log(0, _T("ResultVision Surfave Name Input"));
//
//    //.. 표면 불량 이름 넣기
//    for (int nSurface = (int)DefectType::START_Coater_Surface + 1; nSurface < (int)DefectType::END_Coater_Surface;
//         nSurface++)
//    {
//        //std::vector<CISISpecInfo>* spec = unit->m_isiSpecTable->GetSpecInfo_Multi(nSurface);
//
//        CString str = GetDefectName((DefectType)nSurface);
//        ResultInfo.m_vecSurface_Reason_All.push_back((DefectType)nSurface);
//    }
//    m_loggerTcp.Log(0, _T("ResultVision Dimesnsion Name Input"));
//    //.. 치수 불량 이름 넣기
//    for (int nDimension = (int)DefectType::START_Coater_Dimension + 1;
//         nDimension < (int)DefectType::End_Coater_Dimension; nDimension++)
//    {
//        CString str = GetDefectName((DefectType)nDimension);
//        ResultInfo.m_vecDimension_Reason_All.push_back((DefectType)nDimension);
//    }
//
//    // -- (1) CameraInfo
//    //
//    // REQEUST_JSONDATA 해당 Data 를 받았으면 vecCamra 에 Data 넣어주기
//    if (m_bReceiveRequestData == TRUE)
//    {
//        //auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
//        Share::CameraInfo cameraInfo;
//
//        for (long nCam = 0; nCam < m_cisComm->GetCameraCount(); nCam++)
//        {
//            if (topBack == _T("TOP"))
//            {
//                cameraInfo.m_nCamera_Location = 0; // TOP
//            }
//            else
//            {
//                cameraInfo.m_nCamera_Location = 1; //BACK
//            }
//            cameraInfo.m_nCamera_Number = 1;
//            cameraInfo.m_dResolution_X = g_systemParameter.m_dPixelToUmXUp;
//            cameraInfo.m_dResolution_Y = g_systemParameter.m_dPixelToUmY;
//            cameraInfo.m_nCamera_Gain = m_cisComm->GetGrabExcutor(nCam)->GetSystemGain();
//            double dExposure;
//            m_cisComm->GetGrabExcutor(nCam)->GetExposureTime(dExposure, dExposure);
//            cameraInfo.m_nExposure_Time = 0.0f;
//            cameraInfo.m_fAngle = 0.0f;
//
//            Share::ImageInfo ImageInfo;
//            //align->image
//            cameraInfo.m_vecImageQuality.clear();
//            auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
//            if (nCam == 0)
//            {
//                for (long nEdge = 0; nEdge < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size(); nEdge++)
//                {
//                    IPVM::Rect rtResult;
//                    rtResult = itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge];
//
//                    IPVM::Rect rtCropROI;
//                    rtCropROI.m_left = 0;
//                    rtCropROI.m_top = 0;
//                    rtCropROI.m_bottom = rtResult.Height();
//                    rtCropROI.m_right = rtResult.Width();
//
//                    IPVM::Image_8u_C1 CropImage;
//                    CropImage.Create(rtResult.Width(),
//                        rtResult
//                            .Height()); // 좀 더 크게 보여 주기 위해 어떻게 진행이 되어야 할까? off set 값을 어떻게 계산하여 넣어줄까?
//                    IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageDark, rtResult, rtCropROI, CropImage);
//                    ImageInfo.m_CropImage_LaneEdge = CropImage;
//                    ImageInfo.nEdgeNum = nEdge + 1;
//                    ImageInfo.nLaneNum = 2;
//                    ImageInfo.nSizeX = rtResult.Width();
//                    ImageInfo.nSizeY = rtResult.Height();
//                    ImageInfo.nFocus = 0;
//                    //ImageInfo.nGrayLevelValue[0] = 10; // 이거 해줘야 하나?
//                    //ImageInfo.nGrayLevelValue[1] = 11;
//                    ImageInfo.nGrayLevelValue_Bright = 10; // 이거 해줘야 하나?
//                    ImageInfo.nGrayLevelValue_Dark = 11;
//                    cameraInfo.m_vecImageQuality.push_back(ImageInfo);
//                }
//            }
//
//            ResultInfo.m_vecCamera.push_back(cameraInfo);
//        }
//
//        m_bReceiveRequestData = FALSE;
//    }
//
//    // -- (2) Dimension
//
//    auto* itemCellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);
//    itemCellMeasure->m_rejectInfo;
//    m_loggerTcp.Log(0, _T("ResultVision Dimension Defect Input"));
//
//    if (unit->m_ISICellAllDefect.m_vvMeasureDefect.size() == 0)
//    {
//    }
//    for (long nMeasure = 0; nMeasure < itemCellMeasure->m_para.m_nCountMeasureItem; nMeasure++)
//    {
//        Share::ResultDimension ResultDimesnsion;
//
//        if (topBack == _T("TOP"))
//        {
//            ResultDimesnsion.m_nCamera_Location = 0; // TOP
//        }
//        else
//        {
//            ResultDimesnsion.m_nCamera_Location = 1; //BACK
//        }
//        ResultDimesnsion.m_nCamera_Number = 1; // 우리는 2개의 카메라 영상획득한걸 하나로 합치니까 1개?
//        ResultDimesnsion.m_nScreen_Number = 2; // 우리 1개 Camera 에서 2장 Image 생성 하니까
//
//        if (unit->m_ISICellAllDefect.m_vvMeasureDefect.size() == 0)
//        {
//            ResultDimesnsion.m_strPosition_InspItem = _T("INVALID");
//            ResultDimesnsion.m_strPosition_Sub = _T("INVALID");
//            ResultDimesnsion.m_dValue = 0;
//            ResultDimesnsion.m_dLSL = 0;
//            ResultDimesnsion.m_dUSL = 0;
//            ResultDimesnsion.m_dGL = 0.0f; // G
//            ResultDimesnsion.m_dStart_Point_X = 0;
//            ResultDimesnsion.m_dEnd_Point_X = 0;
//            ResultDimesnsion.m_dStart_Point_Y = 0;
//            ResultDimesnsion.m_dEnd_Point_Y = 0;
//        }
//        else
//        {
//#if INSPECTION_MACHINE_UC1
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftFoil)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT3");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightFoil)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT4");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftCoating)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightCoating)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
//            }
//#else
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_1)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT3");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_2)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT4");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_1)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
//            }
//            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_2)
//            {
//                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
//                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
//            }
//#endif // INSPECTION_MACHINE_UC1
//
//            //ResultDimesnsion.m_strPosition_Sub = strSub; // 무엇이 들어가야 하는지 모르겠음
//            ResultDimesnsion.m_inspType = (DefectType)((long)DefectType::START_Coater_Dimension + 1
//                + itemCellMeasure->m_para.m_vecMeasureType[nMeasure]);
//
//            ResultDimesnsion.m_dValue = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].LENGTH;
//            ResultDimesnsion.m_dLSL = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureOffsetLSL;
//            ResultDimesnsion.m_dUSL = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureOffsetUSL;
//
//            ResultDimesnsion.m_strJudge = itemCellMeasure->GetInspResultString();
//
//            if (ResultDimesnsion.m_strJudge.Left(4) == _T("Pass"))
//            {
//                ResultDimesnsion.m_strJudge = _T("OK");
//            }
//            else
//            {
//                ResultDimesnsion.m_strJudge = _T("NG");
//            }
//            ResultDimesnsion.m_dGL = 0.0f; // Gray Value 값 추가 해서넣어야함.
//            ResultDimesnsion.m_dStart_Point_X
//                = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureStartPosX;
//            ResultDimesnsion.m_dEnd_Point_X = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureEndPosX;
//            ResultDimesnsion.m_dStart_Point_Y = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_top;
//            ResultDimesnsion.m_dEnd_Point_Y = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_bottom;
//
//            if (ResultDimesnsion.m_dStart_Point_X > unit->m_imageSizeX /* imageBright.GetSizeX() */ / 2)
//            {
//                ResultDimesnsion.m_bDefect_Left = FALSE;
//            }
//            else
//            {
//                ResultDimesnsion.m_bDefect_Left = TRUE;
//            }
//            Share::ShowList showlist; // 무엇인지 모르겠음
//            showlist.Reset();
//            showlist.AddRoi(itemCellMeasure->m_rejectInfo[nMeasure].m_rect);
//            IPVM::Point_32f_C2 pt;
//            pt.m_x = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_top;
//            pt.m_y = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_bottom;
//            showlist.AddPoint(pt);
//            ResultDimesnsion.m_stShowList = showlist;
//        }
//        ResultInfo.m_vecDimension.push_back(ResultDimesnsion);
//    }
//
//    // -- (3) Mismatch
//    m_loggerTcp.Log(0, _T("ResultVision Mismatch Defect Input"));
//    // Mismatch 는 결과 Data 에서 푸쉬백 하도록 수정?
//    //m_vecvecDiffWidth;
//
//    // ::WaitForSingleObject(test, INFINITE);
//    //ResetEvent(test);
//    Share::ResultDimension resultmismatch;
//    resultmismatch.m_strJudge = itemCellMeasure->GetInspResultString();
//    for (long nMismatch = 0; nMismatch < m_stResAllMismatchData.m_vecfDiffWidth.size(); nMismatch++)
//    {
//        if (topBack == _T("TOP"))
//        {
//            resultmismatch.m_nCamera_Location = 0; // TOP
//        }
//        else
//        {
//            resultmismatch.m_nCamera_Location = 1; //BACK
//        }
//        resultmismatch.m_nCamera_Number = 1; // 우리는 2개의 카메라 영상획득한걸 하나로 합치니까 1개?
//        resultmismatch.m_nScreen_Number = 2; // 우리 1개 Camera 에서 2장 Image 생성 하니까
//
//        if (topBack == _T("TOP")) // TOP 에서만 Mismatch Data 보내기
//        {
//            resultmismatch.m_strPosition_InspItem = _T("MIMA");
//            m_vecMismatchData;
//            if (nMismatch == 0)
//            {
//                resultmismatch.m_strPosition_InspItem = _T("CC1L");
//
//                resultmismatch.m_inspType = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1
//                    + itemCellMeasure->m_para.m_vecMeasureType[nMismatch]);
//                resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
//                resultmismatch.m_dLSL = -0.8;
//                resultmismatch.m_dUSL = 0.8;
//                resultmismatch.m_dGL = 0;
//                if (resultmismatch.m_strJudge.Left(4) == _T("Pass"))
//                {
//                    resultmismatch.m_strJudge = _T("OK");
//                }
//                else
//                {
//                    resultmismatch.m_strJudge = _T("NG");
//                }
//            }
//            if (nMismatch == 1)
//            {
//                resultmismatch.m_strPosition_InspItem = _T("CC1R");
//
//                resultmismatch.m_inspType = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1
//                    + itemCellMeasure->m_para.m_vecMeasureType[nMismatch]);
//                resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
//            }
//            if (nMismatch == 2)
//            {
//                resultmismatch.m_strPosition_InspItem = _T("CC2L");
//
//                resultmismatch.m_inspType = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1
//                    + itemCellMeasure->m_para.m_vecMeasureType[nMismatch]);
//                resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
//            }
//            if (nMismatch == 3)
//            {
//                resultmismatch.m_strPosition_InspItem = _T("CC2R");
//
//                resultmismatch.m_inspType = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1
//                    + itemCellMeasure->m_para.m_vecMeasureType[nMismatch]);
//                resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
//            }
//        }
//        ResultInfo.m_vecMismatch.push_back(resultmismatch);
//    }
//
//    // -- (4) Defect
//    m_loggerTcp.Log(0, _T("ResultVision Surface Defect Input"));
//    for (long nDefect = 0; nDefect < unit->m_ISICellAllDefect.m_vecCandidate.size(); nDefect++)
//    {
//        if (unit->m_resultValue == II_RESULT_VALUE::PASS)
//        {
//            continue;
//        }
//
//        Share::ResultDefect resultDefect;
//        if (topBack == _T("TOP"))
//        {
//            resultDefect.m_nCamera_Position = 0; // TOP
//            resultDefect.m_nScreen_Number = 1;
//        }
//        else
//        {
//            resultDefect.m_nCamera_Position = 1; //BACK
//            resultDefect.m_nScreen_Number = 1;
//        }
//        resultDefect.m_strCamera_Type = _T("LINE");
//        resultDefect.m_nCamera_Number = 1; // 일단 넣자
//
//        CString strGain;
//        if (m_cisComm->GetGrabExcutor(0) != NULL)
//        {
//            m_cisComm->GetGrabExcutor(0)->GetSystemGain();
//        }
//        double dAverage = 0, dStdev = 0;
//        unsigned char cMin = 0, cMax = 0;
//
//        IPVM::ImageProcessing::GetMeanStdev(unit->m_inspectionImageDark,
//            unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel, dAverage, dStdev);
//        IPVM::ImageProcessing::GetMinMax(
//            unit->m_inspectionImageDark, unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel, cMin, cMax);
//        Share::ResultDefectROI defectROI;
//
//        defectROI.m_nIndex = nDefect;
//        defectROI.m_dRelative_X
//            = (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left * g_systemParameter.m_dPixelToUmXUp)
//            / 1000;
//        defectROI.m_dRelative_Y
//            = (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top * g_systemParameter.m_dPixelToUmY)
//            / 1000;
//        defectROI.m_dWidth = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].WIDTH_mm.GetValue();
//        defectROI.m_dLength = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].LENGTH_mm.GetValue();
//        defectROI.m_dImg_Width = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Width();
//        defectROI.m_dImg_Length = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Height();
//        defectROI.m_dArea_MM2
//            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AREA.GetValue() * g_systemParameter.m_dPixelToUmY;
//        defectROI.m_dArea_Pixels = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AREA.GetValue();
//        defectROI.m_dRatio_Width_Length = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].RATIO_WL.GetValue();
//        // 만들어야 함....
//        defectROI.m_dGray_LV_Avg = dAverage;
//        defectROI.m_dGray_LV_Max = cMax;
//        defectROI.m_dGray_LV_Min = cMin;
//        defectROI.m_dGray_LV_Range = cMax - cMin;
//        defectROI.m_dGray_LV_Stddev = dStdev; // 이건 만들어야 함.
//        ///
//        defectROI.m_dRoi_Gray_LV_Avg
//            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AVG_GV.GetValue(); // 이건 만들어야 함.
//        defectROI.m_dRoi_Gray_LV_Max
//            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MAX_GV.GetValue(); // 이건 만들어야 함.
//        defectROI.m_dRoi_Gray_LV_Min
//            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MIN_GV.GetValue(); // 이건 만들어야 함.
//        defectROI.m_dRoi_Gray_LV_Median
//            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MEDIAN_GV.GetValue(); // 이건 만들어야 함.
//        defectROI.m_dRoi_Gray_LV_Stddev
//            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].STDEV_GV.GetValue(); // 이건 만들어야 함.
//
//        CRect rt;
//        rt.left = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left / CELLRESIZEDIVID;
//        rt.top = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top / CELLRESIZEDIVID;
//        rt.bottom = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_bottom / CELLRESIZEDIVID;
//        rt.right = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_right / CELLRESIZEDIVID;
//        defectROI.m_rtShowROI = rt; // 이건 만들어야 함.
//        defectROI.m_strDefect_Position = _T("COAT"); // 이건 만들어야 함.
//        if (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].nInspectImage == 1)
//        {
//            defectROI.m_strDefect_Color = _T("BRIGHT"); // 이건 만들어야 함.
//        }
//        else
//        {
//            defectROI.m_strDefect_Color = _T("DARK");
//        }
//
//        if (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left
//            > unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Width() / 2)
//        {
//            defectROI.m_nDefect_Lane = 2;
//            defectROI.m_bDefect_Left = FALSE;
//        }
//        else
//        {
//            defectROI.m_nDefect_Lane = 1;
//            defectROI.m_bDefect_Left = TRUE;
//        }
//        resultDefect.m_vecstDefectROI.push_back(defectROI);
//        resultDefect.m_dCamera_Gain = _ttof(strGain);
//
//        resultDefect.m_inspType = (DefectType)unit->m_ISICellAllDefect.m_vecISIDefectType[nDefect];
//        ResultInfo.m_vecDefect.push_back(resultDefect);
//    }
//
//    // -- (5) Original Image
//
//    m_loggerTcp.Log(0, _T("ResultVision Image Input"));
//    for (long nImage = 0; nImage < 2; nImage++) // Bright Dark 2개만 있으니까?
//    {
//        Share::OriginalImageInfo ImageInfo;
//        IPVM::Image_8u_C1 ResizeImage;
//        ResizeImage.Free();
//
//        ResizeImage.Create(unit->m_imageSizeX / CELLRESIZEDIVID, unit->m_imageSizeY / CELLRESIZEDIVID);
//        if (topBack == _T("TOP"))
//        {
//            ImageInfo.nCamera = 0; // TOP
//        }
//        else
//        {
//            ImageInfo.nCamera = 1; //BACK
//        }
//        if (nImage == 0)
//        {
//            ImageInfo.nChannel = 0;
//            IPVM::ImageProcessing::Resize_Linear(unit->m_inspectionImageBright, ResizeImage);
//            ImageInfo.srcImg = ResizeImage;
//        }
//        else
//        {
//            ImageInfo.nChannel = 1;
//            IPVM::ImageProcessing::Resize_Linear(unit->m_inspectionImageDark, ResizeImage);
//            ImageInfo.srcImg = ResizeImage;
//        }
//
//        ImageInfo.nSizeX = unit->m_imageSizeX / CELLRESIZEDIVID;
//        ImageInfo.nSizeY = unit->m_imageSizeY / CELLRESIZEDIVID;
//        ResultInfo.m_vecOriginalImage.push_back(ImageInfo);
//    }
//
//    m_loggerTcp.Log(0, _T("ResultVision Make End"));
//    ResultInfo.m_bNGTag = FALSE;
//
//    long n = sizeof(ResultInfo);
//    ResultData = ResultInfo;
//
//    //m_tcpResultSender->Push(ResultInfo);
//
//    //if (topBack == _T("BACK"))
//    //{
//    //    SendHostData(ResultInfo);
//    //}
//}


void CIBCIDlg::SendHostCameraInfoData(Share::ResultVision& ResultData, CInspectionCell* itemCellAlign)
{
    Share::ResultVision ResultInfo = ResultData;

    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

    CInspectionCell* unit = itemCellAlign;

    // REQEUST_JSONDATA 해당 Data 를 받았으면 vecCamra 에 Data 넣어주기
    if (m_bReceiveRequestData == TRUE) //m_bReceiveRequestData == TRUE
    {
        //auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
        Share::CameraInfo cameraInfo;

        for (long nCam = 0; nCam < m_cisComm->GetCameraCount(); nCam++)
        {
            if (topBack == _T("TOP"))
            {
                cameraInfo.m_nCamera_Location = 0; // TOP
            }
            else
            {
                cameraInfo.m_nCamera_Location = 1; //BACK
            }
            cameraInfo.m_nCamera_Number = 1;
            cameraInfo.m_dResolution_X = g_systemParameter.m_dPixelToUmXUp;
            cameraInfo.m_dResolution_Y = g_systemParameter.m_dPixelToUmY;
            cameraInfo.m_nCamera_Gain = m_cisComm->GetGrabExcutor(nCam)->GetSystemGain();
            double dExposure;
            m_cisComm->GetGrabExcutor(nCam)->GetExposureTime(dExposure, dExposure);
            cameraInfo.m_nExposure_Time = 0.0f;
            cameraInfo.m_fAngle = 0.0f;

            Share::ImageInfo ImageInfo;
            //align->image
            cameraInfo.m_vecImageQuality.clear();
            auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
            if (nCam == 0)
            {
                // 1 lane 인 경우, 1,2 Edge
                // 2 lane 인 경우, 1,2,3,4 Edge
                for (long nEdge = 1; nEdge < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size() - 1; nEdge++)
                {
                    IPVM::Rect rtResult;
                    rtResult = itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect[nEdge];

                    IPVM::Rect rtCropROI;
                    rtCropROI.m_left = 0;
                    rtCropROI.m_top = 0;
                    rtCropROI.m_bottom = rtResult.Height();
                    rtCropROI.m_right = rtResult.Width();

                    IPVM::Image_8u_C1 CropImage;
                    CropImage.Create(rtResult.Width(),
                        rtResult
                            .Height()); // 좀 더 크게 보여 주기 위해 어떻게 진행이 되어야 할까? off set 값을 어떻게 계산하여 넣어줄까?
                    IPVM::ImageProcessing::Copy(itemCellAlign->m_inspectionImageDark, rtResult, rtCropROI, CropImage);
                    ImageInfo.m_CropImage_LaneEdge = CropImage;
                    ImageInfo.nEdgeNum = nEdge + 1;
                    ImageInfo.nLaneNum = 2;
                    ImageInfo.nSizeX = rtResult.Width();
                    ImageInfo.nSizeY = rtResult.Height();
                    ImageInfo.nFocus = 0;
                    //ImageInfo.nGrayLevelValue[0] = 10; // 이거 해줘야 하나?
                    //ImageInfo.nGrayLevelValue[1] = 11;
                    ImageInfo.nGrayLevelValue_Bright
                        = itemCellAlign->m_AlignGrayValue.vecGV_Bright[nEdge]; // 이거 해줘야 하나?
                    ImageInfo.nGrayLevelValue_Dark = itemCellAlign->m_AlignGrayValue.vecGV_Dark[nEdge];
                    cameraInfo.m_vecImageQuality.push_back(ImageInfo);
                }
            }

            ResultData.m_vecCamera.push_back(cameraInfo);
        }
        m_bReceiveRequestData = FALSE;
    }
}

void CIBCIDlg::SendHostDimensionData(Share::ResultVision& ResultData, CInspectionCell* itemCellAlign)
{
    Share::ResultVision ResultInfo = ResultData;

    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

    CInspectionCell* unit = itemCellAlign;

    auto* itemCellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);
    itemCellMeasure->m_rejectInfo;
    m_loggerTcp.Log(0, _T("ResultVision Dimension Defect Input"));

    long nReject = 0;
    for (long nMeasure = 0; nMeasure < itemCellMeasure->m_para.m_nCountMeasureItem; nMeasure++)
    {
        Share::ResultDimension ResultDimesnsion;

        if (topBack == _T("TOP"))
        {
            ResultDimesnsion.m_nCamera_Location = 0; // TOP
        }
        else
        {
            ResultDimesnsion.m_nCamera_Location = 1; //BACK
        }
        ResultDimesnsion.m_nCamera_Number = 1; // 우리는 2개의 카메라 영상획득한걸 하나로 합치니까 1개?
        ResultDimesnsion.m_nScreen_Number = 2; // 우리 1개 Camera 에서 2장 Image 생성 하니까

        if (unit->m_ISICellAllDefect.m_vvMeasureDefect.size() == 0)
        {
            ResultDimesnsion.m_strPosition_InspItem = _T("INVALID");
            ResultDimesnsion.m_strPosition_Sub = _T("INVALID");
            ResultDimesnsion.m_dValue = 0;
            ResultDimesnsion.m_dLSL = 0;
            ResultDimesnsion.m_dUSL = 0;
            ResultDimesnsion.m_dGL = 0.0f; // G
            ResultDimesnsion.m_dStart_Point_X = 0;
            ResultDimesnsion.m_dEnd_Point_X = 0;
            ResultDimesnsion.m_dStart_Point_Y = 0;
            ResultDimesnsion.m_dEnd_Point_Y = 0;
        }
        else
        {
#if INSPECTION_MACHINE_UC1
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftFoil)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT3");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightFoil)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT4");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_LeftCoating)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_RightCoating)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
            }
#else
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_FullLength)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_1)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_MiddleFoil)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT3");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_NonCoating_2)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("NCTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT4");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_1)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT1");
            }
            if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_defectType == DefectType::Width_Coating_2)
            {
                ResultDimesnsion.m_strPosition_InspItem = _T("CTW");
                ResultDimesnsion.m_strPosition_Sub = _T("CT2");
            }
#endif // INSPECTION_MACHINE_UC1

            //ResultDimesnsion.m_strPosition_Sub = strSub; // 무엇이 들어가야 하는지 모르겠음
            ResultDimesnsion.m_inspType = (DefectType)((long)DefectType::START_Coater_Dimension + 1
                + itemCellMeasure->m_para.m_vecMeasureType[nMeasure]);

            ResultDimesnsion.m_dValue = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].LENGTH;
            ResultDimesnsion.m_dLSL = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureOffsetLSL;
            ResultDimesnsion.m_dUSL = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureOffsetUSL;

            ResultDimesnsion.m_strJudge = itemCellMeasure->GetInspResultString();

            if (ResultDimesnsion.m_strJudge.Left(4) == _T("Pass"))
            {
                ResultDimesnsion.m_strJudge = _T("OK");
            }
            else
            {
                ResultDimesnsion.m_strJudge = _T("NG");
            }
            ResultDimesnsion.m_dGL = 0.0f; // Gray Value 값 추가 해서넣어야함.
            ResultDimesnsion.m_dStart_Point_X
                = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureStartPosX;
            ResultDimesnsion.m_dEnd_Point_X = unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].m_fMeasureEndPosX;
            ResultDimesnsion.m_dStart_Point_Y = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_top;
            ResultDimesnsion.m_dEnd_Point_Y = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_bottom;
            if (itemCellMeasure->m_rejectInfo.Count() == 0)
            {
                ResultDimesnsion.m_dStart_Point_Y = 0;
                ResultDimesnsion.m_dEnd_Point_Y = 3508;
            }
            else if (unit->m_ISICellAllDefect.m_vvMeasureDefect[0][nMeasure].GetJudge() == II_RESULT_VALUE::PASS)
            {
                ResultDimesnsion.m_dStart_Point_Y = 0;
                ResultDimesnsion.m_dEnd_Point_Y = 3508;
            }
            else
            {
                ResultDimesnsion.m_dStart_Point_Y = itemCellMeasure->m_rejectInfo[nReject].m_rect.m_top;
                ResultDimesnsion.m_dEnd_Point_Y = itemCellMeasure->m_rejectInfo[nReject].m_rect.m_bottom;
                nReject++;
            }

            if (ResultDimesnsion.m_dStart_Point_X > unit->m_imageSizeX /* imageBright.GetSizeX() */ / 2)
            {
                ResultDimesnsion.m_bDefect_Left = FALSE;
            }
            else
            {
                ResultDimesnsion.m_bDefect_Left = TRUE;
            }
            Share::ShowList showlist; // 무엇인지 모르겠음
            auto* align = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
            showlist.Reset();
            for (long idx = 0; idx < align->m_paraEdgeDetectBlob.m_vecedgePara.size(); idx++)
            {
                showlist.AddRoi(align->m_paraEdgeDetectBlob.m_vecrtDetect[idx]);
                IPVM::Point_32f_C2 pt;
                pt.m_x = align->m_paraEdgeDetectBlob.m_vecrtDetect[idx].m_top;
                pt.m_y = align->m_paraEdgeDetectBlob.m_vecrtDetect[idx].m_bottom;
                showlist.AddPoint(pt);
                ResultDimesnsion.m_stShowList = showlist;
            }
            //showlist.AddRoi(itemCellMeasure->m_rejectInfo[nMeasure].m_rect);
            //IPVM::Point_32f_C2 pt;
            //pt.m_x = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_top;
            //pt.m_y = itemCellMeasure->m_rejectInfo[nMeasure].m_rect.m_bottom;
            //showlist.AddPoint(pt);
            //ResultDimesnsion.m_stShowList = showlist;
        }
        ResultData.m_vecDimension.push_back(ResultDimesnsion);
    }
}

void CIBCIDlg::SendHostMismatchData(Share::ResultVision& ResultData, CInspectionCell* itemCellAlign)
{
    Share::ResultVision ResultInfo = ResultData;

    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

    CInspectionCell* unit = itemCellAlign;
    auto* itemCellMeasure = (CInspectionMeasure*)unit->GetProcessingItem(ProcItemCategory::measure);

    m_loggerTcp.Log(0, _T("ResultVision Mismatch Defect Input"));
    // Mismatch 는 결과 Data 에서 푸쉬백 하도록 수정?
    //m_vecvecDiffWidth;

    // ::WaitForSingleObject(test, INFINITE);
    //ResetEvent(test);
    Share::ResultDimension resultmismatch;

    //long test =
    //auto& vecMeasureDefect = *unit->m_ISICellAllDefect.m_vvMeasureDefect.size();
    auto nSizeMeasureItem = unit->m_ISICellAllDefect.m_vvMeasureDefect;
    long nSizeDivideVertical = nSizeMeasureItem.size();
    for (long n = 0; n < nSizeDivideVertical; n++)
    {
        long nMis = nSizeMeasureItem[n].size();
        resultmismatch.m_strJudge = itemCellMeasure->GetInspResultString();
        for (long nMismatch = 0; nMismatch < nMis; nMismatch++)
        {
            if (topBack == _T("TOP"))
            {
                resultmismatch.m_nCamera_Location = 0; // TOP
            }
            else
            {
                resultmismatch.m_nCamera_Location = 1; //BACK
            }
            resultmismatch.m_nCamera_Number = 1; // 우리는 2개의 카메라 영상획득한걸 하나로 합치니까 1개?
            resultmismatch.m_nScreen_Number = 2; // 우리 1개 Camera 에서 2장 Image 생성 하니까

            if (topBack == _T("TOP")) // TOP 에서만 Mismatch Data 보내기
            {
                resultmismatch.m_strPosition_InspItem = _T("MIMA");
                m_vecMismatchData;
                if (nMismatch == 0)
                {
                    resultmismatch.m_strPosition_Sub = _T("CC1L");

                    resultmismatch.m_inspType
                        = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1 + nMismatch);
                    //resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
                    resultmismatch.m_dValue = 0;
                    resultmismatch.m_dLSL = -0.8;
                    resultmismatch.m_dUSL = 0.8;
                    resultmismatch.m_dGL = 0;
                    if (resultmismatch.m_strJudge.Left(4) == _T("Pass"))
                    {
                        resultmismatch.m_strJudge = _T("OK");
                    }
                    else
                    {
                        resultmismatch.m_strJudge = _T("NG");
                    }
                }
                if (nMismatch == 1)
                {
                    resultmismatch.m_strPosition_Sub = _T("CC1R");

                    resultmismatch.m_inspType
                        = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1 + nMismatch);
                    //resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
                    resultmismatch.m_dValue = 0;
                }
                if (nMismatch == 2)
                {
                    resultmismatch.m_strPosition_Sub = _T("CC2L");

                    resultmismatch.m_inspType
                        = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1 + nMismatch);
                    //resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
                    resultmismatch.m_dValue = 0;
                }
                if (nMismatch == 3)
                {
                    resultmismatch.m_strPosition_Sub = _T("CC2R");

                    resultmismatch.m_inspType
                        = (DefectType)((long)DefectType::Start_Dimension_Mismatch + 1 + nMismatch);
                    //resultmismatch.m_dValue = m_stResAllMismatchData.m_vecfDiffWidth[nMismatch];
                    resultmismatch.m_dValue = 0;
                }
            }
            ResultData.m_vecMismatch.push_back(resultmismatch);
        }
    }
}

void CIBCIDlg::SendHostDefectData(Share::ResultVision& ResultData, CInspectionCell* itemCellAlign)
{
    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

    CInspectionCell* unit = itemCellAlign;

    m_loggerTcp.Log(0, _T("ResultVision Surface Defect Input"));
    for (long nDefect = 0; nDefect < unit->m_ISICellAllDefect.m_vecCandidate.size(); nDefect++)
    {
        if (unit->m_resultValue == II_RESULT_VALUE::PASS)
        {
            continue;
        }

        Share::ResultDefect resultDefect;
        if (topBack == _T("TOP"))
        {
            resultDefect.m_nCamera_Position = 0; // TOP
            resultDefect.m_nScreen_Number = 1;
        }
        else
        {
            resultDefect.m_nCamera_Position = 1; //BACK
            resultDefect.m_nScreen_Number = 1;
        }
        resultDefect.m_strCamera_Type = _T("LINE");
        resultDefect.m_nCamera_Number = 1; // 일단 넣자

        CString strGain;
        if (m_cisComm->GetGrabExcutor(0) != NULL)
        {
            m_cisComm->GetGrabExcutor(0)->GetSystemGain();
        }
        double dAverage = 0, dStdev = 0;
        unsigned char cMin = 0, cMax = 0;

        IPVM::ImageProcessing::GetMeanStdev(unit->m_inspectionImageDark,
            unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel, dAverage, dStdev);
        IPVM::ImageProcessing::GetMinMax(
            unit->m_inspectionImageDark, unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel, cMin, cMax);
        Share::ResultDefectROI defectROI;

        defectROI.m_nIndex = nDefect;
        defectROI.m_dRelative_X
            = (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left * g_systemParameter.m_dPixelToUmXUp)
            / 1000;
        defectROI.m_dRelative_Y
            = (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top * g_systemParameter.m_dPixelToUmY)
            / 1000;
        defectROI.m_dRelative_Img_X = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left;
        defectROI.m_dRelative_Img_Y = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top;
        defectROI.m_dWidth = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].WIDTH_mm.GetValue();
        defectROI.m_dLength = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].LENGTH_mm.GetValue();
        defectROI.m_dImg_Width = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Width();
        defectROI.m_dImg_Length = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Height();
        defectROI.m_dArea_MM2
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AREA.GetValue() * g_systemParameter.m_dPixelToUmY;
        defectROI.m_dArea_Pixels = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AREA.GetValue();
        defectROI.m_dRatio_Width_Length = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].RATIO_WL.GetValue();
        // 만들어야 함....
        defectROI.m_dGray_LV_Avg = dAverage;
        defectROI.m_dGray_LV_Max = cMax;
        defectROI.m_dGray_LV_Min = cMin;
        defectROI.m_dGray_LV_Range = cMax - cMin;
        defectROI.m_dGray_LV_Stddev = dStdev; // 이건 만들어야 함.
        ///
        defectROI.m_dRoi_Gray_LV_Avg
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AVG_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Max
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MAX_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Min
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MIN_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Median
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MEDIAN_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Stddev
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].STDEV_GV.GetValue(); // 이건 만들어야 함.

        CRect rt;
        rt.left = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left /* / CELLRESIZEDIVID*/;
        rt.top = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top /*/ CELLRESIZEDIVID*/;
        rt.bottom = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_bottom /*/ CELLRESIZEDIVID*/;
        rt.right = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_right /* / CELLRESIZEDIVID*/;
        defectROI.m_rtShowROI = rt; // 이건 만들어야 함.
        defectROI.m_strDefect_Position = _T("COAT"); // 이건 만들어야 함.
        if (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].nInspectImage == 1)
        {
            defectROI.m_strDefect_Color = _T("BRIGHT"); // 이건 만들어야 함.
        }
        else
        {
            defectROI.m_strDefect_Color = _T("DARK");
        }

        if (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left
            > unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Width() / 2)
        {
            defectROI.m_nDefect_Lane = 2;
            defectROI.m_bDefect_Left = FALSE;
        }
        else
        {
            defectROI.m_nDefect_Lane = 1;
            defectROI.m_bDefect_Left = TRUE;
        }
        resultDefect.m_vecstDefectROI.push_back(defectROI);
        resultDefect.m_dCamera_Gain = _ttof(strGain);

        resultDefect.m_inspType = (DefectType)unit->m_ISICellAllDefect.m_vecISIDefectType[nDefect];
        ResultData.m_vecDefect.push_back(resultDefect);
    }
}

void CIBCIDlg::SendHostDefectData(Share::ResultVision& ResultData, std::vector<IPVM::Image_8u_C1>& vecBrightImg,std::vector<IPVM::Image_8u_C1>& vecDarkImg, CInspectionCell* itemCellAlign)
{
    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

    CInspectionCell* unit = itemCellAlign;

    m_loggerTcp.Log(0, _T("ResultVision Surface Defect Input"));

    long nImageNum = 0;

    for (long nDefect = 0; nDefect < unit->m_ISICellAllDefect.m_vecCandidate.size(); nDefect++)
    {
        if (unit->m_ISICellAllDefect.m_vecISIResultCode[nDefect] == II_RESULT_VALUE::PASS)
        {
            continue;
        }

        Share::ResultDefect resultDefect;
        if (topBack == _T("TOP"))
        {
            resultDefect.m_nCamera_Position = 0; // TOP
            resultDefect.m_nScreen_Number = 1;
        }
        else
        {
            resultDefect.m_nCamera_Position = 1; //BACK
            resultDefect.m_nScreen_Number = 1;
        }
        resultDefect.m_strCamera_Type = _T("LINE");
        resultDefect.m_nCamera_Number = 1; // 일단 넣자

        CString strGain;
        if (m_cisComm->GetGrabExcutor(0) != NULL)
        {
            m_cisComm->GetGrabExcutor(0)->GetSystemGain();
        }
        double dAverage = 0, dStdev = 0;
        unsigned char cMin = 0, cMax = 0;

        IPVM::ImageProcessing::GetMeanStdev(unit->m_inspectionImageDark,
            unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel, dAverage, dStdev);
        IPVM::ImageProcessing::GetMinMax(
            unit->m_inspectionImageDark, unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel, cMin, cMax);
        Share::ResultDefectROI defectROI;

        defectROI.m_nIndex = nDefect;
        defectROI.m_dRelative_X
            = (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left * g_systemParameter.m_dPixelToUmXUp)
            / 1000;
        defectROI.m_dRelative_Y
            = (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top * g_systemParameter.m_dPixelToUmY)
            / 1000;

        defectROI.m_dRelative_Img_X = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left;
        defectROI.m_dRelative_Img_Y = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top;
        defectROI.m_dWidth = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].WIDTH_mm.GetValue();
        defectROI.m_dLength = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].LENGTH_mm.GetValue();
        defectROI.m_dImg_Width = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Width();
        defectROI.m_dImg_Length = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Height();
        defectROI.m_dArea_MM2
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AREA.GetValue() * g_systemParameter.m_dPixelToUmY;
        defectROI.m_dArea_Pixels = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AREA.GetValue();
        defectROI.m_dRatio_Width_Length = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].RATIO_WL.GetValue();
        // 만들어야 함....
        defectROI.m_dGray_LV_Avg = dAverage;
        defectROI.m_dGray_LV_Max = cMax;
        defectROI.m_dGray_LV_Min = cMin;
        defectROI.m_dGray_LV_Range = cMax - cMin;
        defectROI.m_dGray_LV_Stddev = dStdev; // 이건 만들어야 함.
        ///
        defectROI.m_dRoi_Gray_LV_Avg
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].AVG_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Max
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MAX_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Min
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MIN_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Median
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].MEDIAN_GV.GetValue(); // 이건 만들어야 함.
        defectROI.m_dRoi_Gray_LV_Stddev
            = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].STDEV_GV.GetValue(); // 이건 만들어야 함.

        CRect rt;
        rt.left = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left /* / CELLRESIZEDIVID*/;
        rt.top = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_top /*/ CELLRESIZEDIVID*/;
        rt.bottom = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_bottom /*/ CELLRESIZEDIVID*/;
        rt.right = unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_right /* / CELLRESIZEDIVID*/;
        defectROI.m_rtShowROI = rt; // 이건 만들어야 함.
        defectROI.m_strDefect_Position = _T("COAT"); // 이건 만들어야 함.
        if (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].nInspectImage == 1)
        {
            defectROI.m_strDefect_Color = _T("BRIGHT"); // 이건 만들어야 함.
        }
        else
        {
            defectROI.m_strDefect_Color = _T("DARK");
        }

        if (unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.m_left
            > unit->m_ISICellAllDefect.m_vecCandidate[nDefect].rtRect_pixel.Width() / 2)
        {
            defectROI.m_nDefect_Lane = 2;
            defectROI.m_bDefect_Left = FALSE;
        }
        else
        {
            defectROI.m_nDefect_Lane = 1;
            defectROI.m_bDefect_Left = TRUE;
        }
        defectROI.m_DstCropImage_Bright = vecBrightImg[nImageNum];
        defectROI.m_DstCropImage_Dark = vecDarkImg[nImageNum];
        nImageNum++;
        resultDefect.m_vecstDefectROI.push_back(defectROI);
        resultDefect.m_dCamera_Gain = _ttof(strGain);

        resultDefect.m_inspType = (DefectType)unit->m_ISICellAllDefect.m_vecISIDefectType[nDefect];
        ResultData.m_vecDefect.push_back(resultDefect);
    }
}

void CIBCIDlg::SendHostOriginalImageData(Share::ResultVision& ResultData, CInspectionCell* itemCellAlign)
{
    Share::ResultVision ResultInfo = ResultData;

    CString topBack;
    topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");

    CInspectionCell* unit = itemCellAlign;
    m_loggerTcp.Log(0, _T("ResultVision Image Input"));
    for (long nImage = 0; nImage < 2; nImage++) // Bright Dark 2개만 있으니까?
    {
        Share::OriginalImageInfo ImageInfo;
        IPVM::Image_8u_C1 ResizeImage;
        ResizeImage.Free();

        ResizeImage.Create(unit->m_imageSizeX / CELLRESIZEDIVID, unit->m_imageSizeY / CELLRESIZEDIVID);
        if (topBack == _T("TOP"))
        {
            ImageInfo.nCamera = 0; // TOP
        }
        else
        {
            ImageInfo.nCamera = 1; //BACK
        }
        if (nImage == 0)
        {
            ImageInfo.nChannel = 0;
            IPVM::ImageProcessing::Resize_Linear(unit->m_inspectionImageBright, ResizeImage);
            ImageInfo.srcImg = ResizeImage;
        }
        else
        {
            ImageInfo.nChannel = 1;
            IPVM::ImageProcessing::Resize_Linear(unit->m_inspectionImageDark, ResizeImage);
            ImageInfo.srcImg = ResizeImage;
        }

        ImageInfo.nSizeX = unit->m_imageSizeX / CELLRESIZEDIVID;
        ImageInfo.nSizeY = unit->m_imageSizeY / CELLRESIZEDIVID;
        ResultData.m_vecOriginalImage.push_back(ImageInfo);
    }

}

void CIBCIDlg::LotClear()
{
    m_cellImageProducers->ClearLot();
    m_inlineResultCoupler->ClearLot();
}

void CIBCIDlg::SendHostData(Share::ResultVision ResultData)
{
    m_tcpResultSender->Push(ResultData);
}

BOOL CIBCIDlg::HostSocketConnectCheck()
{
    if (m_pClientSocket->IsConnected() == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
        

}

void CIBCIDlg::GetDefectCropImage(IPVM::Image_8u_C1 brightImg, IPVM::Image_8u_C1 darkImg,
    std::vector<IPVM::Image_8u_C1>& vecBrightImg, std::vector<IPVM::Image_8u_C1>& vecDarkImg,
    CInspectionCell* itemCellAlign)
{
    CInspectionCell* unit = itemCellAlign;
    for (long n = 0; n < unit->m_ISICellAllDefect.m_vecCandidate.size(); n++)
    {
        IPVM::Rect rtDraw = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel;

        IPVM::Rect rect = rtDraw;

        int OriginRect_Width = rect.Width();
        int OriginRect_Height = rect.Height();
        int Offset_X = 0;
        int Offset_Y = 0;
        if (100 > max(rect.Width(), rect.Height()))
        {
            Offset_X = 50;
            Offset_Y = 50;
        }
        else if (rect.Width() > rect.Height())
        {
            int Diff = rect.Width() - rect.Height();
            int Diff_half = Diff / 2;
            Offset_Y = Diff_half;
        }
        else
        {
            int Diff = rect.Height() - rect.Width();
            int Diff_half = Diff / 2;
            Offset_X = Diff_half;
        }
        rect.InflateRect(Offset_X, Offset_Y, Offset_X, Offset_Y);
        rect &= IPVM::Rect(brightImg);
        //vecDefectROI.push_back(rect);
        int offset_left = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_left - rect.m_left;
        int offset_top = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_top - rect.m_top;
        int offset_right = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_right - rect.m_right;
        int offset_bottom = unit->m_ISICellAllDefect.m_vecCandidate[n].rtRect_pixel.m_bottom - rect.m_bottom;

        IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
        IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());

        IPVM::Rect OriginRect(CropImage_Bright);
        OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);

        IPVM::ImageProcessing::Copy(brightImg, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);
        IPVM::ImageProcessing::Copy(darkImg, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);

        if ((II_RESULT_VALUE)unit->m_ISICellAllDefect.m_vecISIResultCode[n] == II_RESULT_VALUE::PASS)
        {
            continue;
        }

        //SaveCropImages(unit, n, imageBright, imageDark, CropImage_Bright, CropImage_Dark, strDirectPath);
        //SaveNG = true;
        //IPVM::CreateDirectories(strDirectPath);
        CString topBack;
        topBack = !g_systemParameter.bDirection ? _T("TOP") : _T("BACK");
        CString lotId = _T("Test1234");
        CString strDirectPath;
        strDirectPath.Format(_T("D:\\RESULT\\%s\\%s\\%010d"), topBack, lotId, unit->m_cellID);
        SPathCropImages sPathCropImages
            = SaveCropImages(unit, n, brightImg, darkImg, CropImage_Bright, CropImage_Dark, strDirectPath);

        // CISIViewerProperty propertyView = MakeISIViewerProperty(unit, n, CropImage_Bright, CropImage_Dark, rtDraw, sPathCropImages);
        //CString strCreateTime;
        // strCreateTime.Format(
        //    _T("%04d%02d%02d%09d"), stCreateTime.wYear, stCreateTime.wMonth, stCreateTime.wDay, unit->m_cellID);
        //propertyView.strTime = strCreateTime;
        //strRows.AppendFormat(_T("%s\n"), MakeCsvRowString(n, propertyView, unit));
        //bResult[unit->m_ISICellAllDefect.m_vecISIDefectType[n]] = TRUE;

        IPVM::Image_8u_C1 ImgCrop_bright(128, 128);
        IPVM::ImageProcessing::Resize_Linear(CropImage_Bright, ImgCrop_bright);

        IPVM::Image_8u_C1 ImgCrop_dark(128, 128);
        IPVM::ImageProcessing::Resize_Linear(CropImage_Dark, ImgCrop_dark);
        // 호스트에게 보내 줄 Crop Image 를 여기에서 넣어준다.
        vecBrightImg.push_back(ImgCrop_bright);
        vecDarkImg.push_back(ImgCrop_dark);
    }
}