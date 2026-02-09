#include "stdafx.h"
#include "ShowPoint.h"
#include "../CommonShare/ArchiveAllType.h"

#include "Types/Point_32f_C2.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ShowPoint::ShowPoint()
{
}

ShowPoint::ShowPoint(const IPVM::Point_32f_C2& point)
	: Point(IPVM::ToMFC(point))
{
}

ShowPoint::~ShowPoint()
{
}

void ShowPoint::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(Point.x);
	ar.Serialize_Element(Point.y);
}
