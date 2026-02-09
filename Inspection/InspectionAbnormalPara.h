#pragma once
#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaLong.h"
#include "../VisionModule/Base/ParaRect.h"
#include "Types/Rect.h"

class Database;

class CInspectionAbnormalPara
{
public:
	CInspectionAbnormalPara();
	~CInspectionAbnormalPara();

public:
	void LinkDataBase(bool bSave, bool isUpCamera, Database& db);

public:
	Base::ParaLong m_nGrayValue;
    Base::ParaLong m_nGrayValue_2;
	Base::ParaRect m_rtOffsetRoi;			
	Base::ParaRect m_rtSetInspArea;
    Base::ParaRect m_rtSetInspArea_2;
	Base::ParaBool m_bInspectionSkip;
};

