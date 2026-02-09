#pragma once
#include "basedef.h"

namespace Base
{
	class __BASE_CLASS__ ParaFloat
	{
	public:
		ParaFloat(LPCTSTR name, float defaultValue);
		ParaFloat(LPCTSTR name, float defaultValue, DefectType Type, ...);

		~ParaFloat();

		float GetValue() const;

		ParaFloat& operator = (const float& value);

		std::vector<int> GetRelationItem() const;

		void Reset();
		bool Link(bool save, Database& db);

		operator const float &() const;
		operator float &();

		float GetValue()
		{
			return m_value;
		}

	private:
		CString m_name;
		float m_value;
		float m_oldValue;
		float m_defaultValue;
		std::vector<int> m_RelationItem;
	};
}
