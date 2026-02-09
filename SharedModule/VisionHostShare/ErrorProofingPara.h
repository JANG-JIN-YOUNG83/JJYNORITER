#pragma once
#include "basedef.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ErrorProofingPara : public CommonShare::BaseObject
	{
	public:
		ErrorProofingPara();
		~ErrorProofingPara();

		void CreateGUID();
		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		CString	m_guid;	// ∞Ì¿ØID
		long	m_proofingCellCount;
	};
}
