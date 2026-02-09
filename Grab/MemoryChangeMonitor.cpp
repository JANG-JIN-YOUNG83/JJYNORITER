#include "stdafx.h"
#include "MemoryChangeMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MemoryChangeMonitor::MemoryChangeMonitor(unsigned char *address, size_t bytes)
	: m_compare(new unsigned char [bytes])
	, m_address(address)
	, m_bytes(bytes)
	, m_stop(false)
{
	::memcpy(m_compare, m_address, bytes);

	__super::BeginWorker();

	__super::Trigger();
}


MemoryChangeMonitor::~MemoryChangeMonitor()
{
	m_stop = true;

	__super::EndWorker();

	delete[] m_compare;
}

bool MemoryChangeMonitor::OnTrigger()
{
	while (m_stop == false)
	{
		for (int i = 0; i < m_bytes; i++)
		{
			if (m_compare[i] != m_address[i])
			{
				::memset(nullptr, 0, 1);
			}
		}
	}

	return false;
}