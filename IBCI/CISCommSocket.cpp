#include "pch.h"
#include "CISCommSocket.h"
#include "resource.h"
#include "DlgViewCellImage.h"
#include "Base/LoggerInterface.h"

CISCommSocket::CISCommSocket(IPVM::LoggerInterface& loggerTcp, IPVM::LoggerInterface& loggerAlarm, const IPVM::MessageSocket::INIT_INFO initInfo, DlgViewCellImage* viewmaindlg)
	:Share::HostCommSocket(loggerTcp, initInfo)
	, m_pViewMainDlg(viewmaindlg)
	, m_loggerAlarm(loggerAlarm)
{
	
}

CISCommSocket::~CISCommSocket()
{
	
}
bool CISCommSocket::SendAlarm_CIS(const int& nCode, Share::ALARM_LEVEL nLevel, LPCTSTR strItem, LPCTSTR strContext, LPCTSTR strDetail, LPCTSTR strTroubleShooting)
{
	SendAlarm(nCode, nLevel, strItem, strContext, strDetail, strTroubleShooting, false);
	CString strLog;
	strLog.Format(_T("[CODE %03d] CONTEXT : %s, DETAIL : %s"), nCode, strContext, strDetail);
	m_loggerAlarm.Log(0, strLog);

	return 0;
}
