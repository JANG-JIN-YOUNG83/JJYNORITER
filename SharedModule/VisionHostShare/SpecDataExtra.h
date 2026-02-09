#pragma once

#include "basedef.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ SpecDataExtra : public CommonShare::BaseObject
	{
	public:
		SpecDataExtra();
		~SpecDataExtra();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		double m_dTabPitch;
		double m_dTabWidth;
		double m_dTabSide;
		long m_nSensingDistance;
		long m_nCIS1TrgPos;
	};
}
