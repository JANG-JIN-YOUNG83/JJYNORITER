#include "stdafx.h"
#include "Accessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

Accessor* Accessor::m_instance = nullptr;

Accessor::Accessor(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
	: m_root(logger, loggerResult)
	, m_isMSA_Mode(false)
	, m_strPrevJobFolder(_T(""))
	, m_strPrevImageFolder(_T(""))
{
}

Accessor::~Accessor()
{
}

Accessor& Accessor::Get()
{
	return *m_instance;
}

Root& Accessor::GetRoot()
{
	return Get().m_root;
}

void Accessor::DelInstance()
{
	delete m_instance;
	m_instance = nullptr;
}

void Accessor::InitInstance(IPVM::LoggerInterface& logger, IPVM::LoggerInterface& loggerResult)
{
	if (m_instance == nullptr)
	{
		m_instance = new Accessor(logger, loggerResult);
	}
}
