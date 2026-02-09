#include "stdafx.h"
#include "ShowList.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ShowList::ShowList()
{
}

ShowList::~ShowList()
{
}

void ShowList::AddPoint(const IPVM::Point_32f_C2& point)
{
	vecPoint.emplace_back(point);
}

void ShowList::AddRoi(const IPVM::Rect& roi)
{
	vecROI.emplace_back(roi);
}

void ShowList::Reset()
{
	vecROI.clear();
	vecPoint.clear();
}

void ShowList::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(vecROI);
	ar.Serialize_Element(vecPoint);
}

IPVM::Rect ShowList::GetTotalRoi() const
{
	CRect rtROI(0, 0, 0, 0);
	if (vecROI.size() > 0)
	{
		rtROI = vecROI[0].ROI;
		for (int i = 1; i < vecROI.size(); i++)
		{
			rtROI |= vecROI[i].ROI;
		}
	}

	return IPVM::FromMFC(rtROI);
}
