#pragma once
#include "basedef.h"

namespace Base
{
	class __BASE_CLASS__ ParaDouble
	{
	public:
		ParaDouble(LPCTSTR name, double defaultValue);
		ParaDouble(LPCTSTR name, double defaultValue, DefectType Type, ...);
		~ParaDouble();

		ParaDouble& operator = (const double& value);

		std::vector<int> GetRelationItem() const;
		CString GetText() const;
		void Reset();
		bool Link(bool save, Database& db);

		operator const double &() const;
		operator double &();

	private:
		CString m_name;
		double m_value;
		double m_oldValue;
		double m_defaultValue;
		std::vector<int> m_RelationItem;
	};
}
