#pragma once

#include "basedef.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ Parameter : public CommonShare::BaseObject
	{
	public:
		Parameter();
		~Parameter();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		// 파라메터 정보
		bool m_isChanged;			// 파라메터가 변경되었는가
		CString m_previousValue;	// 이전값
		CString m_value;			// 현재값
		std::vector<int> m_ReleationItem;	// 관련 검사항목
	};
}
