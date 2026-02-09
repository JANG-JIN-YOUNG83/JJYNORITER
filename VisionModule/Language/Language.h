// Language.h : main header file for the Language DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLanguageApp
// See Language.cpp for the implementation of this class
//

class CLanguageApp : public CWinApp
{
public:
	CLanguageApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
