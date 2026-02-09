#pragma once
#include "SapClassBasic.h"
#include "GrabExecutor.h"
#include "Gadget/SerialPort.h"
#include "Gadget/Socket.h"
#include "manuser.h"

class GrabExecutor_Sapera : public GrabExecutor
{
	SapAcquisition* m_Acq;
	SapBuffer* m_Buffer;
	SapTransfer* m_Xfer;

	SapAcqDevice* m_pDevice;
    SapFeature* m_pFeature;

	int m_iSizeX;
	int m_iSizeY;
	static void XferCallback(SapXferCallbackInfo* pInfo);

	int64_t NumberOfFrames;
	int64_t m_nCallbackCnt;
	long m_nTimeOut;

public:
	GrabExecutor_Sapera(void) = delete;
	GrabExecutor_Sapera(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex, const int channelIndex, LPCTSTR ccfPathName, MachineType machineType);
	virtual ~GrabExecutor_Sapera();

	GrabExecutor_Sapera& operator=(const GrabExecutor_Sapera& rhs) = delete;

	virtual void SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount = -1) override;
    virtual void SetVerticalLength(long nPageLength) override; // GrabExecutor
    virtual void SetSystemGain(CString strGain) override;
    virtual void SetExposureTime(CString strExposure, int nRow) override;
    virtual double GetSystemGain() override;
    virtual void GetExposureTime(double& dBright, double& dDark) override;
    virtual void SetSensorSelector(CString strSensorSel) override;
    virtual void SetSensorGain(CString strSensorGain) override;
    virtual void GetSensorInfo(int& nSensorSelector, double& dSensorGain) override;
    virtual void GetRowGain(double& dRowGainBright, double& dRowGainDark) override;
    virtual void SetRowGain(CString strRowGain, int nRow) override;
    virtual void ResponseLeveling() override;
    virtual void Reset() override;
    virtual double GetFPGATemperature() override;
    virtual BOOL GetCameraConnectCheck() override;

	virtual void SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh) override;

protected:
	virtual bool OnTrigger() override;		// IPVM::GrabExecutor

	virtual void OnStart() override;
	virtual void OnRequestStop() override;	// GrabExecutor
    

public:
	BOOL IsAvailable();
	BOOL OpenPort();
	BOOL SingleGrab_Begin(BYTE* pbyImageBuff, long nTimeout);
	BOOL SingleGrab_End(/*BYTE* pbyImageBuff*/);
	BOOL SetDirection(long nDirection, BOOL bUseGrabber);
	BOOL SetColorGap(long nValue, BOOL bUseGrabber);
	BOOL SetPageLength(long nPageLength);
	BOOL SetTriggerType(BOOL flg);
	BOOL SetExposureTime(float fHigh, float fLow);
	BOOL GetImageInfo(long& nwidth, long& nheight, long& nbpp);
	void GetLastError(CString& strErr);
	static UINT threadPushBuffer(LPVOID pParam);

	void ResetShaftEncoder();

	bool m_bgrabon;
	int64_t m_cellTirggerPosInPixel;
	bool m_useVirtualCellID;

	CString m_strServerName;
    char m_chServerName[CORSERVER_MAX_STRLEN];

	

private:
	const int m_grabberIndex;
	const int m_channelIndex;
	int m_cfIndex;

	float m_fPrevGrabTime;

	IPVM::LoggerInterface& m_logger;

};

