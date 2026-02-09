#pragma once
#include "basedef.h"

#include "./New/CISComm.h"
//#include "../SharedModule/VisionHostShare/HostCommSocket.h"

class __GRAB_CLASS__ CCisSetting
{
public:
	CCisSetting(int m_nGenerationVersion);
	~CCisSetting();

public:
	BOOL m_bOpen;

public:	
	BOOL CIS_Open();
	BOOL CIS_Close();

	BOOL Splitter_EnableDualMode(int handleIndex, BOOL bEnable);
	BOOL Splitter_EnableEncoderMultiplying(int handleIndex, BOOL bEnable);
	BOOL Splitter_EnableRemoteControl(int handleIndex, BOOL bEnable);
	BOOL Splitter_SetParameter(int handleIndex, UINT32 address, UINT32 pValue);
	BOOL Splitter_GetParameter(int handleIndex, UINT32 address, UINT32* pValue);
	BOOL Splitter_Run(int handleIndex);
	BOOL Splitter_Stop(int handleIndex);
	BOOL Splitter_SetDioMode(int handleIndex, CIS_DIO_MODE mode);
	BOOL Splitter_SetHorizontalSize(int handleIndex, UINT32 size);
	BOOL Splitter_SetNumCameras(int handleIndex, UINT32 modNum);
	BOOL Splitter_SetPositionCellID(int handleIndex, BOOL endLine);
	BOOL Splitter_SetTriggerInput(int handleIndex, CIS_TRIGGER_INPUT input);
	BOOL Splitter_SetTriggerDivision(int handleIndex, UINT32 input);

	BOOL Camera_CBRegWrite(int handleIndex, UINT32 cameraIndex, CIS_REGISTER_ADDRESS addr, UINT8 value);
	BOOL Camera_CBRegRead(int handleIndex, UINT32 cameraIndex, CIS_REGISTER_ADDRESS addr, UINT8* pValue);
	BOOL Camera_RedCBRegWrite(int handleIndex, UINT32 cameraIndex, UINT32 value);
	BOOL Camera_GreenCBRegWrite(int handleIndex, UINT32 cameraIndex, UINT32 value);
	BOOL Camera_BlueCBRegWrite(int handleIndex, UINT32 cameraIndex, UINT32 value);

	BOOL Camera_Refresh(int handleIndex, UINT32 cameraIndex);
	BOOL Camera_ScanStart(int handleIndex, UINT32 cameraIndex);
	BOOL Camera_ScanStop(int handleIndex, UINT32 cameraIndex);
	BOOL Camera_SetLedMode(int handleIndex, UINT32 cameraIndex, CIS_LED_MODE mode);
	BOOL Camera_LineDivision(int handleIndex, UINT32 cameraIndex, BOOL bEnable);

	CString m_strError;
	void GetRegisterModule(int handleIndex, UINT32 cameraIndex);

private:
	int m_nGenerationVersion;
	int m_nStartChannel;
	int m_nChannelCount;
	CIS_REGISTER_ADDRESS m_chGainAddress[27];
	CIS_HANDLE m_hCisHandle[6];
	CString m_strPortName[6];

	BOOL GetStatus(CIS_RETURN_STATUS returnStatus, LPCTSTR fmt, ...);

	static LPCTSTR GetStatusString(CIS_RETURN_STATUS returnStatus);
};