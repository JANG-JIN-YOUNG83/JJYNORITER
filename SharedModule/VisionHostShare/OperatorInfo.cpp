#include "stdafx.h"
#include "OperatorInfo.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

OperatorInfo::OperatorInfo()
{
	nOperatorMode = OPERATOR_RUN_STOP;

	strReceipeName = _T("");
	strEquipmentID = _T("");

	strLotID = _T("");
	strRollID = _T("1");

	nModel_index = 0;
	nTotalCount = nokCount = nngCount = nInvalid = 0;
	nDimensionNgCount = nDefectNgCount = nDDTotalCount = 0;

	nEnableNgMarkPrint = 1;
	bFeeder = FALSE;
	dImageSaveRatioX = 1;
	dImageSaveRatioY = 1;

	nBurrCount = 0;
}

OperatorInfo::~OperatorInfo()
{
}

void OperatorInfo::DataCountSetZero()
{
	nTotalCount = nokCount = nngCount = nInvalid = 0;
	nDimensionNgCount = nDefectNgCount = nDDTotalCount = 0;
	nDummyCount = 0;
	nBurrCount = 0;
}

void OperatorInfo::GetResultCount(int* TotalCount, int* ngCount, int* okcount, int* Invalid, int *DDTotalCount, int *DimensionCount, int *DefectCount)
{
	mutexJD.Lock();

	*TotalCount = nTotalCount;
	*ngCount = nngCount;
	*okcount = nokCount;
	*Invalid = nInvalid;

	*DimensionCount = nDimensionNgCount;
	*DefectCount = nDefectNgCount;
	*DDTotalCount = nDDTotalCount;

	mutexJD.Unlock();
}

void OperatorInfo::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(nOperatorMode);
	ar.Serialize_Element(strReceipeName);
	ar.Serialize_Element(strEquipmentID);

	ar.Serialize_Element(strLotID);
	ar.Serialize_Element(strRollID);
	ar.Serialize_Element(bFeeder);

	ar.Serialize_Element(nStabilizationCount);
	ar.Serialize_Element(nContinuePitchCount);
	ar.Serialize_Element(nContinueWidthCount);
	//ar.Serialize_Element(nContinueSideRightCount);
	//ar.Serialize_Element(nContinueSideLeftCount);
	//ar.Serialize_Element(nContinueVHomePitchCount);

	ar.Serialize_Element(nContinueLengthCount);

	ar.Serialize_Element(dImageSaveRatioX);
	ar.Serialize_Element(dImageSaveRatioY);
}

void OperatorInfo::SetNGReasonCount(CString judge, BOOL bDimensionNG, BOOL bDefectNG, BOOL bInvalid, BOOL bBurr)
{
	mutexJD.Lock();
	
	nTotalCount++;
	if (_T("NG") == judge)
		nngCount++;
	else
		nokCount++;

	nInvalid += bInvalid;

	nBurrCount += bBurr;

	if (TRUE == bDimensionNG)
	{
		nDDTotalCount++;
		nDimensionNgCount++;
	}
	else if (TRUE == bDefectNG)
	{
		nDDTotalCount++;
		nDefectNgCount++;
	}
	mutexJD.Unlock();
}
