#pragma once

#include "basedef.h"

namespace Share
{
class __VISION_HOST_SHARE_CLASS__ SpecData : public CommonShare::BaseObject
{
public:
    SpecData();
    ~SpecData();

    virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
    void SetEnableInspection2String(CString str);
    CString IsEnableInspection2String() const;

    void SetEssential_Inspection_Item(CString str);
    CString IsEssential_Inspection_Item() const;

    BOOL IsOnCheckEssential_Item();

    int m_nType; // 0 : 치수 검사, 1 : 표면 검사, 2: Mismach

    DefectType m_inspType; // 검사 항목

    CString m_strCamera_Position;
    CString m_strCell_Position;

    // type이 0일때 보는 항목
    double m_dStandard_Value; // 기준값
    double m_dMinusTolerance; // 하한치
    double m_dPlusTolerance; // 상한치
    double m_dCam_Unit; // Cam unit
    double m_dOffset; // Offset 설정
    double m_dAiMinusTolerance; // 경고 하한치
    double m_dAiPlusTolerance; // 경고 상한치

    int m_nDefectSection;

    BOOL _EnableInspection; //검사 여부
    BOOL _bEssential_Inspection_Item; //필수 검사항목

    //type이 1일때 보는 항목
    double m_dDefectSizeX;
    double m_dDefectSizeY;
    long m_nCompareOperatorX; // m_dDefectSizeX에 대한 부등호(0:Not Use, 1:크, 2:작)
    long m_nCompareOperatorY; // m_dDefectSizeY에 대한 부등호(0:Not Use, 1:크, 2:작)
    long m_nOrAnd; // 0:OR, 1:AND
    long m_nLabelAttach; // 미정
    long m_nContinuous; // 미정
};
} // namespace Share
