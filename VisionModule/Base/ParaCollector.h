#pragma once
#include "basedef.h"
#include <map>
#include <memory>

namespace Base
{
	class __BASE_CLASS__ FUNCTION_PARACOLLECTOR
	{
	public:
		FUNCTION_PARACOLLECTOR(Share::ParameterVision* collector);
		~FUNCTION_PARACOLLECTOR();
	};

	class __BASE_CLASS__ FUNCTION_PARACOLLECTOR_CAMERA
	{
	public:
		FUNCTION_PARACOLLECTOR_CAMERA(LPCTSTR cameraName);
		~FUNCTION_PARACOLLECTOR_CAMERA();
	};

	class __BASE_CLASS__ FUNCTION_PARACOLLECTOR_MODULE
	{
	public:
		FUNCTION_PARACOLLECTOR_MODULE(LPCTSTR moduleName);
		~FUNCTION_PARACOLLECTOR_MODULE();
	};

	void __BASE_CLASS__ ParaCollector_Add(LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value, std::vector<int> ReleationItem);
	void __BASE_CLASS__ ParaCollector_UpdateOldPara(bool enable);
}
