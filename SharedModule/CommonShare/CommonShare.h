// CommonShare.h : main header file for the CommonShare DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCommonShareApp
// See CommonShare.cpp for the implementation of this class
//

class CCommonShareApp : public CWinApp
{
public:
	CCommonShareApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
