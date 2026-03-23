#pragma once

struct ThreadControl
{
	ThreadControl()
		: m_Signal_Kill(INVALID_HANDLE_VALUE)
		, m_Signal_Ready(INVALID_HANDLE_VALUE)
		, m_Signal_Start(INVALID_HANDLE_VALUE)
		, m_Handle_Thread(INVALID_HANDLE_VALUE)
	{

	}

	~ThreadControl()
	{
		Terminate();
	}

	void Terminate()
	{
		if (m_Handle_Thread != INVALID_HANDLE_VALUE)
		{
			::SetEvent(m_Signal_Kill);
			::WaitForSingleObject(m_Handle_Thread, 5000);
		}

		if (m_Signal_Kill != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_Signal_Kill);
		}

		if (m_Signal_Ready != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_Signal_Ready);
		}

		if (m_Signal_Start != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_Signal_Start);
		}

		if (m_Handle_Thread != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_Handle_Thread);
		}

		m_Signal_Kill = INVALID_HANDLE_VALUE;
		m_Signal_Ready = INVALID_HANDLE_VALUE;
		m_Signal_Start = INVALID_HANDLE_VALUE;
		m_Handle_Thread = INVALID_HANDLE_VALUE;
	}

	HANDLE m_Signal_Kill;				// �����带 �����ϴ� ������ ����
	HANDLE m_Signal_Ready;
	HANDLE m_Signal_Start;				// �������� ���� ������ ����
	HANDLE m_Handle_Thread;
};
