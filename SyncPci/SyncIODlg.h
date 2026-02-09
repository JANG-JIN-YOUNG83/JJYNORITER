#pragma once

#include "resource.h"
#include "afxwin.h"
#include "../CompanySharedLibrary/LogControl/SimpleListBoxLogger.h"

// CSyncIODlg 대화 상자입니다.

class CCustomStatic;
class CCustomCheckBox;
class CSyncboard;

class CSyncIODlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSyncIODlg)

public:
	CSyncIODlg(IPVM::LoggerInterface &logger, int nGenerationVersion,void* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSyncIODlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SYNCIODLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	struct ThreadControl	//이현민 수석님
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
				::WaitForSingleObject(m_Handle_Thread, INFINITE);
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

		HANDLE m_Signal_Kill;				// 쓰레드를 종료하는 명령을 내림
		HANDLE m_Signal_Ready;
		HANDLE m_Signal_Start;				// 쓰레드의 동작 명령을 내림
		HANDLE m_Handle_Thread;
	};

	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	CCustomCheckBox* m_checkInput[16];
	CCustomCheckBox* m_checkOutput[16];
	CCustomCheckBox* m_checkCommand[2];
	CCustomStatic* m_staticInput;
	CCustomStatic* m_staticOutput;
	CSyncboard* m_syncBoard;
	int m_nGenerationVersion;
	long m_targetIoCount;
	long m_pocketIoCount;
	long m_fovIoCount;
	BOOL m_bMoveReady;
	BOOL m_bMoveStart;
	BOOL m_bExit;
	BOOL m_bHeartBeat;
	BOOL m_bOutputValue[16];
	BOOL m_bCommandValue[2];
	IPVM::LoggerInterface &m_logger;
	CCriticalSection m_cs;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckOutput(UINT module);
	afx_msg void OnBnClickedCheckCommand(UINT module);
	SimpleListBoxLogger m_listLogIO;
	void ThreadStartIOSignal();
	static UINT ThreadIoSignal(LPVOID pParam);
	UINT ThreadIoSignal();
	void ThreadStartCheckState();
	static UINT ThreadCheckState(LPVOID pParam);
	UINT ThreadCheckState();
	void ThreadStartGetCellCountValue();
	static UINT ThreadGetCellCountValue(LPVOID pParam);
	UINT ThreadGetCellCountValue();

	ThreadControl m_threadControlIOSignal;
	ThreadControl m_threadControlCheckState;
	ThreadControl m_threadControlCellCountValue;

	bool m_isOpenSuccess;

	int m_nGetInputCount;
	bool m_bisHide;
};
