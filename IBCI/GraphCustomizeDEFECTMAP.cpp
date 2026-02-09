#include "pch.h"
#include "GraphCustomizeDEFECTMAP.h"
//#include "Common.h"
//#include "Log.h"

//#include "../InterfaceModules/HostData/SystemParameter.h"
#include "../Module/Utility/SystemFont.h"

#include "Gadget/TimeCheck.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GraphCustomizeDEFECTMAP::GraphCustomizeDEFECTMAP()
{
	COLOR_AXIS_BOARD = RGB(170, 170, 170);	// 축 색 (길이 넣을 공간)
	COLOR_GRADATION = RGB(100, 100, 100);	// 점선 색
	COLOR_TEXT = RGB(20, 20, 20);			// 글자 색
	COLOR_BOARD = RGB(0, 0, 0);				// 보드 빽그라운드 색

	m_PenYText.CreatePen(PS_SOLID, 1, COLOR_GRADATION);
	m_PenNormal.CreatePen(PS_SOLID, 1, COLOR_TEXT);
	m_BrushGraph.CreateSolidBrush(COLOR_BOARD);
}

GraphCustomizeDEFECTMAP::~GraphCustomizeDEFECTMAP()
{
	m_PenYText.DeleteObject();
	m_PenNormal.DeleteObject();
	m_BrushGraph.DeleteObject();
}

void GraphCustomizeDEFECTMAP::UpdateViewSize(CDC* dc, long cx, long cy)
{
	GraphCustomizeBase::UpdateViewSize(dc, cx, cy);

	ReadyToUpdateView();

	Utility::SystemFont::CreateGUIFont(m_FontYaxis, cx * 0.2);
}

void GraphCustomizeDEFECTMAP::Draw(CDC* dc, const DefectPointXY& data)
{
	// Data에 초기값인 -999를 넣어준다는건, 초기화 하고 싶단 것
	if (data.nX == -999 && data.nY == -999)
	{
		m_vecPoint.clear();
	}
	else
	{
		m_vecPoint.push_back(data);
	}

	IPVM::TimeCheck tc;

	CRect rt(0, 0, m_viewSizeX, m_viewSizeY);

	auto nWidth = m_viewSizeX;
	auto nHeight = m_viewSizeY;

	CPoint cpAxisSize(nWidth * 0.12, 0);

	CRect rtAxis(0, 0, cpAxisSize.x, nHeight);
	CRect rtGraph(cpAxisSize.x, 0, nWidth, nHeight);

	if (nWidth != 0 && nHeight != 0)
    {
        dc->FillSolidRect(rt, COLOR_AXIS_BOARD);
        dc->SelectObject(&m_BrushGraph);
        dc->Rectangle(rtGraph);

        // 그래프 내의 선 그리기 함수
        DrawSpecLine(dc, rtGraph);
        float f0 = tc.Elapsed_ms();
        tc.Reset();

        // 그래프 내에 받아온 DATA에 따라 점 찍는 함수
        for (long i = 0; i < m_vecPoint.size(); i++)
        {
            auto data = m_vecPoint[i];

            DrawData(dc, rtGraph, data);
        }
        float f1 = tc.Elapsed_ms();
        tc.Reset();

        // 길이 값 넣는 공간에 대한 함수
        DrawYaxis(dc, rtAxis);
        float f2 = tc.Elapsed_ms();
        tc.Reset();
	}
	
}

void GraphCustomizeDEFECTMAP::DrawSpecLine(CDC* pDC, CRect rtGraph)
{
	pDC->SelectObject(&m_PenYText);

	int nGridStep = (rtGraph.top + rtGraph.bottom) / 10;

	CPoint pt1 = CPoint(rtGraph.left, rtGraph.top + nGridStep);
	CPoint pt2 = CPoint(rtGraph.right, rtGraph.top + nGridStep);
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 2));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 2));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 3));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 3));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 4));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 4));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 5));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 5));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 6));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 6));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 7));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 7));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 8));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 8));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

	pt1 = CPoint(rtGraph.left, rtGraph.top + (nGridStep * 9));
	pt2 = CPoint(rtGraph.right, rtGraph.top + (nGridStep * 9));
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);
}

void GraphCustomizeDEFECTMAP::DrawYaxis(CDC* pDC, CRect rtAxis)
{
	pDC->SetTextColor(COLOR_TEXT);
	pDC->SelectObject(&m_PenYText);
	pDC->SetTextAlign(TA_CENTER);

	int nGridStep = (rtAxis.top + rtAxis.bottom) / 10;

	CPoint pt1 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 1) - 7);
	pDC->TextOut(pt1.x, pt1.y, _T("250M"));

	CPoint pt2 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 2) - 7);
	pDC->TextOut(pt2.x, pt2.y, _T("500M"));

	CPoint pt3 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 3) - 7);
	pDC->TextOut(pt3.x, pt3.y, _T("750M"));

	CPoint pt4 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 4) - 7);
	pDC->TextOut(pt4.x, pt4.y, _T("1000M"));

	CPoint pt5 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 5) - 7);
	pDC->TextOut(pt5.x, pt5.y, _T("1250M"));

	CPoint pt6 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 6) - 7);
	pDC->TextOut(pt6.x, pt6.y, _T("1500M"));

	CPoint pt7 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 7) - 7);
	pDC->TextOut(pt7.x, pt7.y, _T("1750M"));

	CPoint pt8 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 8) - 7);
	pDC->TextOut(pt8.x, pt8.y, _T("2000M"));

	CPoint pt9 = CPoint(rtAxis.CenterPoint().x, rtAxis.top + (nGridStep * 9) - 7);
	pDC->TextOut(pt9.x, pt9.y, _T("2250M"));
}

void GraphCustomizeDEFECTMAP::DrawData(CDC *pDC, CRect rtGraph, const DefectPointXY& data)
{
	int nRadius = 2;
	CBrush* pOldBrush = NULL;

	if (m_BrushDot.CreateSolidBrush(data.Color))
	{
		pOldBrush = pDC->SelectObject(&m_BrushDot);
	}

	CPoint cpGraphStartPoint(rtGraph.left + 2, rtGraph.top + 2);

	CRect	rctArea;
	rctArea.left = (cpGraphStartPoint.x + data.nX) - nRadius;
	rctArea.top = (cpGraphStartPoint.y + data.nY) - nRadius;
	rctArea.right = (cpGraphStartPoint.x + data.nX) + nRadius;
	rctArea.bottom = (cpGraphStartPoint.y + data.nY) + nRadius;
	pDC->FillRect(&rctArea, &m_BrushDot);

	if (pOldBrush != NULL)
	{
		pDC->SelectObject(pOldBrush);
		m_BrushDot.DeleteObject();
	}

	//int nRadius = 2;
	//
	//CBrush* pOldBrush = NULL;
	//
	//for (long i = 0; i < m_vecPoint.size(); i++) 
	//{
	//	DefectPointXY data;
	//	data = m_vecPoint[i];
	//
	//	if (m_BrushDot.CreateSolidBrush(data.Color))
	//	{
	//		pOldBrush = pDC->SelectObject(&m_BrushDot);
	//	}
	//
	//	CPoint cpGraphStartPoint(rtGraph.left + 2, rtGraph.top + 2);
	//
	//	CRect	rctArea;
	//	rctArea.left = (cpGraphStartPoint.x + data.nX) - nRadius;
	//	rctArea.top = (cpGraphStartPoint.y + data.nY) - nRadius;
	//	rctArea.right = (cpGraphStartPoint.x + data.nX) + nRadius;
	//	rctArea.bottom = (cpGraphStartPoint.y + data.nY) + nRadius;
	//	pDC->FillRect(&rctArea, &m_BrushDot);
	//}
	//
	//if (pOldBrush != NULL)
	//{
	//	pDC->SelectObject(pOldBrush);
	//	m_BrushDot.DeleteObject();
	//}
}

void GraphCustomizeDEFECTMAP::ReadyToUpdateView()
{
	m_FontYaxis.DeleteObject();
}

void GraphCustomizeDEFECTMAP::Draw(CDC* dc, const GraphData& data, bool isDebugMode)
{

}