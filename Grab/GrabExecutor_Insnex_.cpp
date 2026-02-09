#include <iostream>
#include <chrono>
#include <string>
#include <time.h>
#include <unordered_map>
//#include <opencv2/opencv.hpp>
#include <thread>
#include <future>
#include <windows.h>
#include <filesystem>
#include <cstring>
#include "../Insnex/ins_chvs_ctrl.hpp"

namespace fs = std::filesystem;

#ifdef _DEBUG
#pragma comment(lib,"InsCHVSControl.lib")
#pragma comment(lib,"opencv_world460d.lib")
#else
#pragma comment(lib,"InsCHVSControl.lib")
#pragma comment(lib,"opencv_world460.lib")
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 对于基本的运行采集图像，所有包含'UserControl'内容的变量，是用户需要关心设置的 
// For basic image collection, all variables containing 'UserControl' content are the ones that users need to care about setting
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 超时时间阈值。
// Timeout threshold.
#define TIMEOUT 1000

// 保存图片数量
// Number of saved images
constexpr int SAVE_IMAGE_COUNT = 5;

#define UNUSED(x) (void)(x)

 /* @brief：Print the error message and exit the program .
  * @param[in] info：Other textual information.
  * @param[in] ret ：The function returns the value.
  */
#define CHECK(info, ret) printCHVSResultAndErrorExit(info, ret)
void printCHVSResultAndErrorExit(const char* info, INS_Result ret);

/* @brief：Print device list information
 * @param[in] _devices_info
 */
void printDevicesInfo(const InsCHVS_DeviceInfoList & _devices_info);

/* @brief：Print device information
 * @param[in] _device_info
 */
void printDeviceInfo(const InsCHVS_DeviceInfo& _device_info);

/* @brief：Select the device to open
 * @param[in] _devices_info
 */
int chooseDeviceByUser(const InsCHVS_DeviceInfoList& _devices_info);

/* @brief：Print time
 */
std::string printTime(bool);

/* @brief：Is it an LXM series camera, 1800/900dpi
 */
int extractDPI(InsCHVS_DeviceType type) {
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

std::vector<cv::Mat> matBufferImageVector;// Container for saving images
cv::Mat matBufferImage; // Only for copying images

int main()
{
    printTime(true);
    std::cout << "Welcome to use INS CHVS!\n";
    /**************** Set Log param (path) **********************************/
    //
    /* 在调用InsCHVS_Initialize和之前修改log相关参数有效 */
    /* Modifying log related parameters is effective before calling InsCHVS_initization*/
    INS_Result ret = INS_Result::INS_OK;
    InsCHVS_LoggerParam UserControl_LoggerParam;
    strcpy_s(UserControl_LoggerParam.path, sizeof(UserControl_LoggerParam.path), "your_relative_or_absolute_path_here");
    //ret = InsCHVSCamera::InsCHVS_Initialize_Logger_Param(UserControl_LoggerParam);
    CHECK("Initialize_Logger: ", ret);
    //
    /**************** SDK version ******************************/
    //
    char version[20];
    InsCHVSCamera::InsCHVS_SoftWare_Version(version, 20);
    std::cout << "SDK version: " << version << std::endl;
    //
    /***************** List devices *****************************/
    // 
    /* Initialize the acquisition card */ 
    InsCHVSCamera::InsCHVS_Initialize();
    // 
    /* local variable */
    InsCHVS_DeviceInfoList deviceInfoListScanned;
    InsCHVS_DeviceInfo deviceInfoOpened;
    InsCHVSCamera camera;
    /* List devices */ 
    camera.InsCHVS_FindDevice(&deviceInfoListScanned);
    printDevicesInfo(deviceInfoListScanned);
    //
    /***************** Open device ******************************/
    //
    int UserControl_ChooseIndex = chooseDeviceByUser(deviceInfoListScanned);
    ret = camera.InsCHVS_Cmd_Open(UserControl_ChooseIndex);
    CHECK("OPEN: ", ret);
    std::cout << "Open camera successfully " << std::endl;
    //
    /* After successful connection, obtain information */
    camera.InsCHVS_Get_DevInfo(deviceInfoOpened);
    printDeviceInfo(deviceInfoOpened);
    //
    /************************************************************/
    //
    /* 打开相机后，选择是否直接运行 */
    /* After opening the camera, choose whether to run it directly */
    bool UserControl_NeedChangeParam = false;
    //
    auto changeParamFunc = [&]() {
        /***************** Set parameters of images ******************************/
        // 
        /* 单张图片的传输高度，不得低于64 */
        /* 用户自由设置 */
        /* The height of a single image, shall not be less than 64 */
        /* User free settings */
        uint32_t UserControl_ImageHeight = 2000;
        ret = camera.InsCHVS_Set_Img_TransHeight(UserControl_ImageHeight, TIMEOUT);
        CHECK("InsCHVS_Set_Img_TransHeight: ", ret);
        //
        /***************** Light source mode ****************************************/
        //
        /* Light source mode */
        InsCHVS_LED_TriggerMode UserControl_LedMode = InsCHVS_LED_TriggerMode::LED_SimultaneousDualBrightness;
        //
        /* Set light source mode */
        ret = camera.InsCHVS_Set_LED_TriggerMode(UserControl_LedMode, TIMEOUT);
        CHECK("InsCHVS_Set_LED_TriggerMode: ", ret);
        //
        /***************** Set parameters of Trigger source 1 ******************************/
        //
        /* 和上位机软件上的《采图模式》设置页对应 */
        /* Corresponding to the <Capture Setting> on the upper computer interface */
        //
        /* 触发源一：设置为内部时钟触发。*/
        /* Trigger source 1: Set mode to 'internal clock' */
        /* When used by users, 'trigger source 1' needs to be set as an internal clock */
        ret = camera.InsCHVS_Set_Acq_TrigSource(InsCHVS_LineTriggerSource::Internal_Clock, TIMEOUT);
        CHECK("InsCHVS_Set_Acq_TrigSource: ", ret);
        // 
        /* 触发源一：设定内部时钟的行频, 注意有最大行频限制 */
        /* 最大行频通过使用 INS_PRM_PERIOD_DPI_MAX 调用 InsCHVS_Get_DevPrm 函数获取 */
        /* 最大行频的范围会受到光源模式的影响。所以先设置光源模式，再设置内部时钟行频 */
        /* 设置时，此行频要大于编码器运行的速度，否则会受限于所设置的较低行频 */
        /* Trigger source 1: Set the line frequency of the internal clock, note that there is a maximum line frequency limit */
        /* The maximum line frequency is obtained by calling the 'InsCHVS_Get_SevPrm' function using 'INS_PRM_PERIOD_DPI_MAX' */
        /* The range of maximum line frequency will be affected by the light source mode. So first set the light source mode, and then set the internal clock line frequency*/
        /* When setting, this line frequency should be greater than the speed at which the encoder runs, otherwise it will be limited by the lower line frequency set*/
        float periodMax = 0;
        camera.InsCHVS_Get_DevPrm(INS_PRM_PERIOD_DPI_MAX, &periodMax);
        float UserControl_Trigger1_LineFrequency = MIN(15, periodMax);
        ret = camera.InsCHVS_Set_Acq_Intern_TrigPeriod(UserControl_Trigger1_LineFrequency, TIMEOUT);
        CHECK("InsCHVS_Set_Acq_Intern_TrigPeriod: ", ret);
        // 
        /**************** Set parameters of Trigger source 2  ******************************/
        //
        /* 和上位机软件上的《采图模式》设置页对应 */
        /* Corresponding to the <Acquire Setting> on the upper computer interface */
        //
        /* 触发源二：打开触发源2 */
        /* Trigger source 2: open trigger source 2 */
        ret = camera.InsCHVS_Set_Acq2_Enable(InsCHVS_FuncEnable::Ins_Enable, TIMEOUT);
        //
        /* 根据场景和需求自主选择外部信号的模式 */
        /* Choose the mode of external signals independently based on the scenario and requirements */
        InsCHVS_LineTriggerSource UserControl_Trigger2_Mode = InsCHVS_LineTriggerSource::Internal_Clock;
        ret = camera.InsCHVS_Set_Acq2_TrigSource(UserControl_Trigger2_Mode, TIMEOUT);
        CHECK("InsCHVS_Set_Acq2_TrigSource: ", ret);
        //
        if (UserControl_Trigger2_Mode == InsCHVS_LineTriggerSource::Internal_Clock)
        {
            /* 没有编码器、io等外部信号时，可以使用此模式，以固定行频进行采集图像 */
            /* 触发源2的行频不得大于触发源1的行频，否则以二者中较小的速度运行 */
            /* When there are no external signals such as encoders or IO, this mode can be used to capture images with a fixed line frequency */
            /* The line frequency of trigger source 2 must not be greater than that of trigger source 1, otherwise it will run at the lower speed of the two */
            float32_t UserControl_Trigger2_LineFrequency = MIN(10, UserControl_Trigger1_LineFrequency);
            ret = camera.InsCHVS_Set_Acq2_Intern_TrigPeriod(UserControl_Trigger2_LineFrequency, TIMEOUT);
            CHECK("InsCHVS_Set_Acq_Intern_TrigPeriod: ", ret);
            /* 设定单次运行采集的图像行数，行数设为0表示持续取图 */
            /* Set the number of image lines to be collected in a single run. Setting the number of lines to 0 indicates continuous image collection */
            ret = camera.InsCHVS_Set_Acq2_Intern_TrigNums(0, TIMEOUT);
            CHECK("InsCHVS_Set_Acq2_Intern_TrigNums: ", ret);
        }
        else if (UserControl_Trigger2_Mode == InsCHVS_LineTriggerSource::External_Encoder)
        {
            /* 使用外部编码器信号时，使用此模式 */
            /* 如果此模式下不出图，请切换到内部时钟模式查看是否出图；若出图，请检查编码器信号和编码器模式的参数设置 */
            /* When using external encoder signals, use this mode */
            /* If the image cannot be displayed in this mode, please switch to the internal clock mode to check if the image is displayed;
               If the image appears, please check the parameter settings of the encoder signaland encoder mode */
            //
            /* 组合计数模式和行程方向有搭配限制 */
            /* 选择‘PhA_RisingEdge_OneWay’时，必须和‘Forward_Scan’组合 */
            /* 不选择‘PhA_RisingEdge_OneWay’时，也无法选择‘Forward_Scan’组合 */
            /* Combination counting mode and travel direction have matching restrictions */
            /* When selecting 'PhA_RisingEdge_oneWay', it must be combined with 'Forward_Scan' */
            /* When 'PhA_RisingEdge_oneWay' is not selected, the 'Forward_Scan' combination cannot be selected either */
            InsCHVS_Encoder_CountMode UserControl_Trigger2_Enocder_CountMode = InsCHVS_Encoder_CountMode::PhA_RisingEdge_OneWay;
            InsCHVS_Encoder_TriggerMode UserControl_Trigger2_Enocder_TravelDirectionMode = InsCHVS_Encoder_TriggerMode::Forward_Scan;
            ret = camera.InsCHVS_Set_Acq2_Encoder_CountMode(UserControl_Trigger2_Enocder_CountMode, TIMEOUT);
            CHECK("InsCHVS_Set_Acq2_Encoder_CountMode: ", ret);
            ret = camera.InsCHVS_Set_Acq2_Encoder_TriggerMode(UserControl_Trigger2_Enocder_TravelDirectionMode, TIMEOUT);
            CHECK("InsCHVS_Set_Acq2_Encoder_TriggerMode: ", ret);
            //
            uint32_t UserControl_Trigger2_Enocder_IgnoreCount = 0;
            float32_t UserControl_Trigger2_Enocder_FilteringTimeWidth = 1.28; // us
            uint32_t UserControl_Trigger2_InputDivide = 1; // Divide the external encoder signal, Greater than 1
            InsCHVS_FuncEnable UserControl_Trigger2_Enable_InputMultiple = InsCHVS_FuncEnable::Ins_Enable; // After enabling, multiple is effective
            uint32_t UserControl_Trigger2_InputMultiple = 1; // Multiple the external encoder signal, Greater than 1
            //camera.InsCHVS_Set_Acq2_Encoder_Ignore(UserControl_Trigger2_Enocder_IgnoreCount, TIMEOUT); // Set as needed. Generally, default is sufficient.
            //camera.InsCHVS_Set_Acq2_Encoder_FilterWidth(UserControl_Trigger2_Enocder_FilteringTimeWidth, TIMEOUT); // Set as needed. Generally, default is sufficient.
            //camera.InsCHVS_Set_Acq2_Encoder_InputDiv(UserControl_Trigger2_InputDivide, TIMEOUT); // Set as needed
            //camera.InsCHVS_Set_Acq2_Encoder_Enable_InputMultiple(UserControl_Trigger2_Enable_InputMultiple, TIMEOUT); // Set as needed
            //camera.InsCHVS_Set_Acq2_Encoder_InputMultiple(UserControl_Trigger2_InputMultiple, TIMEOUT); // Set as needed
        }
        else if (UserControl_Trigger2_Mode == InsCHVS_LineTriggerSource::External_IO)
        {
            /* 使用外部IO信号时，使用此模式 */
            /* 如果此模式下不出图，请切换到内部时钟模式查看是否出图；若出图，请检IO信号是否有20V电压和IO模式的参数设置 */
            /* When using external IO signals, use this mode */
            /* If the image cannot be displayed in this mode, please switch to the internal clock mode to check if the image is displayed;
               If the image appears, please check if the IO signal has a 20V voltage and IO mode parameter settings */
            //
            float32_t UserControl_Trigger2_IO_LineFrequency = 10;
            InsCHVS_IO_TriggerMode UserControl_Trigger2_IO_TriggerMode = InsCHVS_IO_TriggerMode::Input0_Start_Input1_End;
            uint32_t UserControl_Trigger2_IO_TrigNums = 0;
            uint32_t UserControl_Trigger2_IO_OutputDelay = 0;
            float32_t UserControl_Trigger2_IO_FilteringTimeWidth = 1.28; //us
            uint32_t UserControl_Trigger2_IO_InputDivide = 1;
            ret = camera.InsCHVS_Set_Acq2_IO_TrigPeriod(UserControl_Trigger2_IO_LineFrequency, TIMEOUT);
            CHECK("InsCHVS_Set_Acq2_IO_TrigPeriod: ", ret);
            ret = camera.InsCHVS_Set_Acq2_IO_Mode(UserControl_Trigger2_IO_TriggerMode, TIMEOUT);
            CHECK("InsCHVS_Set_Acq2_IO_Mode: ", ret);
            ret = camera.InsCHVS_Set_Acq2_IO_TrigNums(UserControl_Trigger2_IO_TrigNums, TIMEOUT);
            CHECK("InsCHVS_Set_Acq2_IO_TrigNums: ", ret);
            // camera.InsCHVS_Set_Acq2_IO_OutputDelay(UserControl_Trigger2_IO_OutputDelay, TIMEOUT); // Set as needed
            // camera.InsCHVS_Set_Acq2_IO_FilterWidth(UserControl_Trigger2_IO_FilteringTimeWidth, TIMEOUT); // Set as needed
            // camera.InsCHVS_Set_Acq2_IO_InputDiv(UserControl_Trigger2_IO_InputDivide, TIMEOUT); // Set as needed
        }
        else
        {
            std::cout << "Not allowed mode" << std::endl;
            exit(EXIT_FAILURE);
        }
        //
        /**************** Set parameters of exposure time ******************************/
        //
        /* 所有和频率相关的参数设置都会影响曝光最大值，比如触发源1的行频 */
        /* All frequency related parameter settings will affect the maximum exposure value, such as the line frequency of trigger source 1 */
        // 
        /* 曝光最大值 */
        /* Maximum exposure time */
        float32_t maxExposureTime = 0;
        camera.InsCHVS_Get_TimeLine_Max(&maxExposureTime, TIMEOUT);
        /* 曝光值用户自由设置 */
        /* Exposure value can be freely set by users */
        std::vector<float32_t> UserControl_Exposure(3, 0);
        UserControl_Exposure[0] = MIN(15, maxExposureTime);
        UserControl_Exposure[1] = MIN(10, maxExposureTime);
        UserControl_Exposure[2] = MIN(5, maxExposureTime);
        //
        /* 设定曝光 */
        /* LED触发模式为LED_SimultaneousDualBrightness（无分时频闪，正光）、LED_BacklightOnly（无分时频闪，背光）时，调用InsCHVS_Set_LED_ExposureTime1 */
        /* LED触发模式为LED_SeparateTimedFlashingBrightness（两路分时频闪）时，调用InsCHVS_Set_LED_ExposureTime2 */
        /* LED触发模式为LED_TripleIndependentBrightness（三路分时频闪）时，调用InsCHVS_Set_LED_ExposureTime3 */
        /* Set exposure */
        /* When the LED trigger mode is LED_SimultaneousDualBrightness (No time-division strobe, positive light)
                                     or LED_SacklightOnly (No time-division strobe, backlight), call InsCHVS_Set_LEDExposeTime1 */
        /* When the LED trigger mode is LED_SeparateTimedFlashingBrightness(Two channel time-division strobe), call InsCHVS_Set_LEDExposeTime2 */
        /* When the LED trigger mode is LED_TripleIndependentBrightness(Three channel time-division strobe), call InsCHVS_Set_LEDExposeTime3 */
        switch (UserControl_LedMode)
        {
        case InsCHVS_LED_TriggerMode::LED_SimultaneousDualBrightness:
        case InsCHVS_LED_TriggerMode::LED_BacklightOnly:
            ret = camera.InsCHVS_Set_LED_ExposureTime1(UserControl_Exposure[0], TIMEOUT);
            CHECK("Set_LED_ExposureTime1:", ret);
            break;
        case InsCHVS_LED_TriggerMode::LED_SeparateTimedFlashingBrightness:
            ret = camera.InsCHVS_Set_LED_ExposureTime2(UserControl_Exposure[0], UserControl_Exposure[1], TIMEOUT);
            CHECK("Set_LED_ExposureTime2:", ret);

            /* LED触发模式为分时频闪时，可以设置此参数，使得输出的图片分成 2 个图片 */
            /* 开启此模式的要求：行高必须为频闪路数 2 的整数倍 */
            /* When the LED trigger mode is time-division strobe, this parameter can be set to split the output image into two images */
            /* Requirement for activating this mode: The row height must be an integer multiple of 2 strobe channels */
            ret = camera.InsCHVS_Set_DevPrm(INS_PRM_BUFFER_STROBING_MODE, INS_VAL_BUFFER_STROBING_MULTIPLE);
            CHECK("Set_DevPrm(INS_VAL_BUFFER_STROBING_MULTIPLE):", ret);
            break;
        case InsCHVS_LED_TriggerMode::LED_TripleIndependentBrightness:
            ret = camera.InsCHVS_Set_LED_ExposureTime3(UserControl_Exposure[0], UserControl_Exposure[1], UserControl_Exposure[2], TIMEOUT);
            CHECK("Set_LED_ExposureTime3:", ret);

            /* LED触发模式为分时频闪时，可以设置此参数，使得输出的图片分成 3 个图片 */
            /* 开启此模式的要求：行高必须为频闪路数 3 的整数倍 */
            /* When the LED trigger mode is time-division strobe, this parameter can be set to split the output image into three images */
            /* Requirement for activating this mode: The row height must be an integer multiple of 3 strobe channels */
            ret = camera.InsCHVS_Set_DevPrm(INS_PRM_BUFFER_STROBING_MODE, INS_VAL_BUFFER_STROBING_MULTIPLE);
            CHECK("Set_DevPrm(INS_VAL_BUFFER_STROBING_MULTIPLE):", ret);
            break;
        default:
            break;
        }
        //
        /*************** Other parameters ***********************************/
        //
        /* ROI setting */
        //
        /* Set up before starting image acquisition; Set ROI range:*/
        uint32_t UserControl_ROI_xStart = 3000;
        uint32_t UserControl_ROI_xEnd = 5000;
        InsCHVS_FuncEnable UserControl_ROI_Enable = InsCHVS_FuncEnable::Ins_Enable;
        //ret = camera.InsCHVS_Set_ROI_Rangle(UserControl_ROI_xStart, UserControl_ROI_xEnd, TIMEOUT);// Set as needed
        //CHECK("InsCHVS_Set_ROI_Rangle: ", ret);
        //ret = camera.InsCHVS_Set_ROI_Enable(UserControl_ROI_Enable, TIMEOUT);// Set as needed
        //CHECK("InsCHVS_Set_ROI_Enable: ", ret);
        //
        //
        /* Flip setting */
        //
        /* Settings before starting image acquisition*/
        /* Set horizontal flipping (vertical flipping has not been developed, only the interface is left)*/
        bool UserControl_Enable_Flip = false;
        if (UserControl_Enable_Flip == true) {
            ret = camera.InsCHVS_Set_DevPrm(INS_PRM_BUFFER_FLIP, INS_VAL_BUFFER_FLIP_HORIZONTAL); // Set as needed
            CHECK("INS_VAL_BUFFER_FLIP_HORIZONTAL: ", ret);
        }
        else {
            ret = camera.InsCHVS_Set_DevPrm(INS_PRM_BUFFER_FLIP, INS_VAL_BUFFER_FLIP_NORMAL); // Set as needed
            CHECK("INS_VAL_BUFFER_FLIP_NORMAL: ", ret);
        }
        //
    };
    if (UserControl_NeedChangeParam) {
        changeParamFunc();
    }
    else {
        /* LED触发模式为分时频闪时，可以设置此参数，使得输出的图片分成 1~3 个图片 */
        /* 开启此模式的要求：行高必须为频闪路数的整数倍 */
        /* When the LED trigger mode is time-division strobe, this parameter can be set to split the output image into 1~3 images */
        /* Requirement for activating this mode: The row height must be an integer multiple of strobe channels */
        ret = camera.InsCHVS_Set_DevPrm(INS_PRM_BUFFER_STROBING_MODE, INS_VAL_BUFFER_STROBING_MULTIPLE);
    }
    //
    /************************************************************/
    //
    /* 加载相机的icf配置文件 */
    /* 不同型号的相机无法相互加载 */
    /* Load the ICF configuration file of the camera  */
    /* Different models of cameras cannot load each other */
    std::string UserControl_ConfigFilePath = "C:/Users/UserName/Desktop/save.icf";
    //
    // ret = camera.InsCHVS_Load_ConfigFile(UserControl_ConfigFilePath.c_str(), TIMEOUT); // Set as needed
    CHECK("Load icf = ", ret);
    /*************** Save parameters ***********************************/
    //
    /* 保存上面的配置到相机内部，断电重启后配置有效；不断电时，即使不保存参数也一直有效*/
    /* Save the above configuration inside the camera, and the configuration will be valid after power failure and restart; */
    /* When not powered on, it remains valid even without saving parameters */
    //camera.InsCHVS_Save_DeviceControl_Param(TIMEOUT);// Set as needed
    //
    /*************** Receive image cache buffer ************************/
    //
    /* Buffer receives data */
    uint32_t bufferHeight;
    uint32_t bufferImageWidth;
    camera.InsCHVS_Get_DevPrm(INS_PRM_BUFFER_ROI_HEIGHT, &bufferHeight);
    camera.InsCHVS_Get_DevPrm(INS_PRM_BUFFER_ROI_IMG_WIDTH, &bufferImageWidth);
    int type = ((((int)deviceInfoOpened.InsType >> 8) & 0xf) == INS_COLOR) ? CV_8UC3 : CV_8UC1;
    for (INT i = 0; i < SAVE_IMAGE_COUNT; i++)
    {
        cv::Mat image(bufferHeight, bufferImageWidth, type);
        matBufferImageVector.push_back(std::move(image));
    }
    matBufferImage.create(bufferHeight, bufferImageWidth, type);
    std::cout << "Image size: " << bufferHeight << " * " << bufferImageWidth << std::endl;
    //
    isLXMSeriesCamera = is_DPI_EqualTo_1800_900(deviceInfoListScanned.DeviceInfo[0].InsType);
    // 
    /*************** Register Callbacks *********************************/
    // 
    // 注册回调函数,如果不注册回调函数，则需要主动取图调用InsCHVS_AcquireImage、InsCHVS_AcquireError等函数实现取图
    // 若注册至少一个回调函数，则无需使用上面函数
    // Register callback function. If you do not register callback function, you need to actively call InsCHVS_ScquireImage, InsCHVS_ScquireError and other functions to obtain the graph
    // If at least one callback function is registered, there is no need to use the above function
    camera.InsCHVS_RegisterCallback(INS_Event_FrameReady, FrameReadyCallBack, nullptr);
    camera.InsCHVS_RegisterCallback(INS_Event_GrabStart, StartCallBack, nullptr);
    camera.InsCHVS_RegisterCallback(INS_Event_GrabStop, StopCallBack, nullptr);
    camera.InsCHVS_RegisterCallback(INS_Event_FrameLost, FrameLostCallBack, nullptr);
    camera.InsCHVS_RegisterCallback(INS_Event_TimeOut, TimeOutCallBack, nullptr);
    camera.InsCHVS_RegisterCallback_DeviceRemove(CameraRemoveCallback, &camera);
    system("pause");
    // 
    /*************** Start obtaining images *****************************/
    // 
    ret = camera.InsCHVS_Cmd_Start();
    CHECK("START = ", ret);
    //
    /********************************************************************/
    //
    /* 开始取图后，可以进行标定、改变曝光值，但无法设置行频、行高 */ 
    /* After starting to take the image, you can change the exposure value, but you cannot set the line frequency or line height */ 
    system("pause");
    //
    /*************** Stop obtaining images ******************************/
    //
    ret = camera.InsCHVS_Cmd_Stop();
    CHECK("STOP = ", ret);
    camera.InsCHVS_Set_DevPrm(INS_PRM_BUFFER_CLEAR, 1); // Clear the internal buffer
    CHECK("CLEAR = ", ret);
    //
    /*************** Close camera ***************************************/
    //
    std::cout << "Press any key to turn off the camera" << std::endl;
    system("pause");
    ret = camera.InsCHVS_Cmd_Close();
    CHECK("CLOSE = ", ret);
    //
    /*************** Close console window *******************************/
    //
    std::cout << "Press any key to close the console window" << std::endl;
    system("pause");
}

/* @brief：Convert INS_Result to String.
 * @param[in] ret ：The function returns the value.
 */
std::string ResultToString(const INS_Result ret)
{
    constexpr int SIZE = 256;
    std::string info = "";
    {
        size_t size = SIZE;
        char code[SIZE];
        // If the return fails, the array size needs to be increased
        if (InsCHVSCamera::InsCHVS_Get_ResultCodeString(ret, code, &size) == true)
        {
            info += code;
            //std::cout << code << " size:" << size << std::endl;
        }
        else 
        {
            std::cout << "Need to increase the array size, greater than " << size << std::endl;
        }
    }
    {
        size_t size = SIZE;
        char code[SIZE];
        // If the return fails, the array size needs to be increased
        if (InsCHVSCamera::InsCHVS_Get_ResultDescriptionCN(ret, code, &size) == true)
        {
            info += std::string("\n  Chinese Infomation: ") + code;
            //std::cout << code << " size:" << size << std::endl;
        }
        else
        {
            std::cout << "Need to increase the array size, greater than " << size << std::endl;
        }
    }
    {
        size_t size = SIZE;
        char code[SIZE];
        // If the return fails, the array size needs to be increased
        if (InsCHVSCamera::InsCHVS_Get_ResultDescriptionEN(ret, code, &size) == true)
        {
            info += std::string("\n  English Infomation: ") + code;
            //std::cout << code << " size:" << size << std::endl;
        }
        else
        {
            std::cout << "Need to increase the array size, greater than " << size << std::endl;
        }
    }
    return info;
}


/* @brief：Print the error message and exit the program .
 * @param[in] info：Other textual information.
 * @param[in] ret ：The function returns the value.
 */
void printCHVSResultAndErrorExit(const char* info, const INS_Result ret)
{
    if (ret != INS_Result::INS_OK)
    {
        std::cout << std::endl << info << ResultToString(ret) << std::endl;
        exit(EXIT_FAILURE);
    }
}


void printDevicesInfo(const InsCHVS_DeviceInfoList &  _devices_info)
{
    if (_devices_info.DeviceCount == 0) {
        std::cout << std::endl << "No camera found " << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "---------------------------------" << std::endl;
    for (int i = 0; i < _devices_info.DeviceCount; i++) 
    {
        std::cout << "NO. : " << i + 1 << std::endl;
        switch (_devices_info.DeviceInfo[i].TransLayerType)
        {
        case INS_GIGE_DEVICE:
            std::cout << "TYPE: " << _devices_info.DeviceInfo[i].SpecialInfo.GigeInfo[0].DevType << std::endl;
            std::cout << "IP  : " << _devices_info.DeviceInfo[i].SpecialInfo.GigeInfo[0].Ip << std::endl;
            std::cout << "MAC : " << _devices_info.DeviceInfo[i].SpecialInfo.GigeInfo[0].MAC << std::endl;
            break;
        case INS_CAMERALINK_DEVICE:
            std::cout << "TYPE: " << _devices_info.DeviceInfo[i].SpecialInfo.CamLInfo[0].DevType << std::endl;
            std::cout << "SN  : " << _devices_info.DeviceInfo[i].SpecialInfo.CamLInfo[0].SN << std::endl;
            std::cout << "COM : " << _devices_info.DeviceInfo[i].SpecialInfo.CamLInfo[0].COM << std::endl;
            break;
        default:
            std::cout << "Error TransLayer";
            break;
        }
        std::cout << "---------------------------------" << std::endl;
    }
}

void printDeviceInfo(const InsCHVS_DeviceInfo& _device_info)
{
    switch (_device_info.TransLayerType)
    {
    case INS_GIGE_DEVICE:
        std::cout << "---------------------------------" << std::endl;
        std::cout << "Vendor: " << _device_info.SpecialInfo.GigeInfo[0].Vendor << std::endl;
        std::cout << "Ip: " << _device_info.SpecialInfo.GigeInfo[0].Ip << std::endl;
        std::cout << "MAC: " << _device_info.SpecialInfo.GigeInfo[0].MAC << std::endl;
        std::cout << "SN: " << _device_info.SpecialInfo.GigeInfo[0].SN << std::endl;
        std::cout << "GateWay: " << _device_info.SpecialInfo.GigeInfo[0].GateWay << std::endl;
        std::cout << "SubNetMask: " << _device_info.SpecialInfo.GigeInfo[0].SubNetMask << std::endl;
        std::cout << "SoftVersion: " << _device_info.SpecialInfo.GigeInfo[0].SoftVersion << std::endl;
        std::cout << "SoftVersionDate: " << _device_info.SpecialInfo.GigeInfo[0].SoftVersionDate << std::endl;
        std::cout << "---------------------------------" << std::endl;
        break;
    case INS_CAMERALINK_DEVICE:
        std::cout << "---------------------------------" << std::endl;
        std::cout << "BoardIndex: " << _device_info.SpecialInfo.CamLInfo[0].BoardIndex << std::endl;
        std::cout << "COM: " << _device_info.SpecialInfo.CamLInfo[0].COM << std::endl;
        std::cout << "SN: " << _device_info.SpecialInfo.CamLInfo[0].SN << std::endl;
        std::cout << "DevType: " << _device_info.SpecialInfo.CamLInfo[0].DevType << std::endl;
        std::cout << "FullName: " << _device_info.SpecialInfo.CamLInfo[0].FullName << std::endl;
        std::cout << "SoftVersion: " << _device_info.SpecialInfo.CamLInfo[0].SoftVersion << std::endl;
        std::cout << "SoftVersionDate: " << _device_info.SpecialInfo.CamLInfo[0].SoftVersionDate << std::endl;
        std::cout << "---------------------------------" << std::endl;
        break;
    default:
        std::cout << "Error TransLayer";
        break;
    }
}

int chooseDeviceByUser(const InsCHVS_DeviceInfoList& _devices_info)
{
    // choose device
    std::cout << "Select the index you want to open:(1 ~ " << _devices_info.DeviceCount << ")" << std::endl;
    //system("pause");
    int chooseIndex = 0;
    std::cout << ">> ";
    std::cin >> chooseIndex;
    if (chooseIndex <= 0 || chooseIndex > _devices_info.DeviceCount)
    {
        std::cout << "Invalid index!" << std::endl;
        exit(EXIT_FAILURE);
    }
    switch (_devices_info.DeviceInfo[chooseIndex - 1].TransLayerType)
    {
    case INS_GIGE_DEVICE:
        std::cout << "Selected IP: " << _devices_info.DeviceInfo[chooseIndex - 1].SpecialInfo.GigeInfo[0].Ip << std::endl;
        break;
    case INS_CAMERALINK_DEVICE:
        std::cout << "Selected COM: " << _devices_info.DeviceInfo[chooseIndex - 1].SpecialInfo.CamLInfo[0].COM << std::endl;
        break;
    default:
        std::cout << "Error TransLayer";
        break;
    }
    return chooseIndex - 1;
}

int saveImage(const std::string filename, const cv::Mat image) {
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0); // 0表示无损保存 // 0 represents lossless storage
    cv::imwrite(filename, image, compression_params);
    // 如果退出时，崩溃在这里，说明图片太大，线程还没有保存结束。debug下可能会发生
    // If the crash occurs here when exiting, it means that the image is too large and the thread has not been saved yet. Under normal debugging, it may occur
    return 1;
}

void CALLBACK FrameReadyCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    UNUSED(pUser);
    InsCHVSCamera* camera = (InsCHVSCamera*)handle;
    InsCHVS_ProcessInfo* info = (InsCHVS_ProcessInfo*)pInfo;
    InsCHVS_Buffer* buffer = (InsCHVS_Buffer*)pBuffer;

    std::string folderName = "images";
    fs::create_directory(folderName);

    //Save 5 images locally
    if (info->FrameCount <= SAVE_IMAGE_COUNT) 
    {
        // mono image or color image
        if (isLXMSeriesCamera == false) {
            memcpy(matBufferImageVector[info->FrameCount - 1].data, (void*)buffer->p_data, (size_t)buffer->step * buffer->height);
            std::string ImageName = folderName + "/" + printTime(false) + "_NO_" + std::to_string(info->FrameCount) + ".png";
            std::cout << "image saved in : " << ImageName << std::endl;
            std::thread t(saveImage, ImageName, matBufferImageVector[info->FrameCount - 1]);
            t.detach();
        }
        else if (isLXMSeriesCamera == true) { // 900/1800 dpi camera call this
            auto ImageName = [=](int index) { 
                return folderName + "/" + printTime(false) + "_NO_" + std::to_string(info->FrameCount) + "_Light" + std::to_string(index) + ".png"; };
            auto& originalMat = matBufferImageVector[info->FrameCount - 1];

            std::vector<uchar*> srcData({ buffer->p_data ,buffer->p_data2 ,buffer->p_data3 });

            for (int light_number = 0; light_number < srcData.size(); light_number++)
            {
                if (nullptr == srcData[light_number]) continue;
                uchar* dstData = originalMat.data + (size_t)buffer->step * buffer->height * light_number;
                memcpy(dstData, srcData[light_number], (size_t)buffer->step * buffer->height);
                //
                cv::Mat newMat(buffer->height, buffer->width, originalMat.type(), dstData, buffer->step);
                auto imageName = ImageName(light_number + 1);
                std::cout << "image saved in : " << imageName << std::endl;
                std::thread t(saveImage, imageName, newMat);
                t.detach();
            }
        }
    }
    else 
    { 
        // 后续的图片仅执行复制取图操作，其余操作交给用户
        // The subsequent images will only be copied and taken, and the rest will be left to the user
        memcpy(matBufferImage.data, (void*)buffer->p_data, (size_t)buffer->step * buffer->height);
        //
        // ...
    }

    std::cout << "Count: " << info->FrameCount;
    std::cout << "\tframeIndex: " << buffer->frameIndex;
    std::cout << "\tRet: " << (info->RetVal == INS_Result::INS_OK ? std::string("ok") : ResultToString(info->RetVal));
    std::cout << "\tErr: " << info->ErrNum;
    std::cout << "\tErrStr: " << info->ErrStr;
    std::cout << "\tAcquisitionRate:" << info->AcquisitionRate;
    std::cout << "\t  BandWidth:" << std::fixed << std::setprecision(2) << info->BandWidth;
    std::cout << "\tFPS: " << info->FrameRate;
    std::cout << std::endl;

}
void CALLBACK StartCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    std::cout << "************* START ************* " << std::endl;
}
void CALLBACK StopCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    std::cout << "************* STOP ************** " << std::endl;
}
void CALLBACK FrameLostCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    std::cout << "************* FrameLost ********* " << std::endl;
}
void CALLBACK TimeOutCallBack(void* handle, void* pInfo, void* pBuffer, void* pUser)
{
    std::cout << "************* TimeOut *********** " << std::endl;
}

void CALLBACK CameraRemoveCallback(void* pContext)
{
    std::cout << "************* CameraRemoveCallback *********** " << std::endl;
    InsCHVSCamera* camera = (InsCHVSCamera*)pContext;
    InsCHVS_RunState runState;
    camera->InsCHVS_Get_RunState(&runState);
    if (runState == InsCHVS_RunState::Running) {
        camera->InsCHVS_Cmd_Stop();
    }
    camera->InsCHVS_Cmd_Close();
}

std::string printTime(bool print)
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    struct tm timeInfo;
    if (localtime_s(&timeInfo, &now_c) == 0 && print)
        std::cout << "Time: " << timeInfo.tm_hour << ":" << timeInfo.tm_min << ":" << timeInfo.tm_sec << std::endl;
    return (std::to_string(timeInfo.tm_hour) + "_" + std::to_string(timeInfo.tm_min) + "_" + std::to_string(timeInfo.tm_sec)) ;
}