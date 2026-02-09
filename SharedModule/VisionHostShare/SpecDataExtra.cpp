#include "stdafx.h"
#include "SpecDataExtra.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SpecDataExtra::SpecDataExtra()
{
	m_dTabPitch = 0.0f;
	m_dTabWidth = 0.0f;
	m_dTabSide = 0.0f;
	m_nSensingDistance = 0;
	m_nCIS1TrgPos = 0;
}

SpecDataExtra::~SpecDataExtra()
{
}

void SpecDataExtra::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_dTabPitch);
	ar.Serialize_Element(m_dTabWidth);
	ar.Serialize_Element(m_dTabSide);
	ar.Serialize_Element(m_nSensingDistance);
	ar.Serialize_Element(m_nCIS1TrgPos);
}