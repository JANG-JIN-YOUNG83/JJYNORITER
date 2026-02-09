#include "stdafx.h"
#include "ParameterModule.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ParameterModule::ParameterModule()
	: m_isChanged(false)
{
}

ParameterModule::~ParameterModule()
{
}

void ParameterModule::Add(LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value, std::vector<int> ReleationItem)
{
	m_isChanged |= isChanged;

	m_parameters[paraName].m_isChanged = isChanged;
	m_parameters[paraName].m_previousValue = previousValue;
	m_parameters[paraName].m_value = value;
	m_parameters[paraName].m_ReleationItem = ReleationItem;
	
}

void ParameterModule::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_isChanged);
	ar.Serialize_Element(m_parameters);
}

CString ParameterModule::GetParaValue(CString strpara)
{
	auto item = m_parameters.find(strpara);
	if (item != m_parameters.end())
	{
		return item->second.m_value;
	}
	return _T("None");
}