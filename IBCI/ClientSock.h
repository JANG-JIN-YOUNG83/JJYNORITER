#if !defined(AFX_CLIENTSOCK_H__C1249314_6846_403C_9A30_714CEC4151CC__INCLUDED_)
#define AFX_CLIENTSOCK_H__C1249314_6846_403C_9A30_714CEC4151CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSock.h : header file
//


class CClientSock : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CClientSock();
	virtual ~CClientSock();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSock)
public:

	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
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

#endif // !defined(AFX_CLIENTSOCK_H__C1249314_6846_403C_9A30_714CEC4151CC__INCLUDED_)
