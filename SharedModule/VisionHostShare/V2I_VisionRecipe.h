#pragma once
#include "basedef.h"
#include "../../CompanySharedLibrary/Persistence/Database.h"

namespace TCPMSG
{
	namespace V2I
	{
		class __VISION_HOST_SHARE_CLASS__ VisionRecipe : public CommonShare::BaseObject
		{
		public:
			VisionRecipe();
			~VisionRecipe();

			virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

			CString m_recipeName;	// 레시피명
			Database m_db;			// 레시피데이터
		};
	}
}
