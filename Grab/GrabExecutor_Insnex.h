#pragma once

#include "GrabExecutor.h"
#include "../Insnex/ins_chvs_ctrl.hpp"
#include"../Insnex/ins_chvs_fusion_ctrl.h"

class INS_CHVS_DLL_API InsCHVSCamera;
class INS_CHVS_FUSION_DLL_API InsCHVS_StorageFusionImage;

#define BUFFER_CNT 5;



class GrabExecutor_Insnex : public GrabExecutor
{
public:
    GrabExecutor_Insnex(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex,
        const int channelIndex, LPCTSTR ccfPathName, MachineType machineType);
    ~GrabExecutor_Insnex(void);

    GrabExecutor_Insnex& operator=(const GrabExecutor_Insnex& rhs) = delete;

    virtual void SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh) override;

    //virtual void GetExposureTime(double& dBright, double& dDark) override;
    virtual void SetVerticalLength(long nPageLength) override;
    virtual void SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount = -1) override;

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

private:
    //void* m_pParent;
    long m_nGrabCnt;
    //RGBConverter rgbConverter;

protected:
    virtual bool OnTrigger() override; // IPVM::GrabExecutor
    virtual void OnStart() override;
    virtual void OnRequestStop() override; // GrabExecutor

public:
    static void FrameCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser);
    void SetLedTriggerMode(int nMode);
    InsCHVS_LED_TriggerMode GetLedTriggerMode();

    void SetTriggerSource1(int nMode, float fLineFreq, long nScanStep);
    void SetTriggerSource2(int nMode, float fLineFreq, long nScanStep);
    void SetROI(long nXstart, long nXend);
    void SetFlipHorizontal(BOOL bUse);
    //void GetTriggerSource1();
    //void GetTriggerSource2();

    InsCHVSCamera* m_pInsnex_impl;
    InsCHVS_StorageFusionImage* m_pInsnexFusion_impl;

    long m_iWidth;
    long m_iHeight;
    long m_nImgbuff;

    bool m_bgrabon;

    const int m_CameraIndex;
    const int m_grabberIndex;
    const int m_channelIndex;
    int m_cfIndex;

    float m_fPrevGrabTime;

    int64_t NumberOfFrames;
    int64_t m_nCallbackCnt;
    long m_nTimeOut;

    IPVM::LoggerInterface& m_logger;
};
