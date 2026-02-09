#pragma once

#include "basedef.h"

#include "Base/LoggerInterface.h"
#include "afxmt.h"
#include <list>

class __INTEKPLUS_SHARED_LOGCONTROL_CLASS__ SimpleListBoxLogger : public CListBox, public IPVM::LoggerInterface
{
public:
	SimpleListBoxLogger() = delete;
	SimpleListBoxLogger(const bool useLocalTime, const bool newLogToHead, const bool autoScroll, const long maxLineCount);
	virtual ~SimpleListBoxLogger();

public:
	void SetUseLocalTime(const bool useLocalTime);
	void SetNewLogToHead(const bool newLogToHead);
	void SetAutoScroll(const bool autoScroll);
	void SetMaxLineCount(const long maxLineCount);

protected:
	virtual void Write(const long level, const SYSTEMTIME &timeLocal, const SYSTEMTIME &timeSystem, const wchar_t *text) override;

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUpdateLog(WPARAM wparam, LPARAM lparam);

private:
	bool m_useLocalTime;
	bool m_newLogToHead;
	bool m_autoScroll;
	long m_maxLineCount;

	CCriticalSection m_csLog;
	std::list<CString> m_pendingLog;
};
