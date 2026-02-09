#pragma once

#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaByte.h"
#include "../VisionModule/Base/ParaLong.h"
#include "../VisionModule/Base/ParaDouble.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaRect.h"
#include "Types/Rect.h"

#include <vector>
using namespace std;

#define MAX_CAMERA	2

typedef enum class _enumBlobSrcImageType
{
	Brightness = 0,
	Darkness = 1,
    MAX = 2
} ENUM_BLOB_TYPE_SRCIMAGE;

typedef enum class _enumBlobThresholdBright
{
	White = 0,
	Black = 1,
	Mix =2
} ENUM_BLOB_TYPE_THBRIGHT;

typedef enum class _enumBlobInspectionType
{
	Defect1 = 0,
	Defect2,
	Defect3,
	Defect4
} ENUM_BLOB_TYPE_INSPTYPE;

typedef enum class _enumSetTypeInspArea
{
	OffsetRoi,
	From_Left,
	From_Right
} ENUM_BLOB_TYPE_SET_INSPAREA;

class Database;

class CBlobParaRoi
{
public:
	CBlobParaRoi(CString name);

public:
	CString m_name;

	Base::ParaBool m_bUseInspection;
	Base::ParaLong m_nThreshold;
	Base::ParaLong m_nThreshold2;
    Base::ParaBool m_bUseMeanRangeTheshold;
	Base::ParaFloat m_fBlobMinSize_mm;
	Base::ParaLong m_nMassCenterGVsize;
	Base::ParaLong m_nThresholdHorDiv;
	Base::ParaLong m_nThresholdVerDiv;
	Base::ParaLong m_nBlobMergeDistance;
    Base::ParaBool m_bUseIntersectMerge;
	Base::ParaLong m_nBlobMaxCount;
	Base::ParaLong m_nTimeOut_ms;
	Base::ParaFloat m_fBlobMinLength_mm;
	Base::ParaLong m_nImageType;
	Base::ParaLong m_nBlobBrightType;
	Base::ParaLong m_nInspectionType;

	Base::ParaLong m_nInspectionArea;		// Align에서 계산된 Inspection Area Index
	Base::ParaLong m_nSetTypeInspArea;		// Roi Adjust 방법 선택 (offset roi/ from left/ from right)
	Base::ParaRect m_rtOffsetRoi;			//   -. Roi Adjust 방법
	Base::ParaRect m_rtSetInspArea;			// 일단은 여기에 두지만, BlobParaCondition 개수만큼 unionResult에서 관리되는게 좋을듯
	
	Base::ParaBool m_bUseEqual;
	Base::ParaBool m_bUseEqualThreshold;
	Base::ParaLong m_nEqualThresholdMax;
	Base::ParaLong m_nEqualThresholdMin;
	Base::ParaLong m_nEqualThreadCount;
};

class CInspectionBlobPara
{
public:
	CInspectionBlobPara();
	~CInspectionBlobPara();

public:
	void LinkDataBase(bool bSave, Database& db);

public:
	long m_nTotalInspCount;
	vector<Base::ParaLong> m_vecnInspROICount;
	vector<CBlobParaRoi> m_vecBlobParaCondition;
    Base::ParaBool m_bUseSwapXY;
    Base::ParaBool m_bUseDeleteIntersectBlob;
};
