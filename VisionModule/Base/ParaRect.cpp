#include "stdafx.h"
#include "ParaRect.h"
#include "ParaCollector.h"
#include "StringFormat.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_changeParaOldValue;

using namespace Base;

ParaRect::ParaRect(LPCTSTR name, const IPVM::Rect& defaultValue)
	: IPVM::Rect(defaultValue)
	, m_name(name)
	, m_defaultValue(defaultValue)
	, m_oldValue(defaultValue)
{
}
ParaRect::ParaRect(LPCTSTR name, const IPVM::Rect& defaultValue, DefectType Type, ...)
	: IPVM::Rect(defaultValue)
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


ParaRect::~ParaRect()
{
}

ParaRect& ParaRect::operator = (const IPVM::Rect& value)
{
	*((IPVM::Rect*)this) = value;
	if (g_changeParaOldValue) m_oldValue = *this;

	return *this;
}

std::vector <int> ParaRect::GetRelationItem() const
{
	return m_RelationItem;
}

void ParaRect::Reset()
{
	*this = m_defaultValue;
	if (g_changeParaOldValue) m_oldValue = *this;
}

bool ParaRect::Link(bool save, Database& db)
{
	bool success = db.Link(save, *((IPVM::Rect *)this));
	if (!success)
	{
		*((IPVM::Rect*)this) = m_defaultValue;
	}

	ParaCollector_Add(m_name + _T(" Left"), m_oldValue.m_left != m_left,
		StringFormat(_T("%d"), m_oldValue.m_left), StringFormat(_T("%d"), m_left), m_RelationItem);
	ParaCollector_Add(m_name + _T(" Top"), m_oldValue.m_top != m_top,
		StringFormat(_T("%d"), m_oldValue.m_top), StringFormat(_T("%d"), m_top), m_RelationItem);
	ParaCollector_Add(m_name + _T(" Right"), m_oldValue.m_right != m_right,
		StringFormat(_T("%d"), m_oldValue.m_right), StringFormat(_T("%d"), m_right), m_RelationItem);
	ParaCollector_Add(m_name + _T(" Bottom"), m_oldValue.m_bottom != m_bottom,
		StringFormat(_T("%d"), m_oldValue.m_bottom), StringFormat(_T("%d"), m_bottom), m_RelationItem);

	if (g_changeParaOldValue) m_oldValue = *this;

	return success;
}
