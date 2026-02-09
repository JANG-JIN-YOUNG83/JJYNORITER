// SharedImageQueue.h : main header file for the SharedImageQueue DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSharedImageQueueApp
// See SharedImageQueue.cpp for the implementation of this class
//

class CSharedImageQueueApp : public CWinApp
{
public:
	CSharedImageQueueApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
