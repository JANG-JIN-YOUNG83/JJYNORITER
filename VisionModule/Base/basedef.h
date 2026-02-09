#pragma once
#include "../../SharedModule/VisionHostShare/basedef.h"

namespace Base
{
	class Logger;

	class ParaCollector;
	class ParaCollector_Camera;

	class ParaBool;
	class ParaByte;
	class ParaDouble;
	class ParaFloat;
	class ParaLong;
	class ParaRect;
	class ParaString;
}

class Database;

#ifdef __BASE_CLASS_EXPORT__
#define __BASE_CLASS__ __declspec(dllexport)
#else
#define __BASE_CLASS__ __declspec(dllimport)
#endif
