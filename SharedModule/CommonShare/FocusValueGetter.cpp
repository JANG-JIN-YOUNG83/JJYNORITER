#include "stdafx.h"
#include "FocusValueGetter.h"
#include <algorithm>

FocusValueGetter::FocusValueGetter()
{

}


FocusValueGetter::~FocusValueGetter()
{
}

void FocusValueGetter::GetSharpnessGV(IPVM::EdgeDetectionDebugInfo* pEdgeDetectionDebugInfo, int iEdgeInforNum, float& fEdgeVaule)
{
	double sp = 0;
	double cnt = 0;
	double max = -999999999999.0;
	int maxIndex = 0;
	int index = 0;

	std::vector<float> spVector;
	if (iEdgeInforNum > 0)
	{
		for (int i = 0; i < iEdgeInforNum; ++i)
		{			
			if (pEdgeDetectionDebugInfo[i].GetEdgePos() < 0)
				continue;

			max = -999999999999.0;
			maxIndex = 0;
			for (int k = 0; k < pEdgeDetectionDebugInfo[i].GetCorrelationDataCount(); ++k)
			{
				if (max < pEdgeDetectionDebugInfo[i].GetCorrelationData()[k])
				{
					max = pEdgeDetectionDebugInfo[i].GetCorrelationData()[k];
					maxIndex = k;
				}
			}
			spVector.push_back(max);			
		}

		if (spVector.size() != 0)
		{
			std::sort(spVector.begin(), spVector.end());
			
			sp = spVector[spVector.size()*0.5];
			//sp = 0;
			//for (int k = 0; k < spVector.size(); ++k)
			//{
			//	sp += spVector[k];
			//}
		}
		//sp /= (float)spVector.size();
	}

	fEdgeVaule = sp;
}