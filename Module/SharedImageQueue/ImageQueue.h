#pragma once

#ifdef __SHARED_IMAGE_QUEUE_CLASS_EXPORT__
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllexport)
#else
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllimport)
#endif

#include "Base.h"
#include "Image.h"

#include <memory>
#include <vector>

struct QueueMemoryData;

namespace SharedMemory
{
	class __SHARED_IMAGE_QUEUE_CLASS__ ImageQueue : public Base
	{
	public:
		ImageQueue(const long identifier);
		~ImageQueue();

		void Initialize(long imageSizeX, long imageSizeY, long size);
		void Terminate();

		void Reset();
		void Load(LPCTSTR targetFolder, bool virtualMode = false);
		void Save(LPCTSTR targetFolder, long saveMaxCount = -1);
		void Save_Partial(LPCTSTR targetFolder,long saveMaxCount,long PartialSaveIndex_Start, long PartialSaveIndex_End);

		void SetBusy();
		void SetIdle();
		void SetDataIndex(long headIndex, long dataCount);

		long GetCount() const;
		long GetPopIntervalTime_ms() const;
		long GetSize() const;

		bool IsPusherBusy() const;
		bool IsQueueBusy() const;
		bool Pop(IPVM::Image_8u_C1& o_image);
		void SetPopIntervalTime_ms(long time_ms);
		void GetThumbnailImage(long imageSizeY, IPVM::Image_8u_C1& image);
		void GetFrontMergeBufferImage(long maxImageCount, IPVM::Image_8u_C1& image);

		void SetAlarm(long nCode);
		long GetAlarm();
		std::vector<long> m_Xpos;

	private:
		void UpdateQueueBuffer();

		const long m_identifier;
		CString m_identifierKey;
		QueueMemoryData* m_data;
		long m_queueImageSize;
		std::vector<std::shared_ptr<Image>> m_queueImages;
	};
}
