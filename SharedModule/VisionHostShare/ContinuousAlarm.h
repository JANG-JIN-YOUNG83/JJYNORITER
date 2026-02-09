#pragma once

#include "basedef.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ContinuousAlarm : public CommonShare::BaseObject
	{
	public:
		ContinuousAlarm();
		~ContinuousAlarm();

		static LPCTSTR GetAlarmName(long alaramIndex);

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		//[0] 피치
		//[1] 어깨선 상부
		//[2] 어깨선 하부
		//[3] 탭(폭)
		//[4] 단차(전장)
		//[5] 표면불량
		//[6] 절연
		//[7] 오버레이

		int nAlarm[8];
	};
}
