#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(__in)
#define __in
#endif

#if !defined(__out)
#define __out
#endif

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _DLL_MAIN_CPP_
#define EXTERN 
#else 
#define EXTERN extern
#endif


	BOOL								SyncPci_LoadDll(void);
	void								SyncPci_UnloadDll(void);
	EXTERN HANDLE							(*SyncPci_LoadDevice) (ULONG instance);
	EXTERN void							(*SyncPci_UnloadDevice) ();
	EXTERN BOOL							(*SyncPci_WriteMicomIo)(HANDLE hDevice, ULONG offset, byte value);
	EXTERN BOOL							(*SyncPci_WriteHandlerIo)(HANDLE hDevice, ULONG offset, byte value);
	EXTERN BOOL							(*SyncPci_ReadMicomIo)(HANDLE hDevice, ULONG offset, byte* pValue);
	EXTERN BOOL							(*SyncPci_ReadHandlerIo)(HANDLE hDevice, ULONG offset, byte* pValue);
	EXTERN BOOL							(*SyncPci_WriteAvrParam)(HANDLE hDevice, byte addr, byte data);
	EXTERN BOOL							(*SyncPci_ReadHandler)(HANDLE hDevice, ULONG addr, byte *pValue);
	EXTERN BOOL							(*SyncPci_WriteHandler)(HANDLE hDevice, byte addr, byte data, ULONG option);
	EXTERN BOOL							(*SyncPci_ReadAvrStat)(HANDLE hDevice, byte *pValue);
	EXTERN BOOL							(*SyncPci_WriteAvrRst)(HANDLE hDevice, BOOL rst);
	EXTERN BOOL							(*SyncPci_ReadHandlerOut)(HANDLE hDevice, ULONG addr, byte *value);
	EXTERN BOOL							(*SyncPci_Write)(HANDLE hDevice, ULONG cs, ULONG offset, byte value);
	EXTERN BOOL							(*SyncPci_Read)(HANDLE hDevice, ULONG cs, ULONG offset, byte *pValue);


#ifdef __cplusplus
}
#endif
