#include "pch.h"
#include "GraphCustomizeMismatch.h"
#include "GraphData.h"
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

GraphCustomizeMismatch::GraphCustomizeMismatch()
	: m_dCp(0.0)
	, m_dCpK(0.0)
{
	COLOR_AXIS_BOARD = RGB(170, 170, 170);
	COLOR_BOARD = RGB(0, 0, 0);
	COLOR_BOARD_LINE = RGB(0, 24, 200);
	COLOR_GRADATION = RGB(100, 100, 100);
	COLOR_SPEC_MIN = RGB(255, 0, 0);
	COLOR_SPEC_MAX = RGB(255, 0, 0);
	COLOR_SPEC_C = RGB(0, 0, 255);
	COLOR_SPEC_S = RGB(255, 0, 0);
	COLOR_TEXT = RGB(20, 20, 20);
	COLOR_NOMINAL = RGB(255, 255, 255);

	COLOR_SPEC_MINAI = RGB(120, 120, 0);
	COLOR_SPEC_MAXAI = RGB(120, 120, 0);

	COLOR_SPEC_MINNG = RGB(10, 120, 40);
	COLOR_SPEC_MAXNG = RGB(10, 120, 40);

	SetDataColor(0, RGB(255, 255, 0));
	SetDataColor(1, RGB(0, 255, 0));
	SetDataColor(2, RGB(100, 255, 255));
	SetDataColor(3, RGB(0, 0, 255));
	SetDataColor(4, RGB(170, 170, 170));
	SetDataColor(5, RGB(170, 170, 170));
	SetDataColor(6, RGB(170, 170, 170));
	SetDataColor(7, RGB(170, 170, 170));
	SetDataColor(8, RGB(170, 170, 170));
	SetDataColor(9, RGB(170, 170, 170));

	m_PenBoardLine.CreatePen(PS_SOLID, 1, COLOR_BOARD_LINE);
	m_PenXText.CreatePen(PS_SOLID, 1, COLOR_GRADATION);
	m_BrushGraph.CreateSolidBrush(COLOR_BOARD);
	m_Pen_MaxAi.CreatePen(PS_SOLID, 1, COLOR_SPEC_MAXAI);
	m_Pen_MinAi.CreatePen(PS_SOLID, 1, COLOR_SPEC_MINAI);
	m_PenMaxNg.CreatePen(PS_SOLID, 1, COLOR_SPEC_MAXNG);
	m_PenMinNg.CreatePen(PS_SOLID, 1, COLOR_SPEC_MINNG);
	m_PenSpecMax.CreatePen(PS_SOLID, 1, COLOR_SPEC_MAX);
	m_PenSpec_C.CreatePen(PS_SOLID, 1, COLOR_SPEC_C);
	m_PenSpec_S.CreatePen(PS_SOLID, 1, COLOR_SPEC_S);
	m_PenSpecMin.CreatePen(PS_SOLID, 1, COLOR_SPEC_MIN);
	m_PenNominal.CreatePen(PS_SOLID, 1, COLOR_NOMINAL);

	m_PenGrid.CreatePen(PS_DOT, 1, COLOR_GRADATION);
	m_PenInBox.CreatePen(PS_SOLID, 2, RGB(50, 250, 50));
}

GraphCustomizeMismatch::~GraphCustomizeMismatch()
{
	m_PenBoardLine.DeleteObject();
	m_PenXText.DeleteObject();
	m_BrushGraph.DeleteObject();
	m_Pen_MaxAi.DeleteObject();
	m_Pen_MinAi.DeleteObject();
	m_PenMaxNg.DeleteObject();
	m_PenMinNg.DeleteObject();
	m_PenSpecMax.DeleteObject();
	m_PenSpec_C.DeleteObject();
	m_PenSpec_S.DeleteObject();
	m_PenSpecMin.DeleteObject();
	m_PenNominal.DeleteObject();

	m_PenGrid.DeleteObject();
	m_PenInBox.DeleteObject();
}

void GraphCustomizeMismatch::DataSetZero()
{
	m_dCp = 0.0;
	m_dCpK = 0.0;
}

void GraphCustomizeMismatch::Draw(CDC* dc, const DefectPointXY& data)
{
}

void GraphCustomizeMismatch::Draw(CDC* dc, const GraphData& data, bool isDebugMode)
{
	IPVM::TimeCheck tc;

	CRect rt(0, 0, m_viewSizeX, m_viewSizeY);

	auto nWidth = m_viewSizeX;
	auto nHeight = m_viewSizeY;

	CRect rtTitle(0, 0, nWidth, nHeight * 0.1);
	CPoint cpAxisSize(nWidth * 0.110, nHeight * 0.13);
	CRect rtHorAxis(cpAxisSize.x, nHeight - cpAxisSize.y, nWidth, nHeight);
	CRect rtVerAxis(0, rtTitle.bottom, cpAxisSize.x, nHeight - cpAxisSize.y);
	CRect rtGraph(cpAxisSize.x, rtTitle.bottom, nWidth, nHeight - cpAxisSize.y);

	dc->FillSolidRect(rt, COLOR_AXIS_BOARD);
	dc->SelectObject(&m_BrushGraph);
	dc->Rectangle(rtGraph);

	float fmin = NOISE_VALUE_32F;
	float fmax = NOISE_VALUE_32F;

	/*if (NODATA < data.m_fSpecMin &&  NODATA < data.m_fSpecNgMin)
	{
		fmin = min(data.m_fSpecMin, data.m_fSpecNgMin);
	}
	else if (NODATA < data.m_fSpecMin)
	{*/
		fmin = data.m_fSpecMin;
	/*}
	else if (NODATA < data.m_fSpecNgMin)
	{
		fmin = data.m_fSpecNgMin;
	}*/

	/*if (NODATA < data.m_fSpecAiMin)
	{
		fmin = min(fmin, data.m_fSpecAiMin);
	}*/

	/*if (NODATA < data.m_fSpecMax &&  NODATA < data.m_fSpecNgMax)
	{
		fmax = max(data.m_fSpecMax, data.m_fSpecNgMax);
	}
	else if (NODATA < data.m_fSpecMax)
	{*/
		fmax = data.m_fSpecMax;
	/*}
	else if (NODATA < data.m_fSpecNgMax)
	{
		fmax = data.m_fSpecNgMax;
	}

	if (NODATA < data.m_fSpecAiMax)
	{
		fmax = max(fmax, data.m_fSpecAiMax);
	}*/

	float yAxisMin = data.m_fYaxisMin;
	float yAxisMax = data.m_fYaxisMax;
	long dataCount = data.GetPointCount();

	float fTemp = abs((fmax > fmin) ? fmax - fmin : fmin - fmax);
	float fHalfMajinY = fTemp * 0.3f;
	float displayRangeY = fTemp + fHalfMajinY * 2;

	float displayMinY = ((fmax > fmin) ? fmin : fmax) - fHalfMajinY;
	float fZeroOffsetY = displayMinY * -1;

	float fStepY = (float)rtGraph.Height() / displayRangeY;
	float fStepX = (float)rtGraph.Width() / (float)(data.GetMaxPointCount() - 1);

	float f1 = tc.Elapsed_ms();
	tc.Reset();

	DrawSpecLine(dc, rtGraph, fStepX, fStepY, fZeroOffsetY, data);
	float f2 = tc.Elapsed_ms();
	tc.Reset();

	DrawData(dc, rtGraph, fStepX, fStepY, fZeroOffsetY, data);
	float f3 = tc.Elapsed_ms();
	tc.Reset();

	DrawXaxis(dc, rtHorAxis, fStepX, fStepY, fZeroOffsetY, data);
	float f4 = tc.Elapsed_ms();
	tc.Reset();


	//if (g_systemParameter.systemOption.m_bgraphYGradation)
	//{
	//	DrawYaxis_Gradation(dc, rtVerAxis, fStepX, fStepY, fZeroOffsetY, data);
	//}
	//else
	//{
		DrawYaxis_SpecMinMax(dc, rtVerAxis, fStepX, fStepY, fZeroOffsetY, data);
	//}
	

	float f5 = tc.Elapsed_ms();
	tc.Reset();

	dc->FillSolidRect(rtTitle, RGB(240, 240, 240));
	dc->SetTextAlign(TA_LEFT);
	dc->SetTextColor(RGB(0, 0, 200));
	dc->SelectObject(&m_font_title);

	CString title = data.m_title;
	dc->TextOut(0, rtTitle.CenterPoint().y * 0.4, title);

	dc->SetTextColor(COLOR_TEXT);
	dc->SelectObject(&m_PenXText);
	dc->SetTextAlign(TA_LEFT);

	CString strCp, strCpK;
	strCp.Format(_T("Cp : %0.03f"), m_dCp);
	strCpK.Format(_T("CpK : %0.03f"), m_dCpK);

	dc->TextOut(nWidth * 0.45, rtTitle.CenterPoint().y * 0.4, strCp);
	dc->TextOut(nWidth * 0.72, rtTitle.CenterPoint().y * 0.4, strCpK);

	/**/dc->SelectObject(&m_PenBoardLine);

	float f6 = tc.Elapsed_ms();
	tc.Reset();

	if (isDebugMode)
	{
		CString strLog;

		CString str;
		str.Format(_T("%.3fms, %.3fms, %.3f, %.3f, %.3f, %.3f"), f1, f2, f3, f4, f5, f6);
		strLog.Format(_T("ITEM : %s, Draw->Line : %.3fms, DrawLine : %.3fms, DrawData : %.3f, DarwXaxis : %.3f, DrawYaxis : %.3f, end : %.3f"), title, f1, f2, f3, f4, f5, f6);

		dc->SetTextColor(COLOR_TEXT);
		dc->SetTextAlign(TA_LEFT);
		dc->SelectObject(&m_FontYaxis);
		dc->TextOut(10, 22, str);
		//g_LogEtc.Log(0, strLog);
	}
}

void GraphCustomizeMismatch::UpdateViewSize(CDC* dc, long cx, long cy)
{
	GraphCustomizeBase::UpdateViewSize(dc, cx, cy);
	
	ReadyToUpdateView();
	
	CRect rtTitle(0, 0, cx, cy * 0.13);
	Utility::SystemFont::CreateGUIFont_Bold(m_font_title, cx * 0.3);
	
	CPoint cpAxisSize(cx * 0.105, cy * 0.13);
	CRect rtHorAxis(cpAxisSize.x, cy - cpAxisSize.y, cx, cy);
	CRect rtVerAxis(0, rtTitle.bottom, cpAxisSize.x, cy - cpAxisSize.y);
	
	Utility::SystemFont::CreateGUIFont(m_FontValue, cx * 0.3);
	Utility::SystemFont::CreateGUIFont(m_FontXaxis, cx * 0.3);
	Utility::SystemFont::CreateGUIFont(m_FontYaxis, cx * 0.3);
}

void GraphCustomizeMismatch::DrawYaxis_Gradation(CDC* pDC, CRect rtAxis, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	//세로 축값 출력( spec min max )
	pDC->FillSolidRect(rtAxis, COLOR_AXIS_BOARD);
	pDC->SetBkColor(COLOR_AXIS_BOARD);

	if (!isinf(fStepY))
	{
		pDC->SetTextColor(COLOR_TEXT);
		pDC->SetTextAlign(TA_CENTER);

		pDC->SelectObject(&m_PenXText);

		long gradation10Y = ((int)(fZeroOf * -10.f));
		if (gradation10Y % 2) gradation10Y -= 1;	// 짝수단위로 시작

		float gradationY = gradation10Y / 10.f;
		float subStepY = 0.1f;
		long textStepY = 2;

		if (rtAxis.Height() / (subStepY * fStepY) < 10.f)
		{
			// Sub Step 10 이하 표시되면 0.02 단위로 표시하자
			subStepY = 0.05f;
			textStepY = 4;
		}

		for (long count = 0;; count++, gradationY += subStepY)
		{
			int displayY = rtAxis.bottom - fStepY * (gradationY + fZeroOf);

			if (displayY > rtAxis.bottom) continue;
			if (displayY < rtAxis.top) break;

			CPoint cpLeft(rtAxis.right - 6, displayY);
			CPoint cpRight(rtAxis.right - 2, displayY);

			if (count % textStepY == 0)
			{
				CString yValue;
				yValue.Format(_T("%.1f"), gradationY);
				pDC->SetTextColor(RGB(0, 0, 0));
				pDC->TextOut(rtAxis.CenterPoint().x, displayY - 5, yValue);

				cpLeft.x -= 2;
			}

			pDC->MoveTo(cpLeft);
			pDC->LineTo(cpRight);
		}
	}
}

void GraphCustomizeMismatch::DrawYaxis_SpecMinMax(CDC *pDC, CRect rtAxis, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	//¼¼·Î Ãà°ª Ãâ·Â( spec min max )
	pDC->FillSolidRect(rtAxis, COLOR_AXIS_BOARD);
	pDC->SetBkColor(COLOR_AXIS_BOARD);

	if (!isinf(fStepY))
	{
		pDC->SetTextColor(COLOR_TEXT);
		pDC->SetTextAlign(TA_CENTER);

		float specMin = data.m_fSpecMin;
		float specCenter = data.m_fSpecNominal;
		float specMax = data.m_fSpecMax;
		float specMin1 = (specCenter + specMin) / 2;
		float specMax1 = (specCenter + specMax) / 2;

		float specLCL = data.m_fSpecLCL;
		float specUCL = data.m_fSpecUCL;

		float specLSL = data.m_fSpecLSL;
        float specUSL = data.m_fSpecUSL;

		short nTextHalfHeight = 5;
		CString strSpecMax;
		strSpecMax.Format(_T("%.2f"), specMax);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specMax + fZeroOf) * fStepY - 5, strSpecMax);

		CString strSpecMin;
		strSpecMin.Format(_T("%.2f"), specMin);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specMin + fZeroOf) * fStepY - 5, strSpecMin);

		CString strSpecUCL;
		strSpecUCL.Format(_T("%.2f"), specUCL);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specUCL + fZeroOf) * fStepY - 5, strSpecUCL);

		CString strSpecLCL;
		strSpecLCL.Format(_T("%.2f"), specLCL);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specLCL + fZeroOf) * fStepY - 5, strSpecLCL);

		CString strSpecUSL;
        strSpecUSL.Format(_T("%.2f"), specUSL);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specUSL + fZeroOf) * fStepY - 5, strSpecUSL);

		CString strSpecLSL;
        strSpecLSL.Format(_T("%.2f"), specLSL);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specLSL + fZeroOf) * fStepY - 5, strSpecLSL);

		CString strSpecCenter;
		strSpecCenter.Format(_T("%.2f"), specCenter);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specCenter + fZeroOf) * fStepY - 5, strSpecCenter);

		/*CString strSpecMin1;
		strSpecMin1.Format(_T("%.2f"), specMin1);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specMin1 + fZeroOf) * fStepY - 5, strSpecMin1);

		CString strSpecMax1;
		strSpecMax1.Format(_T("%.2f"), specMax1);
		pDC->SetTextColor(COLOR_TEXT);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specMax1 + fZeroOf) * fStepY - 5, strSpecMax1);*/

		pDC->SelectObject(&m_PenXText);
		int nPos = 1;
		while (1)
		{
			nPos = nPos + 2;
			int nY = rtAxis.top + (fStepY * nPos);

			if (nY >= rtAxis.bottom) break;

			CPoint cpLeft(rtAxis.right - 6, nY);
			CPoint cpRight(rtAxis.right - 2, nY);
			pDC->MoveTo(cpLeft);
			pDC->LineTo(cpRight);
		}
	}
}

void GraphCustomizeMismatch::DrawXaxis(CDC *pDC, CRect rtAxis, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	pDC->SetTextColor(COLOR_TEXT);
	pDC->SelectObject(&m_PenXText);
	pDC->SetTextAlign(TA_CENTER);

	int nGridStep = (rtAxis.left + rtAxis.right) / 6;

	long specX = data.m_SpecX;

	CString strSpecX;
    strSpecX.Format(_T("%d"), specX);

	CPoint pt1 = CPoint(rtAxis.left, rtAxis.top + 7);
    pDC->TextOut(pt1.x, pt1.y - 5, strSpecX);

	strSpecX.Format(_T("%d"), (specX/5)*4);
    CPoint pt2 = CPoint(rtAxis.left + nGridStep, rtAxis.top + 7);
    pDC->TextOut(pt2.x, pt2.y - 5, strSpecX);

	strSpecX.Format(_T("%d"), (specX / 5) * 3);
    CPoint pt3 = CPoint(rtAxis.left + (nGridStep * 2), rtAxis.top + 7);
    pDC->TextOut(pt3.x, pt3.y - 5, strSpecX);

	strSpecX.Format(_T("%d"), (specX / 5) * 2);
    CPoint pt4 = CPoint(rtAxis.left + (nGridStep * 3), rtAxis.top + 7);
    pDC->TextOut(pt4.x, pt4.y - 5, strSpecX);

	strSpecX.Format(_T("%d"), (specX / 5) * 1);
    CPoint pt5 = CPoint(rtAxis.left + (nGridStep * 4), rtAxis.top + 7);
    pDC->TextOut(pt5.x, pt5.y - 5, strSpecX);

	/*CPoint pt1 = CPoint(rtAxis.left, rtAxis.top + 7);
	pDC->TextOut(pt1.x, pt1.y - 5, _T("150"));

	CPoint pt2 = CPoint(rtAxis.left + nGridStep, rtAxis.top + 7);
	pDC->TextOut(pt2.x, pt2.y - 5, _T("120"));

	CPoint pt3 = CPoint(rtAxis.left + (nGridStep * 2), rtAxis.top + 7);
	pDC->TextOut(pt3.x, pt3.y - 5, _T("90"));

	CPoint pt4 = CPoint(rtAxis.left + (nGridStep * 3), rtAxis.top + 7);
	pDC->TextOut(pt4.x, pt4.y - 5, _T("60"));

	CPoint pt5 = CPoint(rtAxis.left + (nGridStep * 4), rtAxis.top + 7);
	pDC->TextOut(pt5.x, pt5.y - 5, _T("30"));*/

	// Cp, CpK °è»ê.

	long dataCount = data.GetPointCount();
	if (dataCount < 1)
		return;

	double dStd = 0.0;
	double dSum = 0.0;

	for (int nDataIdx = 0; nDataIdx < dataCount; nDataIdx++)
	{
		if (data.GetPoint(nDataIdx).vecfValue[0] < -9900)
			return;

		dSum += data.GetPoint(nDataIdx).vecfValue[0];
	}

	double dVari = 0;

	dSum /= dataCount;//Æò±Õ

	for (int nDataIdx = 0; nDataIdx < dataCount; nDataIdx++)
	{
		dVari += pow(data.GetPoint(nDataIdx).vecfValue[0] - dSum, 2);//ÆíÂ÷ÀÇ ÇÕ
	}

	dVari /= (dataCount - 1);//ÆíÂ÷ Æò±Õ

	dStd = sqrt(dVari);

	//»óÇÑ-ÇÏÇÑ
	double dLimitRange = data.m_fSpecMax - data.m_fSpecMin;//SpecÀÇ »óÇÑ ÇÏÇÑÄ¡

	//CP = 6À» °öÇÑ´Ù
	double dCPValue = dLimitRange / (6 * (dStd + 0.000001));//CP

	m_dCp = dCPValue;

	//CPK
	// MIN( (»óÇÑ-Æò±Õ)/3*std.Dev , (Æò±Õ-ÇÏÇÑ)/3*std.Dev) )
	double dLimitGap_H_B = data.m_fSpecMax - dSum;
	double dLimitGap_B_L = dSum - data.m_fSpecMin;

	double dCPK = 0;
	if ((dLimitGap_H_B / (3 * (dStd + 0.000001))) < (dLimitGap_B_L / (3 * (dStd + 0.000001))))
	{
		dCPK = dLimitGap_H_B / (3 * (dStd + 0.000001));
	}
	else
	{
		dCPK = dLimitGap_B_L / (3 * (dStd + 0.000001));
	}

	m_dCpK = dCPK;

	//g_dMismatchCpK = m_dCpK;

}

void GraphCustomizeMismatch::DrawData(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	long dataCount = data.GetPointCount();

	for (long idx = 0; idx < data.GetGroupCount(); idx++)
	{
		int nCnt = 0;
		m_nGraphYPos[idx] = 0;

		// ¼±¸¸ ±×¸°´Ù
		pDC->SelectObject(&m_Pen_Data[idx]);

		CPoint pt1;
		CPoint pt2;
		bool firstPoint = true;

		for (long dataIndex = 0; dataIndex < dataCount; dataIndex++)
		{
			const auto& pointValue = data.GetPoint(dataIndex).vecfValue[idx];

			if (pointValue == -9955.0f)
			{
				continue;
			}
			else if (pointValue > NODATA)
			{
				pt1 = pt2;
				pt2 = CPoint(
					(long)(fStepX * dataIndex) + rtGraph.left,
					(rtGraph.bottom) - (pointValue + fZeroOf) * fStepY);

				m_nGraphYPos[idx] += pt2.y;
				nCnt++;

				if (firstPoint)
				{
					firstPoint = false;
				}
				else
				{
					pDC->MoveTo(pt1);
					pDC->LineTo(pt2);

					CRgn rgn;
					rgn.CreateRectRgn(pt2.x - 1, pt2.y - 1, pt2.x + 1, pt2.y + 1);

					CBrush FillBrush(RGB(255, 100, 0));
					pDC->FillRgn(&rgn, &FillBrush);
				}
			}
		}

		if (0 < nCnt && 0 < m_nGraphYPos[idx])
			m_nGraphYPos[idx] = m_nGraphYPos[idx] / nCnt;
	}
}

void GraphCustomizeMismatch::DrawSpecLine(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	BOOL bUseNominal = FALSE;



	
	if (fabs(data.m_fSpecNominal) > 0.001)
	{
		pDC->SelectObject(&m_PenNominal);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);

		bUseNominal = TRUE;
	}
	else if (fabs(data.m_fSpecNominal) < 0.001 && data.m_fSpecMin < -0.001)
	{
		pDC->SelectObject(&m_PenNominal);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);

		bUseNominal = TRUE;
	}

	if (bUseNominal == TRUE)
	{
		pDC->SelectObject(&m_PenGrid);

		int nPosY = (((rtGraph.Height() - (data.m_fSpecMax + fZeroOf) * fStepY) + rtGraph.top) + ((rtGraph.Height() - (data.m_fSpecNominal + fZeroOf) * fStepY) + rtGraph.top)) / 2;

		CPoint pt1 = CPoint(rtGraph.left, nPosY);
		CPoint pt2 = CPoint(rtGraph.right, nPosY);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);

		nPosY = (((rtGraph.Height() - (data.m_fSpecMin + fZeroOf) * fStepY) + rtGraph.top) + ((rtGraph.Height() - (data.m_fSpecNominal + fZeroOf) * fStepY) + rtGraph.top)) / 2;

		pt1 = CPoint(rtGraph.left, nPosY);
		pt2 = CPoint(rtGraph.right, nPosY);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}
	else
	{
		pDC->SelectObject(&m_PenGrid);

		int nSpecMax = (rtGraph.bottom - (data.m_fSpecMax + fZeroOf) * fStepY);
		int nSpecMin = (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY);
		int nPosYCenter = (((rtGraph.Height() - (data.m_fSpecMax + fZeroOf) * fStepY) + rtGraph.top) + ((rtGraph.Height() - (data.m_fSpecMin + fZeroOf) * fStepY) + rtGraph.top)) / 2;

		CPoint pt1 = CPoint(rtGraph.left, nPosYCenter);
		CPoint pt2 = CPoint(rtGraph.right, nPosYCenter);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);

		int nPosY = (nPosYCenter + nSpecMax) / 2;

		pt1 = CPoint(rtGraph.left, nPosY);
		pt2 = CPoint(rtGraph.right, nPosY);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);

		nPosY = (nPosYCenter + nSpecMin) / 2;

		pt1 = CPoint(rtGraph.left, nPosY);
		pt2 = CPoint(rtGraph.right, nPosY);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);


        pt1 = CPoint(rtGraph.left, nPosYCenter + nSpecMax);
        pt2 = CPoint(rtGraph.right, nPosYCenter + nSpecMax);
        pDC->MoveTo(pt1);
        pDC->LineTo(pt2);

        pt1 = CPoint(rtGraph.left, nPosYCenter + nSpecMin);
        pt2 = CPoint(rtGraph.right, nPosYCenter + nSpecMin);
        pDC->MoveTo(pt1);
        pDC->LineTo(pt2);



	}

	if (NODATA < data.m_fSpecMax)
    {
        pDC->SelectObject(&m_PenGrid);
        CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecMax + fZeroOf) * fStepY));
        CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecMax + fZeroOf) * fStepY));
        pDC->MoveTo(pt1);
        pDC->LineTo(pt2);
    }

    if (NODATA < data.m_fSpecMin)
    {
        pDC->SelectObject(&m_PenGrid);
        CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecMin + fZeroOf) * fStepY));
        CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecMin + fZeroOf) * fStepY));
        pDC->MoveTo(pt1);
        pDC->LineTo(pt2);
    }


	if (NODATA < data.m_fSpecUCL)
	{
		pDC->SelectObject(&m_PenSpec_C);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecUCL + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecUCL + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (NODATA < data.m_fSpecLCL)
	{
		pDC->SelectObject(&m_PenSpec_C);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecLCL + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecLCL + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}


	if (NODATA < data.m_fSpecUSL)
	{
		pDC->SelectObject(&m_PenSpec_S);
        CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecUSL + fZeroOf) * fStepY));
        CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecUSL + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (NODATA < data.m_fSpecLSL)
	{
		pDC->SelectObject(&m_PenSpec_S);
        CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecLSL + fZeroOf) * fStepY));
        CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecLSL + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	pDC->SelectObject(&m_PenGrid);

	int nGridStep = (rtGraph.left + rtGraph.right) / 6;

	CPoint pt1 = CPoint(rtGraph.left + nGridStep, rtGraph.top);
	CPoint pt2 = CPoint(rtGraph.left + nGridStep, rtGraph.bottom);
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left + (nGridStep * 2), rtGraph.top);
	pt2 = CPoint(rtGraph.left + (nGridStep * 2), rtGraph.bottom);
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left + (nGridStep * 3), rtGraph.top);
	pt2 = CPoint(rtGraph.left + (nGridStep * 3), rtGraph.bottom);
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left + (nGridStep * 4), rtGraph.top);
	pt2 = CPoint(rtGraph.left + (nGridStep * 4), rtGraph.bottom);
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pDC->SelectObject(&m_PenInBox);
	pDC->MoveTo(rtGraph.left, rtGraph.top + 1);
	pDC->LineTo(rtGraph.right - 1, rtGraph.top + 1);
	pDC->LineTo(rtGraph.right - 1, rtGraph.bottom - 1);
	pDC->LineTo(rtGraph.left + 1, rtGraph.bottom - 1);
	pDC->LineTo(rtGraph.left + 1, rtGraph.top + 1);
}

CString GraphCustomizeMismatch::number2bank(int nData)
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

void GraphCustomizeMismatch::ReadyToUpdateView()
{
	m_FontXaxis.DeleteObject();
	m_font_title.DeleteObject();
	m_Font_Xaxis.DeleteObject();
	m_FontYaxis.DeleteObject();
	m_FontValue.DeleteObject();
}

void GraphCustomizeMismatch::SetDataColor(short index, COLORREF rgb)
{
	m_colorFeeder[index] = rgb;

	if (m_Pen_Data[index].m_hObject)
	{
		m_Pen_Data[index].DeleteObject();
	}

	m_Pen_Data[index].CreatePen(PS_SOLID, 1, m_colorFeeder[index]);
}
