#pragma once
#include "basedef.h"

// YYS Custom

class GdiPlusAccessor;

class __CUSTOMCONTROL_CLASS__ CCustomButton : public CButton
{
	DECLARE_DYNAMIC(CCustomButton)

public:
	CCustomButton();
	virtual ~CCustomButton();

	void	SetEvent_Clicked(HWND target, UINT message);
	void	SetFont(CString fontName, COLORREF fontColor, int fontSize, int fontBold, int fontAlign);
	void	LoadImageFromResource(LPCTSTR resourceType, UINT resourceID);
	void	LoadImageFromResource(UINT resourceID);
	void	SetImage(LPCTSTR imageName);
	void	SetBackColor(COLORREF backColor);
	void	SetThick(int thickness, COLORREF thickColor);
	void	SetBackGradation(COLORREF startColor, COLORREF endColor, int direction);
	void	SetBackGradation(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction);
	void	SetBackPathGradation(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor);
	void	SetTransparentColor(COLORREF color);
	void	SetDraw3D(BOOL bDraw);
	BOOL	Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	int CCustomButton::GetCtrlIndex()
	{
		return m_iBtnIndex;
	}

	void CCustomButton::SetCtrlIndex(int index)
	{
		m_iBtnIndex = index;
	}

protected:
	GdiPlusAccessor* m_accessor;
	
	HWND m_eventClicked_target;
	UINT m_eventClicked_message;

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

	UINT	m_uiTextAlign[9];

	int		m_iBtnIndex;

	BOOL	m_bGradation;
	BOOL	m_bPathGradation;
	int		m_iThickness;
	char	m_chThickColorR;
	char	m_chThickColorG;
	char	m_chThickColorB;
	BOOL	m_bDraw3D;

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
	afx_msg void OnBnClicked();
};


