// ClientSock.cpp : implementation file
//

//#include "stdafx.h"
//#include "Server.h"
//#include "ClientSock.h"
//#include "ServerDlg.h"

#include "pch.h"
#include "ClientSock_S.h"
#include "IBCIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSock

CClientSock_S::CClientSock_S()
{
}

CClientSock_S::~CClientSock_S()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSock_S, CAsyncSocket)
	//{{AFX_MSG_MAP(CClientSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSock member functions

void CClientSock_S::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	CIBCIDlg* dlg = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnReceive_S(this);
	CAsyncSocket::OnReceive(nErrorCode);
}

void CClientSock_S::OnClose(int nErrorCode)
{
	m_pThread->PostThreadMessageW(WM_QUIT, 0, 0);
	CIBCIDlg* dlg = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnServerClose_S(this);
	CAsyncSocket::OnClose(nErrorCode);
}
