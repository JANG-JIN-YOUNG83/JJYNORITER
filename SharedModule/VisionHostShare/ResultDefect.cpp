#include "stdafx.h"
#include "ResultDefect.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ResultDefect::ResultDefect()
    : m_inspType(DefectType::ISI_END)
{
    m_nCamera_Position = enCAMERA::CAMERA_UP;
    m_strCamera_Type = _T("LINE");
    m_nCamera_Number = 1;
    m_nScreen_Number = enImgSec::ImgSec_BRIGHT;
    m_dCamera_Gain = 0.0f;
}

ResultDefect::~ResultDefect()
{
}

void ResultDefect::Serialize(CommonShare::ArchiveAllType& ar)
{
    CString inspName;

    if (ar.IsStoring())
    {
        if (m_inspType != DefectType::ISI_END)
        {
            inspName = GetDefectName(m_inspType);
        }
    }

    ar.Serialize_Element(m_nCamera_Position);
    ar.Serialize_Element(m_strCamera_Type);
    ar.Serialize_Element(m_nCamera_Number);
    ar.Serialize_Element(m_nScreen_Number);
    ar.Serialize_Element(m_dCamera_Gain);
    ar.Serialize_Element(inspName);

    ar.Serialize_Element(m_vecstDefectROI);

    if (ar.IsLoading())
    {
        m_inspType = GetDefectType(inspName);
    }
}

ResultDefect& ResultDefect::operator=(const ResultDefect& src)
{
    m_nCamera_Position = src.m_nCamera_Position;
    m_strCamera_Type = src.m_strCamera_Type;
    m_nCamera_Number = src.m_nCamera_Number;
    m_nScreen_Number = src.m_nScreen_Number;
    m_dCamera_Gain = src.m_dCamera_Gain;
    m_inspType = src.m_inspType;

    m_vecstDefectROI = src.m_vecstDefectROI;

    return *this;
}

const ResultDefectROI* ResultDefect::GetBestROI() const
{
    int nMaxArea = 0;
    int nIndex = -1;
    for (int i = 0; i < m_vecstDefectROI.size(); i++)
    {
        if (nMaxArea <= (int)m_vecstDefectROI[i].m_dArea_Pixels)
        {
            nMaxArea = (int)m_vecstDefectROI[i].m_dArea_Pixels;
            nIndex = i;
        }
    }

    if (0 > nIndex)
    {
        return NULL;
    }

    return &m_vecstDefectROI[nIndex];
}
