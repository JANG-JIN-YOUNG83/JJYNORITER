#include "stdafx.h"
#include "ImageQueuePusher.h"
#include "QueueMemoryData.h"

#include "Types/Image_8u_C1.h"

#include <afxmt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SharedMemory;

ImageQueuePusher::ImageQueuePusher(const long identifier)
	: Base(sizeof(QueueMemoryData), _T("SharedMemory::ImageQueue"), identifier)
	, m_identifier(identifier)
	, m_eventThreadStop(FALSE, TRUE)
	, m_queueImageSizeX(0)
	, m_queueImageSizeY(0)
	, m_queueImageCount(0)
{
	m_identifierKey.Format(_T("SharedMemory::ImageQueue%d"), m_identifier);
	m_data = (QueueMemoryData *)m_sharedMemoryPointer;

	::DuplicateHandle(::GetCurrentProcess(),
		::AfxBeginThread(ThreadGarbage, this)->m_hThread,
		::GetCurrentProcess(),
		&m_threadGarbage,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	::DuplicateHandle(::GetCurrentProcess(),
		::AfxBeginThread(ThreadMake, this)->m_hThread,
		::GetCurrentProcess(),
		&m_threadMake,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	UpdateImageQueue();
}

ImageQueuePusher::~ImageQueuePusher()
{
	m_eventThreadStop.SetEvent();
	::WaitForSingleObject(m_threadGarbage, 5000);
	::WaitForSingleObject(m_threadMake, 5000);
	::CloseHandle(m_threadGarbage);
	::CloseHandle(m_threadMake);
}

bool ImageQueuePusher::Push(const IPVM::Image_8u_C1& image)
{
	if (m_data->m_queueSize <= 0)
	{
		// Mutex�� Lock �ɰ� üũ�ϸ� ����� �Ⱦ�����
		// �ӵ��϶��� ���� �� ���Ƽ� �׳� �ϴ� ���� ���� �о üũ�ϰ� ��

		return false;
	}

	CMutex mutex(TRUE, m_identifierKey);

	LazyUpdateImageQueue();

	if (image.GetWidthBytes() * image.GetSizeY() > m_queueImageSizeX * m_queueImageSizeY)
	{
		// �̹����� ���ۺ��� ũ��
		return false;
	}

	if (m_data->m_queueSize <= 0)
	{
		return false;
	}

	if (m_data->m_isBusy_Pusher)
	{
		return false;
	}

	if (m_data->m_isBusy_Queue)
	{
		// Queue �۾����̶� Push�Ұ���
		return false;
	}

	int queueSize = m_data->m_queueSize;
	int dataCount = m_data->m_dataCount;

	auto index = (m_data->m_headIndex + dataCount) % queueSize;
	m_queueImages[index]->Set(image);

	if (dataCount >= queueSize)
	{
		// �����Ͱ� �� �� ������ ���� ������ ���� �����Ѵ�
		m_data->m_headIndex = (m_data->m_headIndex + 1) % queueSize;
	}
	else
	{
		m_data->m_dataCount++;
	}

	return true;
}

void ImageQueuePusher::Save(LPCTSTR targetFolder, long saveMaxCount)
{
	long dataIndex = 0;
	long dataCount = 0;
	long queueSize = 0;

	if (true)
	{
		CMutex mutex(TRUE, m_identifierKey);

		// Save�� ���� �ɸ� �� �����Ƿ� Lock�� �ɾ���� ���������� �ʴ´�.
		// ��� Save�ϴ� ������ ������ ���� Queue�� �߰� Image�� �״� �۾��� �������� �ʰ� �Ѵ�

		if (!m_data->m_isBusy_Pusher)
		{
			// Run ���̴�
			return;
		}

		dataIndex = m_data->m_headIndex;
		dataCount = m_data->m_dataCount;
		queueSize = m_data->m_queueSize;
	}

	if (saveMaxCount < 0) saveMaxCount = dataCount;

	long saveCount = min(saveMaxCount, dataCount);
	long offset = dataCount - saveCount;

	dataIndex = (dataIndex + offset) % queueSize;
	dataCount -= offset;

	long countIndex = 0;

	while (countIndex < dataCount)
	{
		CString filePath;
		filePath.Format(_T("%s\\%06d.bmp"), targetFolder, countIndex + 1);

		m_queueImages[dataIndex]->SaveBmp(filePath);

		dataIndex = (dataIndex + 1) % queueSize;
		countIndex++;
	}
}

void ImageQueuePusher::SetPusherBusy()
{
	CMutex mutex(TRUE, m_identifierKey);
	m_data->m_isBusy_Pusher = true;
}

void ImageQueuePusher::SetPusherIdle()
{
	CMutex mutex(TRUE, m_identifierKey);
	m_data->m_isBusy_Pusher = false;
}

bool ImageQueuePusher::IsPusherBusy() const
{
	return m_data->m_isBusy_Pusher;
}

void ImageQueuePusher::UpdateImageQueue()
{
	CMutex mutex(TRUE, m_identifierKey);

	if (m_data->m_isBusy_Pusher) return;

	bool changed = false;
	if (m_queueImageSizeX != m_data->m_imageSizeX ||
		m_queueImageSizeY != m_data->m_imageSizeY ||
		m_queueImageCount != m_data->m_queueSize)
	{
		changed = true;
	}

	if (!changed) return;

	m_queueImageSizeX = m_data->m_imageSizeX;
	m_queueImageSizeY = m_data->m_imageSizeY;
	m_queueImageCount = m_data->m_queueSize;

	m_queueImages.clear();

	while ((long)m_queueImages.size() < m_queueImageCount)
	{
		auto imageSize = m_queueImageSizeX * m_queueImageSizeY;
		m_queueImages.push_back(std::make_shared<Image>(m_identifier, (long)m_queueImages.size(), imageSize));
	}
}

void ImageQueuePusher::LazyUpdateImageQueue()
{
	bool changed = false;
	if (m_queueImageSizeX != m_data->m_imageSizeX ||
		m_queueImageSizeY != m_data->m_imageSizeY ||
		m_queueImageCount != m_data->m_queueSize)
	{
		changed = true;
	}

	if (!changed) return;

	m_data->m_isBusy_Pusher = true;
	m_queueImageSizeX = m_data->m_imageSizeX;
	m_queueImageSizeY = m_data->m_imageSizeY;
	m_queueImageCount = m_data->m_queueSize;

	std::queue<std::shared_ptr<SharedMemory::Image>> garbagePushList;

	for (auto image : m_queueImages)
	{
		garbagePushList.push(image);
	}

	// Garbage Collector�� ���۸� ��� �Ѱ��.
	// ���� Queue�� Resize�� Garbage Collector�� ���۽��Ѽ� Thread�� �����ϰ� ����

	m_queueImages.clear();

	if (garbagePushList.size())
	{
		m_csGarbage.Lock();
		while (!garbagePushList.empty())
		{
			auto image = garbagePushList.front();
			m_garbageImages.push(image);
			garbagePushList.pop();
		}

		m_csGarbage.Unlock();
		m_eventThreadRun_Garbage.SetEvent();
	}

	m_eventThreadRun_Make.SetEvent();
}

UINT ImageQueuePusher::ThreadGarbage(LPVOID userData)
{
	return ((ImageQueuePusher *)userData)->ThreadGarbage();
}

UINT ImageQueuePusher::ThreadMake(LPVOID userData)
{
	return ((ImageQueuePusher *)userData)->ThreadMake();
}

UINT ImageQueuePusher::ThreadGarbage()
{
	HANDLE eventHandles[] = { m_eventThreadStop, m_eventThreadRun_Garbage};
	while (true)
	{
		auto evt = WaitForMultipleObjects(2, eventHandles, FALSE, INFINITE);
		if (evt == WAIT_OBJECT_0) break;

		while (true)
		{
			m_csGarbage.Lock();

			if (m_garbageImages.size() == 0)
			{
				m_csGarbage.Unlock();
				break;
			}

			auto image = m_garbageImages.front();
			m_garbageImages.pop();

			m_csGarbage.Unlock();
		}
	}

	return 0;
}

UINT ImageQueuePusher::ThreadMake()
{
	HANDLE eventHandles[] = { m_eventThreadStop, m_eventThreadRun_Make };
	while (true)
	{
		auto evt = WaitForMultipleObjects(2, eventHandles, FALSE, INFINITE);
		if (evt == WAIT_OBJECT_0) break;

		while (true)
		{
			if (m_queueImages.size() == m_queueImageCount)
			{
				// Pusher�� ���� �غ�Ǿ���
				m_data->m_isBusy_Pusher = false;
				break;
			}

			if (m_queueImages.size() > m_queueImageCount)
			{
				ASSERT(!_T("??"));
				break;
			}

			auto imageSize = m_queueImageSizeX * m_queueImageSizeY;
			m_queueImages.push_back(std::make_shared<Image>(m_identifier, (long)m_queueImages.size(), imageSize));
		}
	}

	return 0;
}

void ImageQueuePusher::SetAlarm(long nCode)
{
	/*CString text;
	text.Format(_T("ImageQueuePusher::SetAlarm(%d)"), nCode);

	AfxMessageBox(text);*/

	m_data->m_AlarmSave_Queue = nCode;
}
