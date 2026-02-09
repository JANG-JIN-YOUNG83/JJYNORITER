#include "stdafx.h"
#include "Alarm.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Alarm::Alarm()
{
	use = TRUE;
	level = ALARM_LEVEL::red;
	code = 0;

	item = _T("");
	context = _T("");
	detail = _T("");
	troubleShooting = _T(""); 
	::GetLocalTime(&outbreakTime);

	nDebugImageSaveCount = 0;//HOST 내부 변수
	jsonCode = _T("3"); //20231102
}

Alarm::Alarm(int nCode, ALARM_LEVEL nLevel, CString strItem, CString strContext, CString strDetail, CString strTroubleShooting, SYSTEMTIME ToutbreakTime)
{
	code = nCode;
	level = nLevel;

	item = strItem;
	context = strContext;
	detail = strDetail;
	troubleShooting = strTroubleShooting;
	::GetLocalTime(&outbreakTime);
	
	nDebugImageSaveCount = 0; //HOST 내부 변수
	jsonCode = _T("3"); //20231102
}

Alarm::~Alarm()
{
}

void Alarm::Serialize(CommonShare::ArchiveAllType& ar)
{
	//int nLevel_dst = (int)level;
	ar.Serialize_Element(level);
	ar.Serialize_Element(code);
	ar.Serialize_Element(item);
	ar.Serialize_Element(context);
	ar.Serialize_Element(detail);
	ar.Serialize_Element(troubleShooting);
	//ar.Serialize_Element(Time);
}
