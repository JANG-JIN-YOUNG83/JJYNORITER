#include "stdafx.h"
#include "V2I_VisionInformation.h"
#include "../../VersionInfo.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace TCPMSG::V2I;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VisionInformation::VisionInformation()
	: m_version(IBCI_MACRO_VERSION)
{
}

VisionInformation::~VisionInformation()
{
}

void VisionInformation::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_version);
}
