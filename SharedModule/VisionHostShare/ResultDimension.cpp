#include "stdafx.h"
#include "ResultDimension.h"
#include "SpecData.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ResultDimension::ResultDimension()
    : m_inspType(DefectType::ISI_END)
{
    m_nCamera_Location = enCAMERA::CAMERA_UP;
    m_nCamera_Number = 1;
    m_nScreen_Number = enImgSec::ImgSec_BRIGHT;
    m_strPosition_InspItem = _T("");
    m_strPosition_Sub = _T("");
    m_dValue = 0.0;
    m_dLSL = 0.0;
    m_dUSL = 0.0;
    m_strJudge = _T("");
    m_dGL = 0.0;

    m_dStart_Point_X = 0.0;
    m_dEnd_Point_X = 0.0;
    m_dStart_Point_Y = 0.0;
    m_dEnd_Point_Y = 0.0;
}

ResultDimension::~ResultDimension()
{
}

void ResultDimension::Serialize(CommonShare::ArchiveAllType& ar)
{
    CString inspName;

    if (ar.IsStoring())
    {
        if (m_inspType != DefectType::ISI_END)
        {
            inspName = GetDefectName(m_inspType);
        }
    }

    ar.Serialize_Element(m_nCamera_Location);
    ar.Serialize_Element(m_nCamera_Number);
    ar.Serialize_Element(m_nScreen_Number);
    ar.Serialize_Element(m_strPosition_InspItem);
    ar.Serialize_Element(m_strPosition_Sub);
    ar.Serialize_Element(inspName);
    ar.Serialize_Element(m_dValue);
    ar.Serialize_Element(m_dLSL);
    ar.Serialize_Element(m_dUSL);
    ar.Serialize_Element(m_strJudge);
    ar.Serialize_Element(m_dGL);

    ar.Serialize_Element(m_dStart_Point_X);
    ar.Serialize_Element(m_dEnd_Point_X);
    ar.Serialize_Element(m_dStart_Point_Y);
    ar.Serialize_Element(m_dEnd_Point_Y);

    if (ar.IsLoading())
    {
        m_inspType = GetDefectType(inspName);
    }

    m_stShowList.Serialize(ar);
}

ResultDimension& ResultDimension::operator=(const ResultDimension& src)
{
    m_nCamera_Location = src.m_nCamera_Location;
    m_nCamera_Number = src.m_nCamera_Number;
    m_nScreen_Number = src.m_nScreen_Number;
    m_strPosition_InspItem = src.m_strPosition_InspItem;
    m_strPosition_Sub = src.m_strPosition_Sub;
    m_inspType = src.m_inspType;
    m_dValue = src.m_dValue;
    m_dLSL = src.m_dLSL;
    m_dUSL = src.m_dUSL;
    m_strJudge = src.m_strJudge;
    m_dGL = src.m_dGL;

    m_dStart_Point_X = src.m_dStart_Point_X;
    m_dEnd_Point_X = src.m_dEnd_Point_X;
    m_dStart_Point_Y = src.m_dStart_Point_Y;
    m_dEnd_Point_Y = src.m_dEnd_Point_Y;

    m_stShowList = src.m_stShowList;

    return *this;
}

void ResultDimension::ResetValues()
{
    m_nCamera_Number = 1;
    m_strPosition_InspItem = _T("");
    m_strPosition_Sub = _T("");
    m_dValue = 0.0;
    m_dLSL = 0.0;
    m_dUSL = 0.0;
    m_strJudge = _T("");
    m_dGL = 0.0;

    m_dStart_Point_X = 0.0;
    m_dEnd_Point_X = 0.0;
    m_dStart_Point_Y = 0.0;
    m_dEnd_Point_Y = 0.0;
}