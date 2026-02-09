#pragma once

#include "Gadget/SimpleWorkerThread.h"

class MemoryChangeMonitor : public IPVM::SimpleWorkerThread
{
public:
	MemoryChangeMonitor() = delete;
	MemoryChangeMonitor(const MemoryChangeMonitor &rhs) = delete;
	MemoryChangeMonitor(unsigned char *address, size_t bytes);
	virtual ~MemoryChangeMonitor();

	MemoryChangeMonitor &operator=(const MemoryChangeMonitor &rhs) = delete;

protected:
	virtual bool OnTrigger() override;

private:
	bool m_stop;

	unsigned char *m_compare;
	unsigned char *m_address;
	size_t m_bytes;
};

