#pragma once

#ifdef __SHARED_IMAGE_QUEUE_CLASS_EXPORT__
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllexport)
#else
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllimport)
#endif

#include "Base.h"
#include "Image.h"

#include <afxmt.h>
#include <queue>
#include <memory>
#include <vector>

struct QueueMemoryData;

namespace SharedMemory
{
	class __SHARED_IMAGE_QUEUE_CLASS__ ImageQueuePusher : public Base
	{
	public:
		ImageQueuePusher(const long identifier);
		~ImageQueuePusher();

		bool Push(const IPVM::Image_8u_C1& image);
		void Save(LPCTSTR targetFolder, long saveMaxCount = -1);
		void SetPusherBusy();
		void SetPusherIdle();
		bool IsPusherBusy() const;
		void UpdateImageQueue();

		void SetAlarm(long nCode);

	private:
		void LazyUpdateImageQueue();

		const long m_identifier;
		CString m_identifierKey;
		QueueMemoryData* m_data;

		long m_queueImageSizeX;
		long m_queueImageSizeY;
		long m_queueImageCount;

		std::vector<std::shared_ptr<Image>> m_queueImages;

		CEvent m_eventThreadStop;
		CEvent m_eventThreadRun_Garbage;
		CEvent m_eventThreadRun_Make;

		HANDLE m_threadGarbage;
		HANDLE m_threadMake;

		CCriticalSection m_csGarbage;
		std::queue<std::shared_ptr<SharedMemory::Image>> m_garbageImages;

		static UINT ThreadGarbage(LPVOID userData);
		static UINT ThreadMake(LPVOID userData);
		UINT ThreadGarbage();
		UINT ThreadMake();
	};
}
