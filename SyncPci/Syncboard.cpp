#include "StdAfx.h"
#include "resource.h"
#include "Syncboard.h"
#include "SyncPciDll.h"
#include "SyncIODlg.h"
#include "CommDef.h"
#include "../CompanySharedLibrary/Persistence/IniHelper.h"
#include "../Module/CustomControl/CustomCheckBox.h"
//#include "../SharedModule/VisionHostShare/basedef.h"

#define IO_DELAY	3

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection g_cs;

class CSingleLog
{
public:
	CSingleLog(char *szLog)
	{
	}
};

/*
class CSingleLog
{
public:
	CSingleLog(char *szLog)
		: m_dwThread(GetCurrentThreadId())
		, m_strLog(szLog)
	{
		CSingleLock lock(&g_cs, TRUE);

		FILE *stream = nullptr;
		::fopen_s(&stream, "C:\\SyncLog.txt", "a");

		if (stream)
		{
			::fseek(stream, 0, SEEK_END);
			::fprintf(stream, "%05d IN  : %s\n", m_dwThread, m_strLog.GetBuffer());
			::fclose(stream);
		}
	}

	~CSingleLog()
	{
		CSingleLock lock(&g_cs, TRUE);

		FILE *stream = nullptr;
		::fopen_s(&stream, "C:\\SyncLog.txt", "a");

		if (stream)
		{
			::fseek(stream, 0, SEEK_END);
			::fprintf(stream, "%05d OUT : %s\n", m_dwThread, m_strLog.GetBuffer());
			::fclose(stream);
		}
	}

private:
	DWORD m_dwThread;
	CStringA m_strLog;
};
*/

static LPCTSTR g_szConfigPathName = _T("C:\\intekplus\\IBLI\\System.ini");

// 주의 : PCI-Sync 의 DIO 는 Active Low 임.

CSyncboard::CSyncboard(IPVM::LoggerInterface& logger, HWND wnd, BOOL bEnabled, int GenerationVersion, std::deque<long>& Trigger, std::deque<long>& Buffer, std::deque<long>& Output)

	: m_hDevice(INVALID_HANDLE_VALUE)
	, m_logger(logger)
	, m_nLotIndex(0)
	, m_nRepeatIndex(0)
	, m_pnDeviceUnitIndexList(nullptr)
	, m_nDeviceUnitIndexCount(0)
	, m_targetInputBegin(0)
	, m_targetInputEnd(0)
	, m_pocketInputBegin(0)
	, m_pocketInputEnd(0)
	, m_fovInputBegin(0)
	, m_fovInputEnd(0)
	, m_targetInputMask(0)
	, m_pocketInputMask(0)
	, m_fovInputMask(0)
	, m_bOpen(FALSE)
	, m_hwnd(wnd)
	, m_GenerationVersion(GenerationVersion)
	, nLastCellID(-1)
	, m_dqTrigger_Window(&Trigger)
	, m_dqBuffer_Window(&Buffer)
	, m_dqOutput_Window(&Output)

{
	m_bMonitoring = FALSE;
	m_bCellCountRead = FALSE;

	m_nAlarmType = NORMAL_STATE;
	m_bAlarmChange = FALSE;

	m_bPrevByPass = FALSE;
	m_bPrevVisionReady = FALSE;

	m_drivingTime = CTimeSpan(0);

	m_pivotTime = CTime(0);
	m_prevTime = CTime(0);

	m_fTime = 0.0f;
	if (bEnabled)
	{
		m_bOpen = OpenPort();
		m_bMonitoring = TRUE;
	}

	for (long i = 0; i < (long)SyncboardInput::END; i++)
	{
		m_bCurHandlerInput[i] = FALSE;
	}
	for (long i = 0; i < (long)SyncboardOutput::END; i++)
	{
		m_bCurHandlerOutput[i] = FALSE;
	}

	m_nTabOffset = -1;
//	m_nTabOffset[1] = -1;

	const long targetInputCount = IniHelper::LoadINT(g_szConfigPathName, _T("SYNC_IO_INTERFACE"), _T("TARGET_INPUT_COUNT"), 2);
	const long pocketInputCount = IniHelper::LoadINT(g_szConfigPathName, _T("SYNC_IO_INTERFACE"), _T("POCKET_INPUT_COUNT"), 6);
	const long fovInputCount = IniHelper::LoadINT(g_szConfigPathName, _T("SYNC_IO_INTERFACE"), _T("FOV_INPUT_COUNT"), 4);

	SetIoInterface(targetInputCount, pocketInputCount, fovInputCount);

	//sync init
	//모든값을 
	m_IoThread = NULL;
	m_DlgSyncIO = NULL;
	m_logList = NULL;

	//SetSignal_VisionReady(TRUE);
	//SetSignal_VisionAcquisition(TRUE);

	m_bClearChangeOn = false;

	// Sync I/O UI 만들기
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_DlgSyncIO = new CSyncIODlg(m_logger, m_GenerationVersion, this);
	GetIoInterface(m_DlgSyncIO->m_targetIoCount, m_DlgSyncIO->m_pocketIoCount, m_DlgSyncIO->m_fovIoCount);
	BOOL bSuccess = m_DlgSyncIO->Create(CSyncIODlg::IDD);
	m_logList = &m_DlgSyncIO->m_listLogIO;
}

CSyncboard::~CSyncboard(void)
{
//	TurnOffLight();

	ClosePort();

	delete [] m_pnDeviceUnitIndexList;
	m_logList = NULL;
	delete m_logList;

	if (m_DlgSyncIO)
		delete m_DlgSyncIO;
}

void CSyncboard::SetMeasureMode(BOOL bMoireMeasure)
{
	CSingleLock lock(&m_cs, TRUE);
	
	CSingleLog log(__FUNCTION__);

	Download(SET_MEASURE_MODE, (BYTE)bMoireMeasure);
}

void CSyncboard::SetIllumControllerType(BYTE value)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(ILLUM_CONTROLLER_TYPE, value);
}

void CSyncboard::Set2DLedIntensity(long nFrame, const BYTE pbyIntensity[16])
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	BYTE byAddress = BYTE(nFrame << 4);

	if (byAddress >= 0xa0)	// 내장 싱크 용량 제한으로 10장 사용 가능
	{
		return;
	}

	for (long nChannel = 0; nChannel < 16; nChannel++)
	{
		Download(BYTE(byAddress + nChannel), BYTE(pbyIntensity[nChannel]));
	}
}

void CSyncboard::Set2DCurrentFrame(long nFrame)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(ILLUM_COMMAND, BYTE(nFrame + COMMAND_2D_0));
}

void CSyncboard::Set2DFrameCount(long nFrameCount)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(ILL_2D_COUNT, BYTE(nFrameCount));
}

void CSyncboard::Set2DIllumSel(BYTE value)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(SET_2D_ILLUM_SEL, value);
}

void CSyncboard::Set2DSelMode(BYTE value)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(ILL_2D_SET_MODE, value);
}

void CSyncboard::Make2DLookupTable()
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(ILLUM_COMMAND, COMMAND_MAKE_2D_LOOKUP_TABLE);

	Sleep(10);
}

void CSyncboard::Make2DIllumTable()
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(ILLUM_COMMAND, COMMAND_MAKE_2D_ILLUM_TABLE);

	Sleep(10);
}

BOOL CSyncboard::GetInputBit(SyncboardInput inputType) const
{
	return m_bCurHandlerInput[(long)inputType];
}

BOOL CSyncboard::GetOutputBit(SyncboardOutput outputType) const
{
	return m_bCurHandlerOutput[(long)outputType];
}

BOOL CSyncboard::GetOutputBitFromRealRead(SyncboardOutput outputType)
{
	CSingleLock lock(&m_cs, TRUE);

	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	BYTE Value, Value_1;

	SyncPci_ReadHandlerOut(GetHandleDevice(), 0, &Value);
	SyncPci_ReadHandlerOut(GetHandleDevice(), 1, &Value_1);

	BOOL bOn[(long)SyncboardOutput::END];

	bOn[0] = ((Value & 0x01) != 0x00);
	bOn[1] = ((Value & 0x02) != 0x00);
	bOn[2] = ((Value & 0x04) != 0x00);
	bOn[3] = ((Value & 0x08) != 0x00);
	bOn[4] = ((Value & 0x10) != 0x00);
	bOn[5] = ((Value & 0x20) != 0x00);
	bOn[6] = ((Value & 0x40) != 0x00);
	bOn[7] = ((Value & 0x80) != 0x00);

	bOn[8] = ((Value_1 & 0x01) != 0x00);
	bOn[9] = ((Value_1 & 0x02) != 0x00);
	bOn[10] = ((Value_1 & 0x04) != 0x00);
	bOn[11] = ((Value_1 & 0x08) != 0x00);
	bOn[12] = ((Value_1 & 0x10) != 0x00);
	bOn[13] = ((Value_1 & 0x20) != 0x00);
	bOn[14] = ((Value_1 & 0x40) != 0x00);
	bOn[15] = ((Value_1 & 0x80) != 0x00);

	return bOn[(long)outputType];
}

void CSyncboard::SetFramePeriod(float fFramePeriod_ms)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	if(fFramePeriod_ms > 200.f)
	{
		fFramePeriod_ms = 200.f;
	}

	WORD nClk = (WORD)(fFramePeriod_ms / 0.0032f + 0.5f);			// 3.2us
	Download(SET_TIME_FRAME_PERIOD_H, nClk >> 8);
	Download(SET_TIME_FRAME_PERIOD_L, nClk & 0x00ff);
}

void CSyncboard::SetExposureTime(float exposureTime_ms)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	if(exposureTime_ms > 200.f)
	{
		exposureTime_ms = 200.f;
	}

	WORD nClk = (WORD)(exposureTime_ms / 0.0032f + 0.5f);			// 3.2us
	Download(SET_TIME_HIGH_DURATION_H, nClk >> 8);
	Download(SET_TIME_HIGH_DURATION_L, nClk & 0x00ff);
}

void CSyncboard::SetIlluminationDelay(float fIlluminationDelay_ms)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	if(fIlluminationDelay_ms > 200.f)
	{
		fIlluminationDelay_ms = 200.f;
	}

	WORD nClk = (WORD)(fIlluminationDelay_ms / 0.0032f + 0.5f);			// 3.2us
	Download(SET_TIME_ILLUMINATION_H, nClk >> 8);
	Download(SET_TIME_ILLUMINATION_L, nClk & 0x00ff);
}

BOOL CSyncboard::StartSyncboard()
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	BOOL bRet = FALSE;
    bRet = Download(ILLUM_COMMAND, MEASURE_START);

	return bRet;
}

BOOL CSyncboard::LiveStartSyncboard()
{
    CSingleLock lock(&m_cs, TRUE);

    CSingleLog log(__FUNCTION__);

    if (m_hDevice == INVALID_HANDLE_VALUE)
    {
        return TRUE;
    }

    BOOL bRet = FALSE;
    bRet = Download(ILLUM_COMMAND, COMMAND_2D_0);

    return bRet;
}

void CSyncboard::TurnOffLight()
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(ILLUM_COMMAND, COMMAND_CLEAR);

	Sleep(10);
}

BOOL CSyncboard::OpenPort()
{
	CSingleLock lock(&m_cs, TRUE);

	if (SyncPci_LoadDll())
	{
		m_hDevice = SyncPci_LoadDevice(0);

		if (m_hDevice == INVALID_HANDLE_VALUE)
		{
			::AfxMessageBox(_T("Failed to load Device[PCI Syncboard]!"));
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		::AfxMessageBox(_T("SyncPci.dll file Not Found. [PCI Syncboard]!"));
		return FALSE;
	}

	return TRUE;
}

void CSyncboard::ClosePort()
{
	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice != INVALID_HANDLE_VALUE)
	{
		SyncPci_UnloadDevice();
		SyncPci_UnloadDll();
		m_hDevice = INVALID_HANDLE_VALUE;
	}
}

void CSyncboard::ShowDIOCheckDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_DlgSyncIO == NULL)
	{
		m_DlgSyncIO = new CSyncIODlg(m_logger, m_GenerationVersion, this);
		GetIoInterface(m_DlgSyncIO->m_targetIoCount, m_DlgSyncIO->m_pocketIoCount, m_DlgSyncIO->m_fovIoCount);
		m_DlgSyncIO->Create(CSyncIODlg::IDD);
		m_logList = &m_DlgSyncIO->m_listLogIO;
	//	m_logger.AppendSlaveLogger(&(m_DlgSyncIO->m_listLogIO));
	}

	for (int i = 0; i < 8; i++)
	{
		m_DlgSyncIO->m_checkOutput[i]->SetCheck(m_DlgSyncIO->m_bOutputValue[i]);
	}

	m_DlgSyncIO->ShowWindow(SW_SHOW);
	m_DlgSyncIO->m_bisHide = false;
	//(new CDlgVisionDIOCheck(*this))->Create(CDlgVisionDIOCheck::IDD);
}

void CSyncboard::StartGetTabOfssetTimer()
{
	m_DlgSyncIO->SetTimer(2, 10, NULL);
}

BOOL CSyncboard::GetTabOffset()
{
	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	CSingleLock lock(&m_cs, TRUE);

	BYTE /*BiCellValue1, BiCellValue2, */Value1, Value2/*, Value3, Value4*/;

	//if (!SyncPci_Read(m_hDevice, 1, 6, &BiCellValue1))
	//	return FALSE;
	//if (!SyncPci_Read(m_hDevice, 1, 7, &BiCellValue2))
	//	return FALSE;
	if (!SyncPci_Read(m_hDevice, 1, 12, &Value1))
		return FALSE;
	if (!SyncPci_Read(m_hDevice, 1, 13, &Value2))
		return FALSE;
	//if (!SyncPci_Read(m_hDevice, 1, 14, &Value3))
	//	return FALSE;
	//if (!SyncPci_Read(m_hDevice, 1, 15, &Value4))
	//	return FALSE;

	m_nTabOffset = Value1 + (Value2 * 256);
	if (m_nTabOffset > 0)
	{
		//TRACE(_T("break\n"));
	}
	//if (m_nTabOffset[1] > 0)
	//{
	//	//TRACE(_T("break\n"));
	//}
	//m_nTabOffset[1] = Value3 + (Value4 * 256);

	return TRUE;
}

BYTE CSyncboard::GetCellCountValue()
{
	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	CSingleLock lock(&m_cs, TRUE);

//	SyncPci_ReadHandlerOut(m_hDevice, 8, &Value);

	BYTE byRecValue = 0;

	if (!SyncPci_Read(m_hDevice, 1, 8, &byRecValue))
		return 0;

	int ivalue = byRecValue;

	if (byRecValue > 0)
	{
		//TRACE(_T("Test"));
	}
//	//TRACE(_T("%d\n"), ivalue);
//	m_logger.Log(0, _T("[Input] Cell Count Value %d"), ivalue);
	
	return byRecValue;
}

void CSyncboard::WriteIoLog(LPCTSTR str, ...)
{
	//CString strLog;

	//va_list argptr;
	//va_start(argptr, str);
	//strLog.FormatV(str, argptr);
	//va_end(argptr);

//	m_logger.Log(0, str);
	//CSingleLock lock(&m_csLog, TRUE);

	//CString strLog;

	//va_list argptr;
	//va_start(argptr, str);
	//strLog.FormatV(str, argptr);
	//va_end(argptr);

	//SYSTEMTIME st;
	//GetLocalTime(&st);

	//CString strWrite;
	//strWrite.Format(_T("[%02d:%02d:%02d.%03d] "), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	//strWrite += strLog;
	//strWrite += _T("\n");

	//CString logPath;
	//logPath.Format(_T("C:\\intekplus\\IBLI\\Log\\%04d%02d%02d_Log_IO.txt"), st.wYear, st.wMonth, st.wDay);;

	//CStdioFile file;

	//if (file.Open(logPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	//{
	//	file.SeekToEnd();
	//	file.WriteString(strWrite);
	//	file.Close();
	//}

	//if (m_DlgSyncIO)
	//{
	//	if (m_DlgSyncIO->m_listLogIO.GetCount() > 2000)
	//	{
	//		m_DlgSyncIO->m_listLogIO.DeleteString(1999);
	//	}

	//	m_DlgSyncIO->m_listLogIO.InsertString(0, strWrite);

	//	m_DlgSyncIO->m_listLogIO.SetCurSel(0);
	//}
}

BOOL CSyncboard::Download(BYTE byAddress, BYTE byData)
{
	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	BOOL bRet = FALSE;
	while (!bRet)
	{
		bRet = SyncPci_WriteAvrParam(m_hDevice, byAddress, byData);
		//TRACE(_T("Download\n"));
		Sleep(1);
	}

	byte wr_addr, wr_data;

	while (1)
	{
		SyncPci_Read(m_hDevice, 0, 0, &wr_addr);
		SyncPci_Read(m_hDevice, 0, 1, &wr_data);

		if ((wr_addr == byAddress) && (wr_data == byData))
		{
			break;
		}

		SyncPci_WriteAvrParam(m_hDevice, byAddress, byData);
		Sleep(1);
	}

	return TRUE;
}

BYTE CSyncboard::GetMaxFrameCount()
{
	return 10;
}

BYTE CSyncboard::GetMaxChannelCount()
{
	return 16;
}

void CSyncboard::SetUse3DChannel(long nCh, BOOL bOn)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(BYTE(USE_3D_CHANNEL_0 + nCh), BYTE(bOn));
}

void CSyncboard::Set3DMultibandNum(long nNum)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(SET_CHANNEL_0_MULTIBAND_NUM, (BYTE)nNum);
}

void CSyncboard::Set3DDualSwitch(BOOL bUse)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download(SET_DUAL_GRAB, (BYTE)bUse);
}

void CSyncboard::Set3DLedIntensity(long nCh, long nBand, float fDuration_us)
{
	ASSERT(nBand >= 0 && nBand < 16);

	// Unit Pulse Length : 0.4us
	const float maxDuration_us = 4095 * 0.4f;

	if(fDuration_us > maxDuration_us)
	{
		fDuration_us = maxDuration_us;
	}

	unsigned short usDigit = max(1, (unsigned short)(fDuration_us / 0.4f + 0.5f));

	BYTE byLSB = BYTE(0x00ff & usDigit);
	BYTE byMSB = BYTE((0xff00 & usDigit) >> 8);

	switch(nCh)
	{
	case 0:
		Download(BYTE(ILLUM_3D_CHANNEL_0_MB0_VALUE_LSB + nBand), byLSB);
		Download(BYTE(ILLUM_3D_CHANNEL_0_MB0_VALUE_MSB + nBand), byMSB);
		break;
	case 1:
		Download(BYTE(ILLUM_3D_CHANNEL_0_MB1_VALUE_LSB + nBand), byLSB);
		Download(BYTE(ILLUM_3D_CHANNEL_0_MB1_VALUE_MSB + nBand), byMSB);
		break;
	default:
		ASSERT(!_T("Channel 그런거 없는데?"));
	}
}

void CSyncboard::Set3DFrameCount(long nCh, long nBand, WORD n3DFrameCount)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	Download((BYTE)(ILL_3D_MB0_COUNT_H + nBand), n3DFrameCount >> 8);
	Download((BYTE)(ILL_3D_MB0_COUNT_L + nBand), n3DFrameCount & 0x00ff);
}

void CSyncboard::Set3DCurrentFrame(long nCh, long nBand)
{
	CSingleLock lock(&m_cs, TRUE);

	Download(ILLUM_COMMAND, BYTE(nCh * 0x10 + COMMAND_3D_0 + nBand));
}

// DIO by pci-sync - sjj wsi {{
void CSyncboard::SetSignal_VisionReady(BOOL bNewValue)
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return;
	}

	BYTE Value;
	if (bNewValue)
	{
		SyncPci_ReadHandlerOut(m_hDevice, 0, &Value);
		Value = Value & 0xFE;	// 1111 1110
		SyncPci_WriteHandlerIo(m_hDevice, 0, Value);

		WriteIoLog(_T("[Output] Vision Ready Signal On"));
	}
	else//Off
	{
		SyncPci_ReadHandlerOut(m_hDevice, 0, &Value);
		Value = Value | 0x01;	// 00000001
		SyncPci_WriteHandlerIo(m_hDevice, 0, Value);

		WriteIoLog(_T("[Output] Vision Ready Signal Off"));
	}
}

BOOL CSyncboard::GetSignal_VisionReady()
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BYTE Value = 0x00;
	SyncPci_ReadHandlerOut(m_hDevice, 0, &Value);
	Value = Value & 0x01;

	if (Value == 0x01)
	{
		return FALSE;//준비 안됨
	}
	else
	{
		return TRUE;
	}
}

void CSyncboard::SetSignal_VisionAcquisition(BOOL bNewValue)
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return;
	}

	BYTE Value;
	if (bNewValue)
	{
		SyncPci_ReadHandlerOut(m_hDevice, 0, &Value);
		Value = Value & 0xFD;	// 00000010
		SyncPci_WriteHandlerIo(m_hDevice, 0, Value);
	}
	else //Off
	{
		SyncPci_ReadHandlerOut(m_hDevice, 0, &Value);
		Value = Value | 0x02;	// 00000010
		SyncPci_WriteHandlerIo(m_hDevice, 0, Value);
	}
}


//이건 Grab이후에 핸들러에게 알려주는 I/O
BOOL CSyncboard::GetSignal_VisionAcquisition()
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bRet;
	BYTE Value = 0x00;

	SyncPci_ReadHandlerOut(m_hDevice, 0, &Value);

	Value = Value & 0x02;	// 0000 0010

	if (Value == 0x02)
		return FALSE;
	else
		return TRUE;

}

BOOL CSyncboard::GetSignal_HandlerMoveReady()
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BYTE byRecValue = 0;

	SyncPci_ReadHandler(m_hDevice, 0, &byRecValue);
	byRecValue = byRecValue & 0x20;

	if (byRecValue == 0x20)
	{
		WriteIoLog(_T("[Input] Move Ready Off"));

		return FALSE;			// 0010 0000
	}
	else
	{
		WriteIoLog(_T("[Input] Move Ready On"));

		return TRUE;
	}
}

BOOL CSyncboard::GetSignal_HandlerInput()
{
	//TRACE(_T("Get Input Start\n"));

//	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	BYTE byRecValue = 0, byRecValue_1 = 0, byAlarmClass = 0, byCellID = 0;

	//SyncPci_ReadHandler(m_hDevice, 1, &byRecValue);
	::GetLocalTime(&m_Time);
	float time =  m_Time.wMilliseconds - m_fTime;
	m_fTime = m_Time.wMilliseconds;
	if(30.0<time)
		m_logger.Log(0, _T("SynRead Time - %.2f ms "), time);

	BOOL res = SyncPci_ReadHandlerIo(m_hDevice, 0, &byRecValue);
	Sleep(1);
	BOOL res_1 = SyncPci_ReadHandlerIo(m_hDevice, 1, &byRecValue_1);

	byRecValue = ~byRecValue;
	byCellID = byRecValue_1 & 0x3f;
	byRecValue_1 = ~byRecValue_1;

	SyncPci_Read(m_hDevice, 1, 5, &byAlarmClass);
	bool bOn[(long)SyncboardInput::END];

	for (int i = 0; i < (long)SyncboardInput::END; i++)
	{
		bOn[i] = FALSE;
	}

	bOn[0] = ((byRecValue & 0x01) == 0x00);
	bOn[1] = ((byRecValue & 0x02) == 0x00);
	bOn[2] = ((byRecValue & 0x04) == 0x00);
	bOn[3] = ((byRecValue & 0x08) == 0x00);
	bOn[4] = ((byRecValue & 0x10) == 0x00);
	bOn[5] = ((byRecValue & 0x20) == 0x00);
	bOn[6] = ((byRecValue & 0x40) == 0x00);
	bOn[7] = ((byRecValue & 0x80) == 0x00);

	bOn[8] = ((byRecValue_1 & 0x01) == 0x00);
	bOn[9] = ((byRecValue_1 & 0x02) == 0x00);
	bOn[10] = ((byRecValue_1 & 0x04) == 0x00);
	bOn[11] = ((byRecValue_1 & 0x08) == 0x00);
	bOn[12] = ((byRecValue_1 & 0x10) == 0x00);
	bOn[13] = ((byRecValue_1 & 0x20) == 0x00);
	bOn[14] = ((byRecValue_1 & 0x40) == 0x00);
	bOn[15] = ((byRecValue_1 & 0x80) == 0x00);

	//for (int i = 0; i < (long)SyncboardInput::END; i++)
	//{
	//	bOn[i] = !bOn[i];
	//}

//	byRecValue = byRecValue & 0x20;

	CString logString;
	for (long i = 0; i < (long)SyncboardInput::END; i++)
	{
		if (m_bCurHandlerInput[i] != bOn[i])
		{
			m_bCurHandlerInput[i] = bOn[i];

			switch ((SyncboardInput)i)
			{
			case SyncboardInput::INSPECTION_RUN_ON:
				if (m_bCurHandlerInput[i])
				{
					m_logger.Log(0, _T("[Input] INSPECTION_RUN_ON On"));
					//int byRemainCount = (int)GetIoCount();
					//m_logger.Log(0, _T("Sync Remain Count : %d"), byRemainCount);
				}
				else
				{
					m_logger.Log(0, _T("[Input] INSPECTION_RUN_ON Off"));
					//int byRemainCount = (int)GetIoCount();
					//m_logger.Log(0, _T("Sync Remain Count : %d"), byRemainCount);
				}
				break;
			case SyncboardInput::CYL1_FWD_CHK_SENSOR:
				if (m_bCurHandlerInput[i])
				{
					//m_startOnCount[0]++;
					//CheckMovingAbnormal();
					m_logger.Log(0, _T("[Input] CYL1_FWD_CHK_SENSOR On"));
					//PostMessage(m_hwnd, WM_ELECTRODE_MOVE_START, NULL, NULL);
				}
				else
				{
					//CheckMovingAbnormal();
					m_logger.Log(0, _T("[Input] CYL1_FWD_CHK_SENSOR Off"));
					//PostMessage(m_hwnd, WM_ELECTRODE_MOVE_STOP, NULL, NULL);
				}
				break;
			case SyncboardInput::ENC_FW_CHK_SENSOR:
				if (m_bCurHandlerInput[i])
				{
					m_logger.Log(0, _T("[Input] ENC_FW_CHK_SENSOR On"));
				}
				else
				{
					m_logger.Log(0, _T("[Input] ENC_FW_CHK_SENSOR Off"));
				}
				break;
			case SyncboardInput::NONE_3:
			case SyncboardInput::NONE_4:
			case SyncboardInput::NONE_5:
			case SyncboardInput::NONE_6:
			case SyncboardInput::NONE_7:
			case SyncboardInput::NONE_8:
			case SyncboardInput::NONE_9:
			case SyncboardInput::NONE_11:
			case SyncboardInput::NONE_12:
			case SyncboardInput::NONE_13:
			case SyncboardInput::NONE_14:
			case SyncboardInput::NONE_15:
			case SyncboardInput::NONE_16:
				if (m_bCurHandlerInput[i])
				{
					logString.Format(_T("[Input] NONE_%d On"), i);
					m_logger.Log(0, logString);
				}
				else
				{
					logString.Format(_T("[Input] NONE_%d Off"), i);
					m_logger.Log(0, logString);
				}
				break;
			}
		}
	}

	if (true)   // todo:2024.03.29 use sync option
	{
		ReadCellIDBuffer();
		Check_CellID();
	}

	BOOL checks[2] = { false,false };
	CTime curTime = CTime::GetCurrentTime();
	int nHour = curTime.GetHour();

	if (!checks[0] && checks[1])
	{
		if (m_bPrevByPass != checks[0] || m_bPrevVisionReady != checks[1])
		{
			m_pivotTime = curTime;
		}
	}
	m_bPrevByPass = checks[0];
	m_bPrevVisionReady = checks[1];

	if (nHour == 0 && m_prevTime.GetDay() != curTime.GetDay())
	{
		if (m_pivotTime.GetTime() > 0)
		{
			CTimeSpan timeSpan;
			timeSpan = curTime - m_pivotTime;
			m_drivingTime = timeSpan;
		}
		m_logger.Log(0, _T("Driving Time - %d minutes"), m_drivingTime.GetTotalMinutes());
		m_drivingTime = CTimeSpan(0);
	}
	m_prevTime = curTime;




	//TRACE(_T("Get Input Success\n"));
	return res & res_1;
}

BOOL CSyncboard::GetSignal_HandlerOutput()
{

	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	BYTE Value, Value_1;

	CSingleLock lock(&m_cs, TRUE);
	{
		SyncPci_ReadHandlerOut(GetHandleDevice(), 0, &Value);
		SyncPci_ReadHandlerOut(GetHandleDevice(), 1, &Value_1);
	}

	BOOL bOn[(long)SyncboardOutput::END];

	for (int i = 0; i < (long)SyncboardInput::END; i++)
	{
		bOn[i] = FALSE;
	}
	bOn[0] = ((Value & 0x01) != 0x00);
	bOn[1] = ((Value & 0x02) != 0x00);
	bOn[2] = ((Value & 0x04) != 0x00);
	bOn[3] = ((Value & 0x08) != 0x00);
	bOn[4] = ((Value & 0x10) != 0x00);
	bOn[5] = ((Value & 0x20) != 0x00);
	bOn[6] = ((Value & 0x40) != 0x00);
	bOn[7] = ((Value & 0x80) != 0x00);

	bOn[8] = ((Value_1 & 0x01) != 0x00);
	bOn[9] = ((Value_1 & 0x02) != 0x00);
	bOn[10] = ((Value_1 & 0x04) != 0x00);
	bOn[11] = ((Value_1 & 0x08) != 0x00);
	bOn[12] = ((Value_1 & 0x10) != 0x00);
	bOn[13] = ((Value_1 & 0x20) != 0x00);
	bOn[14] = ((Value_1 & 0x40) != 0x00);
	bOn[15] = ((Value_1 & 0x80) != 0x00);


	for (long i = 0; i < (long)SyncboardOutput::END; i++)
	{
		if (m_bCurHandlerOutput[i] != bOn[i])
		{
			m_DlgSyncIO->m_bOutputValue[(long)i] = bOn[i];
			m_bCurHandlerOutput[i] = bOn[i];
			switch ((SyncboardOutput)i)
			{
			case SyncboardOutput::VisionRunEnable:
				if (m_bCurHandlerOutput[i])
					m_logger.Log(0, _T("[Input] VISION RUN ENABLE ON"));
				else
					m_logger.Log(0, _T("[Input] VISION RUN ENABLE OFF"));

				break;
            case SyncboardOutput::Illumination:
                if (m_bCurHandlerOutput[i])
                    m_logger.Log(0, _T("[Input] VISION Illumination ON"));
                else
                    m_logger.Log(0, _T("[Input] VISION Illumination OFF"));

				break;
			case SyncboardOutput::Reserved:
				break;
			case SyncboardOutput::CellResultEnable:
				break;
			case SyncboardOutput::CellResult:
				break;
			case SyncboardOutput::Marking1:
				break;
			case SyncboardOutput::Marking2:
				break;
			case SyncboardOutput::VisionAlarm:
				break;
			case SyncboardOutput::CellResultID0:
				break;
			case SyncboardOutput::CellResultID1:
				break;
			case SyncboardOutput::CellResultID2:
				break;
			case SyncboardOutput::CellResultID3:
				break;
			case SyncboardOutput::CellResultID4:
				break;
			case SyncboardOutput::CellResultID5:
				break;
			case SyncboardOutput::CellResultID6:
				break;
			case SyncboardOutput::ConnectionTape_or_NoTab:
				break;
			case SyncboardOutput::END:
				break;
			default:
				break;
			}
		}
	}
}

BOOL CSyncboard::SetSignal_HandlerOutput(SyncboardOutput outputType, BOOL bSignal)
{
	CSingleLock lock(&m_cs, TRUE);

	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
	{
		//m_DlgSyncIO->m_bOutputValue[(long)outputType] = bSignal;
		//m_bCurHandlerOutput[(long)outputType] = bSignal;
		return TRUE;
	}

	BYTE Value, Value_1;

	SyncPci_ReadHandlerOut(GetHandleDevice(), 0, &Value);
	SyncPci_ReadHandlerOut(GetHandleDevice(), 1, &Value_1);

	BOOL bOn[(long)SyncboardOutput::END];

	//bOn[0] = ((Value & 0x01) == 0x00);
	//bOn[1] = ((Value & 0x02) == 0x00);
	//bOn[2] = ((Value & 0x04) == 0x00);
	//bOn[3] = ((Value & 0x08) == 0x00);
	//bOn[4] = ((Value & 0x10) == 0x00);
	//bOn[5] = ((Value & 0x20) == 0x00);
	//bOn[6] = ((Value & 0x40) == 0x00);
	//bOn[7] = ((Value & 0x80) == 0x00);

	//bOn[8] = ((Value_1 & 0x01) == 0x00);
	//bOn[9] = ((Value_1 & 0x02) == 0x00);
	//bOn[10] = ((Value_1 & 0x04) == 0x00);
	//bOn[11] = ((Value_1 & 0x08) == 0x00);
	//bOn[12] = ((Value_1 & 0x10) == 0x00);
	//bOn[13] = ((Value_1 & 0x20) == 0x00);
	//bOn[14] = ((Value_1 & 0x40) == 0x00);
	//bOn[15] = ((Value_1 & 0x80) == 0x00);
	bOn[0] = ((Value & 0x01) != 0x00);
	bOn[1] = ((Value & 0x02) != 0x00);
	bOn[2] = ((Value & 0x04) != 0x00);
	bOn[3] = ((Value & 0x08) != 0x00);
	bOn[4] = ((Value & 0x10) != 0x00);
	bOn[5] = ((Value & 0x20) != 0x00);
	bOn[6] = ((Value & 0x40) != 0x00);
	bOn[7] = ((Value & 0x80) != 0x00);

	bOn[8] = ((Value_1 & 0x01) != 0x00);
	bOn[9] = ((Value_1 & 0x02) != 0x00);
	bOn[10] = ((Value_1 & 0x04) != 0x00);
	bOn[11] = ((Value_1 & 0x08) != 0x00);
	bOn[12] = ((Value_1 & 0x10) != 0x00);
	bOn[13] = ((Value_1 & 0x20) != 0x00);
	bOn[14] = ((Value_1 & 0x40) != 0x00);
	bOn[15] = ((Value_1 & 0x80) != 0x00);

//	if (nPort != 1)
	{
		//if (bOn[(long)outputType] == bSignal)
		//	return TRUE;
		//재시도해야 하는 경우 있어서 주석처리
	}

	m_DlgSyncIO->m_bOutputValue[(long)outputType] = bSignal;
	m_bCurHandlerOutput[(long)outputType] = bSignal;


	BYTE signal = 0;
	CString strPort;
	strPort.Format(_T("Port %d"), (long)outputType);


	switch (outputType)
	{
	case SyncboardOutput::VisionRunEnable: //1111 1110
		signal = 0xFE;
		strPort.Format(_T("VISION RUN ENABLE"));
		break;
    case SyncboardOutput::Illumination: //1111 1101
		signal = 0xFD;
		//if (Share::GenerationVersion::SplitterRemoved == m_GenerationVersion)
			strPort.Format(_T("Illumination ON or OFF"));
		//else
//			strPort.Format(_T("ALARM RESET"));
		break;
	case SyncboardOutput::Reserved: //1111 1011
		signal = 0xFB;
		//if (Share::GenerationVersion::SplitterRemoved == m_GenerationVersion)
			strPort.Format(_T("ELECTRODE NG"));
		//else
			//strPort.Format(_T("RESERVED"));
		break;
	case SyncboardOutput::CellResultEnable: //1111 0111
		signal = 0xF7;
		strPort.Format(_T("CELL RESULT ENABLE"));
		break;
	case SyncboardOutput::CellResult: //1110 1111
		signal = 0xEF;
		//if (Share::GenerationVersion::SplitterRemoved == m_GenerationVersion)
			strPort.Format(_T("NOTCHING NG"));
		//else
			//strPort.Format(_T("CELL RESULT"));
		break;
	case SyncboardOutput::Marking1: //1101 1111
		signal = 0xDF;
		strPort.Format(_T("MARKING #1"));
		break;
	case SyncboardOutput::Marking2: //1011 1111
		signal = 0xBF;
		strPort.Format(_T("MARKING #2"));
		break;
	case SyncboardOutput::VisionAlarm: //0111 1111
		signal = 0x7F;
		strPort.Format(_T("VISION ALARM"));
		break;
	case SyncboardOutput::CellResultID0: //1111 1110
		signal = 0xFE;
		strPort.Format(_T("CELL RESULT ID0"));
		break;
	case SyncboardOutput::CellResultID1: //1111 1101
		signal = 0xFD;
		strPort.Format(_T("CELL RESULT ID1"));
		break;
	case SyncboardOutput::CellResultID2: //1111 1011
		signal = 0xFB;
		strPort.Format(_T("CELL RESULT ID2"));
		break;
	case SyncboardOutput::CellResultID3: //1111 0111
		signal = 0xF7;
		strPort.Format(_T("CELL RESULT ID3"));
		break;
	case SyncboardOutput::CellResultID4: //1110 1111
		signal = 0xEF;
		strPort.Format(_T("CELL RESULT ID4"));
		break;
	case SyncboardOutput::CellResultID5: //1101 1111
		signal = 0xDF;
		strPort.Format(_T("CELL RESULT ID5"));
		break;
	case SyncboardOutput::CellResultID6: //1011 1111
		signal = 0xBF;
		strPort.Format(_T("CELL RESULT ID6"));
		break;
	case SyncboardOutput::ConnectionTape_or_NoTab: //0111 1111
		signal = 0x7F;
		//if (Share::GenerationVersion::SplitterRemoved == m_GenerationVersion)
			strPort.Format(_T("NOTAB"));
		//else
			//strPort.Format(_T("CONNECTION TAPE OR NOTAB"));
		break;
	//case 15: //0111 1111 1111 1111
	//	signal = 0x7FFF;
	//	break;
	}
	
	signal = ~signal;

//	if (nPort != 1)
	{
		if (m_DlgSyncIO->m_bOutputValue[(long)outputType])
		{
			if ((long)outputType < 8)
			{
				BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), 0, &Value);
				Value = Value | signal/*0xFE*/;	// 1111 1110
				//Value = Value & signal/*0xFE*/;	// 1111 1110
				SyncPci_WriteHandlerIo(GetHandleDevice(), 0, Value);
			}
			else
			{
				BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), 1, &Value_1);
				Value_1 = Value_1 | signal/*0xFE*/;	// 1111 1110
				//Value_1 = Value_1 & signal/*0xFE*/;	// 1111 1110
				SyncPci_WriteHandlerIo(GetHandleDevice(), 1, Value_1);
			}

			//	if (nPort != 15)
			m_logger.Log(0, _T("[Output] %s on %d"), (LPCTSTR)strPort, Value_1);

			//BOOL res = SyncPci_Read(GetHandleDevice(), 1, 11, &Value);
			//Value = Value | signal/*0xFE*/;	// 1111 1110
			//SyncPci_Write(GetHandleDevice(), 1, 11, Value);
		}
		else//Off
		{
			if ((long)outputType < 8)
			{
				BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), 0, &Value);
				signal = ~signal;
				Value = Value & signal/*0x01*/;	// 0000 0001
				//Value = Value | signal/*0x01*/;	// 0000 0001
				SyncPci_WriteHandlerIo(GetHandleDevice(), 0, Value);
			}
			else
			{
				BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), 1, &Value_1);
				signal = ~signal;
				Value_1 = Value_1 & signal/*0x01*/;	// 0000 0001
				//Value_1 = Value_1 | signal/*0x01*/;	// 0000 0001
				SyncPci_WriteHandlerIo(GetHandleDevice(), 1, Value_1);
			}

			//	if (nPort != 15)
			m_logger.Log(0, _T("[Output] %s off %d"), (LPCTSTR)strPort, Value_1);

			//BOOL res = SyncPci_Read(GetHandleDevice(), 1, 11, &Value);
			//signal = ~signal;
			//Value = Value & signal/*0x01*/;	// 0000 0001
			//SyncPci_Write(GetHandleDevice(), 1, 11, Value);
		}
	}
	//else
	//{
	//	m_DlgSyncIO->PostMessage(WM_COMMAND, IDC_CHECK_OUTPUT_2);
	//}

	return TRUE;
}

//BOOL CSyncboard::SetSignal_HandlerOutputJudgement(BOOL bPosOK, BOOL bPosNG, BOOL bNegOK, BOOL bNegNG, BOOL bPosAlarm, BOOL bNegAlarm, int nCellID)
//{
//	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
//	{
//		return FALSE;
//	}
//
//	BYTE signal;
//	BYTE Value;
//	CString strPort;
//
//	//Only 양극 테스트용
//	//bNegOK = TRUE;
//	//bNegNG = FALSE;
//	//Only 음극 테스트용
//	//bPosOK = TRUE;
//	//bPosNG = FALSE;
//
//	//일반
//	if (bPosOK)
//	{
//		strPort.Format(_T("Positive OK"));
//
//		if (bNegOK)
//		{
//			//1110 1011
//			//signal = 0xEB;
//			//1111 1010
//			//1110 0111
//			//1100 0111
//		//	signal = 0xFA;
//		//	signal = 0xE7; 
//		signal = 0xC7;
//		//	SetSignal_HandlerOutput(3, TRUE, FALSE);
//		//	SetSignal_HandlerOutput(4, TRUE, FALSE);
//		//	SetSignal_HandlerOutput(5, FALSE, FALSE);
//			
//			strPort += _T(" Negative OK");
//		}
//		else if (bNegNG)
//		{
//			//1101 1011
//			//signal = 0xDB;
//			//1111 0110
//			//1111 0111
//			//1101 0111
//		//	signal = 0xF6;
//		//	signal = 0xF7; 
//		signal = 0xD7;
//		//	SetSignal_HandlerOutput(3, TRUE, FALSE);
//		//	SetSignal_HandlerOutput(4, FALSE, FALSE);
//		//	SetSignal_HandlerOutput(5, FALSE, FALSE);
//
//			strPort += _T(" Negative NG");
//		}
//	}
//	else if (bPosNG)
//	{
//		strPort.Format(_T("Positive NG"));
//
//		if (bNegOK)
//		{
//			//1110 0111
//			//signal = 0xE7;
//			//1111 1001
//			//1110 1111
//			//1100 1111
//		//	signal = 0xF9;
//		//	signal = 0xEF; 
//		signal = 0xCF;
//		//	SetSignal_HandlerOutput(3, FALSE, FALSE);
//		//	SetSignal_HandlerOutput(4, TRUE, FALSE);
//		//	SetSignal_HandlerOutput(5, FALSE, FALSE);
//
//			strPort += _T(" Negative OK");
//		}
//		else if (bNegNG)
//		{
//			//1101 0111
//			//signal = 0xD7;
//			//1111 0101
//			//1111 1111
//			//1101 1111
//		//	signal = 0xF5;
//		//	signal = 0xFF; 
//		signal = 0xDF;
//		//	SetSignal_HandlerOutput(3, FALSE, FALSE);
//		//	SetSignal_HandlerOutput(4, FALSE, FALSE);
//		//	SetSignal_HandlerOutput(5, FALSE, FALSE);
//
//			strPort += _T(" Negative NG");
//		}
//	}
//	
//	if (bPosAlarm || bNegAlarm)
//	{
//		signal = signal & 0xFD;
//	}
//
//	signal = ~signal;
//
////	if (m_DlgSyncIO->m_bOutputValue[nPort])
//	{
////		BOOL res = SyncPci_Read(GetHandleDevice(), 1, 11, &Value);
////		Value = Value | signal/*0xFE*/;	// 1111 1110
//		Value = nCellID;
//		//BOOL res = SyncPci_Write(GetHandleDevice(), 1, 11, Value);
//		BOOL res = SyncPci_Write(GetHandleDevice(), 1, 11, signal);
//		//SyncPci_Write(GetHandleDevice(), 1, 11, bValue);
//		CString strRes;
//		if (res)
//			strRes = _T("TRUE");
//		else 
//			strRes = _T("FALSE");
//
//		CString strSize;
//		if (bPosAlarm)
//			strSize = _T("Pos Size Alarm");
//		else
//			strSize = _T("Pos Size OK");
//		if (bNegAlarm)
//			strSize += _T(" Neg Size Alarm");
//		else
//			strSize += _T(" Neg Size OK");
//
//		m_logger.Log(0, _T("[Output] ID %d %s %s %s"), nCellID, strPort, strRes, strSize);
//	}
//	//else//Off
//	//{
//	//	BOOL res = SyncPci_Read(GetHandleDevice(), 1, 11, &Value);
//	//	signal = ~signal;
//	//	Value = Value & signal/*0x01*/;	// 0000 0001
//	//	SyncPci_Write(GetHandleDevice(), 1, 11, Value);
//	//}
//
//	return TRUE;
//}

void CSyncboard::SetJudgeMode(CString str)
{
	m_strJudgeMode = str;
}

BOOL CSyncboard::SetSignal_HandlerOutputJudgement(BOOL bPosOK, BOOL bNegOK, BOOL bPosConOK, BOOL bNegConOK, BOOL bPosAlarm, BOOL bNegAlarm, int nCellID)
{
	CSingleLock lock(&m_cs, TRUE);

	//다른 개발자에게 피해를 주지 않고 살짝 만들어만 두려고 여기에 적용함. 
	//모드 설정에 따라서 결과에 상관없이 all good, all defect
	if (_T("G") == m_strJudgeMode)
	{
		bPosOK = bNegOK = bPosConOK = bNegConOK = true;
	}
	if (_T("D") == m_strJudgeMode)
	{
		bPosOK = bNegOK = bPosConOK = bNegConOK = false;
	}

	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}


	BYTE signal;
	BYTE Value;
	CString strPort;

	BOOL bSendRes[3] = { TRUE, TRUE, TRUE };

	//Only 양극 테스트용
	//bNegOK = TRUE;
	//bNegNG = FALSE;
	//Only 음극 테스트용
	//bPosOK = TRUE;
	//bPosNG = FALSE;

	//일반
	if (bPosOK)
	{
		strPort.Format(_T("Pos OK"));
		bSendRes[0] = TRUE;

		// 음극 Pass
		if (bNegOK)
		{
			//1100 0111
//			signal = 0xC7;

			//0011 1000
			signal = 0x38;

			strPort += _T(" Neg OK");
			bSendRes[1] = TRUE;
		}
		// 음극 NG
		else
		{
			//1101 0111
//			signal = 0xD7;

			//0010 1000
			signal = 0x28;

			strPort += _T(" Neg NG");
			bSendRes[1] = FALSE;
		}
	}
	// 양극 NG
	else
	{
		strPort.Format(_T("Pos NG"));
		bSendRes[0] = FALSE;

		// 음극 Pass
		if (bNegOK)
		{
			//1100 1111
//			signal = 0xCF;

			//0011 0000
			signal = 0x30;

			strPort += _T(" Neg OK");
			bSendRes[1] = TRUE;
		}
		// 음극 NG
		else
		{
			//1101 1111
//			signal = 0xDF;

			//0010 0000
			signal = 0x20;

			strPort += _T(" Neg NG");
			bSendRes[1] = FALSE;
		}
	}

	if (bPosAlarm || bNegAlarm)
	{
		//1111 1101
//		signal = signal & 0xFD;

		//0000 0010
		signal = signal | 0x02;
	}

	if (!bPosConOK || !bNegConOK)
	{
		//0100 0000
		signal = signal | 0x40;
	}

//	signal = ~signal;

	//	if (m_DlgSyncIO->m_bOutputValue[nPort])
	if (m_DlgSyncIO->m_bOutputValue[0])
	{
		//		BOOL res = SyncPci_Read(GetHandleDevice(), 1, 11, &Value);
		//		Value = Value | signal/*0xFE*/;	// 1111 1110
		Value = nCellID;
		BOOL res = SyncPci_Write(GetHandleDevice(), 1, 11, signal);
		//SyncPci_Write(GetHandleDevice(), 1, 11, bValue);
		{
			CSingleLock lock(&m_sendIoDequeSetLock, TRUE);

			if (m_sendIoDeque[0].size() >= 100)
			{
				if (m_sendIoDeque[0][0] == TRUE)
				{
					m_sendIoCountOK[0]++;
				}
				else
				{
					m_sendIoCountNG[0]++;
				}

				m_sendIoDeque[0].pop_front();
			}
			m_sendIoDeque[0].push_back(bSendRes[0]);


			if (m_sendIoDeque[1].size() >= 100)
			{
				if (m_sendIoDeque[1][0] == TRUE)
				{
					m_sendIoCountOK[1]++;
				}
				else
				{
					m_sendIoCountNG[1]++;
				}

				m_sendIoDeque[1].pop_front();
			}
			m_sendIoDeque[1].push_back(bSendRes[1]);
		}

		CString strRes;
		if (res)
			strRes = _T("TRUE");
		else
			strRes = _T("FALSE");

		CString strSize;
		if (bPosAlarm)
			strSize = _T("Pos Size Alarm");
		else
			strSize = _T("Pos Size OK");
		if (bNegAlarm)
			strSize += _T(" Neg Size Alarm");
		else
			strSize += _T(" Neg Size OK");

		m_logger.Log(0, _T("[Output] ID %d %s %s %s"), nCellID, (LPCTSTR)strPort, (LPCTSTR)strRes, (LPCTSTR)strSize);
	}
	//else//Off
	//{
	//	BOOL res = SyncPci_Read(GetHandleDevice(), 1, 11, &Value);
	//	signal = ~signal;
	//	Value = Value & signal/*0x01*/;	// 0000 0001
	//	SyncPci_Write(GetHandleDevice(), 1, 11, Value);
	//}

	return TRUE;
}


//BOOL CSyncboard::SetSignal_VisionResult(int nCellID, BOOL bNotching, BOOL bElectrode, int nMarking)
//{
//	CSingleLock lock(&m_cs, TRUE);
//
//	BOOL bReturnValue1 = FALSE;
//	{
//		BYTE byValue = static_cast<BYTE>(bNotching ? 0x00 : 0x80) | static_cast<BYTE>(nCellID);
//
//		for (int i = 0; i < 10; i++)
//		{
//			bReturnValue1 = SyncPci_Write(GetHandleDevice(), 1, 6, byValue);
//
//			if (bReturnValue1)
//				break;
//		}
//
//		if (!bReturnValue1)
//			m_logger.Log(0, _T("Dimension IO Send Failed"));
//
//		m_logger.Log(0, _T("Vision Result 1 Cell ID %d %s"), nCellID, bNotching ? _T("Dimension OK") : _T("Dimension NG"));
//
//		Sleep(1);
//	}
//
//	BOOL bReturnValue2 = FALSE;
//	{
//		BYTE byValue = static_cast<BYTE>(bElectrode ? 0x00 : 0x04) | static_cast<BYTE>(nMarking);
//
//		for (int i = 0; i < 10; i++)
//		{
//			bReturnValue2 = SyncPci_Write(GetHandleDevice(), 1, 11, byValue);
//
//			if (bReturnValue2)
//				break;
//		}
//
//		if (!bReturnValue2)
//			m_logger.Log(0, _T("Surface IO Send Failed"));
//
//		m_logger.Log(0, _T("Vision Result 2 %d %d %s"), nMarking, byValue, bElectrode ? _T("Surface OK") : _T("Surface NG"));
//
//		Sleep(1);
//	}
//
//	return bReturnValue1 && bReturnValue2;
//}



BOOL CSyncboard::SetSignal_NotchingResult(int nCellID, BOOL bNotching)
{
	if (m_hDevice == INVALID_HANDLE_VALUE) return FALSE;

//	CSingleLock lock(&m_cs, TRUE);

	BOOL bReturnValue = FALSE;

	BYTE byCellID = nCellID;

	BYTE byJudge = 0;

	CString result, text;

	if (!bNotching)
	{
		byJudge = 0x80;
		result = _T("ng");
	}
	else
	{
		result = _T("ok");
	}

	BYTE byValue = byJudge | byCellID;

	//잘 썼겠지... 
	SyncPci_Write(GetHandleDevice(), 1, 6, byValue);

	text.Format(_T("sync board write(1,6) result item : notching, cell_id : %d, result : %s, value : %d"), nCellID, result, byValue);
	m_logger.Log(0, text);

	Sleep(1);

	return bReturnValue;
}


BOOL CSyncboard::SetSignal_ElectrodeResult(BOOL bElectrode, int nMarking)
{
	if (m_hDevice == INVALID_HANDLE_VALUE) return FALSE;

//	CSingleLock lock(&m_cs, TRUE);

	BOOL bReturnValue = FALSE;

	BYTE byMarking = nMarking;

	BYTE byJudge = 0;
	
	CString result, text;

	if (!bElectrode)
	{
		byJudge = 0x04;
		result = _T("ng");
	}
	else
	{
		result = _T("ok");
	}

	BYTE byValue = byMarking | byJudge;

	//잘 썼겠지... 
	SyncPci_Write(GetHandleDevice(), 1, 11, byValue);
	
	text.Format(_T("sync board write(1,11) result item : Electrode, marking : %d, result : %s, value : %d"), nMarking, result, byValue);
	m_logger.Log(0, text);


	Sleep(1);

	return bReturnValue;
}

BOOL CSyncboard::SetSignal_ResultNew(int nCellID, BOOL bNotching, BOOL bElectrode, int nMarking)//230427
{
	CSingleLock lock(&m_cs, TRUE);

	BOOL bResNotching = SetSignal_NotchingResult(nCellID, bNotching);
	Sleep(1);
	BOOL bResElectrode = SetSignal_ElectrodeResult(bElectrode, nMarking);

	return bResNotching && bResElectrode;
}


BOOL CSyncboard::SetSignal_CellSize(int nSize, int nOffset)
{
	CSingleLock lock(&m_cs, TRUE);

	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

//	int nSizePlus = nSize + nOffset;
	int nSizePlus = nSize;

	m_logger.Log(0, _T("Sync Cell Size : %d"), nSizePlus);

	BYTE nSizeLow = nSizePlus % 256;
	BYTE nSizeHigh = nSizePlus / 256;

	BOOL res1 = SyncPci_Write(GetHandleDevice(), 1, 9, nSizeLow);
	Sleep(10);
	BOOL res2 = SyncPci_Write(GetHandleDevice(), 1, 10, nSizeHigh);
	Sleep(10);

	BOOL bCellSizeWrite = FALSE;

	for (int i = 0; i < 10; i++)
	{
		BYTE byReadSizeLow = 0;
		BYTE byReadSizeHigh = 0;
		SyncPci_Read(GetHandleDevice(), 1, 9, &byReadSizeLow);
		Sleep(10);
		SyncPci_Read(GetHandleDevice(), 1, 10, &byReadSizeHigh);
		Sleep(10);

		if (byReadSizeLow == nSizeLow && byReadSizeHigh == nSizeHigh)
		{
			bCellSizeWrite = TRUE;
			break;
		}
	}

	if (!bCellSizeWrite)
		m_logger.Log(0, _T("Sync Cell Size Write Falied"));

	return TRUE;
}


BOOL CSyncboard::SetDummyCount(int nFlow, BYTE byValue)
{
	CSingleLock lock(&m_cs, TRUE);

	BOOL bRes = FALSE;

	if (nFlow == 2)
	{
		bRes = SyncPci_Write(GetHandleDevice(), 1, 3, byValue);	//Bi Cell
	}
	else if (nFlow == 0)
	{
		bRes = SyncPci_Write(GetHandleDevice(), 1, 4, byValue);
	}
	else if (nFlow == 1)
	{
		bRes = SyncPci_Write(GetHandleDevice(), 1, 5, byValue);
	}

	return bRes;
}

BOOL CSyncboard::SetDetailSignal_HandlerOutput(int nCs, int nOffset, int nPort, BOOL bSignal)
{
	CSingleLock lock(&m_cs, TRUE);

	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BYTE signal = 0;
	CString strPort;

	switch (nPort)
	{
	case 0: //0000 0001
		signal = 0x01;
		break;
	case 1: //0000 0010
		signal = 0x02;
		break;
	case 2: //0000 0100
		signal = 0x04;
		break;
	case 3: //0000 1000
		signal = 0x08;
		break;
	case 4: //0001 0000
		signal = 0x10;
		break;
	case 5: //0010 0000
		signal = 0x20;
		break;
	case 6: //0100 0000
		signal = 0x40;
		strPort = _T("Vision By-Pass");
		break;
	case 7: //1000 0000
		signal = 0x80;
		strPort = _T("PC IO Mode");
		break;
	}

	BYTE signal_not = ~signal;

	if (nOffset > -1)
	{
		BYTE byValue = 0;

		if (bSignal)
		{
			BOOL res = SyncPci_Read(GetHandleDevice(), nCs, nOffset, &byValue);
			byValue = byValue | signal;
//			byValue = byValue & signal_not;
			SyncPci_Write(GetHandleDevice(), nCs, nOffset, byValue);

			m_logger.Log(0, _T("[Output] %s off"), (LPCTSTR)strPort);

			return res;
		}
		else
		{
			BOOL res = SyncPci_Read(GetHandleDevice(), nCs, nOffset, &byValue);
			byValue = byValue & signal_not;
			byValue = byValue | signal;
			SyncPci_Write(GetHandleDevice(), nCs, nOffset, byValue);

			m_logger.Log(0, _T("[Output] %s on"), (LPCTSTR)strPort);

			return res;
		}
	}
	else
	{
		BYTE byValue = 0;

		if (bSignal)
		{
			BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), nCs, &byValue);
			byValue = byValue | signal;
//			byValue = byValue & signal_not;
			SyncPci_WriteHandlerIo(GetHandleDevice(), nCs, byValue);

			m_logger.Log(0, _T("[Output] %s off"), (LPCTSTR)strPort);

			return res;
		}
		else
		{
			BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), nCs, &byValue);
			byValue = byValue & signal_not;
//			byValue = byValue | signal;
			SyncPci_WriteHandlerIo(GetHandleDevice(), nCs, byValue);

			m_logger.Log(0, _T("[Output] %s on"), (LPCTSTR)strPort);

			return res;
		}
	}
}

//
//BOOL CSyncboard::SetDetailSignal_HandlerOutput(int nCs, int nOffset, int nPort, BOOL bSignal)
//{
//	CSingleLock lock(&m_cs, TRUE);
//
//	if (GetHandleDevice() == INVALID_HANDLE_VALUE)
//	{
//		return FALSE;
//	}
//
//	BYTE signal = 0;
//	CString strPort;
//
//	switch (nPort)
//	{
//	case 0: //1111 1110
//		signal = 0xFE;
//		break;
//	case 1: //1111 1101
//		signal = 0xFD;
//		break;
//	case 2: //1111 1011
//		signal = 0xFB;
//		break;
//	case 3: //1111 0111
//		signal = 0xF7;
//		break;
//	case 4: //1110 1111
//		signal = 0xEF;
//		break;
//	case 5: //1101 1111
//		signal = 0xDF;
//		break;
//	case 6: //1011 1111
//		signal = 0xBF;
//		strPort = _T("Vision By-Pass");
//		break;
//	case 7: //0111 1111
//		signal = 0x7F;
//		strPort = _T("PC IO Mode");
//		break;
//	}
//
//	if (nOffset > -1)
//	{
//		BYTE byValue = 0;
//
//		if (bSignal)
//		{
//			BOOL res = SyncPci_Read(GetHandleDevice(), nCs, nOffset, &byValue);
//			//Value = Value | signal/*0xFE*/;	// 1111 1110
//			byValue = byValue & signal/*0xFE*/;	// 1111 1110
//			SyncPci_Write(GetHandleDevice(), nCs, nOffset, byValue);
//
//			m_logger.Log(0, _T("[Output] %s on"), strPort);
//
//			return res;
//		}
//		else
//		{
//			BOOL res = SyncPci_Read(GetHandleDevice(), nCs, nOffset, &byValue);
//			signal = ~signal;
//			//Value = Value & signal/*0x01*/;	// 0000 0001
//			byValue = byValue | signal/*0x01*/;	// 0000 0001
//			SyncPci_Write(GetHandleDevice(), nCs, nOffset, byValue);
//
//			m_logger.Log(0, _T("[Output] %s off"), strPort);
//
//			return res;
//		}
//	}
//	else
//	{
//		BYTE byValue = 0;
//
//		if (bSignal)
//		{
//			BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), nCs, &byValue);
//			//Value = Value | signal/*0xFE*/;	// 1111 1110
//			byValue = byValue & signal/*0xFE*/;	// 1111 1110
//			SyncPci_WriteHandlerIo(GetHandleDevice(), nCs, byValue);
//
//			m_logger.Log(0, _T("[Output] %s on"), strPort);
//
//			return res;
//		}
//		else
//		{
//			BOOL res = SyncPci_ReadHandlerOut(GetHandleDevice(), nCs, &byValue);
//			signal = ~signal;
//			//Value = Value & signal/*0x01*/;	// 0000 0001
//			byValue = byValue | signal/*0x01*/;	// 0000 0001
//			SyncPci_WriteHandlerIo(GetHandleDevice(), nCs, byValue);
//
//			m_logger.Log(0, _T("[Output] %s off"), strPort);
//
//			return res;
//		}
//	}
//}



void CSyncboard::SetDeviceUnitIndexList(const long lotIndex, const long repeatIndex, const long count, const long *ids)
{
	CSingleLock lock(&m_cs, TRUE);

	m_nLotIndex = lotIndex;
	m_nRepeatIndex = repeatIndex;

	if (count != m_nDeviceUnitIndexCount)
	{
		delete[] m_pnDeviceUnitIndexList;

		m_pnDeviceUnitIndexList = new long[count];
		m_nDeviceUnitIndexCount = count;
	}

	memcpy(m_pnDeviceUnitIndexList, ids, sizeof(long) * count);
}

void CSyncboard::GetDeviceUnitIndexList(const long *&ids, long &count)
{
	ids = m_pnDeviceUnitIndexList;
	count = m_nDeviceUnitIndexCount;
}

void CSyncboard::SetIoInterface(const long targetIoCount, const long pocketIoCount, const long fovIoCount)
{
	CSingleLock lock(&m_cs, TRUE);

	m_targetInputBegin = 0;
	m_targetInputEnd = m_targetInputBegin + targetIoCount;
	m_pocketInputBegin = m_targetInputEnd;
	m_pocketInputEnd = m_pocketInputBegin + pocketIoCount;
	m_fovInputBegin = m_pocketInputEnd;
	m_fovInputEnd = m_fovInputBegin + fovIoCount;

	m_targetInputMask = 0;

	for (long bit = m_targetInputBegin; bit < m_targetInputEnd; bit++)
	{
		m_targetInputMask |= (1 << bit);
	}

	m_pocketInputMask = 0;

	for (long bit = m_pocketInputBegin; bit < m_pocketInputEnd; bit++)
	{
		m_pocketInputMask |= (1 << bit);
	}

	m_fovInputMask = 0;

	for (long bit = m_fovInputBegin; bit < m_fovInputEnd; bit++)
	{
		m_fovInputMask |= (1 << bit);
	}

	IniHelper::SaveINT(g_szConfigPathName, _T("SYNC_IO_INTERFACE"), _T("TARGET_INPUT_COUNT"), targetIoCount);
	IniHelper::SaveINT(g_szConfigPathName, _T("SYNC_IO_INTERFACE"), _T("POCKET_INPUT_COUNT"), pocketIoCount);
	IniHelper::SaveINT(g_szConfigPathName, _T("SYNC_IO_INTERFACE"), _T("FOV_INPUT_COUNT"), fovIoCount);
}

void CSyncboard::GetIoInterface(long &targetIoCount, long &pocketIoCount, long &fovIoCount)
{
	CSingleLock lock(&m_cs, TRUE);

	targetIoCount = m_targetInputEnd - m_targetInputBegin;
	pocketIoCount = m_pocketInputEnd - m_pocketInputBegin;
	fovIoCount = m_fovInputEnd - m_fovInputBegin;
}

unsigned short CSyncboard::GetHandlerDI()
{
	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	BYTE data1 = 0;

	//FULL
	if (!SyncPci_ReadHandlerIo(m_hDevice, 0, &data1))	// HANDLER0_R
	{
		AfxMessageBox(_T("SyncPCI Read Fail!"));

		return FALSE;
	}

	BYTE data2 = 0;

	if (!SyncPci_ReadHandlerIo(m_hDevice, 1, &data2))	// HANDLER0_R
	{
		AfxMessageBox(_T("SyncPCI Read Fail!"));
		return FALSE;
	}

	// Active Low Signal
	data1 = ~data1;
	data2 = ~data2;

	return (unsigned short)((data2 << 8) | data1);
}

unsigned short CSyncboard::GetHandlerDI(long &scanTarget, long &lotIndex, long &repeatIndex, long &deviceID, long &pocketID, long &fovID)
{
	CSingleLock lock(&m_cs, TRUE);

	CSingleLog log(__FUNCTION__);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	// Pocket ID, Piece ID Check
	unsigned short usHandlerDI = GetHandlerDI();

	scanTarget = (m_targetInputMask & usHandlerDI) >> m_targetInputBegin;
	lotIndex = m_nLotIndex;
	repeatIndex = m_nRepeatIndex;
	pocketID = (m_pocketInputMask & usHandlerDI) >> m_pocketInputBegin;
	fovID = (m_fovInputMask & usHandlerDI) >> m_fovInputBegin;

	if (pocketID < m_nDeviceUnitIndexCount)
	{
		deviceID = m_pnDeviceUnitIndexList[pocketID];
	}

	return usHandlerDI;
}

BOOL CSyncboard::IsOpenPort()
{
	return m_hDevice != INVALID_HANDLE_VALUE;
}

void CSyncboard::SetTriggerMode(BYTE byTrigger)
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return;
	}

	SyncPci_Write(m_hDevice, 1, 8, (BYTE)byTrigger);	// 0 - sync trigger (1~255 분주비로 동작), 1 - pzt ext. real trigger
	Sleep(1);
}

void CSyncboard::SetDigitalValue(BOOL bFlg, long wNewData)
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return;
	}

	//T - > 1구간
	//F  -> Skip구간

	//	9
	//	10
	const int CS1 = 1;
	ULONG offset = 10 - bFlg;
	byte nInputUpper = wNewData >> 8;
	byte nInputLower = wNewData & 0x00FF;


	// 13 , 9
	// 14 , 10
	SyncPci_Write(m_hDevice, CS1, offset, nInputLower);
	Sleep(1);

	SyncPci_Write(m_hDevice, CS1, offset + 4, nInputUpper);
	Sleep(1);
}


BOOL CSyncboard::SyncWrite(ULONG CS, ULONG offset, byte pValue)
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	while (!bRet)
	{
		bRet = SyncPci_Write(m_hDevice, CS, offset, pValue);
		//TRACE(_T("Sync Write\n"));
		Sleep(1);
	}

	return TRUE;
}

BOOL CSyncboard::CheckSyncErrorCondition()
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	byte Data;
	BOOL bError(FALSE);
	if (m_hDevice != NULL)
	{
		if (!SyncPci_ReadHandlerOut(m_hDevice, 1, &Data))	// HANDLER0_R
		{
			AfxMessageBox(_T("SyncPCI Read Fail!"));

			return FALSE;
		}
		Data = Data & 0x80;

		if (Data == 0x80)
		{
			return FALSE;//10000000
		}
		else
		{
			return TRUE;//10000000
		}
	}

	return FALSE;//10000000
}

BOOL CSyncboard::SetSyncErrorCondition()
{
	CSingleLog log(__FUNCTION__);

	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	byte Data;
	byte Data2 = 0xff;
	if (m_hDevice != NULL)
	{
		SyncPci_ReadHandlerOut(m_hDevice, 1, &Data);

		Data2 = Data | 0x80;


		if (!SyncPci_WriteHandlerIo(m_hDevice, 1, Data2))	// HANDLER0_R
		{
			AfxMessageBox(_T("SyncPCI Read Fail!"));
			return FALSE;
		}
		Data = 0x00;

		long nThreshold = 100;
		long nCount = 0;
		while (1 && nCount < nThreshold)
		{
			SyncPci_ReadHandlerOut(m_hDevice, 1, &Data);
			if (Data == Data2)
			{
				break;
			}

			if (!SyncPci_WriteHandlerIo(m_hDevice, 1, Data2))	// HANDLER0_R
			{
				AfxMessageBox(_T("SyncPCI Read Fail!"));
				return FALSE;
			}

			Sleep(1);
			nCount++;
		}
	}

	return FALSE;
}


BYTE CSyncboard::GetIoCount(/*BYTE byCellPos*/)
{
	CSingleLock lock(&m_cs, TRUE);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	BYTE byValue = 0 /*byOffset = 52 + byCellPos*/;

//	SyncPci_Read(m_hDevice, 0, byOffset, &byValue);
	SyncPci_Read(m_hDevice, 1, 8, &byValue);

	return byValue;
}


void CSyncboard::ResetSendIoCount()
{
	CSingleLock lock(&m_cs, TRUE);

	for (int i = 0; i < 3; i++)
	{
		m_sendIoDeque[i].clear();
		m_sendIoCountOK[i] = 0;
		m_sendIoCountNG[i] = 0;
	}
}

BOOL CSyncboard::ReadCellID(long CellID)
{
	//CSingleLock lock(&m_cs_trigger, TRUE);
	if (m_dqCellID_Trigger.size() > 100)
		m_dqCellID_Trigger.clear();//클리어할곳 찾기
	m_dqCellID_Trigger.push_back(CellID);
	m_logger.Log(0, _T("[PushBack Sync CellID Bit] CellID : %d , Deque Size : %d"), CellID, m_dqCellID_Trigger.size());
	m_dqTrigger_Window->push_back(CellID);
	if (64 < m_dqTrigger_Window->size())
		for (int i = 64; i < m_dqTrigger_Window->size() - 64; i++)
		{
			m_dqTrigger_Window->pop_front();
		}
	return TRUE;
}

long CSyncboard::GetCell_ID() //두개의 cellid가 일치하는지 확인
{
	//long CellID = -1;

	//if (m_dqCellID.size())
	//{
	//	//CSingleLock lock(&m_cs_trigger, TRUE);
	//	CellID = m_dqCellID.front();
	//	m_dqCellID.pop_front();
	//}

	return m_nCellID_Test;
}

//BOOL CSyncboard::ReadCellIDBuffer()
//{
//	//CSingleLock lock(&m_cs, TRUE);
//	if (m_hDevice == INVALID_HANDLE_VALUE)
//	{
//		return false;
//	}
//	long CellID = 0;
//	BYTE byRecValue = 0, byStatusValue = 0;
//	BYTE byBufferEmpty = 0, byBufferFull = 0;
//
//	// 12번지 Read Only
//	//	DATA 구성[XX][YYYYYY]
//	//	1)XX:DON`T USE[2BIT] : 상위 2BIT
//	//	2)YYYYYY:CELL ID[6BIT] : 하위6BIT
//
//
//
//	// 14번지 Read Only
//	//	DATA 구성[XXXXXX][Y][Z]
//	//	1)XXXXXX:DON`T USE[6BIT] : 상위 6BIT
//	//	2)Y:READ 쪽 FULL CHECK[1BIT] : 하위1BIT
//	//	2)Z:READ 쪽 EMPTY CHECK[1BIT] : 하위1BIT
//	SyncPci_Read(m_hDevice, CS_NUMBER_1, DATA_IN_BUFFER_STATUS, &byStatusValue);
//	byBufferEmpty = byStatusValue & 0x01;
//	byBufferFull = (byStatusValue >> 1) & 0x01;
//
//	if (!byBufferEmpty && byBufferEmpty != m_byPrevBufferEmpty)
//	{
//		Sleep(2); //바로 읽을땐 슬립 필요
//		SyncPci_Read(m_hDevice, CS_NUMBER_1, DATA_IN_BUFFER_DATA, &byRecValue);
//		CellID = byRecValue & 0x3f;
//		{
//			//CSingleLock lock(&m_cs_buffer, TRUE);
//			if (m_dqCellID_Buffer.size() > 100)
//				m_dqCellID_Buffer.clear();//클리어할곳 찾기
//
//			m_dqCellID_Buffer.push_back(CellID);
//
//			m_logger.Log(0, _T("[PushBack Sync CellID Bit from buffer] CellID : %d , Deque Size : %d, EmptyStatus : %d, FullStatus : %d"), CellID, m_dqCellID_Buffer.size(), byBufferEmpty, byBufferFull);
//
//			m_dqBuffer_Window->push_back(CellID);
//
//			if (64 < m_dqBuffer_Window->size())
//			{
//				for (int i = 64; i < m_dqBuffer_Window->size() - 64; i++)
//				{
//					m_dqBuffer_Window->pop_front();
//				}
//			}
//		}
//
//		m_byPrevBufferEmpty = byBufferEmpty;
//
//		return true;
//	}
//
//	m_byPrevBufferEmpty = byBufferEmpty;
//	return false;
//}

BOOL CSyncboard::ReadCellIDBuffer()
{
	//CSingleLock lock(&m_cs, TRUE);
	CSingleLock lock(&m_cs, TRUE);
	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	long CellID = 0;
	BYTE byRecValue = 0, byStatusValue = 0;
	BYTE byBufferEmpty = 0, byBufferFull = 0;

	// 12번지 Read Only
	//	DATA 구성[XX][YYYYYY]
	//	1)XX:DON`T USE[2BIT] : 상위 2BIT
	//	2)YYYYYY:CELL ID[6BIT] : 하위6BIT

	// 14번지 Read Only
	//	DATA 구성[XXXXXX][Y][Z]
	//	1)XXXXXX:DON`T USE[6BIT] : 상위 6BIT
	//	2)Y:READ 쪽 FULL CHECK[1BIT] : 하위1BIT
	//	2)Z:READ 쪽 EMPTY CHECK[1BIT] : 하위1BIT
	SyncPci_Read(m_hDevice, CS_NUMBER_1, DATA_IN_BUFFER_STATUS, &byStatusValue);
	byBufferEmpty = byStatusValue & 0x01;
	byBufferFull = (byStatusValue >> 1) & 0x01;
	if (!byBufferEmpty)
	{
		Sleep(IO_DELAY); //바로 읽을땐 슬립 필요
		SyncPci_Read(m_hDevice, CS_NUMBER_1, DATA_IN_BUFFER_DATA, &byRecValue);
		CellID = byRecValue & 0x3f;
		{
			if (m_dqCellID_Buffer.size() > 100)
				m_dqCellID_Buffer.clear();//클리어할곳 찾기
			m_dqCellID_Buffer.push_back(CellID);
			m_nCellID_Test = CellID;
			m_logger.Log(0, _T("[PushBack Sync CellID Bit from buffer] CellID : %d , Deque Size : %d, EmptyStatus : %d, FullStatus : %d"), CellID, m_dqCellID_Buffer.size(), byBufferEmpty, byBufferFull);
			//TRACE3("[PushBack Sync CellID Bit from buffer] CellID : %d , Deque Size : %d, EmptyStatus : %d, FullStatus : %d\n", CellID, m_dqCellID_Buffer.size(), byBufferEmpty, byBufferFull);
			/*m_dqBuffer_Window->push_back(CellID);
			if (64 < m_dqBuffer_Window->size())
				for (int i = 64; i < m_dqBuffer_Window->size() - 64; i++)
				{
					m_dqBuffer_Window->pop_front();
				}*/
		}
		return true;
	}

	return false;
}

void CSyncboard::ClearQueue_CellID()
{
	{
		//CSingleLock lock(&m_cs_trigger, TRUE);
		m_dqCellID_Trigger.clear();
	}
	{
		//CSingleLock lock(&m_cs_buffer, TRUE);
		m_dqCellID_Buffer.clear();
	}
	m_dqSyncOutputResult.clear();
	m_dqCellID.clear();
	nLastCellID = -1;
}

void CSyncboard::Check_CellID()
{
	long CellID_Tigger = -1, CellID_Buffer = -1, CellID_Result = -1;

	if (m_dqCellID_Trigger.size())
	{
		//CSingleLock lock(&m_cs_trigger, TRUE);
		CellID_Tigger = m_dqCellID_Trigger.front();
		//m_dqCellID_Trigger.pop_front();
	}
	if (m_dqCellID_Buffer.size())
	{
		//CSingleLock lock(&m_cs_buffer, TRUE);
		CellID_Buffer = m_dqCellID_Buffer.front();
	}
	if (CellID_Buffer != -1  && CellID_Tigger == CellID_Buffer && CellID_Tigger != -1)
	{
		CellID_Result = CellID_Tigger;
		if (-1 != nLastCellID && 63 != nLastCellID)
		{
			if ((nLastCellID + 1) != CellID_Result)
				m_logger.Log(0,_T("[ERROR] LastCellID : %d, CurrentCellID: %d"),nLastCellID, CellID_Result);
		}
		else if (-1 != nLastCellID && 63 == nLastCellID)
		{
			if (0 != CellID_Result)
				m_logger.Log(0, _T("[ERROR] LastCellID : %d, CurrentCellID: %d"), nLastCellID, CellID_Result);
		}
		m_dqCellID.push_back(CellID_Result);
		nLastCellID = CellID_Result;
		m_dqCellID_Buffer.pop_front();
		m_dqCellID_Trigger.pop_front();
		m_logger.Log(0, _T("[Merge Cell ID]CellID : %d"), CellID_Result);

	}
	else if (CellID_Buffer != -1 && CellID_Tigger != -1)
	{
		int nTriggerPopCount = 0, nBufferPopCount = 0;
		for (int i = 0; i < m_dqCellID_Trigger.size(); i++)
		{
			if (0 != nTriggerPopCount || 0 != nBufferPopCount)
				break;
			for (int j = 0; j < m_dqCellID_Buffer.size(); j++)
			{
				if (m_dqCellID_Trigger[i] == m_dqCellID_Buffer[j]) {
					nTriggerPopCount = i;
					nBufferPopCount = j;
					break;
				}
			}
		}
		for (int i = 0; i < nTriggerPopCount; i++)
		{
			int CellID = m_dqCellID_Trigger.front();
			m_dqCellID_Trigger.pop_front();
			m_logger.Log(0, _T("[ERROR] 들어온 CELL ID가 서로 다름  -  CellID_Buffer : %d , CellID_Trigger : %d, CellID_Trigger의 %d삭제"), CellID_Buffer, CellID_Tigger, CellID_Tigger, CellID);
		}
		for (int i = 0; i < nBufferPopCount; i++)
		{
			int CellID = m_dqCellID_Buffer.front();
			m_dqCellID_Buffer.pop_front();
			m_logger.Log(0, _T("[ERROR] 들어온 CELL ID가 서로 다름  -  CellID_Buffer : %d , CellID_Trigger : %d, CellID_Buffer의 %d삭제"), CellID_Buffer, CellID_Tigger, CellID_Buffer, CellID);
		}
	}
}

void CSyncboard::InsertResult(int nCellID, BOOL bNotching, BOOL bElectrode, int nMarking, CString strDefectName)
{
	if (m_hDevice == INVALID_HANDLE_VALUE) return;
	if (-1 == nCellID)
		return;

	CSingleLock lock(&m_cs, TRUE);

	SyncOutputResult result;
	result.init(nCellID, bNotching, bElectrode, nMarking, strDefectName);
	BOOL bisExist = FALSE;
	for (int i = 0; i < m_dqSyncOutputResult.size(); i++)
	{
		if (result.m_nCellID == m_dqSyncOutputResult[i].m_nCellID)
		{
			//배출
			bisExist = TRUE;
			BYTE byValue11, byValue6;
			BYTE byCellID = result.m_nCellID;
			BYTE byNotchingJudge = 0, byElectrodeJudge=0;
			BYTE byMarking = result.m_nMarking | m_dqSyncOutputResult[i].m_nMarking;
			CString strNotchingResult, strElectrodeResult, text;
			
			if (!result.m_bNotching || !m_dqSyncOutputResult[i].m_bNotching)
			{
				byNotchingJudge = 0x80;
				strNotchingResult = _T("NG");
			}
			else
			{
				strNotchingResult = _T("OK");
			}

			if (!result.m_bElectrode || !m_dqSyncOutputResult[i].m_bElectrode)
			{
				byElectrodeJudge = 0x04;
				strElectrodeResult = _T("NG");
			}
			else
			{
				strElectrodeResult = _T("OK");
			}

			byValue6 = byNotchingJudge | byCellID;
			byValue11 = byMarking | byElectrodeJudge;

			SyncPci_Write(GetHandleDevice(), 1, 6, byValue6);
			Sleep(1);
			SyncPci_Write(GetHandleDevice(), 1, 11, byValue11);
			m_logger.Log(0,_T("[Write To Sync]CellID:%d,  전극결과:%s, 노칭결과:%s, 마킹개수:%d, 불량명 :%s,%s"), result.m_nCellID, strElectrodeResult, strNotchingResult, byMarking, result.m_strDefectName, m_dqSyncOutputResult[i].m_strDefectName);
			m_dqSyncOutputResult.erase(m_dqSyncOutputResult.begin() + i);
			m_dqOutput_Window->push_back(result.m_nCellID);
			if (64 < m_dqOutput_Window->size())
				for (int i = 64; i < m_dqOutput_Window->size() - 64; i++)
				{
					m_dqOutput_Window->pop_front();
				}
		}
	}
	if (FALSE == bisExist)
		m_dqSyncOutputResult.push_back(result);

}

void CSyncboard::ManualMarking(int CellID, BOOL bNotching, BOOL bElectrode, int Count)
{
	if (m_hDevice == INVALID_HANDLE_VALUE) return;
	CSingleLock lock(&m_cs, TRUE);

	BYTE byValue11, byValue6;
	BYTE byCellID = CellID;
	//BYTE byNotchingJudge = bNotching, byElectrodeJudge = bElectrode;
	BYTE byMarking = Count;

	BYTE byNotchingJudge = 0, byElectrodeJudge = 0;
	if(!bNotching)
		byNotchingJudge = 0x80;
	if (!bElectrode)
		byElectrodeJudge = 0x04;
	byValue6 = byNotchingJudge | byCellID;
	byValue11 = byMarking | byElectrodeJudge;

	SyncPci_Write(GetHandleDevice(), 1, 6, byValue6);
	Sleep(1);
	SyncPci_Write(GetHandleDevice(), 1, 11, byValue11);
	m_logger.Log(0, _T("[Write To Sync] 가상결과 송부 CellID:%d,  전극결과:%s, 노칭결과:%s, 마킹개수:%d"), byCellID, 
		bNotching == TRUE?_T("OK"):_T("NG"), bElectrode == TRUE ? _T("OK") : _T("NG"), byMarking);
}