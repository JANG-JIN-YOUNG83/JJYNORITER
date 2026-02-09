// CrashDumper.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CrashDumper.h"
#include "dbghelp.h"
#include "tlhelp32.h"
#include "shellapi.h"

#include "../CrashHandler/CrashInfo.h"

#pragma comment(lib, "dbghelp")

void SuspendAllThread(DWORD targetProcessId)
{
	HANDLE h = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (h != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te;
		te.dwSize = sizeof(te);

		if (::Thread32First(h, &te))
		{
			do
			{
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
				{
					if (te.th32OwnerProcessID == targetProcessId)
					{
						HANDLE threadHandle = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);

						if (threadHandle != NULL)
						{
							::SuspendThread(threadHandle);
							::CloseHandle(threadHandle);
						}
					}
				}

				te.dwSize = sizeof(te);
			} while (::Thread32Next(h, &te));
		}

		::CloseHandle(h);
	}
}

void Process(CrashInfo* crashInfo)
{
	static const size_t pathLength = 8192;

	wchar_t* dumpPathName = new wchar_t[pathLength];
	wchar_t* dumpMessage = new wchar_t[pathLength];

	HANDLE processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, crashInfo->m_targetProcessId);

	if (processHandle != NULL)
	{
		SuspendAllThread(crashInfo->m_targetProcessId);

		HANDLE threadHandle = ::OpenThread(THREAD_ALL_ACCESS, FALSE, crashInfo->m_targetThreadId);

		if (threadHandle != NULL)
		{
			MINIDUMP_EXCEPTION_INFORMATION minidumpExceptionInformation = { 0 };
			minidumpExceptionInformation.ThreadId = crashInfo->m_targetThreadId;
			minidumpExceptionInformation.ExceptionPointers = (EXCEPTION_POINTERS*)crashInfo->m_exceptionPointers;
			minidumpExceptionInformation.ClientPointers = TRUE;

			SYSTEMTIME systemTime;
			::GetLocalTime(&systemTime);

			::swprintf_s(dumpPathName, pathLength
				, L"%ls%ls_%04d%02d%02d_%02d%02d%02d.dmp", crashInfo->m_dumpFileDirectory, crashInfo->m_dumpFileTitle
				, systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

			HANDLE hFile = ::CreateFile(dumpPathName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile != INVALID_HANDLE_VALUE)
			{
				if (::MiniDumpWriteDump(processHandle, crashInfo->m_targetProcessId, hFile, MINIDUMP_TYPE(crashInfo->m_miniDumpType), &minidumpExceptionInformation, NULL, NULL))
				{
					if (crashInfo->m_showDialog)
					{
						::swprintf_s(dumpMessage, pathLength, L"A crash dump file was saved!\r\n\r\n\"%ls\"", dumpPathName);

						::MessageBox(NULL, dumpMessage, L"IntekPlus.Shared.CrashDumper.exe", MB_ICONSTOP | MB_SYSTEMMODAL);
					}
				}
				else
				{
					::MessageBox(NULL, L"MiniDumpWriteDump() failed.", L"IntekPlus.Shared.CrashDumper.exe", MB_ICONERROR | MB_SYSTEMMODAL);
				}

				::CloseHandle(hFile);
			}
			else
			{
				::MessageBox(NULL, L"CreateFile() failed.", L"IntekPlus.Shared.CrashDumper.exe", MB_ICONERROR | MB_SYSTEMMODAL);
			}

			::ResumeThread(threadHandle);
			::CloseHandle(threadHandle);
		}
		else
		{
			::MessageBox(NULL, L"OpenThread() failed.", L"IntekPlus.Shared.CrashDumper.exe", MB_ICONERROR | MB_SYSTEMMODAL);
		}

		::CloseHandle(processHandle);
	}
	else
	{
		::MessageBox(NULL, L"OpenProcess() failed.", L"IntekPlus.Shared.CrashDumper.exe", MB_ICONERROR | MB_SYSTEMMODAL);
	}

	delete[] dumpMessage;
	delete[] dumpPathName;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	int nArgs = 0;
	auto szArglist = ::CommandLineToArgvW(lpCmdLine, &nArgs);

	if (nArgs != 1)
	{
		::LocalFree(szArglist);

		return 0;
	}

	const auto processId = ::_ttoi(szArglist[0]);

	wchar_t mappingHandleKey[512];

	::swprintf_s(mappingHandleKey, L"%ls_%d", KEY_mappingHandle, processId);

	if (HANDLE mappingHandle = ::OpenFileMapping(FILE_MAP_READ, FALSE, mappingHandleKey))
	{
		if (CrashInfo* crashInfo = (CrashInfo*)::MapViewOfFile(mappingHandle, FILE_MAP_READ, 0, 0, sizeof(CrashInfo)))
		{
			if (crashInfo->m_targetProcessId && crashInfo->m_targetThreadId && crashInfo->m_exceptionPointers)
			{
				Process(crashInfo);
			}
			else
			{
				::MessageBox(NULL, L"Process ID, thread ID or exception pointers are invalid!", L"IntekPlus.Shared.CrashDumper.exe", MB_ICONERROR | MB_SYSTEMMODAL);
			}

			::UnmapViewOfFile(crashInfo);
		}
		else
		{
			::MessageBox(NULL, L"MapViewOfFile() failed!", L"IntekPlus.Shared.CrashDumper.exe", MB_ICONERROR | MB_SYSTEMMODAL);
		}

		::CloseHandle(mappingHandle);
	}
	else
	{
		::MessageBox(NULL, L"OpenFileMapping() failed!", L"IntekPlus.Shared.CrashDumper.exe", MB_ICONERROR | MB_SYSTEMMODAL);
	}

	::LocalFree(szArglist);

	return 0;
}

