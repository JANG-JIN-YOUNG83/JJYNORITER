#include "stdafx.h"
#include "DriveCleaner.h"
#include "CommonCallBack.h"
#include "ModuleImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace AutoCleaner;

DriveCleaner::DriveCleaner(long driveIndex, CommonCallBack& callback)
	: m_driveIndex(driveIndex)
	, m_running(FALSE, TRUE)
	, m_callback(callback)
	, m_createdThread(false)
	, m_candiateGarbageDrive(driveIndex)
{
	m_candiateCollector = std::make_shared<CandidateCollector>(driveIndex);
	m_sequenceStep = 0;
}

DriveCleaner::~DriveCleaner()
{
	m_running.SetEvent();

	__super::EndWorker();
}

bool DriveCleaner::IsCandidateDriveEnough(float freeRate) const
{
	if (m_monitoringFolders.size() == 0)
	{
		// 관리 폴더가 없다
		return true;
	}

	return CandidateGarbageDrive::GetFreeRate(m_driveIndex) >= freeRate;
}

void DriveCleaner::MonitoringFolder_Reset()
{
	CSingleLock lock(&m_cs, TRUE);

	bool prevRunning = (WaitForSingleObject(m_running, 0) != WAIT_TIMEOUT);

	if (prevRunning)
	{
		Stop();
	}

	m_sequenceStep = 0;
	m_monitoringFolders.clear();

	if (prevRunning)
	{
		Start();
	}
}

void DriveCleaner::MonitoringFolder_Add(long expirationHours, LPCTSTR folderPath, BOOL bOnOff, BOOL bAlways)
{
	CSingleLock lock(&m_cs, TRUE);

	bool prevRunning = (WaitForSingleObject(m_running, 0) != WAIT_TIMEOUT);

	if (prevRunning)
	{
		Stop();
	}

	m_sequenceStep = 0;

	MonitoringFolderInfo info;
	info.m_expirationHours = expirationHours;
	info.m_folderPath = folderPath;
	info.m_OnOfff = bOnOff;
	info.m_Always = bAlways;

	m_monitoringFolders.push_back(info);

	if (prevRunning)
	{
		Start();
	}
}

void DriveCleaner::RemoveConditionsForDeletion()
{
	CSingleLock lock(&m_cs, TRUE);

	bool prevRunning = (WaitForSingleObject(m_running, 0) != WAIT_TIMEOUT);

	if (prevRunning)
	{
		Stop();
	}

	m_candiateGarbageDrive.RemoveConditionsForDeletion();

	if (prevRunning)
	{
		Start();
	}
}

void DriveCleaner::SetDriveFreeRate(float freeRate)
{
	CSingleLock lock(&m_cs, TRUE);

	bool prevRunning = (WaitForSingleObject(m_running, 0) != WAIT_TIMEOUT);

	if (prevRunning)
	{
		Stop();
	}

	m_candiateGarbageDrive.SetDriveFreeRate(freeRate);

	if (prevRunning)
	{
		Start();
	}
}

void DriveCleaner::Start()
{
	CSingleLock lock(&m_cs, TRUE);

	m_running.SetEvent();

	if (!m_createdThread)
	{
		if (Initialize())
		{
			// 모니터링할 폴더가 있을 때만 Thread 활성화
			m_createdThread = true;

			// 그냥 항상 활성화 시켜도 문제는 없겠지만
			// 나중에 다른 일로 Crash Dump체크 때
			// Thread Stack이 너무 많아져서 체크가 귀찮아질까봐

			__super::BeginWorker();
			__super::Trigger();
		}
	}
}

void DriveCleaner::Stop()
{
	CSingleLock lock(&m_cs, TRUE);

	m_running.ResetEvent();
}

bool DriveCleaner::OnTrigger()
{
	WaitForSingleObject(m_running, INFINITE);

	switch (m_sequenceStep)
	{
	case 0:
		//---------------------------------------------------------------------------
		// Module::Impl 초기화
		//---------------------------------------------------------------------------

		if (m_monitoringFolders.size() == 0)
		{
			// 세팅된게 아무것도 없다
			Sleep(1000);
			break;
		}

		Initialize();

		if (true)
		{
			m_callback.StateChanged(_T("[Drive:%d] Initialize"), m_driveIndex + 1);
		}

		m_sequenceStep++;
		break;

	case 1:
		//---------------------------------------------------------------------------
		// 삭제 목록 수집
		//---------------------------------------------------------------------------

		m_candiateCollector->Run();
		int nSize;
		nSize = m_candiateCollector->GetCollectedItemCount();
		if (nSize && m_nSize != nSize)
		{
			m_nSize = nSize;
			m_callback.StateChanged(_T("[Drive:%d] Collecting List ... (Collected Item Count:%d)"),
				m_driveIndex + 1,
				nSize);
		}

		if (m_candiateCollector->IsCompleted())
		{
			m_candiateCollector->Pop(m_candiateGarbageDrive);

			if (m_candiateGarbageDrive.m_garbageObjs.size() == 0 && m_candiateGarbageDrive.m_garbageObjs_Always.size() == 0)
			{
				// 쓰레기가 하나도 없었는데 너무 열정적으로 일하지 말자
				m_callback.StateChanged(_T("[Drive:%d] Collected List is empty"), m_driveIndex + 1);
				Sleep(1000);
			}

			m_sequenceStep++;
		}
		break;

	case 2:
		//---------------------------------------------------------------------------
		// 삭제 조건에 따라 목록 삭제
		//---------------------------------------------------------------------------

		if (m_candiateGarbageDrive.m_garbageObjs.size() == 0 && m_candiateGarbageDrive.m_garbageObjs_Always.size() == 0)
		{
			m_sequenceStep++;
		}
		else
		{
			bool deleteFile = false;

			if (m_candiateGarbageDrive.CollectInfo() && m_candiateGarbageDrive.IsCapacityNeeded() && m_candiateGarbageDrive.m_garbageObjs.size())
			{
				// 지울 조건을 만족시켰다면

				CTime tm(CTime::GetCurrentTime());

				auto itObj = m_candiateGarbageDrive.m_garbageObjs.begin();

				if (itObj->first < tm)
				{
					// 삭제 가능한 상태
					CString fileName = itObj->second->GetPath();
					long fileNameStartIndex = fileName.ReverseFind(_T('\\'));
					if (fileNameStartIndex >= 0)
					{
						fileName = fileName.Mid(fileNameStartIndex + 1);
					}

					if (itObj->second->Delete())
					{
						m_callback.StateChanged(_T("[Drive:%d] Deleted '%s'"), m_driveIndex + 1, (LPCTSTR)fileName);
					}

					m_candiateGarbageDrive.m_garbageObjs.erase(itObj);
					deleteFile = true;
				}
			}
			if (m_candiateGarbageDrive.m_garbageObjs_Always.size()) // 용량이 넉넉할때 항상 지워야하는것을 지우자
			{
				CTime tm(CTime::GetCurrentTime());

				auto itObj_Always = m_candiateGarbageDrive.m_garbageObjs_Always.begin();

				if (itObj_Always->first < tm)
				{
					// 삭제 가능한 상태
					CString fileName = itObj_Always->second->GetPath();
					long fileNameStartIndex = fileName.ReverseFind(_T('\\'));
					if (fileNameStartIndex >= 0)
					{
						fileName = fileName.Mid(fileNameStartIndex + 1);
					}

					if (itObj_Always->second->Delete())
					{
						m_callback.StateChanged(_T("[Drive:%d] Deleted '%s'"), m_driveIndex + 1, (LPCTSTR)fileName);
					}

					m_candiateGarbageDrive.m_garbageObjs_Always.erase(itObj_Always);
					deleteFile = true;

				}
			}
			if (!deleteFile)
			{
				m_sequenceStep++;
				Sleep(1000);
			}

		}
		break;

	case 3:
		//---------------------------------------------------------------------------
		// 파일 목록을 다시 만들어야 함
		//---------------------------------------------------------------------------
		m_sequenceStep = 0;
		break;
	}

	__super::Trigger();

	return true;
}

bool DriveCleaner::Initialize()
{
	m_candiateCollector->Request(m_monitoringFolders);
	if (m_candiateCollector->IsCompleted())
	{
		// 요청하자 마자 끝났다는 얘기는 폴더중에 관심있는 Drive가 없다는 얘기다
		return false;
	}

	return true;
}
