#pragma once

#include "../SharedModule/VisionHostShare/HostCommSocket.h"
#include "../SharedModule/VisionHostShare/basedef.h"
class CDlgViewCellImage;
class CISCommSocket : public Share::HostCommSocket
{
public:
	CISCommSocket(IPVM::LoggerInterface& loggerTcp, IPVM::LoggerInterface& loggerAlarm, const IPVM::MessageSocket::INIT_INFO initInfo, CDlgViewCellImage* viewmaindlg);
	~CISCommSocket();

	bool SendAlarm_CIS(const int& nCode, Share::ALARM_LEVEL nLevel, LPCTSTR strItem, LPCTSTR strContext, LPCTSTR strDetail, LPCTSTR strTroubleShooting);
private:
	CDlgViewCellImage* m_pViewMainDlg;
	IPVM::LoggerInterface& m_loggerAlarm;
};

