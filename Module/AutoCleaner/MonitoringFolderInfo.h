#pragma once

class MonitoringFolderInfo
{
public:
	MonitoringFolderInfo();
	~MonitoringFolderInfo();

	long m_expirationHours;
	CString m_folderPath;
	BOOL m_OnOfff;
	BOOL m_Always;
};

