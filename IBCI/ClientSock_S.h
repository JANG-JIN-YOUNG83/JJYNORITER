#if !defined(AFX_CLIENTSOCK_H__E6AEB86B_5CC9_4A64_A31F_ED76D1F5CBC4__INCLUDED_)
#define AFX_CLIENTSOCK_H__E6AEB86B_5CC9_4A64_A31F_ED76D1F5CBC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSock.h : header file
//




class CClientSock_S : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CClientSock_S();
	virtual ~CClientSock_S();

// Overrides
public:
	CWinThread * m_pThread;
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCK_H__E6AEB86B_5CC9_4A64_A31F_ED76D1F5CBC4__INCLUDED_)
