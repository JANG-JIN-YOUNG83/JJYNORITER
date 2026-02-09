#include "pch.h"
#include "staticDefect.h"

#include "GraphCustomizeDEFECTMAP.h"


IMPLEMENT_DYNAMIC(staticDefect, CStatic)

staticDefect::staticDefect()
{
	m_graphStyle = std::make_shared<GraphCustomizeDEFECTMAP>();

	m_bFirst = true;
	m_bClear = false;
}

staticDefect::~staticDefect()
{
	End();
}

BEGIN_MESSAGE_MAP(staticDefect, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void staticDefect::SetPoint(long x, long y, COLORREF Color)
{
	DefectPointXY XY;
	XY.nX = x;
	XY.nY = y;
	XY.Color = Color;

	m_graphStyle->Draw(&m_MemDC, XY);

	RedrawWindow();
}

void staticDefect::InitStatic()
{
	m_bClear = true;

	RedrawWindow();
}

void staticDefect::OnPaint()
{
	// 처음이면 빈값으로 한번 셋팅
	if (m_bFirst)
	{
		m_bFirst = false;
		Ready();

		DefectPointXY data;
		m_graphStyle->Draw(&m_MemDC, data);
	}

	// 클리어하면 빈값 넣어서 다시 셋팅
	if (m_bClear)
	{
		m_bClear = false;

		DefectPointXY data;
		m_graphStyle->Draw(&m_MemDC, data);
	}

	CPaintDC dc(this);

	auto* oldBrush = m_MemDC.GetCurrentBrush();
	auto* oldFont = m_MemDC.SelectObject(GetFont());
	auto* oldPen = m_MemDC.GetCurrentPen();

	CRect rt;
	GetClientRect(rt);
	long cx = rt.Width();
	long cy = rt.Height();

	m_MemDC.SelectObject(oldBrush);
	m_MemDC.SelectObject(oldFont);
	m_MemDC.SelectObject(oldPen);

	dc.BitBlt(0, 0, cx, cy, &m_MemDC, 0, 0, SRCCOPY);
}

void staticDefect::Ready()
{
	if (m_MemDC.m_hDC)
	{
		End();
	}

	auto* pDC = GetDC();

	CRect rtClient;
	GetClientRect(rtClient);

	BOOL bResult = m_MemDC.CreateCompatibleDC(pDC);
	if (TRUE == bResult)
	{
		m_MemBitmap.CreateCompatibleBitmap(pDC, rtClient.Width(), rtClient.Height());
		m_pOldMemBitmap = m_MemDC.SelectObject(&m_MemBitmap);
	}
	else
	{
		DWORD error = GetLastError();
		TCHAR* message = nullptr;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			nullptr,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(TCHAR*)&message,
			0,
			nullptr);
		AfxMessageBox(_T("GDI A leak error has occurred. 4359834732"));
		AfxMessageBox(message);
	}

	m_graphStyle->UpdateViewSize(pDC, rtClient.Width(), rtClient.Height());

	ReleaseDC(pDC);
}

void staticDefect::End()
{
	m_MemDC.SelectObject(m_pOldMemBitmap);
	m_MemDC.DeleteDC();
	m_MemBitmap.DeleteObject();

	m_pOldMemBitmap = NULL;
}