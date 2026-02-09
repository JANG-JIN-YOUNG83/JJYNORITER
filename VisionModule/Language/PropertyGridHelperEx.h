#pragma once
#include "../Language/LanguageDef.h"

#ifdef __LANGUAGE_CLASS_EXPORT__
#define __LANGUAGE_CLASS__ __declspec(dllexport)
#else
#define __LANGUAGE_CLASS__ __declspec(dllimport)
#endif

class __LANGUAGE_CLASS__ PropertyGridHelperEx
{
public:
	static CXTPPropertyGridItem* AddLink_Folder(CXTPPropertyGridItem* parent, LanguageDef def, HWND parentWnd, CString& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LanguageDef def, CString& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LanguageDef def, long& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LanguageDef def, int& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LanguageDef def, LPCTSTR fmt, double& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LanguageDef def, LPCTSTR fmt, float& value);
	static CXTPPropertyGridItemBool* AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, bool& value);
	static CXTPPropertyGridItemBool* AddLink_Bool(CXTPPropertyGridItem* parent, LanguageDef def, bool& value);
	static CXTPPropertyGridItemBool* AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, BOOL& value);
	static CXTPPropertyGridItemBool* AddLink_Bool(CXTPPropertyGridItem* parent, LanguageDef def, BOOL& value);
	static CXTPPropertyGridItemBool* AddLink_CheckBox(CXTPPropertyGridItem* parent, LPCTSTR caption, bool& value);
	static CXTPPropertyGridItemBool* AddLink_CheckBox(CXTPPropertyGridItem* parent, LanguageDef def, bool& value);
	static CXTPPropertyGridItem* AddLink_Enum(CXTPPropertyGridItem* parent, LanguageDef def, BYTE& value);
	static CXTPPropertyGridItem* AddLink_Enum(CXTPPropertyGridItem* parent, LanguageDef def, int& value);
	static CXTPPropertyGridItem* AddLink_Enum(CXTPPropertyGridItem* parent, LanguageDef def, long& value);
	static CXTPPropertyGridItem* AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR strItemName, long& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LanguageDef def, BYTE& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LanguageDef def, long& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LanguageDef def, int& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, BYTE& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value);
	static CXTPPropertyGridItem* AddLink_Rect(CXTPPropertyGridItem* parent, LPCTSTR caption, CRect& rect);
	static CXTPPropertyGridItem* AddLink_Rect(CXTPPropertyGridItem* parent, LanguageDef def, CRect& rect);
	static CXTPPropertyGridItem* AddLink_Button(CXTPPropertyGridItem* parent, LanguageDef def, BOOL bisFull, BOOL bInitValue);
	static void SetGridHidden(int nID, bool bHide, CXTPPropertyGrid* pGrid);
};