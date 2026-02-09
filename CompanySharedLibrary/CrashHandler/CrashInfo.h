#pragma once

struct CrashInfo
{
	BOOL m_showDialog;
	wchar_t m_dumpFileDirectory[4096];
	wchar_t m_dumpFileTitle[256];
	int m_miniDumpType;

	DWORD m_targetProcessId;
	DWORD m_targetThreadId;
	void* m_exceptionPointers;
};

#define KEY_mappingHandle				L"{7AFF02D9-45CA-4EFA-A4EF-5D28176F1B89}"
