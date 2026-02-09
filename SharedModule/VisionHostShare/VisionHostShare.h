// VisionHostShare.h : main header file for the VisionHostShare DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CVisionHostShareApp
// See VisionHostShare.cpp for the implementation of this class
//

class CVisionHostShareApp : public CWinApp
{
public:
	CVisionHostShareApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
