// Persistence.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "PersistenceApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPersistenceApp

BEGIN_MESSAGE_MAP(CPersistenceApp, CWinApp)
END_MESSAGE_MAP()


// CPersistenceApp 생성

CPersistenceApp::CPersistenceApp()
{
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CPersistenceApp 개체입니다.

CPersistenceApp theApp;


// CPersistenceApp 초기화

BOOL CPersistenceApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

int CPersistenceApp::ExitInstance()
{
	OutputDebugString(_T("Terminate IntekPlus.Shared.Peristence.dll\n"));

	return CWinApp::ExitInstance();
}
