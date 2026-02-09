#include "pch.h"
#include "IniPackage.h"
#include "FileTextLines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Utility;

CString KEY(LPCTSTR value)
{
	CString upper(value);
	return upper.MakeUpper();
}

IniPackage::IniPackage()
{
}

IniPackage::~IniPackage()
{
}

bool IniPackage::Load(LPCTSTR filePath)
{
	FileTextLines textLines;
	if (!textLines.Load(filePath)) return false;

	m_mapCategory.clear();
	CString category;

	for (long rowIndex = 0; rowIndex < textLines.GetCount(); rowIndex++)
	{
		CString lineText = textLines.GetAt(rowIndex);
		lineText.Trim();

		if (lineText.Left(1) == _T("[") && lineText.Right(1) == _T("]"))
		{
			category = lineText.Mid(1, lineText.GetLength() - 2);
			continue;
		}

		long equalIndex = lineText.Find(_T("="));
		if (equalIndex < 0) continue;

		CString valueName = lineText.Left(equalIndex);
		CString value = lineText.Mid(equalIndex + 1);
		valueName.Trim();
		value.Trim();

		GetCategory(category)->GetKey(valueName)->m_value = value;
	}

	return true;
}

bool IniPackage::Save(LPCTSTR filePath)
{
	FileTextLines textLines;

	for (auto& category : m_mapCategory)
	{
		auto categoryItem = category.second;
		textLines.AddLine(_T("[") + categoryItem->m_categoryName + _T("]"));

		for (auto& key : categoryItem->m_mapKey)
		{
			auto keyItem = key.second;
			CString valueLine;
			valueLine.Format(_T("%s = %s"), (LPCTSTR)keyItem->m_keyName, (LPCTSTR)keyItem->m_value);
			textLines.AddLine(valueLine);
		}
	}

	return textLines.Save(filePath);
}

std::shared_ptr<IniCategory> IniPackage::GetCategory(LPCTSTR categoryName)
{
	CString key = KEY(categoryName);
	auto itCategory = m_mapCategory.find(key);
	if (itCategory != m_mapCategory.end())
	{
		return itCategory->second;
	}

	auto item = std::make_shared<IniCategory>();
	item->m_categoryName = categoryName;
	m_mapCategory[key] = item;

	return item;
}

LPCTSTR IniCategory::GetKeyValue(LPCTSTR keyName, LPCTSTR defaultValue) const
{
	CString key = KEY(keyName);
	auto itKey = m_mapKey.find(key);
	if (itKey != m_mapKey.end())
	{
		return itKey->second->m_value;
	}

	return defaultValue;
}

std::shared_ptr<IniKey> IniCategory::GetKey(LPCTSTR keyName)
{
	CString key = KEY(keyName);
	auto itKey = m_mapKey.find(key);
	if (itKey != m_mapKey.end())
	{
		return itKey->second;
	}

	auto item = std::make_shared<IniKey>();
	item->m_keyName = keyName;
	m_mapKey[key] = item;

	return item;
}

void IniCategory::SetKeyValue(LPCTSTR keyName, int value)
{
	GetKey(keyName)->m_value.Format(_T("%d"), value);
}

void IniCategory::SetKeyValue(LPCTSTR keyName, unsigned int value)
{
	GetKey(keyName)->m_value.Format(_T("%d"), value);
}
