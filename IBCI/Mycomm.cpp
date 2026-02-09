#include "pch.h"
#include "Mycomm.h"
#include "stdint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMycomm::CMycomm(CString port, CString baudrate, CString parity, CString databit, CString stopbit)
{
    m_strComPort = port;                                                     // Comport
    m_strBaudRate = baudrate;                                            // Baud Rate
    m_strParity = parity;                                                      // Parity Bit
    m_strDateBit = databit;                                                   // Data Bit
    m_strStopBit = stopbit;                                                   // Stop Bit
    m_bFlowChk = 1;                                                        // Flow Check
    m_bIsOpenned = FALSE;                                             // 통신포트가 열려 있는지
    m_nLength = 0;                                                           // 받는 데이터의 길이
    memset(m_chInBuf, 0, MAXBUF * 2);                                  // Receive Buffer 초기화
    m_pEvent = new CEvent(FALSE, TRUE);                        // 쓰레드에서 사용할 이벤트
}

CMycomm::~CMycomm(void)
{
	if(m_bIsOpenned)
		Close();
	delete m_pEvent;
}

void CMycomm::ResetSerial()
{
	  DCB                    dcb;
             DWORD   DErr;
             COMMTIMEOUTS CommTimeOuts;
             
             if (!m_bIsOpenned)
                                        return;
             
             // 통신포트의 모든 에러를 리셋
             ClearCommError(m_hComDev,&DErr,NULL);
             // 통신포트의Input/Output Buffer 사이즈를 설정
             SetupComm(m_hComDev,InBufSize,OutBufSize);
             // 모든 Rx/Tx 동작을 제한하고 또한 Buffer의 내용을 버림
             PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
 
             // set up for overlapped I/O (MSDN 참조)
             // 통신 선로상에서 한바이트가 전송되고 또한 바이트가 전송되기까지의 시간
             CommTimeOuts.ReadIntervalTimeout = MAXDWORD ;
             // Read doperation 에서 TimeOut을 사용하지 않음
             CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
             CommTimeOuts.ReadTotalTimeoutConstant = 0 ;
                           
             // CBR_9600 is approximately 1byte/ms. For our purposes, allow
             // double the expected time per character for a fudge factor.
             CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
             CommTimeOuts.WriteTotalTimeoutConstant = 1000;
             // 통신포트의TimeOut을설정
             SetCommTimeouts(m_hComDev, &CommTimeOuts);        
 
 
             memset(&dcb,0,sizeof(DCB));
             dcb.DCBlength = sizeof(DCB);
             // 통신포트의 DCB를 얻음
             GetCommState(m_hComDev, &dcb);
             // DCB를 설정(DCB: 시리얼통신의 제어 파라메터, MSDN 참조)
             dcb.fBinary = TRUE;
             dcb.fParity = TRUE;
 
             if (m_strBaudRate == "300")
                           dcb.BaudRate = CBR_300;
             else if (m_strBaudRate == "600")
                           dcb.BaudRate = CBR_600;
             else if (m_strBaudRate == "1200")
                           dcb.BaudRate = CBR_1200;
             else if (m_strBaudRate == "2400")
                           dcb.BaudRate = CBR_2400;
             else if (m_strBaudRate == "4800")
                           dcb.BaudRate = CBR_4800;
             else if (m_strBaudRate == "9600")
                           dcb.BaudRate = CBR_9600;
             else if (m_strBaudRate == "14400")
                           dcb.BaudRate = CBR_14400;
             else if (m_strBaudRate == "19200")
                           dcb.BaudRate = CBR_19200;
             else if (m_strBaudRate == "28800")
                           dcb.BaudRate = CBR_38400;
             else if (m_strBaudRate == "33600")
                           dcb.BaudRate = CBR_38400;
             else if (m_strBaudRate == "38400")
                           dcb.BaudRate = CBR_38400;
             else if (m_strBaudRate == "56000")
                           dcb.BaudRate = CBR_56000;
             else if (m_strBaudRate == "57600")
                           dcb.BaudRate = CBR_57600;
             else if (m_strBaudRate == "115200")
                           dcb.BaudRate = CBR_115200;
             else if (m_strBaudRate == "128000")
                           dcb.BaudRate = CBR_128000;
             else if (m_strBaudRate == "256000")
                           dcb.BaudRate = CBR_256000;
             else if (m_strBaudRate == "PCI_9600")
                          dcb.BaudRate = 1075;
             else if (m_strBaudRate == "PCI_19200")
                           dcb.BaudRate = 2212;
             else if (m_strBaudRate == "PCI_38400")
                           dcb.BaudRate = 4300;
             else if (m_strBaudRate == "PCI_57600")
                           dcb.BaudRate = 6450;
             else if (m_strBaudRate == "PCI_500K")
                           dcb.BaudRate = 56000;
             
 
             if (m_strParity == "None")
                           dcb.Parity = NOPARITY;
             else if (m_strParity == "Even")
                           dcb.Parity = EVENPARITY;
             else if (m_strParity == "Odd")
                           dcb.Parity = ODDPARITY;
 
             if (m_strDateBit == "7 Bit")
                           dcb.ByteSize = 7;
             else if (m_strDateBit == "8 Bit")
                           dcb.ByteSize = 8;               
 
             if (m_strStopBit == "1 Bit")
                           dcb.StopBits = ONESTOPBIT;
             else if (m_strStopBit == "1.5 Bit")
                           dcb.StopBits = ONE5STOPBITS;
             else if (m_strStopBit == "2 Bit")
                           dcb.StopBits = TWOSTOPBITS;
 
             dcb.fRtsControl = RTS_CONTROL_ENABLE;
             dcb.fDtrControl = DTR_CONTROL_ENABLE;
            dcb.fOutxDsrFlow = FALSE;
 
             if (m_bFlowChk) {
                           dcb.fOutX = FALSE;
                           dcb.fInX = FALSE;
                           dcb.XonLim = 2048;
                           dcb.XoffLim = 1024;
             }
             else {
                           dcb.fOutxCtsFlow = TRUE;
                           dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
             }
             // 설정된 DCB로 통신포트의 제어 파라메터를 설정
             SetCommState(m_hComDev, &dcb);
             // Input Buffer에 데이터가 들어왔을 때 이벤트가 발생하도록 설정
             SetCommMask(m_hComDev,EV_RXCHAR);

}

void CMycomm::Close()
{
	if(!m_bIsOpenned)
		return;

	m_bIsOpenned = FALSE;
	SetCommMask(m_hComDev, 0);
	EscapeCommFunction(m_hComDev, CLRDTR);
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT || PURGE_TXCLEAR | PURGE_RXCLEAR);
	Sleep(500);

	HandleClose();
}

UINT CommThread(LPVOID lpData)
{
	extern short g_nRemoteStatus;
	DWORD ErrorFlags;
	COMSTAT comStat;
	DWORD EvtMask;
	char buf[MAXBUF];
	DWORD Length;
	int size;
	int insize = 0;

	CMycomm* Comm = (CMycomm*)lpData;

	while(Comm->m_bIsOpenned)
	{
		EvtMask = 0;
		Length = 0;
		insize = 0;
		memset(buf, '\0', MAXBUF);
		WaitCommEvent(Comm->m_hComDev, &EvtMask, NULL);
		ClearCommError(Comm->m_hComDev, &ErrorFlags, &comStat);
		if((EvtMask & EV_RXCHAR) && comStat.cbInQue)
		{
			if(comStat.cbInQue > MAXBUF)
				size = MAXBUF;
			else
				size = comStat.cbInQue;

			do
			{
				ClearCommError(Comm->m_hComDev, &ErrorFlags, &comStat);
				if(!ReadFile(Comm->m_hComDev, buf+insize, size, &Length, &(Comm->m_OLR)))
				{
					//ERROR
					TRACE("Error in ReadFile\n");
					if(GetLastError() == ERROR_IO_PENDING)
					{
						if(WaitForSingleObject(Comm->m_OLR.hEvent, 1000 ) != WAIT_OBJECT_0)
							Length = 0;
						else
							GetOverlappedResult(Comm->m_hComDev, &(Comm->m_OLR), &Length, TRUE);
					}
					else
						Length = 0;
				}//if
				insize += Length;
			}while((Length != 0) && (insize<size));
			ClearCommError(Comm->m_hComDev, &ErrorFlags, &comStat);

			if(Comm->m_nLength + insize > MAXBUF * 2)
				insize = (Comm->m_nLength + insize) - MAXBUF * 2;

			Comm->m_pEvent->ResetEvent();
			memcpy(Comm->m_chInBuf + Comm->m_nLength, buf, insize);
			Comm->m_nLength += insize;
			Comm->m_pEvent->SetEvent();
			LPARAM temp = (LPARAM)Comm;
			PostMessage(Comm->m_hWnd, WM_MYRECEIVE, Comm->m_nLength, temp);
		}
	}

	PurgeComm(Comm->m_hComDev, PURGE_TXABORT | PURGE_RXABORT || PURGE_TXCLEAR | PURGE_RXCLEAR);
	LPARAM temp = (LPARAM)Comm;
	SendMessage(Comm->m_hWnd, WM_MYCLOSE, 0, temp);

	return 0;
}

void CMycomm::HandleClose()
{
	CloseHandle(m_hComDev);
	CloseHandle(m_OLR.hEvent);
	CloseHandle(m_OLW.hEvent);
}

BOOL CMycomm::Create(HWND hWnd)
{
	m_hWnd = hWnd;     

	m_hComDev = CreateFile(m_strComPort, GENERIC_READ | GENERIC_WRITE,
                                        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                        NULL);                    // 시리얼 포트 오픈
             
             if (m_hComDev!=INVALID_HANDLE_VALUE)    // 포트가 정상적으로 열리면  
                           m_bIsOpenned = TRUE;                 // 성공
             else                                                          // 아니면 
                           return FALSE;                              // 실패하고 빠져나감
 
             ResetSerial();                                            // 시리얼 포트를 설정값대로 초기화
 
             m_OLW.Offset = 0;                                      // Write OVERLAPPED structure 초기화
             m_OLW.OffsetHigh = 0;
             m_OLR.Offset = 0;                                      // Read OVERLAPPED structure 초기화
             m_OLR.OffsetHigh = 0;
             // Overlapped 구조체의 이벤트를 생성
             m_OLR.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
             if (m_OLR.hEvent == NULL) {
                           CloseHandle(m_OLR.hEvent);
                           return FALSE;
             }
             m_OLW.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
             if (m_OLW.hEvent == NULL) {
                           CloseHandle(m_OLW.hEvent);
                           return FALSE;
             }
             // 시리얼 데이터를 받기위한 스레드 생성
             AfxBeginThread(CommThread,(LPVOID)this);
             // DTR (Data Terminal Ready) signal 을 보냄
             EscapeCommFunction(m_hComDev, SETDTR);   // MSDN 참조
             return TRUE;

	return TRUE;
}

/*char msg[] = {0xEF, 0xEF, 0x00, 0x64, 0xBE, 0x0A, 0xDE, 0x0F, 0xEE, 0xEE};
	len = sizeof(msg);*/

BOOL CMycomm::Send(LPCTSTR outbuf, int len)
{	 
	BOOL				bRet=TRUE;
    DWORD				ErrorFlags;
    COMSTAT				ComStat;
    DWORD               BytesWritten;
    DWORD               BytesSent=0;
 
    // 통신 포트의 모든 에러를 리셋
    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
    // overlapped I/O를 통하여 outbuf의 내용을 len길이 만큼 전송
    if (!WriteFile(m_hComDev, outbuf, len, &BytesWritten, &m_OLW))
	{
        if (GetLastError() == ERROR_IO_PENDING)
		{
                    if (WaitForSingleObject(m_OLW.hEvent,1000) != WAIT_OBJECT_0)
					    bRet=FALSE;
                    else
                          GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE);
        }
        else /* I/O error */
                    bRet=FALSE; /* ignore error */
    }
    // 다시 한번 통신포트의 모든 에러를 리셋
    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
             
    return bRet;
}

int	 CMycomm::Receive(LPSTR inbuf, int len)
{
	CSingleLock lockObj((CSyncObject*)m_pEvent, FALSE);

	if(len == 0)
		return -1;
	else if(len > MAXBUF)
		return -1;

	if(m_nLength == 0)
	{
		inbuf[0] = '\0';
		return 0;
	}
	else if(m_nLength <= len)
	{
		lockObj.Lock();
		memcpy(inbuf, m_chInBuf, m_nLength);
		memset(m_chInBuf, 0 , MAXBUF * 2);
		int tmp = m_nLength;
		m_nLength = 0;
		lockObj.Unlock();
		return tmp;
	}
	else
	{
		lockObj.Lock();
		memcpy(inbuf, m_chInBuf, len);
		memmove(m_chInBuf, m_chInBuf + len, MAXBUF * 2 - len);
		m_nLength -= len;
		lockObj.Unlock();
		return len;
	}
}

void CMycomm::Clear()
{
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT || PURGE_TXCLEAR | PURGE_RXCLEAR);
	memset(m_chInBuf, 0, MAXBUF * 2);
	m_nLength;
}




