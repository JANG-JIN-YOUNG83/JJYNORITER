#include "stdafx.h"
#include "SpecDataVision.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SpecDataVision::SpecDataVision()
{
	init();
}

SpecDataVision::~SpecDataVision()
{
}

void SpecDataVision::init()
{
	bIsValidData = FALSE;
	m_strModel_ID = _T("MODEL ID");
	m_strSpec_ID = _T("SPEC ID");
	m_strVisionType = _T("");
	m_vecSpecDataList.clear();
}

void SpecDataVision::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_strModel_ID);
	ar.Serialize_Element(m_strSpec_ID);
	ar.Serialize_Element(m_strVisionType);
	ar.Serialize_Element(m_vecSpecDataList);
}

long SpecDataVision::FindSpecIndex(DefectType type) const
{
	for (long index = 0; index < (long)m_vecSpecDataList.size(); index++)
	{
		auto& spec = m_vecSpecDataList[index];
		if (spec.m_inspType == type)
		{
			return index;
		}
	}

	return -1;
}

BOOL SpecDataVision::IsSpecEnabled(DefectType type)
{
	long index = FindSpecIndex(type);
	if (index < 0) return FALSE;

	return m_vecSpecDataList[index]._EnableInspection;
}

BOOL SpecDataVision::IsSpecEnabled(DefectType type, long& o_index) const
{
	o_index = FindSpecIndex(type);
	if (o_index < 0) return FALSE;

	return m_vecSpecDataList[o_index]._EnableInspection;
}

SpecData* SpecDataVision::FindSpec(DefectType type)
{
	auto index = FindSpecIndex(type);
	if (index < 0) return nullptr;

	return &m_vecSpecDataList[index];
}
