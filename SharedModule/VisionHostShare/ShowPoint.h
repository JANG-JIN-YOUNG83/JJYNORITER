#pragma once

#include "basedef.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ShowPoint : public CommonShare::BaseObject
	{
	public:
		ShowPoint();
		ShowPoint(const IPVM::Point_32f_C2& point);
		~ShowPoint();

		CPoint Point;

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
	};
}
