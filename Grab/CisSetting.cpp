#include "stdafx.h"
#include "CisSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "./lib/CISComm.lib")

CCisSetting::CCisSetting(int nGenerationVersion)
	:m_nGenerationVersion(nGenerationVersion)
{
	m_nStartChannel = -1;
	m_nChannelCount = -1;

	m_bOpen = FALSE;

	CIS_REGISTER_ADDRESS chGainArray[27] = { GAIN_1, GAIN_2, GAIN_3, GAIN_4, GAIN_5, GAIN_6, GAIN_7, GAIN_8, GAIN_9, GAIN_10, GAIN_11, GAIN_12, GAIN_13, GAIN_14, GAIN_15,
		GAIN_16, GAIN_17, GAIN_18, GAIN_19, GAIN_20, GAIN_21, GAIN_22, GAIN_23, GAIN_24, GAIN_25, GAIN_26, GAIN_27 };

	for (int i = 0; i < 27; i++)
	{
		m_chGainAddress[i] = chGainArray[i];
	}
}


CCisSetting::~CCisSetting()
{
}

LPCTSTR CCisSetting::GetStatusString(CIS_RETURN_STATUS returnStatus)
{
	switch (returnStatus)
	{
	case CIS_SUCCESS:						return _T("Success");
	case CIS_ERROR_TIMEOUT:					return _T("Timeout");
	case CIS_ERROR_INVALID_HANDLE:			return _T("Invalid handle");
	case CIS_ERROR_INVALID_PARAMETER:		return _T("Invalid parameter");
	case CIS_ERROR_INVALID_RECV_DATA:		return _T("Invalid receive data");
	case CIS_ERROR_FAILED:					return _T("Failed");
	case CIS_ERROR_FAILED_OPEN:				return _T("Failed to open");
	case CIS_ERROR_FAILED_CL_COMM:			return _T("Failed to Communicate CL");
	case CIS_ERROR_FAILED_CONTROL:			return _T("Failed to Control Io");
	case CIS_ERROR_ALREADY_OPEN:			return _T("It's already opened");
	}

	ASSERT(!_T("정의 안됨"));
	return _T("");
}

BOOL CCisSetting::CIS_Open()
{
	UINT8 ManufacturerInfo[1024];
	if (!GetStatus(CisGetClManufacturerInfo(ManufacturerInfo, 1024), _T("GetManufacturerInfo")))
		return FALSE;

	// Cis Port 개수 가져오기
	UINT32 nNumPorts;
	if (!GetStatus(CisGetClNumSerialPorts(&nNumPorts), _T("GetNumSerialPorts")))
		return FALSE;

	for (int i = 0; i < (int)nNumPorts; i++)
	{
		CIS_HANDLE commHandle;
		// Cis 오픈
		if (!GetStatus(CisOpenSerial(i, &commHandle), _T("OpenSerial")))
			return FALSE;

		// Baudrate 설정하기
		if (!GetStatus(CisSetBaudRate(commHandle, CIS_UART_BAUD_115200), _T("SetBaudRate")))
			return FALSE;

		// Port 정보 가져오기
		UINT8 PortID[1024];
		if (!GetStatus(CisGetClSerialPortId(i, PortID, 1024), _T("GetSerialPortId")))
			return FALSE;

		CStringA strATmp = CStringA(PortID);
		char* pTmp = strATmp.GetBuffer();
		m_strPortName[i] = CString(pTmp);

		// Tap Contorl
		CString strPort;
		strPort.Format(_T("Port %d"), i);

		m_hCisHandle[i] = commHandle;
	}

	// Library, Firmware 버전 가져오기
	CIS_VERSION libVersion;
	if (!GetStatus(CisGetLibraryVersion(&libVersion), _T("GetLibraryVersion")))
		return FALSE;


	CString strLib, strFw;
	strLib.Format(_T("%02x.%02x"), libVersion.majorVersion, libVersion.minorVersion);

	// 최대 Timeout 10초
	CisSetTimeout(10000);

	m_bOpen = TRUE;

	return TRUE;
}


BOOL CCisSetting::CIS_Close()
{
	for (int i = m_nStartChannel; i < m_nChannelCount; i++)
	{
		GetStatus(CisCloseSerial(m_hCisHandle[i]), _T("CIS Close Serial"));
	}

	m_bOpen = FALSE;

	return TRUE;
}

BOOL CCisSetting::Splitter_EnableDualMode(int handleIndex, BOOL bEnable)
{
	return GetStatus(CisEnableDualMode(m_hCisHandle[handleIndex], bEnable), _T("CIS Enable Dual Mode[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_EnableEncoderMultiplying(int handleIndex, BOOL bEnable)
{
	return GetStatus(CisEnableEncoderMultiplying(m_hCisHandle[handleIndex], bEnable), _T("CIS Enable Encoder Multiplying[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_EnableRemoteControl(int handleIndex, BOOL bEnable)
{
	return GetStatus(CisEnableRemoteControl(m_hCisHandle[handleIndex], bEnable), _T("CIS Enable Remote Control[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_SetParameter(int handleIndex, UINT32 address, UINT32 pValue)
{
	return GetStatus(CisSetParameter(m_hCisHandle[handleIndex], address, pValue), _T("CIS Set Parameter[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_GetParameter(int handleIndex, UINT32 address, UINT32* pValue)
{
	return GetStatus(CisGetParameter(m_hCisHandle[handleIndex], address, pValue), _T("CIS Get Parameter[%d]"), handleIndex);
}
BOOL CCisSetting::Splitter_Run(int handleIndex)
{
	return GetStatus(CisRun(m_hCisHandle[handleIndex]), _T("CIS Run[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_Stop(int handleIndex)
{
	return GetStatus(CisStop(m_hCisHandle[handleIndex]), _T("CIS Stop[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_SetDioMode(int handleIndex, CIS_DIO_MODE mode)
{
	return GetStatus(CisSetDioMode(m_hCisHandle[handleIndex], mode), _T("CIS Set DIO Mode[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_SetHorizontalSize(int handleIndex, UINT32 size)
{
	return GetStatus(CisSetHorizontalSize(m_hCisHandle[handleIndex], size), _T("CIS Set Horizontal Size[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_SetNumCameras(int handleIndex, UINT32 modNum)
{
	return GetStatus(CisSetNumCameras(m_hCisHandle[handleIndex], modNum), _T("CIS Set Num Modules[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_SetPositionCellID(int handleIndex, BOOL endLine)
{
	return GetStatus(CisSetPositionCellID(m_hCisHandle[handleIndex], endLine), _T("CIS Set Position CellID[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_SetTriggerInput(int handleIndex, CIS_TRIGGER_INPUT input)
{
	return GetStatus(CisSetTriggerInput(m_hCisHandle[handleIndex], input), _T("CIS Set Trigger Input[%d]"), handleIndex);
}

BOOL CCisSetting::Splitter_SetTriggerDivision(int handleIndex, UINT32 input)
{
	return GetStatus(CisSetTriggerDivision(m_hCisHandle[handleIndex], input), _T("CIS Set Trigger Division[%d]"), handleIndex);
}

BOOL CCisSetting::Camera_CBRegWrite(int handleIndex, UINT32 cameraIndex, CIS_REGISTER_ADDRESS addr, UINT8 value)
{
	CString head;
	head.Format(_T("Module CB Reg Write[%d]"), handleIndex);

	switch (addr)
	{
	case CIS_REGISTER_ADDRESS::LINE_EVEN_ODD_BRIGHT:	head.Format(_T("Camera Set Even Odd Line Bright[%d]"), handleIndex);	break;
	case CIS_REGISTER_ADDRESS::LINE_EVEN_ODD_DARK:		head.Format(_T("Camera Set Even Odd Line Dark[%d]"), handleIndex);		break;
	case CIS_REGISTER_ADDRESS::LED_DUTY_RED:			head.Format(_T("Camera [CH0]CB Reg Write[%d]"), handleIndex);			break;
	case CIS_REGISTER_ADDRESS::LED_DUTY_GREEN:			head.Format(_T("Camera [CH1]CB Reg Write[%d]"), handleIndex);			break;
	case CIS_REGISTER_ADDRESS::LED_DUTY_BLUE:			head.Format(_T("Camera [CH2]CB Reg Write[%d]"), handleIndex);			break;
	}

	return GetStatus(CisCamCBRegWrite(m_hCisHandle[handleIndex], cameraIndex, addr, value), head);
}

BOOL CCisSetting::Camera_CBRegRead(int handleIndex, UINT32 cameraIndex, CIS_REGISTER_ADDRESS addr, UINT8* pValue)
{
	return GetStatus(CisCamCBRegRead(m_hCisHandle[handleIndex], cameraIndex, addr, pValue), _T("Camera CB Reg Write[%d]"), handleIndex);
}

BOOL CCisSetting::Camera_RedCBRegWrite(int handleIndex, UINT32 cameraIndex, UINT32 value)
{
	return Camera_CBRegWrite(handleIndex, cameraIndex, LED_DUTY_RED, (BYTE)min(255, max(0, value)));
}

BOOL CCisSetting::Camera_GreenCBRegWrite(int handleIndex, UINT32 cameraIndex, UINT32 value)
{
	return Camera_CBRegWrite(handleIndex, cameraIndex, LED_DUTY_GREEN, (BYTE)min(255, max(0, value)));
}

BOOL CCisSetting::Camera_BlueCBRegWrite(int handleIndex, UINT32 cameraIndex, UINT32 value)
{
	return Camera_CBRegWrite(handleIndex, cameraIndex, LED_DUTY_BLUE, (BYTE)min(255, max(0, value)));
}

BOOL CCisSetting::Camera_Refresh(int handleIndex, UINT32 cameraIndex)
{
	return GetStatus(CisCamRefresh(m_hCisHandle[handleIndex], cameraIndex), _T("Camera Refresh[%d]"), handleIndex);
}

BOOL CCisSetting::Camera_ScanStart(int handleIndex, UINT32 cameraIndex)
{
	return GetStatus(CisCamScanStart(m_hCisHandle[handleIndex], cameraIndex), _T("Camera Scan Start[%d]"), handleIndex);
}

BOOL CCisSetting::Camera_ScanStop(int handleIndex, UINT32 cameraIndex)
{
	return GetStatus(CisCamScanStop(m_hCisHandle[handleIndex], cameraIndex), _T("Camera Scan Stop[%d]"), handleIndex);
}

BOOL CCisSetting::Camera_SetLedMode(int handleIndex, UINT32 cameraIndex, CIS_LED_MODE mode)
{
	return GetStatus(CisCamSetLedMode(m_hCisHandle[handleIndex], cameraIndex, mode), _T("Camera Set Led Mode[%d]"), handleIndex);
}

BOOL CCisSetting::Camera_LineDivision(int handleIndex, UINT32 cameraIndex, BOOL bEnable)
{
	return GetStatus(CisCamSetLineDivision(m_hCisHandle[handleIndex], cameraIndex, bEnable), _T("Camera Set Line Division[%d]"), handleIndex);
}

BOOL CCisSetting::GetStatus(CIS_RETURN_STATUS returnStatus, LPCTSTR fmt, ...)
{
	auto statusString = GetStatusString(returnStatus);

	CString strMsg;
	va_list argptr;

	va_start(argptr, fmt);
	strMsg.FormatV(fmt, argptr);
	va_end(argptr);

	m_strError.Format(_T("[%s] - %s"), (LPCTSTR)strMsg, statusString);

	if (returnStatus == CIS_SUCCESS)
	{
		return TRUE;
	}

	return FALSE;
}


void CCisSetting::GetRegisterModule(int handleIndex, UINT32 cameraIndex)
{
	//::UpdateData(TRUE);

	CIS_LED_MODE ledMode = CIS_LED_SINGLE_RED_ILLUMINATION;
	CIS_STORAGE storage = CIS_STORAGE_FACTORY;
	BOOL enable = FALSE;
	bool bReturnStatus;

	bReturnStatus = GetStatus(CisCamGetLineDivision(m_hCisHandle, cameraIndex, &enable), _T("CamGetLineDivision[%d]"), handleIndex);
	if (CIS_SUCCESS != bReturnStatus)
		return;

	bReturnStatus = GetStatus(CisCamGetLedMode(m_hCisHandle, cameraIndex, &ledMode), _T("CamGetLedMode[%d]"), handleIndex);
	if (CIS_SUCCESS != bReturnStatus)
		return;

	UINT8 nRed = 0, nGreen = 0, nBlue = 0;
	CisCamCBRegRead(m_hCisHandle, cameraIndex, LED_DUTY_RED, &nRed);
	CisCamCBRegRead(m_hCisHandle, cameraIndex, LED_DUTY_GREEN, &nGreen);
	CisCamCBRegRead(m_hCisHandle, cameraIndex, LED_DUTY_BLUE, &nBlue);
	CisCamGetStorage(m_hCisHandle, cameraIndex, &storage);

	//UpdateData(FALSE);
}