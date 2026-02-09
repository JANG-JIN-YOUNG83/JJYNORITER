#pragma once
#include "basedef.h"
#include "Types/Rect.h"

namespace Base
{
	class __BASE_CLASS__ ParaRect : public IPVM::Rect
	{
	public:
		ParaRect(LPCTSTR name, const IPVM::Rect& defaultValue);
		ParaRect(LPCTSTR name, const IPVM::Rect& defaultValue, DefectType Type, ...);
		~ParaRect();

		ParaRect& operator = (const IPVM::Rect& value);

		std::vector<int> GetRelationItem() const;

		void Reset();
		bool Link(bool save, Database& db);

	private:
		CString m_name;
		IPVM::Rect m_oldValue;
		IPVM::Rect m_defaultValue;
		std::vector<int> m_RelationItem;
	};
}
