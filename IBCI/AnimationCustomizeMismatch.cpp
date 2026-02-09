#include "pch.h"
#include "AnimationCustomizeMismatch.h"
#include "AnimationData.h"
//#include "Common.h"
//#include "Log.h"
//#include "../InterfaceModules/HostData/SystemParameter.h"
//#include "../Common/Utility/SystemFont.h"
#include "../Module/Utility/SystemFont.h"

#include <Gadget/TimeCheck.h>

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AnimationCustomizeMismatch::AnimationCustomizeMismatch()
{
	COLOR_BOARD				= RGB(255, 255, 255);
	COLOR_AREA_FOIL			= RGB(180, 180, 180);
	COLOR_AREA_ELECTRODE	= RGB(0, 0, 0);
	COLOR_TEXT				= RGB(0, 0, 0);
	COLOR_VALUE_NORMAL		= RGB(0, 255, 0);
	COLOR_VALUE_CL_OVER		= RGB(255, 180, 0);
	COLOR_VALUE_SL_OVER		= RGB(255, 0, 0);

	m_BrushAnimation.CreateSolidBrush(COLOR_BOARD);

	m_PenText.CreatePen(PS_SOLID, 1, COLOR_TEXT);
	m_PenValueNormal.CreatePen(PS_SOLID, 1, COLOR_VALUE_NORMAL);
	m_PenValueCLOver.CreatePen(PS_SOLID, 1, COLOR_VALUE_CL_OVER);
	m_PenValueSLOVer.CreatePen(PS_SOLID, 1, COLOR_VALUE_SL_OVER);
	m_PenAreaFoil.CreatePen(PS_SOLID, 1, COLOR_AREA_FOIL);
	m_PenAreaElectrode.CreatePen(PS_SOLID, 1, COLOR_AREA_ELECTRODE);

	//COLOR_AXIS_BOARD = RGB(170, 170, 170);
	//COLOR_BOARD = RGB(0, 0, 0);
	//COLOR_BOARD_LINE = RGB(0, 24, 200);
	//COLOR_GRADATION = RGB(100, 100, 100);
	//COLOR_SPEC_MIN = RGB(255, 0, 0);
	//COLOR_SPEC_MAX = RGB(255, 0, 0);
	//COLOR_TEXT = RGB(20, 20, 20);
	//COLOR_NOMINAL = RGB(255, 255, 255);

	//COLOR_SPEC_MINAI = RGB(120, 120, 0);
	//COLOR_SPEC_MAXAI = RGB(120, 120, 0);

	//COLOR_SPEC_MINNG = RGB(10, 120, 40);
	//COLOR_SPEC_MAXNG = RGB(10, 120, 40);

	//SetDataColor(0, RGB(255, 255, 0));
	//SetDataColor(1, RGB(0, 255, 0));
	//SetDataColor(2, RGB(100, 255, 255));
	//SetDataColor(3, RGB(0, 0, 255));
	//SetDataColor(4, RGB(170, 170, 170));
	//SetDataColor(5, RGB(170, 170, 170));
	//SetDataColor(6, RGB(170, 170, 170));
	// 
	//SetDataColor(7, RGB(170, 170, 170));
	//SetDataColor(8, RGB(170, 170, 170));
	//SetDataColor(9, RGB(170, 170, 170));

	//m_PenBoardLine.CreatePen(PS_SOLID, 1, COLOR_BOARD_LINE);
	//m_PenXText.CreatePen(PS_SOLID, 1, COLOR_GRADATION);
	//m_BrushGraph.CreateSolidBrush(COLOR_BOARD);
	//m_Pen_MaxAi.CreatePen(PS_SOLID, 1, COLOR_SPEC_MAXAI);
	//m_Pen_MinAi.CreatePen(PS_SOLID, 1, COLOR_SPEC_MINAI);
	//m_PenMaxNg.CreatePen(PS_SOLID, 1, COLOR_SPEC_MAXNG);
	//m_PenMinNg.CreatePen(PS_SOLID, 1, COLOR_SPEC_MINNG);
	//m_PenSpecMax.CreatePen(PS_SOLID, 1, COLOR_SPEC_MAX);
	//m_PenSpecMin.CreatePen(PS_SOLID, 1, COLOR_SPEC_MIN);
	//m_PenNominal.CreatePen(PS_SOLID, 1, COLOR_NOMINAL);

	//m_PenGrid.CreatePen(PS_DOT, 1, COLOR_GRADATION);
	//m_PenInBox.CreatePen(PS_SOLID, 2, RGB(50, 250, 50));
}

AnimationCustomizeMismatch::~AnimationCustomizeMismatch()
{
	m_BrushAnimation.DeleteObject();
	m_PenText.DeleteObject();
	m_PenValueNormal.DeleteObject();
	m_PenValueCLOver.DeleteObject();
	m_PenValueSLOVer.DeleteObject();
	m_PenAreaFoil.DeleteObject();
	m_PenAreaElectrode.DeleteObject();
	m_FontTitle.DeleteObject();
	m_FontText.DeleteObject();
	m_FontValueNormal.DeleteObject();
	m_FontValueCLOver.DeleteObject();
	m_FontValueSLOver.DeleteObject();
}

void AnimationCustomizeMismatch::Draw(CDC* dc, const DefectPointXY& data)
{
}
COLORREF AnimationCustomizeMismatch::GetStrColor(ResultSpec temp,float fValue)
{
	//COLORREF	COLOR_VALUE_NORMAL;			// Green (0, 255, 0)
		//COLORREF	COLOR_VALUE_CL_OVER;		// Orange (255, 180, 0)
		//COLORREF	COLOR_VALUE_SL_OVER;		// Red (255, 0, 0)
	COLORREF Result = COLOR_VALUE_NORMAL;
	if (fValue< temp.fMeasureStandard + temp.fMeasureOffsetLCL || fValue>temp.fMeasureStandard + temp.fMeasureOffsetUCL)
	{
		Result = COLOR_VALUE_CL_OVER;
	}
	if (fValue< temp.fMeasureStandard + temp.fMeasureOffsetLSL || fValue>temp.fMeasureStandard + temp.fMeasureOffsetUSL)
	{
		Result = COLOR_VALUE_SL_OVER;
	}

	return Result;
}
float AnimationCustomizeMismatch::GetDrawValue(ResultSpec temp, float fValue, float fSize)// 그려주는 사이즈
{
	if (fSize > 0&& fValue>0)
	{
		float fTemp = temp.fMeasureStandard / fSize;
		float Result = 0.f;	
		if (fValue< temp.fMeasureStandard + temp.fMeasureOffsetLCL || fValue>temp.fMeasureStandard + temp.fMeasureOffsetUCL)
		{
            if (fValue < temp.fMeasureStandard + temp.fMeasureOffsetLCL)
                return (temp.fMeasureOffsetLCL) / fTemp;
            else //if (fValue < temp.fMeasureStandard + temp.fMeasureOffsetLCL)
                return (temp.fMeasureOffsetUCL) / fTemp;
		}
		else if (fValue< temp.fMeasureStandard + temp.fMeasureOffsetLSL || fValue>temp.fMeasureStandard + temp.fMeasureOffsetUSL)
		{
			return (fValue - temp.fMeasureStandard) / fTemp*1.2;
		}
		else
		{
			
			return (fValue - temp.fMeasureStandard) / fTemp;
		}
	}
	

	return 0.f;
}
void AnimationCustomizeMismatch::Draw(CDC* dc, const AnimationData& data, bool isDebugMode)
{
	IPVM::TimeCheck tc;

	// Animation View Size
	//CRect rt(0, 0, m_viewSizeX, m_viewSizeY);
	//dc->FillSolidRect(rt, RGB(120, 120, 120));
	
	// Animation Valid Area Size
	CRect rtAnimation(0, 0, m_viewSizeX, m_viewSizeY);
	auto nWidth = rtAnimation.Width();
	auto nHeight = rtAnimation.Height();
	auto nWidthCenter = nWidth * 0.5;
	auto nHeightCenter = nHeight *  0.5;
	dc->FillSolidRect(rtAnimation, COLOR_BOARD);
	
	// Text Area
	int nTextBoxWidth = nWidth * 0.1;
    int nTextBoxHeight = nHeight * 0.1;
	
	CRect rtTitle(nWidthCenter - (nTextBoxWidth/2), 0, nWidthCenter + (nTextBoxWidth / 2), nTextBoxHeight);
	dc->FillSolidRect(rtTitle, COLOR_BOARD);
	dc->SetTextAlign(TA_CENTER);
	dc->SetTextColor(RGB(0, 0, 0));
	dc->SelectObject(&m_FontTitle);
	
	//CString title = data.m_title;
	CString strTitle = _T("Mismatch Animation");
	dc->TextOut(rtTitle.CenterPoint().x, rtTitle.CenterPoint().y, strTitle);


	float fTopTotalWidth = 0.f;
	float fBackTotalWidth = 0.f;

	fTopTotalWidth = data.m_Mismatch.m_fTopFullLength;
	fBackTotalWidth = data.m_Mismatch.m_fBackFullLength;

	
	// Total Width
	CRect rtTotalWidth(nWidth - nTextBoxWidth * 2, nHeight * 0.01, nWidth, nTextBoxHeight * 3);
	//dc->FillSolidRect(rtTotalWidth, RGB(255, 0, 0));
	CString strTotalWidth_Top;
	strTotalWidth_Top.Format(_T("Total Width(Top) : %.2fmm"), fTopTotalWidth);
	CString strTotalWidth_Back;
	strTotalWidth_Back.Format(_T("Total Width(Back) : %.2fmm"), fBackTotalWidth);
	dc->FillSolidRect(rtTotalWidth, COLOR_BOARD);
	dc->SetTextAlign(TA_LEFT);
	dc->SetTextColor(RGB(0, 0, 0));
	dc->SelectObject(&m_FontText);
	dc->TextOut(rtTotalWidth.left, rtTotalWidth.top, strTotalWidth_Top);
	dc->TextOut(rtTotalWidth.left, rtTotalWidth.CenterPoint().y, strTotalWidth_Back);


	// Back Vision name
	CRect rtVisionBack;
	CString strVisionBackName = _T("[Back]");
	rtVisionBack.SetRect(nWidth * 0.02, nHeightCenter + nTextBoxHeight * 3, nTextBoxWidth / 2, nHeightCenter + nTextBoxHeight * 4);
	//rtVisionBack.SetRect(nWidth * 0.02, nHeightCenter - nTextBoxHeight * 5, nTextBoxWidth / 2, nHeightCenter - nTextBoxHeight * 4);
	dc->FillSolidRect(rtVisionBack, COLOR_BOARD);
	dc->SetTextAlign(TA_CENTER);
	dc->SetTextColor(RGB(0, 0, 0));
	dc->SelectObject(&m_FontText);
	dc->TextOut(rtVisionBack.CenterPoint().x, rtVisionBack.CenterPoint().y, strVisionBackName);

	// Top Vision name
	CRect rtVisionTop;
	CString strVisionTopName = _T("[Top]");
	//rtVisionTop.SetRect(nWidth * 0.02, nHeightCenter + nTextBoxHeight * 3, nTextBoxWidth / 2, nHeightCenter + nTextBoxHeight * 4);
	rtVisionTop.SetRect(nWidth * 0.02, nHeightCenter - nTextBoxHeight * 5, nTextBoxWidth / 2, nHeightCenter - nTextBoxHeight * 4);
	dc->FillSolidRect(rtVisionTop, COLOR_BOARD);
	dc->SetTextAlign(TA_CENTER);
	dc->SetTextColor(RGB(0, 0, 0));
	dc->SelectObject(&m_FontText);
	dc->TextOut(rtVisionTop.CenterPoint().x, rtVisionTop.CenterPoint().y, strVisionTopName);

	// Foil Area
    int nFoilHeight = nHeight * 0.1;
    CRect rtAreaFoil(0, nHeightCenter - (nFoilHeight / 2) + nFoilHeight, nWidth, nHeightCenter + (nFoilHeight / 2) + nFoilHeight);
	dc->FillSolidRect(rtAreaFoil, COLOR_AREA_FOIL);

	bool bRollerOS = data.m_bRollerOS;
	// OS/DS Diaplay (이거 좌/우 반전 가능하게 해야함)
    BOOL bOSDS = data.m_bOSDS;
    if (bOSDS)
    {
        CString strOSDL;
        dc->SetTextColor(RGB(0, 150, 255));
        dc->SelectObject(&m_FontText);
        strOSDL.Format(_T("[OS]"));
        dc->SetTextAlign(TA_LEFT);
        dc->TextOut(rtAreaFoil.left, rtAreaFoil.top, strOSDL);
        strOSDL.Format(_T("[DS]"));
        dc->SetTextAlign(TA_RIGHT);
        dc->TextOut(rtAreaFoil.right, rtAreaFoil.top, strOSDL);
	}
    else
    {
        CString strOSDL;
        dc->SetTextColor(RGB(0, 150, 255));
        dc->SelectObject(&m_FontText);
        strOSDL.Format(_T("[DS]"));
        dc->SetTextAlign(TA_LEFT);
        dc->TextOut(rtAreaFoil.left, rtAreaFoil.top, strOSDL);
        strOSDL.Format(_T("[OS]"));
        dc->SetTextAlign(TA_RIGHT);
        dc->TextOut(rtAreaFoil.right, rtAreaFoil.top, strOSDL);
	}
	
	// Electrode(Lane#) Area
	int nLaneWidth = nWidth * 0.2;
	int nLaneHeight = nHeight * 0.05;
	//const int nLaneCnt = 2;				// 이건 Lane Data Num으로 받아서 하자
	//const int nLaneCnt = 1;				// 이건 Lane Data Num으로 받아서 하자
	int nLaneCnt = data.m_nLaneCount;				// 이건 Lane Data Num으로 받아서 하자
	//data.m_nLaneCount
	std::vector<CRect> vecrtLaneBack;
	std::vector<CRect> vecrtLaneBackWidth;
	std::vector<CRect> vecrtLaneBackFoil;
	std::vector<CRect> vecrtLaneTopFoil;
	std::vector<CRect> vecrtLaneTop;
	std::vector<CRect> vecrtLaneTopWidth;
	std::vector<CRect> vecrtLaneMismatchLeft;
	std::vector<CRect> vecrtLaneMismatchRight;
	std::vector<CRect> vecrtLaneName;
	std::vector<CRect> vecrtLaneBack_Draw;
	std::vector<CRect> vecrtLaneTop_Draw;

	vecrtLaneBack_Draw.resize(nLaneCnt);
	vecrtLaneTop_Draw.resize(nLaneCnt);
	vecrtLaneBack.resize(nLaneCnt);	
	vecrtLaneBackWidth.resize(nLaneCnt);
	vecrtLaneBackFoil.resize(nLaneCnt);
	vecrtLaneTopFoil.resize(nLaneCnt);
	vecrtLaneTop.resize(nLaneCnt);
	vecrtLaneTopWidth.resize(nLaneCnt);
	vecrtLaneMismatchLeft.resize(nLaneCnt);
	vecrtLaneMismatchRight.resize(nLaneCnt);
	vecrtLaneName.resize(nLaneCnt);
	
	int nLaneStartCenter = (int)((nWidth / nLaneCnt)) * 0.5;

	for (int nLaneIdx = 0; nLaneIdx < nLaneCnt; nLaneIdx++)
	{
		int nLaneCenter = nLaneStartCenter * ((nLaneIdx * 2) + 1);
		
		// 그냥 이값을 수정해야 하나?

		int nMismatchLeft_Top = 0.f; // left foil spec 대비 ...
		int nMismatchRight_Top = 0.f; //left coating spec 대비... + foilspec대비 위치 값

		//
		int nMismatchLeft_Back = 0.f;
		int nMismatchRight_Back = 0.f;

		
		int nStandard_Left = nLaneCenter - (nLaneWidth / 2);
		int nStandard_Right = nLaneCenter + (nLaneWidth / 2);

		int nLeftNormalSize;
		int nRightNormalSize;

		//rtAnimation m_viewSizeX
		if (nLaneIdx == 0)
		{
			nLeftNormalSize = nStandard_Left;
			nRightNormalSize = nLaneWidth / 2;
		}
		else if (nLaneIdx == 1)
		{
			nLeftNormalSize = nLaneWidth / 2;
			nRightNormalSize = m_viewSizeX - nStandard_Right;
		}
        if (data.m_Mismatch.m_vecResSpec.size() < 1)
            return;

        if (data.m_Mismatch.m_vecResSpec.size() < 3)
            return;

        int nLeftBack = 0 , nRightBack = 0, nLeftTop = 0, nRightTop = 0;
        
		if (nLaneCnt == 1)
        {
            if (bOSDS)
            {
                nLeftBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2].fLaneWidth, nLeftNormalSize);
                nRightBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2 + 1],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2 + 1].fLaneWidth, nRightNormalSize);

                // Top Vision Lane #
                nLeftTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2].fLaneWidth, nLeftNormalSize);
                nRightTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2 + 1],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2 + 1].fLaneWidth, nRightNormalSize);
			}
            else
            {
                nLeftBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx +2],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx + 2].fLaneWidth, nLeftNormalSize);
                nRightBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2].fLaneWidth, nRightNormalSize);

                // Top Vision Lane #
                nLeftTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx + 2],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx + 2].fLaneWidth, nLeftNormalSize);
                nRightTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2].fLaneWidth, nRightNormalSize);
			}
		}
        else if (nLaneCnt == 2)
        {
            if (bOSDS)
            {
                if (data.m_Mismatch.m_vecResDownLane.size() == 0)
                {
                    return;
                }
                nLeftBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2].fLaneWidth, nLeftNormalSize);
                nRightBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2 + 1],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2 + 1].fLaneWidth, nRightNormalSize);

                // Top Vision Lane #
                nLeftTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2].fLaneWidth, nLeftNormalSize);
                nRightTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2 + 1],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2 + 1].fLaneWidth, nRightNormalSize);
			}
            else
            {
                nLeftBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 1 : 3],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 1 : 3].fLaneWidth,
                    nLeftNormalSize);
                nRightBack = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 0 : 2],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 0 : 2].fLaneWidth,
                    nRightNormalSize);

                // Top Vision Lane #
                nLeftTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 1 : 3],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 1 : 3].fLaneWidth,
                    nLeftNormalSize);
                nRightTop = GetDrawValue(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 0 : 2],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 0 : 2].fLaneWidth,
                    nRightNormalSize);
			}
		}

        vecrtLaneBack[nLaneIdx].SetRect(
            nStandard_Left, rtAreaFoil.bottom, nStandard_Right, rtAreaFoil.bottom + nLaneHeight);
        vecrtLaneTop[nLaneIdx].SetRect(
            nStandard_Left, rtAreaFoil.top - nLaneHeight * 2, nStandard_Right, rtAreaFoil.top);
		
		// first Lane
		if (nLaneIdx == 0)
		{
			// case: lane 1
			if (nLaneCnt == 1)
			{
				nRightNormalSize = m_viewSizeX - nStandard_Right;
                int nRightBack2 = 0, nRightTop2 = 0; 
                if (bOSDS)
                {
                    nRightBack2 = GetDrawValue(data.m_Mismatch.m_vecResSpec[2],
                        data.m_Mismatch.m_vecResDownLane[2].fLaneWidth, nRightNormalSize);
                    nRightTop2 = GetDrawValue(data.m_Mismatch.m_vecResSpec[2],
                        data.m_Mismatch.m_vecResUpLane[2].fLaneWidth, nRightNormalSize);
				}
                else
                {
                    nRightBack2 = GetDrawValue(data.m_Mismatch.m_vecResSpec[0],
                        data.m_Mismatch.m_vecResDownLane[0].fLaneWidth, nRightNormalSize);
                    nRightTop2 = GetDrawValue(data.m_Mismatch.m_vecResSpec[0],
                        data.m_Mismatch.m_vecResUpLane[0].fLaneWidth, nRightNormalSize);
				}
                if (bOSDS) // OS DS
                {
                    vecrtLaneBack_Draw[nLaneIdx].SetRect(nStandard_Left + nLeftBack, rtAreaFoil.bottom,
                        nStandard_Right + nRightBack2, rtAreaFoil.bottom + nLaneHeight);
                    vecrtLaneTop_Draw[nLaneIdx].SetRect(nStandard_Left + nLeftTop, rtAreaFoil.top - nLaneHeight,
                        nStandard_Right + nRightTop2, rtAreaFoil.top);
				}
                else // DS OS
                {
                    vecrtLaneBack_Draw[nLaneIdx].SetRect(nStandard_Left - nLeftBack, rtAreaFoil.bottom,
                        nStandard_Right - nRightBack2, rtAreaFoil.bottom + nLaneHeight);
                    vecrtLaneTop_Draw[nLaneIdx].SetRect(nStandard_Left - nLeftTop, rtAreaFoil.top - nLaneHeight,
                        nStandard_Right - nRightTop2, rtAreaFoil.top);
				}
			}
			// case: lane 2
			else
			{
                if (bOSDS)
                {
                    vecrtLaneBack_Draw[nLaneIdx].SetRect(nStandard_Left + nLeftBack, rtAreaFoil.bottom,
                        nStandard_Right + nRightBack, rtAreaFoil.bottom + nLaneHeight);
                    vecrtLaneTop_Draw[nLaneIdx].SetRect(nStandard_Left + nLeftTop, rtAreaFoil.top - nLaneHeight,
                        nStandard_Right + nRightTop, rtAreaFoil.top);
                }
                else
                {
                    vecrtLaneBack_Draw[nLaneIdx].SetRect(nStandard_Left - nLeftBack, rtAreaFoil.bottom,
                        nStandard_Right - nRightBack, rtAreaFoil.bottom + nLaneHeight);
                    vecrtLaneTop_Draw[nLaneIdx].SetRect(nStandard_Left - nLeftTop, rtAreaFoil.top - nLaneHeight,
                        nStandard_Right - nRightTop, rtAreaFoil.top);
                }
			}	
		}
		// second lane (lane2)
		else if (nLaneIdx == 1)
		{
            if (bOSDS)
            {
                vecrtLaneBack_Draw[nLaneIdx].SetRect(nStandard_Left + nLeftBack + nRightBack, rtAreaFoil.bottom,
                    nStandard_Right + nRightBack, rtAreaFoil.bottom + nLaneHeight);
                vecrtLaneTop_Draw[nLaneIdx].SetRect(nStandard_Left + nLeftTop + nRightTop, rtAreaFoil.top - nLaneHeight,
                    nStandard_Right + nRightTop, rtAreaFoil.top);
			}
            else
            {
                vecrtLaneBack_Draw[nLaneIdx].SetRect(nStandard_Left - nLeftBack - nRightBack, rtAreaFoil.bottom,
                    nStandard_Right - nRightBack, rtAreaFoil.bottom + nLaneHeight);
                vecrtLaneTop_Draw[nLaneIdx].SetRect(nStandard_Left - nLeftTop - nRightTop, rtAreaFoil.top - nLaneHeight,
                    nStandard_Right - nRightTop, rtAreaFoil.top);
			}
		}
		
		dc->FillSolidRect(vecrtLaneBack_Draw[nLaneIdx], COLOR_AREA_ELECTRODE);

		
		dc->FillSolidRect(vecrtLaneTop_Draw[nLaneIdx], COLOR_AREA_ELECTRODE);

		// Back/Top Width Value (data 연동 필요)
		CString strWidth;
			
		// Text 
		// Back electrode
        vecrtLaneBackWidth[nLaneIdx].SetRect(nLaneCenter - (nTextBoxWidth / 2), vecrtLaneBack[nLaneIdx].bottom,
            nLaneCenter + (nTextBoxWidth / 2), vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
        //dc->FillSolidRect(vecrtLaneBackWidth[nLaneIdx], COLOR_VALUE_SL_OVER);
        if (nLaneCnt == 1)
        {
            strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 2 : 1].fLaneWidth);
		}
        else
        {
            if (bOSDS)
				strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 2 : 1].fLaneWidth);
            else
                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 1 : 2].fLaneWidth);
		}
        dc->FillSolidRect(vecrtLaneBackWidth[nLaneIdx], COLOR_BOARD);
        dc->SetTextAlign(TA_CENTER);
        //dc->SetTextColor(COLOR_VALUE_NORMAL);
        if (nLaneCnt == 1)
        {
            dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 2 : 1], data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 2 : 1].fLaneWidth));   
        }
        else // 2 Lane
        {
            if (bOSDS)
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 2 : 1], data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 2 : 1].fLaneWidth));
            else if (!bOSDS)
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 1 : 2], data.m_Mismatch.m_vecResDownLane[nLaneIdx ? 1 : 2].fLaneWidth));
        }
        
        dc->SelectObject(&m_FontValueNormal);
        dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x, vecrtLaneBackWidth[nLaneIdx].top, strWidth);

        // Top electrode
        vecrtLaneTopWidth[nLaneIdx].SetRect(nLaneCenter - (nTextBoxWidth / 2),
            vecrtLaneTop[nLaneIdx].top - nTextBoxHeight, nLaneCenter + (nTextBoxWidth / 2), vecrtLaneTop[nLaneIdx].top);
        //dc->FillSolidRect(vecrtLaneTopWidth[nLaneIdx]	, COLOR_VALUE_SL_OVER);
        if (nLaneCnt == 1)
        {
            strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 2 : 1].fLaneWidth);
        }
        else
        {
            if (bOSDS)
                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 2 : 1].fLaneWidth);
            else
                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 1 : 2].fLaneWidth);
        }
        dc->FillSolidRect(vecrtLaneTopWidth[nLaneIdx], COLOR_BOARD);
        dc->SetTextAlign(TA_CENTER);
        //dc->SetTextColor(COLOR_VALUE_NORMAL);
        if (nLaneCnt == 1)
        {
            dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 2 : 1], data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 2 : 1].fLaneWidth));   
        }
        else // 2 Lane
        {
            if (bOSDS)
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 2 : 1], data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 2 : 1].fLaneWidth));
            else if (!bOSDS)
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx ? 1 : 2], data.m_Mismatch.m_vecResUpLane[nLaneIdx ? 1 : 2].fLaneWidth));
        }
        dc->SelectObject(&m_FontValueNormal);
        dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x, vecrtLaneTopWidth[nLaneIdx].top, strWidth);

		// lane 1 left
		if (nLaneIdx == 0)
		{
            if (nLaneCnt == 1)
            {
                if (bOSDS)
                {
                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                        data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2].fLaneWidth));
                    dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneTopWidth[nLaneIdx].top, strWidth);

                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                        data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2].fLaneWidth));
                    dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneBackWidth[nLaneIdx].top, strWidth);
                }
                else
                {
                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx + 2].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx + 2],
                        data.m_Mismatch.m_vecResUpLane[nLaneIdx + 2].fLaneWidth));
                    dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneTopWidth[nLaneIdx].top, strWidth);

                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx + 2].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx + 2],
                        data.m_Mismatch.m_vecResDownLane[nLaneIdx + 2].fLaneWidth));
                    dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneBackWidth[nLaneIdx].top, strWidth);
                }   
			}
            else if (nLaneCnt == 2)
            {
                if (bOSDS)
                {
                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                        data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2].fLaneWidth));
                    dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneTopWidth[nLaneIdx].top, strWidth);

                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2],
                        data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2].fLaneWidth));
                    dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneBackWidth[nLaneIdx].top, strWidth);
                }
                else
                {
                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx + 3].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx + 3],
                        data.m_Mismatch.m_vecResUpLane[nLaneIdx + 3].fLaneWidth));
                    dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneTopWidth[nLaneIdx].top, strWidth);

                    strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx + 3].fLaneWidth);
                    dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx + 3],
                        data.m_Mismatch.m_vecResDownLane[nLaneIdx + 3].fLaneWidth));
                    dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x - nLaneWidth,
                        vecrtLaneBackWidth[nLaneIdx].top, strWidth);
                }   
			}
		}
		// lane 2
        else if (nLaneIdx == 1)
        {
            if (bOSDS)
            {
                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2 + 1].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2 + 1],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx * 2 + 1].fLaneWidth));
                dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneTopWidth[nLaneIdx].top,
                    strWidth);

                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2 + 1].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx * 2 + 1],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx * 2 + 1].fLaneWidth));
                dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneBackWidth[nLaneIdx].top,
                    strWidth);
            }
            else
            {
                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[0].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[0],
                    data.m_Mismatch.m_vecResUpLane[0].fLaneWidth));
                dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneTopWidth[nLaneIdx].top,
                    strWidth);

                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[0].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[0],
                    data.m_Mismatch.m_vecResDownLane[0].fLaneWidth));
                dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneBackWidth[nLaneIdx].top,
                    strWidth);
			}
		}

		// lane 1 right
		if (nLaneCnt == 1)
        {
            if (bOSDS)
            {
                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx + 2].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx + 2],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx + 2].fLaneWidth));
                dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneTopWidth[nLaneIdx].top,
                    strWidth);

                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx + 2].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx + 2],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx + 2].fLaneWidth));
                dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneBackWidth[nLaneIdx].top,
                    strWidth);
			}
            else
            {
            
				strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResUpLane[nLaneIdx].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx],
                    data.m_Mismatch.m_vecResUpLane[nLaneIdx].fLaneWidth));
                dc->TextOut(vecrtLaneTopWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneTopWidth[nLaneIdx].top,
                    strWidth);

                strWidth.Format(_T("%.2f"), data.m_Mismatch.m_vecResDownLane[nLaneIdx].fLaneWidth);
                dc->SetTextColor(GetStrColor(data.m_Mismatch.m_vecResSpec[nLaneIdx],
                    data.m_Mismatch.m_vecResDownLane[nLaneIdx].fLaneWidth));
                dc->TextOut(vecrtLaneBackWidth[nLaneIdx].CenterPoint().x + nLaneWidth, vecrtLaneBackWidth[nLaneIdx].top,
                    strWidth);
            }
		}


		// Mismatch Value (data 연동 필요)
        if (bOSDS)
        {
            CString strMismatch;
            vecrtLaneMismatchLeft[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].left - (nTextBoxWidth / 2),
                vecrtLaneBack[nLaneIdx].bottom, vecrtLaneBack[nLaneIdx].left,
                vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
            //dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_VALUE_CL_OVER);
            strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[(nLaneIdx * 2) + 0]);
            dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_BOARD);
            dc->SetTextAlign(TA_LEFT);
            //dc->SetTextColor(COLOR_VALUE_NORMAL);
            dc->SetTextColor(COLOR_VALUE_NORMAL);
            dc->SelectObject(&m_FontValueNormal);
            dc->TextOut(vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().x,
                vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().y, strMismatch);

            vecrtLaneMismatchRight[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].right, vecrtLaneBack[nLaneIdx].bottom,
                vecrtLaneBack[nLaneIdx].right + nTextBoxWidth / 2, vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
            //dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_VALUE_CL_OVER);
            strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[(nLaneIdx * 2) + 1]);
            dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_BOARD);
            dc->SetTextAlign(TA_RIGHT);
            dc->SetTextColor(COLOR_VALUE_NORMAL);
            dc->SelectObject(&m_FontValueNormal);
            dc->TextOut(vecrtLaneMismatchRight[nLaneIdx].CenterPoint().x,
                vecrtLaneMismatchRight[nLaneIdx].CenterPoint().y, strMismatch);
		}
        else
        {
            if (nLaneCnt == 1)
			{
                CString strMismatch;
                vecrtLaneMismatchLeft[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].left - (nTextBoxWidth / 2),
                    vecrtLaneBack[nLaneIdx].bottom, vecrtLaneBack[nLaneIdx].left,
                    vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
                //dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_VALUE_CL_OVER);
                strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[(nLaneIdx * 2) + 1]);
                dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_BOARD);
                dc->SetTextAlign(TA_LEFT);
                dc->SetTextColor(COLOR_VALUE_NORMAL);
                dc->SelectObject(&m_FontValueNormal);
                dc->TextOut(vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().x,
                    vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().y, strMismatch);

                vecrtLaneMismatchRight[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].right, vecrtLaneBack[nLaneIdx].bottom,
                    vecrtLaneBack[nLaneIdx].right + nTextBoxWidth / 2, vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
                //dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_VALUE_CL_OVER);
                strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[(nLaneIdx * 2) + 0]);
                dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_BOARD);
                dc->SetTextAlign(TA_RIGHT);
                dc->SetTextColor(COLOR_VALUE_NORMAL);
                dc->SelectObject(&m_FontValueNormal);
                dc->TextOut(vecrtLaneMismatchRight[nLaneIdx].CenterPoint().x,
                    vecrtLaneMismatchRight[nLaneIdx].CenterPoint().y, strMismatch);
			}
            else
            {
                if (nLaneIdx == 0)
                {
                    CString strMismatch;
                    vecrtLaneMismatchLeft[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].left - (nTextBoxWidth / 2),
                        vecrtLaneBack[nLaneIdx].bottom, vecrtLaneBack[nLaneIdx].left,
                        vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
                    //dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_VALUE_CL_OVER);
                    strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[nLaneIdx + 3]);
                    dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_BOARD);
                    dc->SetTextAlign(TA_LEFT);
                    dc->SetTextColor(COLOR_VALUE_NORMAL);
                    dc->SelectObject(&m_FontValueNormal);
                    dc->TextOut(vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().x,
                        vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().y, strMismatch);

                    vecrtLaneMismatchRight[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].right,
                        vecrtLaneBack[nLaneIdx].bottom, vecrtLaneBack[nLaneIdx].right + nTextBoxWidth / 2,
                        vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
                    //dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_VALUE_CL_OVER);
                    strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[nLaneIdx + 2]);
                    dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_BOARD);
                    dc->SetTextAlign(TA_RIGHT);
                    dc->SetTextColor(COLOR_VALUE_NORMAL);
                    dc->SelectObject(&m_FontValueNormal);
                    dc->TextOut(vecrtLaneMismatchRight[nLaneIdx].CenterPoint().x,
                        vecrtLaneMismatchRight[nLaneIdx].CenterPoint().y, strMismatch);
				}
                else
                {
                    CString strMismatch;
                    vecrtLaneMismatchLeft[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].left - (nTextBoxWidth / 2),
                        vecrtLaneBack[nLaneIdx].bottom, vecrtLaneBack[nLaneIdx].left,
                        vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
                    //dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_VALUE_CL_OVER);
                    strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[1]);
                    dc->FillSolidRect(vecrtLaneMismatchLeft[nLaneIdx], COLOR_BOARD);
                    dc->SetTextAlign(TA_LEFT);
                    dc->SetTextColor(COLOR_VALUE_NORMAL);
                    dc->SelectObject(&m_FontValueNormal);
                    dc->TextOut(vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().x,
                        vecrtLaneMismatchLeft[nLaneIdx].CenterPoint().y, strMismatch);

                    vecrtLaneMismatchRight[nLaneIdx].SetRect(vecrtLaneBack[nLaneIdx].right,
                        vecrtLaneBack[nLaneIdx].bottom, vecrtLaneBack[nLaneIdx].right + nTextBoxWidth / 2,
                        vecrtLaneBack[nLaneIdx].bottom + nTextBoxHeight);
                    //dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_VALUE_CL_OVER);
                    strMismatch.Format(_T("%.2f"), data.m_Mismatch.m_vecfDiffWidth[0]);
                    dc->FillSolidRect(vecrtLaneMismatchRight[nLaneIdx], COLOR_BOARD);
                    dc->SetTextAlign(TA_RIGHT);
                    dc->SetTextColor(COLOR_VALUE_NORMAL);
                    dc->SelectObject(&m_FontValueNormal);
                    dc->TextOut(vecrtLaneMismatchRight[nLaneIdx].CenterPoint().x,
                        vecrtLaneMismatchRight[nLaneIdx].CenterPoint().y, strMismatch);
				}
			}
		}

		
		
		
		// Lane Name
        vecrtLaneName[nLaneIdx].SetRect(nLaneCenter - (nTextBoxWidth / 2),
            vecrtLaneTop[nLaneIdx].top - nTextBoxHeight * 3, nLaneCenter + (nTextBoxWidth / 2),
            vecrtLaneTop[nLaneIdx].top - nTextBoxHeight * 2);
		CString strLaneName;
		strLaneName.Format(_T("Lane #%02d"), nLaneIdx + 1);
		dc->FillSolidRect(vecrtLaneName[nLaneIdx], COLOR_BOARD);
		dc->SetTextAlign(TA_CENTER);
		dc->SetTextColor(RGB(0, 0, 0));
		dc->SelectObject(&m_FontText);
		dc->TextOut(vecrtLaneName[nLaneIdx].CenterPoint().x, vecrtLaneName[nLaneIdx].CenterPoint().y, strLaneName);
	}
	
	//if (isDebugMode)
	//{
	//	CString strLog;

	//	CString str;
	//	str.Format(_T("%.3fms, %.3fms, %.3f, %.3f, %.3f, %.3f"), f1, f2, f3, f4, f5, f6);
	//	strLog.Format(_T("ITEM : %s, Draw->Line : %.3fms, DrawLine : %.3fms, DrawData : %.3f, DarwXaxis : %.3f, DrawYaxis : %.3f, end : %.3f"), title, f1, f2, f3, f4, f5, f6);

	//	dc->SetTextColor(COLOR_TEXT);
	//	dc->SetTextAlign(TA_LEFT);
	//	dc->SelectObject(&m_FontYaxis);
	//	dc->TextOut(10, 22, str);
	//	//g_LogEtc.Log(0, strLog);
	//}
}

void AnimationCustomizeMismatch::UpdateViewSize(CDC* dc, long cx, long cy)
{
	AnimationCustomizeBase::UpdateViewSize(dc, cx, cy);
	
	ReadyToUpdateView();

	float fScale = 0.65f;
	
	Utility::SystemFont::CreateGUIFont_Bold(m_FontTitle, cy * fScale);
    Utility::SystemFont::CreateGUIFont(m_FontText, cy * fScale);
    Utility::SystemFont::CreateGUIFont(m_FontValueNormal, cy * fScale);
    Utility::SystemFont::CreateGUIFont(m_FontValueCLOver, cy * fScale);
    Utility::SystemFont::CreateGUIFont(m_FontValueSLOver, cy * fScale);
}

void AnimationCustomizeMismatch::DrawData(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const AnimationData& data)
{
	//long dataCount = data.GetPointCount();

	//for (long idx = 0; idx < data.GetGroupCount(); idx++)
	//{
	//	int nCnt = 0;
	//	m_nGraphYPos[idx] = 0;

	//	// ¼±¸¸ ±×¸°´Ù
	//	pDC->SelectObject(&m_Pen_Data[idx]);

	//	CPoint pt1;
	//	CPoint pt2;
	//	bool firstPoint = true;

	//	for (long dataIndex = 0; dataIndex < dataCount; dataIndex++)
	//	{
	//		const auto& pointValue = data.GetPoint(dataIndex).vecfValue[idx];

	//		if (pointValue == -9955.0f)
	//		{
	//			continue;
	//		}
	//		else if (pointValue > NODATA)
	//		{
	//			pt1 = pt2;
	//			pt2 = CPoint(
	//				(long)(fStepX * dataIndex) + rtGraph.left,
	//				(rtGraph.bottom) - (pointValue + fZeroOf) * fStepY);

	//			m_nGraphYPos[idx] += pt2.y;
	//			nCnt++;

	//			if (firstPoint)
	//			{
	//				firstPoint = false;
	//			}
	//			else
	//			{
	//				pDC->MoveTo(pt1);
	//				pDC->LineTo(pt2);

	//				CRgn rgn;
	//				rgn.CreateRectRgn(pt2.x - 1, pt2.y - 1, pt2.x + 1, pt2.y + 1);

	//				CBrush FillBrush(RGB(255, 100, 0));
	//				pDC->FillRgn(&rgn, &FillBrush);
	//			}
	//		}
	//	}

	//	if (0 < nCnt && 0 < m_nGraphYPos[idx])
	//		m_nGraphYPos[idx] = m_nGraphYPos[idx] / nCnt;
	//}
}

CString AnimationCustomizeMismatch::number2bank(int nData)
{
	CString str, strC(_T(""));
	str.Format(_T("%d"), nData);

	int nCom = 1;
	for (int i = str.GetLength(); i > 0; i--)
	{
		strC += str.GetAt(i - 1);
		if (0 == (nCom % 3) && 1 < i)
			strC += _T(",");
		nCom++;
	}

	CString strReverse(_T(""));
	for (int i = strC.GetLength(); i > 0; i--)
	{
		strReverse += strC.GetAt(i - 1);
	}

	return strReverse;
}

void AnimationCustomizeMismatch::ReadyToUpdateView()
{
	m_FontTitle.DeleteObject();
	m_FontText.DeleteObject();
	m_FontValueNormal.DeleteObject();
	m_FontValueCLOver.DeleteObject();
	m_FontValueSLOver.DeleteObject();
}

//void AnimationCustomizeMismatch::SetDataColor(short index, COLORREF rgb)
//{
//	m_colorFeeder[index] = rgb;
//
//	if (m_Pen_Data[index].m_hObject)
//	{
//		m_Pen_Data[index].DeleteObject();
//	}
//
//	m_Pen_Data[index].CreatePen(PS_SOLID, 1, m_colorFeeder[index]);
//}
