#include "stdafx.h"
#include "CSharpnessCalculator.h"

CSharpnessCalculator::CSharpnessCalculator()
{
}


CSharpnessCalculator::~CSharpnessCalculator()
{
}

float CSharpnessCalculator::GetSharpness128x128(const IPVM::Image_8u_C1& image, const IPVM::Rect& roi)
{
	float fFocusValue = 0.0f;

	double dMax1 = 0, dMax2 = 0, dCount = 0;

	for (int y = roi.m_top; y < roi.m_bottom; y++)
	{
		dMax1 = 0;

		for (int x = roi.m_left + 44; x < roi.m_left + 84; x++)
		{
			double dSharp = abs(image.GetValue(x, y) - image.GetValue(x + 1, y)) * 12;

			if (dMax1 < dSharp)
				dMax1 = dSharp;
		}

		dMax2 += dMax1;
		dCount++;
	}

	if (0 < dCount)
		dMax2 /= dCount;

	fFocusValue = dMax2;

	return fFocusValue;
}