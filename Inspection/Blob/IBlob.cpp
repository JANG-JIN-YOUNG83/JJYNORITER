
#include "stdafx.h"
#include "../stdafx.h"
#include "IBlob.h"

#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CIBlob::CIBlob()
{
	m_nMaxBlobNum = 1000000;
	m_nMaxMergeTableNum = 65000;

	m_vecBlobPartInfo.resize(m_nMaxBlobNum);
	m_vecMergeId.resize(m_nMaxBlobNum);

	m_psBlobPartInfo = &m_vecBlobPartInfo[0];
	m_pMergeId = &m_vecMergeId[0];

	m_vecMergeTable.resize(m_nMaxMergeTableNum);
	for (int i = 0; i < m_nMaxMergeTableNum; i++)
		m_vecMergeTable[i].reserve(16);
}

CIBlob::~CIBlob()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
long CIBlob::GetSafeBlob(BYTE* i_pbiImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, const CRect& rtROI2, CRect& rtSafeROI)
{
	short nBlobNum = 100;
	SBlobInfo* psObjInfo = new SBlobInfo[nBlobNum];
	short nFoundNum = nBlobNum;

	DoBlob(i_pbiImage, o_plLabel, nSizeX, nSizeY, rtROI2);
	GetBlobs(psObjInfo, rtROI2, 0, 10, nFoundNum);

	CRect rtTemp, rtTest;
	rtSafeROI.SetRectEmpty();

	for (int i = 0; i < nFoundNum; i++) {
		rtTest.SetRect(psObjInfo[i].nLeft, psObjInfo[i].nTop, psObjInfo[i].nRight, psObjInfo[i].nBottom);
		if (rtTemp.IntersectRect(rtTest, rtROI)) {
			rtSafeROI.UnionRect(&rtSafeROI, &rtTest);
		}
	}

	delete[] psObjInfo;

	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
long CIBlob::DoBlob(BYTE* i_pbyImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, short nThreshold)
{
	if (nThreshold == 255)
	{
		return DoBlobGray(i_pbyImage, o_plLabel, nSizeX, nSizeY, rtROI, nThreshold);
	}
	return DoBlobGrayHigh(i_pbyImage, o_plLabel, nSizeX, nSizeY, rtROI, nThreshold);

}




// 20060626 강민구. 
// Blob Part들을 Merge할 때, Iteration하지 않고 Eqtable을 이용하여 일괄 작업하도록 수정.
long CIBlob::DoBlobGray(BYTE* pbyImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, const short nValue)
{
	//FUNC_ENTRY_LOG("CIBlob::DoBlobGray()");
	m_nSizeX = nSizeX;
	m_nSizeY = nSizeY;
	m_lSize = nSizeX * nSizeY;

	long lresult;

	if (pbyImage == NULL) return -1;
	if (o_plLabel == NULL) return -1;
	if (rtROI.IsRectNull() == TRUE) return -1;

	m_plLabel = o_plLabel;

	//{{ 주어진 영역확인.
	CRect rtNewROI(rtROI);
	rtNewROI = rtNewROI & CRect(0, 0, m_nSizeX, m_nSizeY);
	//::VerifyROI(rtNewROI, CRect(0, 0, m_nSizeX, m_nSizeY));
	int ys = rtNewROI.top;
	int ye = rtNewROI.bottom;
	int xs = rtNewROI.left;
	int xe = rtNewROI.right;
	//}}

//BOOL bImageSave = FALSE;
//if(bImageSave)
//{
//	CImage insp(m_nSizeX, m_nSizeY, pbyImage);
//	insp.FlipVertical();
//	insp.ExportDIBFile("C:\\IntekPlus\\DebugResult\\blob source.bmp");
//}	
	int i, j;

	// Blob Part Info
	m_nNewLabel = 0; // 물체의 레이블은 1부터 매긴다.
	m_nNewMergeId = 0;
	_DoBlob_InitializeBlobPartInfo(m_psBlobPartInfo[0], 0, 0, 0);

	BYTE* pbyCurImgY = const_cast<BYTE*> (pbyImage + ys * m_nSizeX + xs);
	long* pnCurLabelY = m_plLabel + ys * m_nSizeX + xs;

	BYTE* pbyCurImg;
	long* pnCurLabel;

	UINT32 nCurLabel, nUpperLabel, nLeftLabel;

	int x, y;
	BOOL  bLineRun;

	for (y = ys; y < ye; y++, pbyCurImgY += m_nSizeX, pnCurLabelY += m_nSizeX)
	{
		pbyCurImg = pbyCurImgY;
		pnCurLabel = pnCurLabelY;

		bLineRun = FALSE;

		for (x = xs; x < xe; x++, pbyCurImg++, pnCurLabel++)
		{
			*pnCurLabel = 0;

			if (*pbyCurImg == nValue)
			{
				//Get Upper Line Label
				if (y == ys) nUpperLabel = 0;
				else nUpperLabel = *(pnCurLabel - m_nSizeX);

				if (nUpperLabel == 0 && y != ys && x != xe - 1)
					nUpperLabel = *(pnCurLabel - m_nSizeX + 1);

				if (!bLineRun)
				{
					//Get Left Line Label
					if (x == xs) nLeftLabel = 0;
					else nLeftLabel = *(pnCurLabel - 1);

					if (nLeftLabel == 0 && y != ys && x != xs)
						nLeftLabel = *(pnCurLabel - m_nSizeX - 1);

					lresult = _DoBlob_GetCurrentLabel(x, y, nLeftLabel, nUpperLabel, nCurLabel);
					if (lresult < 0)
						return lresult;

					if (y > m_psBlobPartInfo[nCurLabel].nBottom) m_psBlobPartInfo[nCurLabel].nBottom = y;
					if (x < m_psBlobPartInfo[nCurLabel].nLeft) m_psBlobPartInfo[nCurLabel].nLeft = x;

					bLineRun = TRUE;
				}

				if (nUpperLabel != 0 && nCurLabel != nUpperLabel) { // Met Two different Label
					_DoBlob_uOnMetTwoLabel(nUpperLabel, nCurLabel);
				}

				if (x > m_psBlobPartInfo[nCurLabel].nRight) m_psBlobPartInfo[nCurLabel].nRight = x;
				m_psBlobPartInfo[nCurLabel].lArea++;

				*pnCurLabel = nCurLabel;

			}
			else if (bLineRun) // LineRun이 끝난 후의 다음 점.
			{
				bLineRun = FALSE;
			}

		}
	}

	m_nNewLabel++; // 실제 개수는 1증가해야 한다. 
	m_nNewMergeId++; // 실제 개수는 1증가해야 한다. 

	//{{ Merge BlobPartInfo
	SBlobPartInfo* pBlobPart1;
	SBlobPartInfo* pBlobPart2;
	UINT32 nBaseLabel, nOtherLabel;
	for (j = 1; j < m_nNewMergeId; j++)
	{
		std::vector<int>& vecMergeLabel = m_vecMergeTable[j];

		if (vecMergeLabel.size() < 2) continue;

		nBaseLabel = vecMergeLabel[0];
		pBlobPart1 = &m_psBlobPartInfo[nBaseLabel];

		for (i = 1; i < vecMergeLabel.size(); i++)
		{
			nOtherLabel = vecMergeLabel[i];
			pBlobPart2 = &m_psBlobPartInfo[nOtherLabel];

			_DoBlob_MergeBlobPartInfo(pBlobPart1, pBlobPart2);

			_DoBlob_MergeLabel(pBlobPart1, pBlobPart2, m_plLabel, m_nSizeX, nBaseLabel, nOtherLabel);
		}
	}



	//if(bImageSave)
	//{
	//	FILE *stream = NULL;
	//	if(stream = fopen("C:\\IntekPlus\\DebugResult\\blob label.csv", "a"))
	//	{
	//		for(int y=975;y<1020;y++)
	//		{
	//			for(int x=865;x<920;x++)
	//			{
	//				fprintf(stream, "%d,", m_plLabel[y*m_nSizeX+x]);
	//			}
	//			fprintf(stream, "\n");
	//		}
	//		fprintf(stream, "\n");
	//		fclose(stream);
	//	}
	//
	//	if(stream = fopen("C:\\IntekPlus\\DebugResult\\blob Image.csv", "a"))
	//	{
	//		for(int y=975;y<1020;y++)
	//		{
	//			for(int x=865;x<920;x++)
	//			{
	//				fprintf(stream, "%d,", pbyImage[y*m_nSizeX+x]);
	//			}
	//			fprintf(stream, "\n");
	//		}
	//		fprintf(stream, "\n");
	//		fclose(stream);
	//	}
	//}

	//	for(j=1;j<m_nNewLabel+1;j++)
	//	{
	//		pBlobPart1 = &m_psBlobPartInfo[nBaseLabel];
	//		if(pBlobPart1->nRight-pBlobPart1->nLeft > 100)
	//		{
	//			i=0;
	//		}
	//	}



		//{{ Set Label Data 
	//	for(y=ys; y<ye; y++)
	//	{
	//		pbyCurImg = const_cast<BYTE*> (pbyImage + y*m_nSizeX + xs);
	//		pnCurLabel = m_plLabel + y*m_nSizeX + xs;

	//		for(x=xs; x<xe ; x++, pbyCurImg++, pnCurLabel++)
	//		{
	//			if( *pbyCurImg == nValue ){
	//				*pnCurLabel = m_psBlobPartInfo[*pnCurLabel].lEqLabel ;
	//			}
	//		}
	//	}

	return 0;

}

long CIBlob::DoBlobGrayHigh(BYTE* pbyImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, const short nValue)
{
	//FUNC_ENTRY_LOG("CIBlob::DoBlobGray()");
	m_nSizeX = nSizeX;
	m_nSizeY = nSizeY;
	m_lSize = nSizeX * nSizeY;

	long lresult;

	if (pbyImage == NULL) return -1;
	if (o_plLabel == NULL) return -1;
	if (rtROI.IsRectNull() == TRUE) return -1;

	m_plLabel = o_plLabel;

	//{{ 주어진 영역확인.
	CRect rtNewROI(rtROI);
	rtNewROI = rtNewROI & CRect(0, 0, m_nSizeX, m_nSizeY);
	//::VerifyROI(rtNewROI, CRect(0, 0, m_nSizeX, m_nSizeY));
	int ys = rtNewROI.top;
	int ye = rtNewROI.bottom;
	int xs = rtNewROI.left;
	int xe = rtNewROI.right;
	//}}

//BOOL bImageSave = FALSE;
//if(bImageSave)
//{
//	CImage insp(m_nSizeX, m_nSizeY, pbyImage);
//	insp.FlipVertical();
//	insp.ExportDIBFile("C:\\IntekPlus\\DebugResult\\blob source.bmp");
//}	
	int i, j;

	// Blob Part Info
	m_nNewLabel = 0; // 물체의 레이블은 1부터 매긴다.
	m_nNewMergeId = 0;
	_DoBlob_InitializeBlobPartInfo(m_psBlobPartInfo[0], 0, 0, 0);

	BYTE* pbyCurImgY = const_cast<BYTE*> (pbyImage + ys * m_nSizeX + xs);
	long* pnCurLabelY = m_plLabel + ys * m_nSizeX + xs;

	BYTE* pbyCurImg;
	long* pnCurLabel;

	UINT32 nCurLabel, nUpperLabel, nLeftLabel;

	int x, y;
	BOOL  bLineRun;

	for (y = ys; y < ye; y++, pbyCurImgY += m_nSizeX, pnCurLabelY += m_nSizeX)
	{
		pbyCurImg = pbyCurImgY;
		pnCurLabel = pnCurLabelY;

		bLineRun = FALSE;

		for (x = xs; x < xe; x++, pbyCurImg++, pnCurLabel++)
		{
			*pnCurLabel = 0;

			if (*pbyCurImg > nValue)
			{
				//Get Upper Line Label
				if (y == ys) nUpperLabel = 0;
				else nUpperLabel = *(pnCurLabel - m_nSizeX);

				if (nUpperLabel == 0 && y != ys && x != xe - 1)
					nUpperLabel = *(pnCurLabel - m_nSizeX + 1);

				if (!bLineRun)
				{
					//Get Left Line Label
					if (x == xs) nLeftLabel = 0;
					else nLeftLabel = *(pnCurLabel - 1);

					if (nLeftLabel == 0 && y != ys && x != xs)
						nLeftLabel = *(pnCurLabel - m_nSizeX - 1);

					lresult = _DoBlob_GetCurrentLabel(x, y, nLeftLabel, nUpperLabel, nCurLabel);
					if (lresult < 0)
						return lresult;

					if (y > m_psBlobPartInfo[nCurLabel].nBottom) m_psBlobPartInfo[nCurLabel].nBottom = y;
					if (x < m_psBlobPartInfo[nCurLabel].nLeft) m_psBlobPartInfo[nCurLabel].nLeft = x;

					bLineRun = TRUE;
				}

				if (nUpperLabel != 0 && nCurLabel != nUpperLabel) { // Met Two different Label
					_DoBlob_uOnMetTwoLabel(nUpperLabel, nCurLabel);
				}

				if (x > m_psBlobPartInfo[nCurLabel].nRight) m_psBlobPartInfo[nCurLabel].nRight = x;
				m_psBlobPartInfo[nCurLabel].lArea++;

				*pnCurLabel = nCurLabel;

			}
			else if (bLineRun) // LineRun이 끝난 후의 다음 점.
			{
				bLineRun = FALSE;
			}

		}
	}

	m_nNewLabel++; // 실제 개수는 1증가해야 한다. 
	m_nNewMergeId++; // 실제 개수는 1증가해야 한다. 

	//{{ Merge BlobPartInfo
	SBlobPartInfo* pBlobPart1;
	SBlobPartInfo* pBlobPart2;
	UINT32 nBaseLabel, nOtherLabel;
	for (j = 1; j < m_nNewMergeId; j++)
	{
		std::vector<int>& vecMergeLabel = m_vecMergeTable[j];

		if (vecMergeLabel.size() < 2) continue;

		nBaseLabel = vecMergeLabel[0];
		pBlobPart1 = &m_psBlobPartInfo[nBaseLabel];

		for (i = 1; i < vecMergeLabel.size(); i++)
		{
			nOtherLabel = vecMergeLabel[i];
			pBlobPart2 = &m_psBlobPartInfo[nOtherLabel];

			_DoBlob_MergeBlobPartInfo(pBlobPart1, pBlobPart2);

			_DoBlob_MergeLabel(pBlobPart1, pBlobPart2, m_plLabel, m_nSizeX, nBaseLabel, nOtherLabel);
		}
	}



	//if(bImageSave)
	//{
	//	FILE *stream = NULL;
	//	if(stream = fopen("C:\\IntekPlus\\DebugResult\\blob label.csv", "a"))
	//	{
	//		for(int y=975;y<1020;y++)
	//		{
	//			for(int x=865;x<920;x++)
	//			{
	//				fprintf(stream, "%d,", m_plLabel[y*m_nSizeX+x]);
	//			}
	//			fprintf(stream, "\n");
	//		}
	//		fprintf(stream, "\n");
	//		fclose(stream);
	//	}
	//
	//	if(stream = fopen("C:\\IntekPlus\\DebugResult\\blob Image.csv", "a"))
	//	{
	//		for(int y=975;y<1020;y++)
	//		{
	//			for(int x=865;x<920;x++)
	//			{
	//				fprintf(stream, "%d,", pbyImage[y*m_nSizeX+x]);
	//			}
	//			fprintf(stream, "\n");
	//		}
	//		fprintf(stream, "\n");
	//		fclose(stream);
	//	}
	//}

	//	for(j=1;j<m_nNewLabel+1;j++)
	//	{
	//		pBlobPart1 = &m_psBlobPartInfo[nBaseLabel];
	//		if(pBlobPart1->nRight-pBlobPart1->nLeft > 100)
	//		{
	//			i=0;
	//		}
	//	}



		//{{ Set Label Data 
	//	for(y=ys; y<ye; y++)
	//	{
	//		pbyCurImg = const_cast<BYTE*> (pbyImage + y*m_nSizeX + xs);
	//		pnCurLabel = m_plLabel + y*m_nSizeX + xs;

	//		for(x=xs; x<xe ; x++, pbyCurImg++, pnCurLabel++)
	//		{
	//			if( *pbyCurImg == nValue ){
	//				*pnCurLabel = m_psBlobPartInfo[*pnCurLabel].lEqLabel ;
	//			}
	//		}
	//	}

	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
long  CIBlob::_DoBlob_GetCurrentLabel(short x, short y, UINT32 nLeftLabel, UINT32 nUpperLabel, UINT32& nCurLabel)
{
	long lresult = 0;
	if (nLeftLabel == 0) {
		if (nUpperLabel == 0) { // Add New Label
			m_nNewLabel++;
			lresult = _DoBlob_InitializeBlobPartInfo(m_psBlobPartInfo[m_nNewLabel], m_nNewLabel, x, y);
			nCurLabel = m_nNewLabel;
		}
		else {
			nCurLabel = nUpperLabel;
		}
	}
	else {
		nCurLabel = nLeftLabel;
	}

	return lresult;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 CIBlob::_DoBlob_uOnMetTwoLabel(UINT32 nLabel1, UINT32 nLabel2)
{
	UINT32 	nCurMergeId;

	UINT32& nIDLabel1 = m_pMergeId[nLabel1];
	UINT32& nIDLabel2 = m_pMergeId[nLabel2];

	//{{ GetMerge Id
	if (nIDLabel1 == 0 && nIDLabel2 == 0)
	{
		m_nNewMergeId++; // MergeId를 늘린다. MergeId는 1부터 시작된다.

		if (m_nNewMergeId > m_nMaxMergeTableNum - 1)
		{
			m_nMaxMergeTableNum += 1000;
			m_vecMergeTable.resize(m_nMaxMergeTableNum);
		}

		nCurMergeId = m_nNewMergeId;
		m_vecMergeTable[m_nNewMergeId].clear();

	}
	else if (nIDLabel1 != 0 && nIDLabel2 != 0) // MergeId 끼리 만나면,
	{
		if (nIDLabel1 == nIDLabel2)
			return nIDLabel1;
		else
			nCurMergeId = _DoBlob_CombineMergeTable(nIDLabel1, nIDLabel2);

	}
	else if (nIDLabel1 != 0)
	{
		nCurMergeId = nIDLabel1;
	}
	else
	{
		nCurMergeId = nIDLabel2;
	}
	//}}

	std::vector<int>& vecMergeTable = m_vecMergeTable[nCurMergeId];

	if (nIDLabel1 == 0)
	{
		nIDLabel1 = nCurMergeId;
		vecMergeTable.push_back(nLabel1);
	}

	if (nIDLabel2 == 0)
	{
		nIDLabel2 = nCurMergeId;
		vecMergeTable.push_back(nLabel2);
	}

	return nCurMergeId;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 CIBlob::_DoBlob_CombineMergeTable(UINT32 nMergeID1, UINT32 nMergeID2)
{
	UINT32 nMinID, nOtherID;
	if (nMergeID1 < nMergeID2)
	{
		nMinID = nMergeID1;
		nOtherID = nMergeID2;
	}
	else
	{
		nMinID = nMergeID2;
		nOtherID = nMergeID1;
	}

	std::vector<int>& vecMergeTable1 = m_vecMergeTable[nMinID];
	std::vector<int>& vecMergeTable2 = m_vecMergeTable[nOtherID];

	//{{ 병합당할 Label들의 MergeId를 수정해준다.
	UINT32 nLabel;
	for (int i = 0; i < vecMergeTable2.size(); i++)
	{
		nLabel = vecMergeTable2[i];
		m_pMergeId[nLabel] = nMinID;
	}
	//}}

	vecMergeTable1.insert(vecMergeTable1.end(), vecMergeTable2.begin(), vecMergeTable2.end());
	vecMergeTable2.clear();

	return nMinID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
long CIBlob::_DoBlob_MergeBlobPartInfo(SBlobPartInfo* psBlob1, SBlobPartInfo* psBlob2)
{

	psBlob1->nLeft = min(psBlob1->nLeft, psBlob2->nLeft);
	psBlob1->nTop = min(psBlob1->nTop, psBlob2->nTop);
	psBlob1->nRight = max(psBlob1->nRight, psBlob2->nRight);
	psBlob1->nBottom = max(psBlob1->nBottom, psBlob2->nBottom);

	psBlob1->lArea += psBlob2->lArea;
	psBlob2->lArea = 0;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
long CIBlob::_DoBlob_MergeLabel(SBlobPartInfo* psBlob1, SBlobPartInfo* psBlob2, long* plLabel, const long nImageX, const UINT32 nLabel1, const UINT32 nLabel2)
{
	int i, j;

	long* plTemp = plLabel + psBlob2->nTop * nImageX + psBlob2->nLeft;
	long lOffset = nImageX - (psBlob2->nRight + 1 - psBlob2->nLeft);

	for (i = psBlob2->nTop; i <= psBlob2->nBottom; i++)
	{
		for (j = psBlob2->nLeft; j <= psBlob2->nRight; j++, plTemp++)
		{
			if ((*plTemp) == nLabel2)
				(*plTemp) = nLabel1;
		}

		plTemp += lOffset;
	}

	return 0;
}


long  CIBlob::_DoBlob_InitializeBlobPartInfo(SBlobPartInfo& oBlobPartInfo, UINT32 nLabel, short x, short y)
{
	if (nLabel > m_nMaxBlobNum)
		return -1;

	oBlobPartInfo.lArea = 0;
	oBlobPartInfo.nLeft = x;
	oBlobPartInfo.nRight = x;
	oBlobPartInfo.nTop = y;
	oBlobPartInfo.nBottom = y;

	m_pMergeId[nLabel] = 0;

	return 0;
}

void  CIBlob::ResetBlobPartInfo(SBlobPartInfo& oBlobPartInfo)
{
	oBlobPartInfo.lArea = 0;
	oBlobPartInfo.nLeft = 0;
	oBlobPartInfo.nRight = 0;
	oBlobPartInfo.nTop = 0;
	oBlobPartInfo.nBottom = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

long CIBlob::GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, short nROIOffset, long lAreaThresh, short& nNum)
{
	//FUNC_ENTRY_LOG("CIBlob::GetBlobs()");

	short nObjectNumUL = nNum;

	if (psObjInfo == NULL) return -1;

	int i;

	BOOL bOK;

	for (i = nNum = 0; i < m_nNewLabel; i++) {

		bOK = _GetBlob_CheckArea(m_psBlobPartInfo[i], lAreaThresh);
		if (bOK) bOK = _GetBlob_CheckBlobInROI(m_psBlobPartInfo[i], rtROI, nROIOffset);

		if (bOK) {
			_GetBlob_SetObjectInfo(m_psBlobPartInfo[i], i, psObjInfo[nNum]);
			nNum++; if (nNum >= nObjectNumUL) return -1;
		}
		ResetBlobPartInfo(m_psBlobPartInfo[i]);
	}

	CalcBlobPosition(NULL, m_plLabel, psObjInfo, nNum);


	return 0;

}


long CIBlob::GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, short nROIOffset, long lAreaThresh, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum)
{
	short nObjectNumUL = nNum;

	if (psObjInfo == NULL) return -1;

	int i;

	BOOL bOK;

	for (i = nNum = 0; i < m_nNewLabel; i++) {

		bOK = _GetBlob_CheckArea(m_psBlobPartInfo[i], lAreaThresh);
		if (bOK) bOK = _GetBlob_CheckBlobInROI(m_psBlobPartInfo[i], rtROI, nROIOffset);
		if (bOK) bOK = _GetBlob_CheckBlobSize(m_psBlobPartInfo[i], nWidthUL, nWidthLL, nHeightUL, nHeightLL);

		if (bOK) {
			_GetBlob_SetObjectInfo(m_psBlobPartInfo[i], i, psObjInfo[nNum]);
			nNum++;
			if (nNum >= nObjectNumUL)
				return -1;
		}
		ResetBlobPartInfo(m_psBlobPartInfo[i]);
	}

	CalcBlobPosition(NULL, m_plLabel, psObjInfo, nNum);

	return 0;
}


long CIBlob::GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, CRect rtExcluded, short nROIOffset, long lAreaThresh, float fAreaRatio, float fLengthRatioUL, float fLengthRatioLL, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum)
{
	if (psObjInfo == NULL) return -1;

	short nObjectNumUL = nNum;

	int i;

	for (i = nNum = 0; i < m_nNewLabel; i++)
	{
		if (_GetBlob_CheckArea(m_psBlobPartInfo[i], lAreaThresh))
		{
			if (_GetBlob_CheckBlobInROI(m_psBlobPartInfo[i], rtROI, nROIOffset))
			{
				if (!_GetBlob_CheckBlobInROI(m_psBlobPartInfo[i], rtExcluded, 1))
				{
					if (_GetBlob_CheckBlobSize(m_psBlobPartInfo[i], nWidthUL, nWidthLL, nHeightUL, nHeightLL))
					{
						//						if(_GetBlob_CheckLengthRatio(m_psBlobPartInfo[i], fLengthRatioUL, fLengthRatioLL))
						//						{
						_GetBlob_SetObjectInfo(m_psBlobPartInfo[i], i, psObjInfo[nNum]);
						nNum++;
						if (nNum >= nObjectNumUL)
							return -1;
						//						}
					}
				}
			}
		}

		ResetBlobPartInfo(m_psBlobPartInfo[i]);
	}

	CalcBlobPosition(NULL, m_plLabel, psObjInfo, nNum);

	return 0;
}



long CIBlob::GetBlobs(SBlobInfo* psObjInfo, long lAreaThresh, short& nNum, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL)
{
	short nObjectNumUL = nNum;
	SBlobInfo sbProper;

	if (psObjInfo == NULL) return -1;

	int i;

	int size = sizeof(psObjInfo);
	int size2 = sizeof(SBlobInfo);

	BOOL bOK;

	for (i = nNum = 0; i < m_nNewLabel; i++) {

		bOK = _GetBlob_CheckArea(m_psBlobPartInfo[i], lAreaThresh);
		if (bOK) bOK = _GetBlob_CheckBlobSize(m_psBlobPartInfo[i], nWidthUL, nWidthLL, nHeightUL, nHeightLL);

		if (bOK) {
			_GetBlob_SetObjectInfo(m_psBlobPartInfo[i], i, psObjInfo[nNum]);
			nNum++; if (nNum >= nObjectNumUL) return -1;
		}

		ResetBlobPartInfo(m_psBlobPartInfo[i]);
	}

	CalcBlobPosition(NULL, m_plLabel, psObjInfo, nNum);

	return 0;
}




long CIBlob::GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, short nROIOffset, long lAreaThresh, float fAreaRatio, float fLengthRatioUL, float fLengthRatioLL, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum)
{
	if (psObjInfo == NULL) return -1;

	short nObjectNumUL = nNum;

	int i;

	BOOL bOK;

	for (i = nNum = 0; i < m_nNewLabel; i++) {

		bOK = _GetBlob_CheckArea(m_psBlobPartInfo[i], lAreaThresh);
		if (bOK) bOK = _GetBlob_CheckBlobInROI(m_psBlobPartInfo[i], rtROI, nROIOffset);
		if (bOK) bOK = _GetBlob_CheckBlobSize(m_psBlobPartInfo[i], nWidthUL, nWidthLL, nHeightUL, nHeightLL);
		//		if(bOK) bOK=_GetBlob_CheckLengthRatio(m_psBlobPartInfo[i], fLengthRatioUL, fLengthRatioLL);

		if (bOK) {
			_GetBlob_SetObjectInfo(m_psBlobPartInfo[i], i, psObjInfo[nNum]);
			nNum++; if (nNum >= nObjectNumUL) return -1;
		}
		ResetBlobPartInfo(m_psBlobPartInfo[i]);
	}

	CalcBlobPosition(NULL, m_plLabel, psObjInfo, nNum);
	return 0;
}



// 김일남 2004.03.23
long CIBlob::GetBlobs(long* i_plBuffer, SBlobInfo* psObjInfo, const CRect& rtROI, CRect rtMerge, CRect rtExcluded, short nROIOffset, long lAreaThresh, float fAreaRatio, float fLengthRatioUL, float fLengthRatioLL, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum)
{
	if (psObjInfo == NULL) return -1;

	short nObjectNumUL = nNum;

	int i;

	BOOL bOK;

	for (i = nNum = 0; i < m_nNewLabel; i++) {

		bOK = _GetBlob_CheckArea(m_psBlobPartInfo[i], lAreaThresh);
		if (bOK) bOK = _GetBlob_CheckBlobInROI(m_psBlobPartInfo[i], rtROI, nROIOffset);
		if (bOK) bOK = !_GetBlob_CheckBlobInROI(m_psBlobPartInfo[i], rtExcluded, 1);
		if (bOK) bOK = _GetBlob_CheckBlobSize(m_psBlobPartInfo[i], nWidthUL, nWidthLL, nHeightUL, nHeightLL);
		//		if(bOK) bOK=_GetBlob_CheckLengthRatio(m_psBlobPartInfo[i], fLengthRatioUL, fLengthRatioLL);

		if (bOK) {
			_GetBlob_SetObjectInfo(m_psBlobPartInfo[i], i, psObjInfo[nNum]);
			nNum++; if (nNum >= nObjectNumUL) return -1;
		}
		ResetBlobPartInfo(m_psBlobPartInfo[i]);
	}

	int nFirstID;
	int nMergeNum;
	for (i = nMergeNum = 0; i < nNum; i++) {
		if (psObjInfo[i].nLeft < rtMerge.left) continue;
		if (psObjInfo[i].nRight > rtMerge.right) continue;
		if (psObjInfo[i].nTop < rtMerge.top) continue;
		if (psObjInfo[i].nBottom > rtMerge.bottom) continue;

		if (nMergeNum == 0)
			nFirstID = i;
		else
		{
			int x, y;
			for (y = psObjInfo[i].nTop; y < psObjInfo[i].nBottom; y++)
			{
				for (x = psObjInfo[i].nLeft; x < psObjInfo[i].nRight; x++)
				{
					if (i_plBuffer[x + y * m_nSizeX] == psObjInfo[i].lLabel)
					{
						i_plBuffer[x + y * m_nSizeX] = psObjInfo[nFirstID].lLabel;
					}
				}
			}

			psObjInfo[nFirstID].nLeft = min(psObjInfo[nFirstID].nLeft, psObjInfo[i].nLeft);
			psObjInfo[nFirstID].nRight = max(psObjInfo[nFirstID].nRight, psObjInfo[i].nRight);
			psObjInfo[nFirstID].nTop = min(psObjInfo[nFirstID].nTop, psObjInfo[i].nTop);
			psObjInfo[nFirstID].nBottom = max(psObjInfo[nFirstID].nBottom, psObjInfo[i].nBottom);
			psObjInfo[nFirstID].fCenX = (psObjInfo[nFirstID].fCenX * psObjInfo[nFirstID].lArea + psObjInfo[i].fCenX * psObjInfo[i].lArea) / (psObjInfo[nFirstID].lArea + psObjInfo[i].lArea);
			psObjInfo[nFirstID].fCenY = (psObjInfo[nFirstID].fCenY * psObjInfo[nFirstID].lArea + psObjInfo[i].fCenY * psObjInfo[i].lArea) / (psObjInfo[nFirstID].lArea + psObjInfo[i].lArea);
			psObjInfo[nFirstID].nCenX = int(psObjInfo[nFirstID].fCenX + 0.5f);
			psObjInfo[nFirstID].nCenY = int(psObjInfo[nFirstID].fCenY + 0.5f);
			psObjInfo[nFirstID].lArea = psObjInfo[nFirstID].lArea + psObjInfo[i].lArea;
			psObjInfo[i].lLabel = psObjInfo[nFirstID].lLabel;
			psObjInfo[i].lArea = 0;
		}

		nMergeNum++;
	}


	CalcBlobPosition(NULL, m_plLabel, psObjInfo, nNum);
	return 0;
}


BOOL CIBlob::_GetBlob_CheckBlobInROI(SBlobPartInfo& sBlobPartInfo, const CRect& rtROI, short nOffset)
{
	//	if(nOffset<=0) return TRUE;

	//{{ 2003.10.24 Modify by MinguKang
	nOffset = 0;
	//}}

	if ((sBlobPartInfo.nLeft >= (rtROI.left + nOffset))
		&& (sBlobPartInfo.nRight <= (rtROI.right - nOffset))
		&& (sBlobPartInfo.nTop >= (rtROI.top + nOffset))
		&& (sBlobPartInfo.nBottom <= (rtROI.bottom - nOffset)))
		return TRUE;
	else return FALSE;
}


BOOL  CIBlob::_GetBlob_CheckArea(const SBlobPartInfo& sBlobPartInfo, long lAreaThresh)
{
	if (sBlobPartInfo.lArea < lAreaThresh || sBlobPartInfo.lArea < 1)
		return FALSE;
	else
		return TRUE;
}

BOOL  CIBlob::_GetBlob_CheckAreaRatio(const SBlobPartInfo& sBlobPartInfo, float fAreaRatioThresh)
{
	float fAreaRatio = (float)sBlobPartInfo.lArea / (float)((sBlobPartInfo.nRight - sBlobPartInfo.nLeft) * (sBlobPartInfo.nBottom - sBlobPartInfo.nTop));

	if (fAreaRatio > fAreaRatioThresh) return TRUE;
	else return FALSE;
}

BOOL  CIBlob::_GetBlob_CheckLengthRatio(const SBlobPartInfo& sBlobPartInfo, float fLengthRatioUL, float fLengthRatioLL)
{
	short nHeight = sBlobPartInfo.nBottom - sBlobPartInfo.nTop;
	short nWidth = sBlobPartInfo.nRight - sBlobPartInfo.nLeft;

	if (nHeight < 1) return FALSE;

	float fLengthRatio = (float)nWidth / nHeight;

	if (fLengthRatio > fLengthRatioUL || fLengthRatio < fLengthRatioLL) return FALSE;
	else return TRUE;
}

BOOL  CIBlob::_GetBlob_CheckBlobSize(const SBlobPartInfo& sBlobPartInfo, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL)
{
	short nHeight = sBlobPartInfo.nBottom - sBlobPartInfo.nTop;
	short nWidth = sBlobPartInfo.nRight - sBlobPartInfo.nLeft;

	if (nHeight > nHeightUL || nHeight<nHeightLL || nWidth>nWidthUL || nWidth < nWidthLL) return FALSE;
	else return TRUE;
}


void CIBlob::_GetBlob_SetObjectInfo(const SBlobPartInfo& sBlobPartInfo, UINT32 nLabel, SBlobInfo& oBlobInfo)
{
	oBlobInfo.lLabel = nLabel;
	oBlobInfo.lArea = sBlobPartInfo.lArea;

	//	oBlobInfo.fCenX = ((float)sBlobPartInfo.lCenX/(float)oBlobInfo.lArea);
	//	oBlobInfo.fCenY = ((float)sBlobPartInfo.lCenY/(float)oBlobInfo.lArea);

	//	oBlobInfo.nCenX = (short)oBlobInfo.fCenX;
	//	oBlobInfo.nCenY = (short)oBlobInfo.fCenY;

	oBlobInfo.nLeft = sBlobPartInfo.nLeft;
	oBlobInfo.nTop = sBlobPartInfo.nTop;
	oBlobInfo.nRight = sBlobPartInfo.nRight;
	oBlobInfo.nBottom = sBlobPartInfo.nBottom;

	oBlobInfo.rtROI = CRect(oBlobInfo.nLeft, oBlobInfo.nTop, oBlobInfo.nRight + 1, oBlobInfo.nBottom + 1);
}

// =========================================================
/** @fn		long CIBlob::MergeBlobsByDistance(SBlobInfo* psBlobs, short& nBlobNum, long* plLabel, float fDistance)
	@brief
	@param		SBlobInfo* psBlobs :
	@param		short& nBlobNum :
	@param		long* plLabel :
	@param		float fDistanceThreshold : Merge 여부를 결정하는 Blob 간 거리 조건
	@return		:
	@date		: 2006-04-07 오후 2:16:20
	@author		:
	@warning	: 이 함수가 호출되기 전에 GetBoundaryPoints 함수가 호출되어서 Edge Point 가 미리 계산되어야 함
	@sa			:
	@note		:
*/ // ======================================================
// 2006-07-03 강민구
// blob의 수가 많을 수록 시간이 너무 많이 걸려서, 
// 모든 blob간에 거리를 계산하는 방식이 아닌, blob 영역에서 일정 거리내에 다른 blob이 존재하는지 조사하는 방식으로 변경한다.
/*
long CIBlob::MergeBlobsByDistance( SBlobInfo* psBlobs, short& nBlobNum, long* plLabel, CRect rtROI, float fDistanceThreshold )
{
	//FUNC_ENTRY_LOG("CIBlob::MergeBlobsByDistance()");

	long i, j, k;
	SBlobInfo* pBlob1;
	SBlobInfo* pBlob2;
	long nResult;

	CRect rt1, rt2, rttemp;

	m_nNewMergeId = 0;

	short x, y;
	long* pnCurLabelY;
	long* pnCurLabel;
	long  nThisLabel;

	static std::vector<int> vecLabelToBlobId( m_nMaxBlobNum );
	int *pLabelToBlobId = &vecLabelToBlobId[0];

	memset( pLabelToBlobId, 0, sizeof(int)*(m_nMaxBlobNum) );
	memset( m_pMergeId, 0, sizeof(UINT32)*(nBlobNum+1) );

	for(j=0;j<nBlobNum;j++)
	{
		pLabelToBlobId[psBlobs[j].lLabel] = j ;
	}

	int nDistNum = (int)(fDistanceThreshold+0.5f)*2 + 1;
	std::vector<int> vecNeihbor( nDistNum*nDistNum );
	int *pNeighbor = &vecNeihbor[0];

	int nNeighborNum = _MergeBlob_MakeNeighbor( fDistanceThreshold, pNeighbor );
	int nCurBlobId;

	for (j = 0; j < nBlobNum; j++)
	{
		pBlob1 = &psBlobs[j];
		if(pBlob1->lArea == 0)
			continue;

		// 충분히 큰 것을 Merge할 필요가 있을까?
		if(pBlob1->lArea > 100)
			continue;

		long nEdgeNum = pBlob1->arptEdge.size();
		CPoint *pEdges = &pBlob1->arptEdge[0];
		for( i=0; i< nEdgeNum; i++ )
		{
			x = pEdges[i].x;
			y = pEdges[i].y;

			if( x < fDistanceThreshold+1 || x > m_nSizeX-fDistanceThreshold-1 )
				continue;
			if( y < fDistanceThreshold+1 || y > m_nSizeY-fDistanceThreshold-1 )
				continue;

			pnCurLabelY = plLabel + y*m_nSizeX + x;

			for( k=0; k<nNeighborNum; k++)
			{
				pnCurLabel = pnCurLabelY + pNeighbor[k];
				nCurBlobId = pLabelToBlobId[*pnCurLabel] ;

				if( nCurBlobId == j ||  nCurBlobId == 0 )
					continue;

				_DoBlob_uOnMetTwoLabel( j, nCurBlobId );

			}
		}
	}

	m_nNewMergeId++;

	for(j=1; j<m_nNewMergeId; j++)
	{
		std::vector<int>& vecMergeTable = m_vecMergeTable[j];

		pBlob1 = &psBlobs[vecMergeTable[0]];

		for(i=1; i<vecMergeTable.size(); i++)
		{
			pBlob2 = &psBlobs[vecMergeTable[i]];

			nResult = MergeBlob(pBlob1, pBlob2, plLabel, m_nSizeX);
			if(nResult != 0)
				return nResult;
		}
	}

//	for(i=0;i<nBlobNum;i++)
//	{
//		pBlob1 = &psBlobs[i];
//		if( pBlob1->nRight-pBlob1->nLeft > 150 )
//		{
//			j=0;//break;
//		}
//	}

	return 0;
}


int  CIBlob::_MergeBlob_MakeNeighbor( float fDistance, int *pBuffer )
{
	int nDist = (int)(fDistance+0.5f);

	float fMaxDist = fDistance*fDistance;
	float ftemp;
	float fCurdist;

	int nDnum = 0;
	for(int j=-nDist;j<nDist;j++)
	{
		ftemp = j*j;
		for(int i=-nDist;i<nDist;i++)
		{
			fCurdist = ftemp + i*i;
			if(fCurdist<fMaxDist)
			{
				pBuffer[nDnum++] = j*m_nSizeX + i;
			}
		}
	}

	return nDnum;
}
*/
///*

long CIBlob::MergeBlobsByDistance(SBlobInfo* psBlobs, short& nBlobNum, long* plLabel, CRect rtROI, float fDistanceThreshold)
{
	//FUNC_ENTRY_LOG("CIBlob::MergeBlobsByDistance()");

	long i, j;
	SBlobInfo* pBlob1;
	SBlobInfo* pBlob2;
	long nResult;

	CRect rt1, rt2, rttemp;

	m_vecMergeTable.reserve(nBlobNum + 1);
	m_vecMergeId.reserve(nBlobNum + 1);

	memset(m_pMergeId, 0, sizeof(UINT32) * (nBlobNum + 1));

	for (i = 0; i < nBlobNum + 1; i++)
		m_vecMergeTable[i].reserve(16);

	int nMaxMergeId = 0;
	int nCurMergeId;

	for (j = 0; j < nBlobNum; j++)
	{
		pBlob1 = &psBlobs[j];
		if (pBlob1->lArea == 0)
			continue;

		rt1 = pBlob1->rtROI;

		for (i = j + 1; i < nBlobNum; i++)
		{
			pBlob2 = &psBlobs[i];
			if (pBlob2->lArea == 0)
				continue;

			rt2 = pBlob2->rtROI;

			if (rt1.left - rt2.right > fDistanceThreshold)
				continue;
			if (rt2.left - rt1.right > fDistanceThreshold)
				continue;
			if (rt1.top - rt2.bottom > fDistanceThreshold)
				continue;
			if (rt2.top - rt1.bottom > fDistanceThreshold)
				continue;

			if (IsNeededMerge(*pBlob1, *pBlob2, fDistanceThreshold))
			{
				//{{ GetMerge Id
				if (m_pMergeId[j] == 0 && m_pMergeId[i] == 0)
				{
					nMaxMergeId++; // MergeId를 늘린다. MergeId는 1부터 시작된다.
					nCurMergeId = nMaxMergeId;
					m_vecMergeTable[nMaxMergeId].clear();
				}
				else if (m_pMergeId[j] != 0)
				{
					nCurMergeId = m_pMergeId[j];
				}
				else {
					nCurMergeId = m_pMergeId[i];
				}
				//}}

				std::vector<int>& vecMergeTable = m_vecMergeTable[nCurMergeId];

				if (m_pMergeId[j] == 0)
				{
					m_pMergeId[j] = nCurMergeId;
					vecMergeTable.push_back(j);
				}

				if (m_pMergeId[i] == 0)
				{
					m_pMergeId[i] = nCurMergeId;
					vecMergeTable.push_back(i);
				}
			}

		}

	}

	for (j = 1; j < nMaxMergeId + 1; j++)
	{
		std::vector<int>& vecMergeTable = m_vecMergeTable[j];

		pBlob1 = &psBlobs[vecMergeTable[0]];

		for (i = 1; i < vecMergeTable.size(); i++)
		{
			pBlob2 = &psBlobs[vecMergeTable[i]];

			nResult = MergeBlob(pBlob1, pBlob2, plLabel, m_nSizeX);
			if (nResult != 0)
				return nResult;
		}
	}

	return 0;
}
//*/


// =========================================================
/** @fn		long CIBlob::MixDarkBlobAndBrightBlob(SBlobInfo* psDarkBlob, short& nDarkBlobNum, long* plDarkLabel, SBlobInfo* psBrightBlob, short& nBrightBlobNum, long* plBrightLabel, CRect rtROI)
	@brief		: Dark Blob, Label 정보와 Bright Blob, Label 정보를 합하여 Dark Blob, Label 에 설정
	@param		SBlobInfo* psDarkBlob :
	@param		short& nDarkBlobNum :
	@param		long* plDarkLabel :
	@param		SBlobInfo* psBrightBlob :
	@param		short& nBrightBlobNum :
	@param		long* plBrightLabel :
	@param		CRect rtROI :
	@return		:
	@date		: 2006-04-07 오후 10:44:23
	@author		:
	@warning	:
	@sa			:
	@note		:
*/ // ======================================================
long CIBlob::MixDarkBlobAndBrightBlob(SBlobInfo* psDarkBlob, short& nDarkBlobNum, long* plDarkLabel, SBlobInfo* psBrightBlob, short& nBrightBlobNum, long* plBrightLabel)
{
	long i, x, y;
	long index;
	SBlobInfo* psBlob = psDarkBlob;
	long nBlobNum = nDarkBlobNum + nBrightBlobNum;
	long nMaxDarkLabel = 0;

	long nCurLabel = 0;

	long nBeginY, nEndY;
	long nBeginX, nEndX;

	// Dark Blob 에서 가장 큰 라벨 확인
	for (i = 0; i < nDarkBlobNum; i++)
	{
		if (nMaxDarkLabel < psDarkBlob[i].lLabel)
		{
			nMaxDarkLabel = psDarkBlob[i].lLabel;
		}
	}

	for (i = 0; i < nBrightBlobNum; i++)
	{
		psDarkBlob[nDarkBlobNum + i] = psBrightBlob[i];

		nCurLabel = nMaxDarkLabel + psBrightBlob[i].lLabel;

		psDarkBlob[nDarkBlobNum + i].lLabel = nCurLabel;

		if (psBrightBlob[i].lArea == 0)
			continue;

		nBeginY = psBrightBlob[i].nTop;
		nEndY = psBrightBlob[i].nBottom + 1;
		nBeginX = psBrightBlob[i].nLeft;
		nEndX = psBrightBlob[i].nRight + 1;

		for (y = nBeginY; y < nEndY; y++)
		{
			for (x = nBeginX; x < nEndX; x++)
			{
				index = x + y * m_nSizeX;

				if (plBrightLabel[index] == psBrightBlob[i].lLabel)
				{
					plDarkLabel[index] = nCurLabel;
				}
			}
		}
	}

	nDarkBlobNum = nBlobNum;

	return 0;
}

// =========================================================
/** @fn		long CIBlob::MergeBetweenDarkBlobAndBrightBlobByDistance(SBlobInfo* psBlobs, short& nBlobNum, long* plLabel, CRect rtROI, float fDistanceThreshold, float fMinBrightDarkAreaContribution)
	@brief
	@param		SBlobInfo* psBlobs :
	@param		short& nBlobNum :
	@param		long* plLabel :
	@param		CRect rtROI :
	@param		float fDistanceThreshold :
	@param		float fMinBrightDarkAreaContribution :
	@return		:
	@date		: 2006-04-10 오후 5:50:05
	@author		:
	@warning	:
	@sa			:
	@note		:
*/ // ======================================================
long CIBlob::MergeBetweenDarkBlobAndBrightBlobByDistance(SBlobInfo* psBlobs, short& nBlobNum, long* plLabel, CRect rtROI, float fDistanceThreshold, float fMinBrightDarkAreaContribution)
{
	//FUNC_ENTRY_LOG("CIBlob::MergeBetweenDarkBlobAndBrightBlobByDistance()");

	long i, j;
	SBlobInfo* pBlob1;
	SBlobInfo* pBlob2;
	long nResult;

	CRect rt1, rt2, rttemp;

	m_vecMergeTable.reserve(nBlobNum + 1);
	m_vecMergeId.reserve(nBlobNum + 1);

	memset(m_pMergeId, 0, sizeof(UINT32) * (nBlobNum + 1));

	for (i = 0; i < nBlobNum + 1; i++)
		m_vecMergeTable[i].reserve(16);

	int nMaxMergeId = 0;
	int nCurMergeId;

	for (j = 0; j < nBlobNum; j++)
	{
		pBlob1 = &psBlobs[j];
		if (pBlob1->lArea == 0)
			continue;

		// Dark Blob 인 경우에만 유효
		if (pBlob1->nBrightDarkType != DARK)
			continue;

		rt1 = pBlob1->rtROI;

		for (i = j + 1; i < nBlobNum; i++)
		{
			pBlob2 = &psBlobs[i];
			if (pBlob2->lArea == 0)
				continue;

			// Bright Blob 인 경우에만 유효
			if (pBlob2->nBrightDarkType == DARK)
				continue;

			rt2 = pBlob2->rtROI;

			if (rt1.left - rt2.right > fDistanceThreshold)
				continue;
			if (rt2.left - rt1.right > fDistanceThreshold)
				continue;
			if (rt1.top - rt2.bottom > fDistanceThreshold)
				continue;
			if (rt2.top - rt1.bottom > fDistanceThreshold)
				continue;

			if (IsNeededMerge(*pBlob1, *pBlob2, fDistanceThreshold))
			{
				//{{ GetMerge Id
				if (m_pMergeId[j] == 0 && m_pMergeId[i] == 0)
				{
					nMaxMergeId++; // MergeId를 늘린다. MergeId는 1부터 시작된다.
					nCurMergeId = nMaxMergeId;
				}
				else if (m_pMergeId[j] != 0)
				{
					nCurMergeId = m_pMergeId[j];
				}
				else {
					nCurMergeId = m_pMergeId[i];
				}
				//}}

				std::vector<int>& vecMergeTable = m_vecMergeTable[nCurMergeId];

				if (m_pMergeId[j] == 0)
				{
					m_pMergeId[j] = nCurMergeId;
					vecMergeTable.push_back(j);
				}

				if (m_pMergeId[i] == 0)
				{
					m_pMergeId[i] = nCurMergeId;
					vecMergeTable.push_back(i);
				}
			}

		}

	}

	for (j = 1; j < nMaxMergeId + 1; j++)
	{
		std::vector<int>& vecMergeTable = m_vecMergeTable[j];

		pBlob1 = &psBlobs[vecMergeTable[0]];

		for (i = 1; i < vecMergeTable.size(); i++)
		{
			pBlob2 = &psBlobs[vecMergeTable[i]];

			nResult = MergeBlob(pBlob1, pBlob2, plLabel, m_nSizeX);
			if (nResult != 0)
				return nResult;
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

BOOL  CIBlob::IsNeededMerge(SBlobInfo& i_psObjInfo1, SBlobInfo& i_psObjInfo2, float fMergeDistance)
{
	float min_length1;
	float min_length2;
	float fdummy;

	int x, y;
	int detect_edge11, detect_edge12;
	int detect_edge21, detect_edge22;

	//{{ Blob1의 무게 중심에서 제일 가까운 Blob2의 Edge 를 찾는다
	fdummy = GetMinDistanceFromPointToBlob(i_psObjInfo1.nCenX, i_psObjInfo1.nCenY, i_psObjInfo2, detect_edge12);
	x = i_psObjInfo2.arptEdge[detect_edge12].x;
	y = i_psObjInfo2.arptEdge[detect_edge12].y;
	// Blob2의 Edge에서 Blob1의 가장 가까운 Edge를 찾는다
	min_length1 = GetMinDistanceFromPointToBlob(x, y, i_psObjInfo1, detect_edge11);
	//}}
	if (min_length1 < fMergeDistance) return TRUE;

	//{{ Blob2의 무게 중심에서 제일 가까운 Blob1의 Edge 를 찾는다
	fdummy = GetMinDistanceFromPointToBlob(i_psObjInfo2.nCenX, i_psObjInfo2.nCenY, i_psObjInfo1, detect_edge21);
	if (detect_edge21 == detect_edge11)
	{
		min_length2 = min_length1;
	}
	else
	{
		x = i_psObjInfo1.arptEdge[detect_edge21].x;
		y = i_psObjInfo1.arptEdge[detect_edge21].y;
		// Blob1의 Edge에서 Blob2의 가장 가까운 Edge를 찾는다
		min_length2 = GetMinDistanceFromPointToBlob(x, y, i_psObjInfo2, detect_edge22);
	}
	//}}

	if (min_length2 < fMergeDistance) return TRUE;

	return FALSE;

}


float CIBlob::GetBlobEdgeDistance(SBlobInfo& i_psObjInfo1, SBlobInfo& i_psObjInfo2)
{
	BOOL bFirst = TRUE;

	float min_length1;
	float min_length2;
	float fdummy;

	int x, y;
	int detect_edge;

	//{{ Blob1의 무게 중심에서 제일 가까운 Blob2의 Edge 를 찾는다
	fdummy = GetMinDistanceFromPointToBlob(i_psObjInfo1.nCenX, i_psObjInfo1.nCenY, i_psObjInfo2, detect_edge);
	x = i_psObjInfo2.arptEdge[detect_edge].x;
	y = i_psObjInfo2.arptEdge[detect_edge].y;
	// Blob2의 Edge에서 Blob1의 가장 가까운 Edge를 찾는다
	min_length1 = GetMinDistanceFromPointToBlob(x, y, i_psObjInfo1, detect_edge);
	//}}

	//{{ Blob2의 무게 중심에서 제일 가까운 Blob1의 Edge 를 찾는다
	fdummy = GetMinDistanceFromPointToBlob(i_psObjInfo2.nCenX, i_psObjInfo2.nCenY, i_psObjInfo1, detect_edge);
	x = i_psObjInfo1.arptEdge[detect_edge].x;
	y = i_psObjInfo1.arptEdge[detect_edge].y;
	// Blob1의 Edge에서 Blob2의 가장 가까운 Edge를 찾는다
	min_length2 = GetMinDistanceFromPointToBlob(x, y, i_psObjInfo2, detect_edge);
	//}}

	return min(min_length1, min_length2);
}


float CIBlob::GetMinDistanceFromPointToBlob(int x, int y, SBlobInfo& i_psObjInfo, int& min_edge)
{
	long min_length, length;
	float x2, y2;
	min_edge = -1;

	long nEdgeNum = i_psObjInfo.arptEdge.size();

	for (UINT n = 0; n < nEdgeNum; n++)
	{
		x2 = i_psObjInfo.arptEdge[n].x;
		y2 = i_psObjInfo.arptEdge[n].y;

		length = (x2 - x) * (x2 - x) + (y2 - y) * (y2 - y);

		if (n == 0 || length < min_length) {
			min_length = length;
			min_edge = n;
		}
	}

	return (float)sqrt((float)min_length);

}

long CIBlob::GetBoundaryPoints(long* i_plLabel, const CRect& rtROI, SBlobInfo* io_psObjInfo, short nNum)
{
	//FUNC_ENTRY_LOG("CIBlob::GetBoundaryPoints()");

	// Edge Array 를 초기화 하고,가장큰 Label 번호를 알아냅니다.
	long max_temp = -1;
	long objn;
	for (objn = 0; objn < nNum; objn++)
	{
		if (io_psObjInfo[objn].lLabel > max_temp)
			max_temp = io_psObjInfo[objn].lLabel;

		io_psObjInfo[objn].arptEdge.clear();
		io_psObjInfo[objn].arptEdge.reserve(
			(io_psObjInfo[objn].nBottom - io_psObjInfo[objn].nTop + 1)
			* (io_psObjInfo[objn].nRight - io_psObjInfo[objn].nLeft + 1) / 2);
	}

	std::vector<long> vecEdgeLabel(max_temp + 1);
	long* pEdgeLabel = &vecEdgeLabel[0];

	for (objn = 0; objn <= max_temp; objn++)
		pEdgeLabel[objn] = -1;
	for (objn = 0; objn < nNum; objn++)
		pEdgeLabel[io_psObjInfo[objn].lLabel] = objn;

	long x, y, x2, y2;

	for (objn = 0; objn < nNum; objn++)
	{
		SBlobInfo* pBlob = io_psObjInfo + objn;

		if (pBlob->lArea == 0)
			continue;

		CRect rtObj(pBlob->nLeft, pBlob->nTop, pBlob->nRight + 1, pBlob->nBottom + 1);

		long me = pBlob->lLabel;

		if (me > max_temp || pEdgeLabel[me] == -1)
			continue;

		for (y = rtObj.top; y < rtObj.bottom; y++)
		{
			for (x = rtObj.left; x < rtObj.right; x++)
			{
				if (me != i_plLabel[y * m_nSizeX + x])
					continue;

				// 주변에 하나라도 다른 레이블이 있으면
				// 현재 포인트를 등록한다.
				for (y2 = y - 1; y2 <= y + 1; y2++)
				{
					for (x2 = x - 1; x2 <= x + 1; x2++)
					{
						// x2, y2 가 이미지 범위를 벗어나는 경우에 대한 예외처리가 빠져있음.
						// 고려하기 바람.

						if (me != i_plLabel[y2 * m_nSizeX + x2])
						{
							pBlob->arptEdge.push_back(CPoint(x, y));
							y2 = y + 2;
							break;
						}
					}
				}
			}
		}
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
float CIBlob::GetBlobROIDistance(SBlobInfo& i_psObjInfo1, SBlobInfo& i_psObjInfo2)
{
	int left1, right1, top1, bottom1;
	int left2, right2, top2, bottom2;

	left1 = i_psObjInfo1.nLeft;
	right1 = i_psObjInfo1.nRight;
	top1 = i_psObjInfo1.nTop;
	bottom1 = i_psObjInfo1.nBottom;
	left2 = i_psObjInfo2.nLeft;
	right2 = i_psObjInfo2.nRight;
	top2 = i_psObjInfo2.nTop;
	bottom2 = i_psObjInfo2.nBottom;

	float xdist, ydist, xydist;

	// x distance
	if (left1 < left2)
	{
		if (left2 < right1)
			xdist = 0;
		else
			xdist = (float)left2 - right1;
	}
	else
	{
		if (left1 < right2)
			xdist = 0;
		else
			xdist = (float)left1 - right2;
	}

	// y distance
	if (top1 < top2)
	{
		if (top2 < bottom1)
			ydist = 0;
		else
			ydist = (float)top2 - bottom1;
	}
	else
	{
		if (top1 < bottom2)
			ydist = 0;
		else
			ydist = (float)top1 - bottom2;
	}

	// 둘 중 큰 것 
	xydist = xdist > ydist ? xdist : ydist;

	return xydist;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
long CIBlob::MergeBlob(SBlobInfo* psBlob1, SBlobInfo* psBlob2, long* plLabel, long nImageX)
{

	psBlob1->nLeft = min(psBlob1->nLeft, psBlob2->nLeft);
	psBlob1->nTop = min(psBlob1->nTop, psBlob2->nTop);
	psBlob1->nRight = max(psBlob1->nRight, psBlob2->nRight);
	psBlob1->nBottom = max(psBlob1->nBottom, psBlob2->nBottom);

	psBlob1->fCenX = (psBlob2->fCenX * psBlob2->lArea + psBlob1->fCenX * psBlob1->lArea)
		/ (psBlob2->lArea + psBlob1->lArea);
	psBlob1->fCenY = (psBlob2->fCenY * psBlob2->lArea + psBlob1->fCenY * psBlob1->lArea)
		/ (psBlob2->lArea + psBlob1->lArea);

	psBlob1->rtROI = CRect(psBlob1->nLeft, psBlob1->nTop, psBlob1->nRight, psBlob1->nBottom);

	psBlob1->nCenX = short(psBlob1->fCenX + 0.5f);
	psBlob1->nCenY = short(psBlob1->fCenY + 0.5f);
	psBlob1->lArea += psBlob2->lArea;

	psBlob1->arptEdge.insert(psBlob1->arptEdge.end(), psBlob2->arptEdge.begin(), psBlob2->arptEdge.end());
	psBlob2->arptEdge.clear();


	psBlob2->lArea = 0;

	return EqualizeLabel(psBlob1, psBlob2, plLabel, nImageX);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
long CIBlob::EqualizeLabel(SBlobInfo* psBlob1, SBlobInfo* psBlob2, long* plLabel, long nImageX)
{
	int i, j;

	long* plTemp = plLabel + psBlob2->nTop * nImageX + psBlob2->nLeft;
	long lOffset = nImageX - (psBlob2->nRight + 1 - psBlob2->nLeft);

	for (i = psBlob2->nTop; i <= psBlob2->nBottom; i++)
	{
		for (j = psBlob2->nLeft; j <= psBlob2->nRight; j++, plTemp++)
		{
			if ((*plTemp) == psBlob2->lLabel)
				(*plTemp) = psBlob1->lLabel;
		}

		plTemp += lOffset;
	}

	return 0;
}


// =========================================================
/** @fn		long CIBlob::CalcBlobPosition(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short& nBlobNum)
	@brief		: Blob 의 무게 중심을 계산
	@param		const BYTE* pbyImage :
	@param		SBlobInfo* psBlob :
	@param		short& nBlobNum :
	@return		:
	@date		: 2006-04-10 오후 3:18:42
	@author		:
	@warning	:
	@sa			:
	@note		:
*/ // ======================================================
long CIBlob::CalcBlobPosition(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short& nBlobNum)
{
	//FUNC_ENTRY_LOG("CIBlob::CalcBlobPosition()");

	for (long i = 0; i < nBlobNum; i++)
	{
		if (psBlob[i].lArea == 0)
			continue;

		CalcOneBlobPosition(plLabel, psBlob[i]);
	}

	return 0;
}

void  CIBlob::CalcOneBlobPosition(const long* plLabel, SBlobInfo& sBlob)
{
	short x, y;

	long nSumX = 0;
	long nSumY = 0;
	long nCount = 0;
	long cp;

	for (y = sBlob.nTop; y <= sBlob.nBottom; y++)
	{
		cp = y * m_nSizeX + sBlob.nLeft;
		for (x = sBlob.nLeft; x <= sBlob.nRight; x++, cp++)
		{
			if (sBlob.lLabel == plLabel[cp])
			{
				nSumX += x;
				nSumY += y;
				nCount++;
			}
		}
	}

	if (nCount == 0)
	{
		sBlob.fCenX = 0;
		sBlob.fCenY = 0;
		sBlob.nCenX = 0;
		sBlob.nCenY = 0;
	}
	else
	{
		sBlob.fCenX = (float)nSumX / nCount;
		sBlob.fCenY = (float)nSumY / nCount;
		sBlob.nCenX = (long)(sBlob.fCenX + 0.5f);
		sBlob.nCenY = (long)(sBlob.fCenY + 0.5f);
	}
}

// =========================================================
/** @fn		long CIBlob::CalcBlobContrast(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short& nBlobNum, float *pfBackGroundGV, const std::vector<CRect> &rtROIs, float fMinAreaDarkContrast, float fMinAreaBrightContrast, float fMinAreaHybridContrast, float fMinBrightDarkAreaContribution)
	@brief		: Blob Contrast 를 계산
	@param		const BYTE* pbyImage :
	@param		const long* plLabel :
	@param		SBlobInfo* psBlob :
	@param		short& nBlobNum :
	@param		float *pfBackGroundGV :
	@param		const std::vector<CRect> &rtROIs :
	@param		float fMinAreaDarkContrast :
	@param		float fMinAreaBrightContrast :
	@param		float fMinAreaHybridContrast :
	@param		float fMinBrightDarkAreaContribution :
	@return		:
	@date		: 2006-04-11 오전 11:09:26
	@author		:
	@warning	:
	@sa			:
	@note		:
*/ // ======================================================
long CIBlob::CalcBlobContrast(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short& nBlobNum, float* pfBackGroundGV, const std::vector<CRect>& rtROIs, float fMinAreaDarkContrast, float fMinAreaBrightContrast, float fMinAreaHybridContrast, float fMinBrightDarkAreaContribution)
{
	//FUNC_ENTRY_LOG("CIBlob::CalcBlobContrast()");

	long index;
	long x, y, value;
	long nROI = rtROIs.size();
	long nHistogram[256], nHistoDist[256];
	long nBackgroundGV;
	float fMinAreaContrast;

	int i, j;


	for (j = 0; j < nBlobNum; j++)
	{
		SBlobInfo& Blob = psBlob[j];

		// 블럽 fContrast 초기화
		Blob.fKeyContrast = 0;
		Blob.fAverageContrast = 0;

		if (Blob.lArea == 0)
			continue;

		// 블럽의 위치에 해당하는 Background 의 Intensity
		long back_index = -1;

		for (i = 0; i < nROI; i++)
		{
			if (rtROIs[i].PtInRect(CPoint(Blob.fCenX, Blob.fCenY)))
			{
				back_index = i;
				nBackgroundGV = (long)(pfBackGroundGV[back_index] + 0.5f);
				break;
			}
		}

		if (back_index == -1)
			continue;

		// Histogram 계산
		long nTotalCount = 0;
		long nDarkCount = 0;
		long nBrightCount = 0;

		for (i = 0; i < 256; i++)	// 초기화
		{
			nHistogram[i] = 0;
			nHistoDist[i] = 0;
		}

		for (y = Blob.nTop; y <= Blob.nBottom; y++)
		{
			for (x = Blob.nLeft; x <= Blob.nRight; x++)
			{
				index = x + y * m_nSizeX;

				if (plLabel[index] == Blob.lLabel)
				{
					value = pbyImage[index];
					nHistogram[value]++;
					nTotalCount++;
					if (value <= nBackgroundGV)
						nDarkCount++;
					else
						nBrightCount++;
				}
			}
		}

		// nBrightDarkType 및 fContrast 계산
		float fBrightContribution = nBrightCount * 100.f / nTotalCount;
		float fDarkContribution = nDarkCount * 100.f / nTotalCount;

		if (fBrightContribution >= fMinBrightDarkAreaContribution && fDarkContribution >= fMinBrightDarkAreaContribution)	// Hybrid
		{
			Blob.nBrightDarkType = HYBRID;
			for (i = 0; i < 256; i++)
				nHistoDist[abs(i - nBackgroundGV)] += nHistogram[i];

			fMinAreaContrast = fMinAreaHybridContrast;
		}
		else if (fBrightContribution >= fDarkContribution)	// Bright
		{
			Blob.nBrightDarkType = BRIGHT;
			for (i = nBackgroundGV; i < 256; i++)
				nHistoDist[abs(i - nBackgroundGV)] += nHistogram[i];

			fMinAreaContrast = fMinAreaBrightContrast;
		}
		else	// Dark
		{
			Blob.nBrightDarkType = DARK;
			for (i = 0; i <= nBackgroundGV; i++)
				nHistoDist[abs(i - nBackgroundGV)] += nHistogram[i];

			fMinAreaContrast = fMinAreaDarkContrast;
		}

		long nAccHisto = 0;

		// Find Key Contrast
		for (i = 255; i >= 0; i--)
		{
			nAccHisto += nHistoDist[i];
			if (nAccHisto >= fMinAreaContrast)
			{
				Blob.fKeyContrast = i;
				//				TRACE("*Find Key Contrast %d\n",i);
				break;
			}
		}

		// Find Average Contrast
		nAccHisto = 0;
		for (i = 255; i >= 0; i--)
		{
			nAccHisto += i * nHistoDist[i];
		}

		Blob.fAverageContrast = nAccHisto / float(nTotalCount);
	}

	return 0;
}


// =========================================================
/** @fn		long CIBlob::CalcBlobEdgeEnergy(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short nBlobNum, const BYTE *pbyValidPattern);
	@brief
	@param		const BYTE* pbyImage :
	@param		const long* plLabel :
	@param		SBlobInfo* psBlob :
	@param		short nBlobNum :
	@param		const BYTE *pbyValidPattern :
	@return		:
	@date		: 2006-06-17 오전 11:22:33
	@author		:
	@warning	:
	@sa			:
	@note		:
*/ // ======================================================
long CIBlob::CalcBlobEdgeEnergy(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short nBlobNum, const BYTE* pbyValidPattern)
{
	//FUNC_ENTRY_LOG("CIBlob::CalcBlobEdgeEnergy()");

	long x = 0;
	long y = 0;
	long idx = 0;

	long nBeginY = 0;
	long nEndY = 0;
	long nBeginX = 0;
	long nEndX = 0;

	long nBlobLabel = 0;
	long nEdgeEnergy = 0;

	if (pbyValidPattern)	// Mask Pattern 이 존재하는 경우
	{
		const BYTE* pbyDownImage = pbyImage + m_nSizeX;
		const BYTE* pbyRightImage = pbyImage + 1;

		const BYTE* pbyDownPattern = pbyValidPattern + m_nSizeX;
		const BYTE* pbyRightPattern = pbyValidPattern + 1;

		for (long nBlob = 0; nBlob < nBlobNum; nBlob++)
		{
			SBlobInfo& Blob = psBlob[nBlob];

			Blob.fEdgeEnergy = 0.f;

			if (Blob.lArea == 0)
				continue;

			nBeginY = Blob.nTop;
			nEndY = min(Blob.nBottom + 1, m_nSizeY);
			nBeginX = Blob.nLeft;
			nEndX = min(Blob.nRight + 1, m_nSizeX);

			nBlobLabel = Blob.lLabel;
			nEdgeEnergy = 0;

			for (y = nBeginY; y < nEndY; y++)
			{
				for (x = nBeginX; x < nEndX; x++)
				{
					idx = y * m_nSizeX + x;

					if (plLabel[idx] != nBlobLabel)
						continue;

					if (pbyDownPattern[idx])
					{
						nEdgeEnergy += abs(pbyImage[idx] - pbyDownImage[idx]);
					}

					if (pbyRightPattern[idx])
					{
						nEdgeEnergy += abs(pbyImage[idx] - pbyRightImage[idx]);
					}
				}
			}

			Blob.fEdgeEnergy = float(nEdgeEnergy) / Blob.lArea;
		}
	}
	else
	{
		const BYTE* pbyDownImage = pbyImage + m_nSizeX;
		const BYTE* pbyRightImage = pbyImage + 1;

		for (long nBlob = 0; nBlob < nBlobNum; nBlob++)
		{
			SBlobInfo& Blob = psBlob[nBlob];

			Blob.fEdgeEnergy = 0.f;

			if (Blob.lArea == 0)
				continue;

			nBeginY = Blob.nTop;
			nEndY = min(Blob.nBottom + 1, m_nSizeY);
			nBeginX = Blob.nLeft;
			nEndX = min(Blob.nRight + 1, m_nSizeX);

			nBlobLabel = Blob.lLabel;
			nEdgeEnergy = 0;

			for (y = nBeginY; y < nEndY; y++)
			{
				for (x = nBeginX; x < nEndX; x++)
				{
					idx = y * m_nSizeX + x;

					if (plLabel[idx] != nBlobLabel)
						continue;

					nEdgeEnergy += abs(pbyImage[idx] - pbyDownImage[idx]);
					nEdgeEnergy += abs(pbyImage[idx] - pbyRightImage[idx]);
				}
			}

			Blob.fEdgeEnergy = float(nEdgeEnergy) / Blob.lArea;
		}
	}

	return 0;
}


// =========================================================
/** @fn		long CIBlob::CalcBlobWidthLength(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short& nBlobNum)
	@brief		: 블럽의 장축 방향 길이와 단축 방향 길이를 계산
	@param		const long* plLabel :
	@param		SBlobInfo* psBlob :
	@param		short& nBlobNum :
	@return		:
	@date		: 2006-04-11 오후 1:26:26
	@author		:
	@warning	: 이 함수가 작동하기 전에 Blob Edge 가 미리 계산되어 있어야 함
	@sa			:
	@note		:
*/ // ======================================================
/*long CIBlob::CalcBlobWidthLength(const long* plLabel, SBlobInfo* psBlob, short& nBlobNum)
{
	//FUNC_ENTRY_LOG("CIBlob::CalcBlobWidthLength()");

	double dTheta;
	float fx, fy, f1, f2;
	float fxmin, fxmax, fymin, fymax;
	float fCosVal, fSinVal;

	for(long i = 0; i < nBlobNum; i++)
	{
		SBlobInfo &Blob = psBlob[i];

		if(Blob.lArea == 0)
		{
			Blob.fWidth = 0;
			Blob.fLength = 0;
			continue;
		}

		long nEdgeSize = Blob.arptEdge.size();
		CPoint *pEdges = &Blob.arptEdge[0];

		CPI_Geometry::MoveToMassCenter(pEdges, nEdgeSize);

		Blob.fWidth = 100000000.f;
		Blob.fLength = -100000000.f;

		for(long itr = 0 ; itr < 2 ; itr++)
		{
			if(itr == 0)
				CPI_Geometry::GetBinaryDirection1(pEdges, nEdgeSize, dTheta);
			else
				CPI_Geometry::GetBinaryDirection2(pEdges, nEdgeSize, dTheta);

			fCosVal = cos(dTheta);
			fSinVal = sin(dTheta);

			fxmin = fymin = 10000000000;
			fxmax = fymax = -10000000000;

			for(long j = 0 ; j < nEdgeSize ; j++)
			{
				fx = fCosVal * pEdges[j].x + fSinVal * pEdges[j].y - 1;
				fy = fSinVal * pEdges[j].x - fCosVal * pEdges[j].y - 1;

				if(fx < fxmin)
					fxmin = fx;
				if(fx > fxmax)
					fxmax = fx;

				if(fy < fymin)
					fymin = fy;
				if(fy > fymax)
					fymax = fy;
			}

			f1 = fxmax - fxmin;
			f2 = fymax - fymin;

			Blob.fWidth = max(1.f, min(Blob.fWidth, min(f1, f2)));
			Blob.fLength = max(1.f, max(Blob.fLength, max(f1, f2)));
		}
	}

	return 0;
}




/*
/*
추후 참고용. 2006 강민구.
작동을 보장못함.
long CIBlob::DoBlobGray_ipp( const BYTE* pbyImage, float* pfLabel, const CRect &rtROI,
							   short nBlobIntensity,
							   int nNoiseAreaThreshold,
							   BOOL is4Neibor, BOOL isLabeling, short &nBlobNum )
{
	//FUNC_ENTRY_LOG("CIBlob::DoBlobGray_ipp()");

	if(pbyImage == NULL) return -1;
	if(rtROI.IsRectNull() == TRUE) return -1;

	CRect rtNewROI(rtROI);
	::VerifyROI(rtNewROI, CRect(0, 0, m_nSizeX, m_nSizeY));


	//=================================
	// 1. 영역 정보 준비
	//=================================
	IppiSize ippSize;
	IppStatus res;
	ippSize.width = rtNewROI.Width();
	ippSize.height = rtNewROI.Height();

	Ipp8u *pbSrc = (Ipp8u*)(pbyImage + (rtNewROI.top * m_nSizeX + rtNewROI.left));
	Ipp32f *pfLabelDst = (Ipp32f*)( pfLabel + (rtNewROI.top * m_nSizeX + rtNewROI.left));

	ippiConvert_8u32f_C1R( pbSrc, m_nSizeX, pfLabelDst, m_nSizeX, ippSize );

	float *pfCurImg;

	int nNeedBufSize;	// 요구하는 버퍼의 사이즈
	ippiFloodFillGetSize(ippSize, &nNeedBufSize);
	if( m_nBufferSize < nNeedBufSize) // 버퍼는 원하는 사이즈만큼 있어야 한다.
	{
		delete [] m_pBuffer;
		m_pBuffer = new BYTE[nNeedBufSize];
		m_nBufferSize = nNeedBufSize;
	}
	//}}


	//=================================
	// 2. Blob한다.
	//=================================
	IppiConnectedComp ippConn;	// 찾은 블랍 정보
	IppiPoint ippPt;		// 블랍 검색을 시작할 위치

	const int nBackground = 0;
	const int nMinLabel = 1;
	const int nMaxLabel = 250;


	// 등가 테이블 초기화
	m_nNewLabel = nMinLabel; // 물체의 레이블은 1부터 매긴다.
	nBlobNum = 0;

	int x, y;
	float fFindValue = (float)nBlobIntensity;


	for(y=0; y<ippSize.height; y++)
	{
		pfCurImg = pfLabelDst + y*m_nSizeX;
		ippPt.y = y;

		for(x=0; x<ippSize.width; x++, pfCurImg++)
		{

			if( *pfCurImg == fFindValue )
			{
				ippPt.x = x;

				if(is4Neibor)
					res = ippiFloodFill_4Con_32f_C1IR( pfLabelDst, m_nSizeX, ippSize, ippPt, m_nNewLabel, &ippConn, (void*)m_pBuffer );
				else
					res = ippiFloodFill_8Con_32f_C1IR( pfLabelDst, m_nSizeX, ippSize, ippPt, m_nNewLabel, &ippConn, (void*)m_pBuffer);

				if(res != ippStsNoErr)	// Blob하는데 에러나면 책임 못짐
				{
					ASSERT(0);
					return -1;
				}


				if(ippConn.area < nNoiseAreaThreshold) // Noise면 nBackground 으로 Clear 시킨다.
				{
					if(is4Neibor)
						res = ippiFloodFill_4Con_32f_C1IR( pfLabelDst, m_nSizeX, ippSize, ippPt, nBackground, &ippConn, (void*)m_pBuffer );

					else
						res = ippiFloodFill_8Con_32f_C1IR( pfLabelDst, m_nSizeX, ippSize, ippPt, nBackground, &ippConn, (void*)m_pBuffer);

				}else{

					SetEqTable_with_ippCon( m_psBlobPartInfo[nBlobNum], ippConn );
					m_nNewLabel ++;
					nBlobNum ++;

				} //}} end of if(ippConn.area < nNoiseAreaThreshold)

			}//}}if( *pbyCurImg == nFindValue )

		}//}} x

	}//}} y

	return 0;
}


long CIBlob::DoBlobGray_ipp2( BYTE* pbiSrcDst, const CRect &rtROI, short nBlobIntensity,
							   int nNoiseAreaThreshold,
							   BOOL is4Neibor, BOOL isLabeling, short &nBlobNum )
{
	//FUNC_ENTRY_LOG("CIBlob::DoBlobGray_ipp2()");

	if(pbiSrcDst == NULL) return -1;
	if(rtROI.IsRectNull() == TRUE) return -1;

	CRect rtNewROI(rtROI);
	::VerifyROI(rtNewROI, CRect(0, 0, m_nSizeX, m_nSizeY));


	//=================================
	// 1. 영역 정보 준비
	//=================================
	IppiSize ippSize;
	IppStatus res;
	ippSize.width = rtNewROI.Width();
	ippSize.height = rtNewROI.Height();

	Ipp8u *pbSrc = (Ipp8u*)(pbiSrcDst + (rtNewROI.top * m_nSizeX + rtNewROI.left));
	BYTE* pbyCurImg;

	int nNeedBufSize;	// 요구하는 버퍼의 사이즈
	ippiFloodFillGetSize(ippSize, &nNeedBufSize);
	if( m_nBufferSize < nNeedBufSize) // 버퍼는 원하는 사이즈만큼 있어야 한다.
	{
		delete [] m_pBuffer;
		m_pBuffer = new BYTE[nNeedBufSize];
		m_nBufferSize = nNeedBufSize;
	}
	//}}


	//=================================
	// 2. Blob한다.
	//=================================
	IppiConnectedComp ippConn;	// 찾은 블랍 정보
	IppiPoint ippPt;		// 블랍 검색을 시작할 위치

	const int nBackground = 0;
	const int nMinLabel = 1;
	const int nMaxLabel = 250;

	std::vector<std::vector<int> >  vecBlob_LabelTable( nMaxLabel+1 );

	BOOL IsStatusBlobConflict = FALSE;
	int  nConflictNum = 0;


	// 등가 테이블 초기화
	m_nNewLabel = nMinLabel; // 물체의 레이블은 1부터 매긴다.
	nBlobNum = 0;

	int x, y;
	int nFindValue = nBlobIntensity;


	for(y=0; y<ippSize.height; )
	{
		pbyCurImg = pbSrc + y*m_nSizeX;

		for(x=0; x<ippSize.width; )
		{

			if( *pbyCurImg == nFindValue ) // nFindValue 251 ~ 255만 가능하다.
			{

				ippPt.y = y;
				ippPt.x = x;

				if(is4Neibor)
					res = ippiFloodFill_4Con_8u_C1IR( pbSrc, m_nSizeX, ippSize, ippPt, m_nNewLabel, &ippConn, (void*)m_pBuffer );
				else
					res = ippiFloodFill_8Con_8u_C1IR( pbSrc, m_nSizeX, ippSize, ippPt, m_nNewLabel, &ippConn, (void*)m_pBuffer);

				if(res != ippStsNoErr)	// Blob하는데 에러나면 책임 못짐
				{
					ASSERT(0);
					return -1;
				}


				if(ippConn.area < nNoiseAreaThreshold) // Noise면 nBackground 으로 Clear 시킨다.
				{
					if(is4Neibor)
						res = ippiFloodFill_4Con_8u_C1IR( pbSrc, m_nSizeX, ippSize, ippPt, nBackground, &ippConn, (void*)m_pBuffer );

					else
						res = ippiFloodFill_8Con_8u_C1IR( pbSrc, m_nSizeX, ippSize, ippPt, nBackground, &ippConn, (void*)m_pBuffer);

				}else{

					if(isLabeling){

						std::vector<int>& vecLabelTable = vecBlob_LabelTable[m_nNewLabel];

						// 현재 찾은 Blob과 Label이 같은 Blob이 있는지 조사하고, 영역이 겹치는지 확인한다.
						BOOL  IsInterSect = IsIntersect_BlobLabel( m_nNewLabel, ippConn, vecLabelTable );

						if( IsInterSect )
						{
							nFindValue = m_nNewLabel; // 이전에 FloodFill한 Label을 찾아야 한다.
							m_nNewLabel = ++m_nNewLabel % (nMaxLabel+1) ; // Label만 1올리고 다시 계산 한다.

							if(IsStatusBlobConflict) // 현재 Conflict 상황이면, Count 1 증가
							{
								nConflictNum++;
								if( nConflictNum > nMaxLabel )
								{
									return -1; // Blob 실패 !!! 같은 영역내에 250개 이상의 blob이 혼재하고 있는 경우.
								}
							}else{
								IsStatusBlobConflict = TRUE;
								nConflictNum = 0;
							}

							continue;
						}

						// 영역이 겹치지 않으면 정상적으로 Label을 올린다.
						SetEqTable_with_ippCon( m_psBlobPartInfo[nBlobNum], ippConn );

						vecLabelTable.insert( vecLabelTable.end(), nBlobNum );
						vecLabelTable.clear();

						m_nNewLabel = ++m_nNewLabel % (nMaxLabel+1) ;

					}//}} end of if(isLabeling)

					nBlobNum ++;

				} //}} end of if(ippConn.area < nNoiseAreaThreshold)

				nFindValue = nBlobIntensity;

			}//}}if( *pbyCurImg == nFindValue )

			x++;
			pbyCurImg++;

		}//}} x

		y++;

	}//}} y

	return 0;
}

void  CIBlob::SetEqTable_with_ippCon( SBlobPartInfo& sBlobPartInfo, IppiConnectedComp &ippConn )
{
	sBlobPartInfo.lArea = ippConn.area;
	sBlobPartInfo.lEqLabel = ippConn.value;

	sBlobPartInfo.nLeft = ippConn.rect.x;
	sBlobPartInfo.nTop = ippConn.rect.y;
	sBlobPartInfo.nRight = sBlobPartInfo.nLeft + ippConn.rect.width;
	sBlobPartInfo.nBottom = sBlobPartInfo.nTop + ippConn.rect.height ;
}


//////////////////////////////////////////////////////////////////////////////////////////////
BOOL  CIBlob::IsIntersect_BlobLabel( int nCurLabel, IppiConnectedComp &ippConn, std::vector<int> &vecLabelTable )
{
	BOOL IsInterSect;
	int nSameBlobNum = vecLabelTable.size(); // 현재 찾은 Blob과 Label이 같은 Blob의 수.

	int *pLabelTable = &vecLabelTable[0];

	int nBlobId;

	if( nSameBlobNum > 0)
	{
		for(int i=0;i<nSameBlobNum;i++)
		{
			nBlobId = pLabelTable[i];

			SBlobPartInfo &OldEqTable = m_psBlobPartInfo[nBlobId];
			IppiRect &CurIppiRt = ippConn.rect;

			CRect rtOld( OldEqTable.nLeft, OldEqTable.nTop, OldEqTable.nRight, OldEqTable.nBottom );
			CRect rtCur( CurIppiRt.x, CurIppiRt.y, CurIppiRt.x+CurIppiRt.width, CurIppiRt.y+CurIppiRt.height );
			CRect rectInter;

			IsInterSect = rectInter.IntersectRect(rtOld, rtCur);

			if( IsInterSect ) // 현재 Label이 같은 영역내에서 겹치면, Label번호를 1증가 시켜서 다시 Label을 구한다.
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
*/