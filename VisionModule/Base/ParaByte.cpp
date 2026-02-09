#include "stdafx.h"
#include "ParaByte.h"
#include "ParaCollector.h"
#include "StringFormat.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_changeParaOldValue;

using namespace Base;

ParaByte::ParaByte(LPCTSTR name, BYTE defaultValue)
	: m_name(name)
	, m_defaultValue(defaultValue)
	, m_value(defaultValue)
	, m_oldValue(defaultValue)
{
}

ParaByte::ParaByte(LPCTSTR name, BYTE defaultValue, DefectType Type, ...)
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
ParaByte::~ParaByte()
{
}

ParaByte& ParaByte::operator = (const BYTE& value)
{
	m_value = value ? true : false;
	if (g_changeParaOldValue) m_oldValue = m_value;

	return *this;
}

std::vector <int> ParaByte::GetRelationItem() const
{
	return m_RelationItem;
}

void ParaByte::Reset()
{
	m_value = m_defaultValue;
	if (g_changeParaOldValue) m_oldValue = m_value;
}

bool ParaByte::Link(bool save, Database& db)
{
	bool success = db.Link(save, m_value);
	if (!success)
	{
		m_value = m_defaultValue;
	}

	ParaCollector_Add(m_name, m_oldValue != m_value, StringFormat(_T("%d"), m_oldValue), StringFormat(_T("%d"), m_value), m_RelationItem);
	if (g_changeParaOldValue) m_oldValue = m_value;

	return success;
}

ParaByte::operator const BYTE &() const
{
	return m_value;
}

ParaByte::operator BYTE &()
{
	return m_value;
}
