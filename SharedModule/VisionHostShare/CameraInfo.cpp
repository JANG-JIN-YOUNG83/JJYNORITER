#include "stdafx.h"
#include "CameraInfo.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CameraInfo::CameraInfo()
{
    m_nCamera_Location = Share::enCAMERA::CAMERA_DOWN;
    m_nCamera_Number = 1;
    m_nCamera_Gain = 0;
    m_nExposure_Time = 0;
    m_fAngle = 0.0f;
    m_dResolution_X = 0.0f;
    m_dResolution_Y = 0.0f;
}

CameraInfo::~CameraInfo()
{
}

void CameraInfo::Serialize(CommonShare::ArchiveAllType& ar)
{
    ar.Serialize_Element(m_nCamera_Location);
    ar.Serialize_Element(m_nCamera_Number);
    ar.Serialize_Element(m_dResolution_X);
    ar.Serialize_Element(m_dResolution_Y);

    ar.Serialize_Element(m_nCamera_Gain);
    ar.Serialize_Element(m_nExposure_Time);
    ar.Serialize_Element(m_fAngle);

    ar.Serialize_Element(m_vecImageQuality);
}

CameraInfo& CameraInfo::operator=(const CameraInfo& src)
{
    m_nCamera_Location = src.m_nCamera_Location;
    m_nCamera_Number = src.m_nCamera_Number;
    m_dResolution_X = src.m_dResolution_X;
    m_dResolution_Y = src.m_dResolution_Y;
    m_nCamera_Gain = src.m_nCamera_Gain;
    m_nExposure_Time = src.m_nExposure_Time;
    m_fAngle = src.m_fAngle;

    m_vecImageQuality = src.m_vecImageQuality;

    return *this;
}
