#include "stdafx.h"
#include "StringFormat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString StringFormat(LPCTSTR fmt, ...)
{
	CString text;
	va_list argptr;

	va_start(argptr, fmt);
	text.FormatV(fmt, argptr);
	va_end(argptr);

	return text;
}
