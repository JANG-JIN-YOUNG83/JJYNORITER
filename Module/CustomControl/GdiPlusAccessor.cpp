#include "pch.h"
#include "GdiPlusAccessor.h"

#pragma comment(lib, "Gdiplus.lib")

//============================================================================================================

GdiPlusAccessor::GdiPlusAccessor()
	: m_grBrush_startColor(RGB(0, 0, 0))
	, m_grBrush_midColor(RGB(0, 0, 0))
	, m_grBrush_endColor(RGB(0, 0, 0))
	, m_grBrush_direction(0)
	, m_grBrush_useMid(false)
	, m_grBrush_cx(0)
	, m_grBrush_cy(0)
	, m_pthGrBrush_leftTopColor(RGB(0, 0, 0))
	, m_pthGrBrush_rightTopColor(RGB(0, 0, 0))
	, m_pthGrBrush_leftBottomColor(RGB(0, 0, 0))
	, m_pthGrBrush_rightBottomColor(RGB(0, 0, 0))
	, m_pthGrBrush_midColor(RGB(0, 0, 0))
	, m_pthGrBrush_cx(0)
	, m_pthGrBrush_cy(0)
{
}

GdiPlusAccessor::~GdiPlusAccessor()
{
}

std::shared_ptr<Gdiplus::Image> GdiPlusAccessor::LoadImageFromResource(long id, UINT resourceID)
{
	CBitmap bmp;
	if (!bmp.LoadBitmap(resourceID)) return nullptr;

	auto img = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)bmp.m_hObject, NULL);

	std::shared_ptr<Gdiplus::Image> imgShared(img);
	m_images[id] = imgShared;

	return imgShared;
}

std::shared_ptr<Gdiplus::Image> GdiPlusAccessor::LoadImageFromResource(long id, LPCTSTR resourceType, UINT resourceID)
{
	auto hInstance = AfxGetInstanceHandle();
	HRSRC hResource = ::FindResource(hInstance, MAKEINTRESOURCE(resourceID), resourceType);
	if (!hResource) return nullptr;

	DWORD imageSize = ::SizeofResource(hInstance, hResource);
	if (!imageSize) return nullptr;

	const void* pResourceData = ::LockResource(::LoadResource(hInstance, hResource));
	if (!pResourceData) return nullptr;

	HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (hBuffer == nullptr) return nullptr;

	void* pBuffer = ::GlobalLock(hBuffer);
	if (pBuffer == nullptr)
	{
		::GlobalUnlock(hBuffer);
		::GlobalFree(hBuffer);

		return nullptr;
	}

	CopyMemory(pBuffer, pResourceData, imageSize);

	IStream* pStream = NULL;
	if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
	{
		auto img = Gdiplus::Bitmap::FromStream(pStream);
		pStream->Release();

		if (img)
		{
			if (img->GetLastStatus() != Gdiplus::Ok)
			{
				delete img;
				return nullptr;
			}

			auto fmt = img->GetPixelFormat();
			Gdiplus::Bitmap* cloneImage = new Gdiplus::Bitmap(img->GetWidth(), img->GetHeight(), img->GetPixelFormat());
			Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(cloneImage);
			g->DrawImage(img, 0, 0);
			delete img;

			std::shared_ptr<Gdiplus::Image> imgShared(cloneImage);
			m_images[id] = imgShared;

			::GlobalUnlock(hBuffer);
			::GlobalFree(hBuffer);

			return imgShared;
		}
	}

	::GlobalUnlock(hBuffer);
	::GlobalFree(hBuffer);

	return nullptr;
}

std::shared_ptr<Gdiplus::Image> GdiPlusAccessor::LoadImageFromFile(long id, LPCTSTR filePath)
{
	Gdiplus::Image* img = nullptr;
	try
	{
		img = Gdiplus::Image::FromFile(filePath);
	}
	catch (CException* e)
	{
		LPTSTR str = _T("");
		e->GetErrorMessage(str, 0);
		AfxMessageBox(str);

		delete img;
		return nullptr;
	}

	std::shared_ptr<Gdiplus::Image> imgShared(img);
	m_images[id] = imgShared;

	return imgShared;
}

std::shared_ptr<Gdiplus::Image> GdiPlusAccessor::GetImage(long id)
{
	auto itImage = m_images.find(id);
	if (itImage != m_images.end())
	{
		return itImage->second;
	}

	return nullptr;
}

void GdiPlusAccessor::SetBackGradationPara(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction)
{
	m_grBrush_startColor = startColor;
	m_grBrush_midColor = midColor;
	m_grBrush_endColor = endColor;
	m_grBrush_direction = direction;
	m_grBrush_useMid = true;

	m_grBrush = nullptr;
}

void GdiPlusAccessor::SetBackGradationPara(COLORREF startColor, COLORREF endColor, int direction)
{
	m_grBrush_startColor = startColor;
	m_grBrush_endColor = endColor;
	m_grBrush_direction = direction;
	m_grBrush_useMid = false;

	m_grBrush = nullptr;
}

void GdiPlusAccessor::SetBackPathGradationPara(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor)
{
	m_pthGrBrush_leftTopColor = leftTopColor;
	m_pthGrBrush_rightTopColor = rightTopColor;
	m_pthGrBrush_leftBottomColor = leftBottomColor;
	m_pthGrBrush_rightBottomColor = rightBottomColor;
	m_pthGrBrush_midColor = midColor;

	m_pthGrBrush = nullptr;
}

std::shared_ptr<Gdiplus::LinearGradientBrush> GdiPlusAccessor::GetLinearGradientBrush(long cx, long cy)
{
	if (m_grBrush_cx != cx || m_grBrush_cy != cy)
	{
		m_grBrush = nullptr;
	}

	if (m_grBrush != nullptr)
	{
		return m_grBrush;
	}

	Gdiplus::Rect rect(0, 0, cx, cy);

	auto st_r = GetRValue(m_grBrush_startColor);
	auto st_g = GetGValue(m_grBrush_startColor);
	auto st_b = GetBValue(m_grBrush_startColor);

	auto ed_r = GetRValue(m_grBrush_endColor);
	auto ed_g = GetGValue(m_grBrush_endColor);
	auto ed_b = GetBValue(m_grBrush_endColor);

	if (m_grBrush_useMid)
	{
		auto mid_r = GetRValue(m_grBrush_midColor);
		auto mid_g = GetGValue(m_grBrush_midColor);
		auto mid_b = GetBValue(m_grBrush_midColor);

		Gdiplus::REAL real[] = { 0.0f, 0.5f, 1.0f };
		Gdiplus::Color colors_2[3] = { Gdiplus::Color(st_r, st_g, st_b), Gdiplus::Color(mid_r, mid_g, mid_b), Gdiplus::Color(ed_r, ed_g, ed_b) };
		m_grBrush = std::make_shared<Gdiplus::LinearGradientBrush>(rect, Gdiplus::Color(0, 0, 0), Gdiplus::Color(0, 0, 0), (Gdiplus::LinearGradientMode)m_grBrush_direction);
		m_grBrush->SetInterpolationColors(colors_2, real, 3);
		m_grBrush->SetGammaCorrection(TRUE);
	}
	else
	{
		m_grBrush = std::make_shared<Gdiplus::LinearGradientBrush>(rect, Gdiplus::Color(st_r, st_g, st_b), Gdiplus::Color(ed_r, ed_g, ed_b), (Gdiplus::LinearGradientMode)m_grBrush_direction);
	}

	m_grBrush_cx = cx;
	m_grBrush_cy = cy;

	return m_grBrush;
}

std::shared_ptr<Gdiplus::PathGradientBrush> GdiPlusAccessor::GetBackPathGradationBrush(long cx, long cy)
{
	if (m_pthGrBrush_cx != cx || m_pthGrBrush_cy != cy)
	{
		m_pthGrBrush = nullptr;
	}

	if (m_pthGrBrush != nullptr)
	{
		return m_pthGrBrush;
	}

	Gdiplus::Rect rect(0, 0, cx, cy);

	auto lt_r = GetRValue(m_pthGrBrush_leftTopColor);
	auto lt_g = GetGValue(m_pthGrBrush_leftTopColor);
	auto lt_b = GetBValue(m_pthGrBrush_leftTopColor);

	auto rt_r = GetRValue(m_pthGrBrush_rightTopColor);
	auto rt_g = GetGValue(m_pthGrBrush_rightTopColor);
	auto rt_b = GetBValue(m_pthGrBrush_rightTopColor);

	auto lb_r = GetRValue(m_pthGrBrush_leftBottomColor);
	auto lb_g = GetGValue(m_pthGrBrush_leftBottomColor);
	auto lb_b = GetBValue(m_pthGrBrush_leftBottomColor);

	auto rb_r = GetRValue(m_pthGrBrush_rightBottomColor);
	auto rb_g = GetGValue(m_pthGrBrush_rightBottomColor);
	auto rb_b = GetBValue(m_pthGrBrush_rightBottomColor);

	auto mid_r = GetRValue(m_pthGrBrush_midColor);
	auto mid_g = GetGValue(m_pthGrBrush_midColor);
	auto mid_b = GetBValue(m_pthGrBrush_midColor);

	auto *graphicPath = new Gdiplus::GraphicsPath();
	graphicPath->AddRectangle(Gdiplus::Rect(0, 0, cx, cy));

	m_pthGrBrush = std::make_shared<Gdiplus::PathGradientBrush>(graphicPath);
	m_pthGrBrush->SetCenterColor(Gdiplus::Color(mid_r, mid_g, mid_b));
	Gdiplus::Color colors[4] = { Gdiplus::Color(lt_r, lt_g, lt_b), Gdiplus::Color(rt_r, rt_g, rt_b), Gdiplus::Color(rb_r, rb_g, rb_b), Gdiplus::Color(lb_r, lb_g, lb_b) };
	INT count = 4;
	m_pthGrBrush->SetSurroundColors(colors, &count);
	m_pthGrBrush->SetCenterPoint(Gdiplus::Point(cx / 2, cy / 2));

	delete graphicPath;

	m_pthGrBrush_cx = cx;
	m_pthGrBrush_cy = cy;

	return m_pthGrBrush;
}

//============================================================================================================

GdiPlusManager::GdiPlusManager()
{
}

GdiPlusManager::~GdiPlusManager()
{
	while (m_accessors.size() > 0)
	{
		Release(*m_accessors.begin());
	}
}

GdiPlusAccessor* GdiPlusManager::Get()
{
	CSingleLock lock(&m_cs, TRUE);
	auto accessor = new GdiPlusAccessor;

	if (m_accessors.size() == 0)
	{
		GdiplusStartup(&m_gdiPlusToken, &m_gdiPlusStartInput, NULL);
	}

	m_accessors.insert(accessor);

	return accessor;
}

void GdiPlusManager::Release(GdiPlusAccessor* accessor)
{
	CSingleLock lock(&m_cs, TRUE);

	delete accessor;
	m_accessors.erase(accessor);

	if (m_accessors.size() == 0)
	{
		GdiplusStartup(&m_gdiPlusToken, &m_gdiPlusStartInput, NULL);
	}
}

GdiPlusManager g_gdiPlusManager;
