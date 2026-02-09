#include "pch.h"
#include "SurfaceInspect.h"
#include "InspectionInputData.h"
#include "SystemParameter.h"
//#include "../SharedModule/VisionHostShare/HostCommSocket.h"
#include "../SharedModule/CommonShare/DefectDef.h"
#include "../Inspection/basedef.h"

#include "Gadget/TimeCheck.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

SurfaceInspect::InspectInfo::InspectInfo()
	: m_initSizeX(-1)
	, m_initSizeY(-1)
{
	m_inspect = new CInspect(g_systemParameter.nAnodeCathode);
	m_inspect->m_nCellSize = g_systemParameter.nCamSingleDual;
	m_inspect->m_bLaserAndCathode[0] = g_systemParameter.nAnodeCathode == 0 && g_systemParameter.bIsLaserNotching == true;
	m_inspect->m_bLaserAndCathode[1] = g_systemParameter.nAnodeCathode == 0 && g_systemParameter.bIsLaserNotching == true;

	if (g_systemParameter.nInsulateType == 2)
	{
		m_inspect->m_bPrimer = TRUE;
	}
	else
	{
		m_inspect->m_bPrimer = FALSE;
	}
}

SurfaceInspect::InspectInfo::~InspectInfo()
{
	m_inspect->CloseInspect();
	delete m_inspect;
}

SurfaceInspect::SurfaceInspect(IPVM::LoggerInterface& loggerDebug, CISCommSocket& ClientSocket)
	: m_pClientSocket(ClientSocket)
	, m_loggerDebug(loggerDebug)
{
}

SurfaceInspect::~SurfaceInspect()
{
}

const CInspect& SurfaceInspect::GetAt(long cameraIndex) const
{
	return *m_objs[cameraIndex].m_inspect;
}

CInspect& SurfaceInspect::GetAt(long cameraIndex)
{
	return *m_objs[cameraIndex].m_inspect;
}

void SurfaceInspect::EnableManualMode(BOOL enable)
{
	for (long cameraIndex = 0; cameraIndex < 2; cameraIndex++)
	{
		GetAt(cameraIndex).m_bInspManual = enable;
	}
}

void SurfaceInspect::EnableNgTag(BOOL enable)
{
	for (long cameraIndex = 0; cameraIndex < 2; cameraIndex++)
	{
		GetAt(cameraIndex).m_bNgTag = enable;
	}
}

int SurfaceInspect::Inspect(int cameraIndex, const InspectionInputData& inputData, int offsetUp, int offsetDn, float fTabWidth, float fTabHeight, bool bInline, SURFACE_RESULT_INFO& o_result)
{
	auto camID = CString(cameraIndex == 0 ? _T("Up") : _T("Down"));

	m_bInline = bInline;

	auto& bright = inputData.m_imageBright;
	auto& dark = inputData.m_imageDark;

	int imageSizeX = bright.GetWidthBytes();
	int imageSizeY = bright.GetSizeY();

	CSingleLock lock(&m_objs[cameraIndex].m_surfaceInspectionCs, TRUE);

	if (m_objs[cameraIndex].m_initSizeX != imageSizeX ||
		m_objs[cameraIndex].m_initSizeY != imageSizeY)
	{
		// 이미지 Size가 달라지면 InitInspect 을 호출해야 맞지만
		// 두번 호출하면 죽는다. Inspect의 버그이다. m_bInit은 없애야 맞다고 생각하지만 그냥 남겨둔다

//		if (!GetAt(cameraIndex).m_bInit)	// sjj - 0311
		{
			GetAt(cameraIndex).InitInspect(imageSizeX, imageSizeY, g_systemParameter.nAnodeCathode, g_systemParameter.bUseNewSurfaceInspectLogic);
		}

		m_objs[cameraIndex].m_initSizeX = imageSizeX;
		m_objs[cameraIndex].m_initSizeY = imageSizeY;
	}

	IPVM::TimeCheck timeCheck;
	m_loggerDebug.Log(0, _T("%s-Inspect Start"), (LPCTSTR)camID);

	ResetSurfaceResult(cameraIndex, GetAt(cameraIndex));

	if (cameraIndex == 0)
		GetAt(cameraIndex).m_dscale_x1[cameraIndex] = g_systemParameter.m_dPixelToUmXUp / 1000;
	else
		GetAt(cameraIndex).m_dscale_x1[cameraIndex] = g_systemParameter.m_dPixelToUmXDown / 1000;

	GetAt(cameraIndex).m_dscale_y1[cameraIndex] = (g_systemParameter.m_dPixelToUmY * 2) / 1000;

	int count = GetAt(cameraIndex).Inspect(
		m_loggerDebug,
		cameraIndex,
		(BYTE*)bright.GetMem(),
		(BYTE*)dark.GetMem(),
		imageSizeX,
		imageSizeY,
		g_systemParameter.nInsulateType,
		g_systemParameter.nInspectionMargin,
		offsetUp,
		offsetDn,
		g_systemParameter.nCustomerSite,
		fTabWidth,
		fTabHeight);

	m_loggerDebug.Log(0, _T("inspect end(%s), Surface insp Tact Time(%.3f)"), (LPCTSTR)camID, timeCheck.Elapsed_ms());

	CString textTT;
	textTT.Format(_T("Find Scrab(%.3f), Overlay(%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, zipper(%.3fms))"), GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][2]/*scrab*/, GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][3], GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][4], GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][5], GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][6], GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][7], GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][8], GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][9], GetAt(cameraIndex).m_dInsSubTime1[cameraIndex][16]);
	m_loggerDebug.Log(0, textTT);

	GetSurfaceResult(GetAt(cameraIndex), inputData, o_result);

	if (-9999 == count)
	{
		int nAlarmCount = GetAt(cameraIndex).m_nTimeOutAlarmcount[cameraIndex];
		CString strPosition = (TRUE == inputData.m_isUp ? _T("UP") : _T("DOWN"));
		CString strDetail;
		strDetail.Format(_T("Error CellID : %d , Position : %s , Setting countinue Time Out Count  : %d"),inputData.m_cellID, strPosition, nAlarmCount);
		
		m_pClientSocket.SendAlarm_CIS(140, Share::ALARM_LEVEL::red, _T(""), strDetail, _T(""), _T(""));
		
		m_loggerDebug.Log(0, strDetail);
	}
	return count;
}

void SurfaceInspect::ResetSurfaceResult(int cameraIndex, CInspect& inspect)
{
	CString text;
	int i = cameraIndex;

	inspect.m_dInsulWidth[i] = 0.0f;
	inspect.m_dOverlayWidth[i] = 0.0f;

	for (int f = 0; f < 20; f++)
	{
		inspect.m_dInsSubTime1[i][f] = 0.0f;
	}
	
	for (long index = 0; index < 2; index++)
	{
		inspect.m_dBottomCoat[i][index] = 0.0f;
		inspect.m_dBottomOverlay[i][index] = 0.0f;
	}

	for (int n = 0; n < 100; n++)
	{
		inspect.m_dNewinspTime[i][n] = 0.0f;
	}

	//{
	//	CSingleLock lock(&inspect.m_cs_OverlayThreadEnd[i], TRUE);
	//	inspect.m_bOverlayThreadEnd[i] = false;
	//}

	//{
	//	CSingleLock lock2(&inspect.m_cs_ScrabThreadEnd[i], TRUE);
	//	inspect.m_bScrabThreadEnd[i] = false;
	//}

	text.Format(_T("Reset Surface Result(cam idx %d)"), i);
	m_loggerDebug.Log(0, text);

}


void SurfaceInspect::GetSurfaceResult(CInspect& inspect, const InspectionInputData& inputData, SURFACE_RESULT_INFO& surfaceResultInfo)
{
	int camID = (inputData.m_isUp) ? 0 : 1;

	CString text, temp;
	text = (inputData.m_isUp) ? _T("UP") : _T("DOWN");

	temp.Format(_T("Cell id %d, "), inputData.m_cellID);
	text += temp;

	surfaceResultInfo.m_nScrabRightBoundary = inspect.m_nScrabRightBoundary;
	surfaceResultInfo.m_rtInsulate = inspect.m_rtInsulate[camID];
	surfaceResultInfo.m_rtInsulationInspArea = inspect.m_rtInsulationInspArea[camID];
	surfaceResultInfo.m_rtOverlay = inspect.m_rtOverlay[camID];
	surfaceResultInfo.m_dInsulWidth = inspect.m_dInsulWidth[camID];
	surfaceResultInfo.m_dOverlayWidth = inspect.m_dOverlayWidth[camID];
	temp.Format(_T("InsulationWidth%.3f, OverlayWidth%.3f"), surfaceResultInfo.m_dInsulWidth, surfaceResultInfo.m_dOverlayWidth);
	text += temp;
		
	for (long index = 0; index < 2; index++)
	{
		surfaceResultInfo.m_dBottomCoat[index] = inspect.m_dBottomCoat[camID][index];
		surfaceResultInfo.m_dBottomOverlay[index] = inspect.m_dBottomOverlay[camID][index];

		temp.Format(_T("m_dBottomCoat(%d)%.3f, m_dBottomOverlay(%d)%.3f"), index, surfaceResultInfo.m_dBottomCoat[index], index, surfaceResultInfo.m_dBottomOverlay[index]);
		text += temp;
	}

	m_loggerDebug.Log(0, text);

	for (long index = 0; index < 10; index++) surfaceResultInfo.m_dInsTime[index] = inspect.m_dInsTime[camID][index];
	int imageSizeX = inputData.m_imageBright.GetWidthBytes();
	int imageSizeY = inputData.m_imageBright.GetSizeY();
	CRect rectReverse;
	surfaceResultInfo.m_inputTime = inputData.m_inputTime;
	surfaceResultInfo.fullImage = inputData.m_imageFull;
	surfaceResultInfo.brightImage = inputData.m_imageBright;
	surfaceResultInfo.darkImage = inputData.m_imageDark;
	surfaceResultInfo.isUp = inputData.m_isUp;
	surfaceResultInfo.cellID = inputData.m_cellID;
	surfaceResultInfo.rejectCount = 0;
	surfaceResultInfo.m_nFullDefectCount = inspect.m_nFullDefectCount[camID];
	surfaceResultInfo.m_rectCellArea = IPVM::FromMFC(inspect.m_rtCell[camID]);
	surfaceResultInfo.m_rectTabArea = IPVM::FromMFC(inspect.m_rtCellTab[camID]);
	surfaceResultInfo.m_rectTabROI = IPVM::FromMFC(inspect.m_rectTabROI[camID]);
	CRect botOverlayRect = CRect(inspect.m_dBottomCoat[camID][1], inspect.m_rtInsulate[camID].top, inspect.m_dBottomOverlay[camID][1], inspect.m_rtInsulate[camID].bottom);
	surfaceResultInfo.m_rectBotOverlay = IPVM::FromMFC(botOverlayRect);//결과 넘겨주는 값
	surfaceResultInfo.m_dOverlay = inspect.m_dOverlayWidth[camID]; //결과 넘겨주는 값
	surfaceResultInfo.m_dInsulate = inspect.m_dInsulWidth[camID]; //결과 넘겨주는 값
	surfaceResultInfo.m_rectOverlay = IPVM::FromMFC(inspect.m_rtOverlay[camID]);  //결과 넘겨주는 값
	surfaceResultInfo.m_rectInsulate = IPVM::FromMFC(inspect.m_rtInsulate[camID]); //결과 넘겨주는 값
	surfaceResultInfo.m_dBotOverlay = inspect.m_dBottomOverlay[camID][0];//결과 넘겨주는 값
	surfaceResultInfo.m_rectScrab = IPVM::FromMFC(inspect.m_rtScrabIns[camID]);	// sjj - 0412 변수 수정
	surfaceResultInfo.m_bScrabReject = inspect.m_bScrabReject[camID];
	BOOL bIsMiniCraterNG = FALSE;
	if (inspect.m_bMiniCrater[camID])
	{
		int nMiniCraterCount = 0;
		for (int i = 0; i < inspect.m_nFullDefectCount[camID]; i++)
		{
			if (3000 == inspect.m_nFullDefectType[camID][i]) {
				nMiniCraterCount++;
				if (nMiniCraterCount >= inspect.m_nMiniCrater_Count[camID])
				{
					bIsMiniCraterNG = TRUE;
					break;
				}
			}
		}		
		if (0 < nMiniCraterCount)
		m_loggerDebug.Log(0, _T("Cell ID [%d] , MiniCreater Count [%d]"), surfaceResultInfo.cellID, nMiniCraterCount);
	}
	BOOL bIsMiniPinholeNG = FALSE;
	if (inspect.m_bMiniPinhole[camID])
	{
		int nMiniPinholeCount = 0;
		for (int i = 0; i < inspect.m_nFullDefectCount[camID]; i++)
		{
			if (2000 == inspect.m_nFullDefectType[camID][i]) {
				nMiniPinholeCount++;
				if (nMiniPinholeCount >= inspect.m_nMiniPinhole_Count[camID])
				{
					bIsMiniPinholeNG = TRUE;
					break;
				}
			}
		}
		if (0 < nMiniPinholeCount)
		m_loggerDebug.Log(0, _T("Cell ID [%d] , MiniPinhole Count [%d]"), surfaceResultInfo.cellID , nMiniPinholeCount);
	}
	BOOL bIsMiniProtrutionNG = FALSE;
	if (inspect.m_bMiniProtrution[camID])
	{
		int nMiniProtrution = 0;
		for (int i = 0; i < inspect.m_nFullDefectCount[camID]; i++)
		{
			if (2102 == inspect.m_nFullDefectType[camID][i]) {
				nMiniProtrution++;
				if (nMiniProtrution >= inspect.m_nMiniProtrution_Count[camID])
				{
					bIsMiniProtrutionNG = TRUE;
					break;
				}
			}
		}
		if (0 < nMiniProtrution)
		m_loggerDebug.Log(0, _T("Cell ID [%d] , Mini Protrution Count [%d]"), surfaceResultInfo.cellID, nMiniProtrution);
	}
	BOOL bIsMiniDentNG = FALSE;
	if (inspect.m_bMiniDent[camID])
	{
		int nMiniDentCount = 0;
		for (int i = 0; i < inspect.m_nFullDefectCount[camID]; i++)
		{
			if (2005 == inspect.m_nFullDefectType[camID][i]) {
				nMiniDentCount++;
				if (nMiniDentCount >= inspect.m_nMiniDent_Count[camID])
				{
					bIsMiniDentNG = TRUE;
					break;
				}
			}
		}
		if(0<nMiniDentCount)
		m_loggerDebug.Log(0, _T("Cell ID [%d] , Mini Dent Count [%d]"), surfaceResultInfo.cellID, nMiniDentCount);
	}

	for (int i = 0; i < inspect.m_nFullDefectCount[camID]; i++)
	{
		rectReverse = inspect.m_rtFullDefect[camID][i];
		rectReverse.top = imageSizeY - inspect.m_rtFullDefect[camID][i].bottom;
		rectReverse.bottom = imageSizeY - inspect.m_rtFullDefect[camID][i].top;
		surfaceResultInfo.m_rtFullDefect[i] = rectReverse;												//불량영역
		//surfaceResultInfo.m_rtFullDefect[i] = inspect.m_rtFullDefect[camID][i];						//불량영역
		surfaceResultInfo.m_fmDefect[i] = inspect.m_fmDefect[camID][i];//origin							//불량이미지
		surfaceResultInfo.m_nFullDefectType[i] = inspect.m_nFullDefectType[camID][i];					//불량코드


		if (m_bInline)
		{
			if (surfaceResultInfo.m_nFullDefectType[i] <= 0)
			{
				continue;
			}

			surfaceResultInfo.rejectCount++;
		}
		else
		{
			if (surfaceResultInfo.m_nFullDefectType[i] != -1)
			{
				surfaceResultInfo.rejectCount++;
			}
		}
		if (2000 == inspect.m_nFullDefectType[camID][i])
		{
			if(FALSE == bIsMiniPinholeNG)
				inspect.m_strFullDefectName[camID][i] = _T("OK");
			else
				inspect.m_strFullDefectName[camID][i] = STR_PINHOLE;
		}

		if (2102 == inspect.m_nFullDefectType[camID][i])
		{
			if (FALSE == bIsMiniProtrutionNG)
				inspect.m_strFullDefectName[camID][i] = _T("OK");
			else
				inspect.m_strFullDefectName[camID][i] = STR_PROTRUSION;
		}

		if (2005 == inspect.m_nFullDefectType[camID][i])
		{
			if (FALSE == bIsMiniDentNG)
				inspect.m_strFullDefectName[camID][i] = _T("OK");
			else
				inspect.m_strFullDefectName[camID][i] = STR_DENT;
		}

		if (3000 == inspect.m_nFullDefectType[camID][i])
		{
			if (FALSE == bIsMiniCraterNG)
				inspect.m_strFullDefectName[camID][i] = _T("OK");
			else
				inspect.m_strFullDefectName[camID][i] = STR_CRATER;
		}

		if (6004 == inspect.m_nFullDefectType[camID][i]) {
			if (g_systemParameter.nCustomerSite == CIS_Site::ESOC)
			{
				inspect.m_strFullDefectName[camID][i] = STR_HEATING_WRINKLE;
				inspect.m_nFullDefectType[camID][i] = 1005;
			}
			else
				surfaceResultInfo.m_strFullDefectName[i] = STR_INSULATION_ISLAND;
		}
		else
			surfaceResultInfo.m_strFullDefectName[i] = inspect.m_strFullDefectName[camID][i];						//불량명
		

		surfaceResultInfo.m_strFullDefectName[i] = inspect.m_strFullDefectName[camID][i];						//불량명

		// type 이 -1 이면 good
		//if (surfaceResultInfo.m_nFullDefectType[camID][i] != -1)
		//{
		//	surfaceResultInfo.rejectCount++;
		//}

		surfaceResultInfo.m_rtRawDefect[i]	= inspect.m_rtDetailRawDefect[camID][i];
		surfaceResultInfo.m_dFullArea[i]	= inspect.m_dDetailFullArea[camID][i];
		surfaceResultInfo.m_dFullWidth[i]	= inspect.m_dDetailFullWidth[camID][i];
		surfaceResultInfo.m_dFullLength[i]	= inspect.m_dDetailFullLength[camID][i];



		for (int k = 0; k < 2; k++)
		{							
			//0백색1흑색
			surfaceResultInfo.m_dFullStdValue[i][k] = inspect.m_dFullStdValue[camID][i][k];						//불량편차값
			surfaceResultInfo.m_dFullCompactness[i][k] = inspect.m_dFullCompactness[camID][i][k];				//불량진원도
			surfaceResultInfo.m_dFullDistance[i][k] = inspect.m_dFullDistance[camID][i][k];						//흑백간의거리
			surfaceResultInfo.m_dFullPercent[i][k] = inspect.m_dFullPercent[camID][i][k];						//불량영역내 퍼센트
			surfaceResultInfo.m_dFullPeakDataW[i][k] = inspect.m_dFullPeakDataW[camID][i][k];					//백색Value [0] - OriginValue 1-Preprocessing value
			surfaceResultInfo.m_dFullPeakDataB[i][k] = inspect.m_dFullPeakDataB[camID][i][k];					//흑색Value [0] - OriginValue 1-Preprocessing value
		}

		for (int k = 0; k < 3; k++)
		{
			if (inspect.m_dFullAngle[camID][i][k] > 0)
			{
				surfaceResultInfo.m_dFullAngle[i] = inspect.m_dFullAngle[camID][i][k];
				break;
			}
			else
				surfaceResultInfo.m_dFullAngle[i] = 0;
		}//불량각도

		for (long type = 0; type < 3; type++)
		{
			surfaceResultInfo.m_dFullSizeX[i][type] = inspect.m_dFullSizeX[camID][i][type];			//불량사이즈X
			surfaceResultInfo.m_dFullSizeY[i][type] = inspect.m_dFullSizeY[camID][i][type];			//불량사이즈Y
		}
	}

	if (inspect.m_bIsCurCellTimeOut[camID])
	{
		rectReverse = CRect(0, 0, 0, 0);
		rectReverse.top = 0;
		rectReverse.bottom = 0;

		int nCount = 0;
		
		if (inspect.m_nFullDefectCount[camID] > 0)
			nCount = inspect.m_nFullDefectCount[camID];
		if (surfaceResultInfo.m_nFullDefectCount <= 0)
			surfaceResultInfo.m_nFullDefectCount = 1;

		surfaceResultInfo.m_rtFullDefect[nCount] = rectReverse;								//불량영역
		surfaceResultInfo.m_fmDefect[nCount] = NULL;//origin							//불량이미지
		surfaceResultInfo.m_strFullDefectName[nCount] = STR_TIMEOUT;							//불량코드

		for (int k = 0; k < 2; k++)
		{																		//0백색1흑색
			surfaceResultInfo.m_dFullStdValue[nCount][k] = 0;						//불량편차값
			surfaceResultInfo.m_dFullCompactness[nCount][k] = 0;					//불량진원도
			surfaceResultInfo.m_dFullDistance[nCount][k] = 0;						//흑백간의거리
			surfaceResultInfo.m_dFullPercent[nCount][k] = 0;						//불량영역내 퍼센트
			surfaceResultInfo.m_dFullPeakDataW[nCount][k] = 0;					//백색Value [0] - OriginValue 1-Preprocessing value
			surfaceResultInfo.m_dFullPeakDataB[nCount][k] = 0;					//흑색Value [0] - OriginValue 1-Preprocessing value
		}

		surfaceResultInfo.m_dFullAngle[nCount] = 0;
		for (long type = 0; type < 3; type++)
		{
			surfaceResultInfo.m_dFullSizeX[nCount][type] = 0;			//불량사이즈X
			surfaceResultInfo.m_dFullSizeY[nCount][type] = 0;			//불량사이즈Y
		}

		surfaceResultInfo.rejectCount++;
	}

	surfaceResultInfo.isPetVinyl = false;

}
