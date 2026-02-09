// ServerSock.cpp : implementation file
//

//#include "stdafx.h"

#include "pch.h"
#include "ServerSock.h"
#include "IBCIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerSock

CServerSock::CServerSock()
{
}

CServerSock::~CServerSock()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CServerSock, CAsyncSocket)
	//{{AFX_MSG_MAP(CServerSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CServerSock member functions

void CServerSock::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CIBCIDlg* dlg = (CIBCIDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnAccept_S();
	CAsyncSocket::OnAccept(nErrorCode);
}
