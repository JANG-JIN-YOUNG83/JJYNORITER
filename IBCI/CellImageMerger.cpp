#include "pch.h"
#include "CellImageMerger.h"
#include "InspectionInputData.h"
#include "RecyclableImage.h"
#include "SystemParameter.h"
#include "Types.h"
#include "Utility.h"

#include "../Grab/GrabExecutor.h"
#include "../Inspection/basedef.h"
#include "../SyncPci/Syncboard.h"
#include "../SharedModule/VisionHostShare/HostCommSocket.h"

#include "Base/LoggerInterface.h"
#include "Gadget/TimeCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CellImageMerger::CellImageMerger(GrabExecutor** grabExecutor, long sizeExecutor, IPVM::LoggerInterface& loggerTime, IPVM::LoggerInterface& loggerSensorPosition, IPVM::LoggerInterface& loggerSequence, IPVM::LoggerInterface& loggerGrab)
	: m_loggerGrab(loggerGrab)
	, m_loggerTime(loggerTime)
	, m_loggerSensorPosition(loggerSensorPosition)
	, m_loggerSequence(loggerSequence)
	, m_horizontalSize(0)
	, m_verticalSize(0)
	, m_sizeExecutor(sizeExecutor)
	, m_headFrameIndex(0)
	, m_tailFrameIndex(0)
	, m_maxUsedFrames(0)
{
	for (long cnt = 0; cnt < sizeExecutor; cnt++)
	{
		m_grabExecutor[cnt] = grabExecutor[cnt];
	}

	m_funcCallbackOnFrameMerged = nullptr;
	m_dataCallbackOnFrameMerged = nullptr;

	::memset(m_mergedBuffer, 0, sizeof(m_mergedBuffer));
    ::memset(m_mergeSecdBuffer, 0, sizeof(m_mergeSecdBuffer));


	::memset(m_mergedTotalBuffer, 0, sizeof(m_mergedTotalBuffer));
    ::memset(m_mergeTotlaSecdBuffer, 0, sizeof(m_mergeTotlaSecdBuffer));
	
	
	//__super::BeginWorker();
}

CellImageMerger::~CellImageMerger()
{
	for (int circularBufferIndex = 0; circularBufferIndex < m_mergedBufferCount; circularBufferIndex++)
	{
		delete[] m_mergedBuffer[circularBufferIndex];
        delete[] m_mergeSecdBuffer[circularBufferIndex];

	}
    delete[] m_mergedTotalBuffer[0];
    delete[] m_mergeTotlaSecdBuffer[0];

	__super::EndWorker();
}

void CellImageMerger::RegisterCallbackOnFrameMerged(CALLBACK_ON_FRAME_MERGED callbackFunc, void* callbackData)
{
	m_funcCallbackOnFrameMerged = callbackFunc;
	m_dataCallbackOnFrameMerged = callbackData;
}

UINT CellImageMerger::GetCircularBufferCount() const
{
	return m_mergedBufferCount;
}
void CellImageMerger::SetHorizontalSize(UINT horizontalSize)
{
	m_horizontalSize = horizontalSize;
}
void CellImageMerger::SetVerticalSize(UINT verticalSize)
{
	m_verticalSize = verticalSize;
}

bool CellImageMerger::OnTrigger()
{
	CSingleLock lock(&m_csMerger, TRUE);
	if (m_qMergeCount.size() == 0)
		return true;
	DWORD mergeCount = m_qMergeCount.front();
	//m_qMergeCount.pop_front();
    m_qMergeCount.erase(m_qMergeCount.begin());
    lock.Unlock();


	// MergedBuffer의 한 Line이 모두 채워지면 callBack
	auto errorCode = GrabError::None;
	if (m_funcCallbackOnFrameMerged && m_dataCallbackOnFrameMerged)
	{
		m_funcCallbackOnFrameMerged(
			m_dataCallbackOnFrameMerged,
			m_horizontalSize,
			m_verticalSize,
			mergeCount,
			errorCode);
	}

	lock.Lock();
	if (m_qMergeCount.size())
	{
        lock.Unlock();
        if (g_systemParameter.machineType != MachineType::UC1_mismatch)
        {
            AfxMessageBox(_T("MergedBuffer Ready is faster than earlier abnormally!"));
		}
		__super::Trigger();
	}

	return true;
}


void CellImageMerger::InitializeMergedBuffer()
{
	for (int circularBufferIndex = 0; circularBufferIndex < m_mergedBufferCount; circularBufferIndex++)
	{
		delete[] m_mergedBuffer[circularBufferIndex];
        delete[] m_mergeSecdBuffer[circularBufferIndex];
	}

	delete[] m_mergedTotalBuffer[0];
    delete[] m_mergeTotlaSecdBuffer[0];
	//delete m_queuePusher;

	for (int circularBufferIndex = 0; circularBufferIndex < m_mergedBufferCount; circularBufferIndex++)
	{
		m_mergedBuffer[circularBufferIndex] = new unsigned char[m_horizontalSize * m_verticalSize];
        m_mergeSecdBuffer[circularBufferIndex] = new unsigned char[m_horizontalSize * m_verticalSize];

		::memset(m_mergedBuffer[circularBufferIndex], 0, m_horizontalSize * m_verticalSize);
		::memset(m_mergeSecdBuffer[circularBufferIndex], 0, m_horizontalSize * m_verticalSize);
	}
    m_mergedTotalBuffer[0] = new unsigned char[m_horizontalSize * m_verticalSize *51 ];
    m_mergeTotlaSecdBuffer[0] = new unsigned char[m_horizontalSize * m_verticalSize * 51];

	::memset(m_mergedTotalBuffer[0], 0, m_horizontalSize * m_verticalSize*51);
    ::memset(m_mergeTotlaSecdBuffer[0], 0, m_horizontalSize * m_verticalSize*51);
}

void CellImageMerger::InitializeMergedBufferReady(int cameraCount)
{
	CSingleLock lock(&m_csMerger, TRUE);
	m_vvbBufferReady.resize(m_mergedBufferCount);
	for (long y = 0; y < m_vvbBufferReady.size(); y++)
	{
		m_vvbBufferReady[y].resize(cameraCount);
		for (long x = 0; x < m_vvbBufferReady[y].size(); x++)
		{
			m_vvbBufferReady[y][x] = false;
		}
	}
}

bool CellImageMerger::IsMergedBufferReady(int bufferIndex)
{
	auto& vbBufferReady = m_vvbBufferReady[bufferIndex % m_mergedBufferCount];
	for (long x = 0; x < vbBufferReady.size(); x++)
	{
		if (vbBufferReady[x] == false)
			return false;
	}

	for (long x = 0; x < vbBufferReady.size(); x++)
		vbBufferReady[x] = false;
	return true;
}

void CellImageMerger::TriggerMerge(int bufferIndex, int cameraIndex, bool bValue)
{
	CSingleLock lock(&m_csMerger, TRUE);
	m_vvbBufferReady[bufferIndex % m_mergedBufferCount][cameraIndex] = bValue;
	if (IsMergedBufferReady(bufferIndex))
	{
		m_qMergeCount.push_back(bufferIndex);
		__super::Trigger();
	}
}

void CellImageMerger::Start()
{
	if (this == nullptr)
	{
		return;
	}

	::InterlockedExchange((uint64_t*)&m_headFrameIndex, 0);
	::InterlockedExchange((uint64_t*)&m_tailFrameIndex, 0);

	m_qMergeCount.clear();

	__super::BeginWorker();
	//__super::Trigger();
}

void CellImageMerger::Stop()
{
	if (this == nullptr)
	{
		return;
	}

	//m_threadWaitKilling = true;

	//OnRequestStop();
	__super::EndWorker();

	//m_threadWaitKilling = false;

	{
		CSingleLock lock(&m_csMerger, TRUE);
		m_qMergeCount.clear();
	}
}

UINT CellImageMerger::GetHorizontalSize() const
{
	return m_horizontalSize;
}

UINT CellImageMerger::GetVerticalSize() const
{
	return m_verticalSize;
}

void CellImageMerger::GetUpdatedFrameIndices(int64_t& headFrameIndex, int64_t& tailFrameIndex) const
{
	::InterlockedExchange((uint64_t*)&headFrameIndex, m_headFrameIndex);
	::InterlockedExchange((uint64_t*)&tailFrameIndex, m_tailFrameIndex);
}

const unsigned char* CellImageMerger::GetFrameAddress(const int64_t frameIndex) const
{
	return m_mergedBuffer[frameIndex % m_mergedBufferCount];
}

const unsigned char* CellImageMerger::GetSecFrameAddress(const int64_t frameIndex) const
{
    return m_mergeSecdBuffer[frameIndex % m_mergedBufferCount];
}

const unsigned char* CellImageMerger::GetTotalFrameAddress(const int64_t frameIndex) const
{
    return m_mergedTotalBuffer[frameIndex];
}

const unsigned char* CellImageMerger::GetTotalSecFrameAddress(const int64_t frameIndex) const
{
    return m_mergeTotlaSecdBuffer[frameIndex];
}

float CellImageMerger::GetFrameGrabbedTime_ms(const int64_t frameIndex) const
{
	return 0.f;
	//return m_frameGrabbedTimes_ms[frameIndex % m_circularBufferCount];
}

void CellImageMerger::ProceedHeadFrameIndex(int64_t offset)
{
	if (this == nullptr)
	{
		return;
	}

	DWORD headFrameIndex = m_headFrameIndex + offset;

	ASSERT(headFrameIndex <= m_tailFrameIndex);

	if (headFrameIndex > m_tailFrameIndex)
	{
		m_loggerGrab.Log(0, _T("CellImageMerger Invalid merged buffer head."));
	}

	::InterlockedExchange((uint64_t*)&m_headFrameIndex, headFrameIndex);
}

void CellImageMerger::SetHeadFrameIndexInterval(int interval)
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
		m_loggerGrab.Log(0, _T("CellImageMerger Invalid merged buffer head."));
	}

	::InterlockedExchange((uint64_t*)&m_headFrameIndex, headFrameIndex);
}