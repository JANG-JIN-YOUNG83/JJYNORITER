#pragma once
#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaLong.h"
#include "../VisionModule/Base/ParaRect.h"
#include "Types/Rect.h"

class Database;

class CInspectionChatteringPara
{
public:
    CInspectionChatteringPara();
    ~CInspectionChatteringPara();

public:
    void LinkDataBase(bool bSave, bool isUpCamera, Database& db);
    Base::ParaBool m_bUseInspection;
    Base::ParaLong m_nSelectDarkImage;
    Base::ParaLong m_nDivCount;
    Base::ParaLong m_nStandard;
    Base::ParaLong m_nPeriodMax;
    Base::ParaLong m_nPeriodMin;
    Base::ParaLong m_nAmpMax;
    Base::ParaLong m_nAmpMin;
    Base::ParaRect m_rtSetInspArea;
    Base::ParaRect m_rtOffsetRoi;
    Base::ParaRect m_rtSetAlignArea;
};
