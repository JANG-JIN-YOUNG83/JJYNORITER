#include "pch.h"
#include "CustomRadioButton.h"
#include "GdiPlusAccessor.h"

#include <GdiPlus.h>

extern GdiPlusManager g_gdiPlusManager;
using namespace Gdiplus;

// CSimRadioButton

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCustomRadioButton, CCustomButton)

CCustomRadioButton::CCustomRadioButton()
{
	m_bCheck = false;
}

CCustomRadioButton::~CCustomRadioButton()
{
}

BEGIN_MESSAGE_MAP(CCustomRadioButton, CCustomButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_MESSAGE(WM_RADIO_CHECK_STATE_UPDATE, &CCustomRadioButton::OnRadioStateUpdate)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CSimRadioButton 메시지 처리기입니다.

BOOL CCustomRadioButton::GetCheck()
{
	return m_bCheck;
}

void CCustomRadioButton::SetCheck(BOOL check)
{
	m_bCheck = check;
}


// CSimButtton 메시지 처리기입니다.



void CCustomRadioButton::PreSubclassWindow()
{
	//ModifyStyle(0x0FL, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

void CCustomRadioButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Gdiplus::Pen rect_pen1(Color(m_chThickColorR, m_chThickColorG, m_chThickColorB), (float)m_iThickness);
	Gdiplus::Pen rect_pen2(Color(m_chThickColorR, m_chThickColorG, m_chThickColorB), ((m_iThickness / 2) - 1) * 2.f);

	if (!(GetStyle() & BS_PUSHLIKE))
	{
		CCustomButton::DrawItem(lpDrawItemStruct);

		CString ctrlText;
		GetWindowText(ctrlText);

		CClientDC dc(this);

		CRect rcClient;

		GetClientRect(&rcClient);

		CBitmap bmp;
		CBitmap* pOldBmp;
		CDC memDC;

		memDC.CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		pOldBmp = (CBitmap*)memDC.SelectObject(&bmp);

		dc.SetBkMode(TRANSPARENT);
		dc.SelectObject(&m_cFont);
		dc.SetTextColor(m_sFontColor);
		UINT state = lpDrawItemStruct->itemState;

		Graphics graphics(memDC.m_hDC);

		if (!m_bImage && !m_bGradation && !m_bPathGradation)
		{
			BYTE r = GetRValue(m_sBackColor);
			BYTE g = GetGValue(m_sBackColor);
			BYTE b = GetBValue(m_sBackColor);

			if ((state & ODS_DISABLED))
			{
				if (r > 200)
					r -= 50;
				else
					r += 50;
				if (g > 200)
					g -= 50;
				else
					g += 50;
				if (b > 200)
					b -= 50;
				else
					b += 50;

				dc.FillSolidRect(&rcClient, RGB(r, g, b));
				SolidBrush brush = Color(r, g, b);
				graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				dc.SetTextColor(RGB(128, 128, 128));
				graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else
			{
				if ((state & ODS_SELECTED) && m_bMouseOver)
				{
					SolidBrush brush = Color(r, g, b);
					graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				}
				else if (m_bMouseOver)
				{
					Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
					SolidBrush brush = Color(r, g, b);
					graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
				else
				{
					SolidBrush brush = Color(r, g, b);
					graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
		}
		else if (!m_bImage && m_bGradation && !m_bPathGradation)
		{
			if ((state & ODS_DISABLED))
			{
				LinearGradientBrush disableBrush(Rect(0, 0, rcClient.Width(), rcClient.Height()), Color(128, 128, 128), Color(255, 255, 255), LinearGradientMode::LinearGradientModeVertical);

				graphics.FillRectangle(&disableBrush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				dc.SetTextColor(RGB(32, 32, 32));
				graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else
			{
				auto grBrush = m_accessor->GetLinearGradientBrush(rcClient.Width(), rcClient.Height());
				if ((state & ODS_SELECTED) && m_bMouseOver)
				{
					graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				}
				else if (m_bMouseOver)
				{
					Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
					graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
				else
				{
					graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
		}
		else if (!m_bImage && !m_bGradation && m_bPathGradation)
		{
			if ((state & ODS_DISABLED))
			{
				LinearGradientBrush disableBrush(Rect(0, 0, rcClient.Width(), rcClient.Height()), Color(128, 128, 128), Color(255, 255, 255), LinearGradientMode::LinearGradientModeVertical);

				graphics.FillRectangle(&disableBrush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				dc.SetTextColor(RGB(32, 32, 32));
				graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else
			{
				auto pthGrBrush = m_accessor->GetBackPathGradationBrush(rcClient.Width(), rcClient.Height());
				if ((state & ODS_SELECTED) && m_bMouseOver)
				{
					graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				}
				else if (m_bMouseOver)
				{
					Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
					graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
				else
				{
					graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
		}
		else
		{
			auto image = m_accessor->GetImage(ID_BACK_IMAGE);

			if ((state & ODS_DISABLED))
			{
				try
				{
					graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), (image->GetWidth() / 4) * 3, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					dc.SetTextColor(RGB(128, 128, 128));
				}
				catch (CException* e)
				{
					LPTSTR str = _T("");
					e->GetErrorMessage(str, 0);
					AfxMessageBox(str);
				}
			}
			else
			{

				if ((state & ODS_SELECTED) && m_bMouseOver)
				{
					try
					{
						graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), (image->GetWidth() / 4) * 2, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					}
					catch (CException* e)
					{
						LPTSTR str = _T("");
						e->GetErrorMessage(str, 0);
						AfxMessageBox(str);
					}
				}
				else if (m_bMouseOver)
				{
					try
					{
						graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), image->GetWidth() / 4, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					}
					catch (CException* e)
					{
						LPTSTR str = _T("");
						e->GetErrorMessage(str, 0);
						AfxMessageBox(str);
					}
				}
				else
				{
					try
					{
						graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), 0, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					}
					catch (CException* e)
					{
						LPTSTR str = _T("");
						e->GetErrorMessage(str, 0);
						AfxMessageBox(str);
					}
				}
			}

		}
		Rect circleRect = Rect(rcClient.Width() / 20, (rcClient.Height() / 3), (rcClient.Height() / 3), (rcClient.Height() / 3));


		//CButton::GetCheckedRadioButton()
		//UpdateData(TRUE);
		//int iID = GetDlgCtrlID();
		//int iState = GetCheck();
		//int i = GetCheckedRadioButton(m_iGroupStartID, m_iGroupEndID);
		//UpdateData(FALSE);
		//if (CButton::GetCheckedRadioButton(m_iGroupStartID, m_iGroupEndID) != GetDlgCtrlID())

		Gdiplus::SolidBrush whiteBrush(Color(255, 255, 255));

		if (m_bMouseOver)
		{
			Gdiplus::Pen rect_pen(Color(88, 174, 223), (float)m_iThickness);

			if (!GetCheck())
			{
				graphics.FillEllipse(&whiteBrush, circleRect);
				graphics.DrawEllipse(&rect_pen, circleRect);
			}
			else
			{
				Gdiplus::SolidBrush innerBrush(Color(88, 174, 223));
				
				graphics.FillEllipse(&whiteBrush, circleRect);
				graphics.FillEllipse(&innerBrush, Rect(circleRect.GetLeft() + (circleRect.Width / 4), circleRect.GetTop() + (circleRect.Height / 4), (circleRect.Width / 2), (circleRect.Width / 2)));
				graphics.DrawEllipse(&rect_pen, circleRect);
			}
		}
		else
		{
			Gdiplus::SolidBrush blackBrush(Color(0, 0, 0));

			if (!GetCheck())
			{
				graphics.FillEllipse(&whiteBrush, circleRect);
				graphics.DrawEllipse(&rect_pen1, circleRect);
			}
			else
			{
				graphics.FillEllipse(&whiteBrush, circleRect);
				graphics.FillEllipse(&blackBrush, Rect(circleRect.GetLeft() + (circleRect.Width / 4), circleRect.GetTop() + (circleRect.Height / 4), (circleRect.Width / 2), (circleRect.Width / 2)));
				graphics.DrawEllipse(&rect_pen1, circleRect);
			}
		}

		graphics.SetSmoothingMode(SmoothingModeHighQuality);

		dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();

		if (ctrlText.GetLength() > 0)
		{
			if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
				dc.DrawText(ctrlText, CRect(5, 0, rcClient.Width(), rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
			else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
				dc.DrawText(ctrlText, CRect(0, 0, rcClient.Width() - 5, rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
			else
				dc.DrawText(ctrlText, rcClient, m_uiTextAlign[m_iFontAlign]);
		}
		else
		{ }
	}

	else
	{
		CString ctrlText;
		GetWindowText(ctrlText);

		CClientDC dc(this);

		CRect rcClient;

		GetClientRect(&rcClient);

		CBitmap bmp;
		CBitmap* pOldBmp;
		CDC memDC;

		memDC.CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		pOldBmp = (CBitmap*)memDC.SelectObject(&bmp);

		dc.SetBkMode(TRANSPARENT);
		dc.SelectObject(&m_cFont);
		dc.SetTextColor(m_sFontColor);
		UINT state = lpDrawItemStruct->itemState;

		Graphics graphics(memDC.m_hDC);

		if (!m_bImage && !m_bGradation && !m_bPathGradation)
		{
			BYTE r = GetRValue(m_sBackColor);
			BYTE g = GetGValue(m_sBackColor);
			BYTE b = GetBValue(m_sBackColor);

			if ((state & ODS_DISABLED))
			{
				if (r > 200)
					r -= 50;
				else
					r += 50;
				if (g > 200)
					g -= 50;
				else
					g += 50;
				if (b > 200)
					b -= 50;
				else
					b += 50;

				dc.FillSolidRect(&rcClient, RGB(r, g, b));
				SolidBrush brush = Color(r, g, b);
				graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				dc.SetTextColor(RGB(128, 128, 128));
				graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else
			{
				if ( (m_bClick && m_bMouseOver) | GetCheck() )
				{
					SolidBrush brush = Color(r, g, b);
					graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				}
				else if (m_bMouseOver)
				{
					Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
					SolidBrush brush = Color(r, g, b);
					graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
				else
				{
					SolidBrush brush = Color(r, g, b);
					graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
		}
		else if (!m_bImage && m_bGradation && !m_bPathGradation)
		{
			if ((state & ODS_DISABLED))
			{
				LinearGradientBrush disableBrush(Rect(0, 0, rcClient.Width(), rcClient.Height()), Color(128, 128, 128), Color(255, 255, 255), LinearGradientMode::LinearGradientModeVertical);

				graphics.FillRectangle(&disableBrush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				dc.SetTextColor(RGB(32, 32, 32));
				graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else
			{
				auto grBrush = m_accessor->GetLinearGradientBrush(rcClient.Width(), rcClient.Height());
				if ( (m_bClick && m_bMouseOver) | GetCheck() )
				{
					graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				}
				else if (m_bMouseOver)
				{
					Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
					graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
				else
				{
					graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
		}
		else if (!m_bImage && !m_bGradation && m_bPathGradation)
		{
			if ((state & ODS_DISABLED))
			{
				LinearGradientBrush disableBrush(Rect(0, 0, rcClient.Width(), rcClient.Height()), Color(128, 128, 128), Color(255, 255, 255), LinearGradientMode::LinearGradientModeVertical);

				graphics.FillRectangle(&disableBrush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				dc.SetTextColor(RGB(32, 32, 32));
				graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else
			{
				auto pthGrBrush = m_accessor->GetBackPathGradationBrush(rcClient.Width(), rcClient.Height());
				if ( (m_bClick && m_bMouseOver) | GetCheck() )
				{
					graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				}
				else if (m_bMouseOver)
				{
					Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
					graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
				else
				{
					graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
		}
		else
		{
			auto image = m_accessor->GetImage(ID_BACK_IMAGE);

			if ((state & ODS_DISABLED))
			{
				try
				{
					graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), (image->GetWidth() / 4) * 3, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					dc.SetTextColor(RGB(128, 128, 128));
				}
				catch (CException* e)
				{
					LPTSTR str = _T("");
					e->GetErrorMessage(str, 0);
					AfxMessageBox(str);
				}
			}
			else
			{

				if ( (m_bClick && m_bMouseOver) || GetCheck() )
				{
					try
					{
						graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), (image->GetWidth() / 4) * 2, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					}
					catch (CException* e)
					{
						LPTSTR str = _T("");
						e->GetErrorMessage(str, 0);
						AfxMessageBox(str);
					}
				}
				else if (m_bMouseOver)
				{
					try
					{
						graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), image->GetWidth() / 4, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					}
					catch (CException* e)
					{
						LPTSTR str = _T("");
						e->GetErrorMessage(str, 0);
						AfxMessageBox(str);
					}
				}
				else
				{
					try
					{
						graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), 0, 0, image->GetWidth() / 4, image->GetHeight(), UnitPixel);
					}
					catch (CException* e)
					{
						LPTSTR str = _T("");
						e->GetErrorMessage(str, 0);
						AfxMessageBox(str);
					}
				}
			}

		}

		graphics.SetSmoothingMode(SmoothingModeHighQuality);

		dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();

		if (ctrlText.GetLength() > 0)
		{
			if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
				dc.DrawText(ctrlText, CRect(5, 0, rcClient.Width(), rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
			else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
				dc.DrawText(ctrlText, CRect(0, 0, rcClient.Width() - 5, rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
			else
				dc.DrawText(ctrlText, rcClient, m_uiTextAlign[m_iFontAlign]);
		}
		else
		{ }
	}
}

void CCustomRadioButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bClick = TRUE;
	Invalidate(FALSE);
	CButton::OnLButtonDown(nFlags, point);
}


void CCustomRadioButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rcClient;

	GetClientRect(&rcClient);

	if (m_bClick)
	{
		if (rcClient.PtInRect(point))
		{
			m_bClick = FALSE;

			HWND hthis, hwnd2, hpwnd;
			hpwnd = GetParent()->GetSafeHwnd();	//get button parent handle
			hwnd2 = hthis = GetSafeHwnd();			//get this button handle

			if (!GetCheck())
			{
				SetCheck(true);
				if (hthis && hpwnd)
				{				//consistency check
					for (; ; )
					{	//scan the buttons within the group
						hwnd2 = ::GetNextDlgGroupItem(hpwnd, hwnd2, 0);
						//until we reach again this button
						if ((hwnd2 == hthis) || (hwnd2 == NULL))
							break;
						//post the uncheck message
						if (hwnd2 != this->GetSafeHwnd())
							::PostMessage(hwnd2, WM_RADIO_CHECK_STATE_UPDATE, 0, 0);
					}
				}
				else
				{ }
			}
			else
			{ }


			Invalidate(FALSE);
		}
		else
		{ }
	}
	else
	{
		m_bClick = FALSE;
		Invalidate(FALSE);
	}

	CButton::OnLButtonUp(nFlags, point);
}


LRESULT CCustomRadioButton::OnRadioStateUpdate(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (GetCheck())
	{
		SetCheck(FALSE);
		Invalidate(FALSE);
	}
	else
	{ }

	return S_OK;
}

int CCustomRadioButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCustomButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	else
	{ }

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
