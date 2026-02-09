#include "pch.h"
#include "SaveImageQueue.h"
#include "RecyclableImage.h"

#include "Base/LoggerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IPVM::Status SaveImageData::Save(int& nSizeX, int& nSizeY)
{
	CString filePath = m_filePath.Trim();
	IPVM::Status status = IPVM::Status::unknownError;

	if (filePath.Right(3).MakeUpper() == _T("BMP"))
	{
		status = m_image.SaveBmp(m_filePath);
	}
	else if (filePath.Right(3).MakeUpper() == _T("PNG"))
	{
		status = m_image.SavePng(m_filePath);
	}
	else if (filePath.Right(3).MakeUpper() == _T("JPG"))
	{
		status = m_image.SaveJpeg(m_filePath, 90);
	}

	nSizeX = m_image.GetSizeX();
	nSizeY = m_image.GetSizeY();

	return status;
}

SaveImageQueue::SaveImageQueue(IPVM::LoggerInterface& loggerImageSaveFailed, const long index)
	: m_loggerImageSaveFailed(loggerImageSaveFailed)
	, m_index(index)
	, m_maximumSize(0)
{
	__super::BeginWorker();
}

SaveImageQueue::~SaveImageQueue()
{
	__super::EndWorker();

	while (m_queue.size())
	{
		delete m_queue.front();
		m_queue.pop();
	}

	while (m_idleQueue.size())
	{
		delete m_idleQueue.front();
		m_idleQueue.pop();
	}
}

void SaveImageQueue::SetMaximumQueueSize(long maximumSize)
{
	CSingleLock lock(&m_cs, TRUE);

	m_maximumSize = maximumSize;

	if (m_maximumSize > 0)
	{
		long addBufferCount = m_maximumSize - (long)(m_idleQueue.size() + m_queue.size());

		if (addBufferCount > 0)
		{
			for (long index = 0; index < addBufferCount; index++)
			{
				m_idleQueue.push(new SaveImageData);
			}
		}
		else
		{
			for (long index = 0; index < -addBufferCount; index++)
			{
				if (m_idleQueue.size() == 0) break;
				m_idleQueue.pop();
			}
		}
	}
}

void SaveImageQueue::Push(const IPVM::Image_8u_C1& image, LPCTSTR folderPath, LPCTSTR fileName)
{
	CSingleLock lock(&m_cs, TRUE);

	auto* data = GetBuffer();
	if (data == nullptr)
	{
		long idleCount = (int)m_idleQueue.size();
		long workCount = (int)m_queue.size();

		m_loggerImageSaveFailed.Log(0, _T("Can not save image (%s) Reject Image Queue Overflowed (B:%d, I:%d, W:%d)"), fileName, m_index + 1, idleCount, workCount);
		return;
	}

	data->m_filePath.Format(_T("%s%s"), folderPath, fileName);
	RecyclableImage::Alloc_Full(data->m_image, image.GetSizeX(), image.GetSizeY());
	::memcpy(data->m_image.GetMem(), image.GetMem(), image.GetWidthBytes() * image.GetSizeY());

	m_queue.push(data);
	__super::Trigger();
}

SaveImageData* SaveImageQueue::Pop()
{
	CSingleLock lock(&m_cs, TRUE);

	if (m_queue.size() == 0) return nullptr;

	auto* data = m_queue.front();
	m_queue.pop();

	return data;
}

void SaveImageQueue::Release(SaveImageData* data)
{
	if (data == nullptr) return;

	CSingleLock lock(&m_cs, TRUE);

	m_idleQueue.push(data);
}

SaveImageData* SaveImageQueue::GetBuffer()
{
	if (m_idleQueue.size())
	{
		auto* data = m_idleQueue.front();
		m_idleQueue.pop();

		return data;
	}

	if (m_maximumSize != 0)
	{
		return nullptr;
	}

	return new SaveImageData;
}

bool SaveImageQueue::OnTrigger()
{
	bool working = false;
	while (auto* data = Pop())
	{
		int nSizeX = 0, nSizeY = 0;
		IPVM::Status status = data->Save(nSizeX, nSizeY);
		CString strCode = IPVM::ToString(status);

		if(status != IPVM::Status::success)
			m_loggerImageSaveFailed.Log(0, _T("Error occurred while saving the image [Error Code %s] [Image Size X - %d Y - %d] - %s"), strCode, nSizeX, nSizeY, data->m_filePath);

		Release(data);
		working = true;
	}

	if (working)
	{
		__super::Trigger();
	}

	return true;
}
