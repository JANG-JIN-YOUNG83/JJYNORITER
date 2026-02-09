#pragma once
#include "basedef.h"
#include <map>
#include <memory>

namespace Utility
{
	class __UTILITY_CLASS__ IniKey
	{
	public:
		CString m_keyName;
		CString m_value;
	};

	class __UTILITY_CLASS__ IniCategory
	{
	public:
		LPCTSTR GetKeyValue(LPCTSTR keyName, LPCTSTR defaultValue = _T("")) const;
		std::shared_ptr<IniKey> GetKey(LPCTSTR keyName);

		void SetKeyValue(LPCTSTR keyName, int value);
		void SetKeyValue(LPCTSTR keyName, unsigned int value);

		CString m_categoryName;
		std::map<CString, std::shared_ptr<IniKey>> m_mapKey;
	};

	class __UTILITY_CLASS__ IniPackage
	{
	public:
		IniPackage();
		~IniPackage();

		bool Load(LPCTSTR filePath);
		bool Save(LPCTSTR filePath);

		std::shared_ptr<IniCategory> GetCategory(LPCTSTR categoryName);

	private:
		std::map<CString, std::shared_ptr<IniCategory>> m_mapCategory;
	};
}
