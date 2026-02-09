#pragma once

#include "IBCIDlg.h"
#include "SurfaceInspect.h"
#include "../Inspection/ResultInfo.h"

#include "Gadget/SimpleWorkerThread.h"
#include "..\SharedModule\VisionHostShare\ResultVision.h"

#include <afxmt.h>
#include <deque>

//class CMainDlg;
class CSyncboard;

class InlineResultCoupler : public IPVM::SimpleWorkerThread
{
public:
	InlineResultCoupler(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CSyncboard& syncIO, CIBCIDlg* parent);
	InlineResultCoupler(const InlineResultCoupler &rhs) = delete;
	virtual ~InlineResultCoupler();

	InlineResultCoupler &operator=(const InlineResultCoupler &rhs) = delete;
	long m_nIndexID[2];

	void ClearLot();
	void SetDownCellID(long downCellID);
	//void SetDownCellID_Surface(long downCellID);
    void Push1(const ResultMismatch& result);
    void Push2(const ResultMismatch& result);
    void Push3(const ResultMismatch& result, Share::ResultVision resultdata);
	//void Push(const SURFACE_RESULT_INFO& result);
	//void Push(const RESULT_INFO& result, const SURFACE_RESULT_INFO& result_surface);
	void PushCandidateCellID(const long cell_id);
	void SetErrorProofingCellCount(LPCTSTR requestGuid, const long cellCount);

protected:
	virtual bool OnTrigger() override;

	void CoupleDataCheck();
	//void CoupleDataCheck_Surface();

	struct ExtreaInfoForCellID
	{
		CString	m_errorProofingRequestGuid;
		long	m_errorProofingCellIndex;
	};

	void LogRemainCell(std::deque<RESULT_INFO>& resultinfoDeque, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID);
	//void LogRemainCell(std::deque<SURFACE_RESULT_INFO>& resultinfoDeque, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID);

	

	void SendDummy(long firstCellID, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID);
	void FindFirstMatchedID(std::deque<RESULT_INFO>& resultinfoDeque, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID);

	//void SendDummy_Surface(long firstCellID, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID);
	//void FindFirstMatchedID_Surface(std::deque<SURFACE_RESULT_INFO>& resultinfoDeque, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID);

	IPVM::LoggerInterface& m_loggerIO;
	IPVM::LoggerInterface& m_loggerResult;
	CSyncboard& m_syncIO;
	CIBCIDlg* m_parent;
	bool m_isFirstRealResult;
	//bool m_isFirstRealResult_Surface;
	bool m_procFirstDownCellID;
	//bool m_procFirstDownCellID_Surface;
	long m_FirstMatchedID;
	//long m_FirstMatchedID_Surface;
	long m_firstDownCellID;
	//long m_firstDownCellID_Surface;

	CCriticalSection m_sensorIDInputLock;
	//CCriticalSection m_sensorIDInputLock_Surface;
	std::deque<std::pair<long, ExtreaInfoForCellID>> m_candidateCellID;	// Candiate Cell ID
	//std::deque<std::pair<long, ExtreaInfoForCellID>> m_candidateCellID_Surface;	// Candiate Cell ID

	CString m_errorProofingRequestGuid;						// Interface에서 Error Proofing을 요청했던 명령의 GUID
	long	m_errorProofingCellIndex;						// Interface에서 다음 Cell부터 특정 갯수의 Cell을 Error Proofing하기를 요청하였다

	CCriticalSection m_resultInfoCs;
	//CCriticalSection m_resultInfoCs_Surface;
	//std::deque<RESULT_INFO> m_resultInfoDeque;

    std::deque<ResultMismatch> m_vecMismatchData1;
    std::deque<ResultMismatch> m_vecMismatchData2;
    Share::ResultVision m_VisionData;
    std::vector<Share::ResultVision> m_vecVisionData;
	//std::deque<SURFACE_RESULT_INFO> m_surfaceResultInfoDeque;

	bool m_bisFirstdata;
};
