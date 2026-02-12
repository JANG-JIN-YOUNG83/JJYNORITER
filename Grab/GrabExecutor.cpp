#include "stdafx.h"
#include "GrabExecutor.h"

#include "../Module/SharedImageQueue/ImageQueuePusher.h"

#include "Base/LoggerInterface.h"
#include "Gadget/TimeCheck.h"
#include "Types/Image_8u_C1.h"
#include <Algorithm/ImageProcessing.h>
#include <iostream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GrabExecutor::GrabExecutor(IPVM::LoggerInterface& logger, const int cameraIndex)
	: IPVM::SimpleWorkerThread(INFINITE)
	, m_logger(logger)
	, m_queuePusher(nullptr)
	, m_cameraIndex(cameraIndex)
	, m_threadWaitKilling(false)
	, m_horizontalSize(0)
	, m_verticalSize(0)
	, m_headFrameIndex(0)
	, m_tailFrameIndex(0)
	, m_maxUsedFrames(0)
	, m_firstFrameAfterStart(true)
	, m_grabMode(GrabMode::Inline)
	, m_sensorImageMirror(false)
	, m_machineType(MachineType::UC1_mismatch)
{
	for (long modeIndex = 0; modeIndex < (long)GrabMode::END; modeIndex++)
	{
		m_callbackFunc[modeIndex] = nullptr;
		m_callbackData[modeIndex] = nullptr;
	}

	::memset(m_circularBuffer, 0, sizeof(m_circularBuffer));
    ::memset(m_circularSecBuffer, 0, sizeof(m_circularSecBuffer));

	::memset(m_TotalTopBuffer, 0, sizeof(m_TotalTopBuffer));
    ::memset(m_TotalBottomBuffer, 0, sizeof(m_TotalBottomBuffer));
    
	    
	::memset(m_frameGrabbedTimes_ms, 0, sizeof(m_frameGrabbedTimes_ms));

	::InitializeCriticalSection(&m_cs);
}

GrabExecutor::~GrabExecutor()
{
	for (int circularBufferIndex = 0; circularBufferIndex < m_circularBufferCount; circularBufferIndex++)
	{
		delete[] m_circularBuffer[circularBufferIndex];
        delete[] m_circularSecBuffer[circularBufferIndex];
	}
    delete[] m_TotalTopBuffer[0];
	delete[] m_TotalBottomBuffer[0];

	delete m_queuePusher;

	::DeleteCriticalSection(&m_cs);
}

void GrabExecutor::SetAlarm(long nCode)
{
	if (m_queuePusher != nullptr)
	{
		m_queuePusher->SetAlarm(nCode);
	}
}

void GrabExecutor::RegisterCallbackOnFrameUpdated(GrabMode mode, CALLBACK_ON_FRAME_UPDATED syncCallbackFunc, void *syncCallbackData)
{
	if (this == nullptr)
	{
		return;
	}

	m_callbackFunc[(long)mode] = syncCallbackFunc;
	m_callbackData[(long)mode] = syncCallbackData;
}

bool GrabExecutor::IsInitializeOk() const
{
	return m_circularBuffer != nullptr;
}

void GrabExecutor::Start(GrabMode mode)
{
	if (this == nullptr)
	{
		return;
	}

	m_grabMode = mode;

	if (m_queuePusher != nullptr)
	{
		m_queuePusher->UpdateImageQueue();
	}

	m_maxUsedFrames = 0;
	m_firstFrameAfterStart = true;

	// Total Buffer 초기화 (이전 검사 데이터 잔존 방지)
	if (m_TotalTopBuffer[0] && m_horizontalSize > 0 && m_verticalSize > 0)
	{
		::memset(m_TotalTopBuffer[0], 0, m_horizontalSize * m_verticalSize * 51);
		::memset(m_TotalBottomBuffer[0], 0, m_horizontalSize * m_verticalSize * 51);
	}

	if (mode != GrabMode::AREA)
	{
		OnStart();

		__super::BeginWorker();
		__super::Trigger();
	}
	else
	{
		OnStart();
	}
}

void GrabExecutor::Stop()
{
	if (this == nullptr)
	{
		return;
	}

	m_threadWaitKilling = true;

	OnRequestStop();
	__super::EndWorker();

	m_threadWaitKilling = false;
}

UINT GrabExecutor::GetCircularBufferCount() const
{
	return m_circularBufferCount;
}

UINT GrabExecutor::GetHorizontalSize() const
{
	return m_horizontalSize;
}

UINT GrabExecutor::GetVerticalSize() const
{
	return m_verticalSize;
}

BOOL GrabExecutor::GetSensorImageMirror() const
{
    return m_sensorImageMirror;
}

void GrabExecutor::GetUpdatedFrameIndices(int64_t &headFrameIndex, int64_t &tailFrameIndex) const
{
	::InterlockedExchange((uint64_t *)&headFrameIndex, m_headFrameIndex);
	::InterlockedExchange((uint64_t *)&tailFrameIndex, m_tailFrameIndex);
}

const unsigned char *GrabExecutor::GetFrameAddress(const int64_t frameIndex) const
{
	return m_circularBuffer[frameIndex % m_circularBufferCount];
}
const unsigned char* GrabExecutor::GetSecFrameAddress(const int64_t frameIndex) const
{
    return m_circularSecBuffer[frameIndex % m_circularBufferCount];
}
const unsigned char* GrabExecutor::GetSumAddress_Top(const int64_t frameIndex) const
{
    return m_TotalTopBuffer[frameIndex];
}
const unsigned char* GrabExecutor::GetSumAddress_Bottom(const int64_t frameIndex) const
{
    return m_TotalBottomBuffer[frameIndex ];
}

float GrabExecutor::GetFrameGrabbedTime_ms(const int64_t frameIndex) const
{
	return m_frameGrabbedTimes_ms[frameIndex % m_circularBufferCount];
}


void GrabExecutor::ProceedHeadFrameIndex(int64_t offset)
{
	if (this == nullptr)
	{
		return;
	}

	DWORD headFrameIndex = m_headFrameIndex + offset;

	ASSERT(headFrameIndex <= m_tailFrameIndex);

	if (headFrameIndex > m_tailFrameIndex)
	{
		m_logger.Log(0, _T("GrabExecutor[%d] Invalid circular buffer head."), m_cameraIndex);
	}

	::InterlockedExchange((uint64_t *)&m_headFrameIndex, headFrameIndex);
}

void GrabExecutor::SetHeadFrameIndexInterval(int interval)
{
	if (this == nullptr)
	{
		return;
	}

	if (interval <= 0)
	{
		return;
	}

	if (m_tailFrameIndex - interval <= 1)
	{
		return;
	}

	DWORD headFrameIndex = m_tailFrameIndex - interval;

	if (m_headFrameIndex >= headFrameIndex)
	{
		return;
	}

	ASSERT(headFrameIndex <= m_tailFrameIndex);

	if (headFrameIndex > m_tailFrameIndex)
	{
		m_logger.Log(0, _T("GrabExecutor[%d] Invalid circular buffer head."), m_cameraIndex);
	}

	::InterlockedExchange((uint64_t *)&m_headFrameIndex, headFrameIndex);
}

bool GrabExecutor::OnTrigger()
{
	if (__super::OnTrigger() == false)
	{
		return false;
	}

	m_logger.Log(0, _T("GrabExecutor[%d] Grab started."), m_cameraIndex);

	return true;
}

void GrabExecutor::CallbackOnOneFrameGrabbed(void *userData, DWORD grabCount, void *param1, void *param2)
{
	((GrabExecutor *)userData)->CallbackOnOneFrameGrabbed(grabCount, param1, param2);
}

void GrabExecutor::CallbackOnOneFrameGrabbed(DWORD grabCount, void *param1, void *param2)
{
	UNREFERENCED_PARAMETER(param1);
	UNREFERENCED_PARAMETER(param2);

	if (m_firstFrameAfterStart)
	{
		::InterlockedExchange((uint64_t *)&m_headFrameIndex, grabCount - 1);
		::InterlockedExchange((uint64_t *)&m_tailFrameIndex, grabCount - 1);
		m_firstFrameAfterStart = false;
	}

	if (m_grabMode == GrabMode::AREA)
	{
		//m_logger.Log(0, _T("GrabExecutor[%d] Circular buffer overrun."), m_cameraIndex);

		auto curUsedFrames = m_tailFrameIndex - m_headFrameIndex;
        if (curUsedFrames > m_maxUsedFrames)
        {
            m_maxUsedFrames = curUsedFrames;
            m_logger.Log(0, _T("GrabExecutor[%d] Max Used Frames = %d."), m_cameraIndex, m_maxUsedFrames);
        }
		if (m_callbackFunc[(long)GrabMode::AREA] && m_callbackData[(long)GrabMode::AREA])
		{
			m_callbackFunc[(long)GrabMode::AREA](
				m_callbackData[(long)GrabMode::AREA],
				m_cameraIndex,
				m_horizontalSize,
				m_verticalSize,
				grabCount,
				GrabError::None);
		}

		return;
	}


	auto curUsedFrames = m_tailFrameIndex - m_headFrameIndex;
	if (curUsedFrames > m_maxUsedFrames)
	{
		m_maxUsedFrames = curUsedFrames;
		m_logger.Log(0, _T("GrabExecutor[%d] Max Used Frames = %d."), m_cameraIndex, m_maxUsedFrames);
	}

	if (curUsedFrames >= m_circularBufferCount - 2)
	{
		m_logger.Log(0, _T("GrabExecutor[%d] Circular buffer overrun."), m_cameraIndex);

		// 파단시 이곳에 들어가는 것을 확인함. Overrun임을 알려주고, 영상은 버린다
		if (m_callbackFunc[(long)m_grabMode] && m_callbackData[(long)m_grabMode])
		{
			m_callbackFunc[(long)m_grabMode](
				m_callbackData[(long)m_grabMode],
				m_cameraIndex,
				m_horizontalSize,
				m_verticalSize,
				grabCount,
				GrabError::OverRun);
		}

		return;
	}

	::InterlockedExchange((uint64_t *)&m_tailFrameIndex, grabCount);

	// 임시 코드
	long nVerticalSize = (m_machineType == MachineType::ES_mismatch 
					   || m_machineType == MachineType::ES_rollpress_pinhole
					   || m_machineType == MachineType::ES_rollpress_surface) ? m_verticalSize * 2 : m_verticalSize;

    if (m_sensorImageMirror)
    {
        IPVM::Image_8u_C1 image(m_horizontalSize, m_horizontalSize, nVerticalSize, GetFrameAddress(m_tailFrameIndex - 1));

        IPVM::ImageProcessing::MirrorByAxisY(IPVM::Rect(0, 0, m_horizontalSize, nVerticalSize), image);
    }

	m_frameGrabbedTimes_ms[(m_tailFrameIndex - 1) % m_circularBufferCount] = IPVM::TimeCheck::Current_ms();


	//CString debug;
	//debug.Format(_T("%f s\n"), IPVM::TimeCheck::Current_ms() / 1000.f);
	//::OutputDebugString(debug);


#ifdef _TEST_SPLITTERREOVED_
	IPVM::Image_8u_C1 Loaadimage;

	//const auto pImage = GetFrameAddress(frameIndex);
	CString str, next;
	if ((m_tailFrameIndex - 1) % 2)
		str.Format(_T("D:\\TESTSET\\%06d.bmp"), 0);
	else
		str.Format(_T("D:\\TESTSET\\%06d.bmp"), 1);

	auto* images = GetFrameAddress_test(m_tailFrameIndex - 1);
	Loaadimage.LoadFrom(str);
	for (int i = 0; i < Loaadimage.GetSizeY(); i++)
	{
		memcpy(images + (sizeof(BYTE) * Loaadimage.GetSizeX() * i + 10), Loaadimage.GetMem() + (sizeof(BYTE) * Loaadimage.GetSizeX() * i + 10), ((Loaadimage.GetSizeX() - 10) * sizeof(BYTE)));
	}

#endif
	if (m_queuePusher != nullptr)
	{
		IPVM::Image_8u_C1 image(m_horizontalSize, m_horizontalSize, nVerticalSize, GetFrameAddress(m_tailFrameIndex - 1));
        IPVM::Image_8u_C1 image_sec(m_horizontalSize, m_horizontalSize, nVerticalSize, GetSecFrameAddress(m_tailFrameIndex - 1));
		m_queuePusher->Push(image);
        //m_queuePusher->Push(image_sec);
		
	
		/* CString basePath = _T("D:\\grabimg");
       
		 CString folderPath;
         folderPath.Format(_T("%s\\%d"), basePath, grabCount);

		  if (CreateDirectory(folderPath, NULL))
         {
             
			  CString Ch1filename;
              Ch1filename.Format(_T("%s\\Ch1_%d.bmp"), folderPath, grabCount);
              image.SaveBmp(Ch1filename);

			  CString Ch2filename;
              Ch2filename.Format(_T("%s\\Ch2_%d.bmp"), folderPath, grabCount);
              image_sec.SaveBmp(Ch2filename);

         }
         else
         {
             DWORD err = GetLastError();
             if (err == ERROR_ALREADY_EXISTS)
             {
                			 
			 }
         }   */
	}

	auto errorCode = GrabError::None;

	if (m_tailFrameIndex > 1)
	{
		const auto prevFrameMem = GetFrameAddress(m_tailFrameIndex - 2);
		const auto currFrameMem = GetFrameAddress(m_tailFrameIndex - 1);
		float fPrevFrameGrabbedTimes = GetFrameGrabbedTime_ms(m_tailFrameIndex - 2);
		float fCurFrameGrabbedTimes = GetFrameGrabbedTime_ms(m_tailFrameIndex - 1);

		const int LINE_ERROR_CHECK_POSITION = 2;
		const int nYpos = m_verticalSize - 1;

		const auto prevLastLineCheckValue = prevFrameMem[m_horizontalSize * nYpos + LINE_ERROR_CHECK_POSITION];
		const auto currFirstLineCheckValue = currFrameMem[LINE_ERROR_CHECK_POSITION];

		if (128 == prevLastLineCheckValue || 128 == currFirstLineCheckValue) /*128값은 스플리터가 최초 영상획득을 시작할 때 찍어주는 값이다.*/
		{
			//m_logger.Log(0, _T("Expection : LINE_ERROR_CHECK_POSITION Value 128"));
		}
		else
		{
			if( (unsigned char)(prevLastLineCheckValue + 1) != currFirstLineCheckValue)
			{
                if (m_machineType != MachineType::UC1_mismatch)
                {
                    m_logger.Log(0,
                        _T("GrabExecutor[%d] Circular buffer corrupted. GrabCount(%lld). Prev(%d). Curr(%d). Grabbed ")
                        _T("Times(%.4fms)"),
                        m_cameraIndex, m_tailFrameIndex, prevLastLineCheckValue, currFirstLineCheckValue,
                        fCurFrameGrabbedTimes - fPrevFrameGrabbedTimes);
                    errorCode = GrabError::CircularBufferCorrupted;
				}

			}
		}
	}

	if (m_callbackFunc[(long)m_grabMode] && m_callbackData[(long)m_grabMode])
	{
		m_callbackFunc[(long)m_grabMode](
			m_callbackData[(long)m_grabMode],
			m_cameraIndex,
			m_horizontalSize,
			m_verticalSize,
			grabCount,
			errorCode);
	}
}

void GrabExecutor::InitializeCircularBuffer()
{
	for (int circularBufferIndex = 0; circularBufferIndex < m_circularBufferCount; circularBufferIndex++)
	{
		delete[] m_circularBuffer[circularBufferIndex];
        delete[] m_circularSecBuffer[circularBufferIndex];
	}
   
	delete[] m_TotalTopBuffer[0];
    delete[] m_TotalBottomBuffer[0];

	delete m_queuePusher;

	int nPlane = (m_machineType == MachineType::ES_mismatch 
			   || m_machineType == MachineType::ES_rollpress_pinhole
			   || m_machineType == MachineType::ES_rollpress_surface) ? 2 : 1;

	if (m_grabMode == GrabMode::AREA)
	{
		nPlane = 1;
		long nWidth = 0, nHeight = 0;
		nWidth = m_horizontalSize * nPlane;
		nHeight = m_verticalSize;
		for (int circularBufferIndex = 0; circularBufferIndex < m_circularBufferCount; circularBufferIndex++)
		{
			
			m_circularBuffer[circularBufferIndex] = new unsigned char[nWidth * nHeight];

			::memset(m_circularBuffer[circularBufferIndex], 0, nWidth * nHeight);
		}
	}
	else
	{
		for (int circularBufferIndex = 0; circularBufferIndex < m_circularBufferCount; circularBufferIndex++)
		{
			m_circularBuffer[circularBufferIndex] = new unsigned char[m_horizontalSize * m_verticalSize * nPlane];
			m_circularSecBuffer[circularBufferIndex] = new unsigned char[m_horizontalSize * m_verticalSize * nPlane];
			
			::memset(m_circularBuffer[circularBufferIndex], 0, m_horizontalSize * m_verticalSize * nPlane);
            ::memset(m_circularSecBuffer[circularBufferIndex], 0, m_horizontalSize * m_verticalSize * nPlane);
		}
       
		m_TotalTopBuffer[0] = new unsigned char[m_horizontalSize * m_verticalSize * 51];
        m_TotalBottomBuffer[0] = new unsigned char[m_horizontalSize * m_verticalSize * 51];
		
		::memset(m_TotalTopBuffer[0], 0, m_horizontalSize * m_verticalSize * 51);
        ::memset(m_TotalBottomBuffer[0], 0, m_horizontalSize * m_verticalSize * 51);
	
	}
	
}

void GrabExecutor::SetSensorImageMirror(bool mirror)
{
	m_sensorImageMirror = mirror;
}

void GrabExecutor::SetFrameAddress(int64_t frameIndex, int64_t index, unsigned char value)
{
	m_circularBuffer[frameIndex % m_circularBufferCount][index] = value;
}
void GrabExecutor::SetSecFrameAddress(int64_t frameIndex, int64_t index, unsigned char value)
{
    m_circularSecBuffer[frameIndex % m_circularBufferCount][index] = value;
}

GrabMode GrabExecutor::GetGrabMode()
{
    return m_grabMode;
}

void GrabExecutor::SetFrameIndex_Area(int64_t headFrameIndex, int64_t tailFrameIndex)
{
    ::InterlockedExchange((uint64_t*)&m_headFrameIndex, headFrameIndex);
    ::InterlockedExchange((uint64_t*)&m_tailFrameIndex, tailFrameIndex);
}

#ifdef _TEST_SPLITTERREOVED_
unsigned char* GrabExecutor::GetFrameAddress_test(int64_t frameIndex)
{
	return m_circularBuffer[frameIndex % m_circularBufferCount];
}
#endif