#include "stdafx.h"
#include "PropertyGridHelperEx.h"
#include "LanguageText.h"

#include "../../Module/CustomControl/PropertyGridHelper.h"
#include "../../Module/CustomControl/XTPPropertyGridItemEnum.h"
#include "../../Module/CustomControl/XTPPropertyGridItemSmallBool.h"
#include "../../Module/CustomControl/XTPPropertyGridItemCustomFloat.h"
#include "../../Module/CustomControl/XTPPropertyGridItemCustomNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Folder(CXTPPropertyGridItem* parent, LanguageDef def, HWND parentWnd, CString& value)
{
	return PropertyGridHelper::AddLink_Folder(parent, LANG_TEXT(def), parentWnd, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink(CXTPPropertyGridItem* parent, LanguageDef def, CString& value)
{
	return PropertyGridHelper::AddLink(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink(CXTPPropertyGridItem* parent, LanguageDef def, long& value)
{
	return PropertyGridHelper::AddLink(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink(CXTPPropertyGridItem* parent, LanguageDef def, int& value)
{
	return PropertyGridHelper::AddLink(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value)
{
	return PropertyGridHelper::AddLink(parent, caption, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value)
{
	return PropertyGridHelper::AddLink(parent, caption, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink(CXTPPropertyGridItem* parent, LanguageDef def, LPCTSTR fmt, double& value)
{
	return PropertyGridHelper::AddLink(parent, LANG_TEXT(def), fmt, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink(CXTPPropertyGridItem* parent, LanguageDef def, LPCTSTR fmt, float& value)
{
	return PropertyGridHelper::AddLink(parent, LANG_TEXT(def), fmt, value);
}

CXTPPropertyGridItemBool* PropertyGridHelperEx::AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, bool& value)
{
	auto* item = PropertyGridHelper::AddLink_Bool(parent, caption, value);
	item->SetTrueFalseText(LANG_TEXT(LanguageDef::USE), LANG_TEXT(LanguageDef::NOT_USE));

	return item;
}

CXTPPropertyGridItemBool* PropertyGridHelperEx::AddLink_CheckBox(CXTPPropertyGridItem* parent, LPCTSTR caption, bool& value)
{
	auto* item = PropertyGridHelper::AddLink_Bool(parent, caption, value);
	item->SetCheckBoxStyle();

	return item;
}

CXTPPropertyGridItemBool* PropertyGridHelperEx::AddLink_CheckBox(CXTPPropertyGridItem* parent, LanguageDef def, bool& value)
{
	auto* item = PropertyGridHelper::AddLink_Bool(parent, LANG_TEXT(def), value);
	item->SetCheckBoxStyle();

	return item;
}

CXTPPropertyGridItemBool* PropertyGridHelperEx::AddLink_Bool(CXTPPropertyGridItem* parent, LanguageDef def, bool& value)
{
	return AddLink_Bool(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItemBool* PropertyGridHelperEx::AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, BOOL& value)
{
	auto* item = new CXTPPropertyGridItemBool(caption, value, &value);
	item->SetTrueFalseText(LANG_TEXT(LanguageDef::USE), LANG_TEXT(LanguageDef::NOT_USE));

	return (CXTPPropertyGridItemBool*)parent->AddChildItem(item);
}

CXTPPropertyGridItemBool* PropertyGridHelperEx::AddLink_Bool(CXTPPropertyGridItem* parent, LanguageDef def, BOOL& value)
{
	return AddLink_Bool(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Enum(CXTPPropertyGridItem* parent, LanguageDef def, BYTE& value)
{
	return PropertyGridHelper::AddLink_Enum(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Enum(CXTPPropertyGridItem* parent, LanguageDef def, int& value)
{
	return PropertyGridHelper::AddLink_Enum(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Enum(CXTPPropertyGridItem* parent, LanguageDef def, long& value)
{
	return PropertyGridHelper::AddLink_Enum(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR strItemName, long& value)
{
	return PropertyGridHelper::AddLink_Enum(parent, strItemName, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Threshold(CXTPPropertyGridItem* parent, LanguageDef def, BYTE& value)
{
	return PropertyGridHelper::AddLink_Threshold(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Threshold(CXTPPropertyGridItem* parent, LanguageDef def, long& value)
{
	return PropertyGridHelper::AddLink_Threshold(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Threshold(CXTPPropertyGridItem* parent, LanguageDef def, int& value)
{
	return PropertyGridHelper::AddLink_Threshold(parent, LANG_TEXT(def), value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, BYTE& value)
{
	return PropertyGridHelper::AddLink_Threshold(parent, caption, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value)
{
	return PropertyGridHelper::AddLink_Threshold(parent, caption, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value)
{
	return PropertyGridHelper::AddLink_Threshold(parent, caption, value);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Rect(CXTPPropertyGridItem* parent, LPCTSTR caption, CRect& rect)
{
	return PropertyGridHelper::AddLink_Rect(parent, caption, rect);
}

CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Rect(CXTPPropertyGridItem* parent, LanguageDef def, CRect& rect)
{
	return PropertyGridHelper::AddLink_Rect(parent, LANG_TEXT(def), rect);
}
CXTPPropertyGridItem* PropertyGridHelperEx::AddLink_Button(CXTPPropertyGridItem* parent, LanguageDef def, BOOL bisFull, BOOL bInitValue)
{
	return PropertyGridHelper::AddLink_Button(parent, LANG_TEXT(def), bisFull, bInitValue);
}

void PropertyGridHelperEx::SetGridHidden(int nID, bool bHide, CXTPPropertyGrid* pGrid)
{
	auto* item = pGrid->FindItem(nID);

	if (item != NULL)
		item->SetHidden(!bHide);

}