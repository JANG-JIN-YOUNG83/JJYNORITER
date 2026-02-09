#pragma once
#include "GraphCustomizeBase.h"

class GraphCustomizeDEFECTMAP : public GraphCustomizeBase
{
public:
	GraphCustomizeDEFECTMAP();
	virtual ~GraphCustomizeDEFECTMAP();

	virtual void Draw(CDC* dc, const GraphData& data, bool isDebugMode) override;
	virtual void Draw(CDC* dc, const DefectPointXY& data) override;
	virtual void UpdateViewSize(CDC* dc, long cx, long cy) override;

private:
	COLORREF	COLOR_AXIS_BOARD;
	COLORREF	COLOR_GRADATION;
	COLORREF	COLOR_TEXT;
	COLORREF	COLOR_BOARD;

	CBrush		m_BrushDot;
	CBrush		m_BrushGraph;
	CPen		m_PenYText;
	CPen		m_PenNormal;

	CFont		m_FontYaxis;

	std::vector<DefectPointXY> m_vecPoint;

	void DrawSpecLine(CDC* pDC, CRect rtGraph);
	void DrawData(CDC* pDC, CRect rtGraph, const DefectPointXY& data);
	void DrawYaxis(CDC* pDC, CRect rtAxis);

	void ReadyToUpdateView();
};
