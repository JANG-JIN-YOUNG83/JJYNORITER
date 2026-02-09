#include "stdafx.h"
#include "ResultIO.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
ResultIO::ResultIO()
{
}

ResultIO::~ResultIO()
{
}

void ResultIO::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element((int&)m_Area_Grabmode);
	ar.Serialize_Element((int&)m_CameraPos);
	ar.Serialize_Element(m_nIO);
}

void Share::ResultIO::SetData(int& nIO, AREA_GRABMODE& Area_Grabmode, enCAMERA& CameraPos)
{
	m_Area_Grabmode = Area_Grabmode;
	m_CameraPos = CameraPos;
	m_nIO = nIO;
}
