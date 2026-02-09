#pragma once
#include "basedef.h"
#include "BaseLogger.h"
#include <list>
#include <map>

namespace Utility
{
	class __UTILITY_CLASS__ Logger : public BaseLogger
	{
	public:
		Logger(LPCTSTR logFolder, LPCTSTR fileName, const int32_t flushingInterval_sec = 1);
		~Logger();

	private:
		CString m_fileName;

		void Flush();

		static CString CallBack_GetFileName(LPVOID userData, long type, const LoggerDate& timeLocal);
		CString CallBack_GetFileName(long type, const LoggerDate& timeLocal);
	};
}
