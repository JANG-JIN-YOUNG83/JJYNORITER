#pragma once
#include <afxmt.h>
#include <map>
#include <memory>
#include <set>
#include <GdiPlus.h>

#define ID_BACK_IMAGE	(1)

class GdiPlusAccessor
{
public:
	GdiPlusAccessor();
	~GdiPlusAccessor();

	std::shared_ptr<Gdiplus::Image> LoadImageFromResource(long id, UINT resourceID);
	std::shared_ptr<Gdiplus::Image> LoadImageFromResource(long id, LPCTSTR resourceType, UINT resourceID);
	std::shared_ptr<Gdiplus::Image> LoadImageFromFile(long id, LPCTSTR filePath);
	std::shared_ptr<Gdiplus::Image> GetImage(long id);

	void SetBackGradationPara(COLORREF startColor, COLORREF midColor, COLORREF endColor, int direction);
	void SetBackGradationPara(COLORREF startColor, COLORREF endColor, int direction);

	void SetBackPathGradationPara(COLORREF leftTopColor, COLORREF rightTopColor, COLORREF leftBottomColor, COLORREF rightBottomColor, COLORREF midColor);

	std::shared_ptr<Gdiplus::LinearGradientBrush> GetLinearGradientBrush(long cx, long cy);
	std::shared_ptr<Gdiplus::PathGradientBrush> GetBackPathGradationBrush(long cx, long cy);

private:
	std::map<long, std::shared_ptr<Gdiplus::Image>> m_images;

	COLORREF m_grBrush_startColor;
	COLORREF m_grBrush_midColor;
	COLORREF m_grBrush_endColor;
	int m_grBrush_direction;
	bool m_grBrush_useMid;
	long m_grBrush_cx;
	long m_grBrush_cy;
	std::shared_ptr<Gdiplus::LinearGradientBrush> m_grBrush;

	COLORREF m_pthGrBrush_leftTopColor;
	COLORREF m_pthGrBrush_rightTopColor;
	COLORREF m_pthGrBrush_leftBottomColor;
	COLORREF m_pthGrBrush_rightBottomColor;
	COLORREF m_pthGrBrush_midColor;
	long m_pthGrBrush_cx;
	long m_pthGrBrush_cy;
	std::shared_ptr<Gdiplus::PathGradientBrush> m_pthGrBrush;
};

class GdiPlusManager
{
public:
	GdiPlusManager();
	~GdiPlusManager();

	GdiPlusAccessor* Get();
	void Release(GdiPlusAccessor* accessor);

private:
	ULONG_PTR m_gdiPlusToken;
	Gdiplus::GdiplusStartupInput m_gdiPlusStartInput;

	CCriticalSection m_cs;
	std::set<GdiPlusAccessor* >m_accessors;
};
