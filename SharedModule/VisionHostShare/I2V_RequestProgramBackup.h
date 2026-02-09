#pragma once
#include "basedef.h"

namespace TCPMSG
{
	namespace I2V
	{
		class __VISION_HOST_SHARE_CLASS__ RequestProgramBackup : public CommonShare::BaseObject
		{
		public:
			RequestProgramBackup();
			~RequestProgramBackup();

			virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

			CString m_titleName;	// Backup 이름 (폴더이름)
		};
	}
}
