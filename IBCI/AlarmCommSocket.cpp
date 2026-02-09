#include "pch.h"
#include "AlarmCommSocket.h"
#include "resource.h"
#include "Base/LoggerInterface.h"

AlarmCommSocket::AlarmCommSocket(
    IPVM::LoggerInterface& loggerTcp, IPVM::LoggerInterface& loggerAlarm, const IPVM::MessageSocket::INIT_INFO initInfo)
    : Share::HostCommSocket(loggerTcp, initInfo)
    , m_loggerAlarm(loggerAlarm)
    , m_initInfo(initInfo)
{
	
}

AlarmCommSocket::~AlarmCommSocket()
{
	
}
bool AlarmCommSocket::SendAlarm_Log(const int& nCode, Share::ALARM_LEVEL nLevel, LPCTSTR strItem, LPCTSTR strContext,
    LPCTSTR strDetail, LPCTSTR strTroubleShooting)
{
	SendAlarm(nCode, nLevel, strItem, strContext, strDetail, strTroubleShooting, false);
	CString strLog;
	strLog.Format(_T("[CODE %03d] CONTEXT : %s, DETAIL : %s"), nCode, strContext, strDetail);
	m_loggerAlarm.Log(0, strLog);

	return 0;
}
