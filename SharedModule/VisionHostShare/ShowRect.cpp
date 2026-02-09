#include "stdafx.h"
#include "ShowRect.h"
#include "../CommonShare/ArchiveAllType.h"

#include "Types/Rect.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ShowRect::ShowRect()
{
}

ShowRect::ShowRect(const IPVM::Rect& roi)
	: ROI(IPVM::ToMFC(roi))
{
}

ShowRect::~ShowRect()
{
}

void ShowRect::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(ROI.left);
	ar.Serialize_Element(ROI.top);
	ar.Serialize_Element(ROI.right);
	ar.Serialize_Element(ROI.bottom);
}
