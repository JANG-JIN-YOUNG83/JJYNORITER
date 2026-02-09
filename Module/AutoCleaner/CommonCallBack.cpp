#include "stdafx.h"
#include "CommonCallBack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace AutoCleaner;

CommonCallBack::CommonCallBack(LPVOID userData, CALLBACK_CLEANER_STATE_CHANGED* callbackStateChanged)
	: m_userData(userData)
	, m_callbackStateChanged(callbackStateChanged)
{
}

CommonCallBack::~CommonCallBack()
{
}

void CommonCallBack::StateChanged(LPCTSTR fmt, ...)
{
	CString text;

	va_list argptr;

	va_start(argptr, fmt);
	text.FormatV(fmt, argptr);
	va_end(argptr);

	m_cs.Lock();
	m_callbackStateChanged(m_userData, text);
	m_cs.Unlock();
}
