#pragma once

#include "basedef.h"
#include "Parameter.h"

#include <map>
#include <vector>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ParameterModule : public CommonShare::BaseObject
	{
	public:
		ParameterModule();
		~ParameterModule();

		void Add(LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value, std::vector<int> ReleationItem );

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		bool m_isChanged;		// 변경여부
		std::map<CString, Parameter> m_parameters;
		CString GetParaValue(CString strpara);
	};
}
