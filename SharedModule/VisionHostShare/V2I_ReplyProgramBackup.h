#pragma once
#include "basedef.h"

namespace TCPMSG
{
	namespace V2I
	{
		class __VISION_HOST_SHARE_CLASS__ ReplyProgramBackup : public CommonShare::BaseObject
		{
		public:
			ReplyProgramBackup();
			~ReplyProgramBackup();

			virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

			CString m_titleName;
			bool m_success;
		};
	}
}
