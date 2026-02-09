#include "pch.h"
#include "CustomCheckBox.h"
#include "GdiPlusAccessor.h"

#include <GdiPlus.h>

extern GdiPlusManager g_gdiPlusManager;
using namespace Gdiplus;

// CSimCheckBox

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCustomCheckBox, CButton)

CCustomCheckBox::CCustomCheckBox()
	: m_accessor(g_gdiPlusManager.Get())
{
	m_sBackColor = RGB(200, 200, 200);
	m_sFontColor = RGB(0, 0, 0);
	m_sTransparentColor = NULL;

	m_szImageName = _T("");
	m_szFontName = _T("");

	m_bImage = FALSE;
	m_bClick = FALSE;
	m_bCheck = FALSE;
	m_bMouseOver = FALSE;
	m_bGradation = FALSE;
	m_bPathGradation = FALSE;
	m_checkStyle = DEFAULT_STYLE;

	m_bCenterMultiLine = FALSE;
	m_bGetRect = FALSE;

	m_iThickness = 1;
	m_chThickColorR = 0;
	m_chThickColorG = 0;
	m_chThickColorB = 0;

	m_iFontAlign = 4;

	m_uiTextAlign[0] = DT_SINGLELINE | DT_LEFT | DT_TOP;
	m_uiTextAlign[1] = DT_SINGLELINE | DT_CENTER | DT_TOP;
	m_uiTextAlign[2] = DT_SINGLELINE | DT_RIGHT | DT_TOP;
	m_uiTextAlign[3] = DT_SINGLELINE | DT_LEFT | DT_VCENTER;
	m_uiTextAlign[4] = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
	m_uiTextAlign[5] = DT_SINGLELINE | DT_RIGHT | DT_VCENTER;
	m_uiTextAlign[6] = DT_SINGLELINE | DT_LEFT | DT_BOTTOM;
	m_uiTextAlign[7] = DT_SINGLELINE | DT_CENTER | DT_BOTTOM;
	m_uiTextAlign[8] = DT_SINGLELINE | DT_RIGHT | DT_BOTTOM;

	for (int i = 0; i < 10; i++)
	{
		m_iFontBold[i] = i * 100;
	}

	m_iChkIndex = 0;
}

CCustomCheckBox::~CCustomCheckBox()
{
	g_gdiPlusManager.Release(m_accessor);
}

BEGIN_MESSAGE_MAP(CCustomCheckBox, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CSimCheckBox 메시지 처리기입니다.

void CCustomCheckBox::PreSubclassWindow()
{
	//ModifyStyle(0x0FL, BS_OWNERDRAW | BS_AUTOCHECKBOX, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

void CCustomCheckBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CString ctrlText;
	GetWindowText(ctrlText);

	CClientDC dc(this);

	CRect rcClient;

	GetClientRect(&rcClient);

	CBitmap bmp;
	CBitmap* pOldBmp;
	CDC memDC;

	bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	

	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(&m_cFont);
	dc.SetTextColor(m_sFontColor);

	memDC.CreateCompatibleDC(&dc);
	pOldBmp = (CBitmap*)memDC.SelectObject(&bmp);

	UINT state = lpDrawItemStruct->itemState;

	Gdiplus::Pen rect_pen1(Color(m_chThickColorR, m_chThickColorG, m_chThickColorB), (float)m_iThickness);
	Gdiplus::Pen rect_pen2(Color(m_chThickColorR, m_chThickColorG, m_chThickColorB), ((m_iThickness / 2) - 1) * 2.f);

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
			if (m_bCheck)
			{
				SolidBrush brush = Color(r, g, b);
				graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else if (m_bMouseOver && !m_bCheck)
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
			if (m_bCheck)
			{
				graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else if (m_bMouseOver && !m_bCheck)
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

			if (m_bCheck)
			{
				graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else if (m_bMouseOver && !m_bCheck)
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

			if (m_bCheck)
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
			else if (m_bMouseOver && !m_bCheck)
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

	if (m_sTransparentColor == NULL)
		dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	else
		dc.TransparentBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, rcClient.Width(), rcClient.Height(), m_sTransparentColor);

	memDC.SelectObject(pOldBmp);
	
	if (ctrlText.GetLength() > 0)
	{
		if (m_checkStyle == DEFAULT_STYLE)
		{
			CRect checkRect = CRect(10, rcClient.Height() / 3, (int)(10 + rcClient.Height() / 3.25), (rcClient.Height() / 3) * 2);
			if (m_bCheck)
			{
				dc.FillSolidRect(checkRect, RGB(0, 255, 0));

				CPen pen, *oldPen;
				pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
				oldPen = dc.SelectObject(&pen);

				dc.MoveTo(10, checkRect.top);
				dc.LineTo(checkRect.right, checkRect.top);
				dc.LineTo(checkRect.right, checkRect.bottom);
				dc.LineTo(checkRect.left, checkRect.bottom);
				dc.LineTo(checkRect.left, checkRect.top);

				pen.DeleteObject();
			}
			else
			{
				dc.FillSolidRect(checkRect, RGB(128, 128, 128));

				CPen pen, *oldPen;
				pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
				oldPen = dc.SelectObject(&pen);

				dc.MoveTo(10, checkRect.top);
				dc.LineTo(checkRect.right, checkRect.top);
				dc.LineTo(checkRect.right, checkRect.bottom);
				dc.LineTo(checkRect.left, checkRect.bottom);
				dc.LineTo(checkRect.left, checkRect.top);

				pen.DeleteObject();
			}

			if (!m_bCenterMultiLine)
			{
				if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
					dc.DrawText(ctrlText, CRect(5, 0, rcClient.Width(), rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
				else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
					dc.DrawText(ctrlText, CRect(0, 0, rcClient.Width() - 5, rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
				else
					dc.DrawText(ctrlText, rcClient, m_uiTextAlign[m_iFontAlign]);
			}
			else
			{
				if (!m_bGetRect)
					GetCenterRect(ctrlText, dc);

				if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
					dc.DrawText(ctrlText, CRect(5, m_CenterRect.top, rcClient.Width(), m_CenterRect.bottom), DT_LEFT);
				else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
					dc.DrawText(ctrlText, CRect(0, m_CenterRect.top, rcClient.Width() - 5, m_CenterRect.bottom), DT_RIGHT);
				else
					dc.DrawText(ctrlText, CRect(0, m_CenterRect.top, rcClient.Width(), m_CenterRect.bottom), DT_CENTER);
			}
		}
		else
		{
			if (!m_bCenterMultiLine)
			{
				if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
					dc.DrawText(ctrlText, CRect(5, 0, rcClient.Width(), rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
				else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
					dc.DrawText(ctrlText, CRect(0, 0, rcClient.Width() - 5, rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
				else
					dc.DrawText(ctrlText, rcClient, m_uiTextAlign[m_iFontAlign]);
			}
			else
			{
				if (!m_bGetRect)
					GetCenterRect(ctrlText, dc);

				if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
					dc.DrawText(ctrlText, CRect(5, m_CenterRect.top, rcClient.Width(), m_CenterRect.bottom), DT_LEFT);
				else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
					dc.DrawText(ctrlText, CRect(0, m_CenterRect.top, rcClient.Width() - 5, m_CenterRect.bottom), DT_RIGHT);
				else
					dc.DrawText(ctrlText, CRect(0, m_CenterRect.top, rcClient.Width(), m_CenterRect.bottom), DT_CENTER);
			}
		}
	}

	if (m_bCheck && m_checkStyle != DEFAULT_STYLE)
	{
		dc.DrawEdge(&rcClient, EDGE_ETCHED, BF_RECT);
	}
	else
	{
		dc.Draw3dRect(rcClient, GetSysColor(COLOR_3DLIGHT), GetSysColor(COLOR_3DSHADOW));
	}

	memDC.DeleteDC();
}

void CCustomCheckBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bClick = TRUE;
	Invalidate(FALSE);
	CButton::OnLButtonDown(nFlags, point);
}


void CCustomCheckBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rcClient;

	GetClientRect(&rcClient);

	if (m_bClick)
	{
		if (m_checkStyle == DEFAULT_STYLE)
		{
			CRect checkRect = CRect(10, rcClient.Height() / 4, 10 + rcClient.Height() / 2, (rcClient.Height() / 4) * 3);

			if (checkRect.PtInRect(point))
			{
				m_bClick = FALSE;

				if (m_bCheck)
				{
					m_bCheck = FALSE;
				}
				else
				{
					m_bCheck = TRUE;
				}

				Invalidate(FALSE);
			}
			else
			{
			}
		}
		else
		{
			if (rcClient.PtInRect(point))
			{
				m_bClick = FALSE;

				if (m_bCheck)
				{
					m_bCheck = FALSE;
				}
				else
				{
					m_bCheck = TRUE;
				}

				Invalidate(FALSE);
			}
			else
			{
			}
		}
	}
	else
	{
		m_bClick = FALSE;
		Invalidate(FALSE);
	}

	CButton::OnLButtonUp(nFlags, point);
}

void CCustomCheckBox::SetBackColor(COLORREF backColor)
{
	m_sBackColor = backColor;
}


void CCustomCheckBox::SetThick(int thickness, COLORREF thickColor)
{
	if (thickness < 1)
		thickness = 1;
	else
	{
	}
	
	m_iThickness = thickness * 2;
	m_chThickColorR = GetRValue(thickColor);
	m_chThickColorG = GetGValue(thickColor);
	m_chThickColorB = GetBValue(thickColor);
}

void CCustomCheckBox::SetBackGradation(COLORREF startColor, COLORREF endColor, int direction)
{
	m_accessor->SetBackGradationPara(startColor, endColor, direction);
	m_bGradation = TRUE;
}

void CCustomCheckBox::SetBackGradation(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction)
{
	m_accessor->SetBackGradationPara(startColor, midColor, endColor, direction);
	m_bGradation = TRUE;
}

void CCustomCheckBox::SetBackPathGradation(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor)
{
	m_accessor->SetBackPathGradationPara(leftTopColor, rightTopColor, leftBottomColor, rightBottomColor, midColor);
	m_bPathGradation = TRUE;
}

void CCustomCheckBox::SetImage(CString imageName)
{
	m_szImageName = imageName;

	auto image = m_accessor->LoadImageFromFile(ID_BACK_IMAGE, m_szImageName);

	if (image->GetFlags() == 0)
	{
		OutputDebugString(m_szImageName);
		OutputDebugString(_T("\n"));
		AfxMessageBox(_T("Failed to load image"));
	}
	else
	{
	}

	m_bImage = TRUE;
}

void CCustomCheckBox::SetFont(CString fontName, COLORREF fontColor, int fontSize, int fontBold, int fontAlign)
{
	m_cFont.CreateFont(fontSize, 0, 0, 0, m_iFontBold[fontBold], FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, fontName);
	CButton::SetFont(&m_cFont);

	m_iFontAlign = fontAlign;
	m_sFontColor = fontColor;

	m_iFontAlign--;

}

void CCustomCheckBox::OnDestroy()
{
	CButton::OnDestroy();
}


void CCustomCheckBox::OnMouseLeave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bClick = FALSE;
	m_bMouseOver = FALSE;
	Invalidate(FALSE);

	CButton::OnMouseLeave();
}


void CCustomCheckBox::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = 1;

	::_TrackMouseEvent(&tme);



	CButton::OnMouseMove(nFlags, point);
}

void CCustomCheckBox::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bMouseOver = TRUE;
	Invalidate(FALSE);

	CButton::OnMouseHover(nFlags, point);
}

BOOL CCustomCheckBox::GetCheck()
{
	return m_bCheck;
}

void CCustomCheckBox::SetCheck(BOOL check)
{
	m_bCheck = check;
	PostMessage(WM_PAINT);
//	SendMessage(WM_PAINT);
//	Invalidate();
}


void CCustomCheckBox::SetStyle(UINT style)
{
	m_checkStyle = style;
}

void CCustomCheckBox::SetTransparentColor(COLORREF color)
{
	m_sTransparentColor = color;
}

// CSimCheckBox 메시지 처리기입니다.


int CCustomCheckBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	else
	{
	}

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CCustomCheckBox::SetCenterMultiLine()
{
	m_bCenterMultiLine = TRUE;
}


void CCustomCheckBox::GetCenterRect(CString str, CDC& dc)
{
	CRect msgRect, rcClient;

	dc.DrawText(str, -1, msgRect, DT_NOCLIP | DT_CALCRECT);
//	CSize size = dc.GetTextExtent(str);

	GetClientRect(rcClient);

	int height = msgRect.Height();

	int y = (int)((rcClient.Height() / 2) - (height / 2));
	m_CenterRect.SetRect(rcClient.left, y, rcClient.Width(), y + height);

	m_bGetRect = TRUE;
}
