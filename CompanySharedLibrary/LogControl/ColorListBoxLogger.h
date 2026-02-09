#pragma once

// 아직 완성본이 아니므로 사용하지 마세요...
// 리소스 편집기에서 LBS_OWNERDRAWFIXED 를 설정해야 동작합니다.

#include "basedef.h"

#include "Base/LoggerInterface.h"
#include "afxmt.h"
#include <list>
#include <unordered_map>

class __INTEKPLUS_SHARED_LOGCONTROL_CLASS__ ColorListBoxLogger : public CListBox, public IPVM::LoggerInterface
{
public:
	ColorListBoxLogger() = delete;
	ColorListBoxLogger(const bool useLocalTime, const bool newLogToHead, const bool autoScroll, const long maxLineCount);
	virtual ~ColorListBoxLogger();

public:
	void SetUseLocalTime(const bool useLocalTime);
	void SetNewLogToHead(const bool newLogToHead);
	void SetAutoScroll(const bool autoScroll);
	void SetMaxLineCount(const long maxLineCount);

	void SetLevelColor(const long level, const COLORREF backgroundColor, const COLORREF textColor);

protected:
	virtual void Write(const long level, const SYSTEMTIME &timeLocal, const SYSTEMTIME &timeSystem, const wchar_t *text) override;

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUpdateLog(WPARAM wparam, LPARAM lparam);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

private:
	bool m_useLocalTime;
	bool m_newLogToHead;
	bool m_autoScroll;
	long m_maxLineCount;

	CCriticalSection m_csLog;
	std::list<std::pair<long, CString>> m_pendingLog;

	std::unordered_map<long, std::pair<COLORREF, COLORREF>> m_levelColors;
};
