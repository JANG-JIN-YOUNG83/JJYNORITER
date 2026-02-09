#include "framework.h"

#include "CrashHandler.h"
#include "CrashInfo.h"

#include <new.h>
#include <array>
#include <cstdio>

#include "shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CriticalSection
{
public:
	CriticalSection()
	{
		::InitializeCriticalSection(&m_cs);
	}

	~CriticalSection()
	{
		::DeleteCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;

	friend class SingleLock;
};

static CriticalSection g_cs;

class SingleLock
{
public:
	SingleLock(CriticalSection& cs)
		: m_cs(cs)
	{
		::EnterCriticalSection(&m_cs.m_cs);
	}

	~SingleLock()
	{
		::LeaveCriticalSection(&m_cs.m_cs);
	}

private:
	CriticalSection &m_cs;
};


class CrashHandlerData
{
public:
	CrashHandlerData()
		: m_mappingHandle(NULL)
		, m_crashInfo(nullptr)
	{
		const wchar_t dumperName[] = L"IntekPlus.Shared.CrashDumper.exe\0";

		::memset(m_dumperAppDirectory, 0, sizeof(m_dumperAppDirectory));

		DWORD written = ::GetModuleFileNameW(NULL, m_dumperAppDirectory, sizeof(m_dumperAppDirectory)/sizeof(wchar_t));

		for (DWORD i = written - 1; i >= 0; i--)
		{
			if (m_dumperAppDirectory[i] == L'\\')
			{
				m_dumperAppDirectory[i] = 0;
				break;
			}
		}

		::swprintf_s(m_dumperAppPathName, L"%ls\\%ls", m_dumperAppDirectory, dumperName);

		if (::PathFileExists(m_dumperAppPathName) == FALSE)
		{
			::MessageBox(NULL, L"Cannot find \"IntekPlus.Shared.CrashDumper.exe\".", L"Error", MB_ICONERROR | MB_SYSTEMMODAL);
			::TerminateProcess(::GetCurrentProcess(), STATUS_INVALID_PARAMETER);
		}

		const auto processId = ::GetProcessId(::GetCurrentProcess());

		::swprintf_s(m_dumperCommandLine, L"%ls %d", m_dumperAppPathName, processId);

		wchar_t mappingHandleKey[512];

		::swprintf_s(mappingHandleKey, L"%ls_%d", KEY_mappingHandle, processId);

		if (m_mappingHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CrashInfo), mappingHandleKey))
		{
			if (m_crashInfo = ((CrashInfo*)::MapViewOfFile(m_mappingHandle, FILE_MAP_WRITE, 0, 0, sizeof(CrashInfo))))
			{
				::memset(m_crashInfo, 0, sizeof(CrashInfo));
			}
			else
			{
				::MessageBox(NULL, L"MapViewOfFile() failed!", L"Error", MB_ICONERROR | MB_SYSTEMMODAL);
				::TerminateProcess(::GetCurrentProcess(), STATUS_INVALID_PARAMETER);
			}
		}
		else
		{
			::MessageBox(NULL, L"CreateFileMapping() failed!", L"Error", MB_ICONERROR | MB_SYSTEMMODAL);
			::TerminateProcess(::GetCurrentProcess(), STATUS_INVALID_PARAMETER);
		}
	}

	static CrashHandlerData& Get()
	{
		static CrashHandlerData crashHandlerData;

		return crashHandlerData;
	}

	~CrashHandlerData()
	{

		if (m_crashInfo)
		{
			::UnmapViewOfFile(m_crashInfo);
			m_crashInfo = NULL;
		}

		if (m_mappingHandle)
		{
			::CloseHandle(m_mappingHandle);
			m_mappingHandle = NULL;
		}
	}

public:
	HANDLE m_mappingHandle;
	CrashInfo* m_crashInfo;

	wchar_t m_dumperAppDirectory[4096];
	wchar_t m_dumperAppPathName[4096];
	wchar_t m_dumperCommandLine[5120];
};


LONG CustomExceptionHandler(PEXCEPTION_POINTERS pExcInf)
{
	// 여러 쓰레드에서 예외가 발생하는 경우 문제가 될 수 있으므로 락을 건다.
	SingleLock lock(g_cs);

	auto& crashHandlerData = CrashHandlerData::Get();

	crashHandlerData.m_crashInfo->m_targetProcessId = ::GetCurrentProcessId();
	crashHandlerData.m_crashInfo->m_targetThreadId = ::GetCurrentThreadId();
	crashHandlerData.m_crashInfo->m_exceptionPointers = pExcInf;

	STARTUPINFOW startupInfo = { 0 };
	startupInfo.cb = sizeof(STARTUPINFOW);

	PROCESS_INFORMATION processInformation;

	if (::CreateProcessW(crashHandlerData.m_dumperAppPathName, crashHandlerData.m_dumperCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation))
	{
		::WaitForSingleObject(processInformation.hProcess, INFINITE);

		::CloseHandle(processInformation.hThread);
		::CloseHandle(processInformation.hProcess);

		::TerminateProcess(::GetCurrentProcess(), STATUS_INVALID_PARAMETER);

		return EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		::MessageBox(NULL, L"Cannot launch \"IntekPlus.Shared.CrashDumper.exe\".", L"Error", MB_ICONERROR | MB_SYSTEMMODAL);

		return EXCEPTION_CONTINUE_SEARCH;
	}
}

void PureCallHandler()
{
	::RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
}

void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	UNREFERENCED_PARAMETER(line);

	::RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
}

int CustomNewHandler(size_t size)
{
	::RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);

	return 0;
}

LONG VectoredExceptionHandler(PEXCEPTION_POINTERS pExcInf)
{
	switch (pExcInf->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
	case EXCEPTION_DATATYPE_MISALIGNMENT:
	case EXCEPTION_BREAKPOINT:
	case EXCEPTION_SINGLE_STEP:
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
	case EXCEPTION_FLT_DENORMAL_OPERAND:
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_INEXACT_RESULT:
	case EXCEPTION_FLT_INVALID_OPERATION:
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_FLT_STACK_CHECK:
	case EXCEPTION_FLT_UNDERFLOW:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_OVERFLOW:
	case EXCEPTION_PRIV_INSTRUCTION:
	case EXCEPTION_IN_PAGE_ERROR:
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	case EXCEPTION_STACK_OVERFLOW:
	case EXCEPTION_INVALID_DISPOSITION:
	case EXCEPTION_GUARD_PAGE:
	case EXCEPTION_INVALID_HANDLE:
		//	case EXCEPTION_POSSIBLE_DEADLOCK:
		return CustomExceptionHandler(pExcInf);
	default:
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

void CrashHandler::Register(const bool showDialog, const wchar_t* szDumpDirectory, const wchar_t* szDumpTitle, const MiniDumpType miniDumpType)
{
	if (::PathFileExists(szDumpDirectory) == FALSE)
	{
		::MessageBox(NULL, L"Cannot find the crash dump directory.", L"Error", MB_ICONERROR | MB_SYSTEMMODAL);
		::TerminateProcess(::GetCurrentProcess(), STATUS_INVALID_PARAMETER);
	}

	// 여러 쓰레드에서 예외가 발생하는 경우 문제가 될 수 있으므로 락을 건다.
	SingleLock lock(g_cs);

	auto& crashHandlerData = CrashHandlerData::Get();

	crashHandlerData.m_crashInfo->m_showDialog = showDialog ? TRUE : FALSE;
	::swprintf_s(crashHandlerData.m_crashInfo->m_dumpFileDirectory, L"%ls", szDumpDirectory);
	::swprintf_s(crashHandlerData.m_crashInfo->m_dumpFileTitle, L"%ls", szDumpTitle);
	crashHandlerData.m_crashInfo->m_miniDumpType = static_cast<int>(miniDumpType);

	// For SEH
	::AddVectoredExceptionHandler(1, VectoredExceptionHandler);
	::SetUnhandledExceptionFilter(CustomExceptionHandler);

	// For C-Runtime Library
	::_set_purecall_handler(PureCallHandler);
	::_set_invalid_parameter_handler(InvalidParameterHandler);
	::_set_new_handler(CustomNewHandler);
}

