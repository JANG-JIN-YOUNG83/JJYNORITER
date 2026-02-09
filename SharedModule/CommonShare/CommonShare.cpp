// CommonShare.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "CommonShare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CCommonShareApp, CWinApp)
END_MESSAGE_MAP()


// CCommonShareApp construction

CCommonShareApp::CCommonShareApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCommonShareApp object

CCommonShareApp theApp;


// CCommonShareApp initialization

BOOL CCommonShareApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

int CCommonShareApp::ExitInstance()
{
	OutputDebugString(_T("Terminate CommonShare.dll\n"));

	return CWinApp::ExitInstance();
}
