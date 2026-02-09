#include "stdafx.h"
#include "I2V_RequestProgramBackup.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace TCPMSG::I2V;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

RequestProgramBackup::RequestProgramBackup()
{
}

RequestProgramBackup::~RequestProgramBackup()
{
}

void RequestProgramBackup::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_titleName);
}
