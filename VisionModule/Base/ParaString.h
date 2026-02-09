#pragma once
#include "basedef.h"

namespace Base
{
	class __BASE_CLASS__ ParaString : public CString
	{
	public:
		ParaString(LPCTSTR name, LPCTSTR defaultValue);
		ParaString(LPCTSTR name, LPCTSTR defaultValue, DefectType Type, ...);
		~ParaString();

		ParaString& operator = (LPCTSTR value);

		std::vector<int> GetRelationItem() const;

		void Reset();
		bool Link(bool save, Database& db);

	private:
		CString m_name;
		CString m_oldValue;
		CString m_defaultValue;
		std::vector<int> m_RelationItem;
	};
}
