#pragma once

#ifndef _CIS_COMM_H
#define _CIS_COMM_H

#ifdef CISCOMM_EXPORTS
#define	_CISCommAPI				__declspec(dllexport)
#else
#define _CISCommAPI				__declspec(dllimport)
#endif // CISCOMM_EXPORTS


typedef void*	CIS_HANDLE;								// Handler


typedef enum _CIS_RETURN_STATUS {
	CIS_SUCCESS = 0,									// 성공할 때
	CIS_ERROR_TIMEOUT,									// 송수신 중, 타임아웃
	CIS_ERROR_INVALID_HANDLE,							// 유효하지 않은 핸들
	CIS_ERROR_INVALID_PARAMETER,						// 유효하지 않은 인자값
	CIS_ERROR_INVALID_RECV_DATA,						// 수신 데이터가 유효하지 않음
	CIS_ERROR_FAILED,									// 범용.. (x)
	CIS_ERROR_FAILED_OPEN,								// Open 실패	(x)
	CIS_ERROR_FAILED_CL_COMM,							// Camera Link 통신 실패
	CIS_ERROR_FAILED_CONTROL,							// 레지스터 메모리(Control IO) Read/Write 실패  
	CIS_ERROR_ALREADY_OPEN,								// 이미 열려 있음
	CIS_ERROR_BUFFER_SMALL								// 버퍼 수가 작음
}CIS_RETURN_STATUS;

// Ver3.1 사용 X, Ver 2.2 추가
typedef enum _CIS_MODE {								// CIS Mode 
//	CIS_MODE_MODULE_0 = 0,
//	CIS_MODE_MODULE_1 = 1,								// Ver2.7 사용 X
//	CIS_MODE_MODULE_2 = 2,								// Ver2.7 사용 X
//	CIS_MODE_MODULE_3 = 3,								// Ver2.7 사용 X
//	CIS_MODE_MODULE_4 = 4,								// Ver2.7 사용 X
//	CIS_MODE_MODULE_5 = 5,								// Ver2.7 사용 X
//	CIS_MODE_HUB	  = 6
	CIS_MODE_CAMERA_0 = 0,								// Ver3.1 
	CIS_MODE_CAMERA_1 = 1,								// Ver3.1
	CIS_MODE_CAMERA_2 = 2,								// Ver3.1 
	CIS_MODE_CAMERA_3 = 3,								// Ver3.1 
	CIS_MODE_CAMERA_4 = 4,								// Ver3.1 
	CIS_MODE_CAMERA_5 = 5,								// Ver3.1 
	CIS_MODE_HUB	  = 6,								// Ver3.1, 라미 비전 용
	CIS_MODE_SPLITTER = 6,								// Ver3.1, 노칭 비전 용
}CIS_MODE;

typedef enum _CIS_TRIGGER_INPUT {						// CIS Trigger Input
	CIS_INPUT_ENCODER = 0,
	CIS_INPUT_FRAME
}CIS_TRIGGER_INPUT;

typedef struct _CIS_VERSION {							// CIS Version
	UINT32 majorVersion;
	UINT32 minorVersion;
}CIS_VERSION;


typedef enum _CIS_UART_BAUD_RATE {
	CIS_UART_BAUD_9600 = 1,
	CIS_UART_BAUD_19200 = 2,
	CIS_UART_BAUD_38400 = 4,
	CIS_UART_BAUD_57600 = 8,
	CIS_UART_BAUD_115200 = 16
} CIS_UART_BAUD_RATE;

typedef enum _CIS_LED_TYPE {
	CIS_LED_WHITE = 0,
	CIS_LED_RED,
	CIS_LED_GREEN,
	CIS_LED_BLUE
}CIS_LED_TYPE;

// Ver 2.2
typedef enum _CIS_LED_MODE {
	CIS_LED_SINGLE_RED_ILLUMINATION = 0,
	CIS_LED_SINGLE_RED_GREEN_ILLUMINATION,
	CIS_LED_SINGLE_RED_BLUE_ILLUMINATION,
	CIS_LED_DUAL_GREEN_ILLUMNINATION,
	CIS_LED_DUAL_BLUE_ILLUMNINATION,
	CIS_LED_DUAL_GREEN_BLUE_ILLUMNINATION,
}CIS_LED_MODE;

typedef enum _CIS_STORAGE {
	CIS_STORAGE_FACTORY = 0,
	CIS_STORAGE_USER1,
	CIS_STORAGE_USER2
}CIS_STORAGE;

typedef struct _CIS_INDIV_CALIB {						// Individual Calibration Parameter
	CIS_LED_TYPE	type;
	UINT32			startAddress;
	UINT32			count;
}CIS_INDIV_CALIB;

typedef enum _CIS_REGISTER_ADDRESS {					// CIS Register Address, 메뉴얼 참고

	// CB Register
	LEDCYC1				= 0x80,
	LEDCYC2				= 0x81,
	LEDCYC3				= 0x82,
//	LEDCYC4				= 0x83,
//	LEDCYC5				= 0x84,
//	LEDCYC6				= 0x85,
//	LEDCYC7				= 0x86,
//	LEDCYC8				= 0x87,
//	CONF1				= 0x88,
//	CONF2				= 0x89,
//	ALLELM_LO			= 0x90,
//	ALLELM_UP			= 0x91,
//	EFFELM_LO			= 0x92,
//	EFFELM_UP			= 0x93,
//	STRPIX_LO			= 0x94,
//	STRPIX_UP			= 0x95,
//	INVLCLK				= 0x96,
//	DPISEL				= 0x97,
//	LED_START			= 0x98,
//	OUTSEL				= 0x99,
//	DUM_LINE			= 0x9C,
//	TARG_V_R			= 0xB0,
//	TARG_V_G			= 0xB1,
//	TARG_V_B			= 0xB2,
//	TARG_V_IR			= 0xB3,
//	TARG_V_Add_A		= 0xB4,
//	TARG_V_Add_B		= 0xB5,
//	TARG_V_Add_C		= 0xB6,
//	TARG_V_Add_D		= 0xB7,
	LINE_DIVISION		= 0xDA,
	START_PIXEL			= 0xDB,
	LED_DUTY_RED		= 0xE0,
	LED_DUTY_GREEN		= 0xE1,
	LED_DUTY_BLUE		= 0xE2,
	AUTO_START			= 0x89,
	BRIGTHNESS			= 0xA9,
	SAVED_PARAM			= 0xF0,

	// CL Register
//	J_AFELCK			= 0x00,
//	J_TRIGSEL			= 0x02,
//	J_CLD				= 0x03
	ELEC_SHUTTER		= 0x02,
	CL_DATA_FORMAT		= 0x03,

	LINE_EVEN_ODD_BRIGHT = 0XDC,
	LINE_EVEN_ODD_DARK = 0XDD,

	GAIN_1 = 0x28, 
	GAIN_2 = 0x29, 
	GAIN_3 = 0x2a, 
	GAIN_4 = 0x44, 
	GAIN_5 = 0x45, 
	GAIN_6 = 0x46, 
	GAIN_7 = 0x4c, 
	GAIN_8 = 0x4d, 
	GAIN_9 = 0x4e, 
	GAIN_10 = 0x54, 
	GAIN_11 = 0x55, 
	GAIN_12 = 0x56, 
	GAIN_13 = 0x5c, 
	GAIN_14 = 0x5d, 
	GAIN_15 = 0x5e, 
	GAIN_16 = 0x64, 
	GAIN_17 = 0x65, 
	GAIN_18 = 0x66, 
	GAIN_19 = 0x6c, 
	GAIN_20 = 0x6d, 
	GAIN_21 = 0x6e, 
	GAIN_22 = 0x74, 
	GAIN_23 = 0x75, 
	GAIN_24 = 0x76, 
	GAIN_25 = 0x7c, 
	GAIN_26 = 0x7d, 
	GAIN_27 = 0x7e,

	SPLITTER_FIRMWARE_VERSION = 0x00,
	SPLITTER_ALARM_CELL_ID = 0x0A,
	SPLITTER_VISION_1_CELL_RESULT = 0x0B,
	SPLITTER_VISION_2_CELL_RESULT = 0x0C,
	IGNORE_CELL_TRIGGER_MARGIN = 0x0D, // Ignore Cell Trigger Margin(Cell Trigger 필터링 값) Default 100 ? 100line 동안 Cell Trigger 신호를 필터링함
	CELL_ID_STATUS = 0x0E, // Cell ID Status(알람 발생상황에서 현재 Cell ID 와 이전 ID([15..8] 이전 ID, [7..0] 현재 ID 상태))
	IGNORE_CELL_TRIGGER_COUNT = 0x0F, // Ignore Cell Trigger Count(Cell Trigger  필터링 구간에서 반복되는 입력 신호가 있는지 확인)
	SPLITTER_OUT_RESULT = 0x10, // Splitter Out Result(마지막 배출된 Result)
	SPLITTER_IN_CELL_ID_COUNT = 0x11, // Splitter In Cell ID Count(입력 받은 Cell ID 개수)
	SPLITTER_OUT_RESULT_COUNT = 0x12, // Splitter Out Result Count(배출된  Cell Result 개수)
	SPLITTER_IN_PC1_RESULT_COUNT = 0x13, // Splitter In PC1 Result Count(입력 받은 Vision 1 Result 개수)
	SPLITTER_IN_PC2_RESULT_COUNT = 0x14, // Splitter In PC2 Result Count(입력 받은 Vision 2 Result 개수)
	SPLITTER_IN_CIS1_CELL_TRIGGER_CHECK_COUNT = 0x18,	
	SPLITTER_IN_CIS2_CELL_TRIGGER_CHECK_COUNT = 0x19,

	SPLITTER_VISION_1_NG_MARKING_COUNT_RO	= 0x1C,			//"001 1100(0x1C)"	Vision1 NG Marking Wr Count		RO		Vision1 NG 마킹 입력 횟수	ver 00.23
	SPLITTER_VISION_1_NG_RESULT_COUNT_RO	 = 0x1D,			//"001 1101(0x1D)"	Vision1 NG Result Count		RO		Vision1 검사 불량 발생 횟수	ver 00.23
	SPLITTER_VISION_2_NG_MARKING_COUNT_RO	 = 0x1E,			//"001 1110(0x1E)"	Vision2 NG Marking Wr Count		RO		Vision2 NG 마킹 입력 횟수	ver 00.23
	SPLITTER_VISION_2_NG_RESULT_COUNT_RO	= 0x1F,			//"001 1111(0x1F)"	Vision2 NG Result Count		RO		Vision2 검사 불량 발생 횟수	ver 00.23
	SPLITTER_NG_MARKING_COUNT_RO			= 0x20,					//"010 0000(0x20)"	Splitter NG Marking Count		RO		스플리터 NG 마킹 출력 횟수	ver 00.23
	SPLITTER_NG_RESULT_COUNT_RO				= 0x21,						//"010 0001(0x21)"	Splitter NG Resut Count		RO		스플리터 불량 결과 출력 횟수	ver 00.23

} CIS_REGISTER_ADDRESS;

// Ver 2.5
typedef enum _CIS_DIO_MODE {
	CIS_DIO_NORMAL = 0,
	CIS_DIO_LOOP_BACK,
	CIS_DIO_CELL_ID_TEST,
}CIS_DIO_MODE;

// Ver 2.7
typedef enum _CIS_PORT {
	CIS_PORT_A = 0,
	CIS_PORT_B,
}CIS_PORT;

// Ver 2.10
typedef enum _SPLITTER_ADDRESS {
	SP_FIRMWARE_VERSION = 0,
	SP_MODE = 1,
	SP_TRIGGER = 2,
	SP_RESET = 3,
	SP_REMOTE = 4,
	SP_DIO = 5,
	SP_DUAL_MODE = 6,
	SP_HORIZONTAL_SIZE = 7,
	SP_ENCODER_MULTIPLYING = 8,
	SP_CELL_ID_POSITION = 9,
}SPLITTER_ADDRESS;

#ifdef __cplusplus
extern "C" {
#endif
	
	// ***********************************************************************************
	// **************************     CIS Serial 설정    **********************************
	// ***********************************************************************************
	_CISCommAPI			CIS_RETURN_STATUS		CisGetClNumSerialPorts(UINT32* pPortNum);
	_CISCommAPI			CIS_RETURN_STATUS		CisGetClManufacturerInfo(UINT8* pManufacturerName, UINT32 buffersize);
	_CISCommAPI			CIS_RETURN_STATUS		CisGetClSerialPortId(UINT32 serialIndex, UINT8* pPordId, UINT32 buffersize);
	_CISCommAPI			CIS_RETURN_STATUS		CisSetBaudRate(CIS_HANDLE handle, CIS_UART_BAUD_RATE baudRate);
	_CISCommAPI			CIS_RETURN_STATUS		CisGetBaudRate(CIS_HANDLE handle, CIS_UART_BAUD_RATE* pBaudRate);

	_CISCommAPI			CIS_RETURN_STATUS		CisOpenSerial(UINT32 serialIndex, CIS_HANDLE* pHandle);
	_CISCommAPI			CIS_RETURN_STATUS		CisCloseSerial(CIS_HANDLE handle);

	// ***********************************************************************************
	// **************************     CIS Mode 설정    ***********************************
	// ***********************************************************************************
//	_CISCommAPI			CIS_RETURN_STATUS		CisSelectMode(CIS_HANDLE handle, CIS_MODE mode);					// 사용 X, Ver2.2 SetMode로 대체 
//	_CISCommAPI			CIS_RETURN_STATUS		CisSelectModule(CIS_HANDLE handle, CIS_MODULE module);				// 사용 X, Ver2.2 SetMode로 대체
//	_CISCommAPI			CIS_RETURN_STATUS		CisSetMode(CIS_HANDLE handle, CIS_MODE mode);						// Ver3.1 제거, Ver2.2 추가
//	_CISCommAPI			CIS_RETURN_STATUS		CisGetMode(CIS_HANDLE handle, CIS_MODE* pMode);						// Ver3.1 제거, Ver2.2 추가

	// ***********************************************************************************
	// ****************************   CIS Hub / Splitter Mode    ************************************
	// ***********************************************************************************
	_CISCommAPI			CIS_RETURN_STATUS		CisGetConnectPort(CIS_HANDLE handle, CIS_PORT* pPort);				// Ver2.7 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisGetLibraryVersion(CIS_VERSION* pVersion);
	_CISCommAPI			CIS_RETURN_STATUS		CisGetFirmwareVersion(CIS_HANDLE handle, CIS_VERSION* pVersion);
	_CISCommAPI			CIS_RETURN_STATUS		CisSetNumCameras(CIS_HANDLE handle, UINT32 camNum);					// Ver3.1 함수명 변경
	_CISCommAPI			CIS_RETURN_STATUS		CisGetNumCameras(CIS_HANDLE handle, UINT32* pCamNum);				// Ver3.1 함수명 변경
	_CISCommAPI			CIS_RETURN_STATUS		CisSetTriggerDivision(CIS_HANDLE handle, UINT32 division);
	_CISCommAPI			CIS_RETURN_STATUS		CisGetTriggerDivision(CIS_HANDLE handle, UINT32* pDivision);
	_CISCommAPI			CIS_RETURN_STATUS		CisSetTriggerInput(CIS_HANDLE handle, CIS_TRIGGER_INPUT input);
	_CISCommAPI			CIS_RETURN_STATUS		CisGetTriggerInput(CIS_HANDLE handle, CIS_TRIGGER_INPUT* pInput);
	_CISCommAPI			CIS_RETURN_STATUS		CisRun(CIS_HANDLE handle);
	_CISCommAPI			CIS_RETURN_STATUS		CisStop(CIS_HANDLE handle);
	_CISCommAPI			CIS_RETURN_STATUS		CisReset(CIS_HANDLE handle);										// Ver2.0 - additional, 다시 Open해야 됨.
	_CISCommAPI			CIS_RETURN_STATUS		CisEnableRemoteControl(CIS_HANDLE handle, BOOL bEnable);			// Ver2.2 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisSetDioMode(CIS_HANDLE handle, CIS_DIO_MODE mode);				// Ver2.5 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisGetDioMode(CIS_HANDLE handle, CIS_DIO_MODE* pMode);				// Ver2.5 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisEnableDualMode(CIS_HANDLE handle, BOOL bEnable);					// Ver2.6 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisSetHorizontalSize(CIS_HANDLE handle, UINT32 size);				// Ver2.6 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisGetHorizontalSize(CIS_HANDLE handle, UINT32* pSize);				// Ver2.6 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisEnableEncoderMultiplying(CIS_HANDLE handle, BOOL bEnable);		// Ver2.8 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisSetPositionCellID(CIS_HANDLE handle, BOOL endLine);				// Ver2.9 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisGetPositionCellID(CIS_HANDLE handle, BOOL* pEndLine);			// Ver2.9 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisSetParameter(CIS_HANDLE handle, UINT32 address, UINT32 value);	// Ver2.10 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisGetParameter(CIS_HANDLE handle, UINT32 address, UINT32* pValue);	// Ver2.10 추가

	// *************************************************************************************
	// ****************************   CIS Camera Mode    ***********************************
	// *************************************************************************************
//	_CISCommAPI			CIS_RETURN_STATUS		CisCorrectionStart(CIS_HANDLE handle);								// Ver2.0 - Calibration과 동일, 제거
	_CISCommAPI			CIS_RETURN_STATUS		CisCamScanStart(CIS_HANDLE handle, UINT32 camIndex);				// Ver3.1 함수명 변경
	_CISCommAPI			CIS_RETURN_STATUS		CisCamScanStop(CIS_HANDLE handle, UINT32 camIndex);					// Ver3.1 함수명 변경
	_CISCommAPI			CIS_RETURN_STATUS		CisCamRefresh(CIS_HANDLE handle, UINT32 camIndex);					// Ver3.1 함수명 변경, Ver2.2 - ModuleReset -> ModuleRefresh
	_CISCommAPI			CIS_RETURN_STATUS		CisCamSetRawMode(CIS_HANDLE handle, UINT32 camIndex);				// Ver3.1 함수명 변경
	_CISCommAPI			CIS_RETURN_STATUS		CisCamSetCorrectionMode(CIS_HANDLE handle, UINT32 camIndex);		// Ver3.1 함수명 변경
	_CISCommAPI			CIS_RETURN_STATUS		CisCamCalibrationStart(CIS_HANDLE handle, UINT32 camIndex);			// Ver3.1 함수명 변경
	
	_CISCommAPI			CIS_RETURN_STATUS		CisCamSetIndividualCalib(CIS_HANDLE handle, UINT32 camIndex, CIS_INDIV_CALIB calib, UINT8* pSendData);	// Ver3.1 함수명 변경, Ver2.0 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisCamGetIndividualCalib(CIS_HANDLE handle, UINT32 camIndex, CIS_INDIV_CALIB calib, UINT8* pRecvData);	// Ver3.1 함수명 변경, Ver2.0 추가
	
	_CISCommAPI			CIS_RETURN_STATUS		CisCamSaveStorage(CIS_HANDLE handle, UINT32 camIndex, CIS_STORAGE storage);			// Ver3.1 함수명 변경, Ver2.0 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisCamLoadStorage(CIS_HANDLE handle, UINT32 camIndex, CIS_STORAGE storage);			// Ver3.1 함수명 변경

//	_CISCommAPI			CIS_RETURN_STATUS		CisCamSetStorage(CIS_HANDLE handle, UINT32 camIndex, CIS_STORAGE storage);			// Ver3.1 사용 X, Ver 2.2 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisCamGetStorage(CIS_HANDLE handle, UINT32 camIndex, CIS_STORAGE* pStorage);		// Ver3.1 함수명 변경, Ver 2.2 추가
	
	_CISCommAPI			CIS_RETURN_STATUS		CisCamSetLedMode(CIS_HANDLE handle, UINT32 camIndex, CIS_LED_MODE mode);			// Ver3.1 함수명 변경, Ver 2.2 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisCamGetLedMode(CIS_HANDLE handle, UINT32 camIndex, CIS_LED_MODE* pMode);			// Ver3.1 함수명 변경, Ver 2.2 추가

	_CISCommAPI			CIS_RETURN_STATUS		CisCamSetLineDivision(CIS_HANDLE handle, UINT32 camIndex, BOOL enable);				// Ver3.1 함수명 변경, Ver 2.8 추가
	_CISCommAPI			CIS_RETURN_STATUS		CisCamGetLineDivision(CIS_HANDLE handle, UINT32 camIndex, BOOL* pEnable);			// Ver3.1 함수명 변경, Ver 2.8 추가

	// ************************************************************************************
	// ****************************   CIS Camera 레지스터 설정    ***********************************
	// ************************************************************************************
	_CISCommAPI			CIS_RETURN_STATUS		CisCamCBRegWrite(CIS_HANDLE handle, UINT32 camIndex, CIS_REGISTER_ADDRESS addr, UINT8 value);	// Ver3.1 함수명 변경 
	_CISCommAPI			CIS_RETURN_STATUS		CisCamCBRegRead(CIS_HANDLE handle, UINT32 camIndex, CIS_REGISTER_ADDRESS addr, UINT8* pValue);	// Ver3.1 함수명 변경 
	_CISCommAPI			CIS_RETURN_STATUS		CisCamCLRegWrite(CIS_HANDLE handle, UINT32 camIndex, CIS_REGISTER_ADDRESS addr, UINT8 value);	// Ver3.1 함수명 변경 
	_CISCommAPI			CIS_RETURN_STATUS		CisCamCLRegRead(CIS_HANDLE handle, UINT32 camIndex, CIS_REGISTER_ADDRESS addr, UINT8* pValue);	// Ver3.1 함수명 변경 
	

	// ***********************************************************************************
	// *****************************     CIS ETC     ************************************
	// ***********************************************************************************
	_CISCommAPI			CIS_RETURN_STATUS		CisSetTimeout(UINT32 timeout);
	_CISCommAPI			CIS_RETURN_STATUS		CisGetTimeout(UINT32* pTimeout);

#ifdef __cplusplus
}
#endif


#endif // !_CIS_COMM_H
