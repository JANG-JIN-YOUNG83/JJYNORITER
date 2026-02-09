#include "stdafx.h"
#include "MonitoringFolderInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MonitoringFolderInfo::MonitoringFolderInfo()
	: m_expirationHours(30 * 24)
{
}

MonitoringFolderInfo::~MonitoringFolderInfo()
{
}
