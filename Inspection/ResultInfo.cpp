#include "stdafx.h"
#include "ResultInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

RESULT_INFO::RESULT_INFO()
	: m_measureShoulderLine(Inspection::MeasureOffsetType::ShoulderLine)
	, m_measureTabHeight(Inspection::MeasureOffsetType::TabHeight)
	, m_measureTabHeight2(Inspection::MeasureOffsetType::TabHeight2)
	, m_measureTabPitch(Inspection::MeasureOffsetType::TabPitch)
	, m_measureTabSide(Inspection::MeasureOffsetType::TabSide)
	, m_measureTabWidth(Inspection::MeasureOffsetType::TabWidth)
	, m_measureCellLengthTop(Inspection::MeasureOffsetType::CellRightLength)
	, m_measureCellLengthBot(Inspection::MeasureOffsetType::CellLeftLength)//20220625 [jjh]
	, m_measureCellLength2_Top(Inspection::MeasureOffsetType::CellLength2)
	, m_measureCellLength2_Bot(Inspection::MeasureOffsetType::CellLength2)
	, m_measureCuttingHeightTop(Inspection::MeasureOffsetType::CuttingHeightTop)
	, m_measureCuttingHeightBot(Inspection::MeasureOffsetType::CuttingHeightBot)
	, m_measureMisalign(Inspection::MeasureOffsetType::Misalign)
	, m_measureVHomeTiltTop(Inspection::MeasureOffsetType::VHomeTiltTop)
	, m_measureVHomeTiltBot(Inspection::MeasureOffsetType::VHomeTiltBot)
{
	Clear();
}

RESULT_INFO::~RESULT_INFO()
{
}

void RESULT_INFO::Clear()
{
	// m_imageSavePath는 그전에 다른곳에서 설정하기 때문에 이 함수에서 Clear하지 말것

	for (long categoryIndex = 0; categoryIndex < (long)ProcItemCategory::END; categoryIndex++)
	{
		m_categoryRejectInfo[categoryIndex].Clear();
	}

	intensitySurfaceBright = 0;
	intensitySurfaceDark = 0;
	intensityTabBright = 0;
	intensityTabBright1 = 0;
	intensityTabBright2 = 0;
	intensityTabDark = 0;
	intensityTabDark1 = 0;
	intensityTabDark2 = 0;
	intensityBackgroundBrightTab = 0;
	intensityBackgroundBrightNoTab = 0;
	intensityBackgroundDarkTab = 0;
	intensityBackgroundDarkNoTab = 0;

	focusShoulder = 0;
	focusCellBoundary = 0;

	cameraGain = 0;

	resolutionY = 0;
	exposureBright = 0;
	exposureDark = 0;
	angle = 0;

	isNoTab = false;
	isMismatchReject = false;

	isPetVinyl = false;
	tabImageIndex = 0;

	bTimeOut = false;
}

REJECT_INFO& RESULT_INFO::GetSub(ProcItemCategory categoryIndex)
{
	return m_categoryRejectInfo[(long)categoryIndex];
}
