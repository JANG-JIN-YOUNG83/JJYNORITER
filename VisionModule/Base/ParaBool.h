#pragma once
#include "basedef.h"

namespace Base
{
	class __BASE_CLASS__ ParaBool
	{
	public:
		ParaBool(LPCTSTR name, bool defaultValue);
		ParaBool(LPCTSTR name, BOOL defaultValue);
		ParaBool(LPCTSTR name, bool defaultValue, DefectType Type, ...);
		ParaBool(LPCTSTR name, BOOL defaultValue, DefectType Type, ...);
		~ParaBool();

		ParaBool& operator = (const bool& value);
		ParaBool& operator = (const BOOL& value);

		std::vector<int> GetRelationItem() const;

		void Reset();
		bool Link(bool save, Database& db);

		operator const bool &() const;
		operator bool &();

		bool GetValue()
		{
			return m_value;
		}

	private:
		CString m_name;
		bool m_value;
		bool m_oldValue;
		bool m_defaultValue;
		std::vector<int> m_RelationItem;
	};
}
