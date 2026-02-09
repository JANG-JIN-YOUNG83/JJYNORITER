#pragma once

#include "basedef.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ShowRect : public CommonShare::BaseObject
	{
	public:
		ShowRect();
		ShowRect(const IPVM::Rect& roi);
		~ShowRect();

		CRect ROI;
		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
	};
}
