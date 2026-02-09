#include "pch.h"
#include "GraphCustomizeDefault.h"
#include "GraphData.h"
//#include "Common.h"
//#include "../InterfaceModules/HostData/SystemParameter.h"
#include "../Module/Utility/SystemFont.h"
#include <Gadget/TimeCheck.h>

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GraphCustomizeDefault::GraphCustomizeDefault()
{
	COLOR_AXIS_BOARD = RGB(170, 170, 170);
	COLOR_BOARD = RGB(0, 28, 88);
	COLOR_BOARD_LINE = RGB(0, 24, 200);
	COLOR_GRADATION = RGB(100, 100, 100);
	COLOR_SPEC_MIN = RGB(1, 137, 173);
	COLOR_SPEC_MAX = RGB(255, 0, 0);
	COLOR_SPEC_C = RGB(0, 0, 255);
	COLOR_TEXT = RGB(20, 20, 20);
	COLOR_NOMINAL = RGB(255, 255, 255);

	COLOR_SPEC_MINAI = RGB(120, 120, 0);
	COLOR_SPEC_MAXAI = RGB(120, 120, 0);

	COLOR_SPEC_MINNG = RGB(10, 120, 40);
	COLOR_SPEC_MAXNG = RGB(10, 120, 40);

	SetDataColor(0, RGB(255, 255, 100));
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
	m_PenSpecMin.CreatePen(PS_SOLID, 1, COLOR_SPEC_MIN);
	m_PenNominal.CreatePen(PS_SOLID, 1, COLOR_NOMINAL);
}

GraphCustomizeDefault::~GraphCustomizeDefault()
{
}

void GraphCustomizeDefault::Draw(CDC* dc, const DefectPointXY& data)
{
}

void GraphCustomizeDefault::Draw(CDC* dc, const GraphData& data, bool isDebugMode)
{
	IPVM::TimeCheck tc;

	CRect rt(0, 0, m_viewSizeX, m_viewSizeY);

	auto nWidth = m_viewSizeX;
	auto nHeight = m_viewSizeY;

	CRect rtTitle(0, 0, nWidth, nHeight * 0.13);
	CPoint cpAxisSize(nWidth * 0.110, nHeight * 0.13);
	CRect rtHorAxis(cpAxisSize.x, nHeight - cpAxisSize.y, nWidth, nHeight);
	CRect rtVerAxis(0, rtTitle.bottom, cpAxisSize.x, nHeight - cpAxisSize.y);
	CRect rtGraph(cpAxisSize.x, rtTitle.bottom, nWidth, nHeight - cpAxisSize.y);

	dc->FillSolidRect(rt, COLOR_AXIS_BOARD);
	dc->SelectObject(&m_BrushGraph);
	dc->RoundRect(rtGraph, CPoint(9, 9));

	float fmin = NOISE_VALUE_32F;
	float fmax = NOISE_VALUE_32F;

	if (NODATA < data.m_fSpecMin &&  NODATA < data.m_fSpecNgMin)
	{
		fmin = min(data.m_fSpecMin, data.m_fSpecNgMin);
	}
	else if (NODATA < data.m_fSpecMin)
	{
		fmin = data.m_fSpecMin;
	}
	else if (NODATA < data.m_fSpecNgMin)
	{
		fmin = data.m_fSpecNgMin;
	}

	if (NODATA < data.m_fSpecAiMin)
	{
		fmin = min(fmin, data.m_fSpecAiMin);
	}

	if (NODATA < data.m_fSpecMax &&  NODATA < data.m_fSpecNgMax)
	{
		fmax = max(data.m_fSpecMax, data.m_fSpecNgMax);
	}
	else if (NODATA < data.m_fSpecMax)
	{
		fmax = data.m_fSpecMax;
	}
	else if (NODATA < data.m_fSpecNgMax)
	{
		fmax = data.m_fSpecNgMax;
	}

	if (NODATA < data.m_fSpecAiMax)
	{
		fmax = max(fmax, data.m_fSpecAiMax);
	}

   /* fmin = fmin * 2;
    fmax = fmax * 2;*/

	float yAxisMin = data.m_fYaxisMin;
	float yAxisMax = data.m_fYaxisMax;
	long dataCount = data.GetPointCount();

	//들어오는 최소 최대 값을 정수로 만들자. 
	float fTemp = abs((fmax > fmin) ? fmax - fmin : fmin - fmax);
	float fHalfMajinY = fTemp * 0.3f;
	float displayRangeY = fTemp + fHalfMajinY * 2;

	float displayMinY = ((fmax > fmin) ? fmin : fmax) - fHalfMajinY;
	float fZeroOffsetY = displayMinY * -1;

	float fStepY = (float)rtGraph.Height() / displayRangeY;
	float fStepX = (float)rtGraph.Width() / (float)(data.GetMaxPointCount() - 1);

	float f1 = tc.Elapsed_ms();
	tc.Reset();

	long groupCount = data.GetGroupCount();

	DrawSpecLine(dc, rtGraph, fStepX, fStepY, fZeroOffsetY, data);
	float f2 = tc.Elapsed_ms();
	tc.Reset();

	DrawData(dc, rtGraph, fStepX, fStepY, fZeroOffsetY, data);
	float f3 = tc.Elapsed_ms();
	tc.Reset();

	DrawXaxis(dc, rtHorAxis, fStepX, data);
	float f4 = tc.Elapsed_ms();
	tc.Reset();

	DrawYaxis_SpecMinMax(dc, rtVerAxis, fStepX, fStepY, fZeroOffsetY, data);

	float f5 = tc.Elapsed_ms();
	tc.Reset();

	dc->FillSolidRect(rtTitle, RGB(240, 240, 240));
	dc->SetTextAlign(TA_LEFT);
	dc->SetTextColor(RGB(0, 0, 200));
	dc->SelectObject(&m_font_title);
	dc->TextOut(0, rtTitle.CenterPoint().y *0.4, data.m_title);

	if (0 < dataCount)
	{
		long lastIndex = dataCount - 1;

		const auto& point = data.GetPoint(lastIndex);

		CString strValue;

		for (long groupIndex = 0; groupIndex < groupCount; groupIndex++)
		{
			if (NODATA < point.vecfValue[groupIndex])
			{
				strValue.Format(_T("%.2fmm"), point.vecfValue[groupIndex]);
			}
		}

		if (!strValue.IsEmpty())
		{
			if (point.bJudge) //OK
				dc->SetTextColor(RGB(0, 200, 0));
			else
				dc->SetTextColor(RGB(200, 50, 50));

			dc->SelectObject(&m_FontValue);
			dc->SetTextAlign(TA_RIGHT);
			dc->TextOut(rtTitle.Width(), rtTitle.CenterPoint().y * 0.4, strValue);

		}
	}

	/**/dc->SelectObject(&m_PenBoardLine);

	float f6 = tc.Elapsed_ms();
	tc.Reset();

	if (isDebugMode)
	{
		//주인님모드이면 그래프에 시간 그려주자

		CString strLog;

		CString str;
		str.Format(_T("%.3fms, %.3fms, %.3f, %.3f, %.3f, %.3f"), f1, f2, f3, f4, f5, f6);
		strLog.Format(_T("ITEM : %s, Draw->Line : %.3fms, DrawLine : %.3fms, DrawData : %.3f, DarwXaxis : %.3f, DrawYaxis : %.3f, end : %.3f"), data.m_title, f1, f2, f3, f4, f5, f6);

		dc->SetTextColor(COLOR_TEXT);
		dc->SetTextAlign(TA_LEFT);
		dc->SelectObject(&m_FontYaxis);
		dc->TextOut(10, 22, str);
		//g_LogEtc.Log(0, strLog);
	}
}

void GraphCustomizeDefault::UpdateViewSize(CDC* dc, long cx, long cy)
{
	GraphCustomizeBase::UpdateViewSize(dc, cx, cy);

	ReadyToUpdateView();

	CRect rtTitle(0, 0, cx, cy * 0.13);
	Utility::SystemFont::CreateGUIFont_Bold(m_font_title, cx * 0.28);

	CPoint cpAxisSize(cx * 0.105, cy * 0.13);
	CRect rtHorAxis(cpAxisSize.x, cy - cpAxisSize.y, cx, cy);
	CRect rtVerAxis(0, rtTitle.bottom, cpAxisSize.x, cy - cpAxisSize.y);

	Utility::SystemFont::CreateGUIFont(m_FontValue, cx * 0.25);
	Utility::SystemFont::CreateGUIFont(m_FontXaxis, cx * 0.22);
	Utility::SystemFont::CreateGUIFont(m_FontYaxis, cx * 0.22);
}

//세로 텍스트

void GraphCustomizeDefault::DrawYaxis_SpecMinMax(CDC *pDC, CRect rtAxis, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	//세로 축값 출력( spec min max )
	pDC->FillSolidRect(rtAxis, COLOR_AXIS_BOARD);
	pDC->SetBkColor(COLOR_AXIS_BOARD);

	if (!isinf(fStepY))
	{
		pDC->SetTextColor(COLOR_TEXT);
		pDC->SetTextAlign(TA_CENTER);

		float specMin = data.m_fSpecMin;
		float specMax = data.m_fSpecMax;

		short nTextHalfHeight = 5;
		CString strSpecMax;
		strSpecMax.Format(_T("%.2f"), specMax);
		pDC->SetTextColor(COLOR_SPEC_MAX);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specMax + fZeroOf) * fStepY - 5, strSpecMax);

		CString strSpecMin;
		strSpecMin.Format(_T("%.2f"), specMin);
		pDC->SetTextColor(COLOR_SPEC_MIN);
		pDC->TextOut(rtAxis.CenterPoint().x, (rtAxis.Height() + rtAxis.top) - (specMin + fZeroOf) * fStepY - 5, strSpecMin);

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

	pDC->SelectObject(&m_FontYaxis);

	for (long groupIndex = 0; groupIndex < data.GetGroupCount(); groupIndex++)
	{
		pDC->SetTextColor(m_colorFeeder[groupIndex]);
		//if (g_systemParameter.systemOption.m_graphDisplayByFeeder)
		//	pDC->TextOut(rtAxis.CenterPoint().x * 0.5, m_nGraphYPos[groupIndex], CString((char)('A' + groupIndex), 1));
	}
}

void GraphCustomizeDefault::DrawXaxis(CDC *pDC, CRect rtAxis, float fStepX, const GraphData& data)
{
	pDC->SetTextColor(COLOR_TEXT);
	pDC->SelectObject(&m_Font_Xaxis);
	pDC->SelectObject(&m_PenXText);
	pDC->SetTextAlign(TA_CENTER);

	long dataCount = data.GetPointCount();

	for (long dataIndex = 0; dataIndex < dataCount; dataIndex++)
	{
		auto index = data.GetPoint(dataIndex).nIndex;
		if (0 <= index)
		{
			int nDiv = 20;

			if (999 < index)
			{
				nDiv = 50;
			}

			nDiv = min(nDiv, data.GetMaxPointCount());

			int nLength = 3;
			if (0 == (index % nDiv))
			{
				nLength = 6;

				CPoint pt3 = CPoint((long)(fStepX * dataIndex) + rtAxis.left, rtAxis.top + 2 + nLength + 2);
				pDC->TextOut(pt3.x, pt3.y - 5, number2bank(index));
			}

			CPoint pt1 = CPoint((long)(fStepX * dataIndex) + rtAxis.left, rtAxis.top + 2);
			CPoint pt2 = CPoint((long)(fStepX * dataIndex) + rtAxis.left, rtAxis.top + 2 + nLength);

			pDC->MoveTo(pt1);
			pDC->LineTo(pt2);
		}
	}
}

void GraphCustomizeDefault::DrawData(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	long dataCount = data.GetPointCount();

	for (long idx = 0; idx < data.GetGroupCount(); idx++)
	{
		int nCnt = 0;
		m_nGraphYPos[idx] = 0;

		// 선만 그린다
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
				}
			}
		}

		if (0 < nCnt && 0 < m_nGraphYPos[idx])
			m_nGraphYPos[idx] = m_nGraphYPos[idx] / nCnt;
	}
}

void GraphCustomizeDefault::DrawSpecLine(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const GraphData& data)
{
	//spec 라인출력

	//AI
	if (NODATA < data.m_fSpecAiMax)
	{
		pDC->SelectObject(&m_Pen_MaxAi);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecAiMax + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecAiMax + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (NODATA < data.m_fSpecAiMin)
	{
		pDC->SelectObject(&m_Pen_MinAi);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecAiMin + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecAiMin + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (NODATA < data.m_fSpecNgMax)
	{
		pDC->SelectObject(&m_PenMaxNg);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecNgMax + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecNgMax + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (NODATA < data.m_fSpecNgMin)
	{
		pDC->SelectObject(&m_PenMinNg);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecNgMin + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecNgMin + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (NODATA < data.m_fSpecMax)
	{
		pDC->SelectObject(&m_PenSpecMax);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecMax + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecMax + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (NODATA < data.m_fSpecMin)
	{
		pDC->SelectObject(&m_PenSpecMin);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecMin + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecMin + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}

	if (fabs(data.m_fSpecNominal) > 0.001)
	{
		pDC->SelectObject(&m_PenNominal);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}
	else if (fabs(data.m_fSpecNominal) < 0.001 && data.m_fSpecMin < -0.001)
	{
		pDC->SelectObject(&m_PenNominal);
		CPoint pt1 = CPoint(rtGraph.left, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		CPoint pt2 = CPoint(rtGraph.right, (rtGraph.bottom - (data.m_fSpecNominal + fZeroOf) * fStepY));
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}
}

CString GraphCustomizeDefault::number2bank(int nData)
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

void GraphCustomizeDefault::ReadyToUpdateView()
{
	m_FontXaxis.DeleteObject();
	m_font_title.DeleteObject();
	m_Font_Xaxis.DeleteObject();
	m_FontYaxis.DeleteObject();
	m_FontValue.DeleteObject();
}

void GraphCustomizeDefault::SetDataColor(short index, COLORREF rgb)
{
	m_colorFeeder[index] = rgb;

	if (m_Pen_Data[index].m_hObject)
	{
		m_Pen_Data[index].DeleteObject();
	}

	m_Pen_Data[index].CreatePen(PS_SOLID, 1, m_colorFeeder[index]);
}
