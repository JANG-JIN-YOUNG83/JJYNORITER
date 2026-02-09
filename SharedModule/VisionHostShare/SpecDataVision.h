#pragma once

#include "basedef.h"
#include "SpecData.h"

#include <vector>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ SpecDataVision : public CommonShare::BaseObject
	{
	public:
		SpecDataVision();
		~SpecDataVision();

		void init();
		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		long FindSpecIndex(DefectType type) const;
		BOOL IsSpecEnabled(DefectType type);
		BOOL IsSpecEnabled(DefectType type, long& o_index) const;

		SpecData* FindSpec(DefectType type);

		//
		CString					m_strModel_ID;		// 모델 ID
		CString					m_strSpec_ID;		// Spec ID
		CString					m_strVisionType;	// vision Type
		std::vector<SpecData>	m_vecSpecDataList;	// Spec list

		//내부 사용변수
		BOOL bIsValidData;
	};
}
