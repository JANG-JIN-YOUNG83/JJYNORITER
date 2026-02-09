#pragma once
#include "basedef.h"

#ifdef _XTP_INCLUDE_CONTROLS

class __CUSTOMCONTROL_CLASS__ PropertyGridHelper
{
public:
	static CXTPPropertyGridItem* AddLink_Folder(CXTPPropertyGridItem* parent, LPCTSTR caption, HWND parentWnd, CString& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, CString& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, LPCTSTR fmt, double& value);
	static CXTPPropertyGridItem* AddLink(CXTPPropertyGridItem* parent, LPCTSTR caption, LPCTSTR fmt, float& value);
	static CXTPPropertyGridItemBool* AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, bool& value);
	static CXTPPropertyGridItemBool* AddLink_Bool(CXTPPropertyGridItem* parent, LPCTSTR caption, BOOL& value);
	static CXTPPropertyGridItem* AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR caption, BYTE& value);
	static CXTPPropertyGridItem* AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value);
	static CXTPPropertyGridItem* AddLink_Enum(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, BYTE& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, long& value);
	static CXTPPropertyGridItem* AddLink_Threshold(CXTPPropertyGridItem* parent, LPCTSTR caption, int& value);
	static CXTPPropertyGridItem* AddLink_Rect(CXTPPropertyGridItem* parent, LPCTSTR caption, CRect& rect);
	static CXTPPropertyGridItem* AddLink_Button(CXTPPropertyGridItem* parent, LPCTSTR caption, BOOL bisFull, BOOL bInitValue);
};

#endif
