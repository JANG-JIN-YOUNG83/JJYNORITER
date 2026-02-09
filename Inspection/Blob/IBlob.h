#pragma once

#include "DataStructs.h"
#include "BlobStructures.h"

#define NEIGHBOR	0  // 0:4-connected
// 1:8-connected

class CArbitraryRect;
struct SBlobInfo;

class CIBlob
{

public:
	CIBlob();
	~CIBlob();


public: // do something		

	long GetSafeBlob(BYTE* i_pbiImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, const CRect& rtROI2, CRect& rtSafeROI);
	long DoBlob(BYTE* i_pbyImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, short nThreshold = 255);
	long DoBlobGray(BYTE* pbyImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, const short nValue);
	long DoBlobGrayHigh(BYTE* pbyImage, long* o_plLabel, short nSizeX, short nSizeY, const CRect& rtROI, const short nValue);

	long MergeBlobsByDistance(SBlobInfo* psBlobs, short& nBlobNum, long* plLabel, CRect rtROI, float fDistanceThreshold);
	long MixDarkBlobAndBrightBlob(SBlobInfo* psDarkBlob, short& nDarkBlobNum, long* plDarkLabel, SBlobInfo* psBrightBlob, short& nBrightBlobNum, long* plBrightLabel);
	long MergeBetweenDarkBlobAndBrightBlobByDistance(SBlobInfo* psBlobs, short& nBlobNum, long* plLabel, CRect rtROI, float fDistanceThreshold, float fMinBrightDarkAreaContribution);

	long CalcBlobPosition(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short& nBlobNum);
	long CalcBlobContrast(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short& nBlobNum, float* pfBackGroundGV, const std::vector<CRect>& rtROIs, float fMinAreaDarkContrast, float fMinAreaBrightContrast, float fMinAreaHybridContrast, float fMinBrightDarkAreaContribution);
	long CalcBlobEdgeEnergy(const BYTE* pbyImage, const long* plLabel, SBlobInfo* psBlob, short nBlobNum, const BYTE* pbyValidPattern);
	long CalcBlobWidthLength(const long* plLabel, SBlobInfo* psBlob, short& nBlobNum);

	//김일남::블럽 계산이 끝난 후 두 블럽을 Merge 시켜야 할 필요가 있을 때 블럽 정보 및 Label 정보를 수정
	long MergeBlob(SBlobInfo* psBlob1, SBlobInfo* psBlob2, long* plLabel, long nImageX);

	short GetSizeX() { return m_nSizeX; };
	short GetSizeY() { return m_nSizeY; };


public: // get functions
	long GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, short nROIOffset, long lAreaThresh, short& nNum);
	long GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, short nROIOffset, long lAreaThresh, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum);
	long GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, CRect rtExcluded, short nROIOffset, long lAreaThresh, float fAreaRatio, float fLengthRatioUL, float fLengthRatioLL, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum);
	long GetBlobs(long* i_plBuffer, SBlobInfo* psObjInfo, const CRect& rtROI, CRect rtMerge, CRect rtExcluded, short nROIOffset, long lAreaThresh, float fAreaRatio, float fLengthRatioUL, float fLengthRatioLL, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum);
	long GetBlobs(SBlobInfo* psObjInfo, const CRect& rtROI, short nROIOffset, long lAreaThresh, float fAreaRatio, float fLengthRatioUL, float fLengthRatioLL, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL, short& nNum);
	// Vision Align Func
	long GetBlobs(SBlobInfo* psObjInfo, long lAreaThresh, short& nNum, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL);

	float GetMinDistanceFromPointToBlob(int x, int y, SBlobInfo& i_psObjInfo, int& min_edge);
	float GetBlobEdgeDistance(SBlobInfo& i_psObjInfo1, SBlobInfo& i_psObjInfo2);
	float GetBlobROIDistance(SBlobInfo& i_psObjInfo1, SBlobInfo& i_psObjInfo2);
	long  GetBoundaryPoints(long* i_plLabel, const CRect& rtROI, SBlobInfo* io_psObjInfo, short nNum);



private: // internally only used

	long  _DoBlob_GetCurrentLabel(short x, short y, UINT32 nLeftLabel, UINT32 nUpperLabel, UINT32& nCurLabel);
	UINT32 _DoBlob_uOnMetTwoLabel(UINT32 nLabel1, UINT32 nLabel2);
	UINT32 _DoBlob_CombineMergeTable(UINT32 nMergeID1, UINT32 nMergeID2);
	long _DoBlob_MergeBlobPartInfo(SBlobPartInfo* psBlob1, SBlobPartInfo* psBlob2);
	long _DoBlob_MergeLabel(SBlobPartInfo* psBlob1, SBlobPartInfo* psBlob2, long* plLabel, const long nImageX, const UINT32 nLabel1, const UINT32 nLabel2);
	long  _DoBlob_InitializeBlobPartInfo(SBlobPartInfo& oBlobPartInfo, UINT32 nLabel, short x, short y);


	BOOL  _GetBlob_CheckBlobInROI(SBlobPartInfo& sBlobPartInfo, const CRect& rtROI, short nOffset);
	BOOL  _GetBlob_CheckArea(const SBlobPartInfo& sBlobPartInfo, long lAreaThresh);
	BOOL  _GetBlob_CheckAreaRatio(const SBlobPartInfo& sBlobPartInfo, float fAreaRatioThresh);
	BOOL  _GetBlob_CheckLengthRatio(const SBlobPartInfo& sBlobPartInfo, float fLengthRatioUL, float fLengthRatioLL);
	BOOL  _GetBlob_CheckBlobSize(const SBlobPartInfo& sBlobPartInfo, short nWidthUL, short nWidthLL, short nHeightUL, short nHeightLL);
	void  _GetBlob_SetObjectInfo(const SBlobPartInfo& sBlobPartInfo, UINT32 nLabel, SBlobInfo& oBlobInfo);


	BOOL   IsNeededMerge(SBlobInfo& i_psObjInfo1, SBlobInfo& i_psObjInfo2, float fMergeDistance);


	long   EqualizeLabel(SBlobInfo* psBlob1, SBlobInfo* psBlob2, long* plLabel, long nImageX);

	void   ResetBlobPartInfo(SBlobPartInfo& oBlobPartInfo);

	void  CalcOneBlobPosition(const long* plLabel, SBlobInfo& sBlob);

	int  _MergeBlob_MakeNeighbor(float fDistance, int* pBuffer);

private: // Variables
	int  m_nMaxBlobNum;
	int  m_nMaxMergeTableNum;
	UINT32 m_nNewLabel;
	UINT32 m_nNewMergeId;
	short	m_nSizeX;
	short	m_nSizeY;
	long	m_lSize;
	long* m_plLabel;

	std::vector<SBlobPartInfo>		m_vecBlobPartInfo;
	std::vector<std::vector<int> >	m_vecMergeTable;
	std::vector<UINT32>				m_vecMergeId;

	SBlobPartInfo* m_psBlobPartInfo;
	UINT32* m_pMergeId;

	void* m_pBuffer;
	int    m_nBufferSize;
};

