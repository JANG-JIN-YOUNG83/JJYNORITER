#pragma once
#include "../../VersionInfo.h"
#include "../CommonShare/BaseObject.h"

//---------------------------------------------------------------------------
// 다음 경로들은 현재 Fix 되어 있다
// 해당 경로를 바꾸면 Host의 AutoCleaner에서도 해당 하드코딩되어 있는 경로를
// 지정해 이미지를 자동으로 지워준다

#define AREA_FOLDER_ROOT_INLINE_IMAGE	_T("e:\\DAT\\IMG\\INSP")
#define AREA_FOLDER_ROOT_LOG			_T("C:\\intekplus\\IBNI_Area\\Log")

//---------------------------------------------------------------------------

enum class DefectType;

namespace Share
{
	enum enCAMERA
	{
		CAMERA_UP = 0,
		CAMERA_DOWN = 1,
	};

	enum enImgSec
	{
		ImgSec_BRIGHT = 0,
		ImgSec_DARK = 1,
	};

	enum ALARM_LEVEL
	{
		yellow = 0,
		red = 1,		
	};

	enum WA_ALARM_TEXT //20230316 [jjh] start
	{
		Normal_Alarm = 1,
		Vision_PC_CIS1_Communication_Error_Alarm = 2, //C0 Alarm
		Vision_PC_CIS2_Communication_Error_Alarm = 3, //C0 Alarm
		Vision_PC_Area_Communication_Error_Alarm = 4, //C0 Alarm
		Vision_PC_CIS1_Not_Ready_Alarm = 5,			  //C9 Alarm
		Vision_PC_CIS2_Not_Ready_Alarm = 6,			  //C9 Alarm
		Vision_PC_Area_Not_Ready_Alarm = 7,			  //C9 Alarm
		Vision_Cell_ID_Mismatch_Alarm = 8,            //C11 Alarm
		Vision_Encoder_Alarm = 9,                     //C120 Alarm
		Vision_Buffer_overload_Alarm = 10,            //C100,C179,C24 Alarm
		Vision_Trigger_Board_RS232_Alarm = 11,        //C18 Alarm
		C_Drive_Vision_Disk_Full_Alarm = 12,          //C30 Alarm
		D_Drive_Vision_Disk_Full_Alarm = 13,          //C30 Alarm
		E_Drive_Vision_Disk_Full_Alarm = 14,          //C30 Alarm
		Cell_design_Alarm = 15,                       //C180 Alarm
		Essential_Inspection_Item_Alarm = 16,         //C29 Alarm
		START_STOP_Signal_mismatch_Vision_PLC = 17,   //C108 Alarm
		Vision_Recipe_Alarm = 18,                     //C2,C4,C22,C900 Alarm
		Essential_Ink_Item_Alarm = 19,                //C28 Alarm //202304
	}; //20230316 [jjh] end

	enum enVDstate
	{
		EMPTY = 0,
		OCCUPY = 1,
		FULL = 2,
		POP = 3,
		WAIT_DEL = 4,		//삭제 하기 전에 old 파일을 보여줘야 해서 대기한다.
	};

	enum class VisionStatusType : long
	{
		VisionRunEnable,
	};

	enum OPERATOR_MODE
	{
		OPERATOR_RUN_STOP = 0,
		OPERATOR_RUN_AUTO = 1,
		OPERATOR_LOT_STOP = 2,
		OPERATOR_LOT_START = 3,
	};

	enum class VisionResetReply : long
	{
		NotInlineMode,	// Inline 모드가 아니라 Reset에 대한 응답 거부
		Failed,			// Reset에 실패했다
		Success,		// 성공
	};

	enum enAngle
	{
		angle0 = 0,
		angle90,
		angle180,
		angle270
	};

	enum GenerationVersion
	{
		Original = 0,
		SplitterRemoved,

	};

	enum AREA_GRABMODE
	{
		None = 0,
		OneShoot,
		TwoShoot,
		ThreeShoot,
	};
	class ParameterSystem;
	class ParameterVision;
	class ParameterCamera;
	class ParameterModule;
	class Parameter;

	class Alarm;
	class CameraInfo;
	class ContinuousAlarm;
	class ErrorProofingPara;
	class HostCommSocket;
	class ImageInfo;
	class OperatorInfo;
	class LoginInfo;
	class ResultDefect;
	class ResultDefectROI;
	class ResultDimension;
	class ResultMarkingIO;
	class ResultIO;
	class ResultVision;
	class ShowList;
	class ShowPoint;
	class ShowRect;
	class SpecData;
	class SpecDataExtra;
	class SpecDataVision;
};

namespace TCPMSG
{
	namespace I2V
	{
		class RequestProgramBackup;
		class SimulationInspect;
	}

	namespace V2I
	{
		class ReplyProgramBackup;
		class VisionInformation;
		class VisionRecipe;
	}
}

#ifdef __VISION_HOST_SHARE_CLASS_EXPORT__
#define __VISION_HOST_SHARE_CLASS__	__declspec(dllexport)
#else
#define __VISION_HOST_SHARE_CLASS__	__declspec(dllimport)
#endif
