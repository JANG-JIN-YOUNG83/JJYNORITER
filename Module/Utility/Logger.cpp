#include "pch.h"
#include "Logger.h"
#include "../../VersionInfo.h"

#include "Gadget/Miscellaneous.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Utility;

Logger::Logger(LPCTSTR logFolder, LPCTSTR fileName, const int32_t flushingInterval_sec)
	: BaseLogger(LoggerType::SpeparateHour, logFolder, fileName, this, 0, CallBack_GetFileName, true, flushingInterval_sec)
	, m_fileName(fileName)
{
}

Logger::~Logger()
{
	Terminate();
}

CString Logger::CallBack_GetFileName(LPVOID userData, long type, const LoggerDate& timeLocal)
{
	return ((Logger*)userData)->CallBack_GetFileName(type, timeLocal);
}

CString Logger::CallBack_GetFileName(long type, const LoggerDate& timeLocal)
{
	return m_fileName;
}
