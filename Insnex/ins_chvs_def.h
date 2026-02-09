#pragma once

/****************************************************************************
* 相机型号定义
* Camera model definition
****************************************************************************/
#define INS_CHVS_363                    (1)
#define INS_CHVS_550                    (2)
#define INS_CHVS_934                    (3)
#define INS_CHVS_1245                   (4)
#define INS_CHVS_1350                   (5)
#define INS_CHVS_1550                   (6)
#define INS_CHVS_366                    (7)
#define INS_CHVS_310                    (8)
#define INS_CHVS_190                    (9)
#define INS_CHVS_230                    (10)
#define INS_CHVS_1453                   (11)
#define INS_CHVS_620                    (12) 
#define INS_CHVS_311                    (13)
#define INS_CHVS_69                     (14)
#define INS_CHVS_813                    (15)
#define INS_CHVS_1600                   (16)
#define INS_CHVS_415                    (17)
#define INS_CHVS_940                    (18)
#define INS_CHVS_35                     (19)
#define INS_CHVS_450                    (20)

#define INS_LXM_114                     (1U << 8)
#define INS_LXM_228                     (2U << 8)
#define INS_LXM_340                     (3U << 8)
#define INS_LXM_328                     (4U << 8)
#define INS_LXM_360                     (5U << 8)
#define INS_LXM_656                     (6U << 8)
#define INS_LXM_164                     (7U << 8)

#define INS_DPI_300                     (1)
#define INS_DPI_600                     (2)
#define INS_DPI_1200                    (3)
#define INS_DPI_900                     (4)
#define INS_DPI_1800                    (5)
#define INS_DPI_3600                    (6)

#define INS_MEDIUM_SPEED                (1)
#define INS_HIGH_SPEED                  (2)
#define INS_LOW_SPEED                   (3)
#define INS_MIDDLE_SPEED                (4)
#define INS_FULL_SPEED                  (5)
#define INS_MAX_SPEED                   (7)

#define INS_GIGE                        (1)
#define INS_CAMERALINK                  (2)

#define INS_MONO                        (1)
#define INS_COLOR                       (2)

#define INS_TYPE(TYPE, _INTERFACE, PIXEL, DPI, SPEED)       ((TYPE << 16) + (_INTERFACE << 12) + (PIXEL << 8) + (DPI << 4) + (SPEED << 0))

//API parameter types
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif






/****************************************************************************
*
* 下面"INS_PRM_*"的参数，使用"InsCHVS_Get_DevPrm"、"InsCHVS_Set_DevPrm"调用设置和获取
* 如果有"INS_VAL_*"，则表示限制了可以设置的值
*
* The parameter of "INS_PRM_ *" below can be set and obtained by calling "InsCHVS_Set_DevPrm" and "InsCHVS_Set_SevPrm"
* If there is' INS_VAL_ * ', it means that the values that can be set are limited
*
****************************************************************************/
#define INS_PRM_FIRST						0
#define INS_PRM(number, size)               ((((number) + INS_PRM_FIRST) << 16) | (size))

#define INS_PRM_BUFFER_ROI_HEIGHT           INS_PRM(0, 4)           /* Get the current height. */
#define INS_PRM_BUFFER_ROI_WIDTH            INS_PRM(1, 4)           /* Get the current width of the META data. */
#define INS_PRM_BUFFER_ROI_IMG_WIDTH        INS_PRM(2, 4)           /* Get the current image width. */
#define INS_PRM_PERIOD_DPI_MAX              INS_PRM(3, 4)           /* Get the maximum line frequency limit. */
#define INS_PRM_ETHERNET_PORTS              INS_PRM(4, 4)           /* Get the number of camera Ethernet ports. */
#define INS_PRM_SENSOR_COUNT                INS_PRM(5, 4)           /* Get the number of camera sensors. */

#define INS_PRM_BUFFER_STROBING_MODE        INS_PRM(6, 4)           /* Get or set whether the image buffer separates images under different light sources. */
#define INS_VAL_BUFFER_STROBING_ONE         (0x00000000)            /* Strobing mode for one light source. */
#define INS_VAL_BUFFER_STROBING_MULTIPLE    (0x00000001)            /* Strobing mode for multiple light sources. */

#define INS_PRM_BUFFER_COUNT_BOARD          INS_PRM(7, 4)           /* Set the number of buffer for the acquisition card. */
#define INS_PRM_BUFFER_COUNT_CAMERA         INS_PRM(8, 4)           /* Set the number of buffer for the SDK. */

#define INS_PRM_BUFFER_FLIP                 INS_PRM(9, 4)           /* Set the image flipping function. */
#define INS_VAL_BUFFER_FLIP_NORMAL          (0x00000000)            /* Normal flip mode. */
#define INS_VAL_BUFFER_FLIP_HORIZONTAL      (0x00000001)            /* Horizontal flip mode. */
#define INS_VAL_BUFFER_FLIP_VERTICAL        (0x00000002)            /* Vertical flip mode. */
#define INS_VAL_BUFFER_FLIP_ALL             (0x00000003)            /* Flip all modes. */

/*
* Set the selection of LED lights in different light source modes.
* The camera has 4 light source interfaces, with AB interfaces installed by default.
* The LEDs used in the four light source modes are: AB AB、ABC、C。
* You can freely set the LED selection in the corresponding light source mode using InsCHVS_Set_DevPrm.
* InsCHVS_Get_DevPrm can be used to obtain the LED selection corresponding to the light source mode.
*
* 设置不同光源模式下的led灯选择.
* 相机留有4个光源接口，默认安装了AB接口.
* 四个光源模式使用的led分别为：AB、AB、ABC、C。
* 使用InsCHVS_Set_DevPrm可以自由设置对应光源模式下的led选择。
* 使用InsCHVS_Get_DevPrm可以获取对应光源模式下的led选择。
*/
#define INS_PRM_TDI_LIGHT_SimultaneousDualBrightness                  INS_PRM(10, 4)
#define INS_PRM_TDI_LIGHT_SeparateTimedFlashingBrightness             INS_PRM(11, 4)
#define INS_PRM_TDI_LIGHT_TripleIndependentBrightness                 INS_PRM(12, 4)
#define INS_PRM_TDI_LIGHT_BacklightOnly                               INS_PRM(13, 4)
#define INS_VAL_TDI_LIGHT_A                                           (0B0001)
#define INS_VAL_TDI_LIGHT_B                                           (0B0010)
#define INS_VAL_TDI_LIGHT_C                                           (0B0100)
#define INS_VAL_TDI_LIGHT_D                                           (0B1000)
#define INS_VAL_TDI_LIGHT_AB                                          (0B0011)
#define INS_VAL_TDI_LIGHT_AC                                          (0B0101)
#define INS_VAL_TDI_LIGHT_AD                                          (0B1001)
#define INS_VAL_TDI_LIGHT_BC                                          (0B0110)
#define INS_VAL_TDI_LIGHT_BD                                          (0B1010)
#define INS_VAL_TDI_LIGHT_CD                                          (0B1100)
#define INS_VAL_TDI_LIGHT_ABC                                         (0B0111)
#define INS_VAL_TDI_LIGHT_ABD                                         (0B1011)
#define INS_VAL_TDI_LIGHT_ACD                                         (0B1101)
#define INS_VAL_TDI_LIGHT_BCD                                         (0B1110)
#define INS_VAL_TDI_LIGHT_ABCD                                        (0B1111)

#define INS_PRM_BUFFER_CLEAR                                          INS_PRM(14, 4)  /* Set 1. After finishing the image retrieval, clear the buffer space occupied by the image inside the computer */



// Stitching mode: 
// 1. INS_VAL_STITCH_ALGORITHM——Algorithm stitching, the stitching effect is better than the stitching speed
// 2. INS_VAL_STIICH_CAMERA——Camera stitching, the stitching speed is better than stitching effect
#define INS_PRM_STITCH_MODE											   INS_PRM(15, 4)
#define INS_VAL_STITCH_NONEED										   (0x00000000)
#define INS_VAL_STITCH_ALGORITHM									   (0x00000002)
#define INS_VAL_STIICH_CAMERA										   (0x00000003)
#define INS_VAL_STIICH_CAMERA_HIGH_SPEED						   	   (0x00000004)
#define INS_VAL_STIICH_CAMERA_FUSION						   		   (0x00000005)

// Write 1 to release the buffer cache inside the acquisition board
#define INS_PRM_CLEAR_BUFFER										   INS_PRM(16, 4)

// Obtain exposure error counts at 900 and 1800dpi
#define INS_PRM_EXPOSURE_ERROR_COUNT									INS_PRM(17, 4)
// If the exposure error count continues to increase, it is recommended to switch to full exposure mode
#define INS_PRM_EXPOSURE_MODE											INS_PRM(18, 4)
#define INS_VAL_NORMAL_EXPOSURE_MODE									(0x00000000)
#define INS_VAL_FULL_EXPOSURE_MODE										(0x00000001)

#define INS_PRM_BUFFER_ROI_IMG_STRIDE									INS_PRM(19, 4)  /* Get the current image stride. */
#define INS_PRM_IO_TRIGGER_SIGNAL_MODULE								INS_PRM(20, 4)  /* Obtain the validity of the IO trigger signal module. */


/****************************************************************************
*
* 相机的数据流统计信息需使用函数"InsCHVS_Get_Camera_Counter"获取：
* (1) INS_CHVS_363_10GM_BB、INS_CHVS_363_10GM_AA的相机，FPGA版本日期在2023年11月21日之前的固件使用"INS_363_COUNTER_*"的索引
* (2) 300DPI、600DPI、1200DPI的相机，FPGA版本日期在2024年9月1日之前的固件使用"INS_COUNTER_*"的索引
* (2) 300DPI、600DPI、1200DPI的相机，FPGA版本日期在2024年9月1日之后的固件使用"INS_NEW4CON_COUNTER_*"的索引
* (3) 900DPI、1800DPI的相机，使用"INS_COUNTER_*"的索引
* (4) id 表示网口的序号下标，n个网口的可输入下标范围为：1~n
* (5) index 表示计数器索引，用户常用的有以下部分:  a~c在上位机中的【统计信息->计数器信息】体现，d在图像显示区域中信息栏的【数据流计数】体现
*		a. "*_CaptureTriggerCount" : 采图触发计数
*		b. "*_AcquireTriggerCount" ：取图触发计数
*		c. "*_FrameCount" ： 取图帧计数
*		d. "*_DataStreamCount" ：数据流计数
*
* The data flow statistics information of the camera needs to be obtained using the function "InsCHVS_Get_Camera_Counter":
* (1) For cameras INS_CVS_363_10GM-BB and INS_CVS_363_10GM-AA, firmware with FPGA version dates before November 21, 2023 should use the index "INS_363_COUNTER *"
* (2) For cameras with 300DPI, 600DPI, and 1200DPI, firmware with FPGA version dates before September 1, 2024 should use the index "INS_COUNTER_*"
* (2) For cameras with 300DPI, 600DPI, and 1200DPI, firmware with FPGA version dates after September 1, 2024 should use the index "INS_NEW4CON_COUNTER_*"
* (3) For cameras with 900DPI and 1800DPI, use the index "INS_COUNTER_*"
* (4) id represents the serial index of the network port, and the range of input indices for n network ports is: 1~n
* (5) index represents the counter index, which is commonly used by users in the following parts: 
*	  [a~c] is reflected in the [Statistics -> Counter Information] section of the upper computer, 
*	  [d] is reflected in the [Data Stream] section of the information column in the image display area,
*		a. "* _CaptureTriggerCount": Capture trigger count
*		b. "* _CcquireTiggerCount": Triggering count for taking the graph
*		c. "* _SrameCount": take frame count
*		d. "* _SataStreamCount": Data stream count
* 
****************************************************************************/
#define INS_363_COUNTER_FIRST					(0)
#define INS_363_COUNTER(n, size)                ((((n) + INS_363_COUNTER_FIRST) << 16) | (size))

#define INS_363_COUNTER_TRIG			        INS_363_COUNTER(0, 4)
#define INS_363_COUNTER_SENSOR_CAP	            INS_363_COUNTER(1, 4)
#define INS_363_COUNTER_PIXEL_INSERT	        INS_363_COUNTER(2, 4)
#define INS_363_COUNTER_CACHE_DATA	            INS_363_COUNTER(3, 4)
#define INS_363_COUNTER_BUBBLE_REMOVE	        INS_363_COUNTER(4, 4)
#define INS_363_COUNTER_BLOCK_FRAME	            INS_363_COUNTER(5, 4)
#define INS_363_COUNTER_POST_P_FRAME	        INS_363_COUNTER(6, 4)
#define INS_363_COUNTER_OUTPUT_FRAME	        INS_363_COUNTER(7, 4)
#define INS_363_COUNTER_PIXEL8_FRAME	        INS_363_COUNTER(8, 4)
#define INS_363_COUNTER_FAKE_FRAME	            INS_363_COUNTER(9, 4)
#define INS_363_COUNTER_IPORT_FRAME	            INS_363_COUNTER(10, 4)
#define INS_363_COUNTER_ACQ2_FRAME	            INS_363_COUNTER(11, 4)
#define INS_363_COUNTER_SENSOR_FRAME_O	        INS_363_COUNTER(12, 4)
#define INS_363_COUNTER_SENSOR_DATA_0           INS_363_COUNTER(13, 4)
#define INS_363_COUNTER_SENSOR_FRAME_1          INS_363_COUNTER(14, 4)
#define INS_363_COUNTER_SENSOR_DATA_1           INS_363_COUNTER(15, 4)
#define INS_363_COUNTER_SENSOR_FRAME_2          INS_363_COUNTER(16, 4)
#define INS_363_COUNTER_SENSOR_DATA_2           INS_363_COUNTER(17, 4)
#define INS_363_COUNTER_TRIGGER_LIMIT           INS_363_COUNTER(18, 4)
#define INS_363_COUNTER_CaptureTriggerCount		INS_363_COUNTER_TRIG
#define INS_363_COUNTER_AcquireTriggerCount		INS_363_COUNTER_IPORT_FRAME
#define INS_363_COUNTER_FrameCount				INS_363_COUNTER_ACQ2_FRAME
#define INS_363_COUNTER_DataStreamCount			INS_363_COUNTER_AcquireTriggerCount

#define INS_COUNTER_FIRST						(0x100)
#define INS_COUNTER(n, size)					((((n) + INS_COUNTER_FIRST) << 16) | (size))

#define INS_COUNTER_TRIG						INS_COUNTER(1, 4)
#define INS_COUNTER_TRIGGER_LIMIT				INS_COUNTER(2, 4)
#define INS_COUNTER_SENSOR_DATA_1				INS_COUNTER(3, 4)
#define INS_COUNTER_SENSOR_DATA_2				INS_COUNTER(4, 4)
#define INS_COUNTER_SENSOR_DATA_3				INS_COUNTER(5, 4)
#define INS_COUNTER_SENSOR_DATA_4				INS_COUNTER(6, 4)
#define INS_COUNTER_SENSOR_FRAME_1				INS_COUNTER(7, 4)
#define INS_COUNTER_SENSOR_FRAME_2				INS_COUNTER(8, 4)
#define INS_COUNTER_SENSOR_FRAME_3				INS_COUNTER(9, 4)
#define INS_COUNTER_SENSOR_FRAME_4				INS_COUNTER(10, 4)
#define INS_COUNTER_DATA_CAP_1					INS_COUNTER(11, 4)
#define INS_COUNTER_DATA_CAP_2					INS_COUNTER(12, 4)
#define INS_COUNTER_DATA_CAP_3					INS_COUNTER(13, 4)
#define INS_COUNTER_DATA_CAP_4					INS_COUNTER(14, 4)
#define INS_COUNTER_INSERT_1					INS_COUNTER(15, 4)
#define INS_COUNTER_INSERT_2					INS_COUNTER(16, 4)
#define INS_COUNTER_INSERT_3					INS_COUNTER(17, 4)
#define INS_COUNTER_INSERT_4					INS_COUNTER(18, 4)
#define INS_COUNTER_CACHE_1						INS_COUNTER(19, 4)
#define INS_COUNTER_CACHE_2						INS_COUNTER(20, 4)
#define INS_COUNTER_CACHE_3						INS_COUNTER(21, 4)
#define INS_COUNTER_CACHE_4						INS_COUNTER(22, 4)
#define INS_COUNTER_BR_FRAME_1					INS_COUNTER(23, 4)
#define INS_COUNTER_BR_FRAME_2					INS_COUNTER(24, 4)
#define INS_COUNTER_BR_FRAME_3					INS_COUNTER(25, 4)
#define INS_COUNTER_BR_FRAME_4					INS_COUNTER(26, 4)
#define INS_COUNTER_IP_FRAME_1					INS_COUNTER(27, 4)
#define INS_COUNTER_IP_FRAME_2					INS_COUNTER(28, 4)
#define INS_COUNTER_IP_FRAME_3					INS_COUNTER(29, 4)
#define INS_COUNTER_IP_FRAME_4					INS_COUNTER(30, 4)
#define INS_COUNTER_8P_FRAME					INS_COUNTER(31, 4)
#define INS_COUNTER_IR_FRAME					INS_COUNTER(32, 4)
#define INS_COUNTER_FAKE_FRAME					INS_COUNTER(33, 4)
#define INS_COUNTER_IPORT_FRAME					INS_COUNTER(34, 4)
#define INS_COUNTER_CaptureTriggerCount			INS_COUNTER_TRIG
#define INS_COUNTER_AcquireTriggerCount			INS_COUNTER_IR_FRAME
#define INS_COUNTER_FrameCount					INS_COUNTER_IPORT_FRAME
#define INS_COUNTER_DataStreamCount				INS_COUNTER_AcquireTriggerCount

#define INS_NEW4CON_COUNTER_FIRST				(0x200)
#define INS_NEW4CON_COUNTER(n, size)            ((((n) + INS_NEW4CON_COUNTER_FIRST) << 16) | (size))

#define INS_NEW4CON_COUNTER_TRIG			    INS_NEW4CON_COUNTER(1, 4)
#define INS_NEW4CON_COUNTER_TRIGGER_LIMIT	    INS_NEW4CON_COUNTER(2, 4)
#define INS_NEW4CON_COUNTER_TRIGGER_CAP			INS_NEW4CON_COUNTER(3, 4)
#define INS_NEW4CON_COUNTER_SENSOR_DATA_1	    INS_NEW4CON_COUNTER(4, 4)
#define INS_NEW4CON_COUNTER_SENSOR_DATA_2	    INS_NEW4CON_COUNTER(5, 4)
#define INS_NEW4CON_COUNTER_SENSOR_DATA_3	    INS_NEW4CON_COUNTER(6, 4)
#define INS_NEW4CON_COUNTER_SENSOR_DATA_4	    INS_NEW4CON_COUNTER(7, 4)
#define INS_NEW4CON_COUNTER_SENSOR_FRAME_1	    INS_NEW4CON_COUNTER(8, 4)
#define INS_NEW4CON_COUNTER_SENSOR_FRAME_2	    INS_NEW4CON_COUNTER(9, 4)
#define INS_NEW4CON_COUNTER_SENSOR_FRAME_3	    INS_NEW4CON_COUNTER(10, 4)
#define INS_NEW4CON_COUNTER_SENSOR_FRAME_4	    INS_NEW4CON_COUNTER(11, 4)
#define INS_NEW4CON_COUNTER_DATA_CAP_1	        INS_NEW4CON_COUNTER(12, 4)
#define INS_NEW4CON_COUNTER_DATA_CAP_2	        INS_NEW4CON_COUNTER(13, 4)
#define INS_NEW4CON_COUNTER_DATA_CAP_3	        INS_NEW4CON_COUNTER(14, 4)
#define INS_NEW4CON_COUNTER_DATA_CAP_4	        INS_NEW4CON_COUNTER(15, 4)
#define INS_NEW4CON_COUNTER_INSERT_1            INS_NEW4CON_COUNTER(16, 4)
#define INS_NEW4CON_COUNTER_INSERT_2            INS_NEW4CON_COUNTER(17, 4)
#define INS_NEW4CON_COUNTER_INSERT_3            INS_NEW4CON_COUNTER(18, 4)
#define INS_NEW4CON_COUNTER_INSERT_4            INS_NEW4CON_COUNTER(19, 4)
#define INS_NEW4CON_COUNTER_ORDER_ADJ_1	        INS_NEW4CON_COUNTER(20, 4)
#define INS_NEW4CON_COUNTER_ORDER_ADJ_2	        INS_NEW4CON_COUNTER(21, 4)
#define INS_NEW4CON_COUNTER_ORDER_ADJ_3	        INS_NEW4CON_COUNTER(22, 4)
#define INS_NEW4CON_COUNTER_ORDER_ADJ_4	        INS_NEW4CON_COUNTER(23, 4)
#define INS_NEW4CON_COUNTER_8P_FRAME	        INS_NEW4CON_COUNTER(24, 4)
#define INS_NEW4CON_COUNTER_PC_FRAME	        INS_NEW4CON_COUNTER(25, 4)
#define INS_NEW4CON_COUNTER_HYBIRD_FRAME	    INS_NEW4CON_COUNTER(26, 4)
#define INS_NEW4CON_COUNTER_BPF_FRAME	        INS_NEW4CON_COUNTER(27, 4)
#define INS_NEW4CON_COUNTER_ROI_FRAME	        INS_NEW4CON_COUNTER(28, 4)
#define INS_NEW4CON_COUNTER_FLIP_FRAME	        INS_NEW4CON_COUNTER(29, 4)
#define INS_NEW4CON_COUNTER_IR_FRAME	        INS_NEW4CON_COUNTER(30, 4)
#define INS_NEW4CON_COUNTER_CaptureTriggerCount	INS_NEW4CON_COUNTER_TRIG
#define INS_NEW4CON_COUNTER_AcquireTriggerCount	INS_NEW4CON_COUNTER_IR_FRAME
// #define INS_NEW4CON_COUNTER_FrameCount		(INS_NEW4CON_COUNTER_IR_FRAME / currentImageHeght) // Unable to obtain directly, user needs to calculate
#define INS_NEW4CON_COUNTER_DataStreamCount		INS_NEW4CON_COUNTER_AcquireTriggerCount








/****************************************************************************
*
* 相机的编码器计数信息需使用函数"InsCHVS_Get_Encoder_Counter"获取
*
* The data flow statistics information of the Encoder needs to be obtained using the function "InsCHVS_Get_Encoder_Counter":
*
****************************************************************************/

#define INS_ENCODER_COUNTER_FIRST				(0)
#define INS_ENCODER_COUNTER(n, size)            ((((n) + INS_ENCODER_COUNTER_FIRST) << 16) | (size))

#define INS_ENCODER_COUNTER_LOCATION       	    INS_ENCODER_COUNTER(1, 4)
#define INS_ENCODER_COUNTER_LOCATION_FILTER	    INS_ENCODER_COUNTER(2, 4)
#define INS_ENCODER_COUNTER_PHASE_A				INS_ENCODER_COUNTER(3, 4)
#define INS_ENCODER_COUNTER_PHASE_B				INS_ENCODER_COUNTER(4, 4)
#define INS_ENCODER_COUNTER_PHASE_A_FILTER	    INS_ENCODER_COUNTER(5, 4)
#define INS_ENCODER_COUNTER_PHASE_B_FILTER	    INS_ENCODER_COUNTER(6, 4)


/****************************************************************************
*
* 相机的IO输入计数信息需使用函数"InsCHVS_Get_IOSignal_Counter"获取
*
* The data flow statistics information of the IO signal needs to be obtained using the function "InsCHVS_Get_IOSignal_Counter":
*
****************************************************************************/

#define INS_IOSIGNAL_COUNTER_FIRST				(0)
#define INS_IOSIGNAL_COUNTER(n, size)            ((((n) + INS_IOSIGNAL_COUNTER_FIRST) << 16) | (size))

#define INS_IOSIGNAL_COUNTER_EXTERNAL_INPIT_0       INS_IOSIGNAL_COUNTER(1, 4)
#define INS_IOSIGNAL_COUNTER_EXTERNAL_INPIT_1       INS_IOSIGNAL_COUNTER(2, 4)
#define INS_IOSIGNAL_COUNTER_USED_INTERNALLY	    INS_IOSIGNAL_COUNTER(3, 4)
