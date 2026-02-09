#pragma once
#include "Module.h"
#include "CommonCallBack.h"
#include "DriveCleaner.h"

#include <afxmt.h>

#include "Gadget/SimpleWorkerThread.h"

#define DRIVE_MAX_NUM	(26)

namespace AutoCleaner
{
	class Module::Impl
	{
	public:
		Impl(LPVOID userData, CALLBACK_CLEANER_STATE_CHANGED* callbackStateChanged);
		~Impl();

		long FindNotEnoughDrive(float freeRate) const;
		void MonitoringFolder_Reset();
		void MonitoringFolder_Add(long expirationHours, LPCTSTR folderPath, BOOL bOnOff, BOOL bAlways);
		void RemoveConditionsForDeletion();
		void SetDriveFreeRate(char drive, float freeRate);

		void Start();
		void Stop();

	private:
		void SequenceRun(long driveIndex);

		CCriticalSection m_cs;
		std::shared_ptr<DriveCleaner> m_drives[DRIVE_MAX_NUM];

		CommonCallBack m_callback;
	};
}
