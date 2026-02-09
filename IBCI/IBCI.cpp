
// IBCI.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "IBCI.h"
#include "IBCIDlg.h"

#include "../VisionModule/Language/LanguageText.h"
#include "../Inspection/Accessor.h"
#include "Path.h"
#include "../Module/Utility/Logger.h"
#include "../CompanySharedLibrary/Persistence/IniHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIBCIApp

BEGIN_MESSAGE_MAP(CIBCIApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CIBCIApp 생성

CIBCIApp::CIBCIApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CIBCIApp 개체입니다.

CIBCIApp theApp;


// CIBCIApp 초기화

BOOL CIBCIApp::InitInstance()
{
	CString filePath;
	filePath.Format(_T("%s\\System.ini"), Path::GetRootFolder());


	CString strLogPath = IniHelper::LoadSTRING(filePath, _T("System"), _T("Log Save Path"), _T("C:\\intekplus\\IBCI"));

	CString logFolder;
	logFolder.Format(_T("%s\\Log\\"), strLogPath);

	Utility::Logger loggerSequence(logFolder, _T("Sequence.txt"));
	Utility::Logger loggerSensorPosition(logFolder, _T("SensorPosition.txt"));
	Utility::Logger loggerMarkPosition(logFolder, _T("MarkPosition.txt"));
    Utility::Logger loggerAlarm(logFolder, _T("Alarm.txt"));
	Utility::Logger loggerGrab(logFolder, _T("Grab.txt"));
	Utility::Logger loggerIO(logFolder, _T("IO.txt"));
	Utility::Logger loggerResult(logFolder, _T("Result.txt"));
	Utility::Logger loggerDebug(logFolder, _T("Debug.txt"));
	Utility::Logger loggerTcp(logFolder, _T("Tcp.txt"));
	Utility::Logger loggerTime(logFolder, _T("Time.txt"));
	Utility::Logger loggerCPU(logFolder, _T("CPU Usage.txt"));
	Utility::Logger loggerEswa(logFolder, _T("Eswa.txt"));
	Utility::Logger loggerSimulation(logFolder, _T("Cell Simulation.txt"));
	Utility::Logger loggerImageSaveFailed(logFolder, _T("ImageSaveFailed.txt"));
	Utility::Logger loggerParamChanged(logFolder, _T("ParamChanged.txt"));
	Utility::Logger loggerJudge(logFolder, _T("Judge.txt"));
	Utility::Logger loggerCellResult(logFolder, _T("CellResult.csv"));
	Utility::Logger loggerFocus(logFolder, _T("Focus_Value.csv"));

	Inspection::Accessor::InitInstance(loggerSequence, loggerResult);

	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));

	CString langFile;
	langFile.Format(_T("%s\\LanguageDef.h"), Path::GetRootFolder());
	Language::InitInstance(langFile);	

	CIBCIDlg dlg(loggerSequence, loggerSensorPosition, loggerMarkPosition, loggerAlarm, loggerGrab, loggerIO,
        loggerResult, loggerDebug, loggerTcp, loggerTime, loggerCPU, loggerEswa, loggerSimulation,
        loggerImageSaveFailed, loggerParamChanged, loggerJudge, loggerCellResult, loggerFocus);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고 응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}



int CIBCIApp::ExitInstance()
{
	Inspection::Accessor::DelInstance();

	return CWinApp::ExitInstance();
}
