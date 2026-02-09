#pragma once

#include "basedef.h"

namespace Share
{

	class __VISION_HOST_SHARE_CLASS__ Alarm : public CommonShare::BaseObject
	{
	public:
		Alarm();
		Alarm(int nCode, ALARM_LEVEL nLevel, CString strItem, CString strContext, CString strDetail, CString strTroubleShooting, SYSTEMTIME ToutbreakTime);
		~Alarm();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		BOOL		use;
		int	level;					//알람 등급 {0}경알람, {1}중알람
		int			code;					//알람 종류
		CString		item;					// 구분[네트워크], [디버깅]
		CString		context;				// 발생 내용
		CString		detail;					// 상세 설명
		CString		troubleShooting;		// 문제 해결방법
		SYSTEMTIME	outbreakTime;			// 알람 발생 시간
		CString		type;					// 1통신/2Data/3HW //221206
		CString		jsonCode;				// 1~14 //20231102


		//HOST 내부 변수
		int			nDebugImageSaveCount;    // 디버그 이미지 저장 횟수
	};
}
