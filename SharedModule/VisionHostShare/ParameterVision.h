#pragma once

#include "basedef.h"
#include "ParameterCamera.h"

#include <map>

namespace Share
{
	class __VISION_HOST_SHARE_CLASS__ ParameterVision : public CommonShare::BaseObject
	{
	public:
		ParameterVision();
		~ParameterVision();

		void Add(LPCTSTR cameraName, LPCTSTR moduleName, LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value, std::vector<int> ReleationItem);
		bool Save(LPCTSTR filePath);

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;

		CString m_reason;		// 변경사유
		CString m_modelName;	// 모델명
		bool m_isChanged;		// 변경여부

		std::map<CString, ParameterCamera> m_cameras;
		CString GetVisionValue(CString strposition, CString strmodule, CString strpara);
	};
}
