#include "stdafx.h"
#include "GrabExecutorContainer.h"
#include "GrabExecutor_iGrab.h"
#include "GrabExecutor_Sapera.h"
#include "GrabExecutor_SharedMem.h"
#include "GrabExecutor_Coaxlink.h"
#include "GrabExecutor_Insnex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GrabExecutorContainer::GrabExecutorContainer(
    IPVM::LoggerInterface& logger, LPCTSTR cfPathName, const MachineType machineType, const ExecutorType executorType)
	: m_logger(logger)
	, m_nCameraCount(2)
{
	::memset(m_camera, 0, sizeof(m_camera));
    m_Euresys = nullptr;
	int cameraIndex = 0;

	switch(executorType)
	{
		case ExecutorType::SharedMem:
		{
			long sizeX = 8640;
			long sizeY = 1000;
			if (machineType == MachineType::ES_notching_dual)
			{
				sizeX = 15552;
			}

			m_camera[0] = new GrabExecutor_SharedMem(logger, cameraIndex++, sizeX, sizeY);
			m_camera[1] = new GrabExecutor_SharedMem(logger, cameraIndex++, sizeX, sizeY);
			break;
		}
		case ExecutorType::iGrab:
		{
            if (machineType == MachineType::UC1_mismatch)
			{
				m_nCameraCount = 3;
				/*CString pathMasterMedium("C:\\CIS_MASTER_MEDIUM.ICF");
				CString pathMasterBase("C:\\CIS_SLAVE_BASE_6912.ICF");
				m_camera[0] = new GrabExecutor_iGrab(logger, cameraIndex++, 0, 0, pathMasterMedium.GetBuffer());
				m_camera[1] = new GrabExecutor_iGrab(logger, cameraIndex++, 1, 0, pathMasterMedium.GetBuffer());
				m_camera[2] = new GrabExecutor_iGrab(logger, cameraIndex++, 2, 0, pathMasterBase.GetBuffer());*/

				CString pathMasterMedium("C:\\intekplus\\IBNI\\icfs\\CIS_MASTER_MEDIUM.ICF");
                CString pathSlaveMedium("C:\\intekplus\\IBNI\\icfs\\CIS_SLAVE_MEDIUM.ICF");
                CString pathSlaveBase("C:\\intekplus\\IBNI\\icfs\\CIS_SLAVE_BASE_6912.ICF");
                m_camera[0] = new GrabExecutor_iGrab(logger, cameraIndex++, 0, 0, pathMasterMedium.GetBuffer());
                m_camera[1] = new GrabExecutor_iGrab(logger, cameraIndex++, 1, 0, pathSlaveMedium.GetBuffer());
                m_camera[2] = new GrabExecutor_iGrab(logger,	 cameraIndex++, 2, 0, pathSlaveBase.GetBuffer());

			}
            else
            {
                m_camera[0] = new GrabExecutor_iGrab(logger, cameraIndex++, 0, 0, cfPathName);

                if (machineType == MachineType::ES_notching_dual)
                {
                    m_camera[1] = new GrabExecutor_iGrab(logger, cameraIndex++, 1, 0, cfPathName);
                }
                else
                {
                    m_camera[1] = new GrabExecutor_iGrab(logger, cameraIndex++, 0, 1, cfPathName);
                }
            }
			break;
		}

		case ExecutorType::Sapera:
		{
			int cameraIndex = 0;
			long serverCount = SapManager::GetServerCount();
			char serverName[CORSERVER_MAX_STRLEN];
			char userDefinedName[256];
			int featureCount = 0;

			for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
			{
				SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
				int channelCount = SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice);
				for (int channelIndex = 0; channelIndex < channelCount; channelIndex++)  // 디바이스가 존재하는지 확인
				{
					m_camera[cameraIndex] = new GrabExecutor_Sapera(logger, cameraIndex, serverIndex, channelIndex, cfPathName, machineType);
					cameraIndex++;
				}
			}
			
			//if (cameraIndex > 0)
			//((GrabExecutor_Sapera*)m_camera[0])->ResetShaftEncoder();
			
			m_nCameraCount = cameraIndex;
			break;
		}

		case ExecutorType::Euresys:
		{
			int nGrabberIndex = 0;
			int nMaxCam = 0;
            //0일 경우 무한으로 찍음, 임시로 1회만 찍게 수정
            long nGrabCnt = 1;
			m_Euresys = new GrabExecutor_Coaxlink(logger, cameraIndex, nGrabberIndex);
			nMaxCam = m_Euresys->m_nToTalCameraCnt;
			for(int nCnt = 0; nCnt < nMaxCam; nCnt++)
			{
                m_Euresys->m_pCoaxlink[nCnt]->SetGrabCount(nGrabCnt);
				m_camera[nCnt] = (GrabExecutor*)(m_Euresys->m_pCoaxlink[nCnt]);
				//m_camera[nCnt] = new CCoaxlinkGrabber();
				//((GrabExecutor_Coaxlink*)m_camera[nCnt])->Open();
				if (m_camera[nCnt] != NULL)
				{
					TRACE("Open Success\n");
				}
                
			}
			
				
			m_nCameraCount = nMaxCam;
			break;
		}
        case ExecutorType::Insnex:
        {
            int cameraIndex = 0;
            int featureCount = 0;
            int serverIndex = 0;
            int channelIndex = 0;

			//사페라를 2개로 만들어서 사용하게 되어있으나, 테스트 용으로 1개만 생성해서 테스트 진행 후, 2개까지 붙일 수 있도록 코드 구현해야함
            m_camera[cameraIndex] = new GrabExecutor_Insnex(logger, cameraIndex, serverIndex, channelIndex, cfPathName, machineType);
            cameraIndex++;
            //m_camera[cameraIndex] = new GrabExecutor_Insnex(logger, cameraIndex, serverIndex, channelIndex, cfPathName, machineType);
            //cameraIndex++;
            //if (cameraIndex > 0)
            //((GrabExecutor_Sapera*)m_camera[0])->ResetShaftEncoder();
            m_nCameraCount = cameraIndex;
            break;
        }
	}
}

GrabExecutorContainer::~GrabExecutorContainer()
{
	for (int cameraIndex = 0; cameraIndex < m_nCameraCount; cameraIndex++)
	{
		m_camera[cameraIndex]->Stop();
	}

	for (int cameraIndex = 0; cameraIndex < m_nCameraCount; cameraIndex++)
	{
		delete m_camera[cameraIndex];
	}

	if (m_Euresys != nullptr)
		delete m_Euresys;

}

bool GrabExecutorContainer::IsInitializeOk() const
{
	for (int cameraIndex = 0; cameraIndex < m_nCameraCount; cameraIndex++)
	{
		if (!m_camera[cameraIndex]->IsInitializeOk())
		{
			return false;
		}
	}

	return true;
}

void GrabExecutorContainer::Start(GrabMode grabMode)
{
	for (int cameraIndex = 0; cameraIndex < m_nCameraCount; cameraIndex++)
	{
		m_camera[cameraIndex]->Start(grabMode);
	}
}

void GrabExecutorContainer::Stop()
{
	for (int cameraIndex = 0; cameraIndex < m_nCameraCount; cameraIndex++)
	{
		m_camera[cameraIndex]->Stop();
	}
}

int GrabExecutorContainer::GetCameraCount()
{
	return m_nCameraCount;
}

void GrabExecutorContainer::SetAlarm(long ncode)
{
	m_camera[0]->SetAlarm(ncode);
}

GrabExecutor *GrabExecutorContainer::GetExecutor(int cameraIndex)
{
	return m_camera[cameraIndex];
}

void GrabExecutorContainer::SetPageLength(long nCameraIndex, long nPageLength)
{
    m_camera[nCameraIndex]->SetVerticalLength(nPageLength);
}



