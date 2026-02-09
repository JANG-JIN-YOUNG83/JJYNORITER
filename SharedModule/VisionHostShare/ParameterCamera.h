#pragma once

#include "basedef.h"
#include "ParameterModule.h"
#include <map>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ParameterCamera : public CommonShare::BaseObject
	{
	public:
		ParameterCamera();
		~ParameterCamera();

		void Add(LPCTSTR moduleName, LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value, std::vector<int> ReleationItem);

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		bool m_isChanged;		// 변경여부
		std::map<CString, ParameterModule> m_modules;
		CString GetParaModuleValue(CString strmodule, CString strpara);
	};
}
