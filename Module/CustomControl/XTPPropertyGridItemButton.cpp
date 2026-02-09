#include "pch.h"
#include "XTPPropertyGridItemButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _XTP_INCLUDE_CONTROLS

BEGIN_MESSAGE_MAP(XTPPropertyGridItemButton::InplaceControl, CXTPButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void XTPPropertyGridItemButton::InplaceControl::OnClicked()
{
	if (m_pItem == nullptr)
	{
		return;
	}

	if (m_pItem->m_pGrid == nullptr)
	{
		return;
	}

	m_pItem->m_pGrid->SendNotifyMessage(XTP_PGN_CUSTOM_BUTTON_CLICK, LPARAM(m_pItem));
}

void XTPPropertyGridItemButton::InplaceControl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);
}

void XTPPropertyGridItemButton::InplaceControl::OnKillFocus(CWnd* pOldWnd)
{
	SetButtonStyle(GetButtonStyle() & (~BS_DEFPUSHBUTTON), FALSE);

	__super::OnKillFocus(pOldWnd);
}


XTPPropertyGridItemButton::XTPPropertyGridItemButton(LPCTSTR captionText, LPCTSTR buttonText)
	: CXTPPropertyGridItem(captionText)
{
	m_wndButton.m_pItem = this;
	m_nFlags = 0;
	m_strButtonText = buttonText;
	m_bFullRowButton = FALSE;
}

XTPPropertyGridItemButton::XTPPropertyGridItemButton(LPCTSTR buttonText)
	: CXTPPropertyGridItem(_T(""))
{
	m_wndButton.m_pItem = this;
	m_nFlags = 0;
	m_strButtonText = buttonText;
	m_bFullRowButton = TRUE;
}

void XTPPropertyGridItemButton::SetButtonText(LPCTSTR buttonText)
{
	m_strButtonText = buttonText;

	if (m_bFullRowButton)
	{
		SetCaption(buttonText);
	}

	if (m_wndButton.GetSafeHwnd())
	{
		m_wndButton.SetWindowText(buttonText);
	}
}

void XTPPropertyGridItemButton::GetButtonText(CString &buttonText)
{
	buttonText = m_strButtonText;
}

void XTPPropertyGridItemButton::CreateButton()
{
	if (IsVisible())
	{	
		CRect rc;

		if (m_bFullRowButton)
		{		
			rc = GetItemRect();
			rc.DeflateRect( m_nIndent * 14, 0, 0, 1);
		}
		else
		{
			rc = GetValueRect();
		}

		if (!m_wndButton.m_hWnd)
		{
			static long controlIndex = 8;	// https://docs.microsoft.com/ko-kr/cpp/mfc/tn020-id-naming-and-numbering-conventions?view=vs-2019

			m_wndButton.Create(m_strButtonText, WS_CHILD | BS_FLAT | WS_VISIBLE, rc, (CWnd*)m_pGrid, controlIndex++);
			m_wndButton.SetFont(m_pGrid->GetFont());
		}

		m_wndButton.MoveWindow(rc, FALSE);
		m_wndButton.Invalidate(FALSE);
	}
	else
	{
		m_wndButton.DestroyWindow();
	}
}


void XTPPropertyGridItemButton::SetVisible(BOOL bVisible)
{
	CXTPPropertyGridItem::SetVisible(bVisible);
	CreateButton();
}

void XTPPropertyGridItemButton::OnIndexChanged()
{
	CreateButton();	
}

BOOL XTPPropertyGridItemButton::OnDrawItemValue(CDC& /*dc*/, CRect /*rcValue*/)
{
	CreateButton();
	return FALSE;
}

#endif

