#pragma once
#include "basedef.h"

class GdiPlusAccessor;

#define DEFAULT_STYLE	0x01
#define PUSHLIKE_STYLE	0x02

// YYS Custom

class __CUSTOMCONTROL_CLASS__ CCustomCheckBox : public CButton
{
	DECLARE_DYNAMIC(CCustomCheckBox)

public:
	CCustomCheckBox();
	virtual ~CCustomCheckBox();
	void	SetFont(CString fontName, COLORREF fontColor, int fontSize, int fontBold, int fontAlign);
	void	SetImage(CString imageName);
	void	SetBackColor(COLORREF backColor);
	void	SetThick(int thickness, COLORREF thickColor);
	void	SetBackGradation(COLORREF startColor, COLORREF endColor, int direction);
	void	SetBackGradation(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction);
	void	SetBackPathGradation(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor);
	void	SetCenterMultiLine();
	void	SetTransparentColor(COLORREF color);
	void	SetStyle(UINT style);
	BOOL	GetCheck();
	void	SetCheck(BOOL check);

	int CCustomCheckBox::GetCtrlIndex()
	{
		return m_iChkIndex;
	}

	void CCustomCheckBox::SetCtrlIndex(int index)
	{
		m_iChkIndex = index;
	}

protected:
	GdiPlusAccessor* m_accessor;

	CString m_szImageName;
	CString m_szFontName;

	COLORREF	m_sFontColor;
	COLORREF	m_sBackColor;
	COLORREF	m_sTransparentColor;

	int		m_iFontAlign;
	int		m_iFontBold[10];

	CFont	m_cFont;

	BOOL	m_bImage;
	BOOL	m_bClick;
	BOOL	m_bMouseOver;
	BOOL	m_bCheck;
	BOOL	m_bCenterMultiLine;
	CRect	m_CenterRect;
	BOOL	m_bGetRect;

	UINT	m_uiTextAlign[9];
	UINT	m_checkStyle;

	int		m_iChkIndex;

	BOOL	m_bGradation;
	BOOL	m_bPathGradation;
	int		m_iThickness;
	char	m_chThickColorR;
	char	m_chThickColorG;
	char	m_chThickColorB;

	void GetCenterRect(CString str, CDC& dc);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	afx_msg void OnDestroy();
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};



