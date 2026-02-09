#pragma once
#include "basedef.h"

#ifdef _XTP_INCLUDE_CONTROLS

#define XTP_PGN_CUSTOM_BUTTON_CLICK	(XTP_PGN_USER + 1)		// LPARAM : (CXTPPropertyGridItem *)lparam

class __CUSTOMCONTROL_CLASS__ XTPPropertyGridItemButton : public CXTPPropertyGridItem
{
public:
	XTPPropertyGridItemButton(LPCTSTR captionText, LPCTSTR buttonText);
	XTPPropertyGridItemButton(LPCTSTR buttonText);		// Full row button

	void SetButtonText(LPCTSTR buttonText);
	void GetButtonText(CString &buttonText);

protected:
	virtual void SetVisible(BOOL bVisible);
	BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual void OnIndexChanged();
	void CreateButton();

private:
	class InplaceControl : public CXTPButton
	{
	public:
		DECLARE_MESSAGE_MAP()
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnKillFocus(CWnd* pNewWnd);

	protected:
		XTPPropertyGridItemButton* m_pItem;
		void OnClicked();

		friend class XTPPropertyGridItemButton;
	};

	InplaceControl m_wndButton;
	CString m_strButtonText;
	BOOL m_bFullRowButton;

	friend class InplaceControl;
};

#endif
