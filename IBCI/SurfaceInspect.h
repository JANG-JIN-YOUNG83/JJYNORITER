#pragma once
#include "./Surface/Inspect.h"
#include "CISCommSocket.h"
class InspectionInputData;

struct SURFACE_RESULT_INFO
{
	CString m_imageSavePath;	// 이미지를 획득시점에 저장했다면 이미지의 경로

	bool isUp;
	int cellID;
	IPVM::Image_8u_C1 fullImage; //White, Black 분리가 완료된 이미지
	IPVM::Image_8u_C1 brightImage;
	IPVM::Image_8u_C1 darkImage;
	float m_grabTime_ms;
	float m_inspectionStartTime_ms;
	SYSTEMTIME m_inputTime;
	int	rejectCount;

	double	m_dInsTime[10];
	double  m_dBottomCoat[2];//0mm 1pixel	//0801
	double  m_dBottomOverlay[2];//0mm 1pixel//0801
	double	m_dInsulWidth;
	double	m_dOverlayWidth;//0604 //오버레이폭
	int		m_nScrabRightBoundary;
	CRect	m_rtInsulate;
	CRect	m_rtInsulationInspArea;
	CRect	m_rtOverlay;

public:
	////////////////////////////검사 관련
	int		m_nFullDefectCount;
	CRect	m_rtFullDefect[MAX_FULL_DEFECT_COUNT];//불량 영역


	CRect   m_rtRawDefect[MAX_FULL_DEFECT_COUNT];
	double  m_dFullArea[MAX_FULL_DEFECT_COUNT];
	double  m_dFullWidth[MAX_FULL_DEFECT_COUNT];
	double  m_dFullLength[MAX_FULL_DEFECT_COUNT];


	BYTE*	m_fmDefect[MAX_FULL_DEFECT_COUNT];//불량 이미지
	int		m_nFullDefectType[MAX_FULL_DEFECT_COUNT];//defect Code
	CString	m_strFullDefectName[MAX_FULL_DEFECT_COUNT];
	double	m_dFullStdValue[MAX_FULL_DEFECT_COUNT][2];
	double	m_dFullCompactness[MAX_FULL_DEFECT_COUNT][2];		//0white 1black
	double	m_dFullAngle[MAX_FULL_DEFECT_COUNT];
	double	m_dFullDistance[MAX_FULL_DEFECT_COUNT][2];
	double	m_dFullPercent[MAX_FULL_DEFECT_COUNT][2];
	double	m_dFullSizeX[MAX_FULL_DEFECT_COUNT][3];		//0white 1black
	double	m_dFullSizeY[MAX_FULL_DEFECT_COUNT][3];		//0white 1black
	double	m_dFullPeakDataW[MAX_FULL_DEFECT_COUNT][2];//0-Origin 1Preprocess
	double	m_dFullPeakDataB[MAX_FULL_DEFECT_COUNT][2];//0-Origin 1Preprocess
	CPoint  m_cpDefectCenter[MAX_FULL_DEFECT_COUNT];

	IPVM::Rect m_rectCellArea;
	IPVM::Rect m_rectTabArea;
	IPVM::Rect m_rectTabROI;
	IPVM::Rect m_rectOverlay;
	IPVM::Rect m_rectInsulate;
	IPVM::Rect m_rectBotOverlay;
	IPVM::Rect m_rectScrab;

	BOOL m_bScrabReject;

	double m_dOverlay;
	double m_dInsulate;
	double m_dBotOverlay;
	double m_dBotOverlayLeft;
	double m_dBotOverlayRIght;
	////////////거대 불량

	bool isPetVinyl;
};

class SurfaceInspect
{
public:
	SurfaceInspect(IPVM::LoggerInterface& loggerDebug, CISCommSocket& ClientSocket);
	~SurfaceInspect();

	const CInspect& GetAt(long cameraIndex) const;
	CInspect& GetAt(long cameraIndex);

	void EnableManualMode(BOOL enable);
	void EnableNgTag(BOOL enable);
	int Inspect(int cameraIndex, const InspectionInputData& inputData, int offsetUp, int offsetDn, float fTabWidth, float fTabHeight, bool bInline, SURFACE_RESULT_INFO& o_result);

private:
	IPVM::LoggerInterface& m_loggerDebug;
	CISCommSocket& m_pClientSocket;
	class InspectInfo
	{
	public:
		InspectInfo();
		~InspectInfo();

		CCriticalSection m_surfaceInspectionCs;

		long m_initSizeX;
		long m_initSizeY;

		CInspect* m_inspect;

	};

	InspectInfo m_objs[2];
	void ResetSurfaceResult(int cameraIndex, CInspect& inspect);
	void GetSurfaceResult(CInspect& inspect, const InspectionInputData& inputData, SURFACE_RESULT_INFO& surfaceResultInfo);

	bool m_bInline;
};
