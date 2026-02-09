#include "stdafx.h"
#include "Parameter.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Parameter::Parameter()
{
}


Parameter::~Parameter()
{
}

void Parameter::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_isChanged);
	ar.Serialize_Element(m_previousValue);
	ar.Serialize_Element(m_value);
	ar.Serialize_Element(m_ReleationItem);
}
