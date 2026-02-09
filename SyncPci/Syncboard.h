#pragma once

#ifdef __WSI_PCI_SYNC_CLASS_EXPORT__
#define __WSI_PCI_SYNC_CLASS__	__declspec(dllexport)
#else
#define __WSI_PCI_SYNC_CLASS__	__declspec(dllimport)
#endif

#define WM_SPLITTER_ALARM_OFF WM_USER +1000
#define WM_ELECTRODE_MOVE_START WM_USER +1001
#define WM_ELECTRODE_MOVE_STOP WM_USER +1002

#define CS_NUMBER_0		0
#define CS_NUMBER_1		1

#define DATA_IN_BUFFER_STATUS		14
#define DATA_IN_BUFFER_DATACOUNT	13
#define DATA_IN_BUFFER_DATA			12

#include "ipvmbasedef.h"
#include <afxmt.h>
#include <deque>
#include "Gadget/TimeCheck.h"

class CSyncIODlg;

enum class SyncboardInput : long
{
	INSPECTION_RUN_ON,
	CYL1_FWD_CHK_SENSOR,
	NONE_3,
	NONE_4,
	NONE_5,
	NONE_6,
	NONE_7,
	NONE_8,
	NONE_9,
	ENC_FW_CHK_SENSOR,
	NONE_11,
	NONE_12,
	NONE_13,
	NONE_14,
	NONE_15,
	NONE_16,
	END
};

enum class SyncboardOutput : long
{
	VisionRunEnable,
	Illumination,
	Reserved,
	CellResultEnable,
	CellResult,
	Marking1,
	Marking2,
	VisionAlarm,					// PLC에 직접입력된 IO: Vision Alarm 상태를 알려줌
	CellResultID0,
	CellResultID1,
	CellResultID2,
	CellResultID3,
	CellResultID4,
	CellResultID5,
	CellResultID6,
	ConnectionTape_or_NoTab,		// PLC에 직접입력된 IO: No Tab 이나 Connection Tape이 검출되면 PLC를 Stop시키기 위해 신호를 발생한다
	END
};

struct SyncOutputResult
{
	int m_nCellID;
	BOOL m_bNotching;
	BOOL m_bElectrode;
	int m_nMarking;
	CString m_strDefectName;

	void init(int nCellID, BOOL bNotching, BOOL bElectrode, int nMarking, CString strDefectName)
	{
		m_nCellID = nCellID;
		m_bNotching = bNotching;
		m_bElectrode = bElectrode;
		m_nMarking = nMarking;
		m_strDefectName = strDefectName;
	}

};
class __WSI_PCI_SYNC_CLASS__ CSyncboard
{
public:	// Ctor & Dtor
	CSyncboard(IPVM::LoggerInterface& logger, HWND wnd, BOOL bEnabled, int GenerationVersion, std::deque<long>& Trigger, std::deque<long>& Buffer, std::deque<long>& Output);
	//CSyncboard(BOOL bEnabled);
	~CSyncboard(void);
	std::deque<long>* m_dqTrigger_Window;
	std::deque<long>* m_dqBuffer_Window;
	std::deque<long>* m_dqOutput_Window;
public: // Interfaces
	void SetMeasureMode(BOOL bMoireMeasure);
	void SetIllumControllerType(BYTE value);

	BOOL GetInputBit(SyncboardInput inputType) const;
	BOOL GetOutputBit(SyncboardOutput outputType) const;

	//Camera Setting
	void SetFramePeriod(float fFramePeriod_ms);
	void SetExposureTime(float exposureTime_ms);
	void SetIlluminationDelay(float fIlluminationDelay_ms);

	// 2D Frame
	void Set2DLedIntensity(long nFrame, const BYTE pbyIntensity[16]);
	void Set2DCurrentFrame(long nFrame);
	void Set2DFrameCount(long nFrameCount);
	void Set2DIllumSel(BYTE value);
	void Set2DSelMode(BYTE value);
	void Make2DLookupTable();
	void Make2DIllumTable();

	//3D Frame
	void SetUse3DChannel(long nCh, BOOL bOn);
	void Set3DMultibandNum(long nNum);
	void Set3DDualSwitch(BOOL bUse);
	void Set3DLedIntensity(long nCh, long nBand, float fDuration_us);
	void Set3DFrameCount(long nCh, long nBand, WORD n3DFrameCount);
	void Set3DCurrentFrame(long nCh, long nBand);

	// Command
	BOOL StartSyncboard();
    BOOL LiveStartSyncboard();
	void TurnOffLight();

	// Hardware Restriction
	BYTE GetMaxFrameCount();
	BYTE GetMaxChannelCount();
	void ClosePort();

	// Show Dialog
	void ShowDIOCheckDlg();
	std::deque<SyncOutputResult> m_dqSyncOutputResult;
public:
	CCriticalSection m_cs;
	CCriticalSection m_csInput;
	CWinThread* m_IoThread;
	BYTE	m_bySendValue;
	int		m_nTabOffset/*[2]*/;

	BOOL	GetTabOffset();
	BYTE	GetCellCountValue();

	void	SetSignal_VisionReady(BOOL bNewValue);
	BOOL	GetSignal_VisionReady();

	BYTE	GetIoCount(/*BYTE byCellPos*/);

	std::deque <BOOL> m_sendIoDeque[3];
	long m_sendIoCountOK[3];
	long m_sendIoCountNG[3];
	CCriticalSection m_sendIoDequeSetLock;

	void	SetSignal_VisionAcquisition(BOOL bNewValue);
	BOOL	GetSignal_VisionAcquisition();

	BOOL	GetSignal_HandlerMoveReady();
	BOOL	GetSignal_HandlerInput();
	BOOL	GetSignal_HandlerOutput();
	BOOL	GetOutputBitFromRealRead(SyncboardOutput outputType);
	BOOL	SetSignal_HandlerOutput(SyncboardOutput outputType, BOOL bSignal);
//	BOOL	SetSignal_HandlerOutputJudgement(BOOL bPosOK, BOOL bPosNG, BOOL bNegOK, BOOL bNegNG, BOOL bPosAlarm, BOOL bNegAlarm, int nCellID);
	BOOL	SetSignal_HandlerOutputJudgement(BOOL bPosOK, BOOL bNegOK, BOOL bPosConOK, BOOL bNegConOK, BOOL bPosAlarm, BOOL bNegAlarm, int nCellID);
	BOOL	SetSignal_CellSize(int nSize, int nOffset);
	BOOL	SetDetailSignal_HandlerOutput(int nCs, int nOffset, int nPort, BOOL bSignal);
	BOOL	SetDummyCount(int nFlow, BYTE byValue);

	//TODO: SetSignal_VisionResult() 함수를 사용하고, SetSignal_NotchingResult() 및 SetSignal_ElectrodeResult() 함수는 제거할 것
	//BOOL	SetSignal_VisionResult(int nCellID, BOOL bNotching, BOOL bElectrode, int nMarking);
	BOOL	SetSignal_NotchingResult(int nCellID, BOOL bNotching);
	BOOL	SetSignal_ElectrodeResult(BOOL bElectrode, int nMarking);
	BOOL	SetSignal_ResultNew(int nCellID, BOOL bNotching, BOOL bElectrode, int nMarking);//230427
	CString m_strJudgeMode;
	void	SetJudgeMode(CString str);

	void	SetDeviceUnitIndexList(const long lotIndex, const long repeatIndex, const long count, const long *ids);
	void	GetDeviceUnitIndexList(const long *&ids, long &count);

	void	SetIoInterface(const long targetIoCount, const long pocketIoCount, const long fovIoCount);
	void	GetIoInterface(long &targetIoCount, long &pocketIoCount, long &fovIoCount);

	unsigned short GetHandlerDI();
	unsigned short GetHandlerDI(long &scanTarget, long &lotIndex, long &repeatIndex, long &deviceID, long &pocketID, long &fovID);


	BOOL Download(BYTE byAddress, BYTE byData);
	BOOL SyncWrite(ULONG CS, ULONG offset, BYTE pValue);

	void SetTriggerMode(BYTE byTrigger);

	//use Digital mode
	void	SetDigitalValue(BOOL bFlg, long wNewData);
	//PZT Trigger
	BOOL	SetSyncErrorCondition();

	//sync condition Check!
	BOOL CheckSyncErrorCondition();

	HANDLE GetHandleDevice(){ return m_hDevice; }

	void WriteIoLog(LPCTSTR str, ...);

	CCriticalSection m_csLog;

	void ResetSendIoCount();
	SYSTEMTIME m_Time;
	float m_fTime;
public:
	BOOL IsOpenPort();
	BOOL OpenPort();
	BOOL m_bOpen;
	IPVM::LoggerInterface &m_logger;
	CListBox* m_logList;
	BOOL m_bMonitoring;
	BOOL m_bCellCountRead;
	CTime m_pivotTime;
	CTime m_prevTime;
	CTimeSpan m_drivingTime;
	BOOL m_bPrevByPass;
	BOOL m_bPrevVisionReady;

	enum CELL_TYPE
	{
		MONO_CELL = 0,
		HALF_CELL = 1,
		BI_CELL = 2
	};
	enum ALARM_TYPE
	{
		NORMAL_STATE = 0,
		OVER_FLOW_ALARM = 1,
	//	CELL_SIZE = 2,
	//	ENCODER_ALARM = 3,
	//	GAP_SENSOR_ALARM = 4
	};
	CELL_TYPE m_nCellType;
	ALARM_TYPE m_nAlarmType;
	BOOL m_bAlarmChange;
	void StartGetTabOfssetTimer();

	bool m_bClearChangeOn;
	//1.5세대
	CCriticalSection m_cs_buffer;
	CCriticalSection m_cs_trigger;

	std::deque<long>	m_dqCellID_Buffer;
	std::deque<long>	m_dqCellID_Trigger;
	std::deque<long>	m_dqCellID;
	BOOL ReadCellIDBuffer();
	long m_nCellID_Test;
	void ClearQueue_CellID();
	void Check_CellID();
	void InsertResult(int nCellID, BOOL bNotching, BOOL bElectrode, int nMarking, CString strDefectName);
	void ManualMarking(int CellID, BOOL bNotching, BOOL bElectrode, int Count);
	long GetCell_ID();
	BOOL ReadCellID(long CellID);

	BYTE m_byPrevBufferEmpty;

	int m_GenerationVersion;
	// !1.5세대
	//에이징 테스트용
	int nLastCellID;
private:
	HANDLE m_hDevice;
	HANDLE m_hDevice_2;

	CSyncIODlg* m_DlgSyncIO;

	long m_nLotIndex;
	long m_nRepeatIndex;
	long *m_pnDeviceUnitIndexList;
	long m_nDeviceUnitIndexCount;

	long m_targetInputBegin;
	long m_targetInputEnd;
	long m_pocketInputBegin;
	long m_pocketInputEnd;
	long m_fovInputBegin;
	long m_fovInputEnd;

	long m_targetInputMask;
	long m_pocketInputMask;
	long m_fovInputMask;

	CCriticalSection m_syncCS;

	HWND m_hwnd;
	BOOL m_bCurHandlerInput[(long)SyncboardInput::END];
	BOOL m_bCurHandlerOutput[(long)SyncboardOutput::END];
};
