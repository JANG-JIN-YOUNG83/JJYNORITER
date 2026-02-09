#pragma once

#include "../SharedModule/VisionHostShare/HostCommSocket.h"
#include "../SharedModule/VisionHostShare/basedef.h"

class AlarmCommSocket : public Share::HostCommSocket
{
public:
    AlarmCommSocket(IPVM::LoggerInterface& loggerTcp, IPVM::LoggerInterface& loggerAlarm,
        const IPVM::MessageSocket::INIT_INFO initInfo);
    ~AlarmCommSocket();

	bool SendAlarm_Log(const int& nCode, Share::ALARM_LEVEL nLevel, LPCTSTR strItem, LPCTSTR strContext, LPCTSTR strDetail, LPCTSTR strTroubleShooting);
    IPVM::MessageSocket::INIT_INFO m_initInfo;

private:
	IPVM::LoggerInterface& m_loggerAlarm;
};

