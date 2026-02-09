// Base.h : main header file for the Base DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBaseApp
// See Base.cpp for the implementation of this class
//

class CBaseApp : public CWinApp
{
public:
	CBaseApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
