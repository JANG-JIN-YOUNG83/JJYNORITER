#pragma once
#include "basedef.h"

namespace Utility
{
	class __UTILITY_CLASS__ SystemFont
	{
	public:
		static bool CreateGUIFont(CFont& font, int pointSize);
		static bool CreateGUIFont_Bold(CFont& font, int pointSize);
	};
}
