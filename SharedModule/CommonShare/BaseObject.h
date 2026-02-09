#pragma once
#include "basedef.h"

namespace CommonShare
{
	class __COMMON_SHARE_CLASS__ BaseObject
	{
	public:
		void LoadFromMemory(const void* message, const long messageLength);
		virtual void Serialize(ArchiveAllType& ar) = 0;
	};
}
