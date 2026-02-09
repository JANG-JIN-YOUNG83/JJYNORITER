#pragma once

#include "basedef.h"

namespace Inspection
{
	class __INSPECTOIN_CLASS__ UrgentVariable
	{
	public:
		UrgentVariable();
		~UrgentVariable();

		// 긴급 임시 변수
		float	m_compensateTabPitchOffset;			// 옵셋을 적용하되 Value가 Spec보다 커지면 빼고, 작아지면 더한다
		bool	m_useValueToCloseToTabPitchSpec;	// Up, Down, (Up + Down) / 2 중 Spec에 가까운 값 사용
		float	m_compensateCoatingOffset;			
	};
}
