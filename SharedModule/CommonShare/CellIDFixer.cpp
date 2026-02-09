#include "stdafx.h"
#include "CellIDFixer.h"

CellIDFixer::CellIDFixer()
{
}


CellIDFixer::~CellIDFixer()
{
}

void CellIDFixer::Init(int nErrorCount)
{
	m_nErrorCount = nErrorCount;
	m_nFixCount = 0;
	m_nMaxFrameCount = -1;
	m_nLastID = -1;
	m_bAllReset = true;

	ResetFrameCount();
}

void CellIDFixer::SetLastCellID(int nID)
{
	m_nLastID = nID;
}

//이 비전이 사용하는 frame count를 입력한다. 
// cis 는 1이다.
// area 는  1 ~ 3 이다.
void CellIDFixer::SetMaxFrameCount(int nFC, int nErrorCount)
{
	if (nFC != m_nMaxFrameCount)
	{
		//변화가 생겼으므로 초기화 시켜준다. 
		Init(nErrorCount);
	}

	m_nMaxFrameCount = nFC;
}


void CellIDFixer::ResetFrameCount()
{
	m_nPos_fc = 0;
	for (int j = 0; j < MAX_FRAME_COUNT; j++)
	{
		m_nArrFrameCount[0][j] = -1;
		m_nArrFrameCount[1][j] = 0;
	}
}

//return value
//frame count가 있어서 cell id 보상이 가능하면 셀아이디 값을
//아무것도 없는 상태라면 -1을 리턴한다.
int  CellIDFixer::GetCellID()
{
	int nCell = 0;
	for (int i = 0; i < m_nMaxFrameCount; i++)
	{
		if (0 <= m_nArrFrameCount[0][i])
		{
			return m_nArrFrameCount[0][i];
		}
	}

	return -1;
}


bool CellIDFixer::GetFixedFrameCount(int nCELL_ID, int nFC, int& nFixCell, int& nFixFrameCount)
{
	if (0 >= nFC || m_nMaxFrameCount < nFC)
		return false;

	int nIndex = nFC - 1;
	bool bError[2] = { false, };

	if (true == m_bAllReset)
	{
		m_bAllReset = false;
		m_nPos_fc = nIndex;
	}
	else
	{
		
		//frame count 보상이 필요한가?
		if (m_nPos_fc != nIndex)
		{
			bError[1] = true;
			nIndex = m_nPos_fc;
		}

		if (0 < nIndex)
		{
			//cell id 보상이 필요한가?
			if (m_nArrFrameCount[0][0] != nCELL_ID )
			{
				bError[0] = true;
			}
		}

		if (true == bError[0] && true == bError[1])
		{
			// 둘다 보상이 필요하다는 말은 보상 문제가 아닌것임,, 
		}
		else
		{
			// cell id, frame count 2중에 하나만 문재가 생겼을 때 보상한다. 
			if (true == bError[1])
			{
				nFC = m_nPos_fc + 1;
			}

			if (true == bError[0])
			{
				nCELL_ID = m_nArrFrameCount[0][0];
			}

		}
	}

	m_nArrFrameCount[0][nIndex] = nCELL_ID;
	m_nArrFrameCount[1][nIndex] = nFC;
	m_nPos_fc++;

	nFixCell = m_nArrFrameCount[0][nIndex];
	nFixFrameCount = m_nArrFrameCount[1][nIndex];

	//다 차면 초기화
	
	if(m_nMaxFrameCount <= m_nPos_fc)
		ResetFrameCount();

	if (true == bError[0] && true == bError[1])
		return false;
	else
		return true;
}


bool CellIDFixer::GetFixedCellID(int nCurID, int CurFrameCount, int nBit, CString* szCode, int& nFixCellID, int& nFixFrameCount)
{
	if (0 > m_nMaxFrameCount)	//frame count max를 지정하지 않음
	{
		*szCode = _T("[Fixed Info Error] frame count max를 지정하지 않음(코드에러)");
		return false;
	}
	int CEDD_ID = -1;

	*szCode = _T("[Fixed Info]");

	if (m_nLastID < 0)
	{
		return GetFixedFrameCount(nCurID, CurFrameCount, nFixCellID, nFixFrameCount);
	}
	
	int nTemp = GetCellID();
	if (0 <= nTemp && nTemp <= nCurID)	
	{
		//frame count 이내에서 셀 아이디를 입력 받고 있는 중이라면  GetFixedFrameCount() 함수 안에서 모두 보상한다.  
		if (false == GetFixedFrameCount(nCurID, CurFrameCount, nFixCellID, nFixFrameCount))
		{
			m_nFixCount++;
		}


		if (m_nErrorCount <= m_nFixCount)
			return false;
		else
			return true;
	}

	nTemp = nCurID;
    int nExpression = pow(2, nBit);
	long nID_Diff = 0;
	
	if (nCurID < m_nLastID)
	{
		// 한바퀴 돌았을 거라고 판단하고 
		nTemp = nCurID + nExpression;
	}

	nID_Diff = nTemp - m_nLastID;
	if (0 == nID_Diff)
	{
		//0 - 1 - 2 - 2(error) - 4, 중복 아이디가 발행되면 +1 해준다.
		CEDD_ID = nTemp + 1;
		m_nFixCount++;
	}
	else if (2 < nID_Diff)
	{
		//0 - 1 - 2 - 55(error) - 4, 아이디가 깨진경우 보상 해준다.
		CEDD_ID = m_nLastID + 1;
		m_nFixCount++;
	}
	else
	{
		// 1셀은 
		CEDD_ID = nCurID;
		m_nFixCount = 0;
	}

	CEDD_ID = CEDD_ID % nExpression;

	GetFixedFrameCount(CEDD_ID, CurFrameCount, nFixCellID, nFixFrameCount);
	
	if (m_nErrorCount <= m_nFixCount)
		return false;

	return true;
}