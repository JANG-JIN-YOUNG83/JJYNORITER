#pragma once
#include "basedef.h"

class GdiPlusAccessor;

// YYS Custom

class __CUSTOMCONTROL_CLASS__ CCustomStatic : public CStatic
{
	DECLARE_DYNAMIC(CCustomStatic)

public:
	CCustomStatic();
	virtual ~CCustomStatic();

	void SetFont(CString fontName, COLORREF fontColor, int fontSize, int fontBold, int fontAlign);
	void LoadImageFromResource(LPCTSTR resourceType, UINT resourceID);
	void LoadImageFromResource(UINT resourceID);
	void SetImage(CString imageName);
	void SetBackColor(COLORREF backColor);
	void SetThick(int thickness, COLORREF thickColor);
	void SetBackGradation(COLORREF startColor, COLORREF endColor, int direction);
	void SetBackGradation(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction);
	void SetBackPathGradation(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor);
	void SetTransparent(BOOL transParent);
	void SetCenterMultiLine();
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	int CCustomStatic::GetCtrlIndex()
	{
		return m_iStcIndex;
	}
	void CCustomStatic::SetCtrlIndex(int index)
	{
		m_iStcIndex = index;
	}

protected:
	GdiPlusAccessor* m_accessor;

	CString m_szFontName;

	COLORREF	m_sFontColor;
	COLORREF	m_sBackColor;

	int		m_iFontAlign;
	int		m_iFontBold[10];

	CFont	m_cFont;

	BOOL	m_bImage;
	BOOL	m_bClick;
	BOOL	m_bGroupBox;
	BOOL	m_bCenterMultiLine;
	CRect	m_CenterRect;
	BOOL	m_bGetRect;

	UINT	m_uiTextAlign[9];

	int		m_iStcIndex;

	BOOL	m_bGradation;
	BOOL	m_bPathGradation;
	BOOL	m_bTransparent;
	int		m_iThickness;
	char	m_chThickColorR;
	char	m_chThickColorG;
	char	m_chThickColorB;

	void GetCenterRect(CString str, CDC& dc);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseLeave();
	CRect GetCtrlRect();
};


