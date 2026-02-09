#include "stdafx.h"
#include "CISComm.h"
#include "../Grab/CisSetting.h"
#include "../Grab/GrabExecutor.h"
#include "../Grab/GrabExecutorContainer.h"

#include "Base/LoggerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CISComm::CISComm(IPVM::LoggerInterface& loggerSequence, IPVM::LoggerInterface& loggerGrab, MachineType machineType, int nPC_ID, LPCTSTR cfPath, ExecutorType executorType, int nGenerationVersion)
	: m_loggerSequence(loggerSequence)
	, m_loggerGrab(loggerGrab)
	, m_machineType(machineType)
	, m_nPC_ID(nPC_ID)
	, m_executorType(executorType)
	, m_pCisSetting(nullptr)
	, m_selectedPortIndex(0)
	, m_grabExecutorContainer(nullptr)
	, m_paraCur(machineType)
	, m_paraEdit(machineType)
	, m_nGenerationVersion(nGenerationVersion)
{
		m_grabExecutorContainer = new GrabExecutorContainer(m_loggerGrab, cfPath, machineType, m_executorType);
    GetSetting();
}

CISComm::~CISComm()
{
	delete m_grabExecutorContainer;

	// 메모리 해제
	if (m_pCisSetting)
	{
		m_pCisSetting->CIS_Close();
		delete m_pCisSetting;
	}
}

void CISComm::LinkDataBase(bool bSave, Database &dbJob)
{
	m_paraCur.LinkDataBase(bSave, dbJob);

	if (!bSave)
	{
		m_paraEdit = m_paraCur;
	}
}

bool CISComm::IsGrabExcutorContainerIntialized() const
{
	return m_grabExecutorContainer->IsInitializeOk();
}

GrabExecutor* CISComm::GetGrabExcutor(int cameraIndex)
{
	return m_grabExecutorContainer->GetExecutor(cameraIndex);
}

bool CISComm::SplitterStart()
{
	auto* cisSetting = GetSetting();
	bool success = false;

	for (long loop = 0; loop < 4; loop++)
	{
		// 최대 4번 시도
		if (!cisSetting->Splitter_Run(1)) continue;
		if (!cisSetting->Splitter_Run(0)) continue;

		success = true;
		break;
	}

	return success;
}

bool CISComm::SplitterStop()
{
	auto* cisSetting = GetSetting();
	bool success = false;

	for (long loop = 0; loop < 4; loop++)
	{
		if (!cisSetting->Splitter_Stop(0)) continue;
		if (!cisSetting->Splitter_Stop(1)) continue;

		success = true;
		break;
	}

	return success;
}

CCisSetting* CISComm::GetSetting()
{
	if (m_pCisSetting == nullptr)
	{
		m_pCisSetting = new CCisSetting(m_nGenerationVersion);
	}

	if (!m_pCisSetting->m_bOpen)
	{
		m_pCisSetting->CIS_Open();
	}

	//if (!m_isExistGrabHardware)
	if (m_executorType == ExecutorType::SharedMem)
	{
		m_pCisSetting->m_bOpen = TRUE;
	}

	return m_pCisSetting;
}

void CISComm::GrabExcutorContainerStart(GrabMode grabMode)
{
	m_grabExecutorContainer->Stop();

#ifdef _USE_CIS_LIVE_
	if (m_nPC_ID == 1)
	{
		auto* cisSetting = GetSetting();
		long realCameraCount = 2;

		if (m_nCamSingleDual != 0)
		{
			// Dual Cam
			realCameraCount = 4;
		}
		
		auto triggerInput = CIS_INPUT_ENCODER;
		if (grabMode == GrabMode::Live)
		{
			triggerInput = CIS_INPUT_FRAME;
		}

		for (long index = 0; index < realCameraCount; index++)
		{
			if (!cisSetting->Splitter_SetTriggerInput(index, triggerInput))
			{
				m_loggerSequence.Log(0, _T("failed: %s"), LPCTSTR(cisSetting->m_strError));
			}
		}
	}
#endif

	ExecutorType type = GetExecutorType();
	// coater, cis master scanStop → grabber start → cis master scanStart
	if (m_machineType == MachineType::UC1_mismatch && GetExecutorType() != ExecutorType::Euresys)
	{
		CSingleLock lock(&m_csCisScanStart, TRUE);
		if (!m_pCisSetting->Camera_ScanStop(0, 0))
		{
			m_loggerSequence.Log(0, _T("Download Failed: %s"), LPCTSTR(m_pCisSetting->m_strError));
			Sleep(1000);	// 무엇 때문에 실패했는지좀 보게 Sleep좀 주자
		}
		::AfxBeginThread(ThreadCisMasterScanStart, this);
	}
	m_grabExecutorContainer->Start(grabMode);
}

UINT CISComm::ThreadCisMasterScanStart(LPVOID pParam) 
{
	CISComm* pCisComm = (CISComm*)pParam;
	return pCisComm->ThreadCisMasterScanStart();
}

UINT CISComm::ThreadCisMasterScanStart()
{
	CSingleLock lock(&m_csCisScanStart, TRUE);
	HANDLE wait;
	wait = CreateEvent(NULL, TRUE, FALSE, NULL);
	::WaitForSingleObject(wait, 500);

	if (!m_pCisSetting->Camera_ScanStart(0, 0))
	{
		m_loggerSequence.Log(0, _T("Download Failed: %s"), LPCTSTR(m_pCisSetting->m_strError));
		Sleep(1000);	// 무엇 때문에 실패했는지좀 보게 Sleep좀 주자
	}

	return 0;
}

void CISComm::GrabExcutorContainerStop()
{
	m_grabExecutorContainer->Stop();
}

int CISComm::GetCameraCount()
{
	return m_grabExecutorContainer->GetCameraCount();
}

void CISComm::SetAlarmCode(long nCode)
{
	m_grabExecutorContainer->SetAlarm(nCode);
}

ExecutorType CISComm::GetExecutorType()
{
	return m_executorType;
}

MachineType CISComm::GetMachineType()
{
	return m_machineType;
}
