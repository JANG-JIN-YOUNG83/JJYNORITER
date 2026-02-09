#ifndef INS_CHVS_CTRL_
#define INS_CHVS_CTRL_

#include "ins_chvs_params.hpp"
#include "ins_chvs_ctrl_c.h"

class INS_CHVS_DLL_API InsCHVSCamera
{
public:
    InsCHVSCamera();
    ~InsCHVSCamera();
    
    static INS_Result InsCHVS_Initialize_Logger_Param(IN InsCHVS_LoggerParam param);

    static INS_Result InsCHVS_SoftWare_Version(OUT char* a_version, size_t size);

    static INS_Result InsCHVS_Initialize();
    static INS_Result InsCHVS_Terminate();

    static INS_Result InsCHVS_FindDevice(OUT InsCHVS_DeviceInfoList* device_info_list);

    static bool InsCHVS_Get_ResultCodeString(IN INS_Result ret, OUT char* code, IN size_t* size);
    static bool InsCHVS_Get_ResultDescriptionCN(IN INS_Result ret, OUT char* descriptionCN, IN size_t* size);
    static bool InsCHVS_Get_ResultDescriptionEN(IN INS_Result ret, OUT char* descriptionEN, IN size_t* size);

    INS_Result InsCHVS_Cmd_Open(IN const int a_index);
    INS_Result InsCHVS_Cmd_Close();
    virtual INS_Result InsCHVS_Cmd_Start();
    virtual INS_Result InsCHVS_Cmd_Stop();

    INS_Result InsCHVS_Get_RunState(IN InsCHVS_RunState* a_run_state);
    INS_Result InsCHVS_Get_ConnectionState(IN InsCHVS_ConnectionState* a_connection_state);

    INS_Result InsCHVS_AcquireError(OUT uint32_t& ErrNum, OUT char* ErrStr, size_t size);
    INS_Result InsCHVS_AcquireImage(OUT InsCHVS_Buffer* ins_buffer);
    INS_Result InsCHVS_Get_DevInfo(OUT InsCHVS_DeviceInfo & info);
    INS_Result InsCHVS_Get_DevPrm(IN const int _index, OUT void* _pValue);
    INS_Result InsCHVS_Set_DevPrm(IN const int _index, IN const int _value);
    //
    INS_Result InsCHVS_Load_ConfigFile(IN const char * a_filename, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Save_ConfigFile(IN const char * a_filename, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Set_MetaData_State(IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_MetaData_State(OUT InsCHVS_FuncEnable* a_enable_status, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Set_Dev_TestReg(IN uint32_t aRegVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Dev_TestReg(OUT uint32_t* aRegValPtr, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Cmd_SensorReset(IN uint32_t aTimeOut);/* 功能未开启 */
    INS_Result InsCHVS_Cmd_FPGAReset(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Cmd_DeviceReset(IN uint32_t aTimeOut);

    //
    INS_Result InsCHVS_Set_Acq_TrigSource(IN InsCHVS_LineTriggerSource a_trigger_source, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_TrigSource(OUT InsCHVS_LineTriggerSource* a_trigger_source_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Intern_TrigPeriod(IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Intern_TrigNums(IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_IO_TrigPeriod(IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_IO_Mode(IN InsCHVS_IO_TriggerMode aMode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_IO_TrigNums(IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_IO_OutputDelay(IN float32_t aOutputDelay, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_IO_FilterWidth(IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_IO_InputDiv(IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_CountMode(IN InsCHVS_Encoder_CountMode aCountMode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_TriggerMode(IN InsCHVS_Encoder_TriggerMode aDirMode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_Ignore(IN uint32_t aIgnoreNums, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_FilterWidth(IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_InputDiv(IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_Enable_InputMultiple(IN InsCHVS_FuncEnable a_enable_multi, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_InputMultiple(IN uint32_t aInputMulti, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Encoder_RollbackIgnore(IN uint32_t aEncoderPulsesCount, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Fake_FPGA_Mode(IN InsCHVS_FakePatternMode aMode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Fake_FPGA_Period(IN float32_t aPeriod, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_Fake_FPGA_Format(IN uint16_t aRows, IN uint16_t aCollums, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Set_Acq2_Enable(IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Enable(OUT InsCHVS_FuncEnable* a_enable_status_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_TrigSource(IN InsCHVS_LineTriggerSource a_trigger_source, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_TrigSource(IN InsCHVS_LineTriggerSource* a_trigger_source_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Intern_TrigPeriod(IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Intern_TrigNums(IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_IO_TrigPeriod(IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_IO_Mode(IN InsCHVS_IO_TriggerMode aMode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_IO_TrigNums(IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_IO_OutputDelay(IN float32_t aOutputDelay, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_IO_FilterWidth(IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_IO_InputDiv(IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_CountMode(IN InsCHVS_Encoder_CountMode aCountMode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_TriggerMode(IN InsCHVS_Encoder_TriggerMode aDirMode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_Ignore(IN uint32_t aIgnoreNums, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_FilterWidth(IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_InputDiv(IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_Enable_InputMultiple(IN InsCHVS_FuncEnable a_enable_multi, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_InputMultiple(IN uint32_t aInputMulti, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_Encoder_RollbackIgnore(IN uint32_t aEncoderPulsesCount, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Get_Acq_Intern_TrigPeriod(OUT float32_t* aTrigPeriodPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Intern_TrigNums(OUT uint32_t* aTrigNumsPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_IO_TrigPeriod(OUT float32_t* aTrigPeriodPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_IO_Mode(OUT InsCHVS_IO_TriggerMode* aModePtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_IO_TrigNums(OUT uint32_t* aTrigNumsPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_IO_OutputDelay(OUT float32_t* aOutputDelayPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_IO_FilterWidth(OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_IO_InputDiv(OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_CountMode(OUT InsCHVS_Encoder_CountMode* aCountModePtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_TriggerMode(OUT InsCHVS_Encoder_TriggerMode* aTriggerModePtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_Ignore(OUT uint32_t* aIgnoreNumsPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_FilterWidth(OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_InputDiv(OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_Enable_InputMultiple(OUT InsCHVS_FuncEnable* a_enable_multi, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_InputMultiple(OUT uint32_t* aInputMulti, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Encoder_RollbackIgnore(OUT uint32_t* aEncoderPulsesCount, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Fake_FPGA_Mode(OUT InsCHVS_FakePatternMode* aModePtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Fake_FPGA_Period(OUT float32_t* aPeriodPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Intern_TrigPeriod(OUT float32_t* aTrigPeriodPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Intern_TrigNums(OUT uint32_t* aTrigNumsPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_IO_TrigPeriod(OUT float32_t* aTrigPeriodPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_IO_Mode(OUT InsCHVS_IO_TriggerMode* aModePtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_IO_TrigNums(OUT uint32_t* aTrigNumsPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_IO_OutputDelay(OUT float32_t* aOutputDelayPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_IO_FilterWidth(OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_IO_InputDiv(OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_CountMode(OUT InsCHVS_Encoder_CountMode* aCountModePtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_TriggerMode(OUT InsCHVS_Encoder_TriggerMode* aTriggerModePtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_Ignore(OUT uint32_t* aIgnoreNumsPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_FilterWidth(OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_InputDiv(OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_Enable_InputMultiple(OUT InsCHVS_FuncEnable* a_enable_multi, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_InputMultiple(OUT uint32_t* aInputMulti, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Encoder_RollbackIgnore(OUT uint32_t* aEncoderPulsesCount, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Set_Img_TransHeight(IN uint32_t aTransHeight,  IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Img_TransHeight(OUT uint32_t* aTransHeightPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Img_CombTimeout(IN uint32_t aCombTimeout,  IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Img_CombTimeout(OUT uint32_t* aCombTimeoutPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Img_PixFormat(IN InsCHVS_PixelFormat a_pixel_format, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Img_PixFormat(OUT InsCHVS_PixelFormat* a_pixel_format_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Img_FrameBitsCycle(IN uint32_t aFrameBitsCycle,  IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Img_FrameBitsCycle(OUT uint32_t* aFrameBitsCyclePtr, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Get_Camera_Counter(IN size_t id, IN uint32_t index, OUT uint32_t* pValue);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Trigger(IN size_t id, OUT uint32_t& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Trigger_Limit(IN size_t id, OUT uint32_t& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Sensor_Data(IN size_t id, OUT std::vector<uint32_t>& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Sensor_Frame(IN size_t id, OUT std::vector<uint32_t>& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Data_Cap(IN size_t id, OUT std::vector<uint32_t>& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Insert(IN size_t id, OUT std::vector<uint32_t>& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Cache(IN size_t id, OUT std::vector<uint32_t>& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Br_Frame(IN size_t id, OUT std::vector<uint32_t>& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Ip_Frame(IN size_t id, OUT std::vector<uint32_t>& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_8p_Frame(IN size_t id, OUT uint32_t& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Ir_Frame(IN size_t id, OUT uint32_t& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Fake_Frame(IN size_t id, OUT uint32_t& aCntVal, IN uint32_t aTimeOut);
    INS_CHVS_DLL_DEPRECATED INS_Result InsCHVS_Get_ImgCntr_Iport_Frame(IN size_t id, OUT uint32_t& aCntVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Cmd_Clear_ImageCounters(IN uint32_t aTimeOut);
    // 20240927
    INS_Result InsCHVS_Get_Encoder_Counter(IN uint32_t index, OUT uint32_t* pValue);
    INS_Result InsCHVS_Cmd_Reverse_EncoderCounters(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Cmd_Clear_EncoderCounters(IN uint32_t aTimeOut);
    // 20241121
    INS_Result InsCHVS_Get_IOSignal_Counter(IN uint32_t index, OUT uint32_t* pValue);
    INS_Result InsCHVS_Cmd_Clear_IOSignalCounters(IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Get_FPGA_Temperature(IN size_t id, OUT float & aDevTmpPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Device_Temperature(IN size_t id, OUT float & aDevTmpPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Device_RTC(IN size_t id, OUT uint64_t & aDevRTC, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Device_RTC_Str(IN size_t id, OUT char* aDevRTC_Str, size_t size, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Set_LED_TriggerMode(IN InsCHVS_LED_TriggerMode a_trig_bitmask, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_LED_TriggerMode(OUT InsCHVS_LED_TriggerMode* a_trig_bitmask_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Timing_Cycle(OUT float32_t* aTimeLineMaxPtr, IN uint32_t aTimeOut);
    //set
    INS_Result InsCHVS_Get_TimeLine_Max(OUT float32_t* aTimeLineMaxPtr,  IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_LED_ExposureTime1(IN float32_t a_led_time, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_LED_ExposureTime2(IN float32_t a_led_time_1, IN float32_t a_led_time_2, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_LED_ExposureTime3(IN float32_t a_led_time_1, IN float32_t a_led_time_2, IN float32_t a_led_time_3, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_LED_ExposureTime1(OUT float32_t* a_led_time_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_LED_ExposureTime2(OUT float32_t* a_led1_time_ptr, OUT float32_t* a_led2_time_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_LED_ExposureTime3(OUT float32_t* a_led1_time_ptr, OUT float32_t* a_led2_time_ptr, OUT float32_t* a_led3_time_ptr, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Get_Img_DPI(OUT InsCHVS_DPI* a_img_dpi_ptr, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Set_Correct_Dark_TarVal(IN uint32_t aDarkTarVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Correct_Dark_TarVal(OUT uint32_t* aDarkTarVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Correct_Bright_TarVal(IN uint32_t aBrightTarVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Correct_Bright_TarVal(OUT uint32_t* aBrightTarVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Req_Dark_Correct(IN uint32_t aTimeOut, OUT uint32_t* a_error_bits_ptr = NULL);
    INS_Result InsCHVS_Req_Bright_Correct(IN uint32_t aTimeOut, OUT uint32_t* a_error_bits_ptr = NULL);
    INS_Result InsCHVS_Set_Gamma_Config(IN float32_t aTarVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Gamma_Config(OUT float32_t* aTarVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Gamma_Enable(IN InsCHVS_FuncEnable aEnableBit, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Gamma_Enable(OUT InsCHVS_FuncEnable* aEnableBit_Ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Correct_Pix_TarVal(IN uint32_t aPixTarVal, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Correct_Pix_TarVal(OUT uint32_t* aPixTarVal,  IN uint32_t aTimeOut);
    INS_Result InsCHVS_Req_Correct_Pix(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Pix_Correct_Enable(IN InsCHVS_FuncEnable aEnableBit, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Pix_Correct_Enable(OUT InsCHVS_FuncEnable* aEnableBit_Ptr, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Get_Global_User_DGain(OUT double* aDGainVal_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Global_User_DGain(IN double aDGainVal, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Calibrate_Camera(IN uint32_t aBrightTarVal = 150, IN uint32_t aDarkTarVal = 4, IN uint32_t aPixTarVal = 180, IN uint32_t aBrightTarVal_Backlight = 180, IN uint32_t aPixTarVal_Backlight = 210);
    //
    INS_Result InsCHVS_Save_DarkCorrect_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Delete_DarkCorrect_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Save_WhiteCorrect_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Delete_WhiteCorrect_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Save_PixelCorrect_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Delete_PixelCorrect_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Save_DeviceControl_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Delete_DeviceControl_Param(IN uint32_t aTimeOut);

    INS_Result InsCHVS_Get_BandWidth(OUT double* aBandWidth, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_AcquisitionRate(OUT double* aAcquisitionRate, IN uint32_t aTimeOut);
    
    INS_Result Advanced_Permissions_Register_Read(IN const char* password, IN const uint32_t addr, OUT uint32_t& value, IN size_t id = 1);
    INS_Result Advanced_Permissions_Register_Read(IN const char* password, IN const uint32_t addr, OUT std::vector<uint32_t>& value);
    INS_Result Advanced_Permissions_Register_Write(IN const char* password, IN const uint32_t addr, IN const uint32_t value);
    INS_Result Advanced_Permissions_Register_Write(IN const char* password, IN const uint32_t addr, IN const uint32_t value, IN size_t id);
    INS_Result Advanced_Permissions_Register_Write(IN const char* password, IN const uint32_t addr, IN const std::vector<uint32_t> value);
    INS_Result Advanced_Permissions_Block_Read(IN const char* password, IN const uint32_t addr, OUT uint64_t valueAddress, IN const uint32_t length, IN size_t id);
    INS_Result Advanced_Permissions_Block_Write(IN const char* password, IN const uint32_t addr, IN const uint64_t valueAddress, IN const uint32_t length, IN size_t id);

    INS_Result InsCHVS_Set_Sensor_Offset(IN const std::vector<std::vector<float>>& _OffsetValue);
    INS_Result InsCHVS_Set_Sensor_Dgain(IN const std::vector<std::vector<float>>& _DgainValue);
    INS_Result InsCHVS_Get_Sensor_Offset(OUT std::vector<std::vector<float>>& _OffsetValue);
    INS_Result InsCHVS_Get_Sensor_Dgain(OUT std::vector<std::vector<float>>& _DgainValue);

    virtual INS_Result InsCHVS_RegisterCallback(unsigned int uEventType, RunHookFnPtr fEventFunc, void* pContext);
    virtual INS_Result InsCHVS_UnRegisterCallback(unsigned int uEventType);

    INS_Result InsCHVS_RegisterCallback_DeviceRemove(RemoveHookFnPtr callback, void* pContext);
    INS_Result InsCHVS_UnRegisterCallback_DeviceRemove();

    INS_Result InsCHVS_Set_ROI_Enable(IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_ROI_Enable(OUT InsCHVS_FuncEnable* a_enable_status_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_ROI_Rangle(IN const int startx, IN const int endx, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_ROI_Rangle(OUT int& startx, OUT int& endx, IN uint32_t aTimeOut);


    // blend trigger function, add in 20240531
    INS_Result InsCHVS_Set_Acq_Blend_Enable(IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_Blend_Enable(OUT InsCHVS_FuncEnable* a_enable_status_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendStart_TrigSource(IN InsCHVS_BlendTriggerStartSource a_trigger_source, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_BlendStart_TrigSource(OUT InsCHVS_BlendTriggerStartSource* a_trigger_source_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendEnd_TrigSource(IN InsCHVS_BlendTriggerEndSource a_trigger_source, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_BlendEnd_TrigSource(OUT InsCHVS_BlendTriggerEndSource* a_trigger_source_ptr, IN uint32_t aTimeOut);

    INS_Result InsCHVS_Set_Acq_BlendStart_ManualStart(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendStart_IOMode(IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_BlendStart_IOMode(OUT InsCHVS_BlendTriggerIOMode* a_mode_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendEnd_ManualEnd(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendEnd_IOMode(IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_BlendEnd_IOMode(OUT InsCHVS_BlendTriggerIOMode* a_mode_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendEnd_EncoderWidth(IN uint32_t width, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_BlendEnd_EncoderWidth(OUT uint32_t* width, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendEnd_TriggerNum(IN uint32_t num, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_BlendEnd_TriggerNum(OUT uint32_t* num, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq_BlendEnd_TimerMS(IN float32_t time, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq_BlendEnd_TimerMS(OUT float32_t* time, IN uint32_t aTimeOut);

    // blend trigger function, add in 20240531
    INS_Result InsCHVS_Set_Acq2_Blend_Enable(IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_Blend_Enable(OUT InsCHVS_FuncEnable* a_enable_status_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendStart_TrigSource(IN InsCHVS_BlendTriggerStartSource a_trigger_source, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_BlendStart_TrigSource(OUT InsCHVS_BlendTriggerStartSource* a_trigger_source_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendEnd_TrigSource(IN InsCHVS_BlendTriggerEndSource a_trigger_source, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_BlendEnd_TrigSource(OUT InsCHVS_BlendTriggerEndSource* a_trigger_source_ptr, IN uint32_t aTimeOut);

    INS_Result InsCHVS_Set_Acq2_BlendStart_ManualStart(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendStart_IOMode(IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_BlendStart_IOMode(OUT InsCHVS_BlendTriggerIOMode* a_mode_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendEnd_ManualEnd(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendEnd_IOMode(IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_BlendEnd_IOMode(OUT InsCHVS_BlendTriggerIOMode* a_mode_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendEnd_EncoderWidth(IN uint32_t width, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_BlendEnd_EncoderWidth(OUT uint32_t* width, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendEnd_TriggerNum(IN uint32_t num, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_BlendEnd_TriggerNum(OUT uint32_t* num, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_Acq2_BlendEnd_TimerMS(IN float32_t time, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_Acq2_BlendEnd_TimerMS(OUT float32_t* time, IN uint32_t aTimeOut);

    INS_Result InsCHVS_Cmd_FlatfieldCorrect_Initialize();
    INS_Result InsCHVS_Cmd_FlatfieldCorrect_Terminate();
    INS_Result InsCHVS_Cmd_FlatfieldCorrect_Calcaute(IN const char* imagePath, IN size_t size, IN int width, IN int height);
    INS_Result InsCHVS_Set_FlatfieldCorrect_Enable(IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_FlatfieldCorrect_Enable(OUT InsCHVS_FuncEnable* a_enable_status_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Save_FlatfieldCorrect_Param(IN uint32_t aTimeOut);
    INS_Result InsCHVS_Delete_FlatfieldCorrect_Param(IN uint32_t aTimeOut);
    
    // 20240919
    INS_Result InsCHVS_Set_FrameTrigger_Enable(IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_FrameTrigger_Enable(OUT InsCHVS_FuncEnable* a_enable_status_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_FrameTrigger_Duration(IN float32_t duration, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_FrameTrigger_Duration(OUT float32_t* duration_ptr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_FrameTrigger_Polar(IN uint32_t polar, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_FrameTrigger_Polar(OUT uint32_t* polar_ptr, IN uint32_t aTimeOut);


    /// <summary>
    /// The version number of the camera is valid after November 22, 2024 (2024.11.22)
    /// 1. It is also possible to obtain the validity of this module by calling "InsCHVS_Get_SevPrm(INS_PRM_IO_TRIGGER_SIGNAL_MODULE,&enable)"
    /// 2. New external IO trigger signal setting, independent of other modes, takes effect globally after setting
    /// 3. When this module takes effect, The interfaces of InsCHVS_Set_Acq/Acq2_IO_FilterWidth and InsCHVS_Set_Acq/Acq2_IO_InputDiv are no longer effective
    /// </summary>
    ///<param name="FilterWidth">IO signal filtering width, unit: us</param>
    ///<param name="InputDivision">IO signal cycle T, every T signals, there is a valid signal</param>
    ///<param name="EffectivelySignalNumber">IO signal number n, within T signal cycles, the nth signal is valid</param>
    ///<param name="Polarity">Polarity, default 0, write 1 to indicate polarity reversal</param>
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_FilterWidth(IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_InputDivision(IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_EffectiveSignalNumber(IN uint32_t aNumber, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_Polarity(IN uint32_t aPolarity, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_FilterWidth(OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_InputDivision(OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_EffectiveSignalNumber(OUT uint32_t* aNumberPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_Polarity(OUT uint32_t* aPolarityPtr, IN uint32_t aTimeOut);
    //
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_FilterWidth(IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_InputDivision(IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_EffectiveSignalNumber(IN uint32_t aNumber, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_Polarity(IN uint32_t aPolarity, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_FilterWidth(OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_InputDivision(OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_EffectiveSignalNumber(OUT uint32_t* aNumberPtr, IN uint32_t aTimeOut);
    INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_Polarity(OUT uint32_t* aPolarityPtr, IN uint32_t aTimeOut);

private:
    InsCHVSCamera_p*                p_;
};

#endif // ! INS_CHVS_CTRL_