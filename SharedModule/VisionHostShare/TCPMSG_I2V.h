#pragma once

enum class TCPMSG_I2V	// Interface -> Vision
{
	LOT_INFO = 200,
	REPLY_VISION_ALARM = 410,
	ALARM_RESET = 450,								// 건식 사용 (구현 완료)
	VISION_ALIVE = 500,
	SPEC_ORIGINAL_DATA = 600, // 안쓰는듯?..
	SPEC_CURRENT_DATA = 800,						// 건식 사용 (구현 완료)
	SPEC_NGMARK_INFO = 850,
	LOT_CHANGE = 900,

	NEW_RECEIPE = 1000,
	RECEIPE_CHANGE = 1100,							// 건식 사용 (구현 완료)
	SPEC_CHANGE,
	SHOW_PGM = 1200,
	DATA_RESET = 1300,								// 건식 사용 (구현 완료)
	NEXT_STEP_IO_NUM = 1400,
	MARKING_ON_OFF = 1800,
	CONTINUES_ALARM = 1900,
	BAR_TYPE_CAMERA_INIT = 2000,					// 건식 사용 (구현 완료)
	EXTRA_SPEC_DATA = 2100,
	TEST_GRAB_START = 2200,
	TEST_SURFACE_START = 2210,
	AREA_AUTO_OFFSET = 2300,

	VISION_RESET = 2400,			// Inline 상태면 Grab을 Stop시킨후 Start 시키는 명령 (Inline 상태가 아니면 아무일도 안함)	// 건식 사용  (구현 완료)
	VISION_ERROR_PROOFING = 2500,		// Vision에게 Cell에 대한 Error Proofing을 요청한다
	
	SYNCHRONIZE_TIME = 2600,						// 건식 사용  (구현 완료)

	LOGIN_RESULT = 4000,		//4000번대 login 관련 메시지
	LOGIN_INIT = 4100,
	LOGIN_ACCESS_SEND = 4200,
	REQEUST_VISION_INI = 4500,

	REQEUST_PROGRAM_BACKUP = 5000,
	SIMULATION_INSPECT = 6000,
	REQEUST_JSONDATA = 7000,

	SEND_SHAREDIMAGEQUEUE_DATA = 8000,
	SEND_SAMEIOCHECK = 8100,
};
