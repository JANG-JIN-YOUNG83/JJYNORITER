#pragma once
#include <vector>
#include "../../VersionInfo.h"
namespace CommonShare
{
	class ArchiveAllType;
	class BaseObject;
	class PacketMemFile;
};

// 0 - LGES
// 1 - UC1
#define INSPECTION_MACHINE_UC1 0

#ifdef __COMMON_SHARE_CLASS_EXPORT__
#define __COMMON_SHARE_CLASS__	__declspec(dllexport)
#else
#define __COMMON_SHARE_CLASS__	__declspec(dllimport)
#endif
