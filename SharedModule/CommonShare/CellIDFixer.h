#pragma once
#include "basedef.h"

#define SAME_ID_EXCEPTION 0
#define ID_BLANK_EXCEPTION 1

#define	MAX_FRAME_COUNT	3

class __COMMON_SHARE_CLASS__ CellIDFixer
{
public:
	CellIDFixer();
	~CellIDFixer();

	void SetMaxFrameCount(int nFC, int nErrorCount);

	void ResetFrameCount();
	bool GetFixedFrameCount(int nCELL_ID, int nFC, int &nFixCell, int &nFixFrameCount);

	void SetLastCellID(int nID);
	bool GetFixedCellID(int nCurID, int CurFrameCount, int nBit, CString* szCode, int &nFixCellID, int& nFixFrameCount);

	void Init(int nErrorCount);

private:
	int  GetCellID();

	int m_nLastID;
	int m_nFixCount;
	int m_nErrorCount;
	
	//frame count 관련 변수
	bool m_bAllReset;
	int m_nMaxFrameCount;
	int m_nPos_fc;
	int m_nArrFrameCount[2][MAX_FRAME_COUNT];
};
