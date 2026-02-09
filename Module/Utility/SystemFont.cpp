#include "pch.h"
#include "SystemFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetDefaultGUI_FaceName()
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));

	return ncm.lfMessageFont.lfFaceName;
}

CString g_guiFontFaceName = GetDefaultGUI_FaceName();

using namespace Utility;

bool SystemFont::CreateGUIFont(CFont& font, int pointSize)
{
	font.DeleteObject();
	return font.CreatePointFont(pointSize, GetDefaultGUI_FaceName()) ? true : false;
}

bool SystemFont::CreateGUIFont_Bold(CFont& font, int pointSize)
{
	if (!CreateGUIFont(font, pointSize)) return false;

	// 원래 폰트에 Bold 옵션만 추가해 주자
	LOGFONT logFont;
	font.GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;

	font.DeleteObject();
	return font.CreateFontIndirect(&logFont) ? true : false;
}
