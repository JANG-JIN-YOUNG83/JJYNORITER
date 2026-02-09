#pragma once
#include "basedef.h"

#include "CustomButton.h"

// YYS Custom

#define WM_RADIO_CHECK_STATE_UPDATE WM_USER +1000

class __CUSTOMCONTROL_CLASS__ CCustomRadioButton : public CCustomButton
{
	DECLARE_DYNAMIC(CCustomRadioButton)

public:
	CCustomRadioButton();
	virtual ~CCustomRadioButton();

	BOOL GetCheck();
	void SetCheck(BOOL check);

protected:
	BOOL m_bCheck;
	int	 m_iGroupStartID;
	int	 m_iGroupEndID;

	afx_msg LRESULT OnRadioStateUpdate(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void PreSubclassWindow();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


