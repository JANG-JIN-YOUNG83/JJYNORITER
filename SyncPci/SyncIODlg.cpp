// SyncIODlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SyncIODlg.h"
#include "Syncboard.h"
#include "SyncPciDll.h"
#include "afxdialogex.h"

#include "../Module/CustomControl/CustomStatic.h"
#include "../Module/CustomControl/CustomCheckBox.h"
//#include "../SharedModule/VisionHostShare/basedef.h"
#include "Gadget/TimeCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CSyncIODlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSyncIODlg, CDialogEx)

CSyncIODlg::CSyncIODlg(IPVM::LoggerInterface &logger, int nGenerationVersion, void* pParent /*=NULL*/)
	: CDialogEx(CSyncIODlg::IDD, NULL)
	, m_logger(logger)
	, m_listLogIO(true, true, true, 2000)
	, m_isOpenSuccess(false)
	, m_bisHide(true)
	, m_nGenerationVersion(nGenerationVersion)
{
	for (int i = 0; i < 16; i++)
	{
		m_checkInput[i] = new CCustomCheckBox;
	}
	for (int i = 0; i < 16; i++)
	{
		m_checkOutput[i] = new CCustomCheckBox;
		m_bOutputValue[i] = FALSE; 
	}
	for (int i = 0; i < 2; i++)
	{
		m_checkCommand[i] = new CCustomCheckBox;
		m_bCommandValue[i] = FALSE;
	}

	m_staticInput = new CCustomStatic;
	m_staticOutput = new CCustomStatic;

	m_syncBoard = (CSyncboard*)pParent;

	m_bMoveReady = FALSE;
	m_bMoveStart = FALSE;
	m_bExit = FALSE;
	m_bHeartBeat = TRUE;
	m_nGetInputCount = 0;
}

CSyncIODlg::~CSyncIODlg()
{
	m_bExit = TRUE;
	m_syncBoard->m_bMonitoring = FALSE;
	::SetEvent(m_threadControlIOSignal.m_Signal_Kill);
	m_threadControlIOSignal.Terminate();
	m_threadControlCheckState.Terminate();
	Sleep(100);

	Sleep(100);


	for (int i = 0; i < 16; i++)
	{
		delete m_checkInput[i];
	}
	for (int i = 0; i < 16; i++)
	{
		delete m_checkOutput[i];
	}
	for (int i = 0; i < 2; i++)
	{
		delete m_checkCommand[i];
	}

	delete m_staticInput;
	delete m_staticOutput;
}

void CSyncIODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < 12; i++)
	{
		DDX_Control(pDX, IDC_CHECK_INPUT_1 + i, *m_checkInput[i]);
	}
	for (int i = 12; i < 16; i++)
	{
		DDX_Control(pDX, IDC_CHECK_INPUT_13 + i - 12, *m_checkInput[i]);
	}
	for (int i = 0; i < 16; i++)
	{
		DDX_Control(pDX, IDC_CHECK_OUTPUT_1 + i, *m_checkOutput[i]);
	}
	for (int i = 0; i < 2; i++)
	{
		DDX_Control(pDX, IDC_CHECK_COMMAND_1 + i, *m_checkCommand[i]);
	}
	DDX_Control(pDX, IDC_STATIC_INPUT, *m_staticInput);
	DDX_Control(pDX, IDC_STATIC_OUTPUT, *m_staticOutput);
	DDX_Control(pDX, IDC_LIST_LOG_IO, m_listLogIO);
}


BEGIN_MESSAGE_MAP(CSyncIODlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_OUTPUT_1, IDC_CHECK_OUTPUT_16, OnBnClickedCheckOutput)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_COMMAND_1, IDC_CHECK_COMMAND_2, OnBnClickedCheckCommand)
END_MESSAGE_MAP()


// CSyncIODlg 메시지 처리기입니다.



void CSyncIODlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	::SuspendThread(m_threadControlIOSignal.m_Handle_Thread);
	::SuspendThread(m_threadControlCheckState.m_Handle_Thread);
//	KillTimer(0);
	ShowWindow(SW_HIDE);
	m_bisHide = true;
	::ResumeThread(m_threadControlIOSignal.m_Handle_Thread);
	::ResumeThread(m_threadControlCheckState.m_Handle_Thread);
	//CDialogEx::OnCancel();
}


BOOL CSyncIODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_logger.AppendSlaveLogger(&m_listLogIO);
	m_logger.Log(0, _T("Show IO Dialog"));

	SetBackgroundColor(RGB(255, 255, 255));
	SetForegroundWindow();

	for (int i = 0; i < 16; i++)
	{
		{
			m_checkInput[i]->ModifyStyle(0x0FL, BS_OWNERDRAW | BS_MULTILINE);
			//m_checkInput[i]->SetCenterMultiLine();
            m_checkInput[i]->SetBackGradation(RGB(47, 85, 151), RGB(56, 100, 179), RGB(47, 85, 151), 0);
            m_checkInput[i]->SetFont(_T("MS Shell Dlg"), RGB(255, 255, 255), 12, 6, 6);
            m_checkInput[i]->SetThick(0, RGB(0, 0, 0));
        }
    }
    m_checkInput[0]->SetWindowText(_T("INSPECTION_RUN_ON\n"));
    m_checkInput[1]->SetWindowText(_T("CYL1_FWD_CHK_SENSOR\n"));
    m_checkInput[2]->SetWindowText(_T("NONE_3"));
    m_checkInput[3]->SetWindowText(_T("NONE_4"));
    m_checkInput[4]->SetWindowText(_T("NONE_5"));
    m_checkInput[5]->SetWindowText(_T("NONE_6"));
    m_checkInput[6]->SetWindowText(_T("NONE_7"));
    m_checkInput[7]->SetWindowText(_T("NONE_8"));
    m_checkInput[8]->SetWindowText(_T("NONE_9"));
    m_checkInput[9]->SetWindowText(_T("ENC_FW_CHK_SENSOR\n"));
    m_checkInput[10]->SetWindowText(_T("NONE_11"));
    m_checkInput[11]->SetWindowText(_T("NONE_12"));
    m_checkInput[12]->SetWindowText(_T("NONE_13"));
	m_checkInput[13]->SetWindowText(_T("NONE_14"));
	m_checkInput[14]->SetWindowText(_T("NONE_15"));
	m_checkInput[15]->SetWindowText(_T("NONE_16"));
	for (int i = 0; i < 16; i++)
	{
		m_checkInput[i]->SetCenterMultiLine();
		m_checkInput[i]->Invalidate();
	}

	for (int i = 0; i < 16; i++)
	{
		{
			m_checkOutput[i]->ModifyStyle(0x0FL, BS_OWNERDRAW);
			m_checkOutput[i]->SetBackGradation(RGB(47, 85, 151), RGB(56, 100, 179), RGB(47, 85, 151), 0);
			m_checkOutput[i]->SetFont(_T("MS Shell Dlg"), RGB(255, 255, 255), 12, 6, 6);
			m_checkOutput[i]->SetThick(0, RGB(0, 0, 0));
		}
	}
	m_checkOutput[0]->SetWindowText(_T("NONE_1"));
	m_checkOutput[1]->SetWindowText(_T("NONE_2"));
	m_checkOutput[2]->SetWindowText(_T("NONE_3"));
	m_checkOutput[3]->SetWindowText(_T("NONE_4"));
	m_checkOutput[4]->SetWindowText(_T("NONE_5"));
	m_checkOutput[5]->SetWindowText(_T("NONE_6"));
	m_checkOutput[6]->SetWindowText(_T("NONE_7"));
	m_checkOutput[7]->SetWindowText(_T("NONE_8"));
	m_checkOutput[8]->SetWindowText(_T("NONE_9"));
	m_checkOutput[9]->SetWindowText(_T("NONE_10"));
	m_checkOutput[10]->SetWindowText(_T("NONE_11"));
	m_checkOutput[11]->SetWindowText(_T("NONE_12"));
	m_checkOutput[12]->SetWindowText(_T("NONE_13"));
	m_checkOutput[13]->SetWindowText(_T("NONE_14"));
	m_checkOutput[14]->SetWindowText(_T("NONE_15"));
	m_checkOutput[15]->SetWindowText(_T("NONE_16"));
	for (int i = 0; i < 16; i++)
	{
		m_checkOutput[i]->SetCenterMultiLine();
		m_checkOutput[i]->Invalidate();
	}

	for (int i = 0; i < 2; i++)
	{
		{
			m_checkCommand[i]->ModifyStyle(0x0FL, BS_OWNERDRAW);
			m_checkCommand[i]->SetBackGradation(RGB(47, 85, 151), RGB(56, 100, 179), RGB(47, 85, 151), 0);
			m_checkCommand[i]->SetFont(_T("MS Shell Dlg"), RGB(255, 255, 255), 8, 6, 6);
			m_checkCommand[i]->SetThick(0, RGB(0, 0, 0));
		}
	}
	m_checkCommand[0]->SetWindowText(_T("INLINE_MODE"));
	m_checkCommand[1]->SetWindowText(_T("LIVE_SYNC_TRIGGER"));
	for (int i = 0; i < 2; i++)
	{
		m_checkCommand[i]->SetCenterMultiLine();
		m_checkCommand[i]->Invalidate();
	}
	
	//ThreadStartIOSignal();
	if (m_syncBoard->m_bOpen)
	{
		if (m_syncBoard->IsOpenPort())
		{
			ThreadStartIOSignal();
			ThreadStartCheckState();
		//	ThreadStartGetCellCountValue();

		//	::SetEvent(m_threadControlCellCountValue.m_Signal_Start);

			//SetTimer(0, 10, NULL);
			SetTimer(1, 100, NULL);
		//	SetTimer(2, 10, NULL);

			m_isOpenSuccess = true;
		}
		//	SetTimer(0, 50, NULL);
		//if (Share::GenerationVersion::SplitterRemoved == m_nGenerationVersion) {
			SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 7, 0); // 싱크 펌웨어 OPTION 0 : CIS, 1 : AREA
			m_logger.Log(0, _T("[Output] Sync Firmware Option 0 (0 : CIS, 1 : AREA)"));

		//}
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 9, 20);
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 10, 5);
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 9, 76);
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 10, 4);
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 9, 218);
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 10, 4);

		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 3, 5);	//Bi Cell
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 4, 5);
		//SyncPci_Write(m_syncBoard->GetHandleDevice(), 1, 5, 5);

		BYTE Value1, Value2, Value3, Value4, Value5;


		//SyncPci_Read(m_syncBoard->GetHandleDevice(), 1, 9, &Value1);
		//SyncPci_Read(m_syncBoard->GetHandleDevice(), 1, 10, &Value2);
		SyncPci_Read(m_syncBoard->GetHandleDevice(), 0, 48, &Value3);
		SyncPci_Read(m_syncBoard->GetHandleDevice(), 0, 49, &Value4);

		CString str;
		//str.Format(_T("[Output] Value Check %d %d"), (int)Value1, (int)Value2/*, (int)Value3, (int)Value4, (int)Value5*/);
		//m_logger.Log(0, str);

		str.Format(_T("Input                MICOM : %03d, CPLD : %03d"), (int)Value3, (int)Value4);
		m_staticInput->SetWindowTextW(str);
//		m_syncBoard->SetSignal_HandlerOutput(0, TRUE, FALSE);

		

//		m_syncBoard->SetSignal_HandlerOutput(0, FALSE, FALSE);

		//BYTE Value1, Value2;

		//SyncPci_ReadHandlerOut(m_syncBoard->GetHandleDevice(), 12, &Value1);
		//SyncPci_ReadHandlerOut(m_syncBoard->GetHandleDevice(), 13, &Value2);

		//m_syncBoard->m_nTabOffset = Value1 + (Value2 * 256);

	}
	
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSyncIODlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//if (!m_bMoveReady && m_syncBoard->GetSignal_HandlerMoveReady())
	//{
	//	m_bMoveReady = TRUE;

	//	m_syncBoard->SetSignal_VisionReady(TRUE);
	//}
	//if (m_bMoveReady && m_syncBoard->GetSignal_HandlerMoveStart())
	//{
	//	m_bMoveStart = TRUE;
	//}
	//else if (m_bMoveStart && !m_syncBoard->GetSignal_HandlerMoveStart())
	//{
	//	m_bMoveReady = FALSE;
	//	m_bMoveStart = FALSE;
	//	m_syncBoard->SetSignal_VisionReady(FALSE);
	//}
	//if (m_syncBoard->GetSignal_HandlerMoveReady())
	//{
	//	m_checkInput[0]->SetCheck(TRUE);
	//}
	//else
	//{
	//	m_checkInput[0]->SetCheck(FALSE);
	//}
	/*if (m_syncBoard->GetSignal_HandlerInput())
	{
		m_checkInput[0]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::NotchingRunEnable));
		m_checkInput[1]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::ElectrodeMoveStart));
		m_checkInput[2]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::ByPass));
		m_checkInput[3]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::ResultReset));
		m_checkInput[4]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::CISSplitterAlarm));
		m_checkInput[5]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::Encoder));
		m_checkInput[6]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::CellTrigger));
		m_checkInput[7]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::InternalAlarm));
	}*/
	//else
	//{
	//	m_checkInput[1]->SetCheck(FALSE);
	//}
	if (nIDEvent == 0)
	{
		//TRACE(_T("Wait IO Signal\n"));
//		m_logger.Log(0, _T("m_threadControlIOSignal Start"));
		::WaitForSingleObject(m_threadControlIOSignal.m_Signal_Ready, INFINITE);
//		m_logger.Log(0, _T("m_threadControlIOSignal End"));
		//TRACE(_T("Finish IO Signal\n"));
		::SetEvent(m_threadControlIOSignal.m_Signal_Start);
	}
	else if (nIDEvent == 1)
	{
		//TRACE(_T("Wait Check State\n"));
//		m_logger.Log(0, _T("m_threadControlCheckState Start"));
		::WaitForSingleObject(m_threadControlCheckState.m_Signal_Ready, INFINITE);
//		m_logger.Log(0, _T("m_threadControlCheckState End"));
		//TRACE(_T("Finish Check State\n"));
		::SetEvent(m_threadControlCheckState.m_Signal_Start);
	}
	else if (nIDEvent == 2)
	{
		if (!m_isOpenSuccess)
		{
			KillTimer(2);
		}
		else
		{
			/*if (m_syncBoard->m_nTabOffset[0] == 0 || m_syncBoard->m_nTabOffset[1] == 0)*/
			if (m_syncBoard->GetCellCountValue() == 0)
			{
				//TRACE(_T("Wait Cell Count Value\n"));
				m_logger.Log(0, _T("m_threadControlCellCountValue Start"));
				::WaitForSingleObject(m_threadControlCellCountValue.m_Signal_Ready, INFINITE);
				m_logger.Log(0, _T("m_threadControlCellCountValue End"));
				//TRACE(_T("Finish Cell Count Value\n"));
				::SetEvent(m_threadControlCellCountValue.m_Signal_Start);
			}
			//else if (m_syncBoard->m_nTabOffset[0] > 0 && m_syncBoard->m_nTabOffset[1] > 0)
			/*else if (m_syncBoard->GetCellCountValue() > 0)
			{
				KillTimer(2);

			}*/
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CSyncIODlg::ThreadStartIOSignal()
{
	m_threadControlIOSignal.Terminate();

	m_threadControlIOSignal.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControlIOSignal.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControlIOSignal.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

	HANDLE hInspThread = ::AfxBeginThread(ThreadIoSignal, LPVOID(this), THREAD_PRIORITY_NORMAL)->m_hThread;
	::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &m_threadControlIOSignal.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
}

UINT CSyncIODlg::ThreadIoSignal(LPVOID pParam)
{
	return ((CSyncIODlg*)pParam)->ThreadIoSignal();
}

UINT CSyncIODlg::ThreadIoSignal()
{
	//::SetEvent(m_threadControlIOSignal.m_Signal_Ready);
	::SetEvent(m_threadControlIOSignal.m_Signal_Start);

	HANDLE pHandles[] = { m_threadControlIOSignal.m_Signal_Kill, m_threadControlIOSignal.m_Signal_Start };
	const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);
	IPVM::TimeCheck TIme;
	DWORD dwWaitResult;
	//long fTime = GetTickCount64();
	while (1)
	{
		dwWaitResult = ::WaitForMultipleObjects(nSignalCount, pHandles, FALSE, 10);
		//if ((WAIT_OBJECT_0 + 1) == ::WaitForMultipleObjects(nSignalCount, pHandles, FALSE, 15))
		if(WAIT_TIMEOUT == dwWaitResult)
		{
			
		//	CSingleLock lock(&m_syncBoard->m_csInput, TRUE);

			if (m_syncBoard->GetSignal_HandlerInput())
			{
				m_syncBoard->GetSignal_HandlerOutput();
				//m_nGetInputCount++;

				//if (m_nGetInputCount > 1999)
				//{
				//	m_nGetInputCount = 0;

				//	int nDrivingTime = m_syncBoard->m_drivingTime.GetTimeSpan();

				//	CTime time = CTime::GetCurrentTime();
				//	CString filePath;
				//	filePath.Format(_T("C:\\Intekplus\\IBLI\\Log\\Driving Time\\%04d%02d%02d.txt"), time.GetYear(), time.GetMonth(), time.GetDay());
				//	CStdioFile file;
				//	if (file.Open(filePath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate))
				//	{
				//		file.SeekToEnd();
				//		CString str;
				//		str.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d] %d\r\n"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), nDrivingTime);
				//		file.WriteString(str);
				//		file.Close();
				//	}
				////	m_syncBoard->m_loggerDrivingTime.Log(0, _T("%d"), m_syncBoard->m_drivingTime);
				//}
			}

			//if (!m_syncBoard->m_bCellCountRead)
			//{
			//	BYTE cellCount = m_syncBoard->GetCellCountValue();
			//	if (cellCount > 0)
			//	{
			//		m_syncBoard->GetTabOffset();

			//		m_syncBoard->m_bCellCountRead = TRUE;
			//		//	{
			//		//		if (m_syncBoard->m_nTabOffset[0] > 0 && m_syncBoard->m_nTabOffset[1] > 0)
			//		m_logger.Log(0, _T("[Input] *GetCellCount* %d *GetTabOffset* %d %d"), cellCount, m_syncBoard->m_nTabOffset/*, m_syncBoard->m_nTabOffset[1]*/);
			//		//	}
			//	}
			//}
			//while (true)
			//{
			//	if (15.0f < TIme.Elapsed_ms())
			//		break;
			//}
			TIme.Reset();
			//::SetEvent(m_threadControlIOSignal.m_Signal_Ready);
		}
		else if(WAIT_OBJECT_0== dwWaitResult)
			break;
	}

	return 0;
}


void CSyncIODlg::ThreadStartCheckState()
{
	m_threadControlCheckState.Terminate();

	m_threadControlCheckState.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControlCheckState.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControlCheckState.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

	HANDLE hInspThread = ::AfxBeginThread(ThreadCheckState, LPVOID(this))->m_hThread;
	::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &m_threadControlCheckState.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
}


UINT CSyncIODlg::ThreadCheckState(LPVOID pParam)
{
	return ((CSyncIODlg*)pParam)->ThreadCheckState();
}


UINT CSyncIODlg::ThreadCheckState()
{
	::SetEvent(m_threadControlCheckState.m_Signal_Ready);

	HANDLE pHandles[] = { m_threadControlCheckState.m_Signal_Kill, m_threadControlCheckState.m_Signal_Start };
	const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

	while (1)
	{
		if ((WAIT_OBJECT_0 + 1) == ::WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE))
		{
			//TRACE(_T("Thread Check State Start\n"));
			if (false == m_bisHide)
			{
                m_checkInput[0]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: INSPECTION_RUN_ON));
                m_checkInput[1]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: CYL1_FWD_CHK_SENSOR));
                m_checkInput[2]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: NONE_3));
                m_checkInput[3]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: NONE_4));
                m_checkInput[4]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: NONE_5));
                m_checkInput[5]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: NONE_6));
                m_checkInput[6]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: NONE_7));
                m_checkInput[7]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: NONE_8));
                m_checkInput[8]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: NONE_9));
                m_checkInput[9]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput:: ENC_FW_CHK_SENSOR));
                m_checkInput[10]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::NONE_11));
                m_checkInput[11]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::NONE_12));
                m_checkInput[12]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::NONE_13));
                m_checkInput[13]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::NONE_14));
                m_checkInput[14]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::NONE_15));
                m_checkInput[15]->SetCheck(m_syncBoard->GetInputBit(SyncboardInput::NONE_16));
				m_checkOutput[0]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::VisionRunEnable));
                m_checkOutput[1]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::Illumination));
				m_checkOutput[2]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::Reserved));
				m_checkOutput[3]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultEnable));
				m_checkOutput[4]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResult));
				m_checkOutput[5]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::Marking1));
				m_checkOutput[6]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::Marking2));
				m_checkOutput[7]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::VisionAlarm));
				m_checkOutput[8]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultID0));
				m_checkOutput[9]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultID1));
				m_checkOutput[10]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultID2));
				m_checkOutput[11]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultID3));
				m_checkOutput[12]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultID4));
				m_checkOutput[13]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultID5));
				m_checkOutput[14]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::CellResultID5));
				m_checkOutput[15]->SetCheck(m_syncBoard->GetOutputBit(SyncboardOutput::ConnectionTape_or_NoTab));
			}
			//TRACE(_T("Thread Check State Finish\n"));

			::SetEvent(m_threadControlCheckState.m_Signal_Ready);
		}
		else
			break;
	}

	return 0;
}

void CSyncIODlg::ThreadStartGetCellCountValue()
{
	m_threadControlCellCountValue.Terminate();

	m_threadControlCellCountValue.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControlCellCountValue.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadControlCellCountValue.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

	HANDLE hInspThread = ::AfxBeginThread(ThreadGetCellCountValue, LPVOID(this))->m_hThread;
	::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &m_threadControlCellCountValue.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
}


UINT CSyncIODlg::ThreadGetCellCountValue(LPVOID pParam)
{
	return ((CSyncIODlg*)pParam)->ThreadGetCellCountValue();
}


UINT CSyncIODlg::ThreadGetCellCountValue()
{
	::SetEvent(m_threadControlCellCountValue.m_Signal_Ready);

	HANDLE pHandles[] = { m_threadControlCellCountValue.m_Signal_Kill, m_threadControlCellCountValue.m_Signal_Start };
	const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

	while (1)
	{
		if ((WAIT_OBJECT_0 + 1) == ::WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE))
		{
			BYTE cellCount = m_syncBoard->GetCellCountValue();
			if (cellCount > 0)
			{
				KillTimer(2);

				m_syncBoard->GetTabOffset();
			//	{
			//		if (m_syncBoard->m_nTabOffset[0] > 0 && m_syncBoard->m_nTabOffset[1] > 0)
						m_logger.Log(0, _T("[Input] *GetCellCount* %d *GetTabOffset* %d"), cellCount, m_syncBoard->m_nTabOffset/*, m_syncBoard->m_nTabOffset[1]*/);
			//	}
			}

			::SetEvent(m_threadControlCellCountValue.m_Signal_Ready);
		}
		else
			break;
	}

	return 0;
}



void CSyncIODlg::OnBnClickedCheckOutput(UINT module)
{
	long port = module - IDC_CHECK_OUTPUT_1;

	m_bOutputValue[port] = !m_bOutputValue[port];
	m_syncBoard->SetSignal_HandlerOutput((SyncboardOutput)port, !m_syncBoard->GetOutputBit((SyncboardOutput)port));

	m_checkOutput[port]->SetCheck(m_bOutputValue[port]);

	{
		CSingleLock lock(&m_cs, TRUE);

		if (m_syncBoard->GetHandleDevice() == INVALID_HANDLE_VALUE)
		{
			return;
		}

		BYTE Value = 0;
		BYTE signal = 1 << (port % 8);

		if (m_bOutputValue[port])
		{
			if (port < 8)
			{
				BOOL res = SyncPci_ReadHandlerOut(m_syncBoard->GetHandleDevice(), 0, &Value);
				Value = Value | signal/*0xFE*/;	// 1111 1110
				//Value = Value & signal/*0xFE*/;	// 1111 1110
				SyncPci_WriteHandlerIo(m_syncBoard->GetHandleDevice(), 0, Value);

				m_logger.Log(0, _T("[Manual IO] Port %d On"), port);
			}
			else
			{
				BOOL res = SyncPci_ReadHandlerOut(m_syncBoard->GetHandleDevice(), 1, &Value);
				Value = Value | signal/*0xFE*/;	// 1111 1110
				//Value = Value & signal/*0xFE*/;	// 1111 1110
				SyncPci_WriteHandlerIo(m_syncBoard->GetHandleDevice(), 1, Value);

				m_logger.Log(0, _T("[Manual IO] Port %d On"), port);
			}
		}
		else//Off
		{
			if (port < 8)
			{
				BOOL res = SyncPci_ReadHandlerOut(m_syncBoard->GetHandleDevice(), 0, &Value);
				signal = ~signal;
				Value = Value & signal/*0x01*/;	// 0000 0001
				//Value = Value | signal/*0x01*/;	// 0000 0001
				SyncPci_WriteHandlerIo(m_syncBoard->GetHandleDevice(), 0, Value);

				m_logger.Log(0, _T("[Manual IO] Port %d Off"), port);
			}
			else
			{
				BOOL res = SyncPci_ReadHandlerOut(m_syncBoard->GetHandleDevice(), 1, &Value);
				signal = ~signal;
				Value = Value & signal/*0x01*/;	// 0000 0001
				//Value = Value | signal/*0x01*/;	// 0000 0001
				SyncPci_WriteHandlerIo(m_syncBoard->GetHandleDevice(), 1, Value);

				m_logger.Log(0, _T("[Manual IO] Port %d Off"), port);
			}
		}
	}
}

void CSyncIODlg::OnBnClickedCheckCommand(UINT module)
{
	long port = module - IDC_CHECK_COMMAND_1;

	BYTE ADDR, DATA;
	CString command;
	switch (port)
	{
	case 0:
		ADDR = 176;
		DATA = 240;
		command.Format(_T("INLIND_MODE"));
		break;
	case 1:
		ADDR = 176;
		DATA = 16;
		command.Format(_T("LIVE_SYNC_TRIGGER"));
		break;
	default:;
	}


	{
		CSingleLock lock(&m_cs, TRUE);

		if (m_syncBoard->GetHandleDevice() == INVALID_HANDLE_VALUE)
		{
			return;
		}

		m_bCommandValue[port] = !m_bCommandValue[port];
		m_syncBoard->Download(ADDR, DATA);
		m_checkCommand[port]->SetCheck(m_bCommandValue[port]);

		if (m_bCommandValue[port])
		{
			m_logger.Log(0, _T("[Manual IO] Command %s On"), command);
		}
		else//Off
		{
			m_logger.Log(0, _T("[Manual IO] Command %s Off"), command);
		}
	}
}