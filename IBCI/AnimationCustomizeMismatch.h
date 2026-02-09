#pragma once
#include "AnimationCustomizeBase.h"

class AnimationCustomizeMismatch : public AnimationCustomizeBase
{
public:
	AnimationCustomizeMismatch();
	virtual ~AnimationCustomizeMismatch();

	virtual void Draw(CDC* dc, const AnimationData& data, bool isDebugMode) override;
	virtual void Draw(CDC* dc, const DefectPointXY& data) override;
	virtual void UpdateViewSize(CDC* dc, long cx, long cy) override;
	COLORREF GetStrColor(ResultSpec temp, float fValue);
	float GetDrawValue(ResultSpec temp, float fValue, float fSize = 0.f);
private:
	COLORREF	COLOR_BOARD;				// White (255, 255, 255)
	COLORREF	COLOR_AREA_FOIL;			// Gray  (180, 180, 180)
	COLORREF	COLOR_AREA_ELECTRODE;		// Black (0, 0, 0)
	COLORREF	COLOR_TEXT;					// Black (0, 0, 0)
	COLORREF	COLOR_VALUE_NORMAL;			// Green (0, 255, 0)
	COLORREF	COLOR_VALUE_CL_OVER;		// Orange (255, 180, 0)
	COLORREF	COLOR_VALUE_SL_OVER;		// Red (255, 0, 0)

	CBrush		m_BrushAnimation;
	
	CPen		m_PenText;					// 일반 Text
	CPen		m_PenValueNormal;			// Value : Normal
	CPen		m_PenValueCLOver;			
	CPen		m_PenValueSLOVer;
	CPen		m_PenAreaFoil;
	CPen		m_PenAreaElectrode;

	CFont		m_FontTitle;
	CFont		m_FontText;
	CFont		m_FontValueNormal;
	CFont		m_FontValueCLOver;
	CFont		m_FontValueSLOver;

	
	//COLORREF	COLOR_AXIS_BOARD;
	//COLORREF	COLOR_BOARD;
	//COLORREF	COLOR_BOARD_LINE;
	//COLORREF	COLOR_GRADATION;	//눈금
	//COLORREF	COLOR_SPEC_MIN;
	//COLORREF	COLOR_SPEC_MAX;
	//COLORREF	COLOR_NOMINAL;
	//COLORREF	COLOR_SPEC_MINAI;
	//COLORREF	COLOR_SPEC_MAXAI;
	//COLORREF	COLOR_SPEC_MINNG;
	//COLORREF	COLOR_SPEC_MAXNG;
	//COLORREF	COLOR_TEXT;
	//COLORREF	m_colorFeeder[ANIMATION_DATA_MAX_NUM];
	
	//int		m_nGraphYPos[ANIMATION_DATA_MAX_NUM];

	//CBrush m_BrushGraph;
	//
	//CPen m_PenBoardLine;
	//CPen m_PenXText;
	//CFont m_FontXaxis;
	//
	//CFont m_font_title;
	//CFont m_FontYaxis;
	//CFont m_FontValue;
	//CFont m_Font_Xaxis;
	//
	//CPen m_Pen_Data[ANIMATION_DATA_MAX_NUM];
	//CPen m_Pen_MaxAi;
	//CPen m_Pen_MinAi;
	//CPen m_PenMaxNg;
	//CPen m_PenMinNg;
	//CPen m_PenSpecMax;
	//CPen m_PenSpecMin;
	//CPen m_PenNominal;
	//CPen m_PenGrid;
	//CPen m_PenInBox;

	void DrawData(CDC *pDC, CRect rtGraph, float fStepX, float fStepY, float fZeroOf, const AnimationData& data);
	CString number2bank(int nData);

	void ReadyToUpdateView();
	//void SetDataColor(short index, COLORREF rgb);
};
