#include "stdafx.h"
#include "GrabExecutor_iGrab.h"
#include "iManGDef.h"
#include "iManG.h" 

#include "../Module/SharedImageQueue/ImageQueuePusher.h"
#include "Base/LoggerInterface.h"

#pragma comment(lib, "./lib/iManG64.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static bool g_systemOpened[3] = {false, false, false};

GrabExecutor_iGrab::GrabExecutor_iGrab(IPVM::LoggerInterface& logger, const int cameraIndex, const int grabberIndex, const int channelIndex, LPCTSTR cfPathName)
	: GrabExecutor(logger, cameraIndex)
	, m_grabberIndex(grabberIndex)
	, m_channelIndex(channelIndex)
	, m_cfIndex(-1)
{
	if (g_systemOpened[grabberIndex] == false)
	{
        
		if (::Ig_OpenSystem(grabberIndex) == FALSE)
		{
			CString strMsg;
			strMsg.Format(_T("iGrab[%d] 그랩 보드 시스템 열기 실패"), grabberIndex);
			AfxMessageBox(strMsg);
			return;
//			::exit(0);
		}

		g_systemOpened[grabberIndex] = true;
	}

	if (::Ig_NewICF(grabberIndex, channelIndex, m_cfIndex) == FALSE)
	{
		CString strMsg;
		strMsg.Format(_T("iGrab[%d:%d] ICF 객체 생성에 실패했습니다."), grabberIndex, channelIndex);
		AfxMessageBox(strMsg);
		return;
//		::exit(0);
	}

	//CStringA astrPathName("C:\\CIS_MASTER_MEDIUM.ICF");
	CStringA astrPathName(cfPathName);

	if (!::Ig_OpenICF(grabberIndex, channelIndex, m_cfIndex, astrPathName.GetBuffer()))
	{
		CString strMsg;
		strMsg.Format(_T("iGrab[%d:%d] \"%s\" 파일 열기 실패하였습니다."), grabberIndex, channelIndex, cfPathName);
		AfxMessageBox(strMsg);
		return;
//		::exit(0);
	}

	::Ig_GetICFParam(grabberIndex, channelIndex, m_cfIndex, ICF_CTL_HORIZONTAL_SIZE, &m_horizontalSize);
	::Ig_GetICFParam(grabberIndex, channelIndex, m_cfIndex, ICF_CTL_VERTICAL_SIZE, &m_verticalSize);

	if (m_horizontalSize == 0 || m_verticalSize == 0)
	{
		CString strMsg;
		strMsg.Format(_T("iGrab[%d:%d] ICF 이미지 사이즈가 [%d , %d] 입니다."), grabberIndex, channelIndex, m_horizontalSize, m_verticalSize);
		AfxMessageBox(strMsg);
		return;
//		::exit(0);
	}

	InitializeCircularBuffer();
	m_queuePusher = new SharedMemory::ImageQueuePusher(cameraIndex);
}

GrabExecutor_iGrab::~GrabExecutor_iGrab()
{
	if (g_systemOpened[m_grabberIndex])
	{
		::Ig_CloseSystem(m_grabberIndex);

		g_systemOpened[m_grabberIndex] = false;
	}
}

CString GetErrorString(DWORD error)
{
	CString strError;

	switch (error)
	{
	case IGSTATUS_GRAB_SUCCESS:
		strError = _T("Grab Success");
		break;
	case IGSTATUS_GRAB_TIMEOUT:
		strError = _T("[Grab Error] TIME_OUT");
		break;
	case IGSTATUS_GRAB_START_SUCCESS:
		strError = _T("Grab Start Success");
		break;
	case IGSTATUS_GRAB_START_FAIL:
		strError = _T("[Grab Error] Grab Start Fail");
		break;
	case IGSTATUS_GRAB_STOP:
		strError = _T("Grab Stop");
		break;
	case IGSTATUS_GRAB_DMA_OVER_FAIL:
		strError = _T("[Grab Error] DMA_OVER_FAIL");
		break;
	case IGSTATUS_GRAB_FIFO_OVER_FAIL:
		strError = _T("[Grab Error] FIFO_OVER_FAIL");
		break;
	case IGSTATUS_GRAB_FAIL:
		strError = _T("[Grab Error] GRAB_FAIL");
		break;
	case IGSTATUS_GRAB_PT_PASSED_FAIL:
		strError = _T("[Grab Error] PT_PASSED_FAIL");
		break;
	case IGSTATUS_GRAB_PT_NO_DETECTED_FAIL:
		strError = _T("[Grab Error] PT_NO_DETECTED_FAIL");
		break;
	case IGSTATUS_GRAB_OPEN_CABLE_FAIL:
		strError = _T("[Grab Error] OPEN_CABLE_FAIL");
		break;
	}

	return strError;
}

bool GrabExecutor_iGrab::OnTrigger()
{
	if (__super::OnTrigger() == false)
	{
		return false;
	}

	DWORD dwRet = ::Ig_MultiGrabProcess(m_grabberIndex, m_channelIndex, m_circularBuffer, m_circularBufferCount, -1, MTG_END_WAIT, -1, CallbackOnOneFrameGrabbed, this);

	if (dwRet != IGSTATUS_GRAB_SUCCESS)
	{
		m_logger.Log(0, _T("iGrab[%d/%d] Ig_MultiGrabProcess returned %s"), m_grabberIndex, m_channelIndex, (LPCTSTR)GetErrorString(dwRet));
	}

	return true;
}

void GrabExecutor_iGrab::OnStart()
{
}

void GrabExecutor_iGrab::OnRequestStop()
{
	::Ig_GrabStop(m_grabberIndex, m_channelIndex);
}


void GrabExecutor_iGrab::SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount)
{
	if (m_queuePusher != nullptr)
	{
		CString strPath;

		CFileFind find;

		if (m_cameraIndex == 0)
		{
			strPath.Format(_T("%s\\UP"), targetFolder);

			if (!find.FindFile(strPath))
				CreateDirectory(strPath, NULL);
		}
		else
		{
			strPath.Format(_T("%s\\DOWN"), targetFolder);

			if (!find.FindFile(strPath))
				CreateDirectory(strPath, NULL);
		}

		if (!m_queuePusher->IsPusherBusy())
		{
			m_queuePusher->SetPusherBusy();
			m_queuePusher->Save(strPath, saveMaxCount);
			m_queuePusher->SetPusherIdle();
		}
		else
			m_queuePusher->Save(strPath, saveMaxCount);
	}
}

void GrabExecutor_iGrab::SetVerticalLength(long nPageLength)
{
    return;
}

void GrabExecutor_iGrab::SetSystemGain(CString strGain)
{
    return;
}

void GrabExecutor_iGrab::SetExposureTime(CString strExposure, int nRow)
{
    return;
}

double GrabExecutor_iGrab::GetSystemGain()
{
    return 0;
}

void GrabExecutor_iGrab::GetExposureTime(double& dBright, double& dDark)
{
    return;
}

void GrabExecutor_iGrab::SetSensorSelector(CString strSensorSel)
{
    return;
}

void GrabExecutor_iGrab::SetSensorGain(CString strSensorGain)
{
    return;
}

void GrabExecutor_iGrab::GetSensorInfo(int& nSensorSelector, double& dSensorGain)
{
    return;
}

void GrabExecutor_iGrab::GetRowGain(double& dRowGainBright, double& dRowGainDark)
{
    return;
}

void GrabExecutor_iGrab::SetRowGain(CString strRowGain, int nRow)
{
    return;
}
void GrabExecutor_iGrab::ResponseLeveling()
{
    return;
}
void GrabExecutor_iGrab::Reset()
{
    return;
}

double GrabExecutor_iGrab::GetFPGATemperature()
{
    return 0;
}

BOOL GrabExecutor_iGrab::GetCameraConnectCheck()
{
    return TRUE;
}

void GrabExecutor_iGrab::SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh)
{
    return;
}