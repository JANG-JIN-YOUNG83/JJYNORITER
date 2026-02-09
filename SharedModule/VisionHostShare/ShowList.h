#pragma once

#include "basedef.h"
#include "ShowRect.h"
#include "ShowPoint.h"

#include <vector>

#include "Types/Rect.h"

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ShowList : public CommonShare::BaseObject
	{
	public:
		ShowList();
		~ShowList();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		void		AddPoint(const IPVM::Point_32f_C2& point);
		void		AddRoi(const IPVM::Rect& roi);
		void		Reset();
		IPVM::Rect	GetTotalRoi() const;

		std::vector<ShowRect> vecROI;		// Area에서 3개 부터 보내는 roi는 탭 찢김 roi이다. 22.09.18 강줄기 책임 여기요.
		std::vector<ShowPoint> vecPoint;	//0~4개는 라인2개, 나머지는 진짜 point
	};
}
