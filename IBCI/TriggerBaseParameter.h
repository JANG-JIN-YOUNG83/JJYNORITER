#pragma once

#include "basedef.h"
#include "TriggerBoardDefine.h"
#include "pch.h"
class  TriggerBaseParameter
{
public:
    TriggerBaseParameter();
    ~TriggerBaseParameter();

    void Load(LPCTSTR filePath);
    void Save(LPCTSTR filePath);

    int m_nPortNo;
    int m_nBaundRate;
    int m_idleRollerDiameter;
    int m_rotaryEncoderPulse;
};
