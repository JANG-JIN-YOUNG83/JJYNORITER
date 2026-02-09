
#include <vector>
#include "../stdafx.h"

enum enBrightDarkType{ DARK, BRIGHT, HYBRID, ALL };

struct SBlobInfo
{
	//{{ Image Space
		long	lLabel;
		
		// 여기 있는 left, right, top, bottom은 닫힌 구간임.
		// CRect 에서는 반개 구간인 것과 차이를 두고 프로그래밍할 것.
		short	nLeft;
		short	nRight;
		short	nTop;
		short	nBottom;
		long	lArea;

		CRect	rtROI;

		short	nCenX; // Blob Center (pixel)
		short	nCenY;
		float	fCenX; // Blob Center (sub pixel)
		float	fCenY;
		float	fTipCenX; // Tip Center (sub pixel)
		float	fTipCenY;  

		std::vector<CPoint> arptEdge;	// Edge Points
		float	fKeyContrast;		// Diff. From Average
		float	fAverageContrast;
		float	fContrast;
		float	fAuxLocalContrast;
		float	fDeviation;
		float	fAuxDeviation;
		float	fLocalContrast;		
		float	fLocalContrast_ForegroundMean;
		float	fLocalContrast_BackgroundMean;
		float	fLocalContrast_ForegroundStd;
		float	fLocalContrast_BackgroundStd;
		float	fDistributionContrast;
		enBrightDarkType nBrightDarkType;	// BRIGHT, DARK, HYBRID

		float	fWidth;		// 블럽 Axis 에 대한 단축 방향 길이
		float	fLength;	// 블럽 Axis 에 대한 장축 방향 길이
		
		float	fThickness;	// 블럽의 두께...
		float	fLocus;		// 블럽의 궤적...

		float	fEdgeEnergy;
	//}}

	//{{ Real space
		float   fAveWidth; // Average Width
		float   fAveHeight; // Average Height
		
		// Line 
		//SLine   EdgeLine[4];

		// 3D Output
		// Unit : um
		float	fAvgZ;		// PMP Result
		float	fAvgZ2;		// Stereo Result

		// Calibrated Real Postion
		float   fRealTipX;
		float   fRealTipY;
		float	fRealCenX;
		float	fRealCenY;

		// 풋 앵글 계산용
		float	fA;
		float	fB;
		float	fC;
		float	fAngle;
		
		// Clear One:20080227 sjj
		float	fMeanColor_R;
		float	fMeanColor_G;
		float	fMeanColor_B;
		float	fMeanColor_RGB;
		float	fRGBRatio_RG;
		float	fRGBRatio_RB;
		float	fRGBRatio_GB;
		float	fMeanColor_C;
		float	fMeanColor_M;
		float	fMeanColor_Y;
		float	fMeanColor_K;
		
	//}}

	//For Internal use only
//	long	lDatLength;
//	unsigned int uiError;
};
