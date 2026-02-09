#pragma once
#include "ins_chvs_params.hpp"

namespace InsCHVS
{
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Initialize_Logger_Param(IN InsCHVS_LoggerParam param);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_SoftWare_Version(OUT char* a_version, size_t size);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Initialize();
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Terminate();

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_FindDevice(OUT InsCHVS_DeviceInfoList * device_info_list);

    extern "C" INS_CHVS_DLL_API bool InsCHVS_Get_ResultCodeString(IN INS_Result ret, OUT char* code, IN size_t * size);
    extern "C" INS_CHVS_DLL_API bool InsCHVS_Get_ResultDescriptionCN(IN INS_Result ret, OUT char* descriptionCN, IN size_t * size);
    extern "C" INS_CHVS_DLL_API bool InsCHVS_Get_ResultDescriptionEN(IN INS_Result ret, OUT char* descriptionEN, IN size_t * size);

    extern "C" INS_CHVS_DLL_API void InsCHVS_CreateHandle(void** handle);
    extern "C" INS_CHVS_DLL_API void InsCHVS_DestroyHandle(void* handle);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Open(IN void* ins_handle, IN const int a_index);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Close(IN void* ins_handle);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Start(IN void* ins_handle);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Stop(IN void* ins_handle);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_RunState(IN void* ins_handle, IN InsCHVS_RunState* a_run_state);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_ConnectionState(IN void* ins_handle, IN InsCHVS_ConnectionState* a_connection_state);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_AcquireError(IN void* ins_handle, OUT uint32_t * ErrNum, OUT char* ErrStr, int size);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_AcquireImage(IN void* ins_handle, OUT InsCHVS_Buffer * ins_buffer);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_DevInfo(IN void* ins_handle, OUT InsCHVS_DeviceInfo * info);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_DevPrm(IN void* ins_handle, IN int _index, OUT void* _pValue);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_DevPrm(IN void* ins_handle, IN int _index, IN int _value);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Load_ConfigFile(IN void* ins_handle, IN const char* a_filename, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Save_ConfigFile(IN void* ins_handle, IN const char* a_filename, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_MetaData_State(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_MetaData_State(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_status, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Dev_TestReg(IN void* ins_handle, IN uint32_t aRegVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Dev_TestReg(IN void* ins_handle, OUT uint32_t * aRegValPtr, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_SensorReset(IN void* ins_handle, IN uint32_t aTimeOut);/* 功能未开启 */
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_FPGAReset(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_DeviceReset(IN void* ins_handle, IN uint32_t aTimeOut);

    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_TrigSource(IN void* ins_handle, IN InsCHVS_LineTriggerSource a_trigger_source, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_TrigSource(IN void* ins_handle, OUT InsCHVS_LineTriggerSource * a_trigger_source_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Intern_TrigPeriod(IN void* ins_handle, IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Intern_TrigNums(IN void* ins_handle, IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_IO_TrigPeriod(IN void* ins_handle, IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_IO_Mode(IN void* ins_handle, IN InsCHVS_IO_TriggerMode aMode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_IO_TrigNums(IN void* ins_handle, IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_IO_OutputDelay(IN void* ins_handle, IN float32_t aOutputDelay, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_IO_FilterWidth(IN void* ins_handle, IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_IO_InputDiv(IN void* ins_handle, IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_CountMode(IN void* ins_handle, IN InsCHVS_Encoder_CountMode aCountMode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_TriggerMode(IN void* ins_handle, IN InsCHVS_Encoder_TriggerMode aDirMode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_Ignore(IN void* ins_handle, IN uint32_t aIgnoreNums, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_FilterWidth(IN void* ins_handle, IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_InputDiv(IN void* ins_handle, IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_Enable_InputMultiple(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_multi, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_InputMultiple(IN void* ins_handle, IN uint32_t aInputMulti, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Encoder_RollbackIgnore(IN void* ins_handle, IN uint32_t aEncoderPulsesCount, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Fake_FPGA_Mode(IN void* ins_handle, IN InsCHVS_FakePatternMode aMode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Fake_FPGA_Period(IN void* ins_handle, IN float32_t aPeriod, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Fake_FPGA_Format(IN void* ins_handle, IN uint16_t aRows, IN uint16_t aCollums, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_status_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_TrigSource(IN void* ins_handle, IN InsCHVS_LineTriggerSource a_trigger_source, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_TrigSource(IN void* ins_handle, OUT InsCHVS_LineTriggerSource * a_trigger_source_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Intern_TrigPeriod(IN void* ins_handle, IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Intern_TrigNums(IN void* ins_handle, IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_IO_TrigPeriod(IN void* ins_handle, IN float32_t aTrigPeriod, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_IO_Mode(IN void* ins_handle, IN InsCHVS_IO_TriggerMode aMode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_IO_TrigNums(IN void* ins_handle, IN uint32_t aTrigNums, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_IO_OutputDelay(IN void* ins_handle, IN float32_t aOutputDelay, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_IO_FilterWidth(IN void* ins_handle, IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_IO_InputDiv(IN void* ins_handle, IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_CountMode(IN void* ins_handle, IN InsCHVS_Encoder_CountMode aCountMode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_TriggerMode(IN void* ins_handle, IN InsCHVS_Encoder_TriggerMode aDirMode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_Ignore(IN void* ins_handle, IN uint32_t aIgnoreNums, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_FilterWidth(IN void* ins_handle, IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_InputDiv(IN void* ins_handle, IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_Enable_InputMultiple(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_multi, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_InputMultiple(IN void* ins_handle, IN uint32_t aInputMulti, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Encoder_RollbackIgnore(IN void* ins_handle, IN uint32_t aEncoderPulsesCount, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Intern_TrigPeriod(IN void* ins_handle, OUT float32_t * aTrigPeriodPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Intern_TrigNums(IN void* ins_handle, OUT uint32_t * aTrigNumsPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_IO_TrigPeriod(IN void* ins_handle, OUT float32_t * aTrigPeriodPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_IO_Mode(IN void* ins_handle, OUT InsCHVS_IO_TriggerMode * aModePtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_IO_TrigNums(IN void* ins_handle, OUT uint32_t * aTrigNumsPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_IO_OutputDelay(IN void* ins_handle, OUT float32_t * aOutputDelayPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_IO_FilterWidth(IN void* ins_handle, OUT float32_t * aFilterWidthPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_IO_InputDiv(IN void* ins_handle, OUT uint32_t * aInputDivPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_CountMode(IN void* ins_handle, OUT InsCHVS_Encoder_CountMode * aCountModePtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_TriggerMode(IN void* ins_handle, OUT InsCHVS_Encoder_TriggerMode * aTriggerModePtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_Ignore(IN void* ins_handle, OUT uint32_t * aIgnoreNumsPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_FilterWidth(IN void* ins_handle, OUT float32_t * aFilterWidthPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_InputDiv(IN void* ins_handle, OUT uint32_t * aInputDivPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_Enable_InputMultiple(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_multi, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_InputMultiple(IN void* ins_handle, OUT uint32_t * aInputMulti, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Encoder_RollbackIgnore(IN void* ins_handle, OUT uint32_t* aEncoderPulsesCount, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Fake_FPGA_Mode(IN void* ins_handle, OUT InsCHVS_FakePatternMode * aModePtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Fake_FPGA_Period(IN void* ins_handle, OUT float32_t * aPeriodPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Intern_TrigPeriod(IN void* ins_handle, OUT float32_t * aTrigPeriodPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Intern_TrigNums(IN void* ins_handle, OUT uint32_t * aTrigNumsPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_IO_TrigPeriod(IN void* ins_handle, OUT float32_t * aTrigPeriodPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_IO_Mode(IN void* ins_handle, OUT InsCHVS_IO_TriggerMode * aModePtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_IO_TrigNums(IN void* ins_handle, OUT uint32_t * aTrigNumsPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_IO_OutputDelay(IN void* ins_handle, OUT float32_t * aOutputDelayPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_IO_FilterWidth(IN void* ins_handle, OUT float32_t * aFilterWidthPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_IO_InputDiv(IN void* ins_handle, OUT uint32_t * aInputDivPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_CountMode(IN void* ins_handle, OUT InsCHVS_Encoder_CountMode * aCountModePtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_TriggerMode(IN void* ins_handle, OUT InsCHVS_Encoder_TriggerMode * aTriggerModePtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_Ignore(IN void* ins_handle, OUT uint32_t * aIgnoreNumsPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_FilterWidth(IN void* ins_handle, OUT float32_t * aFilterWidthPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_InputDiv(IN void* ins_handle, OUT uint32_t * aInputDivPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_Enable_InputMultiple(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_multi, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_InputMultiple(IN void* ins_handle, OUT uint32_t * aInputMulti, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Encoder_RollbackIgnore(IN void* ins_handle, OUT uint32_t* aEncoderPulsesCount, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Img_TransHeight(IN void* ins_handle, IN uint32_t aTransHeight, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Img_TransHeight(IN void* ins_handle, OUT uint32_t * aTransHeightPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Img_CombTimeout(IN void* ins_handle, IN uint32_t aCombTimeout, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Img_CombTimeout(IN void* ins_handle, OUT uint32_t * aCombTimeoutPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Img_PixFormat(IN void* ins_handle, IN InsCHVS_PixelFormat a_pixel_format, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Img_PixFormat(IN void* ins_handle, OUT InsCHVS_PixelFormat * a_pixel_format_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Img_FrameBitsCycle(IN void* ins_handle, IN uint32_t aFrameBitsCycle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Img_FrameBitsCycle(IN void* ins_handle, OUT uint32_t * aFrameBitsCyclePtr, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Camera_Counter(IN void* ins_handle, IN int id, IN uint32_t index, OUT uint32_t* pValue);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Clear_ImageCounters(IN void* ins_handle, IN uint32_t aTimeOut);
    // 20240927
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Encoder_Counter(IN void* ins_handle, IN uint32_t index, OUT uint32_t * pValue);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Reverse_EncoderCounters(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Clear_EncoderCounters(IN void* ins_handle, IN uint32_t aTimeOut);
    // 20241121
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOSignal_Counter(IN void* ins_handle, IN uint32_t index, OUT uint32_t* pValue);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_Clear_IOSignalCounters(IN void* ins_handle, IN uint32_t aTimeOut);

    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_FPGA_Temperature(IN void* ins_handle, IN int id, OUT float* aDevTmpPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Device_Temperature(IN void* ins_handle, IN int id, OUT float* aDevTmpPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Device_RTC(IN void* ins_handle, IN int id, OUT uint64_t * aDevRTC, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Device_RTC_Str(IN void* ins_handle, IN int id, OUT char* aDevRTC_Str, int size, IN uint32_t aTimeOut);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_LED_TriggerMode(IN void* ins_handle, IN InsCHVS_LED_TriggerMode a_trig_bitmask, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_LED_TriggerMode(IN void* ins_handle, OUT InsCHVS_LED_TriggerMode * a_trig_bitmask_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Timing_Cycle(IN void* ins_handle, OUT float32_t * aTimeLineMaxPtr, IN uint32_t aTimeOut);
    //set
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_TimeLine_Max(IN void* ins_handle, OUT float32_t * aTimeLineMaxPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_LED_ExposureTime1(IN void* ins_handle, IN float32_t a_led_time, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_LED_ExposureTime2(IN void* ins_handle, IN float32_t a_led_time_1, IN float32_t a_led_time_2, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_LED_ExposureTime3(IN void* ins_handle, IN float32_t a_led_time_1, IN float32_t a_led_time_2, IN float32_t a_led_time_3, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_LED_ExposureTime1(IN void* ins_handle, OUT float32_t * a_led_time_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_LED_ExposureTime2(IN void* ins_handle, OUT float32_t * a_led1_time_ptr, OUT float32_t * a_led2_time_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_LED_ExposureTime3(IN void* ins_handle, OUT float32_t * a_led1_time_ptr, OUT float32_t * a_led2_time_ptr, OUT float32_t * a_led3_time_ptr, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Img_DPI(IN void* ins_handle, OUT InsCHVS_DPI * a_img_dpi_ptr, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Correct_Dark_TarVal(IN void* ins_handle, IN uint32_t aDarkTarVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Correct_Dark_TarVal(IN void* ins_handle, OUT uint32_t * aDarkTarVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Correct_Bright_TarVal(IN void* ins_handle, IN uint32_t aBrightTarVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Correct_Bright_TarVal(IN void* ins_handle, OUT uint32_t * aBrightTarVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Req_Dark_Correct(IN void* ins_handle, IN uint32_t aTimeOut, OUT uint32_t * a_error_bits_ptr = NULL);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Req_Bright_Correct(IN void* ins_handle, IN uint32_t aTimeOut, OUT uint32_t * a_error_bits_ptr = NULL);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Gamma_Config(IN void* ins_handle, IN float32_t aTarVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Gamma_Config(IN void* ins_handle, OUT float32_t * aTarValPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Gamma_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable aEnableBit, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Gamma_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * aEnableBit_Ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Correct_Pix_TarVal(IN void* ins_handle, IN uint32_t aPixTarVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Correct_Pix_TarVal(IN void* ins_handle, OUT uint32_t * aPixTarVal, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Req_Correct_Pix(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Pix_Correct_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable aEnableBit, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Pix_Correct_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * aEnableBit_Ptr, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Global_User_DGain(IN void* ins_handle, OUT double* aDGainVal_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Global_User_DGain(IN void* ins_handle, IN double aDGainVal, IN uint32_t aTimeOut);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Calibrate_Camera(IN void* ins_handle, IN uint32_t aBrightTarVal = 150, IN uint32_t aDarkTarVal = 4, IN uint32_t aPixTarVal = 180, IN uint32_t aBrightTarVal_Backlight = 180, IN uint32_t aPixTarVal_Backlight = 210);
    //
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Save_DarkCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Delete_DarkCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Save_WhiteCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Delete_WhiteCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Save_PixelCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Delete_PixelCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Save_DeviceControl_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Delete_DeviceControl_Param(IN void* ins_handle, IN uint32_t aTimeOut);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_BandWidth(IN void* ins_handle, OUT double* aBandWidth, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_AcquisitionRate(IN void* ins_handle, OUT double* aAcquisitionRate, IN uint32_t aTimeOut);

    extern "C" INS_CHVS_DLL_API INS_Result Advanced_Permissions_Register_Read_Single(IN void* ins_handle, IN const char* password, IN const uint32_t addr, OUT uint32_t & value, IN int id = 1);
    extern "C" INS_CHVS_DLL_API INS_Result Advanced_Permissions_Register_Read_Multiple(IN void* ins_handle, IN const char* password, IN const uint32_t addr, OUT uint32_t value[], IN int size);
    extern "C" INS_CHVS_DLL_API INS_Result Advanced_Permissions_Register_Write_ALL(IN void* ins_handle, IN const char* password, IN const uint32_t addr, IN const uint32_t value);
    extern "C" INS_CHVS_DLL_API INS_Result Advanced_Permissions_Register_Write_Single(IN void* ins_handle, IN const char* password, IN const uint32_t addr, IN const uint32_t value, IN int id);
    extern "C" INS_CHVS_DLL_API INS_Result Advanced_Permissions_Register_Write_Multiple(IN void* ins_handle, IN const char* password, IN const uint32_t addr, IN const uint32_t value[], IN int size);
    extern "C" INS_CHVS_DLL_API INS_Result Advanced_Permissions_Block_Read(IN void* ins_handle, IN const char* password, IN const uint32_t addr, OUT uint64_t valueAddress, IN const uint32_t length, IN int id);
    extern "C" INS_CHVS_DLL_API INS_Result Advanced_Permissions_Block_Write(IN void* ins_handle, IN const char* password, IN const uint32_t addr, IN const uint64_t valueAddress, IN const uint32_t length, IN int id);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Sensor_Offset(IN void* ins_handle, IN const float _OffsetValue[], IN int numRows, IN int numColumns);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Sensor_Dgain(IN void* ins_handle, IN const float _DgainValue[], IN int numRows, IN int numColumns);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Sensor_Offset(IN void* ins_handle, OUT float _OffsetValue[], IN int size, int id);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Sensor_Dgain(IN void* ins_handle, OUT float _DgainValue[], IN int size, int id);
    
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_RegisterCallback(IN void* ins_handle, unsigned int uEventType, RunHookFnPtr fEventFunc, void* pContext);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_UnRegisterCallback(IN void* ins_handle, unsigned int uEventType);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_RegisterCallback_DeviceRemove(IN void* ins_handle, RemoveHookFnPtr callback, void* pContext);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_UnRegisterCallback_DeviceRemove(IN void* ins_handle);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_ROI_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_ROI_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_status_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_ROI_Rangle(IN void* ins_handle, IN const int startx, IN const int endx, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_ROI_Rangle(IN void* ins_handle, OUT int* startx, OUT int* endx, IN uint32_t aTimeOut);

    // blend trigger function, add in 20240531, testing now
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_Blend_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_Blend_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_status_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendStart_TrigSource(IN void* ins_handle, IN InsCHVS_BlendTriggerStartSource a_trigger_source, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_BlendStart_TrigSource(IN void* ins_handle, OUT InsCHVS_BlendTriggerStartSource * a_trigger_source_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendEnd_TrigSource(IN void* ins_handle, IN InsCHVS_BlendTriggerEndSource a_trigger_source, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_BlendEnd_TrigSource(IN void* ins_handle, OUT InsCHVS_BlendTriggerEndSource * a_trigger_source_ptr, IN uint32_t aTimeOut);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendStart_ManualStart(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendStart_IOMode(IN void* ins_handle, IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_BlendStart_IOMode(IN void* ins_handle, OUT InsCHVS_BlendTriggerIOMode * a_mode_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendEnd_ManualEnd(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendEnd_IOMode(IN void* ins_handle, IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_BlendEnd_IOMode(IN void* ins_handle, OUT InsCHVS_BlendTriggerIOMode * a_mode_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendEnd_EncoderWidth(IN void* ins_handle, IN uint32_t width, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_BlendEnd_EncoderWidth(IN void* ins_handle, OUT uint32_t * width, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendEnd_TriggerNum(IN void* ins_handle, IN uint32_t num, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_BlendEnd_TriggerNum(IN void* ins_handle, OUT uint32_t * num, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq_BlendEnd_TimerMS(IN void* ins_handle, IN float32_t time, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq_BlendEnd_TimerMS(IN void* ins_handle, OUT float32_t * time, IN uint32_t aTimeOut);

    // blend trigger function, add in 20240531, testing now
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_Blend_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_Blend_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_status_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendStart_TrigSource(IN void* ins_handle, IN InsCHVS_BlendTriggerStartSource a_trigger_source, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_BlendStart_TrigSource(IN void* ins_handle, OUT InsCHVS_BlendTriggerStartSource * a_trigger_source_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendEnd_TrigSource(IN void* ins_handle, IN InsCHVS_BlendTriggerEndSource a_trigger_source, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_BlendEnd_TrigSource(IN void* ins_handle, OUT InsCHVS_BlendTriggerEndSource * a_trigger_source_ptr, IN uint32_t aTimeOut);

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendStart_ManualStart(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendStart_IOMode(IN void* ins_handle, IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_BlendStart_IOMode(IN void* ins_handle, OUT InsCHVS_BlendTriggerIOMode * a_mode_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendEnd_ManualEnd(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendEnd_IOMode(IN void* ins_handle, IN InsCHVS_BlendTriggerIOMode a_mode, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_BlendEnd_IOMode(IN void* ins_handle, OUT InsCHVS_BlendTriggerIOMode * a_mode_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendEnd_EncoderWidth(IN void* ins_handle, IN uint32_t width, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_BlendEnd_EncoderWidth(IN void* ins_handle, OUT uint32_t * width, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendEnd_TriggerNum(IN void* ins_handle, IN uint32_t num, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_BlendEnd_TriggerNum(IN void* ins_handle, OUT uint32_t * num, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_Acq2_BlendEnd_TimerMS(IN void* ins_handle, IN float32_t time, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_Acq2_BlendEnd_TimerMS(IN void* ins_handle, OUT float32_t * time, IN uint32_t aTimeOut);


    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_FlatfieldCorrect_Initialize(IN void* ins_handle);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_FlatfieldCorrect_Terminate(IN void* ins_handle);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Cmd_FlatfieldCorrect_Calcaute(IN void* ins_handle, IN const char* imagePath, IN size_t size, IN int width, IN int height);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_FlatfieldCorrect_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_FlatfieldCorrect_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_status_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Save_FlatfieldCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Delete_FlatfieldCorrect_Param(IN void* ins_handle, IN uint32_t aTimeOut);
    
    // 20240919
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_FrameTrigger_Enable(IN void* ins_handle, IN InsCHVS_FuncEnable a_enable_status, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_FrameTrigger_Enable(IN void* ins_handle, OUT InsCHVS_FuncEnable * a_enable_status_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_FrameTrigger_Duration(IN void* ins_handle, IN float32_t duration, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_FrameTrigger_Duration(IN void* ins_handle, OUT float32_t* duration_ptr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_FrameTrigger_Polar(IN void* ins_handle, IN uint32_t polar, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_FrameTrigger_Polar(IN void* ins_handle, OUT uint32_t* polar_ptr, IN uint32_t aTimeOut);



    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_FilterWidth(IN void* ins_handle, IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_InputDivision(IN void* ins_handle, IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_EffectiveSignalNumber(IN void* ins_handle, IN uint32_t aNumber, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input0_Polarity(IN void* ins_handle, IN uint32_t aPolarity, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_FilterWidth(IN void* ins_handle, OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_InputDivision(IN void* ins_handle, OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_EffectiveSignalNumber(IN void* ins_handle, OUT uint32_t* aNumberPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input0_Polarity(IN void* ins_handle, OUT uint32_t* aPolarityPtr, IN uint32_t aTimeOut);
    //

    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_FilterWidth(IN void* ins_handle, IN float32_t aFilterWidth, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_InputDivision(IN void* ins_handle, IN uint32_t aInputDiv, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_EffectiveSignalNumber(IN void* ins_handle, IN uint32_t aNumber, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Set_IOExternalTriggerSignal_Input1_Polarity(IN void* ins_handle, IN uint32_t aPolarity, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_FilterWidth(IN void* ins_handle, OUT float32_t* aFilterWidthPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_InputDivision(IN void* ins_handle, OUT uint32_t* aInputDivPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_EffectiveSignalNumber(IN void* ins_handle, OUT uint32_t* aNumberPtr, IN uint32_t aTimeOut);
    extern "C" INS_CHVS_DLL_API INS_Result InsCHVS_Get_IOExternalTriggerSignal_Input1_Polarity(IN void* ins_handle, OUT uint32_t* aPolarityPtr, IN uint32_t aTimeOut);
}

