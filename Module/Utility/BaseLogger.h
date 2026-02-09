#pragma once
#include "basedef.h"
#include "Base/LoggerInterface.h"
#include "Gadget/SimpleWorkerThread.h"
#include <map>
#include <list>

namespace Utility
{
	typedef CString(LOG_CALLBACK_GET_FILE_NAME)(LPVOID userData, long type, const LoggerDate& timeLocal);

	enum class LoggerType
	{
		Normal,
		SpeparateDate,
		SpeparateHour,
	};

	class __UTILITY_CLASS__ LoggerDate
	{
	public:
		LoggerDate();
		LoggerDate(long year, long month, long day, long hour);

		LoggerDate& operator = (const LoggerDate& rhs);

		bool operator < (const LoggerDate& rhs) const;
		bool operator != (const LoggerDate& rhs) const;

		long m_year;
		long m_month;
		long m_day;
		long m_hour;
	};

	class __UTILITY_CLASS__ BaseLogger : public IPVM::LoggerInterface, private IPVM::SimpleWorkerThread
	{
	public:
		BaseLogger(LoggerType loggerType, LPCTSTR logFolder, LPCTSTR fileName, LPVOID userData, long type, LOG_CALLBACK_GET_FILE_NAME* cbGetFileName, bool appendAutoLog = true, const int32_t flushingInterval_sec = 1);
		~BaseLogger();

		void SetLogFolder(LPCTSTR folderPath);
		void Terminate();

	protected:
		virtual void Write(const long level, const SYSTEMTIME& timeLocal, const SYSTEMTIME& timeSystem, const wchar_t* text) override;
		virtual bool OnTrigger() override;

	private:
		void Write(LPCTSTR fileName, const SYSTEMTIME& time, const wchar_t* text);

		const LoggerType m_loggerType;
		LPVOID m_userData;
		const long m_type;
		LOG_CALLBACK_GET_FILE_NAME* m_cbGetFileName;
		const bool m_appendAutoLog;
		const int32_t m_flushingInterval_sec;
		bool m_isTerminated;

		CString m_logDirectory;

		CRITICAL_SECTION m_csLog;
		std::map<LoggerDate, std::map<CString, std::list<CString>>> m_pendingLog;	// 날짜, 파일명, 로그들
		LoggerDate m_lastDate;

		void Flush();

    public:
        BOOL isRegisted = false;
        CEdit* m_pEdit;
        void RegistToDisplay(CEdit* _pEdit);
        void AddStringToDisplay(CString _str);
	};
}
