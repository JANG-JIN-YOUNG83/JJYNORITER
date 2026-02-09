#include "stdafx.h"
#include "ContinuousAlarm.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ContinuousAlarm::ContinuousAlarm()
{
	ZeroMemory(nAlarm, sizeof(int) * 8);
}

ContinuousAlarm::~ContinuousAlarm()
{
}

LPCTSTR ContinuousAlarm::GetAlarmName(long alaramIndex)
{
	switch (alaramIndex)
	{
	case 0:	return GetDefectName(DefectType::TabPitch);
	case 1:	return GetDefectName(DefectType::TopCoating);
	case 2:	return GetDefectName(DefectType::BottomCoating);
	case 3:	return GetDefectName(DefectType::TabWidth);
	case 4:	return _T("Length");
	case 5:	return GetDefectName(DefectType::SurfaceDefect);
	case 6:	return _T("Insulation Width");
	case 7:	return _T("Top Overlay Width");
	}

	return _T("NONAME");
}

void ContinuousAlarm::Serialize(CommonShare::ArchiveAllType& ar)
{
	for (int i = 0; i < 8; i++)
	{
		ar.Serialize_Element(nAlarm[i]);
	}
}