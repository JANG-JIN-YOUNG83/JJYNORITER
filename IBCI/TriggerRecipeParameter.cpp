#include "pch.h"
#include "TriggerRecipeParameter.h"
#include "..\CompanySharedLibrary\Persistence\IniHelper.h"
//#include "Utility.h"
TriggerRecipeParameter::TriggerRecipeParameter()
    : m_tabLength(0)
    , m_cellPitch(0)
    , m_sensorMinOffset(0)
    , m_noTabTRGOffset(0)
    , m_visionOffset(0)
    , m_topMarkingPos(0)
    , m_bottomMarkingPos(0)
    , m_nTapeDetectionUse(0) //20220913 [jjh]
    , m_nInkMarkingUseTimeMethod(0) //20221207 [jjh]
    , m_nTabDetectionCounterSettingUse(0) //20230602 [jjh] start
    , m_nTabDetectionCounterSettingNormalCellRange(5)
    , m_nTabDetectionCounterSettingNormalCellCount(1) //20230602 [jjh] end
{
    for (int i = 0; i < _TB_DataEnd; i++)
    {
        m_nArrData[i] = 0;
    }

}

TriggerRecipeParameter::~TriggerRecipeParameter()
{
}

bool TriggerRecipeParameter::Load(LPCTSTR filePath)
{
    Utility::IniPackage iniPackage;
    if (!iniPackage.Load(filePath))
        return false;

    Link(false, iniPackage, filePath); //20221206 [jjh]

    m_nArrData[_TB19_Line_TriggerReset] = 0;
    m_nArrData[_TB18_Line_TriggerEnable] = 1;
    m_nArrData[_TB47_Dross_Line_TriggerEnable] = 1;

    return true;
}

bool TriggerRecipeParameter::Save(LPCTSTR filePath)
{
    Utility::IniPackage iniPackage;
    Link(true, iniPackage, filePath); //20221206 [jjh]

    return iniPackage.Save(filePath);
}

void TriggerRecipeParameter::Link(bool save, Utility::IniPackage& iniPackage, LPCTSTR filePath) //20221206 [jjh]
{
    if (save)
    {
        if (auto category = iniPackage.GetCategory(_T("RS232Data")))
        {
            for (int i = 0; i < 10; i++)
            {
                CString str;
                str.Format(_T("%d"), m_nArrData[i]);

                //category->SetKeyValue(enum_ParaDesc[i], m_nArrData[i]);
            }
        }

        if (auto category = iniPackage.GetCategory(_T("Basic_Setting")))
        {
            category->SetKeyValue(_T("TabLength"), m_tabLength);
            category->SetKeyValue(_T("CellPitch"), m_cellPitch);
            category->SetKeyValue(_T("SensorMinOffset"), m_sensorMinOffset);
            category->SetKeyValue(_T("NoTabTRGOffset"), m_noTabTRGOffset);
            category->SetKeyValue(_T("VisionOffset"), m_visionOffset);
            category->SetKeyValue(_T("TapeDetectionUse"), m_nTapeDetectionUse); //20220913 [jjh]
            category->SetKeyValue(_T("InkMarkingUseTimeMethod"), m_nInkMarkingUseTimeMethod); //20221207 [jjh]
            category->SetKeyValue(_T("TabDetectionCounterSettingUse"), m_nTabDetectionCounterSettingUse); //20230602 [jjh] start
            category->SetKeyValue(_T("TabDetectionCounterSettingNormalCellRange"), m_nTabDetectionCounterSettingNormalCellRange);
            category->SetKeyValue(_T("TabDetectionCounterSettingNormalCellCount"), m_nTabDetectionCounterSettingNormalCellCount); //20230602 [jjh] end
        }
    }
    else
    {
        // 해당 부분 필요 없을 것 같음
    //    int cellpitch = 0; //20221207 [jjh]
    //    if (auto category = iniPackage.GetCategory(_T("RS232Data")))
    //    {
    //        for (int i = 0; i < 10; i++)
    //        {
    //            if ((i >= _TB31_TapeDetection_TabPitch && i <= _TB36_TapeDetection_InkMarkingCount)
    //                && (HostBase::enumSite::LGESOC == HostBase::Site::GetSiteType(g_systemParameter.machineinfo.SITE))) //20221206 [jjh] start
    //            {
    //                if (i == _TB31_TapeDetection_TabPitch)
    //                {
    //                    cellpitch = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("CellPitch"), 0);
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], cellpitch);
    //                }
    //                else if (i == _TB32_TapeDetection_TopInkMarkingPos)
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], m_nArrData[_TB20_Marking_TopTrigger]);
    //                else if (i == _TB33_TapeDetection_BottomInkMarkingPos)
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], m_nArrData[_TB21_Marking_BottomTrigger]);
    //                else if (i == _TB34_TapeDetection_TopSmartVisionPos)
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], m_nArrData[_TB23_Marking_TopSmartVisionTrigger]);
    //                else if (i == _TB35_TapeDetection_BottomSmartVisionPos)
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], m_nArrData[_TB24_Marking_BottomSmartVisionTrigger]);
    //                else if (i == _TB36_TapeDetection_InkMarkingCount)
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], 3);
    //            }
    //            else if (i == _TB37_Marking_Length_Time)
    //            {
    //                if (HostBase::enumSite::LGESMI == HostBase::Site::GetSiteType(g_systemParameter.machineinfo.SITE))
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], 50000);
    //                else
    //                    m_nArrData[i] = IniHelper::LoadINT(filePath, _T("RS232Data"), enum_ParaDesc[i], 0);
    //            }
    //            else //20221206 [jjh] end
    //                m_nArrData[i] = _ttoi(category->GetKeyValue(enum_ParaDesc[i]));
    //        }
    //    }

    //    if (auto category = iniPackage.GetCategory(_T("Basic_Setting")))
    //    {
    //        m_tabLength = _ttoi(category->GetKeyValue(_T("TabLength")));
    //        m_cellPitch = _ttoi(category->GetKeyValue(_T("CellPitch")));
    //        m_sensorMinOffset = _ttoi(category->GetKeyValue(_T("SensorMinOffset")));
    //        m_noTabTRGOffset = _ttoi(category->GetKeyValue(_T("NoTabTRGOffset")));
    //        m_visionOffset = _ttoi(category->GetKeyValue(_T("VisionOffset")));
    //        if (HostBase::enumSite::LGESOC == HostBase::Site::GetSiteType(g_systemParameter.machineinfo.SITE)) //20221206 [jjh] start
    //            m_nTapeDetectionUse = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("TapeDetectionUse"), 1);
    //        else
    //            m_nTapeDetectionUse = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("TapeDetectionUse"), 0); //20221206 [jjh] end

    //        if (HostBase::enumSite::LGESMI == HostBase::Site::GetSiteType(g_systemParameter.machineinfo.SITE)) //20221207 [jjh] start
    //            m_nInkMarkingUseTimeMethod = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("InkMarkingUseTimeMethod"), 0 /*임시1*/);
    //        else
    //            m_nInkMarkingUseTimeMethod = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("InkMarkingUseTimeMethod"), 0); //20221207 [jjh] end

    //        m_nTabDetectionCounterSettingUse = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("TabDetectionCounterSettingUse"), 0); //20230602 [jjh] start
    //        m_nTabDetectionCounterSettingNormalCellRange = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("TabDetectionCounterSettingNormalCellRange"), 5);
    //        m_nTabDetectionCounterSettingNormalCellCount
    //            = IniHelper::LoadINT(filePath, _T("Basic_Setting"), _T("TabDetectionCounterSettingNormalCellCount"), 1); //20230602 [jjh] end
    //    }
    }
}

void TriggerRecipeParameter::UpdateCalculationParameter(const TriggerBaseParameter& basePara)
{
    m_nArrData[_TB01_EncoderPulse] = 0;
    m_nArrData[_TB02_SensorMinDistance] = m_tabLength - m_sensorMinOffset;
    m_nArrData[_TB03_VirtualTrigger] = m_cellPitch + m_noTabTRGOffset;
    m_nArrData[_TB05_V1_CellID] = m_nArrData[_TB04_V1_Trigger] - m_visionOffset;
    m_nArrData[_TB07_V1_AlarmPosition] = min(m_topMarkingPos, m_bottomMarkingPos) - m_cellPitch;
    m_nArrData[_TB09_V2_CellID] = m_nArrData[_TB08_V2_Trigger] - m_visionOffset;
    m_nArrData[_TB11_V2_AlarmPosition] = min(m_topMarkingPos, m_bottomMarkingPos) - m_cellPitch;
    m_nArrData[_TB40_Dross_Vision_CellID] = m_nArrData[_TB39_Dross_Vision_Trigger] - m_visionOffset;
    m_nArrData[_TB42_Dross_Vision_AlarmPosition] = min(m_topMarkingPos, m_bottomMarkingPos) - m_cellPitch;

    if (basePara.m_idleRollerDiameter != 0 && basePara.m_rotaryEncoderPulse != 0)
    {
        m_nArrData[_TB01_EncoderPulse] = (basePara.m_rotaryEncoderPulse * 4) / (basePara.m_idleRollerDiameter * 3.141592);
    }
}
