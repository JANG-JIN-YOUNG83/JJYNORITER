#include "stdafx.h"
#include "Base.h"

#include <afxmt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SharedMemory;

Base::Base(const long bytes, LPCTSTR name, const long identifier)
	: Base(bytes, name, identifier, 0)
{
}

Base::Base(const long bytes, LPCTSTR name, const long identifier1, const long identifier2)
	: m_shardMemoryHandle(NULL)
	, m_sharedMemoryPointer(nullptr)
{
	if (bytes == 0)
	{
		return;
	}

	CString strName;
	strName.Format(_T("%s_%d_%d"), name, identifier1, identifier2);

	CMutex mutex(TRUE, _T("SharedMemory::Base"));

	m_shardMemoryHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)bytes, strName);

	// 얘는 에러 핸들은 INVALID_HANDLE_VALUE 가 아닌 NULL 을 반환하므로 신경써야 함.
	if (m_shardMemoryHandle == NULL)
	{
		::AfxMessageBox(_T("Cannot create shared memory!!"), MB_ICONERROR);
		return;
	}

	DWORD dwError = ::GetLastError();

	m_sharedMemoryPointer = ::MapViewOfFile(m_shardMemoryHandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	if (m_sharedMemoryPointer == nullptr)
	{
		::AfxMessageBox(_T("Cannot map shared memory!!"), MB_ICONERROR);
		return;
	}

	// 공유 메모리가 처음 만들어지는 것이라면 0 으로 초기화해 주자.
	if (dwError != ERROR_ALREADY_EXISTS)
	{
		memset(m_sharedMemoryPointer, 0, bytes);
	}
}

Base::~Base()
{
	UnmapViewOfFile(m_sharedMemoryPointer);
	CloseHandle(m_shardMemoryHandle);
}
