#include "stdafx.h"
#include "SimpleListBoxLogger.h"

#include "Gadget/TimeCheck.h"

enum
{
	UM_FIRST = WM_USER + 1,
	UM_UPDATE_LOG,
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SimpleListBoxLogger::SimpleListBoxLogger(const bool useLocalTime, const bool newLogToHead, const bool autoScroll, const long maxLineCount)
	: m_useLocalTime(useLocalTime)
	, m_newLogToHead(newLogToHead)
	, m_autoScroll(autoScroll)
	, m_maxLineCount(maxLineCount)
{
}


SimpleListBoxLogger::~SimpleListBoxLogger()
{
}

BEGIN_MESSAGE_MAP(SimpleListBoxLogger, CListBox)
	ON_MESSAGE(UM_UPDATE_LOG, &SimpleListBoxLogger::OnUpdateLog)
END_MESSAGE_MAP()

void SimpleListBoxLogger::SetUseLocalTime(const bool useLocalTime)
{
	m_useLocalTime = useLocalTime;
}

void SimpleListBoxLogger::SetNewLogToHead(const bool newLogToHead)
{
	m_newLogToHead = newLogToHead;
}

void SimpleListBoxLogger::SetAutoScroll(const bool autoScroll)
{
	m_autoScroll = autoScroll;
}

void SimpleListBoxLogger::SetMaxLineCount(const long maxLineCount)
{
	m_maxLineCount = max(1, maxLineCount);
}

void SimpleListBoxLogger::Write(const long level, const SYSTEMTIME &timeLocal, const SYSTEMTIME &timeSystem, const wchar_t *text)
{
	UNREFERENCED_PARAMETER(level);

	const auto &time = m_useLocalTime ? timeLocal : timeSystem;

	CString strWrite;
	strWrite.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d.%03d] %s"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, text);

	CSingleLock lock(&m_csLog, TRUE);

	m_pendingLog.push_back(strWrite);

	if (GetSafeHwnd())
	{
		PostMessage(UM_UPDATE_LOG);
	}
}

LRESULT SimpleListBoxLogger::OnUpdateLog(WPARAM wparam, LPARAM lparam)
{
	UNREFERENCED_PARAMETER(wparam);
	UNREFERENCED_PARAMETER(lparam);

	std::list<CString> pendingLog;
	{
		MSG msg;

		while (PeekMessage(&msg, GetSafeHwnd(), UM_UPDATE_LOG, UM_UPDATE_LOG, PM_REMOVE));

		CSingleLock lock(&m_csLog, TRUE);

		std::swap(pendingLog, m_pendingLog);
	}

	SetRedraw(FALSE);

	if (m_newLogToHead)
	{
		for (const auto &str : pendingLog)
		{
			InsertString(0, str);

			if (GetCount() > m_maxLineCount)
			{
				DeleteString(GetCount() - 1);
			}
		}

		if (m_autoScroll)
		{
			SetCurSel(0);
		}
	}
	else
	{
		for (const auto &str : pendingLog)
		{
			InsertString(GetCount(), str);

			if (GetCount() > m_maxLineCount)
			{
				DeleteString(0);
			}
		}

		if (m_autoScroll)
		{
			SetCurSel(GetCount() - 1);
		}
	}

	SetRedraw(TRUE);

	return 1L;
}
