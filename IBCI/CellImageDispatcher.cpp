#include "pch.h"
#include "CellImageDispatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CellImageDispatcher::CellImageDispatcher()
	: m_funcCallback(nullptr)
	, m_dataCallback(nullptr)
{
}

CellImageDispatcher::~CellImageDispatcher()
{
}

void CellImageDispatcher::RegisterCallbackOnCellImageDispatched(CALLBACK_ON_CELL_IMAGE_DISPATHCED callbackFunc, void* callbackData)
{
	m_funcCallback = callbackFunc;
	m_dataCallback = callbackData;
}

void CellImageDispatcher::Start()
{
	__super::BeginWorker();
	__super::Trigger();
}

void CellImageDispatcher::Stop()
{
	__super::EndWorker();
}

void CellImageDispatcher::ClearLot()
{
	CSingleLock lock(&m_inspectionInputDataSetLock, TRUE);
	m_vecInspectionInputDataSet.clear();
}

void CellImageDispatcher::Push(InspectionInputData& data)
{
	CSingleLock lock(&m_inspectionInputDataSetLock, TRUE);
	m_vecInspectionInputDataSet.push_back(data);

	__super::Trigger();
}

bool CellImageDispatcher::OnTrigger()
{
	InspectionInputData inputData;
	bool nextDataExist = false;
	if (true)
	{
		CSingleLock lock(&m_inspectionInputDataSetLock, TRUE);
		if (m_vecInspectionInputDataSet.size() == 0) 
			return true;

		inputData = m_vecInspectionInputDataSet.front();
		//m_vecInspectionInputDataSet.pop_front();
        m_vecInspectionInputDataSet.erase(m_vecInspectionInputDataSet.begin());

		nextDataExist = (m_vecInspectionInputDataSet.size() > 0);
	}

	if (m_funcCallback) 
		m_funcCallback(m_dataCallback, inputData);
	if (nextDataExist)
		__super::Trigger();

	return true;
}
