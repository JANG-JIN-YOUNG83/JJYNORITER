#pragma once

#include "basedef.h"

namespace Share
{
	enum class MarkingResultType : long
	{
		Pass,
		Notching,
		Electrode,
	};

	class __VISION_HOST_SHARE_CLASS__ ResultMarkingIO : public CommonShare::BaseObject
	{
	public:
		ResultMarkingIO();
		~ResultMarkingIO();

		void Reset();
		void Apply(const Share::SpecData& specItem);
		void Apply(DefectType defectType, int section, int markingCount, int rank);
		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		MarkingResultType	m_resultType;
		int					m_markCount;
		int					m_rank;
		DefectType			m_masterDefectType;	// Marking 정보를 대표하는 불량명 (Pass = DefectType::END)
	};
}
