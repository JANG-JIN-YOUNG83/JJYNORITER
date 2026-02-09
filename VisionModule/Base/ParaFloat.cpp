#include "stdafx.h"
#include "ParaFloat.h"
#include "ParaCollector.h"
#include "StringFormat.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_changeParaOldValue;

using namespace Base;

ParaFloat::ParaFloat(LPCTSTR name, float defaultValue)
	: m_name(name)
	, m_defaultValue(defaultValue)
	, m_value(defaultValue)
	, m_oldValue(defaultValue)
{
}
ParaFloat::ParaFloat(LPCTSTR name, float defaultValue, DefectType Type, ...)
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
ParaFloat::~ParaFloat()
{
}

ParaFloat& ParaFloat::operator = (const float& value)
{
	m_value = value;
	if (g_changeParaOldValue) m_oldValue = m_value;

	return *this;
}

float ParaFloat::GetValue() const
{
	return m_value;
}

std::vector <int> ParaFloat::GetRelationItem() const
{
	return m_RelationItem;
}

void ParaFloat::Reset()
{
	m_value = m_defaultValue;
	if (g_changeParaOldValue) m_oldValue = m_value;
}

bool ParaFloat::Link(bool save, Database& db)
{
	bool success = db.Link(save, m_value);
	if (!success)
	{
		m_value = m_defaultValue;
	}

	ParaCollector_Add(m_name, m_oldValue != m_value, StringFormat(_T("%.3f"), m_oldValue), StringFormat(_T("%.3f"), m_value), m_RelationItem);
	if (g_changeParaOldValue) m_oldValue = m_value;

	return success;
}

ParaFloat::operator const float &() const
{
	return m_value;
}

ParaFloat::operator float &()
{
	return m_value;
}
