#include "stdafx.h"
#include "Module.h"
#include "ModuleImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace AutoCleaner;

Module::Module(LPVOID userData, CALLBACK_CLEANER_STATE_CHANGED* callbackStateChanged)
{
	m_impl = new Impl(userData, callbackStateChanged);
}

Module::~Module()
{
	delete m_impl;
}

long Module::FindNotEnoughDrive(float freeRate) const
{
	return m_impl->FindNotEnoughDrive(freeRate);
}

void Module::MonitoringFolder_Reset()
{
	m_impl->MonitoringFolder_Reset();
}

void Module::MonitoringFolder_Add(long expirationHours, LPCTSTR folderPath, BOOL bOnOff, BOOL bAlways)
{
	m_impl->MonitoringFolder_Add(expirationHours, folderPath,  bOnOff,  bAlways);
}

void Module::RemoveConditionsForDeletion()
{
	m_impl->RemoveConditionsForDeletion();
}

void Module::SetDriveFreeRate(char drive, float freeRate)
{
	m_impl->SetDriveFreeRate(drive, freeRate);
}

void Module::Start()
{
	m_impl->Start();
}

void Module::Stop()
{
	m_impl->Stop();
}
