#include "stdafx.h"
#include "ParaDouble.h"
#include "ParaCollector.h"
#include "StringFormat.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_changeParaOldValue;

using namespace Base;

ParaDouble::ParaDouble(LPCTSTR name, double defaultValue)
	: m_name(name)
	, m_defaultValue(defaultValue)
	, m_value(defaultValue)
	, m_oldValue(defaultValue)
{
}
ParaDouble::ParaDouble(LPCTSTR name, double defaultValue, DefectType Type, ...)
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

ParaDouble::~ParaDouble()
{
}

ParaDouble& ParaDouble::operator = (const double& value)
{
	m_value = value;
	if (g_changeParaOldValue) m_oldValue = m_value;

	return *this;
}

std::vector <int> ParaDouble::GetRelationItem() const
{
	return m_RelationItem;
}
CString ParaDouble::GetText() const
{
	return StringFormat(_T("%.2f"), m_value);
}
void ParaDouble::Reset()
{
	m_value = m_defaultValue;
	if (g_changeParaOldValue) m_oldValue = m_value;
}

bool ParaDouble::Link(bool save, Database& db)
{
	bool success = db.Link(save, m_value);
	if (!success)
	{
		m_value = m_defaultValue;
	}

	ParaCollector_Add(m_name, m_oldValue != m_value, StringFormat(_T("%.3f"), m_oldValue), StringFormat(_T("%.3f"), m_value),  m_RelationItem);
	if (g_changeParaOldValue) m_oldValue = m_value;

	return success;
}

ParaDouble::operator const double &() const
{
	return m_value;
}

ParaDouble::operator double &()
{
	return m_value;
}
