#pragma once

// TRIGGER BOARD PARAMETER(2021.06.01)
enum enumTriggerBoardParameter
{
    /*													Description									Address(Read, Write)			*/
    /*												---------------------------------------------------------------------------		*/
    _TB01_EncoderPulse, /*	1mm당 엔코더 펄스 수							(,0x9002)			[pulse]		*/
    _TB02_SensorMinDistance, /*	센서 최소 감지 거리								(0x4000,0x9000)		[mm]		*/
    _TB03_VirtualTrigger, /*	가상 트리거 발생 거리(앞셀기준)					(0x4001,0x9001)		[mm]		*/
    _TB04_V1_Trigger, /*	Vision#1 TRIGGER 발생 위치						(0x4010,0x9010)		[mm]		*/
    _TB05_V1_CellID, /*	Vision#1 Cell ID 발생 위치						(0x4212,0x9012)		[mm]		*/
    _TB06_V1_TriggerOnTime, /*	Vision#1 TRIGGER ON TIME						(,0x9011)			[us]		*/
    _TB07_V1_AlarmPosition, /*	Vision#1 검사 결과 미응답 확인 거리				(0x4013,0x9013)		[mm]		*/
    /*	Vision#1 검사 결과 알람 발생 위치												*/
    _TB08_V2_Trigger, /*	Vision#2 TRIGGER 발생 위치						(0x4020,0x9020)		[mm]		*/
    _TB09_V2_CellID, /*	Vision#2 Cell ID 발생 위치						(0x4022,0x9022)		[mm]		*/
    _TB10_V2_TriggerOnTime, /*	Vision#2 TRIGGER ON TIME						(,0x9021)			[us]		*/
    _TB11_V2_AlarmPosition, /*	Vision#2 검사 결과 미응답 확인 거리				(0x4023,0x9023)		[mm]		*/
    /*	Vision#2 검사 결과 알람 발생 위치																							*/
    _TB12_V2_Trigger2, /*	Vision#2 Trigger 2 발생 위치					(0x4120,0x9120)		[mm]		*/
    _TB13_V2_Trigger3, /*	Vision#2 Trigger 3 발생 위치					(0x4220,0x9220)		[mm]		*/
    _TB14_Line_TriggerPosition, /*	LINE TRIGGER 발생 시작 위치						(0x5010,0xA010)		[pulse]		*/
    _TB15_Line_TriggerRatio, /*	LINE TRIGGER 분주비								(0x5011,0xA011)		[pulse]		*/
    _TB16_Line_TriggerOnTime, /*	LINE TRIGGER ON TIME							(0x5012,0xA012)		[us]		*/
    _TB17_Line_TriggerCount, /*	LINE TRIGGER 개수(0=무한개)						(0x5013,0xA013)		[0/1]		*/
    _TB18_Line_TriggerEnable, /*	LINE TRIGGER ENABLE 유무(0=DISABLE,1=ENABLE)	(,0xA014)			[0/1]		*/
    _TB19_Line_TriggerReset, /*	LINE TRIGGER 카운트 리셋(0->1->0)				(,0xA000)			[0/1]		*/
    _TB20_Marking_TopTrigger, /*	TOP_INK MARKING TRIGGER 발생 위치				(0x4080,0x9080)		[mm]		*/
    _TB21_Marking_BottomTrigger, /*	BOTTOM_INK MARKING TRIGGER 발생 위치			(0x4081,0x9081)		[mm]		*/
    _TB22_Marking_Length, /*	INK MARKING LENGTH								(0x4180,0x9180)		[mm]		*/
    _TB22_Marking_Length_LONG, /*  INK MARKING LENGTH (노칭 불량, LONG TYPE)		(0x4280,0x9280)		[mm]		*/
    _TB23_Marking_TopSmartVisionTrigger, /*	TOP_SMART VISION TRIGGER POSITION				(0x4090,0x9090)		[mm]		*/
    _TB24_Marking_BottomSmartVisionTrigger, /*	BOTTOM_SMART VISION TRIGGER POSITION			(0x4092,0x9092)		[mm]		*/
    _TB25_Marking_SmartVisionTriggerOnTime, /*	SMART VISION TRIGGER ON TIME					(0x40A1,0x90A1)		[us]		*/ //20231031 [jjh] Read 항목 800 버전부터 적용
    _TB00_Marking_SmartVisionSignalWaitingTime, /*	SMARTVISION SIGNAL WAITING TIME					(0x40A2,0x90A2)		[us]		*/ //20231031 [jjh] Read 항목 800 버전부터 적용
    _TB26_Manual_RepeatCycle, /*	초당 반복 주기(최소값1)							(,0x9008)			[count]		*/
    _TB27_Manual_RepeatCount, /*	반복 횟수(0=무한)								(,0x9007)			[count]		*/
    _TB28_Manual_TriggerReset, /*	메뉴얼 트리거 카운트 리셋(0->1->0)				(,0x8000)			[0/1]		*/
    _TB29_Manual_TriggerOn, /*	메뉴얼 트리거 동작(1=ON,0=OFF)					(,0x9108)			[0/1]		*/
    _TB30_Trigger_Abnormal_Filtering_Period, /*	트리거 신호 이상 동작 방지를 위한 필터링 주기	(0x4004,0x9004)		[mm]		*/ //20220823 [jjh] 트리거 필터링 주기 항목 추가
    _TB31_TapeDetection_TabPitch, /*	테이프 감지 관련 파라미터(탭 피치)				(0x0100,0x8100)		[mm]		*/ //20220901 [jjh] 테이프감지파라미터 추가 start
    _TB32_TapeDetection_TopInkMarkingPos, /*	테이프 감지 관련 파라미터(상부 잉크 마킹 위치)	(0x0101,0x8101)		[mm]		*/
    _TB33_TapeDetection_BottomInkMarkingPos, /*	테이프 감지 관련 파라미터(하부 잉크 마킹 위치)	(0x0201,0x8201)		[mm]		*/
    _TB34_TapeDetection_TopSmartVisionPos, /*	테이프 감지 관련 파라미터(상부 스마트 비젼 위치)(0x0102,0x8102)		[mm]		*/
    _TB35_TapeDetection_BottomSmartVisionPos, /*	테이프 감지 관련 파라미터(하부 스마트 비젼 위치)(0x0202,0x8202)		[mm]		*/
    _TB36_TapeDetection_InkMarkingCount, /*	테이프 감지 관련 파라미터(잉크 마킹 수량)		(0x0103,0x8103)		[count]		*/ //20220901 [jjh] 테이프감지파라미터 추가 end
    _TB37_Marking_Length_Time, /*  INK MARKING LENGTH TIME                         (,0x9380)			[ms]        */ //20221207 [jjh]
    _TB49_Marking_UseTimeMethod, /*  INK MARKING UseTimeMethod                       (,0x9480)			[0/1]       */ //20221212
    /*	BOARD 내부 RESET(셀ID 0부터 시작하고 잉크마킹									*/
    /*	에 반영되지 않은 내부 결과는 모두 RESET)										*/
    /*	DATA : 0->1->0 으로 해야 리셋됨.												*/
    _TB39_Dross_Vision_Trigger, /*	DROSS VISION TRIGGER 발생 위치					(0x4030,0x9030)		[mm]		*/
    _TB40_Dross_Vision_CellID, /*	DROSS VISION Cell ID 발생 위치					(0x4032,0x9032)		[mm]		*/
    _TB41_Dross_Vision_TriggerOnTime, /*	DROSS VISION TRIGGER ON TIME					(,0x9031)			[us]		*/
    _TB42_Dross_Vision_AlarmPosition, /*	DROSS VISION 검사 결과 미응답 확인 거리			(0x4033,0x9033)		[mm]		*/
    /*	DROSS VISION 검사 결과 알람 발생 위치											*/
    _TB43_Dross_Line_TriggerPosition, /*	DROSS LINE TRIGGER 발생 시작 위치				(0x7010,0xC010)		[Pulse]		*/
    _TB44_Dross_Line_TriggerRatio, /*	DROSS LINE TRIGGER 분주비						(0x7011,0xC011)		[Pulse]		*/
    _TB45_Dross_Line_TriggerOnTime, /*	DROSS LINE TRIGGER ON TIME						(0x7012,0xC012)		[us]		*/
    _TB46_Dross_Line_TriggerCount, /*	DROSS LINE TRIGGER 개수 (0 = 무한개)			(0x7013,0xC013)		[0/1]		*/
    _TB47_Dross_Line_TriggerEnable, /*	DROSS LINE TRIGGER ENABLE 유무(0:Dis, 1:En)		(,0xC014)			[0/1]		*/
    _TB48_Dross_Line_TriggerReset, /*	DROSS LINE TRIGGER 카운트 리셋(0->1->0)			(,0xC000)			[0/1]		*/

    _TBTopContinuousInkMarkingOffset, /*상부연속잉크마킹오프셋							(0x0290,0xC290)                 */ //20231031 [jjh] 800 버전부터 적용
    _TBBottomContinuousInkMarkingOffset, /*하부연속잉크마킹오프셋							(0x0291,0xC291)                 */ //20231031 [jjh] 800 버전부터 적용
    _TBTopContinuousSmartTriggerOffset, /*상부연속스마트트리거오프셋						(0x0292,0xC292)                 */ //20231031 [jjh] 800 버전부터 적용
    _TBBottomContinuousSmartTriggerOffset, /*하부연속스마트트리거오프셋						(0x0293,0xC293)                 */ //20231031 [jjh]	800 버전부터 적용
    _TBResetTriggerCount, /*리셋트리거수량 전원투입orBCDID리셋 후 설정된 수량만큼 강제 잉크마킹 진행 (0x0294,0xC294)*/ //20231031 [jjh] 800 버전부터 적용

    _TB_DataEnd,
    _TB37_Etc_Version, /*	버전											(0xFFFF,)						*/
    _TB38_Etc_BoardReset, /*	전극 파단시 트리거 보드 내부 검사 결과 RESET	(,0xBCD0)			[0/1]		*/

    _TBLotChangeReset, /*  LotChangeReset 후 카운터 값을 레지스터에 저장   (,0xBCD1)                       */ //20231031 [jjh] 800 버전부터 적용

    _TB58_CIS_Dimension_Defect_Count, /*  Vision1(CIS비젼) 치수 불량 카운트               (0x0011,)						*/
    _TB59_CIS_Surface_Defect_Count, /*  Vision1(CIS비젼) 표면 불량 카운트               (0x0012,)						*/
    _TB60_AREA_Dimension_Defect_Count, /*  Vision2(치수비젼) 치수 불량 카운트              (0x0013,)						*/
    _TB61_AREA_Surface_Defect_Count, /*  Vision2(치수비젼) 표면 불량 카운트              (0x0014,)						*/
    _TB89_FOIL_Dimension_Defect_Count, /*  Vision3(FOIL비젼) 치수 불량 카운트              (0x0015,)						*/ //20231024 [jjh]
    _TB90_FOIL_Surface_Defect_Count, /*  Vision3(FOIL비젼) 표면 불량 카운트              (0x0016,)						*/ //20231024 [jjh]
    _TB62_Up_Ink_Mark_Trigger_Count, /*  상부 잉크 마킹 트리거 카운터                    (0x0021,)						*/
    _TB63_Up_Dimension_Defect_Count, /*  상부 치수 불량 카운터                           (0x0022,)						*/
    _TB64_Up_Surface_Defect_Count, /*  상부 표면 불량 카운터                           (0x0023,)						*/
    _TB65_Down_Ink_Mark_Trigger_Count, /*  하부 잉크 마킹 트리거 카운터                    (0x0024,)						*/
    _TB66_Down_Dimension_Defect_Count, /*  하부 치수 불량 카운터                           (0x0025,)						*/
    _TB67_Down_Surface_Defect_Count, /*  하부 표면 불량 카운터                           (0x0026,)						*/
    _TB68_Up_Smart_Vision_Defect_COunt, /*  상부 스마트 비젼 불량 검사 카운터               (0x0031,)						*/
    _TB69_Up_Smart_Vision_NG_COunt, /*  상부 스마트 비젼 검사 결과 NG 카운터            (0x0032,)						*/
    _TB70_Down_Smart_Vision_Defect_COunt, /*  하부 스마트 비젼 불량 검사 카운터               (0x0033,)						*/
    _TB71_Down_Smart_Vision_NG_COunt, /*  하부 스마트 비젼 검사 결과 NG 카운터            (0x0034,)						*/
    _TB72_CIS_nonresponse_Ink_Mark_Count, /*  Vision 1(CIS비젼)미응답 잉크 마킹 카운터        (0x0041,)						*/
    _TB73_AREA_nonresponse_Ink_Mark_Count, /*  Vision 2(치수비젼)미응답 잉크 마킹 카운터       (0x0042,)						*/
    _TB91_FOIL_nonresponse_Ink_Mark_Count, /*  Vision3(FOIL비젼) 미응답 잉크 마킹 카운터       (0x0043,)						*/ //20231024 [jjh]
    _TB73_All_Count_Reset, /*                                                  (0x0080,)						*/ //확인필요
    _TB74_All_Count_Set_0, /*	모든 카운터 '0' 으로 초기화(0->1->0)			(,0x0080)						*/
    _TB75_Trigger2CIS_id_Print_Count, /*  트리거보드에서 vision1(CIS비젼) 셀ID 발행 개수  (0x0051,)						*/
    _TB76_CIS2Trigger_Data_Send_Count, /*  Vision1(CIS 비젼) 에서 검사결과 전송한 개수     (0x0052,)						*/
    _TB77_Trigger2CIS_id_Print_Count, /*  트리거보드에서 vision2(치수비젼) 셀ID 발행 개수 (0x0053,)						*/
    _TB78_CIS2Trigger_Data_Send_Count, /*  Vision2(치수비젼) 에서 검사결과 전송한 개수     (0x0054,)						*/
    _TB92_Trigger2CIS_id_Print_Count, /*  트리거보드에서 vision3(FOIL비젼) 셀ID 발행 개수 (0x0055,)						*/ //20231024 [jjh]
    _TB93_CIS2Trigger_Data_Send_Count, /*  Vision3(FOIL비젼) 에서 검사결과 전송한 개수     (0x0056,)						*/ //20231024 [jjh]
    _TB79_EEPROM_WRITE_0_1_0, /*                                                  (,0xEEEE)						*/
    _TB80_ENABLE_INK_MARKING, /*                                                  (,0x9085)						*/
    _TB81_DARK_ON_COUNTER1, /*  DarkOnCounter1									(0xD000,)						*/ //2023.06.13 Add TrgVer704
    _TB82_DARK_ON_COUNTER2, /*  DarkOnCounter2									(0xD001,)						*/ //2023.06.13 Add TrgVer704
    _TB83_DARK_ON_COUNTER3, /*  DarkOnCounter3									(0xD002,)						*/ //2023.06.13 Add TrgVer704
    _TB84_DARK_ON_COUNTER4, /*  DarkOnCounter4									(0xD003,)						*/ //2023.06.13 Add TrgVer704
    _TB85_BLANK_ON_COUNTER1, /*  BlankOnCounter1									(0xB000,)						*/ //2023.06.13 Add TrgVer704
    _TB86_BLANK_ON_COUNTER2, /*  BlankOnCounter2									(0xB001,)						*/ //2023.06.13 Add TrgVer704
    _TB87_BLANK_ON_COUNTER3, /*  BlankOnCounter3									(0xB002,)						*/ //2023.06.13 Add TrgVer704
    _TB88_BLANK_ON_COUNTER4, /*  BlankOnCounter4									(0xB003,)						*/ //2023.06.13 Add TrgVer704
    _TabSensorCount, /*  Tab 센서 감지 횟수								(0x3000,)						*/ //20240116 [jjh]
    _NoTabTriggerCount, /*  가상탭 트리거 발생 횟수							(0x3001,)						*/ //20240116 [jjh]
    _TB_CellID_TriggerBoard_To_V1, /*  셀아이디 (트리거보드->비전1)						(0xE001,)						*/ //20240227 [jjh] start
    _TB_CellID_V1_To_TriggerBoard, /*  셀아이디 (비전1->트리거보드)						(0xE011,)						*/
    _TB_CellID_TriggerBoard_To_V2, /*  셀아이디 (트리거보드->비전2)						(0xE002,)						*/
    _TB_CellID_V2_To_TriggerBoard, /*  셀아이디 (비전2->트리거보드)						(0xE022,)						*/
    _TB_CellID_TriggerBoard_To_V3, /*  셀아이디 (트리거보드->비전3)						(0xE003,)						*/
    _TB_CellID_V3_To_TriggerBoard, /*  셀아이디 (비전3->트리거보드)						(0xE033,)						*/ //20240227 [jjh] end

    //20231031 [jjh] Read 항목 800 버전부터 적용 (카운터/레지스터) start
    _TBSensorTabCounter, //센서탭카운터 0x0008 / 0x0208 Sensor Min 값으로 정상 동작하는 탭 수량 읽기(Lot 체인지 후 카운터 저장)
    _TBSensorNoTabCounter, //센서노탭카운터 0x0009 / 0x0209 Sensor Max 값으로 정상 동작한 가상 탭 수량 읽기(Lot 체인지 후 카운터 저장)
    _TBV1_TriggerCounter, //V1_트리거카운터 0x0010 / 0x0210
    _TBV1_ResponsePulseCounter, //V1_응답펄스카운터 0x0011 / 0x0211
    _TBV1_NoResponseCounter, //V1_미응답카운터 0x0012 / 0x0212
    _TBV1_NGCounter, //V1_NG카운터 0x0013 / 0x0213
    _TBV1_3MarkingNGCounter, //V1_3마킹NG카운터 0x0014 / 0x0214
    _TBV1_0MarkingNGCounter, //V1_0마킹NG카운터 0x0015 / 0x0215
    _TBV2_TriggerCounter, //V2_트리거카운터 0x0020 / 0x0220
    _TBV2_ResponsePulseCounter, //V2_응답펄스카운터 0x0021 / 0x0221
    _TBV2_NoResponseCounter, //V2_미응답카운터 0x0022 / 0x0222
    _TBV2_NGCounter, //V2_NG카운터 0x0023 / 0x0223
    _TBV2_3MarkingNGCounter, //V2_3마킹NG카운터 0x0024 / 0x0224
    _TBV2_0MarkingNGCounter, //V2_0마킹NG카운터 0x0025 / 0x0225
    _TBV3_TriggerCounter, //V3_트리거카운터 0x0030 / 0x0230
    _TBV3_ResponsePulseCounter, //V3_응답펄스카운터 0x0031 / 0x0231
    _TBV3_NoResponseCounter, //V3_미응답카운터 0x0032 / 0x0232
    _TBV3_NGCounter, //V3_NG카운터 0x0033 / 0x0233
    _TBV3_3MarkingNGCounter, //V3_3마킹NG카운터 0x0034 / 0x0234
    _TBV3_0MarkingNGCounter, //V3_0마킹NG카운터 0x0035 / 0x0235
    _TBTopV1InkCounter, //상부V1잉크카운터 0x0050 / 0x0250
    _TBTopV2InkCounter, //상부V2잉크카운터 0x0051 / 0x0251
    _TBTopV3InkCounter, //상부V3잉크카운터 0x0052 / 0x0252
    _TBTopV1V2InkCounter, //상부V1V2잉크카운터 0x0053 / 0x0253
    _TBTopV1V3InkCounter, //상부V1V3잉크카운터 0x0054 / 0x0254
    _TBTopV2V3InkCounter, //상부V2V3잉크카운터 0x0055 / 0x0255
    _TBTopV1V2V3InkCounter, //상부V1V2V3잉크카운터 0x0056 / 0x0256
    _TBTopNGTagInkMarkingCounter, //상부NGTAG잉크마킹카운터 0x0057 / 0x0257
    _TBTopTriggerResetInkMarkingCounter, //상부트리거리셋잉크마킹카운터 0x0058 / 0x0258 트리거 리셋 후 설정된 수량만큼 연속으로 잉크 마킹
    _TBTopVisionInkMarkingTriggerCounter, //상부비전잉크마킹트리거카운터 0x0059 / 0x0259
    _TBBottomV1InkCounter, //하부V1잉크카운터 0x0060 / 0x0260
    _TBBottomV2InkCounter, //하부V2잉크카운터 0x0061 / 0x0261
    _TBBottomV3InkCounter, //하부V3잉크카운터 0x0062 / 0x0262
    _TBBottomV1V2InkCounter, //하부V1V2잉크카운터 0x0063 / 0x0263
    _TBBottomV1V3InkCounter, //하부V1V3잉크카운터 0x0064 / 0x0264
    _TBBottomV2V3InkCounter, //하부V2V3잉크카운터 0x0065 / 0x0265
    _TBBottomV1V2V3InkCounter, //하부V1V2V3잉크카운터 0x0066 / 0x0266
    _TBBottomNGTagInkMarkingCounter, //하부NGTAG잉크마킹카운터 0x0067 / 0x0267
    _TBBottomTriggerResetInkMarkingCounter, //하부트리거리셋잉크마킹카운터 0x0068 / 0x0268 트리거 리셋 후 설정된 수량만큼 연속으로 잉크 마킹
    _TBBottomVisionInkMarkingTriggerCounter, //하부비전잉크마킹트리거카운터 0x0069 / 0x0269
    _TBTopSmartTriggerCounter, //상부스마트트리거카운터 0x0070 / 0x0270
    _TBTopSmartMarkingInspectionCounter, //상부스마트마킹검사카운터 0x0071 / 0x0271
    _TBBottomSmartTriggerCounter, //하부스마트트리거카운터 0x0080 / 0x0280
    _TBBottomSmartMarkingInspectionCounter, //하부스마트마킹검사카운터 0x0081 / 0x0281

    _TBSensorTabRegister, //센서탭카운터 0x0008 / 0x0208 Sensor Min 값으로 정상 동작하는 탭 수량 읽기(Lot 체인지 후 카운터 저장)
    _TBSensorNoTabRegister, //센서노탭카운터 0x0009 / 0x0209 Sensor Max 값으로 정상 동작한 가상 탭 수량 읽기(Lot 체인지 후 카운터 저장)
    _TBV1_TriggerRegister, //V1_트리거카운터 0x0010 / 0x0210
    _TBV1_ResponsePulseRegister, //V1_응답펄스카운터 0x0011 / 0x0211
    _TBV1_NoResponseRegister, //V1_미응답카운터 0x0012 / 0x0212
    _TBV1_NGRegister, //V1_NG카운터 0x0013 / 0x0213
    _TBV1_3MarkingNGRegister, //V1_3마킹NG카운터 0x0014 / 0x0214
    _TBV1_0MarkingNGRegister, //V1_0마킹NG카운터 0x0015 / 0x0215
    _TBV2_TriggerRegister, //V2_트리거카운터 0x0020 / 0x0220
    _TBV2_ResponsePulseRegister, //V2_응답펄스카운터 0x0021 / 0x0221
    _TBV2_NoResponseRegister, //V2_미응답카운터 0x0022 / 0x0222
    _TBV2_NGRegister, //V2_NG카운터 0x0023 / 0x0223
    _TBV2_3MarkingNGRegister, //V2_3마킹NG카운터 0x0024 / 0x0224
    _TBV2_0MarkingNGRegister, //V2_0마킹NG카운터 0x0025 / 0x0225
    _TBV3_TriggerRegister, //V3_트리거카운터 0x0030 / 0x0230
    _TBV3_ResponsePulseRegister, //V3_응답펄스카운터 0x0031 / 0x0231
    _TBV3_NoResponseRegister, //V3_미응답카운터 0x0032 / 0x0232
    _TBV3_NGRegister, //V3_NG카운터 0x0033 / 0x0233
    _TBV3_3MarkingNGRegister, //V3_3마킹NG카운터 0x0034 / 0x0234
    _TBV3_0MarkingNGRegister, //V3_0마킹NG카운터 0x0035 / 0x0235
    _TBTopV1InkRegister, //상부V1잉크카운터 0x0050 / 0x0250
    _TBTopV2InkRegister, //상부V2잉크카운터 0x0051 / 0x0251
    _TBTopV3InkRegister, //상부V3잉크카운터 0x0052 / 0x0252
    _TBTopV1V2InkRegister, //상부V1V2잉크카운터 0x0053 / 0x0253
    _TBTopV1V3InkRegister, //상부V1V3잉크카운터 0x0054 / 0x0254
    _TBTopV2V3InkRegister, //상부V2V3잉크카운터 0x0055 / 0x0255
    _TBTopV1V2V3InkRegister, //상부V1V2V3잉크카운터 0x0056 / 0x0256
    _TBTopNGTagInkMarkingRegister, //상부NGTAG잉크마킹카운터 0x0057 / 0x0257
    _TBTopTriggerResetInkMarkingRegister, //상부트리거리셋잉크마킹카운터 0x0058 / 0x0258 트리거 리셋 후 설정된 수량만큼 연속으로 잉크 마킹
    _TBTopVisionInkMarkingTriggerRegister, //상부비전잉크마킹트리거카운터 0x0059 / 0x0259
    _TBBottomV1InkRegister, //하부V1잉크카운터 0x0060 / 0x0260
    _TBBottomV2InkRegister, //하부V2잉크카운터 0x0061 / 0x0261
    _TBBottomV3InkRegister, //하부V3잉크카운터 0x0062 / 0x0262
    _TBBottomV1V2InkRegister, //하부V1V2잉크카운터 0x0063 / 0x0263
    _TBBottomV1V3InkRegister, //하부V1V3잉크카운터 0x0064 / 0x0264
    _TBBottomV2V3InkRegister, //하부V2V3잉크카운터 0x0065 / 0x0265
    _TBBottomV1V2V3InkRegister, //하부V1V2V3잉크카운터 0x0066 / 0x0266
    _TBBottomNGTagInkMarkingRegister, //하부NGTAG잉크마킹카운터 0x0067 / 0x0267
    _TBBottomTriggerResetInkMarkingRegister, //하부트리거리셋잉크마킹카운터 0x0068 / 0x0268 트리거 리셋 후 설정된 수량만큼 연속으로 잉크 마킹
    _TBBottomVisionInkMarkingTriggerRegister, //하부비전잉크마킹트리거카운터 0x0069 / 0x0269
    _TBTopSmartTriggerRegister, //상부스마트트리거카운터 0x0070 / 0x0270
    _TBTopSmartMarkingInspectionRegister, //상부스마트마킹검사카운터 0x0071 / 0x0271
    _TBBottomSmartTriggerRegister, //하부스마트트리거카운터 0x0080 / 0x0280
    _TBBottomSmartMarkingInspectionRegister, //하부스마트마킹검사카운터 0x0081 / 0x0281
    //20231031 [jjh] Read 항목 800 버전부터 적용 (카운터/레지스터) end

    _TB_EncorderPulsePer1mm, // read 0x4002 // Encorder Pulse Per 1mm
    _TB_V1_CellIDPos, // read 0x4012, // V#1 CellID Pos
    _TB_V2_LTRGCycle, // read 0x5021, // V#2 LTRG Cycle

    _TB_Line_TriggerOnTime_2, //write 0xA021
};
