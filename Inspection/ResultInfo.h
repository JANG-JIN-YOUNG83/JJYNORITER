#pragma once

#include "basedef.h"
#include "MeasureValue.h"
#include "ProcessingItemDef.h"
#include "RejectInfo.h"
#include "Types/LineSeg_32f.h"
#include "Types/Point_32f_C2.h"
#include "Types/Rect_32f.h"

class __INSPECTOIN_CLASS__ RESULT_INFO
{
public:
	RESULT_INFO();
	~RESULT_INFO();

	void Clear();
	REJECT_INFO& GetSub(ProcItemCategory categoryIndex);

public:
	CString	m_imageSavePath;	// 이미지를 획득시점에 저장했다면 이미지의 경로

	IPVM::Rect_32f tabRect;
	IPVM::Rect_32f tabRectForHeight;

	REJECT_INFO m_totalRejectInfo;

	IPVM::LineSeg_32f shoulderLine;
	IPVM::LineSeg_32f cellBoundaryLine;
	int nImageWidth;
	int nImageHeight;

	IPVM::Point_32f_C2 ptOrigin;

	bool isUp;
	BOOL bInvalid;

	long imagePosition;
	int cellID;
	int nIndexID;

	// 20200605
	float			m_grabTime_ms;
	long			cellOffset;
	int				cellSizePixel;
	bool			cellSizeReject;

	float			m_inspectionStartTime_ms;

	SYSTEMTIME		m_inputTime;

	IPVM::Image_8u_C1 imageFull, imageBright, imageDark;


	IPVM::Rect		shoulderLineRect;

	IPVM::Point_32f_C2	shoulderPoint1;
	IPVM::Point_32f_C2	shoulderPoint2;
	IPVM::Rect_32f	shoulderRect1;
	IPVM::Rect_32f	shoulderRect2;

	IPVM::Rect_32f		vhomeHeightLeftRect;
	IPVM::Rect_32f		vhomeHeightRightRect;

	IPVM::Rect_32f		vhomeTiltTopRightRect;
	IPVM::Rect_32f		vhomeTiltTopLeftRect;
	IPVM::Point_32f_C2	vhomeTiltTopRightInner;
	IPVM::Point_32f_C2	vhomeTiltTopRightOuter;
	IPVM::Point_32f_C2	vhomeTiltTopLeftInner;
	IPVM::Point_32f_C2	vhomeTiltTopLeftOuter;
	IPVM::Rect_32f		vhomeTiltBotRightRect;
	IPVM::Rect_32f		vhomeTiltBotLeftRect;
	IPVM::Point_32f_C2	vhomeTiltBotRightInner;
	IPVM::Point_32f_C2	vhomeTiltBotRightOuter;
	IPVM::Point_32f_C2	vhomeTiltBotLeftInner;
	IPVM::Point_32f_C2	vhomeTiltBotLeftOuter;

	IPVM::Point_32f_C2	vhomeTiltTopLeftInnerFloat;
	IPVM::Point_32f_C2	vhomeTiltTopLeftOuterFloat;
	IPVM::Point_32f_C2	vhomeTiltBotLeftInnerFloat;
	IPVM::Point_32f_C2	vhomeTiltBotLeftOuterFloat;


	//{{ 수치 결과
	Inspection::MeasureValue m_measureShoulderLine;
	Inspection::MeasureValue m_measureTabHeight;
	Inspection::MeasureValue m_measureTabHeight2;
	Inspection::MeasureValue m_measureTabPitch;
	Inspection::MeasureValue m_measureTabSide;
	Inspection::MeasureValue m_measureTabWidth;
	Inspection::MeasureValue m_measureCellLengthTop;
	Inspection::MeasureValue m_measureCellLengthBot;
	Inspection::MeasureValue m_measureCellLength2_Top;
	Inspection::MeasureValue m_measureCellLength2_Bot;
	Inspection::MeasureValue m_measureCuttingHeightTop;
	Inspection::MeasureValue m_measureCuttingHeightBot;
	Inspection::MeasureValue m_measureMisalign;
	Inspection::MeasureValue m_measureVHomeTiltTop;
	Inspection::MeasureValue m_measureVHomeTiltBot;

	float mismatchUm;		// um
	float cellLengthTopUmOffset; // um
	float cellLengthBotUmOffset; // um
	float vhomeHeightLeftUm;
	float vhomeHeightRightUm;

	float shoulderLineCoatingUm;	// 

	float cellPitchErrorUm;	// um
	float cellLengthTopErrorUm;	// um
	float cellLengthBotErrorUm;	// um
	float tabWidthErrorUm;		// um
	float tabHeightErrorUm;	// um
	float misalignErrorUm;		// um
	float vhomeTiltBotErrorUm;	// um
	float vhomeTiltTopErrorUm;	// um
	float tabSideErrorUm;		// um


	//	float cellPitchPixel;		// px
	//	float cellHeightPixel;		// px
	//	float tabWidthPixel;		// px
	//	float tabHeightPixel;		// px
	//	float misalignPixel;		// px
	////	float vhomeTiltTopPixel;	// px
	////	float vhomeTiltBotPixel;	// px
	//	float tabSidePixel;
	//}} 수치 결과

	//{{ 수치 중간 결과
	IPVM::Point_32f_C2 vhomeLeftStartPixel;	// px
	IPVM::Point_32f_C2 vhomeLeftEndPixel;	// px
	IPVM::Point_32f_C2 vhomeRightStartPixel;	// px
	IPVM::Point_32f_C2 vhomeRightEndPixel;	// px

	IPVM::Rect_32f	cuttingHeightTopRectPixel;
	IPVM::Rect_32f	cuttingHeightBotRectPixel;
	//}} 수치 중간 결과


	bool isNoTab;
	bool isMismatchReject;


	//{{ ESWA option
	float intensitySurfaceBright;
	float intensitySurfaceDark;
	float intensityTabBright;
	float intensityTabBright1;
	float intensityTabBright2;
	float intensityTabDark;
	float intensityTabDark1;
	float intensityTabDark2;
	float intensityBackgroundBrightTab;
	float intensityBackgroundDarkTab;
	float intensityBackgroundBrightNoTab;
	float intensityBackgroundDarkNoTab;

	float stdSurfaceBright;
	float stdSurfaceDark;
	float stdTabBright;
	float stdTabBright1;
	float stdTabBright2;
	float stdTabDark;
	float stdTabDark1;
	float stdTabDark2;
	float stdBackgroundBrightTab;
	float stdBackgroundDarkTab;
	float stdBackgroundBrightNoTab;
	float stdBackgroundDarkNoTab;

	float focusShoulder;
	float focusCellBoundary;

	float cameraGain;

	float resolutionY;
	float exposureBright;
	float exposureDark;
	float angle;
	//}} ESWA option

	bool isPetVinyl;
	int	tabImageIndex;


	int inkAutoCalibError;

	//time out 연속 카운트를 봐야해서 time out 인지 main 이 알아야함.
	bool bTimeOut;

	// test
	BOOL m_isRejectTape;
	BOOL m_isRejectUncoatingBoundary;
	long m_DetialResult;

private:
	REJECT_INFO m_categoryRejectInfo[(long)ProcItemCategory::END];
};
