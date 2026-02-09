#include "stdafx.h"
#include "V2I_ReplyProgramBackup.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace TCPMSG::V2I;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ReplyProgramBackup::ReplyProgramBackup()
	: m_success(false)
{
}

ReplyProgramBackup::~ReplyProgramBackup()
{
}

void ReplyProgramBackup::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_titleName);
	ar.Serialize_Element(m_success);
}
