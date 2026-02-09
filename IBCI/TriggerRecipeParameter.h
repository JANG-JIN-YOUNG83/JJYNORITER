#pragma once

#include "TriggerBaseParameter.h"
#include "basedef.h"
#include "TriggerBoardDefine.h"
//#include "Utility.h"
#include"..\Module\Utility\IniPackage.h"
#include "pch.h"

struct stMessageMap
{
    CCriticalSection cs;
    std::deque<BYTE*> data;

    stMessageMap()
    {
    }

    int GetSize()
    {
        int nSize = 0;

        cs.Lock();
        nSize = data.size();
        cs.Unlock();

        return nSize;
    }

    void Clear()
    {
        while (1)
        {
            BYTE by[8];
            if (FALSE == pop(*by))
            {
                break;
            }
        }
    }

    void push(BYTE* byMSG)
    {
        BYTE* pbyte = new BYTE[8];
        memcpy(pbyte, byMSG, sizeof(BYTE) * 8);

        cs.Lock();
        data.push_back(pbyte);
        cs.Unlock();
    }

    BOOL pop(BYTE& byMSG)
    {
        if (0 < data.size())
        {
            cs.Lock();
            memcpy(&byMSG, data[0], sizeof(BYTE) * 8);
            delete data[0];
            data.pop_front();
            cs.Unlock();
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
};

class TriggerRecipeParameter
{
public:
    TriggerRecipeParameter();
    ~TriggerRecipeParameter();

    bool Load(LPCTSTR filePath);
    bool Save(LPCTSTR filePath);
    void Link(bool save, Utility::IniPackage& iniPackage, LPCTSTR filePath = _T("")); //20221206 [jjh]

    int m_tabLength;
    int m_cellPitch;
    int m_sensorMinOffset;
    int m_noTabTRGOffset;
    int m_visionOffset;
    int m_topMarkingPos;
    int m_bottomMarkingPos;
    int m_nTapeDetectionUse; //20220913 [jjh]
    int m_nInkMarkingUseTimeMethod; //20221207 [jjh]
    int m_nTabDetectionCounterSettingUse; //20230602 [jjh] start
    int m_nTabDetectionCounterSettingNormalCellRange;
    int m_nTabDetectionCounterSettingNormalCellCount; //20230602 [jjh] end
    unsigned int m_nArrData[_TB_DataEnd];
    //unsigned int m_nArrData[10];

    void UpdateCalculationParameter(const TriggerBaseParameter& basePara);
};