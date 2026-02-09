#include "pch.h"
#include "CustomStatic.h"
#include "GdiPlusAccessor.h"

#include <GdiPlus.h>

extern GdiPlusManager g_gdiPlusManager;
using namespace Gdiplus;

// CSimStatic

IMPLEMENT_DYNAMIC(CCustomStatic, CStatic)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCustomStatic::CCustomStatic()
	: m_accessor(g_gdiPlusManager.Get())
{

	m_sBackColor = RGB(240, 240, 240);
	m_sFontColor = RGB(0, 0, 0);

	m_szFontName = _T("");

	m_bImage = FALSE;
	m_bClick = FALSE;
	m_bGroupBox = FALSE;
	m_bGradation = FALSE;
	m_bPathGradation = FALSE;
	m_bTransparent = FALSE;
	m_bCenterMultiLine = FALSE;
	m_bGetRect = FALSE;

	m_iThickness = 0;
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

	m_iStcIndex = 0;
}

CCustomStatic::~CCustomStatic()
{
	g_gdiPlusManager.Release(m_accessor);
}


BEGIN_MESSAGE_MAP(CCustomStatic, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



// CSimButtton 메시지 처리기입니다.

void CCustomStatic::LoadImageFromResource(LPCTSTR resourceType, UINT resourceID)
{
	m_accessor->LoadImageFromResource(ID_BACK_IMAGE, resourceType, resourceID);
	m_bImage = TRUE;
}

void CCustomStatic::LoadImageFromResource(UINT resourceID)
{
	auto image = m_accessor->LoadImageFromResource(ID_BACK_IMAGE, resourceID);

	if (image->GetFlags() == 0)
	{
		AfxMessageBox(_T("Failed to load image"));
	}
	else
	{
	}

	m_bImage = TRUE;
}

void CCustomStatic::SetImage(CString imageName)
 {
	 auto image = m_accessor->LoadImageFromFile(ID_BACK_IMAGE, imageName);

	 if (image->GetFlags() == 0)
	 {
		 AfxMessageBox(_T("Failed to load image"));
	 }
	 else
	 { }

	 m_bImage = TRUE;
 }

void CCustomStatic::SetBackColor(COLORREF backColor)
 {
	 m_sBackColor = backColor;
 }


void CCustomStatic::SetThick(int thickness, COLORREF thickColor)
 {
	 m_iThickness = thickness * 2;
	 m_chThickColorR = GetRValue(thickColor);
	 m_chThickColorG = GetGValue(thickColor);
	 m_chThickColorB = GetBValue(thickColor);
 }

void CCustomStatic::SetBackGradation(COLORREF startColor, COLORREF endColor, int direction)
{
	 m_accessor->SetBackGradationPara(startColor, endColor, direction);
	 m_bGradation = TRUE;
}

void CCustomStatic::SetBackGradation(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction)
{
	m_accessor->SetBackGradationPara(startColor, midColor, endColor, direction);
	m_bGradation = TRUE;
}

void CCustomStatic::SetBackPathGradation(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor)
{
	m_accessor->SetBackPathGradationPara(leftTopColor, rightTopColor, leftBottomColor, rightBottomColor, midColor);
	m_bPathGradation = TRUE;
}

void CCustomStatic::SetFont(CString fontName, COLORREF fontColor, int fontSize, int fontBold, int fontAlign)
{
	m_cFont.DeleteObject();

	m_cFont.CreateFont(fontSize, 0, 0, 0, m_iFontBold[fontBold], FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, fontName);
	CStatic::SetFont(&m_cFont);

	m_iFontAlign = fontAlign;
	m_sFontColor = fontColor;

	m_iFontAlign--;
}



void CCustomStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bClick = TRUE;

	CRect rect, parentRect;
	GetWindowRect(rect);
	this->GetParent()->GetWindowRect(parentRect);

	if (rect.TopLeft() == parentRect.TopLeft())
	{
		UINT moveNFlags = 2;
	}
	else
	{ }

	CStatic::OnLButtonDown(nFlags, point);
}


void CCustomStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rcClient;

	GetClientRect(&rcClient);

	if (m_bClick)
	{
		if (rcClient.PtInRect(point))
		{
			m_bClick = FALSE;
		}
		else
		{ }
	}
	else
	{
		m_bClick = FALSE;
	}

	//GetParent()->SendMessage(WM_LBUTTONUP);

	CStatic::OnLButtonUp(nFlags, point);
}


void CCustomStatic::OnDestroy()
{
	CStatic::OnDestroy();
}


void CCustomStatic::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//ModifyStyle(0x0FL, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CStatic::PreSubclassWindow();
}


void CCustomStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = 1;

	::_TrackMouseEvent(&tme);

	CStatic::OnMouseMove(nFlags, point);
}


void CCustomStatic::OnMouseLeave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bClick = FALSE;

	CStatic::OnMouseLeave();
}

void CCustomStatic::SetTransparent(BOOL transparent)
{
	m_bTransparent = transparent;
}

void CCustomStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	// TODO:  지정된 항목을 그리는 코드를 추가합니다.
	CDC *dc = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rcClient;

	GetClientRect(&rcClient);

	//dc->SetBkMode(TRANSPARENT);
	//dc->SelectObject(&m_cFont);
	//dc->SetTextColor(m_sFontColor);

	CBitmap bmp;
	CBitmap* pOldBmp;
	CDC memDC;

	memDC.CreateCompatibleDC(dc);
	bmp.CreateCompatibleBitmap(dc, rcClient.Width(), rcClient.Height());
	pOldBmp = (CBitmap*)memDC.SelectObject(&bmp);
	memDC.PatBlt(0, 0, rcClient.Width(), rcClient.Height(), BLACKNESS);
	memDC.SetBkMode(TRANSPARENT);
	memDC.SelectObject(&m_cFont);
	memDC.SetTextColor(m_sFontColor);

	Graphics graphics(memDC.m_hDC);
	//Graphics graphics(dc->GetSafeHdc());

	CString ctrlText;
	GetWindowText(ctrlText);

	Gdiplus::Pen rect_pen(Color(m_chThickColorR, m_chThickColorG, m_chThickColorB), (float)m_iThickness);
	
	if (!m_bImage && !m_bGradation && !m_bPathGradation && !m_bTransparent)
	{
		BYTE r = GetRValue(m_sBackColor);
		BYTE g = GetGValue(m_sBackColor);
		BYTE b = GetBValue(m_sBackColor);

		SolidBrush brush = Color(r, g, b);
		graphics.FillRectangle(&brush, Rect(0, 0, rcClient.Width(), rcClient.Height()));
	}
	else if (!m_bImage && m_bGradation && !m_bPathGradation && !m_bTransparent)
	{
		auto grBrush = m_accessor->GetLinearGradientBrush(rcClient.Width(), rcClient.Height());
		graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
	}
	else if (!m_bImage && !m_bGradation && m_bPathGradation && !m_bTransparent)
	{
		auto grBrush = m_accessor->GetLinearGradientBrush(rcClient.Width(), rcClient.Height());
		graphics.FillRectangle(grBrush.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()));
	}
	else if (m_bImage && !m_bGradation && !m_bPathGradation && !m_bTransparent)
	{
		auto image = m_accessor->GetImage(ID_BACK_IMAGE);

		try
		{
			graphics.DrawImage(image.get(), Rect(0, 0, rcClient.Width(), rcClient.Height()), 0, 0, image->GetWidth(), image->GetHeight(), UnitPixel);
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
	}

	if (m_iThickness > 0)
		graphics.DrawRectangle(&rect_pen, Rect(0, 0, rcClient.Width(), rcClient.Height()));
	else
	{ }

	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	//dc->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	//dc->TransparentBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, rcClient.Width(), rcClient.Height(), RGB(255, 0, 254));

	/*memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();*/
	//dc.DrawFrameControl(rcClient, DFC_CAPTION, DFC_BUTTONC);
	//dc.DrawEdge(rcClient, EDGE_ETCHED, BF_RECT);

	if (ctrlText.GetLength() > 0)
	{
		/*if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
			dc->DrawText(ctrlText, CRect(5, 0, rcClient.Width(), rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
		else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
			dc->DrawText(ctrlText, CRect(0, 0, rcClient.Width() - 5, rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
		else
			dc->DrawText(ctrlText, rcClient, m_uiTextAlign[m_iFontAlign]);*/
		if (!m_bCenterMultiLine)
		{
			if (m_iFontAlign == 0 || m_iFontAlign == 3 || m_iFontAlign == 6)
				memDC.DrawText(ctrlText, CRect(5, 0, rcClient.Width(), rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
			else if (m_iFontAlign == 2 || m_iFontAlign == 5 || m_iFontAlign == 8)
				memDC.DrawText(ctrlText, CRect(0, 0, rcClient.Width() - 5, rcClient.Height()), m_uiTextAlign[m_iFontAlign]);
			else
				memDC.DrawText(ctrlText, rcClient, m_uiTextAlign[m_iFontAlign]);
		}
		else
		{
			if (!m_bGetRect)
			{
				GetCenterRect(ctrlText, memDC);
				memDC.DrawText(ctrlText, m_CenterRect, DT_CENTER);
			}
			else
				memDC.DrawText(ctrlText, m_CenterRect, DT_CENTER);
		}
	}
	else
	{ }
	
	dc->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
}

BOOL CCustomStatic::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL result = CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);

	if (result)
		SetParent(pParentWnd);

	return result;
}


int CCustomStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	else
	{ }

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


CRect CCustomStatic::GetCtrlRect()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	return rcClient;
}


void CCustomStatic::SetCenterMultiLine()
{
	m_bCenterMultiLine = TRUE;
}


void CCustomStatic::GetCenterRect(CString str, CDC& dc)
{
	CRect msgRect, rcClient;

	dc.DrawText(str, -1, msgRect, DT_CENTER | DT_CALCRECT);

	GetClientRect(rcClient);

	int height = msgRect.Height();
	int y = (int)((rcClient.Height() - height) / 2);
	m_CenterRect.SetRect(rcClient.left, y, rcClient.Width(), y + height);

	m_bGetRect = TRUE;
}