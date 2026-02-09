#pragma once

#ifdef __SHARED_IMAGE_QUEUE_CLASS_EXPORT__
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllexport)
#else
#define __SHARED_IMAGE_QUEUE_CLASS__ __declspec(dllimport)
#endif

namespace SharedMemory
{
	class __SHARED_IMAGE_QUEUE_CLASS__ Base
	{
	public:
		Base() = delete;
		Base(const Base &obj) = delete;
		Base(const long bytes, LPCTSTR name, const long identifier);
		Base(const long bytes, LPCTSTR name, const long identifier1, const long identifier2);
		~Base();

		Base &operator=(const Base &obj) = delete;

	protected:
		HANDLE m_shardMemoryHandle;
		void *m_sharedMemoryPointer;
	};
}
