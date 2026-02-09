#include "stdafx.h"
#include "ParaString.h"
#include "ParaCollector.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_changeParaOldValue;

using namespace Base;

ParaString::ParaString(LPCTSTR name, LPCTSTR defaultValue)
	: CString(defaultValue)
	, m_name(name)
	, m_defaultValue(defaultValue)
	, m_oldValue(defaultValue)
{
}

ParaString::ParaString(LPCTSTR name, LPCTSTR defaultValue, DefectType Type, ...)
	: CString(defaultValue)
	, m_name(name)
	, m_defaultValue(defaultValue)
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

ParaString::~ParaString()
{
}

ParaString& ParaString::operator = (LPCTSTR value)
{
	*((CString*)this) = value;
	if (g_changeParaOldValue) m_oldValue = *this;

	return *this;
}

std::vector <int> ParaString::GetRelationItem() const
{
	return m_RelationItem;
}

void ParaString::Reset()
{
	*this = m_defaultValue;
	if (g_changeParaOldValue) m_oldValue = *this;
}

bool ParaString::Link(bool save, Database& db)
{
	bool success = db.Link(save, *((CString *)this));
	if (!success)
	{
		*((CString*)this) = m_defaultValue;
	}

	ParaCollector_Add(m_name, m_oldValue != *this, m_oldValue, *((CString *)this), m_RelationItem);

	if (g_changeParaOldValue) m_oldValue = *this;

	return success;
}
