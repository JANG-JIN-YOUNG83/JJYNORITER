#include "pch.h"
#include "CustomButton.h"
#include "GdiPlusAccessor.h"

#include <GdiPlus.h>

extern GdiPlusManager g_gdiPlusManager;
using namespace Gdiplus;

// CSimButtton

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCustomButton, CButton)

CCustomButton::CCustomButton()
	: m_accessor(g_gdiPlusManager.Get())
	, m_eventClicked_target(NULL)
	, m_eventClicked_message(0)
{
	m_sBackColor = RGB(200,200,200);
	m_sFontColor = RGB(0, 0, 0);
	m_sTransparentColor = NULL;

	m_szFontName = _T("");

	m_bImage = FALSE;
	m_bClick = FALSE;
	m_bMouseOver = FALSE;
	m_bGradation = FALSE;
	m_bPathGradation = FALSE;
	m_bDraw3D = FALSE;

	m_iThickness = 1;
	m_chThickColorR = 0;
	m_chThickColorG = 0;
	m_chThickColorB = 0;
	
	m_iFontAlign = 4;

	m_cFont.CreateFont(12, 0, 4, 0, 100, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("굴림"));

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

	m_iBtnIndex = 0;
}

CCustomButton::~CCustomButton()
{
	g_gdiPlusManager.Release(m_accessor);
}

BEGIN_MESSAGE_MAP(CCustomButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(BN_CLICKED, &CCustomButton::OnBnClicked)
END_MESSAGE_MAP()

// CSimButtton 메시지 처리기입니다.

void CCustomButton::PreSubclassWindow()
{
	//ModifyStyle(0x0FL, BS_OWNERDRAW, SWP_FRAMECHANGED);

	//GdiplusStartup(&m_gdiPlusToken, &m_gdiPlusStartInput, NULL);

	CButton::PreSubclassWindow();
}

void CCustomButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CString ctrlText;
	GetWindowText(ctrlText);

	//CClientDC dc(this);
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rcClient;

	GetClientRect(&rcClient);

	CBitmap bmp;
	CBitmap* pOldBmp;
	CDC memDC;

	memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
	pOldBmp = (CBitmap*)memDC.SelectObject(&bmp);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(TRANSPARENT);
	pDC->SelectObject(&m_cFont);
	pDC->SetTextColor(m_sFontColor);
	UINT state = lpDrawItemStruct->itemState;

	Gdiplus::Pen rect_pen1(Color(m_chThickColorR, m_chThickColorG, m_chThickColorB), (float)m_iThickness);
	Gdiplus::Pen rect_pen2(Color(m_chThickColorR, m_chThickColorG, m_chThickColorB), ((m_iThickness / 2) - 1) * 2.f);

	Graphics graphics(memDC.m_hDC);

	if(!m_bImage && !m_bGradation && !m_bPathGradation)
	{
		BYTE r = GetRValue(m_sBackColor);
		BYTE g = GetGValue(m_sBackColor);
		BYTE b = GetBValue(m_sBackColor);

		if((state & ODS_DISABLED))
		{
			if (r > 200)
			{
				r -= 50;
			}
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
			
			SolidBrush brush = Color(r, g, b);
			graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			pDC->SetTextColor(RGB(128, 128, 128));
			if (!m_bDraw3D)
			graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
		}
		else
		{
			if (m_bClick && m_bMouseOver)
			{
				SolidBrush brush = Color(r, g, b);
				graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				if (!m_bDraw3D)
				{
					graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				}
			}
			else if (m_bMouseOver)
			{
				Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);

				SolidBrush brush = Color(r, g, b);
				graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
				if (!m_bDraw3D)
				{
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
			else
			{
				SolidBrush brush = Color(r, g, b);
				graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
				if (!m_bDraw3D)
				{
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
		}
	}
	else if (!m_bImage && m_bGradation && !m_bPathGradation)
	{
		if ((state & ODS_DISABLED))
		{
			LinearGradientBrush disableBrush(Rect(0, 0, rcClient.Width(), rcClient.Height()), Color(128, 128, 128), Color(192, 192, 192), LinearGradientMode::LinearGradientModeVertical);

			graphics.FillRectangle(&disableBrush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			pDC->SetTextColor(RGB(32, 32, 32));
			if (!m_bDraw3D)

			graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
		}
		else
		{
			auto grBrush = m_accessor->GetLinearGradientBrush(rcClient.Width(), rcClient.Height());
			if (m_bClick && m_bMouseOver)
			{
				graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				if (!m_bDraw3D)
				graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else if (m_bMouseOver)
			{
				Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
				graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
				if (!m_bDraw3D)
				{
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
			else
			{
				graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				if (!m_bDraw3D)
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
			pDC->SetTextColor(RGB(32, 32, 32));
			if (!m_bDraw3D)
			{
				graphics.DrawRectangle(&rect_pen1, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
		}
		else
		{
			auto pthGrBrush = m_accessor->GetBackPathGradationBrush(rcClient.Width(), rcClient.Height());
			if (m_bClick && m_bMouseOver)
			{
				graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				if (!m_bDraw3D)
				graphics.DrawRectangle(&rect_pen2, Rect(0, 0, rcClient.Width(), rcClient.Height()));
			}
			else if (m_bMouseOver)
			{
				Gdiplus::Pen rect_pen(Color(67, 218, 236), ((m_iThickness / 2) - 1) * 2.f);
				graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				graphics.DrawRectangle(&rect_pen, 2, 2, rcClient.Width() - 4, rcClient.Height() - 4);
				if (!m_bDraw3D)
				{
					graphics.DrawRectangle(&rect_pen2, 0, 0, rcClient.Width(), rcClient.Height());
				}
			}
			else
			{
				graphics.FillRectangle(pthGrBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
				if (!m_bDraw3D)
				{
					graphics.DrawRectangle(&rect_pen1, 0, 0, rcClient.Width(), rcClient.Height());
				}
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
				pDC->SetTextColor(RGB(128, 128, 128));
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

			if (m_bClick && m_bMouseOver)
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
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	if (m_sTransparentColor == NULL)
	{
		pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	}
	else
	{
		pDC->TransparentBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, rcClient.Width(), rcClient.Height(), m_sTransparentColor);
	}

 	memDC.SelectObject(pOldBmp);
 	memDC.DeleteDC();

	if (!m_bClick && m_iThickness > 0 && m_bDraw3D)
	{
		pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DLIGHT), GetSysColor(COLOR_3DSHADOW));
	}

	if (ctrlText.GetLength() > 0)
	{
		if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
			pDC->DrawText(ctrlText, CRect(5, 0, rcClient.Width(), rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
		else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
			pDC->DrawText(ctrlText, CRect(0, 0, rcClient.Width() - 5, rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
		else
		pDC->DrawText(ctrlText, rcClient, m_uiTextAlign[m_iFontAlign]);
	}
}

void CCustomButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bClick = TRUE;
	Invalidate(FALSE);
	CButton::OnLButtonDown(nFlags, point);
}


void CCustomButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rcClient;

	GetClientRect(&rcClient);

	if (m_bClick)
	{
		if (rcClient.PtInRect(point))
		{
			m_bClick = FALSE;
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

void CCustomButton::SetBackColor(COLORREF backColor)
{
	m_sBackColor = backColor;
}


void CCustomButton::SetThick(int thickness, COLORREF thickColor)
{
	if (thickness < 1)
	{
		thickness = 1;
	}
	else
	{ }

	m_iThickness = thickness * 2;
	m_chThickColorR = GetRValue(thickColor);
	m_chThickColorG = GetGValue(thickColor);
	m_chThickColorB = GetBValue(thickColor);
}

void CCustomButton::SetBackGradation(COLORREF startColor, COLORREF endColor, int direction)
{
	m_accessor->SetBackGradationPara(startColor, endColor, direction);
	m_bGradation = TRUE;
}

void CCustomButton::SetBackGradation(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction)
{
	m_accessor->SetBackGradationPara(startColor, midColor, endColor, direction);
	m_bGradation = TRUE;
}

void CCustomButton::SetBackPathGradation(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor)
{
	m_accessor->SetBackPathGradationPara(leftTopColor, rightTopColor, leftBottomColor, rightBottomColor, midColor);
	m_bPathGradation = TRUE;
}

void CCustomButton::LoadImageFromResource(LPCTSTR resourceType, UINT resourceID)
{
	m_accessor->LoadImageFromResource(ID_BACK_IMAGE, resourceType, resourceID);
	m_bImage = TRUE;
}

void CCustomButton::LoadImageFromResource(UINT resourceID)
{
	auto image = m_accessor->LoadImageFromResource(ID_BACK_IMAGE, resourceID);

	if (image == nullptr || image->GetFlags() == 0)
	{
		AfxMessageBox(_T("Failed to load image"));
	}
	else
	{
	}

	m_bImage = TRUE;
}

void CCustomButton::SetImage(LPCTSTR imageName)
{
	auto image = m_accessor->LoadImageFromFile(ID_BACK_IMAGE, imageName);

	if (image->GetFlags() == 0)
	{
		OutputDebugString(imageName);
		OutputDebugString(_T("\n"));
		AfxMessageBox(_T("Failed to load image"));
	}
	else
	{ }

	m_bImage = TRUE;
}

void CCustomButton::SetEvent_Clicked(HWND target, UINT message)
{
	m_eventClicked_target = target;
	m_eventClicked_message = message;
}

void CCustomButton::SetFont(CString fontName, COLORREF fontColor, int fontSize, int fontBold, int fontAlign)
{
	m_cFont.DeleteObject();

	m_cFont.CreateFont(fontSize, 0, 0, 0, m_iFontBold[fontBold], FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, fontName);
	CButton::SetFont(&m_cFont);

	m_iFontAlign = fontAlign;
	m_sFontColor = fontColor;

	m_iFontAlign--;
}


void CCustomButton::SetTransparentColor(COLORREF color)
{
	m_sTransparentColor = color;
}

void CCustomButton::SetDraw3D(BOOL bDraw)
{
	m_bDraw3D = bDraw;
}

void CCustomButton::OnDestroy()
{
	CButton::OnDestroy();
}


void CCustomButton::OnMouseLeave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bClick = FALSE;
	m_bMouseOver = FALSE;
	Invalidate(FALSE);

	CButton::OnMouseLeave();
}


void CCustomButton::OnMouseMove(UINT nFlags, CPoint point)
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


void CCustomButton::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bMouseOver = TRUE;
	Invalidate(FALSE);
//	::AnimateWindow(GetSafeHwnd(), 1000, AW_BLEND | AW_HIDE);
//	::AnimateWindow(GetSafeHwnd(), 1000, AW_BLEND);

	CButton::OnMouseHover(nFlags, point);
}

BOOL CCustomButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL result = CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);

	if (result && !(dwStyle & BS_AUTORADIOBUTTON) && !(dwStyle & BS_RADIOBUTTON))
	{
		SetParent(pParentWnd);
	}
	else
	{ }

	return result;
}

int CCustomButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int res = 0;

	if (CButton::OnCreate(lpCreateStruct) == -1)
		res = -1;
	else
	{ }

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return res;
}

void CCustomButton::OnBnClicked()
{
	if (m_eventClicked_target != nullptr)
	{
		::SendMessage(m_eventClicked_target, m_eventClicked_message, GetDlgCtrlID(), 0);
	}
	else
	{
		GetOwner()->SendMessage(WM_COMMAND, GetDlgCtrlID(), 0);
	}
}
