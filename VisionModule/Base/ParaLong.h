#pragma once
#include "basedef.h"

namespace Base
{
	class __BASE_CLASS__ ParaLong
	{
	public:
		ParaLong(LPCTSTR name, long defaultValue);
		ParaLong(LPCTSTR name, long defaultValue, DefectType Type, ...);
		~ParaLong();

		ParaLong& operator = (const long value);

		bool operator == (const ParaLong& value) const;
		bool operator == (const int value) const;
		bool operator == (const long value) const;

		bool operator != (const ParaLong& value) const;
		bool operator != (const int value) const;
		bool operator != (const long value) const;

		CString GetText() const;
		std::vector<int> GetRelationItem() const;

		void Reset();
		bool Link(bool save, Database& db);

		operator const long &() const;
		operator long &();

		long GetValue()
		{
			return m_value;
		}

	private:
		CString m_name;
		long m_value;
		long m_oldValue;
		long m_defaultValue;
		std::vector<int> m_RelationItem;
	};
}
 