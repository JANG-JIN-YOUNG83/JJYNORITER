#include "pch.h"
#include "PropertyGridHelper.h"
#include "XTPPropertyGridItemEnum.h"
#include "XTPPropertyGridItemFolder.h"
#include "XTPPropertyGridItemSmallBool.h"
#include "XTPPropertyGridItemCustomFloat.h"
#include "XTPPropertyGridItemCustomNumber.h"
#include "CustomItems.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _XTP_INCLUDE_CONTROLS

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Folder(CXTPPropertyGridItem* parent, LPCTSTR caption, HWND parentWnd, CString& value)
{
	auto* item = parent->AddChildItem(new XTPPropertyGridItemFolder(caption, parentWnd, value));
	item->AddExpandButton();

	return item;
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, CString& value)
{
	return parent->AddChildItem(new CXTPPropertyGridItem(caption, value, &value));
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value)
{
	return parent->AddChildItem(new CXTPPropertyGridItemNumber(caption, value, &value));
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value)
{
	return parent->AddChildItem(new XTPPropertyGridItemCustomNumber<int>(caption, value, _T("%d"), value));
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, LPCTSTR fmt, double& value)
{
	return parent->AddChildItem(new CXTPPropertyGridItemDouble(caption, value, fmt, &value));
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, LPCTSTR fmt, float& value)
{
	return parent->AddChildItem(new XTPPropertyGridItemCustomFloat<float>(caption, value, fmt, value));
}

CXTPPropertyGridItemBool* PropertyGridHelper::AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, bool& value)
{
	auto* item = new XTPPropertyGridItemSmallBool(caption, value, &value);

	return (CXTPPropertyGridItemBool *)parent->AddChildItem(item);
}

CXTPPropertyGridItemBool* PropertyGridHelper::AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, BOOL& value)
{
	auto* item = new CXTPPropertyGridItemBool(caption, value, &value);

	return (CXTPPropertyGridItemBool *)parent->AddChildItem(item);
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR caption, BYTE& value)
{
	return parent->AddChildItem(new XTPPropertyGridItemEnum<BYTE>(caption, value, value));
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value)
{
	return parent->AddChildItem(new XTPPropertyGridItemEnum<int>(caption, value, value));
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value)
{
	return parent->AddChildItem(new XTPPropertyGridItemEnum<long>(caption, value, value));
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, BYTE& value)
{
	auto* item = parent->AddChildItem(new XTPPropertyGridItemCustomNumber<BYTE>(caption, value, _T("%d"), value));
	if (item != nullptr)
	{
		auto *slider = item->AddSliderControl();
		slider->SetMin(0);
		slider->SetMax(255);
	}

	return item;
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value)
{
	auto* item = parent->AddChildItem(new CXTPPropertyGridItemNumber(caption, value, &value));
	if (item != nullptr)
	{
		auto *slider = item->AddSliderControl();
		slider->SetMin(0);
		slider->SetMax(255);
	}

	return item;
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value)
{
	auto* item = parent->AddChildItem(new XTPPropertyGridItemCustomNumber<int>(caption, value, _T("%d"), value));
	if (item != nullptr)
	{
		auto *slider = item->AddSliderControl();
		slider->SetMin(0);
		slider->SetMax(255);
	}

	return item;
}

CXTPPropertyGridItem* PropertyGridHelper::AddLink_Rect(CXTPPropertyGridItem* parent, LPCTSTR caption, CRect& rect)
{
	return parent->AddChildItem(new CCustomItemChilds(caption, &rect));
}
CXTPPropertyGridItem* PropertyGridHelper::AddLink_Button(CXTPPropertyGridItem* parent, LPCTSTR caption, BOOL bisFull, BOOL bInitValue)
{
	return parent->AddChildItem(new CCustomItemButton(caption, bisFull, bInitValue));
}
#endif
