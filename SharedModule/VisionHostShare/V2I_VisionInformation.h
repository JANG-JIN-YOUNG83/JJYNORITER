#pragma once
#include "basedef.h"

namespace TCPMSG
{
	namespace V2I
	{
		class __VISION_HOST_SHARE_CLASS__ VisionInformation : public CommonShare::BaseObject
		{
		public:
			VisionInformation();
			~VisionInformation();

			virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

			CString m_version;
		};
	}
}
