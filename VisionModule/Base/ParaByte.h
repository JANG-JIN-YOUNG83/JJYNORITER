#pragma once
#include "basedef.h"

namespace Base
{
	class __BASE_CLASS__ ParaByte
	{
	public:
		ParaByte(LPCTSTR name, BYTE defaultValue);
		ParaByte(LPCTSTR name, BYTE defaultValue, DefectType Type, ...);
		~ParaByte();

		ParaByte& operator = (const BYTE& value);

		std::vector<int> GetRelationItem() const;

		void Reset();
		bool Link(bool save, Database& db);

		operator const BYTE &() const;
		operator BYTE &();

	private:
		CString m_name;
		BYTE m_value;
		BYTE m_oldValue;
		BYTE m_defaultValue;
		std::vector<int> m_RelationItem;
	};
}
