#include "pch.h"
#include "RecyclableImage.h"

#include <deque>
#include <map>

#include <Base/basedef.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class RecyclableAllocator
{
public:
	RecyclableAllocator()
	{
	}

	~RecyclableAllocator()
	{
		for (auto& buffer : m_buffers)
		{
			delete []buffer.first;
		}

		for (auto& buffer : m_pending)
		{
			delete[]buffer.first;
		}
	}

	static void *Alloc(void *userData, size_t bytes)
	{
		return ((RecyclableAllocator *)userData)->Alloc(bytes);
	}

	static void Free(void *userData, void *mem)
	{
		((RecyclableAllocator *)userData)->Free(mem);
	}

private:
	CCriticalSection m_cs;
	//std::deque<std::pair<BYTE*, size_t>> m_buffers;
    std::vector<std::pair<BYTE*, size_t>> m_buffers;
	std::map<BYTE*, size_t> m_pending;

	void *Alloc(size_t bytes)
	{
		CSingleLock lock(&m_cs, TRUE);
		if (m_buffers.size() == 0)
		{
			auto buffer = new BYTE[bytes];
			if (buffer == nullptr)
			{
				// 할당실패
				return nullptr;
			}

			m_buffers.emplace_back(buffer, bytes);
		}

		auto bufferData = m_buffers.front();
		//m_buffers.pop_front();
        m_buffers.erase(m_buffers.begin());

		if (bufferData.second < bytes)
		{
			// 메모리크기가 작으면 새로 할당한다
			delete []bufferData.first;

			bufferData.first = new BYTE[bytes];
			bufferData.second = bytes;

			if (bufferData.first == nullptr)
			{
				// 할당실패
				return nullptr;
			}

		}

		m_pending[bufferData.first] = bufferData.second;

		return bufferData.first;
	}

	void Free(void *mem)
	{
		CSingleLock lock(&m_cs, TRUE);
		auto itPending = m_pending.find((BYTE *)mem);
		if (itPending == m_pending.end())
		{
			ASSERT(!_T("잉? 어디서 할당 받은겨?"));
			return;
		}

		auto* buffer = itPending->first;
		auto size = itPending->second;

		m_pending.erase(itPending);
		m_buffers.emplace_back(buffer, size);
	}
};

RecyclableAllocator g_allocChannel;
RecyclableAllocator g_allocFull;

bool RecyclableImage::Alloc_Channel(IPVM::Image_8u_C1& image, long sizeX, long sizeY)
{
	if (image.Create(sizeX, sizeY, &g_allocChannel, RecyclableAllocator::Alloc, RecyclableAllocator::Free) != IPVM::Status::success)
	{
		return false;
	}

	return true;
}

bool RecyclableImage::Alloc_Full(IPVM::Image_8u_C1& image, long sizeX, long sizeY)
{
	if (image.Create(sizeX, sizeY, &g_allocFull, RecyclableAllocator::Alloc, RecyclableAllocator::Free) != IPVM::Status::success)
	{
		return false;
	}

	return true;
}
