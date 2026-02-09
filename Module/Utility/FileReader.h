#pragma once
#include "basedef.h"
#include <vector>

namespace Utility
{
	class __UTILITY_CLASS__ FileReader
	{
	public:
		static bool Load(LPCTSTR filePath, std::vector<BYTE>& o_data);
	};
}
