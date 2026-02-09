#pragma once

#include "GrabExecutor.h"

class GrabExecutor_iGrab : public GrabExecutor
{
public:
	GrabExecutor_iGrab() = delete;
	GrabExecutor_iGrab(const GrabExecutor_iGrab &rhs) = delete;
	GrabExecutor_iGrab(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex, const int channelIndex, LPCTSTR cfPathName);
	virtual ~GrabExecutor_iGrab();

	GrabExecutor_iGrab &operator=(const GrabExecutor_iGrab &rhs) = delete;

	virtual void SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh) override;

	virtual void SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount = -1) override;
    virtual void SetVerticalLength(long nPageLength);
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

protected:
	virtual bool OnTrigger() override;		// IPVM::GrabExecutor

	virtual void OnStart() override;
	virtual void OnRequestStop() override;	// GrabExecutor

private:
	const int m_grabberIndex;
	const int m_channelIndex;
	int m_cfIndex;
};

