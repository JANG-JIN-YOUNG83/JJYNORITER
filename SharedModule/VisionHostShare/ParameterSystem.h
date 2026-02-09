#pragma once

#include "basedef.h"
#include "Parameter.h"

#include <map>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ParameterSystem : public CommonShare::BaseObject
	{
	public:
		ParameterSystem();
		~ParameterSystem();

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		void Add(LPCTSTR paraName, LPCTSTR previousValue, LPCTSTR value);
		void Add(LPCTSTR paraName, double previousValue, double value);
		void Add(LPCTSTR paraName, float previousValue, float value);
		void Add(LPCTSTR paraName, int previousValue, int value);
		void Add(LPCTSTR paraName, long previousValue, long value);

		CString m_reason;		// 변경사유
		bool m_isChanged;		// 변경여부

		std::map<CString, Parameter> m_parameters;
	};
}
