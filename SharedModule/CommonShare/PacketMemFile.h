#pragma once

#include "basedef.h"

namespace CommonShare
{
	class __COMMON_SHARE_CLASS__ PacketMemFile : public CMemFile
	{
		DECLARE_DYNAMIC(PacketMemFile)

	public:
		// Constructors
		explicit PacketMemFile(ULONGLONG nGrowBytes = 1024);
		PacketMemFile(BYTE* lpBuffer, ULONGLONG nBufferSize, ULONGLONG nGrowBytes = 0);
		virtual ~PacketMemFile();

		ULONGLONG GetBufferSize() const;
	};
}
