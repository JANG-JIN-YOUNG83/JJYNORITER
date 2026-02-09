
#ifndef __iManG_H__ 
#define __iManG_H__

#define IGG_LIB_VERSION			0x010003			// CIS iGrabG2 Library 

#include "iManGDef.h"

#ifdef __iManGDLL_EXPORT__
#define _iManGAPI __declspec(dllexport)
#else
#define _iManGAPI __declspec(dllimport)
#endif


#ifdef	__cplusplus
extern	"C" {
#endif	//__cplusplus

// System\
_iManGAPI	int		Ig_DeviceList();

_iManGAPI	BOOL	Ig_OpenSystem(int nFgID, int nFgMode = 0);
_iManGAPI	BOOL	Ig_CloseSystem(int nFgID);

_iManGAPI	BOOL	Ig_GetDeviceType(int nFgID, unsigned int * pGetHWType);

_iManGAPI	BOOL	Ig_GetFirmwareVersion(int nFgID, unsigned int& nVer);
_iManGAPI	BOOL	Ig_GetLibraryVersion(unsigned int& nVer);

_iManGAPI	BOOL	Ig_GetSysDmaMemoryInfo(int nFgID, IGSYSTEM_MEM_INFO *pMemInfo);		//in 64bit OS
_iManGAPI	BOOL	Ig_GetSysDmaMemoryInfo32(int nFgID, IGSYSTEM_MEM_INFO32 *pMemInfo);	//in 32bit OS

_iManGAPI	BOOL	Ig_GetChDmaMemoryInfo(int nFgID, int nCh, IGSYSTEM_MEM_INFO *pMemInfo);			//in 64bit OS
_iManGAPI	BOOL	Ig_GetChDmaMemoryInfo32(int nFgID, int nCh, IGSYSTEM_MEM_INFO32 *pMemInfo);		//in 32bit OS

_iManGAPI	void	Ig_SystemThreadPriority(int nFgID, int nCh, int nPriority);
//------------------------------------------------------

//ICF
_iManGAPI	BOOL	Ig_NewICF(int nFgID, int nCh, int &nICFID);

_iManGAPI	BOOL	Ig_OpenICF(int nFgID, int nCh, int &nICFID, char* pICFPath);
_iManGAPI	BOOL	Ig_SaveICF(int nFgID, int nCh, int nICFID, char* pICFPath);

_iManGAPI	BOOL	Ig_SetActiveICF(int nFgID, int nCh, const int nICFID);

_iManGAPI	BOOL	Ig_SetICFParam(int nFgID, int nCh, int nICFID, int nSection, unsigned int nValue);
_iManGAPI	BOOL	Ig_GetICFParam(int nFgID, int nCh, int nICFID, int nSection, unsigned int* pValue);
_iManGAPI	BOOL	Ig_GetICFAll(int nFgID, int nCh, int nICFID, IFG_ICFPARAM* pICFParam);
_iManGAPI	BOOL	Ig_SetICFExposureTime(int nFgID, int nCh, int nICFID, float fHighTime, float fLowTime);		//ms
_iManGAPI	BOOL	Ig_GetICFExposureTime(int nFgID, int nCh, int nICFID, float &fHighTime, float &fLowTime);	//ms
//------------------------------------------------------

//user io
_iManGAPI	BOOL	Ig_SetIo(int nFgID, int nCh, int nIoBit, int nBitVal);
_iManGAPI	BOOL	Ig_GetIo(int nFgID, int nCh, int nIoBit, int &nBitVal);
//------------------------------------------------------

//cc io
_iManGAPI	BOOL	Ig_SetGpioOut(int nFgID, int nCh, int nIoBit, int nBitVal);
_iManGAPI	BOOL	Ig_GetGpioOut(int nFgID, int nCh, int nIoBit, int &nBitVal);
//------------------------------------------------------

//
_iManGAPI	BOOL	Ig_SetCustomGrabEvent(int nFgID, int nCh, int nCustEvtNum);
_iManGAPI	BOOL	Ig_GetCustomGrabEvent(int nFgID, int nCh, int *pCustEvtNum, int *pMaxCustEvtNum);
//
_iManGAPI	BOOL	Ig_GetLineScanDivision(int nFgID, int nCh, int *pDivNum);								

//grab
_iManGAPI	DWORD	Ig_MultiGrabProcess(int nFgID,
int nCh,
BYTE** ppBuffer,
int nBufferNum,
int nGrabNum,
int nGrabIssue = MTG_END_WAIT,
DWORD dwTimeout = 1000,
IGCALLBACK_GRBEVT fnCallback = NULL,
LPVOID lpUser = NULL);



_iManGAPI	DWORD	Ig_MultiGrabWait(int nFgID, int nCh);
_iManGAPI	BOOL	Ig_GrabStop(int nFgID, int nCh);


_iManGAPI	float	Ig_GetFrameRate(int nFgID, int nCh);
_iManGAPI	USHORT	Ig_GetDebugCount(int nFgID, int nCh);

_iManGAPI	void	Ig_Reset(int nFgID, int nCh);

//etc
_iManGAPI	void	Ig_DevWriteMemory8(int nFgID, int nCh, int nAddr, BYTE Data8);
_iManGAPI	void	Ig_DevWriteMemory16(int nFgID, int nCh, int nAddr, USHORT Data16);
_iManGAPI	void	Ig_DevWriteMemory32(int nFgID, int nCh, int nAddr, ULONG Data32);
_iManGAPI	void	Ig_DevReadMemory8(int nFgID, int nCh, int nAddr, BYTE* pData8);
_iManGAPI	void	Ig_DevReadMemory16(int nFgID, int nCh, int nAddr, USHORT* pData16);
_iManGAPI	void	Ig_DevReadMemory32(int nFgID, int nCh, int nAddr, ULONG* pData32);

_iManGAPI	BOOL	Ig_AuxSetEncoderCounter(int nFgID, int nCh, BOOL bEnable);
_iManGAPI	BOOL	Ig_AuxSetEncoderDirection(int nFgID, int nCh, BOOL bDirection);

_iManGAPI	BOOL	Ig_GetTemperature(int nFgID, double* pfTemper);
_iManGAPI	BOOL	Ig_GetRemainedImgSize(int nFgID, int nCh, BOOL &isValid, UINT& nImgSize);			// [2019.11.06] 추가, Firmware Ver2.0 이상에서 사용할 것

#ifdef	__cplusplus
}
#endif


#endif;		// __iManG_H__
