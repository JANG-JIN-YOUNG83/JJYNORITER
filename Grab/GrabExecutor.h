#pragma once
#include "basedef.h"

#include <stdint.h>
#include "Gadget/SimpleWorkerThread.h"
#include <vector>

namespace SharedMemory
{
	class ImageQueuePusher;
}

enum class GrabMode
{
	Inline,
	Live,
	MSA,
	AREA,
	END,
	
};

enum class MachineType
{
	UC1_mismatch,
	ES_mismatch,
	ES_notching_single,
	ES_notching_dual,
    ES_rollpress_pinhole,
	ES_rollpress_surface,
};

enum class ExecutorType
{
	iGrab,
	Sapera,
	SharedMem,
	Euresys,
	Insnex,
};

enum class GrabError
{
	None,
	OverRun,
	CircularBufferCorrupted,
};

enum InspectionMode
{
	Grabonly = 0,// 0 : Grab only
	Grabandcellcutting,// 1 : Grab and cell cutting
	Grabandcellcuttingandinspection,// 2 : Grab and cell cutting and inspection
};

typedef void(*CALLBACK_ON_FRAME_UPDATED)(void *callbackData, int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount, GrabError error);
typedef void(*IGCALLBACK_GRBEVT)(void* userData, DWORD grabCount, void* param1, void* param2);

class __GRAB_CLASS__ GrabExecutor : public IPVM::SimpleWorkerThread
{
public:
	GrabExecutor() = delete;
	GrabExecutor(const GrabExecutor &rhs) = delete;
	GrabExecutor(IPVM::LoggerInterface& logger, const int cameraIndex);
	virtual ~GrabExecutor();

	virtual void SetAlarm(long nCode);


	GrabExecutor &operator=(const GrabExecutor &rhs) = delete;

public:
	// 이 함수는 프레임 그래버 라이브러리의 콜백 쓰레드에서 수행되는 함수를 등록함.
	// 그러므로, 이 콜백 내에서는 최소한의 작업만 수행해야 함.
	// 이미지에서 특정 값을 읽어서 체크한다든지 하는 작업들을 수행하면 적당.
	// 추가로, 다른 쓰레드를 위해서 이벤트를 날리는 정도까지만 할 것.
	void RegisterCallbackOnFrameUpdated(GrabMode mode, CALLBACK_ON_FRAME_UPDATED syncCallbackFunc, void *syncCallbackData);

public:
	bool IsInitializeOk() const;
	void Start(GrabMode mode);	// 그랩 쓰레드 시작
	void Stop();				// 그랩 쓰레드 중단

public:
    void SetFrameIndex_Area(int64_t headFrameIndex, int64_t tailFrameIndex);
    GrabMode GetGrabMode();

	UINT GetCircularBufferCount() const;
	UINT GetHorizontalSize() const;
	UINT GetVerticalSize() const;
    BOOL GetSensorImageMirror() const;

	void GetUpdatedFrameIndices(int64_t &headFrameIndex, int64_t &tailFrameIndex) const;
	const unsigned char *GetFrameAddress(const int64_t frameIndex) const;
    const unsigned char* GetSecFrameAddress(const int64_t frameIndex) const;
    const unsigned char* GetSumAddress_Top(const int64_t frameIndex) const;
    const unsigned char* GetSumAddress_Bottom(const int64_t frameIndex) const;

	float GetFrameGrabbedTime_ms(const int64_t frameIndex) const;
	void ProceedHeadFrameIndex(int64_t offset);
	void SetSensorImageMirror(bool mirror);
	void SetHeadFrameIndexInterval(int interval);
	//11.5세대
	void SetFrameAddress(int64_t frameIndex, int64_t index, unsigned char value);
    void SetSecFrameAddress(int64_t frameIndex, int64_t index, unsigned char value);
	//!1.5세대
	virtual void SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount = -1) = 0;
    virtual void SetVerticalLength(long nPageLength) = 0;
    virtual void SetSystemGain(CString strGain) = 0;
    virtual void SetExposureTime(CString strExposure, int nRow) = 0;
    virtual double GetSystemGain() = 0;
    virtual void GetExposureTime(double& dBright, double& dDark) = 0;
    virtual void SetSensorSelector(CString strSensorSel) = 0;
    virtual void SetSensorGain(CString strSensorGain) = 0;
    virtual void GetSensorInfo(int& nSensorSelector, double& dSensorGain) = 0;
    virtual void GetRowGain(double& dRowGainBright, double& dRowGainDark) = 0;
    virtual void SetRowGain(CString strRowGain, int nRow) = 0;
    virtual void ResponseLeveling() = 0;
    virtual void Reset() = 0;

	virtual double GetFPGATemperature() = 0;
    virtual BOOL GetCameraConnectCheck() = 0;

	virtual void SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh) = 0;

#ifdef _TEST_SPLITTERREOVED_
	unsigned char* GetFrameAddress_test(int64_t frameIndex);
#endif
protected:
	virtual bool OnTrigger() override;		// IPVM::SimpleThreadWorker
	virtual void OnStart() = 0;
	virtual void OnRequestStop() = 0;

	static void CallbackOnOneFrameGrabbed(void *userData, DWORD grabCount, void *param1, void *param2);
	void CallbackOnOneFrameGrabbed(DWORD grabCount, void *param1, void *param2);

	void InitializeCircularBuffer();

protected:
    static const UINT m_circularBufferCount = 10;

	IPVM::LoggerInterface& m_logger;
	SharedMemory::ImageQueuePusher* m_queuePusher;

	CRITICAL_SECTION m_cs;

	const int m_cameraIndex;

	bool	m_threadWaitKilling;
	UINT	m_horizontalSize;
	UINT	m_verticalSize;

	unsigned char *m_circularBuffer[m_circularBufferCount];
    unsigned char* m_circularSecBuffer[m_circularBufferCount];
    unsigned char* m_TotalTopBuffer[1];
    unsigned char* m_TotalBottomBuffer[1];
	
	float m_frameGrabbedTimes_ms[m_circularBufferCount];

	int64_t m_headFrameIndex;		// Closed boundary
	int64_t m_tailFrameIndex;		// Opened boundary
	int64_t m_maxUsedFrames;
	bool m_firstFrameAfterStart;

	GrabMode m_grabMode;
	CALLBACK_ON_FRAME_UPDATED	m_callbackFunc[(long)GrabMode::END];
	void*						m_callbackData[(long)GrabMode::END];
	bool m_sensorImageMirror;
	MachineType m_machineType;
};

