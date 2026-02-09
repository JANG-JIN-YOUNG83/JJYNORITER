#include "stdafx.h"
#include "ParameterVision.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ParameterVision::ParameterVision()
	: m_isChanged(false)
{
}

ParameterVision::~ParameterVision()
{
}

void ParameterVision::Add(LPCTSTR cameraName, LPCTSTR moduleName, LPCTSTR paraName, bool isChanged, LPCTSTR previousValue, LPCTSTR value, std::vector<int> ReleationItem)
{
	m_isChanged |= isChanged;
	m_cameras[cameraName].Add(moduleName, paraName, isChanged, previousValue, value, ReleationItem);
}

bool ParameterVision::Save(LPCTSTR filePath)
{
	CString text;
	text.AppendFormat(_T("=============================================\r\n"));
	text.AppendFormat(_T("Changed Parameter\r\n"));
	text.AppendFormat(_T("=============================================\r\n"));

	for (auto& itCamera : m_cameras)
	{
		if (!itCamera.second.m_isChanged) continue;

		text.AppendFormat(_T("@ Camera: %s\r\n"), (LPCTSTR)itCamera.first);

		for (auto& itModule : itCamera.second.m_modules)
		{
			if (!itModule.second.m_isChanged) continue;

			text.AppendFormat(_T(" - Module: %s\r\n"), (LPCTSTR)itModule.first);

			for (auto& itParameter : itModule.second.m_parameters)
			{
				auto& para = itParameter.second;
				if (!para.m_isChanged) continue;
				text.AppendFormat(_T("  : %s = %s -> %s\r\n"),
					(LPCTSTR)itParameter.first,
					(LPCTSTR)para.m_previousValue,
					(LPCTSTR)para.m_value);
			}
		}
	}

	text.AppendFormat(_T("=============================================\r\n"));
	text.AppendFormat(_T("All Parameter\r\n"));
	text.AppendFormat(_T("=============================================\r\n"));

	for (auto& itCamera : m_cameras)
	{
		text.AppendFormat(_T("@ Camera: %s\r\n"), (LPCTSTR)itCamera.first);

		for (auto& itModule : itCamera.second.m_modules)
		{
			text.AppendFormat(_T(" - Module: %s\r\n"), (LPCTSTR)itModule.first);

			for (auto& itParameter : itModule.second.m_parameters)
			{
				auto& para = itParameter.second;
				text.AppendFormat(_T("  : %s = %s\r\n"), (LPCTSTR)itParameter.first, (LPCTSTR)para.m_value);
			}
		}
	}

	CFile file;
	if (!file.Open(filePath, CFile::modeCreate | CFile::modeWrite)) return false;

	file.Write(text.GetBuffer(), text.GetLength() * sizeof(TCHAR));
	file.Close();
	return true;
}

void ParameterVision::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_reason);
	ar.Serialize_Element(m_modelName);
	ar.Serialize_Element(m_isChanged);

	ar.Serialize_Element(m_cameras);
}

CString ParameterVision::GetVisionValue(CString strposition, CString strmodule, CString strpara)
{
	auto item = m_cameras.find(strposition);
	if (item != m_cameras.end())
	{
		return item->second.GetParaModuleValue(strmodule, strpara);
	}
	return _T("NONE");
}
