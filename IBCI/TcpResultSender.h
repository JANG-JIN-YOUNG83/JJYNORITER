#pragma once

#include "Gadget/SimpleWorkerThread.h"
#include "../SharedModule/VisionHostShare/ResultVision.h"
#include "AlarmCommSocket.h"
class PacketMemFile;

class TcpResultSender : public IPVM::SimpleWorkerThread
{
public:
	TcpResultSender() = delete;
	TcpResultSender(const TcpResultSender &rhs) = delete;
	TcpResultSender(AlarmCommSocket &messageSocket, IPVM::LoggerInterface &tcpLogger, long generationversion = Share::GenerationVersion::Original, int nCISInspType = 0);
	virtual ~TcpResultSender();

	TcpResultSender &operator=(const TcpResultSender &rhs) = delete;

public:
	void Push(const Share::ResultVision &tcpResult);
   

protected:
	virtual bool OnTrigger() override;

private:
    AlarmCommSocket& m_messageSocket;
	IPVM::LoggerInterface &m_tcpLogger;

	CommonShare::PacketMemFile *m_memFile;

	CCriticalSection m_cs;
	std::deque<Share::ResultVision> m_queue;
	long m_generationversion;
	int n_nCISInspType;
};

