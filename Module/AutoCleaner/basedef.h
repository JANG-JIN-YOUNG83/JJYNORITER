#pragma once

namespace AutoCleaner
{
	typedef void (CALLBACK_CLEANER_STATE_CHANGED)(LPVOID userData, LPCTSTR state);

	class Module;
};

#ifdef __AUTO_CLEANER_CLASS_EXPORT__
#define __AUTO_CLEANER_CLASS__ __declspec(dllexport)
#else
#define __AUTO_CLEANER_CLASS__ __declspec(dllimport)
#endif
