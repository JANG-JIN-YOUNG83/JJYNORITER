#include "stdafx.h"
#include "ParameterSystem.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ParameterSystem::ParameterSystem()
	: m_isChanged(false)
{
}

ParameterSystem::~ParameterSystem()
{
}

void ParameterSystem::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_reason);
	ar.Serialize_Element(m_isChanged);
	ar.Serialize_Element(m_parameters);
}

void ParameterSystem::Add(LPCTSTR paraName, LPCTSTR previousValue, LPCTSTR value)
{
	bool isChanged = (CString(previousValue) != value);

	m_isChanged |= isChanged;

	m_parameters[paraName].m_isChanged = isChanged;
	m_parameters[paraName].m_previousValue = previousValue;
	m_parameters[paraName].m_value = value;
}

void ParameterSystem::Add(LPCTSTR paraName, double previousValue, double value)
{
	bool isChanged = (previousValue != value);

	m_isChanged |= isChanged;

	m_parameters[paraName].m_isChanged = isChanged;
	m_parameters[paraName].m_previousValue.Format(_T("%.3f"), previousValue);
	m_parameters[paraName].m_value.Format(_T("%.3f"), value);
}

void ParameterSystem::Add(LPCTSTR paraName, float previousValue, float value)
{
	bool isChanged = (previousValue != value);

	m_isChanged |= isChanged;

	m_parameters[paraName].m_isChanged = isChanged;
	m_parameters[paraName].m_previousValue.Format(_T("%.3f"), previousValue);
	m_parameters[paraName].m_value.Format(_T("%.3f"), value);
}

void ParameterSystem::Add(LPCTSTR paraName, int previousValue, int value)
{
	bool isChanged = (previousValue != value);

	m_isChanged |= isChanged;

	m_parameters[paraName].m_isChanged = isChanged;
	m_parameters[paraName].m_previousValue.Format(_T("%d"), previousValue);
	m_parameters[paraName].m_value.Format(_T("%d"), value);
}

void ParameterSystem::Add(LPCTSTR paraName, long previousValue, long value)
{
	bool isChanged = (previousValue != value);

	m_isChanged |= isChanged;

	m_parameters[paraName].m_isChanged = isChanged;
	m_parameters[paraName].m_previousValue.Format(_T("%d"), previousValue);
	m_parameters[paraName].m_value.Format(_T("%d"), value);
}
