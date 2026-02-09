#include "stdafx.h"
#include "ParameterCamera.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ParameterCamera::ParameterCamera()
	: m_isChanged(false)
{
}

ParameterCamera::~ParameterCamera()
{
}

void ParameterCamera::Add(LPCTSTR moduleName, LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value , std::vector<int> ReleationItem)
{
	m_isChanged |= isChanged;
	m_modules[moduleName].Add(paraName, isChanged, previousValue, value, ReleationItem);
}

void ParameterCamera::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_isChanged);
	ar.Serialize_Element(m_modules);
}

CString ParameterCamera::GetParaModuleValue(CString strmodule, CString strpara)
{
	auto item = m_modules.find(strmodule);
	if (item != m_modules.end())
	{
		return item->second.GetParaValue(strpara);
	}
	return _T("None");
}