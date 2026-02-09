#include "stdafx.h"
#include "BaseObject.h"
#include "ArchiveAllType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace CommonShare;

void BaseObject::LoadFromMemory(const void* message, const long messageLength)
{
	CMemFile memFile((BYTE*)message, messageLength);
	ArchiveAllType ar((CFile*)&memFile, ArchiveAllType::load);

	Serialize(ar);

	ar.Close();
	memFile.Close();
}
