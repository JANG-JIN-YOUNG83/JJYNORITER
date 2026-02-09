#include "stdafx.h"
#include "GrabExecutor_Insnex.h"
#include "../Insnex/ins_chvs_params.hpp"
#include "../Insnex/ins_chvs_fusion_ctrl.h"
#include "../Insnex/ins_chvs_fusion_def.h"
#include <string>

#include "../Module/SharedImageQueue/ImageQueuePusher.h"
#include "Base/LoggerInterface.h"
//#include <Types/Image_8u_C1.h>
//#include <Types/Image_8u_C3.h>



#ifdef _DEBUG
#pragma comment(lib, "../Insnex/lib/InsCHVSControl.lib")
#pragma comment(lib, "../Insnex/lib/opencv_world460d.lib")
#else
#pragma comment(lib, "../Insnex/lib/InsCHVSControl.lib")
#pragma comment(lib, "../Insnex/lib/opencv_world460.lib")
#endif

/* @brief：Is it an LXM series camera, 1800/900dpi
 */
int extractDPI(InsCHVS_DeviceType type)
{
    return ((uint32_t)type & 0xF0) >> 4;
}
bool is_DPI_EqualTo_1800_900(InsCHVS_DeviceType type)
{
    return extractDPI(type) == INS_DPI_900 || extractDPI(type) == INS_DPI_1800 || extractDPI(type) == INS_DPI_3600;
}
bool isLXMSeriesCamera;

/* @brief：Callback function for collecting images
 */
void CALLBACK FrameReadyCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser);
void CALLBACK StartCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser);
void CALLBACK StopCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser);
void CALLBACK FrameLostCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser);
void CALLBACK TimeOutCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser);
void CALLBACK CameraRemoveCallback(void* pContext);

GrabExecutor_Insnex::GrabExecutor_Insnex(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex,
    const int channelIndex, LPCTSTR ccfPathName, MachineType machineType)
    : GrabExecutor(logger, cameraIndex)
    , m_grabberIndex(grabberIndex)
    , m_channelIndex(channelIndex)
    , m_cfIndex(-1)
    , m_bgrabon(false)
    , m_logger(logger)
    , m_nCallbackCnt(1)
    , m_CameraIndex(cameraIndex + 1)
{

    m_machineType = machineType;

    INS_Result ret = INS_Result::INS_OK;
   
    m_pInsnex_impl = nullptr;
    m_pInsnex_impl = new InsCHVSCamera();
  /*  
    m_pInsnexFusion_impl = nullptr;
    m_pInsnexFusion_impl = new InsCHVSFusion();
    */
  

    
     /***************** List devices *****************************/
    //
    /* Initialize the acquisition card */
    //InsCHVSCamera::InsCHVS_Initialize();
    m_pInsnex_impl->InsCHVS_Initialize();
    //
    /* local variable */
    InsCHVS_DeviceInfoList deviceInfoListScanned;
    InsCHVS_DeviceInfo deviceInfoOpened;
    //InsCHVSCamera camera;
    /* List devices */
    m_pInsnex_impl->InsCHVS_FindDevice(&deviceInfoListScanned);
    //printDevicesInfo(deviceInfoListScanned);
    //
    /***************** Open device ******************************/
    //
    int UserControl_ChooseIndex = m_CameraIndex;
    deviceInfoListScanned.DeviceCount;
    
    //연결되어 있는 디바이스가 일단 하나라고 가정 함, 2개 이상 일경우 수정 필요
    /*if (UserControl_ChooseIndex <= 0 || UserControl_ChooseIndex > deviceInfoListScanned.DeviceCount)
    {
        return;
    }*/
    CString strDeviceMod = _T("");
    switch (deviceInfoListScanned.DeviceInfo[m_CameraIndex - 1].TransLayerType)
    {
        case INS_GIGE_DEVICE:
            strDeviceMod = deviceInfoListScanned.DeviceInfo[m_CameraIndex - 1].SpecialInfo.GigeInfo[0].Ip;
            break;
        case INS_CAMERALINK_DEVICE:
            strDeviceMod = deviceInfoListScanned.DeviceInfo[m_CameraIndex - 1].SpecialInfo.CamLInfo[0].COM;
            break;
        default:
            break;
    }


    ret = m_pInsnex_impl->InsCHVS_Cmd_Open(m_CameraIndex - 1);

    /* After successful connection, obtain information */
    m_pInsnex_impl->InsCHVS_Get_DevInfo(deviceInfoOpened);
    //printDeviceInfo(deviceInfoOpened);
    //
    /************************************************************/
    
    //CLOSE 시점을 언제로 할지? 또는 매번 열고 닫고 해줘야하는지? 화인 필요
    //ret = m_pInsnex_impl->InsCHVS_Cmd_Close();

    /* Load the ICF configuration file of the camera  */
    /* Different models of cameras cannot load each other */
    CStringA strUserControl_ConfigFilePath = "C:/Intekplus/insnex_1.icf";
    ret = m_pInsnex_impl->InsCHVS_Load_ConfigFile(
        strUserControl_ConfigFilePath.GetBuffer(), m_nTimeOut); // Set as needed
   
                                                                
    ret = m_pInsnex_impl->InsCHVS_Set_DevPrm(INS_PRM_BUFFER_STROBING_MODE, INS_VAL_BUFFER_STROBING_MULTIPLE);  
    std::vector<float32_t> UserControl_Exposure(3, 0);
    UserControl_Exposure[0] = 10;
    UserControl_Exposure[1] = 9;
    UserControl_Exposure[2] = 5;

    ret = m_pInsnex_impl->InsCHVS_Set_LED_ExposureTime2(UserControl_Exposure[0], UserControl_Exposure[1], m_nTimeOut);
   
    int UserControl_ROI_xStart = 5700;
    int UserControl_ROI_xEnd = 18600;
    InsCHVS_FuncEnable UserControl_ROI_Enable = InsCHVS_FuncEnable::Ins_Enable;
     ret = m_pInsnex_impl->InsCHVS_Set_ROI_Rangle(
        UserControl_ROI_xStart, UserControl_ROI_xEnd, m_nTimeOut); // Set as needed
    //CHECK("InsCHVS_Set_ROI_Rangle: ", ret);
     ret = m_pInsnex_impl->InsCHVS_Set_ROI_Enable(UserControl_ROI_Enable, m_nTimeOut); // Set as needed
    //CHECK("InsCHVS_Set_ROI_Enable: ", ret);

    /*************** Enable fusion *************************************/
    //
    InsCHVS_PMSFusionParam param;
 /*
    ret = camera.InsCHVS_Set_PMSFusion_Param(&param);
   
    ret = camera.InsCHVS_Get_PMSFusion_Param(&param);
    ret = camera.InsCHVS_Set_PMSFusion_Enable(InsCHVS_FuncEnable::Ins_Enable);*/
   
    //
    /************************************************************/
   
    
    
    //m_iWidth = getWidth();
    //m_iHeight = getHeight();
    //ScanStep
    m_nTimeOut = 1000;
    m_fPrevGrabTime = -1.f;
    m_nGrabCnt = 20;
    NumberOfFrames = m_nGrabCnt;
    ///////// 사이즈 0이 맞는가???
    uint32_t bufferHeight;
    uint32_t bufferImageWidth;
    m_pInsnex_impl->InsCHVS_Get_DevPrm(INS_PRM_BUFFER_ROI_HEIGHT, &bufferHeight);
    m_pInsnex_impl->InsCHVS_Get_DevPrm(INS_PRM_BUFFER_ROI_IMG_WIDTH, &bufferImageWidth);

    m_iWidth = bufferImageWidth;
    m_iHeight = bufferHeight/2;

    //m_iWidth = GetHorizontalSize();
    //m_iHeight = GetVerticalSize();

    m_verticalSize = m_iHeight;
    m_horizontalSize = m_iWidth;

    m_nImgbuff = BUFFER_CNT;
    if (GetCircularBufferCount() <= m_nImgbuff)
    {
        m_nImgbuff = GetCircularBufferCount();
    }

    InitializeCircularBuffer();





    //콜백 등록, 나중에 필요한것 살려서 쓰기.
    m_pInsnex_impl->InsCHVS_RegisterCallback(INS_Event_FrameReady, FrameReadyCallBack, this);
    m_pInsnex_impl->InsCHVS_RegisterCallback(INS_Event_GrabStart, StartCallBack, this);
    m_pInsnex_impl->InsCHVS_RegisterCallback(INS_Event_GrabStop, StopCallBack, this);
    m_pInsnex_impl->InsCHVS_RegisterCallback(INS_Event_FrameLost, FrameLostCallBack, this);
    m_pInsnex_impl->InsCHVS_RegisterCallback(INS_Event_TimeOut, TimeOutCallBack, this);
    m_pInsnex_impl->InsCHVS_RegisterCallback_DeviceRemove(CameraRemoveCallback, &m_pInsnex_impl);

    m_queuePusher = new SharedMemory::ImageQueuePusher(cameraIndex);
}

GrabExecutor_Insnex::~GrabExecutor_Insnex(void)
{
    if (m_pInsnex_impl != nullptr)
    {
        //InsCHVS_RunState runState;
        //m_pInsnex_impl->InsCHVS_Get_RunState(&runState);
        //if (runState == InsCHVS_RunState::Running)
        //{
        //    m_pInsnex_impl->InsCHVS_Cmd_Stop();
        //}
        //m_pInsnex_impl->InsCHVS_Cmd_Close();
        //터미네이트가 삭제까지 하는지 알수없음, 확인해서 delete 두던가 빼기
        m_pInsnex_impl->InsCHVS_Terminate();
        delete m_pInsnex_impl;
    }
}

void CALLBACK FrameReadyCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    //UNUSED(pUser);
    GrabExecutor_Insnex* pParent = (GrabExecutor_Insnex*)pUser;
    
    //InsCHVSCamera* camera = (InsCHVSCamera*)handle;
    //InsCHVS_ProcessInfo* info = (InsCHVS_ProcessInfo*)pInfo;
    //InsCHVS_Buffer* buffer = (InsCHVS_Buffer*)pBuffer;

    pParent->FrameCallBack(handle, pInfo, pBuffer, pUser);
}

void GrabExecutor_Insnex::FrameCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    GrabExecutor_Insnex* pParent = (GrabExecutor_Insnex*)pUser;

    InsCHVSCamera* camera = (InsCHVSCamera*)handle;
    InsCHVS_ProcessInfo* info = (InsCHVS_ProcessInfo*)pInfo;
    InsCHVS_Buffer* buffer = (InsCHVS_Buffer*)pBuffer;

    void* pData_buf; // = pbyImageBuff;
    void* pData_secbuf; // = secpbyImageBuff;

    pData_buf = (void*)buffer->p_data;
    pData_secbuf = (void*)buffer->p_data2;

    //int circleBufIndex = (pParent->m_nCallbackCnt - 1) % m_circularBufferCount; // 그랩 개수라 1부터 시작하는 숫자라서....-1한다..
    long nSizeX = pParent->m_horizontalSize;
    long nSizeY = pParent->m_verticalSize;
    //임시로 카메라 자체 저장되는 크기 가져옴
    uint32_t bufferHeight;
    uint32_t bufferImageWidth;
    camera->InsCHVS_Get_DevPrm(INS_PRM_BUFFER_ROI_HEIGHT, &bufferHeight);
    camera->InsCHVS_Get_DevPrm(INS_PRM_BUFFER_ROI_IMG_WIDTH, &bufferImageWidth);
    nSizeX = (long)bufferImageWidth;
    nSizeY = (long)bufferHeight/2;

    const int64_t frameIndex = pParent->m_nCallbackCnt - 1;
    BYTE* pDst = (BYTE*)pParent->m_circularBuffer[frameIndex % m_circularBufferCount];
    BYTE* psecDst = (BYTE*)pParent->m_circularSecBuffer[frameIndex % m_circularBufferCount];
  
    long size = nSizeX * nSizeY;
    
    if (!(pParent->m_horizontalSize == 0 && pParent->m_verticalSize == 0))
    {
        memcpy(pDst, pData_buf, size);
        memcpy(psecDst, ((BYTE*)pData_secbuf), size);
    }
    

    pParent->CallbackOnOneFrameGrabbed(pParent, pParent->m_nCallbackCnt, NULL, NULL);
    pParent->m_nCallbackCnt++;
   
    TRACE("Callback Cnt %d\n", frameIndex);
}

void CALLBACK StartCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    //std::cout << "************* START ************* " << std::endl;
}
void CALLBACK StopCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    //std::cout << "************* STOP ************** " << std::endl;
}
void CALLBACK FrameLostCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    //std::cout << "************* FrameLost ********* " << std::endl;
}
void CALLBACK TimeOutCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    //std::cout << "************* TimeOut *********** " << std::endl;
}

void CALLBACK CameraRemoveCallback(void* pContext)
{
    //std::cout << "************* CameraRemoveCallback *********** " << std::endl;
    InsCHVSCamera* camera = (InsCHVSCamera*)pContext;
    InsCHVS_RunState runState;
    camera->InsCHVS_Get_RunState(&runState);
    if (runState == InsCHVS_RunState::Running)
    {
        camera->InsCHVS_Cmd_Stop();
    }
    camera->InsCHVS_Cmd_Close();
}


void GrabExecutor_Insnex::SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount)
{
    if (m_queuePusher != nullptr)
    {
        CString strPath;

        CFileFind find;

        if (m_cameraIndex == 0)
        {
            strPath.Format(_T("%s\\UP"), targetFolder);

            if (!find.FindFile(strPath))
                CreateDirectory(strPath, NULL);
        }
        else
        {
            strPath.Format(_T("%s\\DOWN"), targetFolder);

            if (!find.FindFile(strPath))
                CreateDirectory(strPath, NULL);
        }

        if (!m_queuePusher->IsPusherBusy())
        {
            m_queuePusher->SetPusherBusy();
            m_queuePusher->Save(strPath, saveMaxCount);
            m_queuePusher->SetPusherIdle();
        }
        else
            m_queuePusher->Save(strPath, saveMaxCount);
    }
}

bool GrabExecutor_Insnex::OnTrigger()
{
    if (__super::OnTrigger() == false)
    {
        return false;
    }

    //PutGrabbuffer();
    long frameIndex = 1;
    return true;
}

void GrabExecutor_Insnex::OnStart()
{
    INS_Result ret = INS_Result::INS_OK;

    InsCHVS_RunState runState;
    m_pInsnex_impl->InsCHVS_Get_RunState(&runState);
    if (runState == InsCHVS_RunState::Running)
    {
        return;
    }
    else
    {
        m_bgrabon = TRUE;
        ret = m_pInsnex_impl->InsCHVS_Cmd_Start();
    }
}

void GrabExecutor_Insnex::OnRequestStop()
{
    INS_Result ret = INS_Result::INS_OK;

    m_bgrabon = FALSE;
    ret = m_pInsnex_impl->InsCHVS_Cmd_Stop();
    
    m_pInsnex_impl->InsCHVS_Set_DevPrm(INS_PRM_BUFFER_CLEAR, 1); // Clear the internal buffer
}

void GrabExecutor_Insnex::SetVerticalLength(long nPageLength)
{
    // nPageLength와 n값이 같은지 확인
    long nTotalLine = nPageLength * 28.5;


    INS_Result ret = INS_Result::INS_OK;
    /***************** Set parameters of images ******************************/
    //최소 64이상, 길이 설정 필요
    uint32_t UserControl_ImageHeight = nPageLength;
    //m_pInsnex_impl->InsCHVS_Cmd_Open(m_CameraIndex);
    ret = m_pInsnex_impl->InsCHVS_Set_Img_TransHeight(UserControl_ImageHeight, m_nTimeOut);
    //m_pInsnex_impl->InsCHVS_Cmd_Close();
}

void GrabExecutor_Insnex::SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh) // Row 값 추가
{
    INS_Result ret = INS_Result::INS_OK;

    //채널 몇개 쓸건지, 또는 어떤 채널에 값을 집어넣을건지 
    int nCh = nCntCh;

    //GET 함수 하나 만들어서 불러와야겠지?
    InsCHVS_LED_TriggerMode UserControl_LedMode = GetLedTriggerMode();

    /**************** Set parameters of exposure time ******************************/
    //
    /* All frequency related parameter settings will affect the maximum exposure value, such as the line frequency of trigger source 1 */
    //
    /* Maximum exposure time */
    float32_t maxExposureTime = 0;
    m_pInsnex_impl->InsCHVS_Get_TimeLine_Max(&maxExposureTime, m_nTimeOut);
    
    /* Exposure value can be freely set by users */
    std::vector<float32_t> UserControl_Exposure(3, 0);
    UserControl_Exposure[0] = 15;
    UserControl_Exposure[1] = 10;
    UserControl_Exposure[2] = 5;
    if (vecstrExposureValue.size() == 0 || vecstrExposureValue.size() < 0)
    {
        return;
    }

    for (auto nidx = 0; nidx < vecstrExposureValue.size(); nidx++)
    {
        if (_ttof(vecstrExposureValue[nidx]) > maxExposureTime)
        {
            UserControl_Exposure[nidx] = maxExposureTime - 10;
        }
        else
        {
            UserControl_Exposure[nidx] = _ttof(vecstrExposureValue[nidx]);
        }
        
    }

    //원하는 노출시간 설정 0 1 2 각각 분할하여 어떤 Mode를 사용해 led on 할건지에 따라 설정을 다르게 해서 사용
    /* Set exposure */
    /* When the LED trigger mode is LED_SimultaneousDualBrightness (No time-division strobe, positive light)
                                     or LED_SacklightOnly (No time-division strobe, backlight), call InsCHVS_Set_LEDExposeTime1 */
    /* When the LED trigger mode is LED_SeparateTimedFlashingBrightness(Two channel time-division strobe), call InsCHVS_Set_LEDExposeTime2 */
    /* When the LED trigger mode is LED_TripleIndependentBrightness(Three channel time-division strobe), call InsCHVS_Set_LEDExposeTime3 */
    switch (UserControl_LedMode)
    {
        case InsCHVS_LED_TriggerMode::LED_SimultaneousDualBrightness:
        case InsCHVS_LED_TriggerMode::LED_BacklightOnly:
            ret = m_pInsnex_impl->InsCHVS_Set_LED_ExposureTime1(UserControl_Exposure[0], m_nTimeOut);
            //CHECK("Set_LED_ExposureTime1:", ret);
            break;
        case InsCHVS_LED_TriggerMode::LED_SeparateTimedFlashingBrightness:
            ret = m_pInsnex_impl->InsCHVS_Set_LED_ExposureTime2(UserControl_Exposure[0], UserControl_Exposure[1], m_nTimeOut);
            //CHECK("Set_LED_ExposureTime2:", ret);

            /* When the LED trigger mode is time-division strobe, this parameter can be set to split the output image into two images */
            /* Requirement for activating this mode: The row height must be an integer multiple of 2 strobe channels */
            ret = m_pInsnex_impl->InsCHVS_Set_DevPrm(INS_PRM_BUFFER_STROBING_MODE, INS_VAL_BUFFER_STROBING_MULTIPLE);
            //CHECK("Set_DevPrm(INS_VAL_BUFFER_STROBING_MULTIPLE):", ret);
            break;
        case InsCHVS_LED_TriggerMode::LED_TripleIndependentBrightness:
            ret = m_pInsnex_impl->InsCHVS_Set_LED_ExposureTime3(
                UserControl_Exposure[0], UserControl_Exposure[1], UserControl_Exposure[2], m_nTimeOut);
            //CHECK("Set_LED_ExposureTime3:", ret);

            /* When the LED trigger mode is time-division strobe, this parameter can be set to split the output image into three images */
            /* Requirement for activating this mode: The row height must be an integer multiple of 3 strobe channels */
            ret = m_pInsnex_impl->InsCHVS_Set_DevPrm(INS_PRM_BUFFER_STROBING_MODE, INS_VAL_BUFFER_STROBING_MULTIPLE);
            //CHECK("Set_DevPrm(INS_VAL_BUFFER_STROBING_MULTIPLE):", ret);
            break;
        default:
            break;
    }
}

//1, 3, 7, 4 사용
void GrabExecutor_Insnex::SetLedTriggerMode(int nMode)
{
    INS_Result ret = INS_Result::INS_OK;
    InsCHVS_LED_TriggerMode UserControl_LedMode;
    
    /***************** Light source mode ****************************************/
    /* Light source mode */
    switch (nMode)
    {
        case (long)InsCHVS_LED_TriggerMode::LED_SimultaneousDualBrightness:
            UserControl_LedMode = InsCHVS_LED_TriggerMode::LED_SimultaneousDualBrightness;
            break;
        case (long)InsCHVS_LED_TriggerMode::LED_SeparateTimedFlashingBrightness:
            UserControl_LedMode = InsCHVS_LED_TriggerMode::LED_SeparateTimedFlashingBrightness;
            break;
        case (long)InsCHVS_LED_TriggerMode::LED_TripleIndependentBrightness:
            UserControl_LedMode = InsCHVS_LED_TriggerMode::LED_TripleIndependentBrightness;
            break;
        case (long)InsCHVS_LED_TriggerMode::LED_BacklightOnly:
            UserControl_LedMode = InsCHVS_LED_TriggerMode::LED_BacklightOnly;
            break;
        default:
            break;
    }

    /* Set light source mode */
    ret = m_pInsnex_impl->InsCHVS_Set_LED_TriggerMode(UserControl_LedMode, m_nTimeOut);
}

InsCHVS_LED_TriggerMode GrabExecutor_Insnex::GetLedTriggerMode()
{
    INS_Result ret = INS_Result::INS_OK;
    InsCHVS_LED_TriggerMode pLEDTriggerMode;

    ret = m_pInsnex_impl->InsCHVS_Get_LED_TriggerMode(&pLEDTriggerMode, m_nTimeOut);

    if (ret == INS_Result::INS_OK)
        return pLEDTriggerMode;
    else
        return (InsCHVS_LED_TriggerMode)0x00;
}

//Mode:0, 1, 2, 3, 255 사용가능, 트리거 소스 설정 전 LED트리거모드가 먼저 설정되어야 함
void GrabExecutor_Insnex::SetTriggerSource1(int nMode, float fLineFreq, long nScanStep)
{
    INS_Result ret = INS_Result::INS_OK;
    InsCHVS_LineTriggerSource UserControl_Trigger1_Mode;
    /***************** Set parameters of Trigger source 1 ******************************/
    /* Corresponding to the <Capture Setting> on the upper computer interface */
    /* Trigger source 1: Set mode to 'internal clock' */
    /* When used by users, 'trigger source 1' needs to be set as an internal clock */
    switch (nMode)
    {
        case (long)InsCHVS_LineTriggerSource::Internal_Clock:
            UserControl_Trigger1_Mode = InsCHVS_LineTriggerSource::Internal_Clock;
            break;
        case (long)InsCHVS_LineTriggerSource::External_IO:
            UserControl_Trigger1_Mode = InsCHVS_LineTriggerSource::External_IO;
            break;
        case (long)InsCHVS_LineTriggerSource::External_Encoder:
            UserControl_Trigger1_Mode = InsCHVS_LineTriggerSource::External_Encoder;
            break;
        case (long)InsCHVS_LineTriggerSource::Fake_Pattern:
            UserControl_Trigger1_Mode = InsCHVS_LineTriggerSource::Fake_Pattern;
            break;
        case (long)InsCHVS_LineTriggerSource::No_active:
            UserControl_Trigger1_Mode = InsCHVS_LineTriggerSource::No_active;
            break;
        default:
            break;
    }
    ret = m_pInsnex_impl->InsCHVS_Set_Acq_TrigSource(UserControl_Trigger1_Mode, m_nTimeOut);

    /* Trigger source 1: Set the line frequency of the internal clock, note that there is a maximum line frequency limit */
    /* The maximum line frequency is obtained by calling the 'InsCHVS_Get_SevPrm' function using 'INS_PRM_PERIOD_DPI_MAX' */
    /* The range of maximum line frequency will be affected by the light source mode. So first set the light source mode, and then set the internal clock line frequency*/
    /* When setting, this line frequency should be greater than the speed at which the encoder runs, otherwise it will be limited by the lower line frequency set*/
    float periodMax = 0;
    m_pInsnex_impl->InsCHVS_Get_DevPrm(INS_PRM_PERIOD_DPI_MAX, &periodMax);
    //최소 15이상 사용
    

    if (UserControl_Trigger1_Mode == InsCHVS_LineTriggerSource::Internal_Clock)
    {
        
        float UserControl_Trigger1_LineFrequency = fLineFreq;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq_Intern_TrigPeriod(UserControl_Trigger1_LineFrequency, m_nTimeOut);
        /* Set the number of image lines to be collected in a single run. Setting the number of lines to 0 indicates continuous image collection */
        //한번 동작에서 몇번의 이미지를 모을건지, 만약 0으로 값을 넣으면 계속해서(stop 하기전 까지 계속 인듯?) 얻는것으로 설정
        //long nScanStep = nScanStep;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq_Intern_TrigNums(nScanStep, m_nTimeOut);
    }
    else if (UserControl_Trigger1_Mode == InsCHVS_LineTriggerSource::External_Encoder)
    {
        /* When using external encoder signals, use this mode */
        /* If the image cannot be displayed in this mode, please switch to the internal clock mode to check if the image is displayed;
               If the image appears, please check the parameter settings of the encoder signal and encoder mode */
        /* Combination counting mode and travel direction have matching restrictions */
        /* When selecting 'PhA_RisingEdge_oneWay', it must be combined with 'Forward_Scan' */
        /* When 'PhA_RisingEdge_oneWay' is not selected, the 'Forward_Scan' combination cannot be selected either */
        InsCHVS_Encoder_CountMode UserControl_Trigger1_Enocder_CountMode
            = InsCHVS_Encoder_CountMode::PhA_RisingEdge_OneWay;
        InsCHVS_Encoder_TriggerMode UserControl_Trigger1_Enocder_TravelDirectionMode
            = InsCHVS_Encoder_TriggerMode::Forward_Scan;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq_Encoder_CountMode(UserControl_Trigger1_Enocder_CountMode, m_nTimeOut);
        ret = m_pInsnex_impl->InsCHVS_Set_Acq_Encoder_TriggerMode(
            UserControl_Trigger1_Enocder_TravelDirectionMode, m_nTimeOut);
        //
        uint32_t UserControl_Trigger1_Enocder_IgnoreCount = 0;
        float32_t UserControl_Trigger1_Enocder_FilteringTimeWidth = 1.28; // us
        uint32_t UserControl_Trigger1_InputDivide = 1; // Divide the external encoder signal, Greater than 1
        InsCHVS_FuncEnable UserControl_Trigger1_Enable_InputMultiple
            = InsCHVS_FuncEnable::Ins_Enable; // After enabling, multiple is effective
        uint32_t UserControl_Trigger1_InputMultiple = 1; // Multiple the external encoder signal, Greater than 1
        //camera.InsCHVS_Set_Acq2_Encoder_Ignore(UserControl_Trigger2_Enocder_IgnoreCount, TIMEOUT); // Set as needed. Generally, default is sufficient.
        //camera.InsCHVS_Set_Acq2_Encoder_FilterWidth(UserControl_Trigger2_Enocder_FilteringTimeWidth, TIMEOUT); // Set as needed. Generally, default is sufficient.
        //camera.InsCHVS_Set_Acq2_Encoder_InputDiv(UserControl_Trigger2_InputDivide, TIMEOUT); // Set as needed
        //camera.InsCHVS_Set_Acq2_Encoder_Enable_InputMultiple(UserControl_Trigger2_Enable_InputMultiple, TIMEOUT); // Set as needed
        //camera.InsCHVS_Set_Acq2_Encoder_InputMultiple(UserControl_Trigger2_InputMultiple, TIMEOUT); // Set as needed
    }
    else if (UserControl_Trigger1_Mode == InsCHVS_LineTriggerSource::External_IO)
    {
        /* When using external IO signals, use this mode */
        /* If the image cannot be displayed in this mode, please switch to the internal clock mode to check if the image is displayed;
               If the image appears, please check if the IO signal has a 20V voltage and IO mode parameter settings */
        //
        float32_t UserControl_Trigger1_IO_LineFrequency = fLineFreq;
        InsCHVS_IO_TriggerMode UserControl_Trigger1_IO_TriggerMode = InsCHVS_IO_TriggerMode::Input0_Start_Input1_End;
        uint32_t UserControl_Trigger1_IO_TrigNums = nScanStep;
        uint32_t UserControl_Trigger1_IO_OutputDelay = 0;
        float32_t UserControl_Trigger1_IO_FilteringTimeWidth = 1.28; //us
        uint32_t UserControl_Trigger1_IO_InputDivide = 1;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq_IO_TrigPeriod(UserControl_Trigger1_IO_LineFrequency, m_nTimeOut);
        ret = m_pInsnex_impl->InsCHVS_Set_Acq_IO_Mode(UserControl_Trigger1_IO_TriggerMode, m_nTimeOut);
        ret = m_pInsnex_impl->InsCHVS_Set_Acq_IO_TrigNums(UserControl_Trigger1_IO_TrigNums, m_nTimeOut);
        // camera.InsCHVS_Set_Acq2_IO_OutputDelay(UserControl_Trigger2_IO_OutputDelay, TIMEOUT); // Set as needed
        // camera.InsCHVS_Set_Acq2_IO_FilterWidth(UserControl_Trigger2_IO_FilteringTimeWidth, TIMEOUT); // Set as needed
        // camera.InsCHVS_Set_Acq2_IO_InputDiv(UserControl_Trigger2_IO_InputDivide, TIMEOUT); // Set as needed
    }
}

//Mode:0, 1, 2, 3, 255 사용가능, 2번째 트리거의 경우 1번보다 Freq가 빠르면 안됨
void GrabExecutor_Insnex::SetTriggerSource2(int nMode, float fLineFreq, long nScanStep)
{
    INS_Result ret = INS_Result::INS_OK;
    InsCHVS_LineTriggerSource UserControl_Trigger2_Mode;
    /***************** Set parameters of Trigger source 1 ******************************/
    /* Corresponding to the <Capture Setting> on the upper computer interface */
    /* Trigger source 1: Set mode to 'internal clock' */
    /* When used by users, 'trigger source 1' needs to be set as an internal clock */
    switch (nMode)
    {
        case (long)InsCHVS_LineTriggerSource::Internal_Clock:
            UserControl_Trigger2_Mode = InsCHVS_LineTriggerSource::Internal_Clock;
            break;
        case (long)InsCHVS_LineTriggerSource::External_IO:
            UserControl_Trigger2_Mode = InsCHVS_LineTriggerSource::External_IO;
            break;
        case (long)InsCHVS_LineTriggerSource::External_Encoder:
            UserControl_Trigger2_Mode = InsCHVS_LineTriggerSource::External_Encoder;
            break;
        case (long)InsCHVS_LineTriggerSource::Fake_Pattern:
            UserControl_Trigger2_Mode = InsCHVS_LineTriggerSource::Fake_Pattern;
            break;
        case (long)InsCHVS_LineTriggerSource::No_active:
            UserControl_Trigger2_Mode = InsCHVS_LineTriggerSource::No_active;
            break;
        default:
            break;
    }

    /* The maximum line frequency is obtained by calling the 'InsCHVS_Get_SevPrm' function using 'INS_PRM_PERIOD_DPI_MAX' */
    /* The range of maximum line frequency will be affected by the light source mode. So first set the light source mode, and then set the internal clock line frequency*/
    /* When setting, this line frequency should be greater than the speed at which the encoder runs, otherwise it will be limited by the lower line frequency set*/
    float periodMax = 0;
    m_pInsnex_impl->InsCHVS_Get_DevPrm(INS_PRM_PERIOD_DPI_MAX, &periodMax);

    /**************** Set parameters of Trigger source 2  ******************************/
    //
    /* Corresponding to the <Acquire Setting> on the upper computer interface */
    /* Trigger source 2: open trigger source 2 */
    ret = m_pInsnex_impl->InsCHVS_Set_Acq2_Enable(InsCHVS_FuncEnable::Ins_Enable, m_nTimeOut);
    //
    /* Choose the mode of external signals independently based on the scenario and requirements */
    //라인 트리거 소스 결정 : internal_clock(내부트리거), External_Encoder(외부엔코더-트리거위스로 추정), External_IO(전압 인가 시점부터 영상 획득 시작)
    ret = m_pInsnex_impl->InsCHVS_Set_Acq2_TrigSource(UserControl_Trigger2_Mode, m_nTimeOut);
    //
    if (UserControl_Trigger2_Mode == InsCHVS_LineTriggerSource::Internal_Clock)
    {
        /* When there are no external signals such as encoders or IO, this mode can be used to capture images with a fixed line frequency */
        /* The line frequency of trigger source 2 must not be greater than that of trigger source 1, otherwise it will run at the lower speed of the two */
        //외부신호(엔코더, i/o 등)로 트리거를 입력 받지 않을 시, 내부적으로 트리거 생성하여 동작시키게끔 해줌, 트리거 소스1 세팅보다 값이 크게 되면 더 느리게 동작 됨
        //소스 1보다 값을 크게 설정할 경우 문제 생김, 작게 설정
        float32_t UserControl_Trigger2_LineFrequency = fLineFreq;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq2_Intern_TrigPeriod(UserControl_Trigger2_LineFrequency, m_nTimeOut);
        /* Set the number of image lines to be collected in a single run. Setting the number of lines to 0 indicates continuous image collection */
        //한번 동작에서 몇번의 이미지를 모을건지, 만약 0으로 값을 넣으면 계속해서(stop 하기전 까지 계속 인듯?) 얻는것으로 설정
        //long nScanStep = nScanStep;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq2_Intern_TrigNums(nScanStep, m_nTimeOut);
    }
    else if (UserControl_Trigger2_Mode == InsCHVS_LineTriggerSource::External_Encoder)
    {
        /* When using external encoder signals, use this mode */
        /* If the image cannot be displayed in this mode, please switch to the internal clock mode to check if the image is displayed;
               If the image appears, please check the parameter settings of the encoder signal and encoder mode */
        /* Combination counting mode and travel direction have matching restrictions */
        /* When selecting 'PhA_RisingEdge_oneWay', it must be combined with 'Forward_Scan' */
        /* When 'PhA_RisingEdge_oneWay' is not selected, the 'Forward_Scan' combination cannot be selected either */
        InsCHVS_Encoder_CountMode UserControl_Trigger2_Enocder_CountMode
            = InsCHVS_Encoder_CountMode::PhA_RisingEdge_OneWay;
        InsCHVS_Encoder_TriggerMode UserControl_Trigger2_Enocder_TravelDirectionMode
            = InsCHVS_Encoder_TriggerMode::Forward_Scan;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq2_Encoder_CountMode(UserControl_Trigger2_Enocder_CountMode, m_nTimeOut);
        ret = m_pInsnex_impl->InsCHVS_Set_Acq2_Encoder_TriggerMode(
            UserControl_Trigger2_Enocder_TravelDirectionMode, m_nTimeOut);
        //
        uint32_t UserControl_Trigger2_Enocder_IgnoreCount = 0;
        float32_t UserControl_Trigger2_Enocder_FilteringTimeWidth = 1.28; // us
        uint32_t UserControl_Trigger2_InputDivide = 1; // Divide the external encoder signal, Greater than 1
        InsCHVS_FuncEnable UserControl_Trigger2_Enable_InputMultiple
            = InsCHVS_FuncEnable::Ins_Enable; // After enabling, multiple is effective
        uint32_t UserControl_Trigger2_InputMultiple = 1; // Multiple the external encoder signal, Greater than 1
        //camera.InsCHVS_Set_Acq2_Encoder_Ignore(UserControl_Trigger2_Enocder_IgnoreCount, TIMEOUT); // Set as needed. Generally, default is sufficient.
        //camera.InsCHVS_Set_Acq2_Encoder_FilterWidth(UserControl_Trigger2_Enocder_FilteringTimeWidth, TIMEOUT); // Set as needed. Generally, default is sufficient.
        //camera.InsCHVS_Set_Acq2_Encoder_InputDiv(UserControl_Trigger2_InputDivide, TIMEOUT); // Set as needed
        //camera.InsCHVS_Set_Acq2_Encoder_Enable_InputMultiple(UserControl_Trigger2_Enable_InputMultiple, TIMEOUT); // Set as needed
        //camera.InsCHVS_Set_Acq2_Encoder_InputMultiple(UserControl_Trigger2_InputMultiple, TIMEOUT); // Set as needed
    }
    else if (UserControl_Trigger2_Mode == InsCHVS_LineTriggerSource::External_IO)
    {
        /* When using external IO signals, use this mode */
        /* If the image cannot be displayed in this mode, please switch to the internal clock mode to check if the image is displayed;
               If the image appears, please check if the IO signal has a 20V voltage and IO mode parameter settings */
        //
        float32_t UserControl_Trigger2_IO_LineFrequency = fLineFreq;
        InsCHVS_IO_TriggerMode UserControl_Trigger2_IO_TriggerMode = InsCHVS_IO_TriggerMode::Input0_Start_Input1_End;
        uint32_t UserControl_Trigger2_IO_TrigNums = nScanStep;
        uint32_t UserControl_Trigger2_IO_OutputDelay = 0;
        float32_t UserControl_Trigger2_IO_FilteringTimeWidth = 1.28; //us
        uint32_t UserControl_Trigger2_IO_InputDivide = 1;
        ret = m_pInsnex_impl->InsCHVS_Set_Acq2_IO_TrigPeriod(UserControl_Trigger2_IO_LineFrequency, m_nTimeOut);
        ret = m_pInsnex_impl->InsCHVS_Set_Acq2_IO_Mode(UserControl_Trigger2_IO_TriggerMode, m_nTimeOut);
        ret = m_pInsnex_impl->InsCHVS_Set_Acq2_IO_TrigNums(UserControl_Trigger2_IO_TrigNums, m_nTimeOut);
        // camera.InsCHVS_Set_Acq2_IO_OutputDelay(UserControl_Trigger2_IO_OutputDelay, TIMEOUT); // Set as needed
        // camera.InsCHVS_Set_Acq2_IO_FilterWidth(UserControl_Trigger2_IO_FilteringTimeWidth, TIMEOUT); // Set as needed
        // camera.InsCHVS_Set_Acq2_IO_InputDiv(UserControl_Trigger2_IO_InputDivide, TIMEOUT); // Set as needed
    }
}

void GrabExecutor_Insnex::SetROI(long nXstart, long nXend)
{
    INS_Result ret = INS_Result::INS_OK;
    /* ROI setting */
    //
    /* Set up before starting image acquisition; Set ROI range:*/
    uint32_t UserControl_ROI_xStart = nXstart;
    uint32_t UserControl_ROI_xEnd = nXend;
    InsCHVS_FuncEnable UserControl_ROI_Enable = InsCHVS_FuncEnable::Ins_Enable;
    ret = m_pInsnex_impl->InsCHVS_Set_ROI_Rangle(UserControl_ROI_xStart, UserControl_ROI_xEnd, m_nTimeOut);// Set as needed
    //CHECK("InsCHVS_Set_ROI_Rangle: ", ret);
    ret = m_pInsnex_impl->InsCHVS_Set_ROI_Enable(UserControl_ROI_Enable, m_nTimeOut); // Set as needed
    //CHECK("InsCHVS_Set_ROI_Enable: ", ret);
    //
    //
}

void GrabExecutor_Insnex::SetFlipHorizontal(BOOL bUse)
{
    INS_Result ret = INS_Result::INS_OK;
    /* Flip setting */
    //수평 플립만 지원 함
    /* Settings before starting image acquisition*/
    /* Set horizontal flipping (vertical flipping has not been developed, only the interface is left)*/
    bool UserControl_Enable_Flip = false;
    if (UserControl_Enable_Flip == true)
    {
        ret = m_pInsnex_impl->InsCHVS_Set_DevPrm(INS_PRM_BUFFER_FLIP, INS_VAL_BUFFER_FLIP_HORIZONTAL); // Set as needed
        //CHECK("INS_VAL_BUFFER_FLIP_HORIZONTAL: ", ret);
    }
    else
    {
        ret = m_pInsnex_impl->InsCHVS_Set_DevPrm(INS_PRM_BUFFER_FLIP, INS_VAL_BUFFER_FLIP_NORMAL); // Set as needed
        //CHECK("INS_VAL_BUFFER_FLIP_NORMAL: ", ret);
    }
}

void GrabExecutor_Insnex::SetSystemGain(CString strGain)
{
    return;
}

void GrabExecutor_Insnex::SetExposureTime(CString strExposure, int nRow)
{
    return;
}

double GrabExecutor_Insnex::GetSystemGain()
{
    return 0;
}

void GrabExecutor_Insnex::GetExposureTime(double& dBright, double& dDark)
{
    return;
}

void GrabExecutor_Insnex::SetSensorSelector(CString strSensorSel)
{
    return;
}

void GrabExecutor_Insnex::SetSensorGain(CString strSensorGain)
{
    return;
}

void GrabExecutor_Insnex::GetSensorInfo(int& nSensorSelector, double& dSensorGain)
{
    return;
}

void GrabExecutor_Insnex::GetRowGain(double& dRowGainBright, double& dRowGainDark)
{
    return;
}

void GrabExecutor_Insnex::SetRowGain(CString strRowGain, int nRow)
{
    return;
}
void GrabExecutor_Insnex::ResponseLeveling()
{
    return;
}

void GrabExecutor_Insnex::Reset()
{
    return;
}

double GrabExecutor_Insnex::GetFPGATemperature()
{
    float fTemperature = 0.f;
    m_pInsnex_impl->InsCHVS_Get_FPGA_Temperature(m_CameraIndex, fTemperature, m_nTimeOut);
    return (double)fTemperature;
}

BOOL GrabExecutor_Insnex::GetCameraConnectCheck()
{
    BOOL bCheck = FALSE;
    InsCHVS_ConnectionState pState = InsCHVS_ConnectionState::Disconnected;
    m_pInsnex_impl->InsCHVS_Get_ConnectionState(&pState);
    if (pState == InsCHVS_ConnectionState::Connected)
    {
        bCheck = TRUE;
        return bCheck;
    }
    else
    {
        return bCheck;
    }
}