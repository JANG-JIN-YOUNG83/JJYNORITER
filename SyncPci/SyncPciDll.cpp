#include "stdafx.h"

#if defined(__BCPLUSPLUS__)  // Borland C++ Builder specific
#include <vcl.h>
#pragma hdrstop
#endif

#include <windows.h>
#include <stdio.h>

#define _DLL_MAIN_CPP_
#include "SyncPciDll.h"

typedef struct{
	char *name;
	FARPROC* ptr;
}LIB_FUNCT;

static HINSTANCE hLibrary=NULL;
#define __DLL_FILE_FULL_PATH__  "SyncPci64.dll"

BOOL SyncPci_LoadDll(void)
{
	LIB_FUNCT Func[] = {
		{"SyncPci_LoadDevice",		(FARPROC*)&SyncPci_LoadDevice},
		{"SyncPci_UnloadDevice",	(FARPROC*)&SyncPci_UnloadDevice},
		{"SyncPci_WriteMicomIo",	(FARPROC*)&SyncPci_WriteMicomIo},
		{"SyncPci_WriteHandlerIo",	(FARPROC*)&SyncPci_WriteHandlerIo},
		{"SyncPci_ReadMicomIo",		(FARPROC*)&SyncPci_ReadMicomIo},
		{"SyncPci_ReadHandlerIo",	(FARPROC*)&SyncPci_ReadHandlerIo},
		{"SyncPci_WriteAvrParam",	(FARPROC*)&SyncPci_WriteAvrParam},
		{"SyncPci_ReadHandler",		(FARPROC*)&SyncPci_ReadHandler},
		{"SyncPci_WriteHandler",	(FARPROC*)&SyncPci_WriteHandler},
		{"SyncPci_ReadAvrStat",		(FARPROC*)&SyncPci_ReadAvrStat},
		{"SyncPci_WriteAvrRst",		(FARPROC*)&SyncPci_WriteAvrRst},
		{"SyncPci_ReadHandlerOut",	(FARPROC*)&SyncPci_ReadHandlerOut},
		{"SyncPci_Write",		(FARPROC*)&SyncPci_Write},
		{"SyncPci_Read",		(FARPROC*)&SyncPci_Read},
		{NULL, NULL}
	};

	LIB_FUNCT *f;

	CHAR szBuffer[MAX_PATH + _MAX_FNAME] = "";

	if((hLibrary = LoadLibrary (_T(__DLL_FILE_FULL_PATH__))) == NULL)
		return FALSE;

	f = &Func[0];
	while(f->ptr != NULL)
	{
		*(f->ptr) = GetProcAddress ((HMODULE)hLibrary, f->name);

		f++;
	}
	return TRUE;
}

void SyncPci_UnloadDll(void)
{
	if(hLibrary){
		FreeLibrary(hLibrary);
		hLibrary = NULL;
	}
}