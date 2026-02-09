#pragma once

enum class TCPMSG_V2I	// Vision -> Interface
{
	VISION_RESULT_DATA = 0,					// 건식 사용 
	VISION_IO = 100,
	REPLY_OPERATOR_INFO = 200,
	ALARM = 400,							// 건식 사용 
	REPLY_ALARM_RESET = 460,				// 건식 사용 
	SPEC_ORIGINAL_DATA = 600,
	VISION_ALIVE = 500,
	SPEC_CURRENT_DATA = 800,				// 건식 사용 
#ifndef _NOT_USED_HISTORY_MESSAGE_
	SPEC_HISTORY = 700,
#endif
	REPLY_MODEL_CHANGE = 1100,				// 건식 사용 
	REPLY_SPEC_CHANGE,						// 건식 사용 
	SHOW_PGM = 1200,
	REPLY_BAR_TYPE_CAMERA_INIT = 2000,		// 건식 사용 

	VISION_GRAB_READY = 2100,		// Grab Start될 때 Host에게 알려줌 (PC1만 해당 메시지 전송)
	VISION_GRAB_STOPPED = 2200,		// Grab Stop될 때 Host에게 알려줌 (PC1만 해당 메시지 전송)
	PARAMETER_VISION = 2300,		// 비전파라메터 정보			// 건식 사용 		
	PARAMETER_SYSTEM = 2400,		// 시스템파라메터 정보			// 건식 사용 

	VISION_SURFACE_TEST = 3000,    //AutoTest용 메시지

	LOGIN_TRY = 4000,				//4000번대 login 관련 메시지
	LOGIN_INIT_REQUEST = 4100,
	LOGIN_ACCESS_REQUEST = 4200,

	VISION_RESET_REPLY = 4300,				// 건식 사용 
	VISION_CRASH_DUMP = 4400,
	VISION_INI_FILE = 4500,

	REPLY_PROGRAM_BACKUP = 5000,

	VISION_INFORMATION = 6000,				// 건식 사용 
	VISION_STATUS = 6100,					// 건식 사용 
	VISION_RECIPE = 6200,					// 건식 사용 

	SEND_JSONDATA = 7000,

	VISION_NORMAL_TURN_OFF = 8000,	// 비전 프로그램을 '정상 종료'할 때, 호스트 0번 알람을 발생하지 않게 하는 신호	
											// 건식 사용 (구현완료)
};
