#include "pch.h"
#include "BaseLogger.h"
#include "../../VersionInfo.h"

#include <string>
#include "Gadget/Miscellaneous.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Utility;

SYSTEMTIME g_loggerStartTime;

class LoggerStarter
{
public:
	LoggerStarter()
	{
		GetLocalTime(&g_loggerStartTime);
	}
};

LoggerStarter g_loggerStarter;

LoggerDate::LoggerDate()
	: m_year(0)
	, m_month(0)
	, m_day(0)
	, m_hour(0)
{
}

LoggerDate::LoggerDate(long year, long month, long day, long hour)
	: m_year(year)
	, m_month(month)
	, m_day(day)
	, m_hour(hour)
{
}

LoggerDate& LoggerDate::operator = (const LoggerDate& rhs)
{
	m_year = rhs.m_year;
	m_month = rhs.m_month;
	m_day = rhs.m_day;
	m_hour = rhs.m_hour;

	return *this;
}

bool LoggerDate::operator < (const LoggerDate& rhs) const
{
	if (m_year < rhs.m_year) return true;
	if (m_year > rhs.m_year) return false;
	if (m_month < rhs.m_month) return true;
	if (m_month > rhs.m_month) return false;
	if (m_day < rhs.m_day) return true;
	if (m_day > rhs.m_day) return false;

	return (m_hour < rhs.m_hour);
}

bool LoggerDate::operator != (const LoggerDate& rhs) const
{
	if (m_year != rhs.m_year) return true;
	if (m_month != rhs.m_month) return true;
	if (m_day != rhs.m_day) return true;

	return (m_hour != rhs.m_hour);
}

BaseLogger::BaseLogger(LoggerType loggerType, LPCTSTR logFolder, LPCTSTR fileName, LPVOID userData, long type, LOG_CALLBACK_GET_FILE_NAME* cbGetFileName, bool appendAutoLog, const int32_t flushingInterval_sec)
	: m_loggerType(loggerType)
	, m_logDirectory(logFolder)
	, m_userData(userData)
	, m_type(type)
	, m_cbGetFileName(cbGetFileName)
	, m_appendAutoLog(appendAutoLog)
	, m_flushingInterval_sec(flushingInterval_sec)
	, m_isTerminated(false)
{
	if (*m_logDirectory.Right(1) != _T('\\')) m_logDirectory += _T("\\");
	IPVM::CreateDirectories(m_logDirectory);

	::InitializeCriticalSection(&m_csLog);

	__super::BeginWorker();

	if (m_appendAutoLog)
	{
		// 모든 로그에 프로그램 시작시 버전을 표기해서 프로그램 재시작 상태를 확인하기 쉽게 하자
		CString firstFileName = fileName;
		if (!firstFileName.IsEmpty())
		{
			CString firstLog;
			firstLog.Format(_T("\n------------------------------------------------------------\n Start Intekplus Battery Notching Inspection %s\n------------------------------------------------------------")
				, (LPCTSTR)CString(IBCI_MACRO_VERSION));

			SYSTEMTIME time;
			GetLocalTime(&time);
			Write(firstFileName, time, firstLog);
		}
	}
}

BaseLogger::~BaseLogger()
{
	if (!m_isTerminated)
	{
		// Log 함수에서 Callback을 쓰고 있어서 여기서 LOG를 정리하려고 하면 안되서
		// Callback을 가진 부모가 Terminate 하게 함수를 따로 뺐다. 소멸자에서 Terminate 시키고 끝내자

		ASSERT(!_T("반드시 로거종료전 Terminate 함수를 호출해 주자."));
	}

	::DeleteCriticalSection(&m_csLog);
}

void BaseLogger::SetLogFolder(LPCTSTR folderPath)
{
	::EnterCriticalSection(&m_csLog);
	{
		if (m_logDirectory != folderPath)
		{
			m_logDirectory = folderPath;
			if (*m_logDirectory.Right(1) != _T('\\'))
			{
				m_logDirectory += _T("\\");
			}

			IPVM::CreateDirectories(folderPath);
		}
	}

	::LeaveCriticalSection(&m_csLog);
}

void BaseLogger::Terminate()
{
	if (m_isTerminated) return;	// 이미 Terminate 되었다

	__super::EndWorker();

	if (m_appendAutoLog)
	{
		// Terminate 로그
		Log(0, _T("\n------------------------------------------------------------\n Terminate Logger\n------------------------------------------------------------"));
	}

	// 남은 것들을 써준다.
	Flush();

	::EnterCriticalSection(&m_csLog);

	m_isTerminated = true;

	::LeaveCriticalSection(&m_csLog);
}

void BaseLogger::Write(const long level, const SYSTEMTIME& timeLocal, const SYSTEMTIME& timeSystem, const wchar_t* text)
{
	const auto& time = timeLocal;

	LoggerDate date(time.wYear, time.wMonth, time.wDay, time.wHour);

	Write(m_cbGetFileName(m_userData, m_type, date), timeLocal, text);
}

void BaseLogger::Write(LPCTSTR fileName, const SYSTEMTIME& time, const wchar_t* text)
{
	LoggerDate date(time.wYear, time.wMonth, time.wDay, time.wHour);

	CString writeText;
	writeText.Format(_T("[%02d:%02d:%02d.%03d] %s"), time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, text);

	::EnterCriticalSection(&m_csLog);
	{
		if (m_isTerminated)
		{
			// Terminate 된 이후로는 기록을 중단한다
			::LeaveCriticalSection(&m_csLog);
			return;
		}

		m_pendingLog[date][fileName].push_back(writeText);
	}
	::LeaveCriticalSection(&m_csLog);

	__super::Trigger();
}

bool BaseLogger::OnTrigger()
{
	Sleep(1000 * m_flushingInterval_sec);

	Flush();

	return true;
}

void BaseLogger::Flush()
{
	std::map<LoggerDate, std::map<CString, std::list<CString>>> pendingLog;
	CString logDirectory;

	::EnterCriticalSection(&m_csLog);
	{
		logDirectory = m_logDirectory;
		std::swap(pendingLog, m_pendingLog);
	}
	::LeaveCriticalSection(&m_csLog);

	if (pendingLog.size() == 0)
	{
		return;
	}

	for (auto& dateLog : pendingLog)
	{
		CString folderPath = logDirectory;

		switch (m_loggerType)
		{
		case LoggerType::SpeparateHour:
			folderPath.Format(_T("%s%04d%02d%02d\\%02d\\"),
				(LPCTSTR)logDirectory,
				dateLog.first.m_year,
				dateLog.first.m_month,
				dateLog.first.m_day,
				dateLog.first.m_hour);

			if (m_lastDate != dateLog.first)
			{
				IPVM::CreateDirectories(folderPath);
				m_lastDate = dateLog.first;
			}
			break;
		case LoggerType::SpeparateDate:
			folderPath.Format(_T("%s%04d%02d%02d\\"),
				(LPCTSTR)logDirectory,
				dateLog.first.m_year,
				dateLog.first.m_month,
				dateLog.first.m_day);

			if (m_lastDate != dateLog.first)
			{
				IPVM::CreateDirectories(folderPath);
				m_lastDate = dateLog.first;
			}
			break;
		}

		for (auto& fileLog : dateLog.second)
		{
			CString logFile = folderPath + fileLog.first;

			HANDLE file = ::CreateFile(logFile, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (file == INVALID_HANDLE_VALUE)
			{
				file = CreateFile(logFile, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

				USHORT nShort = 0xfeff;  // Unicode BOM(Byte Order Mark).
				DWORD len;
				WriteFile(file, &nShort, 2, &len, NULL);

				if (m_appendAutoLog)
				{
					CString header;
					header.Format(_T("[          Version info : %s [Start Time:%04d/%02d/%02d %02d:%02d:%02d]       ]\n"),
						(LPCTSTR)CString(IBCI_MACRO_VERSION),
						g_loggerStartTime.wYear, g_loggerStartTime.wMonth, g_loggerStartTime.wDay,
						g_loggerStartTime.wHour, g_loggerStartTime.wMinute, g_loggerStartTime.wSecond);

					header.AppendFormat(_T("LOG-DATE : %04d/%02d/%02d %02d Hour"), dateLog.first.m_year, dateLog.first.m_month, dateLog.first.m_day, dateLog.first.m_hour);
					header.AppendFormat(_T("\n\n"));

					WriteFile(file, LPCTSTR(header), header.GetLength() * sizeof(TCHAR), &len, nullptr);
				}

				if (file == INVALID_HANDLE_VALUE)
				{
					continue;
				}
			}

			LARGE_INTEGER liDistanceToMove;
			liDistanceToMove.QuadPart = 0;

			LARGE_INTEGER newFilePointer;
			::SetFilePointerEx(file, liDistanceToMove, &newFilePointer, FILE_END);

			DWORD dwWrote = 0;

			for (const auto& str : fileLog.second)
			{
				::WriteFile(file, (LPCTSTR)str, str.GetLength() * sizeof(TCHAR), &dwWrote, nullptr);
				::WriteFile(file, _T("\r\n"), 2 * sizeof(TCHAR), &dwWrote, nullptr);

                if (m_pEdit != nullptr && isRegisted)
                {
                   // AddStringToDisplay(str);
                }
			}

			::CloseHandle(file);
		}
	}
}

void BaseLogger::RegistToDisplay(CEdit* _pEdit)
{
    if (_pEdit == nullptr)
        return;
    m_pEdit = _pEdit;
    isRegisted = true;
}

void BaseLogger::AddStringToDisplay(CString _str)
{
    CString strLog;
    strLog.Format(_T("\r\n%s"), _str);
    int nLen = m_pEdit->GetWindowTextLengthW();
    m_pEdit->SetSel(nLen, nLen);
    m_pEdit->ReplaceSel(strLog);
}