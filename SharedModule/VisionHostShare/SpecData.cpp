#include "stdafx.h"
#include "SpecData.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SpecData::SpecData()
    : m_inspType(DefectType::ISI_END)
    , m_nCompareOperatorX(0)
    , m_nCompareOperatorY(0)
    , m_nOrAnd(0)
    , m_nLabelAttach(0)
    , m_nContinuous(0)
{
    m_strCamera_Position = _T("NA");
    m_strCell_Position = _T("");

    m_dStandard_Value = 0;
    m_dMinusTolerance = 0.0;
    m_dPlusTolerance = 0.0;
    m_dCam_Unit = 0.0;
    m_dOffset = 0.0;

    m_dAiMinusTolerance = -9999;
    m_dAiPlusTolerance = -9999;

    m_dDefectSizeX = 0.0;
    m_dDefectSizeY = 0.0;

    m_nDefectSection = ensection::NOTCHING;

    _EnableInspection = TRUE;
    _bEssential_Inspection_Item = TRUE;
}

SpecData::~SpecData()
{
}

void SpecData::Serialize(CommonShare::ArchiveAllType& ar)
{
    CString stringDummy; // 없어진 데이터 (호환성을 위한 것 / 추후 지울것)
    CString stringInspName; // 호환성을 위해 String으로 남김

    if (ar.IsStoring())
    {
        if (m_inspType != DefectType::ISI_END)
        {
            stringInspName = GetDefectName(m_inspType);
        }
    }

    ar.Serialize_Element(m_nType);

    ar.Serialize_Element(m_strCamera_Position);
    ar.Serialize_Element(m_strCell_Position);

    ar.Serialize_Element(stringInspName);
    ar.Serialize_Element(stringDummy);
    ar.Serialize_Element(m_dStandard_Value);
    ar.Serialize_Element(m_dMinusTolerance);
    ar.Serialize_Element(m_dPlusTolerance);
    ar.Serialize_Element(m_dCam_Unit);
    ar.Serialize_Element(m_dOffset);
    ar.Serialize_Element(m_dAiMinusTolerance);
    ar.Serialize_Element(m_dAiPlusTolerance);
    ar.Serialize_Element(_EnableInspection);
    ar.Serialize_Element(_bEssential_Inspection_Item);
    ar.Serialize_Element(m_nDefectSection);

    ar.Serialize_Element(m_dDefectSizeX);
    ar.Serialize_Element(m_dDefectSizeY);

    ar.Serialize_Element(m_nCompareOperatorX);
    ar.Serialize_Element(m_nCompareOperatorY);
    ar.Serialize_Element(m_nOrAnd);
    ar.Serialize_Element(m_nLabelAttach);
    ar.Serialize_Element(m_nContinuous);

    if (ar.IsLoading())
    {
        m_inspType = GetDefectType(stringInspName);
    }
}

void SpecData::SetEnableInspection2String(CString str)
{
    if (_T("O") == str)
    {
        _EnableInspection = TRUE;
    }
    else if (_T("X") == str)
    {
        _EnableInspection = FALSE;
    }
    else
    {
        AfxMessageBox(_T("There is a problem with the code. 678678678"));
    }
}

CString SpecData::IsEnableInspection2String() const
{
    if (TRUE == _EnableInspection)
    {
        return _T("O");
    }
    else
    {
        return _T("X");
    }
}

void SpecData::SetEssential_Inspection_Item(CString str)
{
    if (_T("O") == str)
    {
        _bEssential_Inspection_Item = TRUE;
    }
    else if (_T("X") == str)
    {
        _bEssential_Inspection_Item = FALSE;
    }
    else
    {
        AfxMessageBox(_T("There is a problem with the code. 678678545"));
    }
}

CString SpecData::IsEssential_Inspection_Item() const
{
    if (TRUE == _bEssential_Inspection_Item)
    {
        return _T("O");
    }
    else
    {
        return _T("X");
    }
}

//return : FALSE의미는.. 반드시 켜야하는 항목이 꺼져있다는 의미이다. 에러임
BOOL SpecData::IsOnCheckEssential_Item()
{
    if (TRUE == _bEssential_Inspection_Item) //반드시 켜야 하는 항목인가?
    {
        if (FALSE == _EnableInspection) //꺼져있다면
        {
            return FALSE;
        }
    }

    return TRUE;
}