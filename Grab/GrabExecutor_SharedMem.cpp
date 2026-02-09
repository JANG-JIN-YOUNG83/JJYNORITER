#include "stdafx.h"
#include "GrabExecutor_SharedMem.h"

#include "Base/LoggerInterface.h"
#include "Gadget/TimeCheck.h"
#include "Types/Image_8u_C1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GrabExecutor_SharedMem::GrabExecutor_SharedMem(IPVM::LoggerInterface& logger, const int cameraIndex, long imageSizeX, long imageSizeY)
	: GrabExecutor(logger, cameraIndex)
	, m_queue(cameraIndex)
	, m_frameIndex(0)
{
	m_horizontalSize = imageSizeX;
	m_verticalSize = imageSizeY;

	InitializeCircularBuffer();
}

GrabExecutor_SharedMem::~GrabExecutor_SharedMem()
{
}

void GrabExecutor_SharedMem::SetAlarm(long nCode)
{
	CString text;
	text.Format(_T("GrabExecutor_SharedMem::SetAlarm(%d)"), nCode);

	AfxMessageBox(text);

	m_queue.SetAlarm(nCode);
}

bool GrabExecutor_SharedMem::OnTrigger()
{
	if (__super::OnTrigger() == false)
	{
		return false;
	}

	long frameIndex = 0;

	while (true)
	{
		IPVM::TimeCheck timeCheck;

		IPVM::Image_8u_C1 image;

		if (m_grabMode == GrabMode::Live)
		{
			image.Create(m_horizontalSize, m_verticalSize);
			static BYTE lastValue = 0;
			for (long index = 0; index < image.GetWidthBytes() * image.GetSizeY(); index++)
			{
				static long incValue[] = { -5, -3, -2, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 5};
				image.GetMem()[index] = lastValue;
				long value = incValue[rand() % (sizeof(incValue) / sizeof(long))];
				
				lastValue = (BYTE)min(255, max(0, (long)lastValue + value));
			}
		}

		if (m_grabMode == GrabMode::Live || m_queue.Pop(image))
		{
			::memcpy(m_circularBuffer[frameIndex++ % m_circularBufferCount], image.GetMem(), image.GetWidthBytes() * image.GetSizeY());

			CallbackOnOneFrameGrabbed(this, frameIndex, nullptr, nullptr);

			//m_logger.Log(0, _T("Grab-End,[%d],[%d],[%d]"), m_cameraIndex, m_headFrameIndex, m_tailFrameIndex);
		}

		const long sleepTime_ms = m_queue.GetPopIntervalTime_ms() - static_cast<long>(timeCheck.Elapsed_ms() + 0.5f);

		if (sleepTime_ms > 0)
		{
			::Sleep(sleepTime_ms);
		}

		if (m_threadWaitKilling)
		{
			break;
		}
	}

	return true;
}

void GrabExecutor_SharedMem::OnStart()
{
}

void GrabExecutor_SharedMem::OnRequestStop()
{
}

void GrabExecutor_SharedMem::SaveQueueImage(LPCTSTR targetFolder, long saveMaxCount)
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

	if (!m_queue.IsQueueBusy())
	{
		m_queue.SetBusy();
		m_queue.Save(strPath, saveMaxCount);
		m_queue.SetIdle();
	}
	else
		m_queue.Save(strPath, saveMaxCount);
}

void GrabExecutor_SharedMem::SetVerticalLength(long nPageLength)
{
    return;
}

void GrabExecutor_SharedMem::SetSystemGain(CString strGain)
{
    return;
}

void GrabExecutor_SharedMem::SetExposureTime(CString strExposure, int nRow)
{
    return;
}

double GrabExecutor_SharedMem::GetSystemGain()
{
    return 0;
}

void GrabExecutor_SharedMem::GetExposureTime(double& dBright, double& dDark)
{
    return;
}

void GrabExecutor_SharedMem::SetSensorSelector(CString strSensorSel)
{
    return;
}

void GrabExecutor_SharedMem::SetSensorGain(CString strSensorGain)
{
    return;
}

void GrabExecutor_SharedMem::GetSensorInfo(int& nSensorSelector, double& dSensorGain)
{
    return;
}

void GrabExecutor_SharedMem::GetRowGain(double& dRowGainBright, double& dRowGainDark)
{
    return;
}

void GrabExecutor_SharedMem::SetRowGain(CString strRowGain, int nRow)
{
    return;
}
void GrabExecutor_SharedMem::ResponseLeveling()
{
    return;
}
void GrabExecutor_SharedMem::Reset()
{
    return;
}

double GrabExecutor_SharedMem::GetFPGATemperature()
{
    return 0;
}
BOOL GrabExecutor_SharedMem::GetCameraConnectCheck()
{
    return TRUE;
}

void GrabExecutor_SharedMem::SetExposureTime_Multi(std::vector<CString> vecstrExposureValue, int nCntCh)
{
    return;
}