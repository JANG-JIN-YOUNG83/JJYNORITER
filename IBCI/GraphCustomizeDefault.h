#pragma once
#include "GraphCustomizeBase.h"

class GraphCustomizeDefault : public GraphCustomizeBase
{
public:
	GraphCustomizeDefault();
	virtual ~GraphCustomizeDefault();

	virtual void Draw(CDC* dc, const GraphData& data, bool isDebugMode) override;
	virtual void Draw(CDC* dc, const DefectPointXY& data) override;
	virtual void UpdateViewSize(CDC* dc, long cx, long cy) override;

private:
	COLORREF	COLOR_AXIS_BOARD;
	COLORREF	COLOR_BOARD;
	COLORREF	COLOR_BOARD_LINE;
	COLORREF	COLOR_GRADATION;	//´«±Ý
	COLORREF	COLOR_SPEC_MIN;
	COLORREF	COLOR_SPEC_MAX;
	COLORREF	COLOR_SPEC_C;
	COLORREF	COLOR_NOMINAL;
	COLORREF	COLOR_SPEC_MINAI;
	COLORREF	COLOR_SPEC_MAXAI;
	COLORREF	COLOR_SPEC_MINNG;
	COLORREF	COLOR_SPEC_MAXNG;
	COLORREF	COLOR_TEXT;
	COLORREF	m_colorFeeder[GRAPH_DATA_MAX_NUM];
	int			m_nGraphYPos[GRAPH_DATA_MAX_NUM];

	CBrush m_BrushGraph;
	CPen m_PenBoardLine;
	CPen m_PenXText;
	CFont m_FontXaxis;
	CFont m_font_title;
	CFont m_FontYaxis;
	CFont m_FontValue;
	CFont m_Font_Xaxis;
	CPen m_Pen_Data[GRAPH_DATA_MAX_NUM];
	CPen m_Pen_MaxAi;
	CPen m_Pen_MinAi;
	CPen m_PenMaxNg;
	CPen m_PenMinNg;
	CPen m_PenSpecMax;
	CPen m_PenSpec_C;
	CPen m_PenSpecMin;
	CPen m_PenNominal;

	void DrawData(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const GraphData& data);
	void DrawXaxis(CDC *pDC, CRect rtAxis, float fStepX, const GraphData& data);
	void DrawYaxis_SpecMinMax(CDC *pDC, CRect rtAxis, float fStepX, float fStepY, float fZeroOf, const GraphData& data);
	void DrawSpecLine(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const GraphData& data);
	CString number2bank(int nData);

	void ReadyToUpdateView();
	void SetDataColor(short index, COLORREF rgb);
};
