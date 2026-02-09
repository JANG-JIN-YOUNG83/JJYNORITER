#pragma once
#include "InspectionInputData.h"
#include <deque>

#include "Gadget/SimpleWorkerThread.h"

typedef void(*CALLBACK_ON_CELL_IMAGE_DISPATHCED)(void* callbackData, InspectionInputData& inputData);

class CellImageDispatcher : public IPVM::SimpleWorkerThread
{
public:
	CellImageDispatcher();
	~CellImageDispatcher();

	void RegisterCallbackOnCellImageDispatched(CALLBACK_ON_CELL_IMAGE_DISPATHCED callbackFunc, void* callbackData);
	void Start();
	void Stop();

	void ClearLot();

	void Push(InspectionInputData& data);

private:
	virtual bool OnTrigger() override;

	CCriticalSection m_inspectionInputDataSetLock;
	//std::deque<InspectionInputData> m_vecInspectionInputDataSet;
    std::vector<InspectionInputData> m_vecInspectionInputDataSet;


	CALLBACK_ON_CELL_IMAGE_DISPATHCED m_funcCallback;
	void* m_dataCallback;
};
