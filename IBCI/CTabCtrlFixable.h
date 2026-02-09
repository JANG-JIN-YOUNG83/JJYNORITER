#pragma once


// CTabCtrlFixable

class CTabCtrlFixable : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlFixable)

public:
	CTabCtrlFixable();
	virtual ~CTabCtrlFixable();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


