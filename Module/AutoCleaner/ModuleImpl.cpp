#include "stdafx.h"
#include "ModuleImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace AutoCleaner;

Module::Impl::Impl(LPVOID userData, CALLBACK_CLEANER_STATE_CHANGED* callbackStateChanged)
	: m_callback(userData, callbackStateChanged)
{
	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		m_drives[driveIndex] = std::make_shared<DriveCleaner>(driveIndex, m_callback);
	}
}

Module::Impl::~Impl()
{
	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		m_drives[driveIndex] = nullptr;
	}
}

long Module::Impl::FindNotEnoughDrive(float freeRate) const
{
	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		if (!m_drives[driveIndex]->IsCandidateDriveEnough(freeRate))
		{
			return driveIndex;
		}
	}

	return -1;
}

void Module::Impl::MonitoringFolder_Reset()
{
	CSingleLock lock(&m_cs, TRUE);

	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		m_drives[driveIndex]->MonitoringFolder_Reset();
	}
}

void Module::Impl::MonitoringFolder_Add(long expirationHours, LPCTSTR folderPath, BOOL bOnOff, BOOL bAlways)
{
	CSingleLock lock(&m_cs, TRUE);

	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		m_drives[driveIndex]->MonitoringFolder_Add(expirationHours, folderPath,  bOnOff,  bAlways);
	}
}

void Module::Impl::RemoveConditionsForDeletion()
{
	CSingleLock lock(&m_cs, TRUE);

	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		m_drives[driveIndex]->RemoveConditionsForDeletion();
	}
}

void Module::Impl::SetDriveFreeRate(char drive, float freeRate)
{
	CSingleLock lock(&m_cs, TRUE);

	long driveIndex = drive - 'A';
	if (driveIndex >= 0 && driveIndex < DRIVE_MAX_NUM)
	{
		m_drives[driveIndex]->SetDriveFreeRate(freeRate);
	}
}

void Module::Impl::Start()
{
	CSingleLock lock(&m_cs, TRUE);

	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		m_drives[driveIndex]->Start();
	}
}

void Module::Impl::Stop()
{
	CSingleLock lock(&m_cs, TRUE);

	for (long driveIndex = 0; driveIndex < DRIVE_MAX_NUM; driveIndex++)
	{
		m_drives[driveIndex]->Stop();
	}
}
