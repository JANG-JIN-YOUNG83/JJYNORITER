#include "stdafx.h"
#include "I2V_SimulationInspect.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace TCPMSG::I2V;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SimulationInspect::SimulationInspect()
	: m_cellID(-1)
{
}

SimulationInspect::~SimulationInspect()
{
}

void SimulationInspect::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_cellID);
	ar.Serialize_Element(m_cisUpImagePath);
	ar.Serialize_Element(m_cisDownImagePath);
	ar.Serialize_Element(m_areaImagePath);
}
