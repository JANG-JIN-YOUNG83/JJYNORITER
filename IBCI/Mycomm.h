#pragma once
#include "afxwin.h"

#define MAXBUF			50000
#define InBufSize		50000
#define OutBufSize		50000
#define ASCII_XON		0x11
#define ASCII_XOFF		0x13
#define WM_MYRECEIVE	(WM_USER+578)	//
#define WM_MYCLOSE		(WM_USER+579)	// 


class CMycomm :
	public CCmdTarget
{
public:
	CMycomm(CString port, CString baudrate, CString parity, CString databit, CString stopbit);
	~CMycomm(void);

public:
	HANDLE		m_hComDev;
	HWND		m_hWnd;

	BOOL		m_bIsOpenned;
	CString		m_strComPort;
	CString		m_strBaudRate;
	CString		m_strParity;
	CString		m_strDateBit;
	CString		m_strStopBit;
	BOOL		m_bFlowChk;


	OVERLAPPED	m_OLR, m_OLW;
	char		m_chInBuf[MAXBUF * 2];
	int			m_nLength;
	CEvent*		m_pEvent;

public:
	void	Clear();
	int		Receive(LPSTR inbuf, int len);
	BOOL	Send(LPCTSTR outbuf, int len);
	BOOL	Create(HWND hWnd);
	void	HandleClose();
	void	Close();
	void	ResetSerial();
};

