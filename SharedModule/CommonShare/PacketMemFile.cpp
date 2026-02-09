#include "stdafx.h"
#include "PacketMemFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace CommonShare;

IMPLEMENT_DYNAMIC(PacketMemFile, CMemFile);

PacketMemFile::PacketMemFile(ULONGLONG nGrowBytes)
	: CMemFile(nGrowBytes)
{

}

PacketMemFile::PacketMemFile(BYTE* lpBuffer, ULONGLONG nBufferSize, ULONGLONG nGrowBytes)
	: CMemFile(lpBuffer, nBufferSize, nGrowBytes)
{
}

PacketMemFile::~PacketMemFile()
{

}

ULONGLONG PacketMemFile::GetBufferSize() const
{
	return m_nBufferSize;
}
