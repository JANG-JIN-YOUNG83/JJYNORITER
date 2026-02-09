#pragma once

#include "basedef.h"
#include "InspMerge.h"

class Database;

namespace Inspection
{
	class InspMergeOptions
	{
	public:
		InspMergeOptions();
		~InspMergeOptions();

		void LinkDataBase(bool bSave, Database& dbJob);

		InspMerge m_mergeLength;
		InspMerge m_mergeTopTilt;
		InspMerge m_mergeBottomTilt;
		InspMerge m_mergeMisalign;
	};
}
