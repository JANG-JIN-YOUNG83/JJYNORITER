#pragma once
#include "basedef.h"

namespace TCPMSG
{
	namespace I2V
	{
		class __VISION_HOST_SHARE_CLASS__ SimulationInspect : public CommonShare::BaseObject
		{
		public:
			SimulationInspect();
			~SimulationInspect();

			virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

			long m_cellID;	// 검사대상의 Cell ID
			// 로드해서 계산할 CIS 이미지 경로
			CString m_cisUpImagePath;		// CIS1,2가 로드할 Up용 Image 경로
			CString m_cisDownImagePath;		// CIS1,2가 로드할 Down용 Image 경로
			CString m_areaImagePath;		// Area가 로드할 Image 경로
		};
	}
}
