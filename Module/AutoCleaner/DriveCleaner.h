#pragma once
#include "Module.h"
#include "CandidateCollector.h"
#include "CandidateGarbageDrive.h"
#include "MonitoringFolderInfo.h"

#include "Gadget/SimpleWorkerThread.h"

#include <afxmt.h>
#include <map>
#include <vector>

namespace AutoCleaner
{
	class CommonCallBack;

	class DriveCleaner : IPVM::SimpleWorkerThread
	{
	public:
		DriveCleaner(long driveIndex, CommonCallBack& callback);
		~DriveCleaner();

		bool IsCandidateDriveEnough(float freeRate) const;
		void MonitoringFolder_Reset();
		void MonitoringFolder_Add(long expirationHours, LPCTSTR folderPath, BOOL bOnOff, BOOL bAlways);
		void RemoveConditionsForDeletion();
		void SetDriveFreeRate(float freeRate);

		void Start();
		void Stop();

	private:
		const long m_driveIndex;
		bool m_createdThread;

		CommonCallBack& m_callback;
		virtual bool OnTrigger() override;

		CCriticalSection m_cs;
		CEvent m_running;

		std::vector<MonitoringFolderInfo> m_monitoringFolders;

		std::shared_ptr<CandidateCollector>	m_candiateCollector;
		CandidateGarbageDrive m_candiateGarbageDrive;
		long m_sequenceStep;
		int m_nSize;
		bool Initialize();
	};
}
