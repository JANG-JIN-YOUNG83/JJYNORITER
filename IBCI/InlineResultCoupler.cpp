#include "pch.h"
#include "resource.h"
#include "InlineResultCoupler.h"
#include "IBCIDlg.h"
#include "SystemParameter.h"

#include "../SyncPci/Syncboard.h"

#include "Base/LoggerInterface.h"
#include "Gadget/TimeCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

InlineResultCoupler::InlineResultCoupler(IPVM::LoggerInterface& loggerIO, IPVM::LoggerInterface& loggerResult, CSyncboard& syncIO, CIBCIDlg* parent)
	: m_loggerIO(loggerIO)
	, m_loggerResult(loggerResult)
	, m_syncIO(syncIO)
	, m_parent(parent)
	, m_isFirstRealResult(false)
	, m_procFirstDownCellID(false)
	, m_firstDownCellID(-1)
	, m_errorProofingCellIndex(-1)
	, m_bisFirstdata(true)
	, m_FirstMatchedID(-1)
{
	__super::BeginWorker();
	m_nIndexID[0] = 0;
	m_nIndexID[1] = 0;
}

InlineResultCoupler::~InlineResultCoupler()
{
	__super::EndWorker();
}

void InlineResultCoupler::ClearLot()
{
	if (true)
	{
		CSingleLock lock(&m_sensorIDInputLock, TRUE);
		m_candidateCellID.clear();
		m_firstDownCellID = -1;
		m_FirstMatchedID = -1;
	}

	if (true)
	{
		CSingleLock lock(&m_resultInfoCs, TRUE);
        m_vecMismatchData1.clear();
        m_vecMismatchData2.clear();
	}

	m_procFirstDownCellID = false;
	m_bisFirstdata = true;
	m_isFirstRealResult = false;
}

void InlineResultCoupler::SetDownCellID(long downCellID)
{
	CSingleLock lock(&m_sensorIDInputLock, TRUE);

	if (m_firstDownCellID < 0)
	{
		m_firstDownCellID = downCellID;
	}
}

void InlineResultCoupler::Push1(const ResultMismatch& result)
{
	if (g_systemParameter.nPC_ID != 1)
	{
		ASSERT(!_T("Measure 검사용 PC가 아니면서 이함수를 쓰면 안될거 같다."));
		return;
	}

	ResultMismatch result_dst = result;

	CSingleLock lock(&m_resultInfoCs, TRUE);
    m_vecMismatchData1.push_back(result_dst);

	__super::Trigger();
}

void InlineResultCoupler::Push2(const ResultMismatch& result)
{
    if (g_systemParameter.nPC_ID != 1)
    {
        ASSERT(!_T("Measure 검사용 PC가 아니면서 이함수를 쓰면 안될거 같다."));
        return;
    }

    ResultMismatch result_dst = result;

    CSingleLock lock(&m_resultInfoCs, TRUE);
    m_vecMismatchData2.push_back(result_dst);

    __super::Trigger();
}


void InlineResultCoupler::Push3(const ResultMismatch& result, Share::ResultVision resultdata)
{
    if (g_systemParameter.nPC_ID != 1)
    {
        ASSERT(!_T("Measure 검사용 PC가 아니면서 이함수를 쓰면 안될거 같다."));
        return;
    }

    ResultMismatch result_dst = result;

    CSingleLock lock(&m_resultInfoCs, TRUE);
    m_vecMismatchData1.push_back(result_dst);
    m_VisionData = resultdata;
    m_loggerResult.Log(0, _T("%d Batch Thread Trigger"), resultdata.m_nCellindex);
    __super::Trigger();
}

void InlineResultCoupler::PushCandidateCellID(const long cell_id)
{
	{
		CSingleLock lock(&m_sensorIDInputLock, TRUE);
		m_candidateCellID.emplace_back(cell_id, ExtreaInfoForCellID{ m_errorProofingRequestGuid, m_errorProofingCellIndex });
	}

	
	if (m_errorProofingCellIndex >= 0)
	{
		m_errorProofingCellIndex--;
	}
}

void InlineResultCoupler::SetErrorProofingCellCount(LPCTSTR requestGuid, const long cellCount)
{

	//---------------------------------------------------------
	// 새로운 요청이 왔다.
	// 기존 Error Proofing 처리를 위해 대기중이던 모든 ID 후보를
	// Error Proofing을 위한 것이 아닌것으로 처리하자
	//---------------------------------------------------------
	{
		CSingleLock lock(&m_sensorIDInputLock, TRUE);

		for (auto& candidate : m_candidateCellID)
		{
			candidate.second.m_errorProofingCellIndex = -1;
		}
	}

	m_errorProofingRequestGuid = requestGuid;

	if (cellCount <= 0)
	{
		m_errorProofingCellIndex = -1;
	}
	else
	{
		m_errorProofingCellIndex = cellCount;
	}
}

bool InlineResultCoupler::OnTrigger() //여러번 들어올수있지 않을까?..
{
    m_loggerResult.Log(0, _T("%d Batch Thread Coupler Check()"), m_VisionData.m_nCellindex);
	CoupleDataCheck();
	return true;
}

void InlineResultCoupler::CoupleDataCheck()
{
    // find
    std::deque<ResultMismatch> resultinfoDeque1, resultinfoDeque2;
    Share::ResultVision shVisionData;

    {
        CSingleLock lock(&m_resultInfoCs, TRUE);

        resultinfoDeque1 = m_vecMismatchData1;
        resultinfoDeque2 = m_vecMismatchData2;
        shVisionData = m_VisionData;
    }
    //m_VisionData.Clear();
	// notpair
    if (g_systemParameter.bNotPair == 1 || g_systemParameter.bRollerTwoSide == 0)
    {
        if (resultinfoDeque1.size() <= 0)
            return;

        ResultMismatch stResAllMismatch;
        auto stResMismatch1 = resultinfoDeque1[resultinfoDeque1.size() - 1];
        //if (g_systemParameter.bDirection == 0) //top
        {
            stResAllMismatch.m_nCellIndex = stResMismatch1.m_nCellIndex;
            stResAllMismatch.m_vecResUpLane = stResMismatch1.m_vecResUpLane;
            stResAllMismatch.m_vecResDownLane = stResMismatch1.m_vecResDownLane;
            stResAllMismatch.m_vecResSpec = stResMismatch1.m_vecResSpec;
            stResAllMismatch.m_fTopFullLength = stResMismatch1.m_fBackFullLength;
            stResAllMismatch.m_fBackFullLength = stResMismatch1.m_fBackFullLength;
        }
        /*if(m_parent->HostSocketConnectCheck())
        {*/
        m_parent->ProcCoupleResult(stResAllMismatch, shVisionData);
        /*}
        else
        {
            m_parent->ProcCoupleResult(stResAllMismatch);

        }*/
        

        // erase
        {
            CSingleLock lock(&m_resultInfoCs, TRUE);
            m_loggerResult.Log(0, _T("Removed Cell %s [%d] CurProcessing Cell id[%d] "), _T("1"),
                resultinfoDeque1.size() - 1,
                stResMismatch1.m_nCellIndex);

            if (m_vecMismatchData1.size() != 0)
            {
                m_vecMismatchData1.erase(
                    m_vecMismatchData1.begin(), m_vecMismatchData1.begin() + resultinfoDeque1.size());
            }

        }
        //}} couple cell 제거
        return;
    }

    if (m_vecMismatchData1.size() == 0 || m_vecMismatchData2.size() == 0)
        return;

    // m_vecMismatchData2가 항상 먼저 들어오는 조건
    long nIndexDelete1 = 0;
    long nIndexDelete2 = 0;
    ResultMismatch stResMismatch1;
    ResultMismatch stResMismatch2;
    for (long v1 = 0; v1 < resultinfoDeque1.size(); v1++)
    {
        for (long v2 = 0; v2 < resultinfoDeque2.size(); v2++)
        {
            if (   resultinfoDeque1[v1].m_strLotID == resultinfoDeque2[v2].m_strLotID
				&& resultinfoDeque1[v1].m_nCellIndex == resultinfoDeque2[v2].m_nCellIndex)
            {
                nIndexDelete1 = v1;
                nIndexDelete2 = v2;
                stResMismatch1 = resultinfoDeque1[v1];
                stResMismatch2 = resultinfoDeque2[v2];
                goto MISMATCH_FOUND;
            }
        }
        if (v1 == resultinfoDeque1.size() - 1)
        {
            return;
        }
    }
MISMATCH_FOUND:;

    ResultMismatch stResAllMismatch;
    stResAllMismatch.m_nCellIndex = stResMismatch1.m_nCellIndex;
    if (g_systemParameter.bDirection == 0) //top
    {
        stResAllMismatch.m_vecResUpLane = stResMismatch1.m_vecResUpLane;
        stResAllMismatch.m_vecResDownLane = stResMismatch2.m_vecResDownLane;
        stResAllMismatch.m_vecResSpec = stResMismatch1.m_vecResSpec;
        stResAllMismatch.m_fTopFullLength = stResMismatch1.m_fBackFullLength;
        stResAllMismatch.m_fBackFullLength = stResMismatch2.m_fBackFullLength;
    }
    else //back
    {
        stResAllMismatch.m_vecResUpLane = stResMismatch2.m_vecResUpLane;
        stResAllMismatch.m_vecResDownLane = stResMismatch1.m_vecResDownLane;
        stResAllMismatch.m_vecResSpec = stResMismatch1.m_vecResSpec;
        stResAllMismatch.m_fTopFullLength = stResMismatch2.m_fTopFullLength;
        stResAllMismatch.m_fBackFullLength = stResMismatch1.m_fBackFullLength;
    }
    //m_parent->ProcCoupleResult(stResAllMismatch);
    //if (m_VisionData.m_nCellindex > -1)
    //{
    m_parent->ProcCoupleResult(stResAllMismatch, shVisionData);

    //}
	// erase
    {
        CSingleLock lock(&m_resultInfoCs, TRUE);
        CString logMismatch1;
        logMismatch1.Format(_T("Removed Cell %s [%d] CurProcessing Cell id[%d] "), _T("1"), nIndexDelete1, stResMismatch1.m_nCellIndex);
        for (long cnt = 0; cnt <= nIndexDelete1; cnt++)
        {
            logMismatch1.AppendFormat(_T(",[%d]"), m_vecMismatchData1[cnt].m_nCellIndex);
		}
        TRACE(logMismatch1);
        m_loggerResult.Log(0, logMismatch1);
        m_vecMismatchData1.erase(m_vecMismatchData1.begin() , m_vecMismatchData1.begin() + nIndexDelete1 + 1);
        m_loggerResult.Log(0, _T("Removed	Cell %s [%d] CurProcessing Cell id[%d] "), _T("2"), nIndexDelete2, stResMismatch2.m_nCellIndex);
        m_vecMismatchData2.erase(m_vecMismatchData2.begin() , m_vecMismatchData2.begin() + nIndexDelete2 + 1);
        m_loggerResult.Log(0, _T("Removed Cell %s [%d] CurProcessing Cell id[%d] "), _T("2"), nIndexDelete2, stResMismatch2.m_nCellIndex);
    }
    //}} couple cell 제거
}

void InlineResultCoupler::LogRemainCell(std::deque<RESULT_INFO>& resultinfoDeque, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID)
{
	//{{ remain count
	int upCount = 0;
	int upFirstID = -1, upLastID = -1;
	int downCount = 0;
	int downFirstID = -1, downLastID = -1;
	for (long i = 0; i < resultinfoDeque.size(); i++)
	{
		if (resultinfoDeque[i].isUp)
		{
			if (upCount == 0)
			{
				upFirstID = upLastID = resultinfoDeque[i].cellID;
			}
			upLastID = resultinfoDeque[i].cellID;
			upCount++;
		}
		else
		{
			if (downCount == 0)
			{
				downFirstID = downLastID = resultinfoDeque[i].cellID;
			}
			downLastID = resultinfoDeque[i].cellID;
			downCount++;
		}
	}
	int nFirstCellID = -1;
	CString text(_T("candidate : "));
	for (int i = 0; i < candidateCellID.size(); i++)
	{
		CString temp;
		temp.Format(_T("%d, "), candidateCellID[i].first);
		text += temp;
		if (0 == i)
			nFirstCellID = candidateCellID[i].first;
	}

	CString log1, log2;

	if (upCount)
	{
		log1.Format(_T("[Measure]Remain Up Count : %d (%d ~ %d),"), upCount, upFirstID, upLastID);
	}
	else
	{
		log1 = _T("[Measure]Remain Up Count : 0,");
	}
	if (downCount)
	{
		log2.Format(_T("Down Count : %d (%d ~ %d), "), downCount, downFirstID, downLastID);
	}
	else
	{
		log2 = _T("Down Count : 0,");
	}
	CString strError(_T(""));
	if (-1 != downFirstID && -1 != upFirstID && -1 != nFirstCellID && true == m_bisFirstdata)
	{
		m_bisFirstdata = false;
		if (downFirstID != upFirstID || upFirstID != nFirstCellID || nFirstCellID != downFirstID) {
			CString str;
			str.Format(_T("CandidateID:%d, UPID:%d, DOWNID:%d "), nFirstCellID,upFirstID, downFirstID);
			//m_parent->SetAlarm(164, Share::ALARM_LEVEL::red, _T("Result and Cell are not matched"), _T(""), str, _T(""));
			strError.Format(_T("[%s]"),str); 
		}
	}
	m_loggerResult.Log(0, strError+log1 + log2 + text);
	//}} remain count
}

void InlineResultCoupler::FindFirstMatchedID(std::deque<RESULT_INFO>& resultinfoDeque, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID)
{
	if (m_FirstMatchedID != -1) return;	// First Cell ID에 대한 처리를 이미 마쳤다
	if(resultinfoDeque.size()<2) return; // 후보가 아직 들어온게 없다.

	int CellCount = 64; //pow(2, g_systemParameter.nCellIDBit);
	std::vector<int> vecCellICandi_up; //up 체크 백터
	std::vector<int> vecCellICandi_down;//down 체크 벡터
	vecCellICandi_up.resize(CellCount, 0);
	vecCellICandi_down.resize(CellCount, 0);

	int MatchedCellID = -1;
	for (int i = 0; i < resultinfoDeque.size(); i++)
	{
		int cellID = resultinfoDeque[i].cellID;
		bool bisup = resultinfoDeque[i].isUp;

		if (true == bisup)
			vecCellICandi_up[cellID]++;
		else
			vecCellICandi_down[cellID]++;

		if (1 <= vecCellICandi_down[cellID] && 1 <= vecCellICandi_up[cellID])//같은 cellid가 이미 있다면 매칭 됐다고 판단<순서대로 들어온다고 가정>.
		{
			MatchedCellID = cellID;
			break;
		}
	}
	if (-1 == MatchedCellID) return; // 아직 상하부 매칭이 되는 후보가 들어오지 않음.
	for (int i = 0; i < candidateCellID.size(); i++)
	{
		if (MatchedCellID == candidateCellID[i].first)
		{
			m_FirstMatchedID = MatchedCellID;
			m_loggerResult.Log(0,_T("[Fist Matched Cell ID] CellID : %d"), m_FirstMatchedID);
			return;
		}
	}
	
}
void InlineResultCoupler::SendDummy(long firstCellID, std::deque<std::pair<long, ExtreaInfoForCellID>>& candidateCellID)
{
	if (m_procFirstDownCellID) return;	// First Cell ID에 대한 처리를 이미 마쳤다
	if (firstCellID < 0) return;		// 아직 First Cell ID가 들어오지 않았다

	while (candidateCellID.size())
	{
		if (candidateCellID[0].first == firstCellID)
		{
			// First Cell ID가 첫번째 Sensor ID가 되었다
			// 이제 부터 정상적으로 수집을 시작하자

			m_procFirstDownCellID = true;
			return;
		}

		m_loggerIO.Log(0, _T("dummy[%d]"), candidateCellID[0].first);
		//m_syncIO.SetSignal_NotchingResult(candidateCellID[0].first, FALSE);
		//m_syncIO.SetSignal_ElectrodeResult(FALSE, 1);

		m_syncIO.SetSignal_ResultNew(candidateCellID[0].first, FALSE, FALSE, 1);

		Sleep(10);

		candidateCellID.pop_front();

		CSingleLock lock(&m_sensorIDInputLock, TRUE);
		if (m_candidateCellID.size() > 0)
		{
			m_candidateCellID.pop_front();
		}
	}
}
