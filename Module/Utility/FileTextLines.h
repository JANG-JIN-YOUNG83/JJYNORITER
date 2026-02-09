#pragma once
#include "basedef.h"
#include <vector>

namespace Utility
{
	class __UTILITY_CLASS__ FileTextLines
	{
	public:
		FileTextLines();
		~FileTextLines();

		long GetCount() const;
		LPCTSTR GetAt(long index) const;

		void AddLine(LPCTSTR text);
		void SubLines(long startIndex, long count, FileTextLines& o_result);

		bool Load(LPCTSTR filePath);
		bool Save(LPCTSTR filePath);

	private:
		std::vector<CString> m_lineTexts;
	};
}
