#include "stdafx.h"
#include "ParaLong.h"
#include "ParaCollector.h"
#include "StringFormat.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_changeParaOldValue;

using namespace Base;

ParaLong::ParaLong(LPCTSTR name, long defaultValue)
	: m_name(name)
	, m_defaultValue(defaultValue)
	, m_value(defaultValue)
	, m_oldValue(defaultValue)
{
}
ParaLong::ParaLong(LPCTSTR name, long defaultValue, DefectType Type, ...)
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
ParaLong::~ParaLong()
{
}

ParaLong& ParaLong::operator = (const long value)
{
	m_value = value;
	if (g_changeParaOldValue) m_oldValue = m_value;

	return *this;
}

bool ParaLong::operator == (const ParaLong& value) const
{
	return m_value == value.m_value;
}

bool ParaLong::operator == (const int value) const
{
	return m_value == value;
}

bool ParaLong::operator == (const long value) const
{
	return m_value == value;
}

bool ParaLong::operator != (const ParaLong& value) const
{
	return m_value != value.m_value;
}

bool ParaLong::operator != (const int value) const
{
	return m_value != value;
}

bool ParaLong::operator != (const long value) const
{
	return m_value != value;
}

CString ParaLong::GetText() const
{
	return StringFormat(_T("%d"), m_value);
}

std::vector <int> ParaLong::GetRelationItem() const
{
	return m_RelationItem;
}

void ParaLong::Reset()
{
	m_value = m_defaultValue;
	if (g_changeParaOldValue) m_oldValue = m_value;
}

bool ParaLong::Link(bool save, Database& db)
{
	bool success = db.Link(save, m_value);
	if (!success)
	{
		m_value = m_defaultValue;
	}

	ParaCollector_Add(m_name, m_oldValue != m_value, StringFormat(_T("%d"), m_oldValue), StringFormat(_T("%d"), m_value),  m_RelationItem);
	if (g_changeParaOldValue) m_oldValue = m_value;

	return success;
}

ParaLong::operator const long &() const
{
	return m_value;
}

ParaLong::operator long &()
{
	return m_value;
}
