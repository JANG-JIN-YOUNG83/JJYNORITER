#include "pch.h"
#include "TcpResultSender.h"
#include "SystemParameter.h"

#include "../SharedModule/CommonShare/PacketMemFile.h"
//#include "../SharedModule/VisionHostShare/HostCommSocket.h"
#include "../SharedModule/VisionHostShare/TCPMessageDef.h"
#include "../SharedModule/CommonShare/DefectDef.h"
#include "Base/LoggerInterface.h"
#include "Gadget/MessageSocket.h"

#include "../Inspection/basedef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TcpResultSender::TcpResultSender(
    AlarmCommSocket& messageSocket, IPVM::LoggerInterface& tcpLogger, long generationversion, int nCISInspType)
	: m_messageSocket(messageSocket)
	, m_tcpLogger(tcpLogger)
	, m_memFile(new CommonShare::PacketMemFile(30000))
	, m_generationversion(generationversion)
	, n_nCISInspType(nCISInspType)
{
	__super::BeginWorker();
}


TcpResultSender::~TcpResultSender()
{
	__super::EndWorker();

	delete m_memFile;
}

void TcpResultSender::Push(const Share::ResultVision &tcpResult)
{
	CSingleLock lock(&m_cs, TRUE);

	m_queue.push_back(tcpResult);
	__super::Trigger();
}

bool TcpResultSender::OnTrigger()
{
    m_cs.Lock();

    if (m_queue.size() == 0)
    {
        m_cs.Unlock();
        return true;
    }

    auto tcpResult = m_queue.front();
    m_queue.erase(m_queue.begin());
    m_cs.Unlock();

	BOOL res = m_messageSocket.SendVisionResultData(tcpResult, *m_memFile);
	if (res)
	{
        m_tcpLogger.Log(0, _T("Send Vision Result [Cell ID] %d"), tcpResult.m_nCellindex);
	}
    tcpResult.Clear();

    m_cs.Lock();
	if (m_queue.size())
	{
		__super::Trigger();
	}
    m_cs.Unlock();
	return true;
}