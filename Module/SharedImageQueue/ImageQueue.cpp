#include "stdafx.h"
#include "ImageQueue.h"
#include "QueueMemoryData.h"

#include "Base/basedef.h"
#include "Algorithm/ImageProcessing.h"
#include "Types/Image_8u_C1.h"

#include <afxmt.h>
#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SharedMemory;

ImageQueue::ImageQueue(const long identifier)
	: Base(sizeof(QueueMemoryData), _T("SharedMemory::ImageQueue"), identifier)
	, m_identifier(identifier)
	, m_queueImageSize(0)
{
	m_identifierKey.Format(_T("SharedMemory::ImageQueue%d"), m_identifier);
	m_data = (QueueMemoryData *)m_sharedMemoryPointer;

	CMutex mutex(TRUE, m_identifierKey);

	auto oldBusy_queue = m_data->m_isBusy_Queue;
	m_data->m_isBusy_Queue = true;

	UpdateQueueBuffer();

	m_data->m_isBusy_Queue = oldBusy_queue;
	m_data->m_AlarmSave_Queue = -1;
}

ImageQueue::~ImageQueue()
{
}

void ImageQueue::Initialize(long imageSizeX, long imageSizeY, long size)
{
	CMutex mutex(TRUE, m_identifierKey);

	auto oldBusy_queue = m_data->m_isBusy_Queue;
	m_data->m_isBusy_Queue = true;

	bool changed = false;
	if (m_data->m_imageSizeX != imageSizeX ||
		m_data->m_imageSizeY != imageSizeY)
	{
		m_queueImages.clear();
		m_data->m_imageSizeX = imageSizeX;
		m_data->m_imageSizeY = imageSizeY;
		changed = true;
	}

	if (m_queueImages.size() != size)
	{
		m_queueImages.clear();
		m_data->m_queueSize = size;
		changed = true;
	}

	if (changed)
	{
		UpdateQueueBuffer();

		// Queue Size가 바뀌면 Queue Index가 무효화 될 수 있으므로 클리어 시킨다
		m_data->m_headIndex = m_data->m_dataCount = 0;
	}

	m_data->m_isBusy_Queue = oldBusy_queue;

	m_data->m_AlarmSave_Queue = -1;
}

void ImageQueue::Terminate()
{
	CMutex mutex(TRUE, m_identifierKey);

	if (m_data->m_isBusy_Pusher)
	{
		// Queue 작업중이라 변경불가능
		return;
	}

	auto oldBusy_queue = m_data->m_isBusy_Queue;

	m_data->m_isBusy_Queue = true;
	m_data->m_headIndex = 0;
	m_data->m_dataCount = 0;
	m_data->m_queueSize = 0;

	m_data->m_isBusy_Queue = oldBusy_queue;
}

void ImageQueue::Reset()
{
	CMutex mutex(TRUE, m_identifierKey);

	m_data->m_headIndex = m_data->m_dataCount = 0;

	m_data->m_AlarmSave_Queue = -1;
}

void ImageQueue::Load(LPCTSTR targetFolder, bool virtualMode)
{
	Reset();

	std::set<CString> fileList;
	CFileFind ff;
	CString filter;
	filter.Format(_T("%s\\*.*"), targetFolder);
	auto foundFile = ff.FindFile(filter);

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDirectory()) continue;
		fileList.insert(ff.GetFilePath());
	}

	if (true)
	{
		CMutex mutex(TRUE, m_identifierKey);

		if (!m_data->m_isBusy_Queue)
		{
			// Run 중이다
			return;
		}
	}

	if (virtualMode)
	{
		IPVM::Image_8u_C1 accImage;

		for (auto& filePath : fileList)
		{
			if (m_data->m_dataCount >= m_data->m_queueSize)
			{
				// 최대 Queue Size만큼만 로드한다
				break;
			}
			IPVM::Image_8u_C1 loadedImage;
			if (loadedImage.LoadFrom(filePath) != IPVM::Status::success)
			{
				continue;
			}

			if (true)
			{
				// 이미지 누적
				if (accImage.GetSizeX() != 0 && accImage.GetSizeX() != loadedImage.GetSizeX())
				{
					ASSERT(!_T("이미지 크기가 다른게 들어왔네.."));
					break;
				}

				IPVM::Image_8u_C1 temp;
				temp.Create(loadedImage.GetSizeX(), accImage.GetSizeY() + loadedImage.GetSizeY());

				// 기존이미지를 복사
				IPVM::ImageProcessing::Copy(accImage, IPVM::Rect(accImage), IPVM::Rect(accImage), temp);

				// Load된 이미지를 맨 끝에 붙인다
				IPVM::Rect dstRoi(loadedImage);
				dstRoi.OffsetRect(0, accImage.GetSizeY());
				IPVM::ImageProcessing::Copy(loadedImage, IPVM::Rect(loadedImage), dstRoi, temp);

				accImage = temp;
			}

			while (m_data->m_imageSizeY <= accImage.GetSizeY())
			{
				if (m_data->m_dataCount >= m_data->m_queueSize)
				{
					// 최대 Queue Size만큼만 로드한다
					break;
				}

				// 앞에서부터 이미지 가져감
				IPVM::Image_8u_C1 image;
				image.Create(accImage.GetSizeX(), m_data->m_imageSizeY);

				IPVM::ImageProcessing::Copy(accImage, IPVM::Rect(image), IPVM::Rect(image), image);

				auto index = (m_data->m_headIndex + m_data->m_dataCount) % m_data->m_queueSize;
				m_queueImages[index]->Set(image);

				m_data->m_dataCount++;

				// 뒷이미지만 복사해서 남긴다
				long tailSize = accImage.GetSizeY() - image.GetSizeY();

				IPVM::Image_8u_C1 temp;
				if (tailSize > 0)
				{
					temp.Create(accImage.GetSizeX(), tailSize);

					IPVM::Rect srcRoi(temp);
					srcRoi.OffsetRect(0, image.GetSizeY());
					IPVM::ImageProcessing::Copy(accImage, srcRoi, IPVM::Rect(temp), temp);
					accImage = temp;
				}
				else
				{
					accImage.Free();
				}
			}
		}
	}
	else
	{
		for (auto& filePath : fileList)
		{
			if (m_data->m_dataCount >= m_data->m_queueSize)
			{
				// 최대 Queue Size만큼만 로드한다
				break;
			}
			IPVM::Image_8u_C1 image;
			if (image.LoadFrom(filePath) != IPVM::Status::success)
			{
				continue;
			}

			auto index = (m_data->m_headIndex + m_data->m_dataCount) % m_data->m_queueSize;
			m_queueImages[index]->Set(image);

			m_data->m_dataCount++;
		}
	}
}

void ImageQueue::Save(LPCTSTR targetFolder, long saveMaxCount)
{
	long dataIndex = 0;
	long dataCount = 0;
	long queueSize = 0;

	if (true)
	{
		CMutex mutex(TRUE, m_identifierKey);

		// Save는 오래 걸릴 수 있으므로 Lock을 걸어놓고 진행하지는 않는다.
		// 대신 Save하는 동안은 안전을 위해 Queue에 추가 Image를 쌓는 작업을 진행하지 않게 한다

		if (!m_data->m_isBusy_Queue)
		{
			// Run 중이다
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

void ImageQueue::Save_Partial(LPCTSTR targetFolder, long saveMaxCount, long PartialSaveIndex_Start, long PartialSaveIndex_End)
{
	long dataIndex = 0;
	long dataCount = 0;
	long queueSize = 0;

	if (true)
	{
		CMutex mutex(TRUE, m_identifierKey);

		// Save는 오래 걸릴 수 있으므로 Lock을 걸어놓고 진행하지는 않는다.
		// 대신 Save하는 동안은 안전을 위해 Queue에 추가 Image를 쌓는 작업을 진행하지 않게 한다

		if (!m_data->m_isBusy_Queue)
		{
			// Run 중이다
			return;
		}

		dataIndex = m_data->m_headIndex;
		dataCount = m_data->m_dataCount;
		queueSize = m_data->m_queueSize;
	}

	long countIndex = 0;
	long IndexCount = abs(PartialSaveIndex_Start - PartialSaveIndex_End);
	long Start = PartialSaveIndex_Start;
	long End = PartialSaveIndex_End;
	if (Start > End) {
		Start = PartialSaveIndex_End;
		End = PartialSaveIndex_Start;
	}
	long imageSizeX = m_data->m_imageSizeX;
	long imageSizeY = m_data->m_imageSizeY;
	IPVM::Image_8u_C1 image;
	image.Create(imageSizeX , imageSizeY * IndexCount);

	IPVM::Image_8u_C1 oringinalImage;
	long offset = 0;
	for (int dataIndex = Start; dataIndex <= End; dataIndex++)
	{
		m_queueImages[dataIndex]->Get(oringinalImage);

		auto sRoi = IPVM::Rect(oringinalImage);
		auto tRoi = sRoi;
		tRoi.OffsetRect(0, imageSizeY * offset);

		IPVM::ImageProcessing::Copy(oringinalImage, sRoi, tRoi, image);
		offset++;
	}


	CString filePath;
	filePath.Format(_T("%s\\PARTIALIMAGE.bmp"), targetFolder);

	image.SaveBmp(filePath);

}

void ImageQueue::SetBusy()
{
	CMutex mutex(TRUE, m_identifierKey);
	m_data->m_isBusy_Queue = true;
}

void ImageQueue::SetIdle()
{
	CMutex mutex(TRUE, m_identifierKey);
	m_data->m_isBusy_Queue = false;
}

void ImageQueue::SetDataIndex(long headIndex, long dataCount)
{
	// 강제로 Data Count를 Set해주는 함수다.
	// 실제 상황에서는 쓸 일이 없고, Queue 이미 이미지가 들어있었는데
	// 모두 Pop하여 Data Count를 다시 강제로 셋한후 돌리고 싶을때 시뮬레이션 용도로만 사용된다

	CMutex mutex(TRUE, m_identifierKey);
	m_data->m_headIndex = headIndex;
	m_data->m_dataCount = min(m_data->m_queueSize, dataCount);
}

long ImageQueue::GetCount() const
{
	CMutex mutex(TRUE, m_identifierKey);

	return m_data->m_dataCount;
}

long ImageQueue::GetPopIntervalTime_ms() const
{
	CMutex mutex(TRUE, m_identifierKey);

	return m_data->m_popIntervalTime_ms;
}

long ImageQueue::GetSize() const
{
	CMutex mutex(TRUE, m_identifierKey);

	return m_data->m_queueSize;
}

bool ImageQueue::IsPusherBusy() const
{
	return m_data->m_isBusy_Pusher;
}

bool ImageQueue::IsQueueBusy() const
{
	return m_data->m_isBusy_Queue;
}

bool ImageQueue::Pop(IPVM::Image_8u_C1& o_image)
{
	CMutex mutex(TRUE, m_identifierKey);

	if (m_data->m_dataCount <= 0 || m_data->m_isBusy_Queue)
	{
		return false;
	}

	UpdateQueueBuffer();
	m_queueImages[m_data->m_headIndex]->Get(o_image);

	m_data->m_headIndex = (m_data->m_headIndex + 1) % m_data->m_queueSize;
	m_data->m_dataCount--;

	return true;
}

void ImageQueue::SetPopIntervalTime_ms(long time_ms)
{
	CMutex mutex(TRUE, m_identifierKey);

	m_data->m_popIntervalTime_ms = time_ms;
}

void ImageQueue::GetThumbnailImage(long imageSizeY, IPVM::Image_8u_C1& image)
{
	long dataIndex = 0;
	long dataCount = 0;
	long queueSize = 0;

	if (true)
	{
		CMutex mutex(TRUE, m_identifierKey);

		// Save는 오래 걸릴 수 있으므로 Lock을 걸어놓고 진행하지는 않는다.
		// 대신 Save하는 동안은 안전을 위해 Queue에 추가 Image를 쌓는 작업을 진행하지 않게 한다

		if (!m_data->m_isBusy_Queue)
		{
			// Run 중이다
			image.Free();
			return;
		}

		dataIndex = m_data->m_headIndex;
		dataCount = m_data->m_dataCount;
		queueSize = m_data->m_queueSize;
	}

	long oneImageSizeX = m_data->m_imageSizeY * imageSizeY / m_data->m_imageSizeX;
	long seperateWith = 20;

	dataIndex = (dataIndex + dataCount - 1) % queueSize;

	long fullImageSizeX = (oneImageSizeX + 1) * dataCount + ((dataCount - 1) / 10) * seperateWith;
	image.Create(fullImageSizeX, imageSizeY);

	IPVM::ImageProcessing::Fill(IPVM::Rect(image), 0, image);

	IPVM::Image_8u_C1 oneImage;
	IPVM::Image_8u_C1 oneRotatedImage;

	oneImage.Create(imageSizeY, oneImageSizeX);
	oneRotatedImage.Create(oneImageSizeX, imageSizeY);

	long x = fullImageSizeX - oneImageSizeX;
	long countIndex = dataCount - 1;
	m_Xpos.clear();
	while (countIndex >= 0)
	{
		IPVM::Image_8u_C1 oringinalImage;
		m_queueImages[dataIndex]->Get(oringinalImage);

		IPVM::ImageProcessing::ResizeDown_Super(oringinalImage, oneImage);
		IPVM::ImageProcessing::Rotate_ccw90(oneImage, oneRotatedImage);

		auto sRoi = IPVM::Rect(oneRotatedImage);
		auto tRoi = sRoi;
		tRoi.OffsetRect(x, 0);

		IPVM::ImageProcessing::Copy(oneRotatedImage, sRoi, tRoi, image);

		m_Xpos.push_back(x);
		x -= (oneImageSizeX + 1);

		dataIndex = (dataIndex + queueSize - 1) % queueSize;
		countIndex--;

		if ((dataCount - countIndex - 1) % 10 == 0) x -= seperateWith;
	}
}

void ImageQueue::GetFrontMergeBufferImage(long maxImageCount, IPVM::Image_8u_C1& image)
{
	long dataIndex = 0;
	long dataCount = 0;
	long queueSize = 0;

	if (true)
	{
		CMutex mutex(TRUE, m_identifierKey);

		// Save는 오래 걸릴 수 있으므로 Lock을 걸어놓고 진행하지는 않는다.
		// 대신 Save하는 동안은 안전을 위해 Queue에 추가 Image를 쌓는 작업을 진행하지 않게 한다

		if (!m_data->m_isBusy_Queue)
		{
			// Run 중이다
			image.Free();
			return;
		}

		dataIndex = m_data->m_headIndex;
		dataCount = max(0, min(maxImageCount, m_data->m_dataCount));
		queueSize = m_data->m_queueSize;
	}

	long oneImageSizeX = m_data->m_imageSizeY;
	long oneImageSizeY = m_data->m_imageSizeX;

	dataIndex = (dataIndex + dataCount - 1) % queueSize;

	long fullImageSizeX = oneImageSizeX * dataCount;
	image.Create(fullImageSizeX, oneImageSizeY);

	IPVM::ImageProcessing::Fill(IPVM::Rect(image), 0, image);

	IPVM::Image_8u_C1 oneImage;
	IPVM::Image_8u_C1 oneRotatedImage;

	oneRotatedImage.Create(oneImageSizeX, oneImageSizeY);

	long x = fullImageSizeX - oneImageSizeX;
	long countIndex = dataCount - 1;
	while (countIndex >= 0)
	{
		m_queueImages[dataIndex]->Get(oneImage);

		IPVM::ImageProcessing::Rotate_ccw90(oneImage, oneRotatedImage);

		auto sRoi = IPVM::Rect(oneRotatedImage);
		auto tRoi = sRoi;
		tRoi.OffsetRect(x, 0);

		IPVM::ImageProcessing::Copy(oneRotatedImage, sRoi, tRoi, image);

		x -= oneImageSizeX;

		dataIndex = (dataIndex + queueSize - 1) % queueSize;
		countIndex--;
	}
}

void ImageQueue::SetAlarm(long nCode)
{
	CMutex mutex2(true, 0);
	m_data->m_AlarmSave_Queue = nCode;
}

long ImageQueue::GetAlarm()
{
	CMutex mutex2(true, 0);
	return m_data->m_AlarmSave_Queue;
}

void ImageQueue::UpdateQueueBuffer()
{
	long bufferSize = m_data->m_imageSizeX * m_data->m_imageSizeY;

	if (m_queueImages.size() == m_data->m_queueSize &&
		m_queueImageSize == bufferSize)
	{
		return;
	}

	m_queueImages.clear();
	m_queueImageSize = bufferSize;

	while (m_data->m_queueSize > m_queueImages.size())
	{
		m_queueImages.push_back(std::make_shared<Image>(m_identifier, (long)m_queueImages.size(), bufferSize));
	}
}
