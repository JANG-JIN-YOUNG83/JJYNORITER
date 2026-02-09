#include "stdafx.h"
#include "ParaBool.h"
#include "ParaCollector.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_changeParaOldValue;

using namespace Base;

ParaBool::ParaBool(LPCTSTR name, bool defaultValue)
	: m_name(name)
	, m_defaultValue(defaultValue)
	, m_value(defaultValue)
	, m_oldValue(defaultValue)
{
}

ParaBool::ParaBool(LPCTSTR name, BOOL defaultValue)
	: m_name(name)
	, m_defaultValue(defaultValue ? true : false)
	, m_value(defaultValue ? true : false)
	, m_oldValue(defaultValue ? true : false)
{
}
ParaBool::ParaBool(LPCTSTR name, bool defaultValue, DefectType Type, ...)
	: m_name(name)
	, m_defaultValue(defaultValue)
	, m_value(defaultValue)
	, m_oldValue(defaultValue)
{
	CString strFmt, strText;
	va_list argptr;

	va_start(argptr, Type);
	int arg = 0;
	for (int i = 0; i < (int)Type; i++)
	{
		arg = va_arg(argptr, int);
		m_RelationItem.push_back(arg);
	}
	va_end(argptr);
}

ParaBool::ParaBool(LPCTSTR name, BOOL defaultValue, DefectType Type, ...)
	: m_name(name)
	, m_defaultValue(defaultValue ? true : false)
	, m_value(defaultValue ? true : false)
	, m_oldValue(defaultValue ? true : false)
{
	CString strFmt, strText;
	va_list argptr;

	va_start(argptr, Type);
	int arg = 0;
	for (int i = 0; i < (int)Type; i++)
	{
		arg = va_arg(argptr, int);
		m_RelationItem.push_back(arg);
	}
	va_end(argptr);
}

ParaBool::~ParaBool()
{
}

ParaBool& ParaBool::operator = (const bool& value)
{
	m_value = value;
	if (g_changeParaOldValue) m_oldValue = m_value;

	return *this;
}

ParaBool& ParaBool::operator = (const BOOL& value)
{
	m_value = value ? true : false;
	if (g_changeParaOldValue) m_oldValue = m_value;

	return *this;
}

std::vector <int> ParaBool::GetRelationItem() const
{
	return m_RelationItem;
}

void ParaBool::Reset()
{
	m_value = m_defaultValue;
	if (g_changeParaOldValue) m_oldValue = m_value;
}

bool ParaBool::Link(bool save, Database& db)
{
	bool success = false;

	if (!save && db.GetType() == 3 /* REGISTERDB_KEYTYPE_INT32 */)
	{
		// 과거 bool이 아닌 BOOL 변수로 저장하여서 INT로 저장되었던 변수다
		// Load하면서 bool값으로 치환해주자

		int oldValue = 0;
		db.Link(save, oldValue);
		m_value = oldValue ? true : false;
	}
	else
	{
		success = db.Link(save, m_value);
	}

	if (!success)
	{
		m_value = m_defaultValue;
	}

	ParaCollector_Add(m_name, m_oldValue != m_value,
		m_oldValue ? _T("TRUE") : _T("FALSE"),
		m_value ? _T("TRUE") : _T("FALSE"),  m_RelationItem);

	if (g_changeParaOldValue) m_oldValue = m_value;

	return success;
}

ParaBool::operator const bool &() const
{
	return m_value;
}

ParaBool::operator bool &()
{
	return m_value;
}
