#pragma once
#include "basedef.h"
#include <afxmt.h>

namespace AutoCleaner
{
	class CommonCallBack
	{
	public:
		CommonCallBack(LPVOID userData, CALLBACK_CLEANER_STATE_CHANGED* callbackStateChanged);
		~CommonCallBack();

		void StateChanged(LPCTSTR fmt, ...);

		CCriticalSection m_cs;
		LPVOID m_userData;
		CALLBACK_CLEANER_STATE_CHANGED* m_callbackStateChanged;
	};
}
